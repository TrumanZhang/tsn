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

#include "UdpScheduledTrafficApp.h"

namespace nesting {

Define_Module(UdpScheduledTrafficApp);

void UdpScheduledTrafficApp::initialize(int stage)
{
    ApplicationBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        schedule = buildSchedule(par("trafficSchedule").xmlValue());
        localPort = par("localPort").intValue();
        // statistics
        WATCH(numSent);
        WATCH(numReceived);
    }
}

void UdpScheduledTrafficApp::finish()
{
    // TODO - Generated method body
}

void UdpScheduledTrafficApp::handleMessageWhenUp(cMessage *msg)
{
    // TODO
}

void UdpScheduledTrafficApp::handleStartOperation(inet::LifecycleOperation *operation)
{
    // TODO
}

void UdpScheduledTrafficApp::handleStopOperation(inet::LifecycleOperation *operation)
{
    // TODO
}

void UdpScheduledTrafficApp::handleCrashOperation(inet::LifecycleOperation *operation)
{
    // TODO
}

void UdpScheduledTrafficApp::socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet)
{
    // TODO
}

void UdpScheduledTrafficApp::socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication)
{
    // TODO
}

void UdpScheduledTrafficApp::socketClosed(inet::UdpSocket *socket)
{
    // TODO
}

} //namespace
