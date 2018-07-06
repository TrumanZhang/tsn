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

#include "EtherTrafGenQueue.h"

#include <omnetpp/ccomponent.h>
#include <omnetpp/cexception.h>
#include <omnetpp/clog.h>
#include <omnetpp/cmessage.h>
#include <omnetpp/cnamedobject.h>
#include <omnetpp/cobjectfactory.h>
#include <omnetpp/cpacket.h>
#include <omnetpp/cpar.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/cwatch.h>
#include <omnetpp/regmacros.h>
#include <omnetpp/simutil.h>
#include <cstdio>
#include <iostream>
#include <string>

#include "inet/linklayer/common/Ieee802Ctrl.h"

namespace nesting {

Define_Module(EtherTrafGenQueue);

void EtherTrafGenQueue::initialize() {
  // Signals
  sentPkSignal = registerSignal("sentPk");

  // Initialize sequence-number for generated packets
  seqNum = 0;

  // NED parameters
  etherType = &par("etherType");
  vlanTagEnabled = &par("vlanTagEnabled");
  pcp = &par("pcp");
  dei = &par("dei");
  vid = &par("vid");
  packetLength = &par("packetLength");
  const char *destAddress = par("destAddress");
  if (!destMACAddress.tryParse(destAddress)) {
    throw new cRuntimeError("Invalid MAC Address");
  }

  // Statistics
  packetsSent = 0;
  WATCH(packetsSent);

}

void EtherTrafGenQueue::handleMessage(cMessage *msg) {
  throw cRuntimeError("cannot handle messages.");
}

cPacket* EtherTrafGenQueue::generatePacket() {
  seqNum++;

  char msgname[40];
  sprintf(msgname, "pk-%d-%ld", getId(), seqNum);

  cPacket *packet = new cPacket(msgname, IEEE802CTRL_DATA);

  long len = packetLength->longValue();
  packet->setByteLength(len);

  Ieee8021QCtrl *etherctrl = new Ieee8021QCtrl();

  etherctrl->setEtherType(etherType->longValue());
  etherctrl->setDest(destMACAddress);
  etherctrl->setTagged(vlanTagEnabled->boolValue());
  etherctrl->setPCP(pcp->longValue());
  etherctrl->setDEI(dei->boolValue());
  etherctrl->setVID(vid->longValue());

  packet->setControlInfo(etherctrl);

  return packet;
}

void EtherTrafGenQueue::requestPacket() {
  Enter_Method("requestPacket(...)");

  cPacket* packet = generatePacket();

  Ieee8021QCtrl* ctrlInfo = static_cast<Ieee8021QCtrl*>(
  packet->getControlInfo()
  );

  if(par("verbose")) {
    EV_TRACE << getFullPath() << ": Send packet `" << packet->getName() << "' dest=" << ctrlInfo->getDestinationAddress()
    << " length=" << packet->getBitLength() << "B type=" << ctrlInfo->getEtherType()
    << " vlan-tagged=" << ctrlInfo->isTagged() << " pcp=" << ctrlInfo->getPCP()
    << " dei=" << ctrlInfo->getDEI() << " vid=" << ctrlInfo->getVID() << "\n";
  }
  emit(sentPkSignal, packet);
  send(packet, "out");
  packetsSent++;
}

int EtherTrafGenQueue::getNumPendingRequests() {
  // Requests are always served immediately,
  // therefore no pending requests exist.
  return 0;
}

bool EtherTrafGenQueue::isEmpty() {
  // Queue is never empty
  return false;
}

cMessage* EtherTrafGenQueue::pop() {
  return generatePacket();
}

} // namespace nesting
