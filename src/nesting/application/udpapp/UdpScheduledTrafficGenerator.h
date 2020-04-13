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

#ifndef NESTING_APPLICATION_UDPAPP_UDPSCHEDULEDTRAFFICGENERATOR_H_
#define NESTING_APPLICATION_UDPAPP_UDPSCHEDULEDTRAFFICGENERATOR_H_

#include <omnetpp.h>

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/networklayer/common/L3Address.h"

#include "nesting/common/schedule/Schedule.h"
#include "nesting/common/schedule/DatagramScheduleManager.h"

using namespace omnetpp;

namespace nesting {

class UdpScheduledTrafficGenerator : public inet::ApplicationBase, public inet::UdpSocket::ICallback,
        public DatagramScheduleManager::IOperStateListener
{
protected:
    DatagramScheduleManager* scheduleManager;
    int localPort = -1;
    // statistics
    int numSent = 0;
    int numReceived = 0;
protected:
    virtual int numInitStages() const override;
    virtual void initialize(int stage);
    virtual void finish() override;

    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
    virtual void handleStopOperation(inet::LifecycleOperation *operation) override;
    virtual void handleCrashOperation(inet::LifecycleOperation *operation) override;

    virtual void socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet) override;
    virtual void socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication) override;
    virtual void socketClosed(inet::UdpSocket *socket) override;

    virtual void onOperStateChange(SendDatagramEvent sendDatagramEvent) override;
};

} // namespace nesting

#endif
