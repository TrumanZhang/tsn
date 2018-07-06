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
  }
  else {
    processPacketFromHigherLevel(packet);
  }
}

void VLANEncap::processPacketFromHigherLevel(cPacket* packet) {
    EV_INFO << getFullPath() << ": Received " << packet << " from upper layer." << endl;

  totalFromHigherLayer++;

  // Packet control info
  Ieee8021QCtrl* oldCtrlInfo = check_and_cast<Ieee8021QCtrl*>(packet->removeControlInfo());
  Ieee802Ctrl* newCtrlInfo = new Ieee802Ctrl(*oldCtrlInfo);

  // Encapsulate VLAN Tag depending on Ieee8021QCtrl info
  if (oldCtrlInfo->isTagged() || tagUntaggedFrames) {
    Ethernet1QTag* ether1QTag = new Ethernet1QTag(packet->getName());
    ether1QTag->setPcp(oldCtrlInfo->getPCP());
    ether1QTag->setDe(oldCtrlInfo->getDEI());
    ether1QTag->setVID(oldCtrlInfo->getVID());
    ether1QTag->setByteLength(Ieee8021q::kVLANTagByteLength);
    ether1QTag->encapsulate(packet);

    // Statistics and logging
     EV_INFO << getFullPath() << ":Encapsulating higher layer packet `" << packet->getName() << "' into VLAN tag"
             << endl;

    totalEncap++;
    emit(encapPkSignal, packet);

    // Ether1QTag is the new packet
    packet = ether1QTag;

  }

  // Old control info is not needed anymore
  delete oldCtrlInfo;

  packet->setControlInfo(newCtrlInfo);

  EV_TRACE << getFullPath() << ": Packet-length is " << packet->getByteLength() << " and Destination is "
          << newCtrlInfo->getDest() << " before sending packet to lower layer" << endl;

  send(packet, "lowerLayerOut");
}

void VLANEncap::processPacketFromLowerLevel(cPacket* packet) {
    EV_INFO << getFullPath() << ": Received " << packet << " from lower layer." << endl;


  totalFromLowerLayer++;

  // Packet control info
  Ieee802Ctrl* oldCtrlInfo = check_and_cast<Ieee802Ctrl*>(packet->removeControlInfo());
  Ieee8021QCtrl* newCtrlInfo = new Ieee8021QCtrl();

  // Copy the values from the old, less detailed control info into the new one
  newCtrlInfo->Ieee802Ctrl::operator=(*oldCtrlInfo);
  delete oldCtrlInfo;

  // Decapsulate packet if it is a VLAN Tag, otherwise just insert default
  // values into the control information
  if (Ethernet1QTag* ethernet1QTag = dynamic_cast<Ethernet1QTag*>(packet)) {
    newCtrlInfo->setPCP(ethernet1QTag->getPcp());
    newCtrlInfo->setDEI(ethernet1QTag->getDe());
    int vid = ethernet1QTag->getVID();
    if (vid < Ieee8021q::kMinValidVID || vid > Ieee8021q::kMaxValidVID) {
      vid = pvid;
    }
    newCtrlInfo->setVID(ethernet1QTag->getVID());
    newCtrlInfo->setTagged(true);
    packet = ethernet1QTag->decapsulate();

    // Statistics and logging
    EV_TRACE << getFullPath() << ": Decapsulating packet `" << ethernet1QTag->getName()
          << "', passing up contained packet `" << packet->getName() << "' to higher layer" << endl;

    totalDecap++;
    emit(decapPkSignal, ethernet1QTag);

    // Delete decapsulated packet
    delete ethernet1QTag;
  }
  else {
    newCtrlInfo->setPCP(Ieee8021q::kDefaultPCPValue);
    newCtrlInfo->setDEI(Ieee8021q::kDefaultDEIValue);
    newCtrlInfo->setVID(pvid);
  }

  packet->setControlInfo(newCtrlInfo);

  EV_TRACE << getFullPath() << ": Packet-length is " << packet->getByteLength() << ", destination is "
        << newCtrlInfo->getDest() << ", PCP Value is " << newCtrlInfo->getPCP() << " before sending packet up" << endl;

  // Send packet to upper layer
  send(packet, "upperLayerOut");
}

void VLANEncap::refreshDisplay() const {
  char buf[80];
  sprintf(buf, "up/decap: %ld/%ld\ndown/encap: %ld/%ld", totalFromLowerLayer, totalDecap, totalFromHigherLayer,
      totalEncap);
  getDisplayString().setTagArg("t", 0, buf);
}

int VLANEncap::getPVID() {
  return pvid;
}

} // namespace nesting
