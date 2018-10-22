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

#include "ForwardingRelayUnit.h"
#define COMPILETIME_LOGLEVEL omnetpp::LOGLEVEL_TRACE

namespace nesting {

Define_Module(ForwardingRelayUnit);

void ForwardingRelayUnit::initialize() {
    fdb = getModuleFromPar<FilteringDatabase>(par("filteringDatabaseModule"),
            this);
    numberOfPorts = par("numberOfPorts");
}

void ForwardingRelayUnit::handleMessage(cMessage *msg) {
    cPacket* packet = check_and_cast<cPacket*>(msg);
    Ieee8021QCtrl* ctrlInfo = check_and_cast<Ieee8021QCtrl*>(
            packet->getControlInfo());

    // Distinguish between broadcast-, multicast- and unicast-ethernet frames
    if (ctrlInfo->getDestinationAddress().isBroadcast()) {
        processBroadcast(packet);
    } else if (ctrlInfo->getDestinationAddress().isMulticast()) {
        processMulticast(packet);
    } else {
        processUnicast(packet);
    }
}

void ForwardingRelayUnit::processBroadcast(cPacket* packet) {
    // Flood packets everywhere except of ingress port
    // TODO this is just a temporary solution not sure how correct that is
    for (int portId = 0; portId < gateSize("out"); portId++) {
        cGate *outputGate = gate("out", portId);
        if (!packet->arrivedOn("in", portId)) {
            send(duplicatePacketWithCtrlInfo(packet), outputGate);
        }
    }
    delete packet;
}

void ForwardingRelayUnit::processMulticast(cPacket* packet) {
    processBroadcast(packet);
}

void ForwardingRelayUnit::processUnicast(cPacket* packet) {
    // Control info is needed to retrieve destination MAC address
    Ieee8021QCtrl* ctrlInfo = check_and_cast<Ieee8021QCtrl*>(
            packet->getControlInfo());

    //Learning MAC port mappings
    fdb->insert(ctrlInfo->getSourceAddress(), simTime(),
            packet->getArrivalGate()->getIndex());
    int forwardingPort = fdb->getPort(ctrlInfo->getDestinationAddress(),
            simTime());

    //Routing entry available?
    if (forwardingPort == -1) {
        processBroadcast(duplicatePacketWithCtrlInfo(packet));
        EV_INFO << getFullPath() << ": Broadcasting packets `" << packet
                       << "` to all ports" << endl;
    } else {
        EV_INFO << getFullPath() << ": Forwarding packet `" << packet
                       << "` to port " << forwardingPort << endl;
        send(duplicatePacketWithCtrlInfo(packet), gate("out", forwardingPort));
    }

    delete packet;
}

cPacket* ForwardingRelayUnit::duplicatePacketWithCtrlInfo(cPacket* packet) {
    // Duplicate packet
    cPacket* dupPacket = packet->dup();

    // Duplicate control because it is not duplicated implicitly and attach it to
    // the duplicated packet
    Ieee8021QCtrl* ctrlInfo = check_and_cast<Ieee8021QCtrl*>(
            packet->getControlInfo());
    Ieee8021QCtrl* dupCtrlInfo = new Ieee8021QCtrl(*ctrlInfo);
    dupPacket->setControlInfo(dupCtrlInfo);

    return dupPacket;
}

} // namespace nesting
