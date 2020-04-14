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

UdpScheduledTrafficGenerator::~UdpScheduledTrafficGenerator()
{
    cancelEvent(selfMsg);
}

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
    ApplicationBase::finish();
}

void UdpBasicApp::setSocketOptions()
{
    int timeToLive = par("timeToLive");
    if (timeToLive != -1)
        socket.setTimeToLive(timeToLive);

    int typeOfService = par("typeOfService");
    if (typeOfService != -1)
        socket.setTypeOfService(typeOfService);

    const char *multicastInterface = par("multicastInterface");
    if (multicastInterface[0]) {
        IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        InterfaceEntry *ie = ift->getInterfaceByName(multicastInterface);
        if (!ie)
            throw cRuntimeError("Wrong multicastInterface setting: no interface named \"%s\"", multicastInterface);
        socket.setMulticastOutputInterface(ie->getInterfaceId());
    }

    bool receiveBroadcast = par("receiveBroadcast");
    if (receiveBroadcast)
        socket.setBroadcast(true);

    bool joinLocalMulticastGroups = par("joinLocalMulticastGroups");
    if (joinLocalMulticastGroups) {
        MulticastGroupList mgl = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this)->collectMulticastGroups();
        socket.joinLocalMulticastGroups(mgl);
    }
    socket.setCallback(this);
}

void UdpScheduledTrafficGenerator::processStart()
{
    socket.setOutputGate(gate("socketOut"));
    const char *localAddress = par("localAddress");
    socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
    setSocketOptions();
}

void UdpScheduledTrafficGenerator::processSend()
{
    // TODO
}

void UdpScheduledTrafficGenerator::processStop()
{
    socket.close();
}

void UdpScheduledTrafficGenerator::processPacket(Packet *msg)
{
    // TODO
}

void UdpScheduledTrafficGenerator::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        assert(msg == &selfMsg);
        switch (msg->getKind()) {
        case START:
            processStart();
            break;
        case STOP:
            processStop();
        case SEND:
            processSend();
        default:
            throw cRuntimeError("Invalid kind %d in self message", (int)selfMsg->getKind());
    } else {
        socket.processMessage(msg);
    }
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
