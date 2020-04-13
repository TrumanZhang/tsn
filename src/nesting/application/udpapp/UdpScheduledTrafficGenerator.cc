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

#include "nesting/application/udpapp/UdpScheduledTrafficGenerator.h"

#include "inet/common/ModuleAccess.h"

namespace nesting {

Define_Module(UdpScheduledTrafficGenerator);

int UdpScheduledTrafficGenerator::numInitStages() const {
    return inet::NUM_INIT_STAGES;
}

void UdpScheduledTrafficGenerator::initialize(int stage)
{
    ApplicationBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        localPort = par("localPort").intValue();
        scheduleManager = getModuleFromPar<DatagramScheduleManager>(par("datagramScheduleManagerModule"), this);
        scheduleManager->subscribeOperStateChanges(*this);
        // statistics
        WATCH(numSent);
        WATCH(numReceived);
    }
}

void UdpScheduledTrafficGenerator::finish()
{
    scheduleManager->unsubscribeOperStateChanges(*this);
}

void UdpScheduledTrafficGenerator::handleMessageWhenUp(cMessage *msg)
{
    // TODO
}

void UdpScheduledTrafficGenerator::handleStartOperation(inet::LifecycleOperation *operation)
{
    // TODO
}

void UdpScheduledTrafficGenerator::handleStopOperation(inet::LifecycleOperation *operation)
{
    // TODO
}

void UdpScheduledTrafficGenerator::handleCrashOperation(inet::LifecycleOperation *operation)
{
    // TODO
}

void UdpScheduledTrafficGenerator::socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet)
{
    // TODO
}

void UdpScheduledTrafficGenerator::socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication)
{
    // TODO
}

void UdpScheduledTrafficGenerator::socketClosed(inet::UdpSocket *socket)
{
    // TODO
}

void UdpScheduledTrafficGenerator::onOperStateChange(SendDatagramEvent sendDatagramEvent)
{
    // TODO
}

} //namespace
