//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "VLANEncap.h"
#define COMPILETIME_LOGLEVEL omnetpp::LOGLEVEL_TRACE

namespace nesting {

Define_Module(VLANEncap);

void VLANEncap::initialize() {
    // Signals
    encapPkSignal = registerSignal("encapPk");
    decapPkSignal = registerSignal("decapPk");

    verbose = par("verbose");
    tagUntaggedFrames = par("tagUntaggedFrames");
    pvid = par("pvid");

    totalFromHigherLayer = 0;
    WATCH(totalFromHigherLayer);

    totalFromLowerLayer = 0;
    WATCH(totalFromLowerLayer);

    totalEncap = 0;
    WATCH(totalEncap);

    totalDecap = 0;
    WATCH(totalDecap);
}

void VLANEncap::handleMessage(cMessage* msg) {
    cPacket* packet = check_and_cast<cPacket*>(msg);

    if (packet->arrivedOn("lowerLayerIn")) {
        processPacketFromLowerLevel(packet);
    } else {
        processPacketFromHigherLevel(packet);
    }
}

void VLANEncap::processPacketFromHigherLevel(Packet *packet) {
    EV_INFO << getFullPath() << ": Received " << packet << " from upper layer."
                   << endl;

    totalFromHigherLayer++;

    // Encapsulate VLAN Header
    if (packet->findTag<VLANTagReq>()) {
        auto vlanTag = packet->getTag<VLANTagReq>();
        const auto& vlanHeader = makeShared<Ieee802_1QHeader>();
        vlanHeader->setPcp(vlanTag->getPcp());
        vlanHeader->setDe(vlanTag->getDe());
        vlanHeader->setVID(vlanTag->getVID());
        packet->insertAtFront(vlanHeader);
        delete packet->removeTagIfPresent<VLANTagReq>();
        // Statistics and logging
        EV_INFO << getFullPath() << ":Encapsulating higher layer packet `"
                       << packet->getName() << "' into VLAN tag" << endl;
        totalEncap++;
        emit(encapPkSignal, packet);
    }

    EV_TRACE << getFullPath() << ": Packet-length is "
                    << packet->getByteLength() << " and Destination is "
                    << packet->getTag<MacAddressReq>()->getDestAddress()
                    << " before sending packet to lower layer" << endl;

    send(packet, "lowerLayerOut");
}

void VLANEncap::processPacketFromLowerLevel(Packet *packet) {
    EV_INFO << getFullPath() << ": Received " << packet << " from lower layer."
                   << endl;

    totalFromLowerLayer++;

    // Decapsulate packet if it is a VLAN Tag, otherwise just insert default
    // values into the control information
    if (packet->hasAtFront<Ieee802_1QHeader>()) {
        auto vlanHeader = packet->popAtFront<Ieee802_1QHeader>();
        auto vlanTag = packet->addTagIfAbsent<VLANTagInd>();
        vlanTag->setPcp(vlanHeader->getPcp());
        vlanTag->setDe(vlanHeader->getDe());
        short vid = vlanHeader->getVID();
        if (vid < Ieee8021q::kMinValidVID || vid > Ieee8021q::kMaxValidVID) {
            vid = pvid;
        }
        vlanTag->setVID(vid);
        EV_TRACE << getFullPath() << ": Decapsulating packet and `"
                        << "' passing up contained packet `"
                        << packet->getName() << "' to higher layer" << endl;

        totalDecap++;
        emit(decapPkSignal, ethernet1QTag);
        delete vlanHeader;
    } else {
        auto vlanTag = packet->addTagIfAbsent<VLANTagInd>();
        vlanTag->setPcp(Ieee8021q::kDefaultPCPValue);
        vlanTag->setDe(Ieee8021q::kDefaultDEIValue);
        vlanTag->setVID(pvid);
    }

    EV_TRACE << getFullPath() << ": Packet-length is "
                    << packet->getByteLength() << ", destination is "
                    << packet->getTag<MacAddressInd>()->getDestAddress()
                    << ", PCP Value is " << vlanTag->getPcp()
                    << " before sending packet up" << endl;

    // Send packet to upper layer
    send(packet, "upperLayerOut");
}

void VLANEncap::refreshDisplay() const {
    char buf[80];
    sprintf(buf, "up/decap: %ld/%ld\ndown/encap: %ld/%ld", totalFromLowerLayer,
            totalDecap, totalFromHigherLayer, totalEncap);
    getDisplayString().setTagArg("t", 0, buf);
}

int VLANEncap::getPVID() {
    return pvid;
}

} // namespace nesting
