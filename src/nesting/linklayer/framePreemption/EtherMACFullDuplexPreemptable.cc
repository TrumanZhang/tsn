//
// Copyright (C) 2006 Levente Meszaros
// Copyright (C) 2011 Zoltan Bojthe
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include "EtherMACFullDuplexPreemptable.h"

#include "inet/common/queue/IPassiveQueue.h"
#include "inet/common/NotifierConsts.h"
#include "inet/linklayer/ethernet/EtherFrame.h"
#include "inet/networklayer/common/InterfaceEntry.h"
#include "PreemptedFrame.h"

namespace nesting {

Define_Module(EtherMACFullDuplexPreemptable);

simsignal_t EtherMACFullDuplexPreemptable::preemptCurrentFrameSignal =
        registerSignal("preemptCurrentFrameSignal");
simsignal_t EtherMACFullDuplexPreemptable::transmittedExpressFrameSignal =
        registerSignal("transmittedExpressFrameSignal");
simsignal_t EtherMACFullDuplexPreemptable::transmittedPreemptableFrameSignal =
        registerSignal("transmittedPreemptableFrameSignal");
simsignal_t EtherMACFullDuplexPreemptable::transmittedPreemptableFramePartSignal =
        registerSignal("transmittedPreemptableFramePartSignal");
simsignal_t EtherMACFullDuplexPreemptable::transmittedPreemptableNonFinalSignal =
        registerSignal("transmittedPreemptableNonFinalSignal");
simsignal_t EtherMACFullDuplexPreemptable::transmittedPreemptableFinalSignal =
        registerSignal("transmittedPreemptableFinalSignal");
simsignal_t EtherMACFullDuplexPreemptable::transmittedPreemptableFullSignal =
        registerSignal("transmittedPreemptableFullSignal");
simsignal_t EtherMACFullDuplexPreemptable::expressFrameEnqueuedWhileSendingPreemptableSignal =
        registerSignal("expressFrameEnqueuedWhileSendingPreemptableSignal");

EtherMACFullDuplexPreemptable::~EtherMACFullDuplexPreemptable() {
    cancelAndDelete(recheckForQueuedExpressFrameMsg);
    cancelAndDelete(preemptCurrentFrameMsg);

    delete receivedPreemptedFrame;
    delete currentPreemptableFrame;
    delete currentExpressFrame;
}

void EtherMACFullDuplexPreemptable::initialize(int stage) {
    EtherMACFullDuplex::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        transmissionSelectionModule = getModuleFromPar<TransmissionSelection>(
                par("queueModule"), this);
        transmissionSelectionModule->addListener(this);
        preemptCurrentFrameMsg = new cMessage("preemptCurrentFrame");
    }
}

void EtherMACFullDuplexPreemptable::handleMessage(cMessage *msg) {
    if (!isOperational) {
        handleMessageWhenDown(msg);
        return;
    }

    if (channelsDiffer) {
        readChannelParameters(true);
    }
    if (msg->isSelfMessage()) {
        handleSelfMessage(msg);
    } else if (msg->arrivedOn("upperLayerPreemptableIn")) {
        processFrameFromUpperLayer(check_and_cast<EtherFrame *>(msg));
    } else if (msg->arrivedOn("upperLayerIn")) {
        processFrameFromUpperLayer(check_and_cast<EtherFrame *>(msg));
    } else if (msg->getArrivalGate() == physInGate) {
        PreemptedFrame* pFrame = dynamic_cast<PreemptedFrame*>(msg);
        if (pFrame == nullptr) {
            processMsgFromNetwork(PK(msg));
        } else if (dynamic_cast<EtherPhyFrame *>(pFrame->getCompleteFrame())) {
            if (receivedPreemptedFrame == nullptr
                    || strcmp(receivedPreemptedFrame->getName(),
                            pFrame->getCompleteFrame()->getName()) != 0) {
                delete receivedPreemptedFrame;
                receivedPreemptedFrame = pFrame->getCompleteFrame()->dup();
            }
            preemptedBytesReceived = pFrame->getBytesSent();

            //8 extra bytes for encapsulation with Preamble and SFD
            if (receivedPreemptedFrame->getByteLength()
                    == preemptedBytesReceived + 8) {
                //enough bytes from the preempted frame received -> send it up
                processMsgFromNetwork(
                        PK(
                                check_and_cast<EtherPhyFrame *>(
                                        receivedPreemptedFrame)));
                receivedPreemptedFrame = nullptr;
            }
            delete pFrame;
        }
    } else {
        throw cRuntimeError("Message received from unknown gate!");
    }
}

void EtherMACFullDuplexPreemptable::handleSelfMessage(cMessage *msg) {
    EV_TRACE << "Self-message " << msg << " received" << endl;
    if (msg == endTxMsg)
        handleEndTxPeriod();
    else if (msg == endIFGMsg)
        handleEndIFGPeriod();
    else if (msg == endPauseMsg)
        handleEndPausePeriod();
    else if (strcmp(msg->getName(), "recheckForQueuedExpressFrame") == 0)
        checkForAndRequestExpressFrame();
    else if (msg == preemptCurrentFrameMsg)
        preemptCurrentFrame();
    else if (strcmp(msg->getName(), "holdRequest") == 0) {
        hold(SIMTIME_ZERO);
        delete msg;
    } else {
        throw cRuntimeError("Unknown self message received!");
    }
}

void EtherMACFullDuplexPreemptable::processFrameFromUpperLayer(
        EtherFrame *frame) {
    Enter_Method_Silent("processFrameFromUpperLayer(frame)");
    ASSERT(frame->getByteLength() >= MIN_ETHERNET_FRAME_BYTES);

    bool isExpressFrame = !(frame->arrivedOn("upperLayerPreemptableIn"));

    EV_INFO << getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS) << "ns:" << " Received " << frame << " from upper layer. Express: " << isExpressFrame << endl;

    emit(packetReceivedFromUpperSignal, frame);

    if (frame->getDest().equals(address)) {
        throw cRuntimeError("logic error: frame %s from higher layer has local MAC address as dest (%s)",
        frame->getFullName(), frame->getDest().str().c_str());
    }

    if (frame->getByteLength() > MAX_ETHERNET_FRAME_BYTES) { //FIXME two MAX FRAME BYTES in specif...
        throw cRuntimeError("packet from higher layer (%d bytes) exceeds maximum Ethernet frame size (%d)",
        (int) (frame->getByteLength()), MAX_ETHERNET_FRAME_BYTES);
    }

    if (!connected || disabled) {
        EV_WARN << (!connected ? "Interface is not connected" : "MAC is disabled") << " -- dropping packet " << frame
        << endl;
        emit(dropPkFromHLIfaceDownSignal, frame);
        numDroppedPkFromHLIfaceDown++;
        delete frame;

        requestNextFrameFromExtQueue();
        return;
    }

    // fill in src address if not set
    if (frame->getSrc().isUnspecified()) {
        frame->setSrc(address);
    }

    bool isPauseFrame = (dynamic_cast<EtherPauseFrame *>(frame) != nullptr);

    if (!isPauseFrame) {
        numFramesFromHL++;
        emit(rxPkFromHLSignal, frame);
    }

    if (isExpressFrame && recheckForQueuedExpressFrameMsg!=nullptr && recheckForQueuedExpressFrameMsg->isScheduled()) {
        cancelAndDelete(recheckForQueuedExpressFrameMsg);
    }
    bool nowPreempting = false;
    if ((!transmittingExpressFrame && isExpressFrame) || txQueue.extQueue) {
        ASSERT(
        transmitState == TX_IDLE_STATE || transmitState == PAUSE_STATE
        || (!transmittingExpressFrame && isExpressFrame));
        if (transmittingPreemptableFrame && isExpressFrame) {
            //An express frame arrived at the correct time to preempt a preemptable frame
            currentExpressFrame = frame;
            preemptCurrentFrame();
            nowPreempting = true;
        } else if (isExpressFrame && transmitState == WAIT_IFG_STATE) {
            //If an express frame arrives during the IFG, save it for later. Otherwise the assert in handleEndIFGPeriod() would fail
            currentExpressFrame = frame;
        } else {
            curTxFrame = frame;
        }
    } else {
        if (txQueue.innerQueue->isFull()) {
            throw cRuntimeError("txQueue length exceeds %d -- this is probably due to "
            "a bogus app model generating excessive traffic "
            "(or if this is normal, increase txQueueLimit!)", txQueue.innerQueue->getQueueLimit());
        }
        // store frame and possibly begin transmitting
        EV_DETAIL << getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS) << "ns:" << " Frame " << frame << " arrived from higher layers, enqueueing" << endl;
        txQueue.innerQueue->insertFrame(frame);

        if (!curTxFrame && !txQueue.innerQueue->isEmpty() && transmitState == TX_IDLE_STATE) {
            curTxFrame = (EtherFrame *) txQueue.innerQueue->pop();
        }
    }
    if(onHold && !isExpressFrame && !currentPreemptableFrame) {
        currentPreemptableFrame = curTxFrame;
        curTxFrame = nullptr;
    }
    if (transmitState == TX_IDLE_STATE && !nowPreempting) {
        startFrameTransmission();
    }
}

void EtherMACFullDuplexPreemptable::startFrameTransmission() {
    Enter_Method_Silent("startFrameTransmission()");
    ASSERT(curTxFrame);

    bool isExpressFrame = !(curTxFrame->arrivedOn("upperLayerPreemptableIn"));
    EtherFrame *frame = curTxFrame->dup(); // note: we need to duplicate the frame because we emit a signal with it in endTxPeriod()
    if (frame->getSrc().isUnspecified()) {
        frame->setSrc(address);}

    if (frame->getByteLength() < curEtherDescr->frameMinBytes) {
        frame->setByteLength(curEtherDescr->frameMinBytes); // extra padding
    }

    EV_INFO << getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS) << "ns:" << " Starting Transmission of " << frame << ". Express: " << isExpressFrame << endl;
    ASSERT(!transmittingExpressFrame);
    ASSERT(!transmittingPreemptableFrame);

    //If frame preemption is disabled, treat all frames as express so they are properly displayed
    if (!par("enablePreemptingFrames") || isExpressFrame) {
        //Send frame out normally
        transmittingExpressFrame = true;
        send(encapsulate(frame), physOutGate);
        scheduleAt(transmissionChannel->getTransmissionFinishTime(), endTxMsg);
    } else {
        //"Sending" preemptable frame
        //(block the link for the theoretical type and send zero-time frame later, indicating how many bytes were transferred)

        //Check if it is a new preemptable frame
        if (currentPreemptableFrame == nullptr || strcmp(currentPreemptableFrame->getName(), curTxFrame->getName())!=0) {
            // Reset number of bytes sent if it's a new frame
            preemptedBytesSent = 0;
            delete currentPreemptableFrame;
            //curTxFrame is deleted after every link transmission -> duplicate frame
            currentPreemptableFrame = curTxFrame->dup();
        }

        transmittingPreemptableFrame = true;
        preemptableTransmissionStart = simTime();

        EtherPhyFrame* frameCopyToCalculateLength = encapsulate(frame);
        //Calculate number of bytes to theoretically send
        unsigned int preemptedPacketLength = frameCopyToCalculateLength->getByteLength() - preemptedBytesSent;
        frameCopyToCalculateLength->setByteLength(preemptedPacketLength);
        //Block link for the theoretically needed amount of time
        scheduleAt(simTime() + transmissionChannel->calculateDuration(frameCopyToCalculateLength), endTxMsg);
        delete frameCopyToCalculateLength;
    }
    transmitState = TRANSMITTING_STATE;
    emit(transmitStateSignal, TRANSMITTING_STATE);
}

void EtherMACFullDuplexPreemptable::getNextFrameFromQueue() {
    ASSERT(nullptr == curTxFrame);
    //Check if there is an express frame waiting to be sent after finishing preemption
    if (currentExpressFrame) {
        EV_DETAIL << getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS)
                         << "ns:"
                         << " Queuing express frame now that preemption is finished."
                         << endl;
        curTxFrame = currentExpressFrame;
        currentExpressFrame = nullptr;
        //Check if a new express frame is available
    } else if (par("enablePreemptingFrames")
            && checkForAndRequestExpressFrame()) {
        return;
        //If there is a started preemptable frame, continue it
    } else if (!onHold && currentPreemptableFrame) {
        //Otherwise continue started preemptable frame if it existed
        curTxFrame = currentPreemptableFrame->dup();
        EV_DETAIL << getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS)
                         << "ns:" << " Getting preempted frame " << curTxFrame
                         << " instead of one from the queue." << endl;
        //Otherwise ask queues like in the superclass
    } else if (txQueue.extQueue) {
        requestNextFrameFromExtQueue();
    } else if (txQueue.innerQueue && !txQueue.innerQueue->isEmpty()) {
        curTxFrame = (EtherFrame *) txQueue.innerQueue->pop();
    }
}

void EtherMACFullDuplexPreemptable::handleEndTxPeriod() {
    //TODO add other content from original method
    if (par("enablePreemptingFrames") && transmittingPreemptableFrame) {
        //A (part of a) preemptable frame was sent
        emit(transmittedPreemptableFramePartSignal, currentPreemptableFrame);
        bool beginningOfPreemptableFrame = preemptedBytesSent == 0;
        //Update bytes sent so far for this preemptable frame
        int bytesSentInThisPart = calculatePreemptedPayloadBytesSent(simTime());
        preemptedBytesSent += bytesSentInThisPart;
        if (preemptedBytesSent + 4
                == currentPreemptableFrame->getByteLength()) {
            bytesSentInThisPart += 4;
            //Add last checksum that is included in the encapped frame instead of the "mpacket"/frame preemption calculation
            preemptedBytesSent = currentPreemptableFrame->getByteLength();
            emit(transmittedPreemptableFrameSignal, currentPreemptableFrame);
            if (beginningOfPreemptableFrame) {
                emit(transmittedPreemptableFullSignal, currentPreemptableFrame);
            } else {
                emit(transmittedPreemptableFinalSignal,
                        currentPreemptableFrame);
            }
        } else {
            emit(transmittedPreemptableNonFinalSignal, currentPreemptableFrame);
        }
        EV_INFO << getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS)
                       << "ns:" << " PreemptedFrame " << currentPreemptableFrame
                       << " transmitted: " << bytesSentInThisPart << "B, "
                       << preemptedBytesSent << "/"
                       << currentPreemptableFrame->getByteLength() << "B"
                       << endl;
        PreemptedFrame* preemptedBytesSentMessage = new PreemptedFrame(
                encapsulate(currentPreemptableFrame->dup()));
        preemptedBytesSentMessage->setBytesSent(preemptedBytesSent);
        preemptedBytesSentMessage->setBytesTotal(
                currentPreemptableFrame->getByteLength());
        preemptedBytesSentMessage->setBytesInThisPart(bytesSentInThisPart);
        preemptedBytesSentMessage->setByteLength(0);

        //Temporarily set propagation delay to zero and send PreemptedFrame
        if (dynamic_cast<cDelayChannel *>(transmissionChannel) != nullptr) {
            cDelayChannel* delayChannel = check_and_cast<cDelayChannel *>(
                    transmissionChannel);
            simtime_t originalDelay = delayChannel->getDelay();
            delayChannel->setDelay(0.0);
            send(preemptedBytesSentMessage, physOutGate);
            delayChannel->setDelay(originalDelay.dbl());
        } else if (dynamic_cast<cDatarateChannel *>(transmissionChannel)
                != nullptr) {
            cDatarateChannel* dataRateChannel = check_and_cast<
                    cDatarateChannel *>(transmissionChannel);
            simtime_t originalDelay = dataRateChannel->getDelay();
            dataRateChannel->setDelay(0.0);
            send(preemptedBytesSentMessage, physOutGate);
            dataRateChannel->setDelay(originalDelay.dbl());
        } else {
            if (dynamic_cast<cIdealChannel *>(transmissionChannel) == nullptr) {
                EV_WARN << getFullPath() << " at t="
                               << simTime().inUnit(SIMTIME_NS) << "ns:"
                               << " Unsupported channel configured. Zero-time PreemptedFrame messages may be delayed."
                               << endl;
            }
            send(preemptedBytesSentMessage, physOutGate);
        }

        //If this was the final part of a preemptable frame, delete it
        if (preemptedBytesSent == currentPreemptableFrame->getByteLength()) {
            delete currentPreemptableFrame;
            currentPreemptableFrame = nullptr;
        }
    } else {
        EV_DETAIL << getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS)
                         << "ns:" << " Express Frame " << curTxFrame
                         << " finished to transmit." << endl;
        //Can only be express frame, as it is the default if frame preemption is disabled
        emit(transmittedExpressFrameSignal, curTxFrame);
    }
    transmittingExpressFrame = false;
    transmittingPreemptableFrame = false;
    EtherMACFullDuplex::handleEndTxPeriod();
}

void EtherMACFullDuplexPreemptable::handleEndIFGPeriod() {
    ASSERT(nullptr == curTxFrame);
    if (transmitState != WAIT_IFG_STATE)
        throw cRuntimeError("Not in WAIT_IFG_STATE at the end of IFG period");

    // End of IFG period, okay to transmit
    EV_DETAIL << getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS)
                     << "ns:" << " IFG elapsed" << endl;

    getNextFrameFromQueue();
    beginSendFrames();
}

bool EtherMACFullDuplexPreemptable::checkForAndRequestExpressFrame() {
    ASSERT(isPreemptionNowPossible());
    if (!transmittingExpressFrame && !transmissionSelectionModule->isEmpty()
            && transmissionSelectionModule->hasExpressPacketEnqueued()) {
        EV_INFO << getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS)
                       << "ns:"
                       << " Currently not transmitting or preemption is possible: requesting express frame."
                       << endl;
        transmissionSelectionModule->requestPacket();
        return true;
    }
    return false;
}

void EtherMACFullDuplexPreemptable::preemptCurrentFrame() {
    ASSERT(transmittingPreemptableFrame);
    ASSERT(currentPreemptableFrame);
    ASSERT(!transmittingExpressFrame);
    ASSERT(isPreemptionNowPossible());

    //Don't mark the transmission channel as "transmitting" anymore
    transmissionChannel->forceTransmissionFinishTime(simTime());
    cancelEvent(endTxMsg);

    emit(preemptCurrentFrameSignal, currentPreemptableFrame);

    EV_INFO << getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS)
                   << "ns:" << " Preempting current frame "
                   << currentPreemptableFrame << endl;
    //"End" transmission after four bytes from now (to send preemptable frame-part checksum)
    scheduleAt(simTime() + calculateTransmissionDuration(4), endTxMsg);
}

int EtherMACFullDuplexPreemptable::calculatePreemptedPayloadBytesSent(
        simtime_t timeToCheck) {
    if (!transmittingPreemptableFrame) {
        return 0;
    }
    double transmitRate = getTxRate();
    ASSERT(transmitRate > 0);
    simtime_t timeElapsed = timeToCheck - preemptableTransmissionStart;
    int bytesTransmittedInTotal = (timeElapsed
            / (SimTime(1, SIMTIME_S) / transmitRate)) / 8;
    bytesTransmittedInTotal -= 12; //subtract preamble (7B), SFD (1B) and checksum (4B)
    if (bytesTransmittedInTotal < 0) {
        return 0;
    } else if (bytesTransmittedInTotal
            >= currentPreemptableFrame->getByteLength()) {
        throw cRuntimeError(
                "Supposedly transmitted more bytes than the frame's length.");
    }
    return bytesTransmittedInTotal;
}

bool EtherMACFullDuplexPreemptable::isPreemptionNowPossible() {
    if (!par("enablePreemptingFrames") || !transmittingPreemptableFrame) {
        return true;
    } else if (transmittingExpressFrame) {
        return false;
    }
    int payloadBytesSent = calculatePreemptedPayloadBytesSent(simTime());
    int payloadBytesRemaining = currentPreemptableFrame->getByteLength()
            - payloadBytesSent - preemptedBytesSent;
    if (payloadBytesRemaining
            < Ieee8021q::kFramePreemptionMinFinalPayloadSize) {
        //Final fragment size would be too short -> Preemption forbidden
        return false;
    } else if (payloadBytesSent
            >= Ieee8021q::kFramePreemptionMinNonFinalPayloadSize) {
        //Both the first part as well as the remaining part are large enough -> preemption possible at this time
        return true;
    }
    //Too late to preempt or not early enough to preempt
    return false;
}

simtime_t EtherMACFullDuplexPreemptable::calculateTransmissionDuration(
        int bytes) {
    double transmitRate = getTxRate();
    ASSERT(transmitRate > 0);
    simtime_t timeForOneBit = SimTime(1, SIMTIME_S) / transmitRate;
    return timeForOneBit * bytes * 8;
}

simtime_t EtherMACFullDuplexPreemptable::isPreemptionLaterPossible() {
    if (isPreemptionNowPossible()) {
        return simTime();
    }
    int payloadBytesSentByNow = calculatePreemptedPayloadBytesSent(simTime());
    int payloadBytesRemaining = currentPreemptableFrame->getByteLength()
            - payloadBytesSentByNow - preemptedBytesSent;
    if (payloadBytesSentByNow
            < Ieee8021q::kFramePreemptionMinNonFinalPayloadSize
            && payloadBytesRemaining
                    >= Ieee8021q::kFramePreemptionMinNonFinalPayloadSize + 4
                            - payloadBytesSentByNow
                            + Ieee8021q::kFramePreemptionMinFinalPayloadSize) {
        //Preemption not yet possible, but after a short time -> Need to wait to preempt
        int bytesToWait = (Ieee8021q::kFramePreemptionMinNonFinalPayloadSize
                - payloadBytesSentByNow);
        return simTime() + calculateTransmissionDuration(bytesToWait);
    }
    //Too late to preempt this frame at all
    return SIMTIME_ZERO;
}

void EtherMACFullDuplexPreemptable::packetEnqueued(IPassiveQueue *queue) {
    Enter_Method("packetEnqueued()");
    if (transmittingPreemptableFrame && !transmittingExpressFrame && transmissionSelectionModule->hasExpressPacketEnqueued()) {
        emit(expressFrameEnqueuedWhileSendingPreemptableSignal, 0);
        string loggingPrefix = " Received express frame enqueued notification, ";
        if(isPreemptionNowPossible()) {
            //If direct sending is possible, request the frame
            EV_DETAIL << getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS) << "ns:" << loggingPrefix << "Preemption is possible immediately, requesting frame." << endl;
            transmissionSelectionModule->requestPacket();
        } else {
            simtime_t nextPreemptionPossible = isPreemptionLaterPossible();
            if(!nextPreemptionPossible.isZero()) {
                //If preemption is only possible later, schedule frame request to that time
                EV_DETAIL << getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS) << "ns:" << loggingPrefix << "Preemption is possible later, scheduling express frame request for later." << endl;
                cancelAndDelete(recheckForQueuedExpressFrameMsg);
                recheckForQueuedExpressFrameMsg = new cMessage("recheckForQueuedExpressFrame");
                scheduleAt(nextPreemptionPossible, recheckForQueuedExpressFrameMsg);
            } else {
                EV_DETAIL << getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS) << "ns:" << loggingPrefix << "Preemption is not possible at all." << endl;
            }
        }
        //TODO think of last four bytes in 124 example
        //TODO test parameter functionality
        //Signals: Enter_Method oder originale mac davor, oder über super aufrufen, oder irgendwie ownen
    }
}

void EtherMACFullDuplexPreemptable::hold(simtime_t delay) {
    Enter_Method("hold()");
    if(par("enablePreemptingFrames")) {
        if(delay.isZero()) {
            //Execute hold request -> preempt current preemptable traffic, don't allow new one
            EV_INFO << getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS) << "ns:" << " Got hold request."<<endl;
            onHold = true;
            if (transmittingPreemptableFrame && isPreemptionNowPossible()) {
                //Preempt now if possible
                preemptCurrentFrame();
            } else if (transmittingPreemptableFrame) {
                //Preempt as soon as possible, if possible at all
                simtime_t nextPreemptionPossible = isPreemptionLaterPossible();
                if (!nextPreemptionPossible.isZero()) {
                    if(preemptCurrentFrameMsg->isScheduled()) {
                        cancelEvent(preemptCurrentFrameMsg);
                    }
                    scheduleAt(nextPreemptionPossible, preemptCurrentFrameMsg);
                    EV_INFO<<getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS) << "ns:" << " Scheduling preemption after first mPacket is large enough."<<endl;
                } else {
                    EV_INFO<<getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS) << "ns:" << " Preemption is not possible at all."<<endl;}
            } else if (transmitState == TX_IDLE_STATE) {
                //If no traffic is sent currently, request frame if needed
                EV_INFO<<getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS) << "ns:" << " No frame to transmit. Requesting express frame."<<endl;
                getNextFrameFromQueue();
            }
        } else {
            //Schedule hold on specified time
            EV_INFO<<getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS) << "ns:" << " Scheduling hold in "<<delay.inUnit(SIMTIME_US)<<"us."<<endl;
            scheduleAt(simTime() + delay, new cMessage("holdRequest", 1));
        }
    }
}

void EtherMACFullDuplexPreemptable::release() {
    Enter_Method("release()");
    if(par("enablePreemptingFrames")) {
        onHold = false;
        EV_INFO<<getFullPath() << " at t=" << simTime().inUnit(SIMTIME_NS) << "ns:" << " Got release request. Requesting frame."<<endl;
        //Clear pending requests from this module, otherwise
        transmissionSelectionModule->removePendingRequests();
        if(!transmittingExpressFrame) {
            if(!endIFGMsg->isScheduled()) {
                getNextFrameFromQueue();
                beginSendFrames();
            }
            //check for preemptable frame, assert that no preempted frame is ready for transmission
        }
    }
}

simtime_t EtherMACFullDuplexPreemptable::getHoldAdvance() {
    Enter_Method_Silent("release()");
    //Calculate the hold advance i.e. the maximum delay needed before express traffic can flow after a preemption/hold event
    int bitsToWait = INTERFRAME_GAP_BITS + Ieee8021q::kFramePreemptionMinNonFinalPayloadSize
    + Ieee8021q::kFramePreemptionMinFinalPayloadSize + 4;
    double transmitRate = getTxRate();
    ASSERT(transmitRate > 0);
    simtime_t timeForOneBit = SimTime(1, SIMTIME_S) / transmitRate;
    return timeForOneBit * bitsToWait;
}

bool EtherMACFullDuplexPreemptable::isOnHold() {
    Enter_Method_Silent("isOnHold()");
    return onHold;
}

void EtherMACFullDuplexPreemptable::refreshDisplay() const {
    char buf[200];
    const char* currentPreemptableFrameName =
            (nullptr == currentPreemptableFrame) ?
                    "nullptr" : currentPreemptableFrame->getName();
    const char* currentExpressFrameName =
            (nullptr == currentExpressFrame) ?
                    "nullptr" : currentExpressFrame->getName();
    sprintf(buf,
            "onHold: %s\ntransmittingExpressFrame: %s\ntransmittingPreemptableFrame: %s\ncurrentPreemptableFrame: %s\ncurrentExpressFrame: %s",
            onHold ? "true" : "false",
            transmittingExpressFrame ? "true" : "false",
            transmittingPreemptableFrame ? "true" : "false",
            currentPreemptableFrameName, currentExpressFrameName);
    getDisplayString().setTagArg("t", 0, buf);
}

bool EtherMACFullDuplexPreemptable::isFramePreemptionEnabled() {
    return par("enablePreemptingFrames");
}

}
// namespace inet

