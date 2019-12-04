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

package nesting.node.ethernet;

import inet.linklayer.contract.IEthernetInterface;
import inet.networklayer.common.InterfaceTable;
import nesting.application.ethernet.VlanEtherTrafGen;
import nesting.common.time.IOscillator;
import nesting.ieee8021q.clock.IClock;


//
// Simple Host with the capability to create and receive VLAN tagged ethernet
// traffic. Mainly used for test purposes.
//
// @see ~VlanEtherTrafGen
//
module VlanEtherHostQ
{
    parameters:
        @display("i=device/pc2;bgb=381,407");
        @networkNode();
        @labels(node,ethernet-node);
        **.clockModule = default(absPath(".legacyClock"));
        **.oscillatorModule = default(absPath(".oscillator"));
    gates:
        inout ethg @labels(EtherFrame-conn);
    submodules:
        trafGenApp: VlanEtherTrafGen {
            @display("p=232,70");
        }
        oscillator: <default("IdealOscillator")> like IOscillator {
            @display("p=53,101;is=s");
        }
        legacyClock: <default("IdealClock")> like IClock {
            @display("p=53,39;is=s");
        }
        eth: <default("VlanEthernetInterface")> like IEthernetInterface {
            parameters:
                @display("p=232,283,row,150;q=txQueue");
                interfaceTableModule = "^.interfaceTable";
                mac.mtu = 1500B;
        }
        interfaceTable: InterfaceTable {
            @display("p=53,164;is=s");
        }
    connections:
        eth.upperLayerIn <-- trafGenApp.out;
        eth.upperLayerOut --> trafGenApp.in;
        eth.phys <--> ethg;
}
