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

import inet.linklayer.ethernet.EtherMacFullDuplex;
import inet.linklayer.ethernet.IEtherEncap;
import nesting.application.ethernet.SchedEtherVLANTrafGen;
import nesting.ieee8021q.clock.IClock;
import nesting.ieee8021q.queue.gating.ScheduleSwap;
import nesting.linklayer.ethernet.VLANEncap;


//
// Simple Host with the capability to send out frames according to a given schedule
//
// @see ~EtherVLANTaggedTrafGen
//
module VLANEtherHost
{
    parameters:
        *.interfaceTableModule = default("");
        @networkNode();
        @labels(node,ethernet-node);
        @display("i=device/pc2;bgb=385.52002,483.12003");
    gates:
        inout ethg @labels(EtherFrame-conn);
    submodules:
        tsn_app: SchedEtherVLANTrafGen {
            @display("p=212.768,69.296005");
        }
        etherEncap: <default("EtherEncap")> like IEtherEncap {
            @display("p=138,287");
        }
        vlanEncap: VLANEncap {
            @display("p=138,174");
        }
        mac: EtherMacFullDuplex {
            @display("p=138,402;q=txQueue");
        }
        clock: <default("IdealClock")> like IClock {
            @display("p=330,69");
        }
        scheduleSwap: ScheduleSwap {
            usedInHost = true;
            
            @display("p=330,149;b=40,40");
        }
    connections:
        tsn_app.out --> vlanEncap.upperLayerIn;
        vlanEncap.lowerLayerOut --> etherEncap.upperLayerIn;
        etherEncap.lowerLayerOut --> mac.upperLayerIn;

        mac.upperLayerOut --> etherEncap.lowerLayerIn;
        etherEncap.upperLayerOut --> vlanEncap.lowerLayerIn;
        vlanEncap.upperLayerOut --> tsn_app.in;
        mac.phys <--> ethg;
}
