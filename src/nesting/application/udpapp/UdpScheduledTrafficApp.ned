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

package nesting.application.udpapp;

import inet.applications.contract.IApp;


//
// TODO auto-generated module
//
simple UdpScheduledTrafficApp like IApp
{
    parameters:
        @display("i=block/app");
        string interfaceTableModule; // The path to the InterfaceTable module
        string clockModule;
        xml trafficSchedule = xml("<schedule/>");
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
        @signal[packetDropped](type=inet::Packet);
        @signal[outOfOrderPk](type=inet::Packet);
        @statistic[packetSent](title="packets sent"; source=packetSent; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[dropPk](title="packets dropped"; source=packetDropped; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[outOfOrderPk](title="packets received out of order"; source=outOfOrderPk; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[endToEndDelay](title="delay"; source="dataAge(packetReceived)"; unit=s; record=histogram,vector; interpolationmode=none);
    gates:
        input socketIn @labels(UdpControlInfo/up);
        output socketOut @labels(UdpControlInfo/down);
}
