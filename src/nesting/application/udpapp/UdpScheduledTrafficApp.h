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

#ifndef __NESTINGNG_UDPSCHEDULEDTRAFFIC_H_
#define __NESTINGNG_UDPSCHEDULEDTRAFFIC_H_

#include <omnetpp.h>

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/networklayer/common/L3Address.h"

#include "nesting/common/schedule/Schedule.h"

using namespace omnetpp;

namespace nesting {

/**
 * TODO - Generated class
 */
class UdpScheduledTrafficApp : public inet::ApplicationBase, public inet::UdpSocket::ICallback
{
public:
    struct SendEvent {
        inet::L3Address destAddress;
        int destPort;
        int pcp;
        int vid;
        int payloadSize;
        int maxPayloadFragmentSize;
    };
protected:
    int localPort = -1;
    Schedule<SendEvent> schedule;
    // statistics
    int numSent = 0;
    int numReceived = 0;
public:
    static Schedule<SendEvent> buildSchedule(cXMLElement *xml);
protected:
    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    virtual void initialize(int stage);
    virtual void finish() override;

    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
    virtual void handleStopOperation(inet::LifecycleOperation *operation) override;
    virtual void handleCrashOperation(inet::LifecycleOperation *operation) override;

    virtual void socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet) override;
    virtual void socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication) override;
    virtual void socketClosed(inet::UdpSocket *socket) override;

};

} //namespace

#endif
