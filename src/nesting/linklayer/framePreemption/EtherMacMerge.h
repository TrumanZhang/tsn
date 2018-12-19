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

#ifndef NESTING_LINKLAYER_FRAMEPREEMPTION_ETHERMACMERGE_H_
#define NESTING_LINKLAYER_FRAMEPREEMPTION_ETHERMACMERGE_H_

#include <omnetpp.h>
#include "../../ieee8021q/Ieee8021q.h"
#include "MPacketPointerTag_m.h"
#include "inet/common/packet/Packet.h"

using namespace omnetpp;

namespace nesting {

class EtherMacMerge: public cSimpleModule {
public:
    enum macMergeState {
        TX_EXPRESS_FRAME,
        TX_PREEMPTIBLE_FRAME,
        IDLE,
        PREEMPTED,
        RX_EXPRESS_FRAME,
        RX_PREEMPTIBLE_FRAME
    };

    int64_t minFrag; // number of octets required for a minimum size fragment
    int addFragSize; // integer in range 0:3 that controls minimum non-final packet length specified in 802.3/99.4.4.
    ~EtherMacMerge();
protected:
    static simsignal_t preemptCurrentFrameSignal;
    static simsignal_t transmittedExpressFrameSignal;
    static simsignal_t transmittedPreemptableFrameSignal;
    static simsignal_t transmittedPreemptableFramePartSignal;
    static simsignal_t transmittedPreemptableNonFinalSignal;
    static simsignal_t transmittedPreemptableFinalSignal;
    static simsignal_t transmittedPreemptableFullSignal;
    static simsignal_t expressFrameEnqueuedWhileSendingPreemptableSignal;

    inet::Packet* curTxFrame = nullptr;
    inet::Packet* finalPreemptZeroMsg = nullptr;
    inet::Packet* firstPreemptZeroMsg = nullptr;

    cMessage* sendFinalPreemptZeroMsg = new cMessage("sendFinalPreemptZeroMsg");

    macMergeState state = IDLE;

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void handleSelfMessage(cMessage *msg);
    void handleMessageFromUpper(cMessage *msg);
    void handleMessageFromMac(cMessage *msg);
    void setState(macMergeState state);
    macMergeState getState() const {
        return state;
    }
    simtime_t calcTransmissionDelay(uint64_t bit);
};
}

#endif /* NESTING_LINKLAYER_FRAMEPREEMPTION_ETHERMACMERGE_H_ */
