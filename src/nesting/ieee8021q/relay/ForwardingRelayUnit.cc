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

#include "nesting/ieee8021q/relay/ForwardingRelayUnit.h"

#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/linklayer/ethernet/EtherFrame_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/linklayer/vlan/VlanTag_m.h"

namespace nesting {

Define_Module(ForwardingRelayUnit);

void ForwardingRelayUnit::initialize(int stage) {
    if (stage == INITSTAGE_LOCAL) {
        fdb = getModuleFromPar<FilteringDatabase>(par("filteringDatabaseModule"), this);
        ifTable = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        numberOfPorts = par("numberOfPorts");
    } else if (stage == INITSTAGE_LINK_LAYER) {
        registerService(Protocol::ethernetMac, nullptr, gate("ifIn"));
        registerProtocol(Protocol::ethernetMac, gate("ifOut"), nullptr);
    }
}

void ForwardingRelayUnit::handleMessage(cMessage *msg) {
    Packet* packet = check_and_cast<Packet*>(msg);
    const auto& frame = packet->peekAtFront<EthernetMacHeader>();
    int arrivalInterfaceId = packet->getTag<InterfaceInd>()->getInterfaceId();

    // Remove old service indications but keep packet protocol tag and add VLAN request
    auto oldPacketProtocolTag = packet->removeTag<PacketProtocolTag>();
    auto vlanInd = packet->removeTag<VlanInd>();
    packet->clearTags();
    auto newPacketProtocolTag = packet->addTag<PacketProtocolTag>();
    *newPacketProtocolTag = *oldPacketProtocolTag;
    auto vlanReq = packet->addTag<VlanReq>();
    vlanReq->setVlanId(vlanInd->getVlanId());
    delete oldPacketProtocolTag;

    packet->trim();

    // Distinguish between broadcast-, multicast- and unicast-ethernet frames
    if (frame->getDest().isBroadcast()) {
        processBroadcast(packet, arrivalInterfaceId);
    } else if (frame->getDest().isMulticast()) {
        processMulticast(packet, arrivalInterfaceId);
    } else {
        processUnicast(packet, arrivalInterfaceId);
    }
}

void ForwardingRelayUnit::processBroadcast(Packet* packet, int arrivalInterfaceId) {
    // Flood packets everywhere except of ingress port
    // TODO this is just a temporary solution not sure how correct that is
    for (int portId = 0; portId < gateSize("out"); portId++) {
        cGate *outputGate = gate("out", portId);
        if (!packet->arrivedOn("in", portId)) {
            Packet* dupPacket = packet->dup();
            send(dupPacket, outputGate);
        }
    }
    delete packet;
}

void ForwardingRelayUnit::processMulticast(Packet* packet, int arrivalInterfaceId) {
    const auto& frame = packet->peekAtFront<EthernetMacHeader>();
    int arrivalGate = packet->getArrivalGate()->getIndex();

    std::vector<int> forwardingPorts = fdb->getDestInterfaceIds(frame->getDest(), simTime());

    if (forwardingPorts.at(0) == -1) {
        throw cRuntimeError(
                "Static multicast forwarding for packet didn't work. Entry in forwarding table was empty!");
    } else {
        std::string forwardingPortsString = "";
        for (auto forwardingPort : forwardingPorts) {
            // skip arrival gate
            if (forwardingPort == arrivalGate) {
                continue;
            }
            Packet* dupPacket = packet->dup();
            send(dupPacket, gate("out", forwardingPort));
            forwardingPortsString = forwardingPortsString.append(
                    std::to_string(forwardingPort));
        }
        EV_INFO << getFullPath() << ": Forwarding multicast packet `" << packet << "` to ports "
                << forwardingPortsString << endl;
    }
    delete packet;
}

void ForwardingRelayUnit::processUnicast(Packet* packet, int arrivalInterfaceId) {
    //Learning MAC port mappings
    const auto& frame = packet->peekAtFront<EthernetMacHeader>();
    learn(frame->getSrc(), arrivalInterfaceId);
    int destInterfaceId = fdb->getDestInterfaceId(frame->getDest(), simTime());

    //Routing entry available?
    if (destInterfaceId == -1) {
        EV_INFO << getFullPath() << ": Broadcasting packet `" << packet << "` to all ports" << endl;
        processBroadcast(packet, arrivalInterfaceId);
    } else {;
        EV_INFO << getFullPath() << ": Forwarding packet `" << packet << "` to port " << destInterfaceId << endl;
        packet->addTagIfAbsent<InterfaceReq>()->setInterfaceId(destInterfaceId);
        send(packet, gate("ifOut"));
    }
}

void ForwardingRelayUnit::learn(MacAddress srcAddr, int arrivalInterfaceId)
{
    fdb->insert(srcAddr, simTime(), arrivalInterfaceId);
}

} // namespace nesting
