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

package nesting.application.ethernet;

//
// Traffic generator application for VLAN tagged ethernet frames. This
// module should be connected to a VLANEncap module and the VLANEncap
// module should be connected to an EtherEncap module to provide encap/decap
// functionality for VLAN Tag and Ethernet Header.
//
// This application module does not generate encapsulated packets but adds
// the necessary information, that other modules (EtherEncap, VLANEncap) can
// provide the necessary encapsulation/decapsulation. Therefore an
// Ieee8021QCtrl control information is added to packets.
//
// @see ~VLANEncap, ~EtherEncap, ~Ieee8021QCtrl
//
simple VlanEtherTrafGenSched
{
    parameters:
        @class(VlanEtherTrafGenSched);

        @signal[sentPkId](type=long); // type=unique packet id
        @signal[rcvdPkId](type=long); // type=unique packet id
        @signal[sentPk](type=inet::Packet);
        @signal[rcvdPk](type=inet::Packet);
		@signal[packetMap](type=long); // should be non-negative

        //Actual schedule has to be set in .ini file
        xml initialSchedule = default(xml("<schedule><cycle>100</cycle></schedule>"));
        xml emptySchedule = default(xml("<schedule><cycle>100</cycle></schedule>"));
        string clockModule = default("^.clock");
        string hostModule = default("^");
        bool verbose = default(false);
        double jitter @unit(s) = default(0s); // random time, for which transmission of packet can be delayed. Unit depends on clock rate.
        int seed = default(0);
        string mapping = default("");
        
        @statistic[packetReceived](title="packets received"; source=rcvdPkId; record=count, vector; interpolationmode=none);
        @statistic[packetSent](title="packets sent"; source=sentPkId; record=count, vector; interpolationmode=none);
        @statistic[endToEndDelay](title="end-to-end delay"; source="dataAge(rcvdPk)"; unit=s; record=histogram,vector; interpolationmode=none);
    gates:
        input in;
        output out;
}



