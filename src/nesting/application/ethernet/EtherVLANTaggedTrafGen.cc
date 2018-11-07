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

#include "EtherVLANTaggedTrafGen.h"

namespace nesting {

Define_Module(EtherVLANTaggedTrafGen);

void EtherVLANTaggedTrafGen::initialize(int stage) {
    EtherTrafGen::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        vlanTagEnabled = &par("vlanTagEnabled");
        pcp = &par("pcp");
        dei = &par("dei");
        vid = &par("vid");
    }
}

void EtherVLANTaggedTrafGen::sendBurstPackets() {
    int n = numPacketsPerBurst->intValue();
    for (int i = 0; i < n; i++) {
        seqNum++;

        char msgname[40];
        sprintf(msgname, "pk-%d-%ld", getId(), seqNum);

        // create new packet
        long len = packetLength->intValue();
        auto data = makeShared<ByteCountChunk>(B(len));
        auto *packet = new Packet(msgname, data);

        // create control info for encap modules
        // ctrlInfo->setTagged(vlanTagEnabled->boolValue()); , see if omitting this causes problems further down the road
        auto ethMacHeader = makeShared<EthernetMacHeader>();
        ethMacHeader->setDest(destMacAddress);
        if (vlanTagEnabled) {
            ethMacHeader->setTypeOrLength(ETHERTYPE_8021Q_TAG); // ok here?
        } // else ethertype = default = 0

        // create VLAN control info
        auto ieee8021q = makeShared<Ieee802_1QHeader>();
        ieee8021q->setPcp(pcp->intValue());
        ieee8021q->setDe(dei->boolValue());
        ieee8021q->setVID(vid->intValue());

        packet->insertAtFront(ieee8021q);
        packet->insertAtFront(ethMacHeader); // 8021q header according to standard should be placed in the EthernetMacHeader

        EV_TRACE << getFullPath() << ": Send packet `" << packet->getName()
                        << "' dest=" << ethMacHeader->getDest() << " length="
                        << packet->getBitLength() << "B type="
                        << ethMacHeader->getTypeOrLength() << " vlan-tagged="
                        << ethMacHeader->getTypeOrLength() << " pcp="
                        << ieee8021q->getPcp() << " dei=" << ieee8021q->getDe()
                        << " vid=" << ieee8021q->getVID() << endl;

        // emit(sentPkSignal, packet); no sentPkSignal in EtherTrafGen in Inet 4.0.0
        packetsSent++;
    }
}

} // namespace nesting
