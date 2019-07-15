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

#include "../queue/QueuingFrames.h"
#define COMPILETIME_LOGLEVEL omnetpp::LOGLEVEL_TRACE

namespace nesting {

Define_Module(QueuingFrames);

void QueuingFrames::initialize() {
    //Initialize the number of queues, which is equal to the number of "out"
    // vector gate. The default values is 8, if user has not specified it.
    numberOfQueues = gateSize("out");

    //Precondition: numberOfQueues must have a valid number, i.e <= number of
    // all possible pcp values.

    if (numberOfQueues > kNumberOfPCPValues || numberOfQueues < 1) {
        throw new cRuntimeError(
                "Invalid assignment of numberOfQueues. Number of queues should not "
                        "be bigger than the number of all possible pcp values!");
    }
}

void QueuingFrames::handleMessage(cMessage *msg) {
    if (dynamic_cast<inet::Request*>(msg)) {
        delete msg;
        return;
    }

    inet::Packet *packet = check_and_cast<inet::Packet *>(msg);

    // switch ingoing VLAN Tag to outgoing Tag
    int pcpValue;
    int DeValue;
    int VIDValue;
    if (packet->findTag<VLANTagInd>()) {
        auto vlanTagIn = packet->removeTag<VLANTagInd>();
        pcpValue = vlanTagIn->getPcp();
        DeValue = vlanTagIn->getDe();
        VIDValue = vlanTagIn->getVID();
        delete vlanTagIn;
    } else {
        auto vlanTagIn = packet->removeTag<VLANTagReq>();
        pcpValue = vlanTagIn->getPcp();
        DeValue = vlanTagIn->getDe();
        VIDValue = vlanTagIn->getVID();
        delete vlanTagIn;
    }
    auto vlanTagOut = packet->addTag<VLANTagReq>();
    vlanTagOut->setPcp(pcpValue);
    vlanTagOut->setDe(DeValue);
    vlanTagOut->setVID(VIDValue);

    // switch ingoing MAC Tag to outgoing MAC Tag
    inet::MacAddress in;
    inet::MacAddress out;
    if (packet->findTag<inet::MacAddressInd>()) {
        auto macTagIn = packet->removeTag<inet::MacAddressInd>();
        in = macTagIn->getSrcAddress();
        out = macTagIn->getDestAddress();
        delete macTagIn;
    } else {
        auto macTagIn = packet->removeTag<inet::MacAddressReq>();
        in = macTagIn->getSrcAddress();
        out = macTagIn->getDestAddress();
        delete macTagIn;
    }
    auto macTagOut = packet->addTag<inet::MacAddressReq>();
    macTagOut->setDestAddress(out);
    macTagOut->setSrcAddress(in);

    // switch incoming llsap tag
    int ssap;
    int dsap;
    if (packet->findTag<inet::Ieee802SapInd>()) {
        auto sapTagIn = packet->removeTag<inet::Ieee802SapInd>();
        ssap = sapTagIn->getSsap();
        dsap = sapTagIn->getDsap();
        delete sapTagIn;
    } else {
        auto sapTagIn = packet->removeTag<inet::Ieee802SapReq>();
        ssap = sapTagIn->getSsap();
        dsap = sapTagIn->getDsap();
        delete sapTagIn;
    }
    auto sapTagOut = packet->addTag<inet::Ieee802SapReq>();
    sapTagOut->setDsap(dsap);
    sapTagOut->setSsap(ssap);

    // remove encapsulation
    packet->trim();

    // Check whether the PCP value is correct.
    if (pcpValue > kNumberOfPCPValues) {
        throw new cRuntimeError(
                "Invalid assignment of PCP value. The value of PCP should not be "
                        "bigger than the number of supported queues.");
    }

    // Get the corresponding queue from the 2-dimensional matrix
    // standardTrafficClassMapping.
    int queueIndex =
            this->standardTrafficClassMapping[numberOfQueues - 1][pcpValue];

    // Get the corresponding gate and transmit the frame to it.
    EV_TRACE << getFullPath() << ": Sending packet '" << packet
                    << "' with pcp value '" << pcpValue << "' to queue "
                    << queueIndex << endl;

    cGate* outputGate = gate("out", queueIndex);
    send(msg, outputGate);
}

} // namespace nesting
