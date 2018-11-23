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

    if (numberOfQueues > Ieee8021q::kNumberOfPCPValues || numberOfQueues < 1) {
        throw new cRuntimeError(
                "Invalid assignment of numberOfQueues. Number of queues should not "
                        "be bigger than the number of all possible pcp values!");
    }
}

void QueuingFrames::handleMessage(cMessage *msg) {
    Packet *packet = check_and_cast<Packet *>(msg);

    // TODO check if correct tag is found
    auto vlanTag = packet->getTag<VLANTagBase>();

    int pcpValue = vlanTag->getPcp();

    // Check whether the PCP value is correct.
    if (pcpValue > Ieee8021q::kNumberOfPCPValues) {
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
