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

#include "EtherMacMerge.h"

namespace nesting {

Define_Module(EtherMacMerge);

simsignal_t EtherMacMerge::preemptCurrentFrameSignal = registerSignal(
        "preemptCurrentFrameSignal");
simsignal_t EtherMacMerge::transmittedExpressFrameSignal = registerSignal(
        "transmittedExpressFrameSignal");
simsignal_t EtherMacMerge::transmittedPreemptableFrameSignal = registerSignal(
        "transmittedPreemptableFrameSignal");
simsignal_t EtherMacMerge::transmittedPreemptableFramePartSignal =
        registerSignal("transmittedPreemptableFramePartSignal");
simsignal_t EtherMacMerge::transmittedPreemptableNonFinalSignal =
        registerSignal("transmittedPreemptableNonFinalSignal");
simsignal_t EtherMacMerge::transmittedPreemptableFinalSignal = registerSignal(
        "transmittedPreemptableFinalSignal");
simsignal_t EtherMacMerge::transmittedPreemptableFullSignal = registerSignal(
        "transmittedPreemptableFullSignal");
simsignal_t EtherMacMerge::expressFrameEnqueuedWhileSendingPreemptableSignal =
        registerSignal("expressFrameEnqueuedWhileSendingPreemptableSignal");

EtherMacMerge::~EtherMacMerge() {

}

void EtherMacMerge::initialize() {

}

void EtherMacMerge::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        this->handleSelfMessage(msg);
    } else {
        if (msg->arrivedOn("eUpperLayerIn")
                || msg->arrivedOn("pUpperLayerIn")) {
            this->handleMessageFromUpper(msg);
        }
    }
}

void EtherMacMerge::handleSelfMessage(cMessage *msg) {
    if (msg == sendFinalPreemptZeroMsg) {
        ASSERT(firstPreemptZeroMsg != nullptr);
        ASSERT(finalPreemptZeroMsg == nullptr);
        ASSERT(curTxFrame != nullptr);
    }
}

void EtherMacMerge::handleMessageFromUpper(cMessage *msg) {
    inet::Packet* mPacket = check_and_cast<inet::Packet *>(msg);
    if (mPacket->arrivedOn("pUpperLayerIn")) {
        if (this->getState() == IDLE) {
            ASSERT(firstPreemptZeroMsg == nullptr);
            ASSERT(finalPreemptZeroMsg != nullptr);
            ASSERT(curTxFrame == nullptr);
            simtime_t endTXTime = simTime()
                    + calcTransmissionDelay(
                            Ieee8021q::getFinalEthernet2FrameBitLength(
                                    mPacket));
            scheduleAt(endTXTime, sendFinalPreemptZeroMsg);
        } else {
            throw cRuntimeError("Packet enqueued at wrong time.");
        }
    } else if (mPacket->arrivedOn("eUpperLayerIn")) {

    }
}

void EtherMacMerge::setState(macMergeState state) {
    switch (state) {
    case IDLE:
        state = IDLE;
        break;
    case TX_EXPRESS_FRAME:
        state = TX_EXPRESS_FRAME;
        break;
    case TX_PREEMPTIBLE_FRAME:
        state = TX_PREEMPTIBLE_FRAME;
        break;
    case PREEMPTED:
        state = PREEMPTED;
        break;
    case RX_EXPRESS_FRAME:
        state = RX_EXPRESS_FRAME;
        break;
    case RX_PREEMPTIBLE_FRAME:
        state = RX_PREEMPTIBLE_FRAME;
        break;
    default:
        throw cRuntimeError("%s is undefined state.", state);
        break;
    }
}

simtime_t EtherMacMerge::calcTransmissionDelay(uint64_t bit) {
    return SIMTIME_ZERO;
}

} // namespace nesting

