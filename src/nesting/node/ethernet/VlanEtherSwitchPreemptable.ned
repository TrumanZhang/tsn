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

import inet.common.MessageDispatcher;
import inet.common.queue.Delayer;
import inet.linklayer.contract.IMacRelayUnit;
import inet.networklayer.common.InterfaceTable;
import nesting.ieee8021q.clock.IClock;
import nesting.ieee8021q.queue.gating.ScheduleSwap;
import nesting.ieee8021q.relay.FilteringDatabase;
import nesting.linklayer.ethernet.VlanEthernetInterfaceSwitchPreemptable;
import nesting.common.time.IOscillator;


//
// This module implements a switch that supports frame preemption.
//
module VlanEtherSwitchPreemptable
{
    parameters:
        @networkNode();
        @display("i=device/switch;bgb=1429,570");
        *.vlanTagType = default("c");
        *.interfaceTableModule = absPath(".interfaceTable");
        *.filteringDatabaseModule = absPath(".filteringDatabase");
        **.clockModule = default(absPath(".legacyClock"));
        **.oscillatorModule = default(absPath(".oscillator"));
    gates:
        inout ethg[];
    submodules:
        eth[sizeof(ethg)]: VlanEthernetInterfaceSwitchPreemptable {
            mac.queueModule = "^.^.eth[" + string(index) + "].queuing.transmissionSelection";
            mac.mtu = 1500B;
            mac.promiscuous = true;
            mac.interfaceTableModule = "^.interfaceTable";
            queuing.tsAlgorithms[*].macModule = "^.^.^.eth[" + string(index) + "].mac";
            queuing.gateController.macModule = "^.^.^.eth[" + string(index) + "].mac";
            encapE.typename = "EtherEncapDummy";
            encapP.typename = "EtherEncapDummy";
            @display("p=249,480,r,150");
        }
        relayUnit: <default("ForwardingRelayUnit")> like IMacRelayUnit {
            numberOfPorts = sizeof(ethg);
            @display("p=800,264");
        }
        processingDelay: Delayer {
            @display("p=925,264,r,200;i=block/delay");
            delay = default(4us);
        }
        oscillator: <default("IdealOscillator")> like IOscillator {
            @display("p=181.88078,30.313463;is=s");
        }
        legacyClock: <default("IdealClock")> like IClock {
            @display("p=290.73367,30.313463;is=s");
        }
        filteringDatabase: FilteringDatabase {
            @display("p=60,105;is=s");
        }
        scheduleSwap: ScheduleSwap {
            @display("p=182,105;i=block/switch;is=s");
        }
        interfaceTable: InterfaceTable {
            @display("p=60,31;is=s");
        }
        up: MessageDispatcher {
            parameters:
                @display("p=800,160;b=1200,5");
        }
        down: MessageDispatcher {
            parameters:
                @display("p=800,380;b=1200,5");
        }
    connections:
        for i=0..sizeof(ethg)-1 {
            eth[i].phys <--> ethg[i];
            eth[i].upperLayerOut --> down.in++;
            down.out++ --> eth[i].upperLayerIn;
        }
        down.out++ --> relayUnit.ifIn;
        relayUnit.ifOut --> processingDelay.in;
        processingDelay.out --> down.in++;
        relayUnit.upperLayerOut --> up.in++;
        relayUnit.upperLayerIn <-- up.out++;
}

