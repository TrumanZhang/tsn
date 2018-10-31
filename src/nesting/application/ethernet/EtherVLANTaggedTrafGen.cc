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

        cPacket *packet = new cPacket(msgname, IEEE802CTRL_DATA);

        long len = packetLength->intValue();
        packet->setByteLength(len);

        // Create control info for encap modules
        Ieee8021QCtrl *ctrlInfo = new Ieee8021QCtrl();
        ctrlInfo->setEtherType(etherType);
        ctrlInfo->setDest(destMacAddress);
        ctrlInfo->setTagged(vlanTagEnabled->boolValue());
        ctrlInfo->setPCP(pcp->intValue());
        ctrlInfo->setDEI(dei->boolValue());
        ctrlInfo->setVID(vid->intValue());
        packet->setControlInfo(ctrlInfo);

        EV_TRACE << getFullPath() << ": Send packet `" << packet->getName()
                        << "' dest=" << ctrlInfo->getDestinationAddress()
                        << " length=" << packet->getBitLength() << "B type="
                        << ctrlInfo->getEtherType() << " vlan-tagged="
                        << ctrlInfo->isTagged() << " pcp=" << ctrlInfo->getPCP()
                        << " dei=" << ctrlInfo->getDEI() << " vid="
                        << ctrlInfo->getVID() << endl;

        emit(sentPkSignal, packet);
        send(packet, "out");
        packetsSent++;
    }
}

} // namespace nesting
