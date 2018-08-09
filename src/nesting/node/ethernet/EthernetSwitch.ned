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

import inet.common.queue.Delayer;
import inet.networklayer.common.InterfaceTable;
import nesting.ieee8021q.clock.IClock;
import nesting.ieee8021q.queue.Queuing;
import nesting.ieee8021q.queue.gating.ScheduleSwap;
import nesting.ieee8021q.relay.FilteringDatabase;
import nesting.ieee8021q.relay.RelayUnit;
import nesting.linklayer.LowerLayer;


//
// This module implements an ethernet switch conform to the Ieee802.1Qbv, 
// Ieee802.1Qbu and Ieee802.1Qav standard.
//
// @see ~Queuing, ~LowerLayer, ~RelayUnit, ~FrameAssembler, ~Delayer, ~IClock
// @see ~FilteringDatabase, ~ScheduleSwap
//
module EthernetSwitch
{
    parameters:
        @networkNode();
        @display("i=device/switch;bgb=,521");
        **.interfaceTableModule = default("");
    gates:
        inout ethg[];
    submodules:
        queuing[sizeof(ethg)]: Queuing {
            @display("p=195,370,r,200");
            tsAlgorithms[*].macModule = "^.^.lowerLayer[" + string(index) + "].mac";
            gateController.macModule = "^.^.lowerLayer[" + string(index) + "].mac";
        }
        lowerLayer[sizeof(ethg)]: LowerLayer {
            mac.queueModule = "^.^.queuing[" + string(index) + "].transmissionSelection";
            mac.mtu = 1500B;
            @display("p=140,440,r,200");
        }
        relayUnit: <default("ForwardingRelayUnit")> like RelayUnit {
            numberOfPorts = sizeof(ethg);
            @display("p=400,180");
        }
        processingDelay[sizeof(ethg)]: Delayer {
            @display("p=85,350,r,200;i=block/delay");
            delay = default(4us);
        }
        clock: <default("IdealClock")> like IClock {
            @display("p=557.496,63.011997;is=s");
        }
        filteringDatabase: FilteringDatabase {
            @display("p=557.496,128.16;is=s");
        }
        scheduleSwap: ScheduleSwap {
            @display("p=557.496,194.37599;i=block/switch;is=s");
        }
        interfaceTable: InterfaceTable {
            @display("p=73,65;is=s");
        }

    connections:
        for i=0..sizeof(ethg)-1 {
            lowerLayer[i].phys <--> ethg[i];
            lowerLayer[i].upperLayerOut --> processingDelay[i].in;
            processingDelay[i].out --> relayUnit.in[i];
            relayUnit.out[i] --> queuing[i].in;
            queuing[i].eOut --> lowerLayer[i].upperLayerEIn;
            queuing[i].pOut --> lowerLayer[i].upperLayerPIn;
        }
}
