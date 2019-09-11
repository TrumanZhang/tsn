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

#include "EnhancedIeee8021qEncap.h"

#include <algorithm>

#include "inet/linklayer/ethernet/EtherEncap.h"
#include "inet/common/Simsignals.h"

#include "../vlan/VlanTag_m.h"

namespace nesting {

Define_Module(EnhancedIeee8021qEncap);

void EnhancedIeee8021qEncap::initialize(int stage)
{
    cSimpleModule::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        vlanTagType = par("vlanTagType");
        parseParameters("inboundVlanIdFilter", "inboundVlanIdMap", inboundVlanIdFilter, inboundVlanIdMap);
        parseParameters("outboundVlanIdFilter", "outboundVlanIdMap", outboundVlanIdFilter, outboundVlanIdMap);
        WATCH_VECTOR(inboundVlanIdFilter);
        WATCH_MAP(inboundVlanIdMap);
        WATCH_VECTOR(outboundVlanIdFilter);
        WATCH_MAP(outboundVlanIdMap);
    }
}

void EnhancedIeee8021qEncap::parseParameters(const char *filterParameterName, const char *mapParameterName, std::vector<int>& vlanIdFilter, std::map<int, int>& vlanIdMap)
{
    cStringTokenizer filterTokenizer(par(filterParameterName));
    while (filterTokenizer.hasMoreTokens())
        vlanIdFilter.push_back(atoi(filterTokenizer.nextToken()));
    cStringTokenizer mapTokenizer(par(mapParameterName));
    while (mapTokenizer.hasMoreTokens()) {
        auto fromVlanId = atoi(mapTokenizer.nextToken());
        auto toVlanId = atoi(mapTokenizer.nextToken());
        vlanIdMap[fromVlanId] = toVlanId;
    }
}

inet::Ieee8021qHeader *EnhancedIeee8021qEncap::findVlanTag(const inet::Ptr<inet::EthernetMacHeader>& ethernetMacHeader)
{
    if (*vlanTagType == 'c')
        return ethernetMacHeader->getCTagForUpdate();
    else if (*vlanTagType == 's')
        return ethernetMacHeader->getSTagForUpdate();
    else
        throw cRuntimeError("Unknown VLAN tag type");
}

inet::Ieee8021qHeader *EnhancedIeee8021qEncap::addVlanTag(const inet::Ptr<inet::EthernetMacHeader>& ethernetMacHeader)
{
    auto vlanTag = new inet::Ieee8021qHeader();
    ethernetMacHeader->setChunkLength(ethernetMacHeader->getChunkLength() + inet::B(4));
    if (*vlanTagType == 'c')
        ethernetMacHeader->setCTag(vlanTag);
    else if (*vlanTagType == 's')
        ethernetMacHeader->setSTag(vlanTag);
    else
        throw cRuntimeError("Unknown VLAN tag type");
    return vlanTag;
}

inet::Ieee8021qHeader *EnhancedIeee8021qEncap::removeVlanTag(const inet::Ptr<inet::EthernetMacHeader>& ethernetMacHeader)
{
    ethernetMacHeader->setChunkLength(ethernetMacHeader->getChunkLength() - inet::B(4));
    if (*vlanTagType == 'c')
        return ethernetMacHeader->dropCTag();
    else if (*vlanTagType == 's')
        return ethernetMacHeader->dropSTag();
    else
        throw cRuntimeError("Unknown VLAN tag type");
}

void EnhancedIeee8021qEncap::processPacket(inet::Packet *packet, std::vector<int>& vlanIdFilter, std::map<int, int>& vlanIdMap, cGate *gate)
{
    packet->trimFront();
    const auto& ethernetMacHeader = packet->removeAtFront<inet::EthernetMacHeader>();
    inet::Ieee8021qHeader *vlanTag = findVlanTag(ethernetMacHeader);
    auto oldVlanId = vlanTag != nullptr ? vlanTag->getVid() : -1;
    auto vlanReq = packet->removeTagIfPresent<VlanReq>();
    auto newVlanId = vlanReq != nullptr ? vlanReq->getVlanId() : oldVlanId;
    bool acceptPacket = vlanIdFilter.empty() || std::find(vlanIdFilter.begin(), vlanIdFilter.end(), newVlanId) != vlanIdFilter.end();
    if (acceptPacket) {
        auto it = vlanIdMap.find(newVlanId);
        if (it != vlanIdMap.end())
            newVlanId = it->second;
        if (newVlanId != oldVlanId) {
            EV_WARN << "Changing VLAN ID: new = " << newVlanId << ", old = " << oldVlanId << ".\n";
            if (oldVlanId == -1 && newVlanId != -1)
                addVlanTag(ethernetMacHeader)->setVid(newVlanId);
            else if (oldVlanId != -1 && newVlanId == -1)
                removeVlanTag(ethernetMacHeader);
            else
                vlanTag->setVid(newVlanId);
            packet->insertAtFront(ethernetMacHeader);
            auto oldFcs = packet->removeAtBack<inet::EthernetFcs>();
            inet::EtherEncap::addFcs(packet, oldFcs->getFcsMode());
        }
        else
            packet->insertAtFront(ethernetMacHeader);
        packet->addTagIfAbsent<VlanInd>()->setVlanId(newVlanId);
        send(packet, gate);
    }
    else {
        EV_WARN << "Received VLAN ID = " << oldVlanId << " is not accepted, dropping packet.\n";
        inet::PacketDropDetails details;
        details.setReason(inet::OTHER_PACKET_DROP);
        emit(inet::packetDroppedSignal, packet, &details);
        delete packet;
    }
}

void EnhancedIeee8021qEncap::handleMessage(cMessage *message)
{
    if (message->getArrivalGate()->isName("upperLayerIn"))
        processPacket(check_and_cast<inet::Packet *>(message), outboundVlanIdFilter, outboundVlanIdMap, gate("lowerLayerOut"));
    else if (message->getArrivalGate()->isName("lowerLayerIn"))
        processPacket(check_and_cast<inet::Packet *>(message), inboundVlanIdFilter, inboundVlanIdMap, gate("upperLayerOut"));
    else
        throw cRuntimeError("Unknown message");
}

} //namespace
