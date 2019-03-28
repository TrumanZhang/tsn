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

package nesting.simulations.scratch00;

import inet.node.ethernet.EtherLink; // GEORG REFACTOR: this line does not work
import ned.DatarateChannel;
import nesting.node.ethernet.VlanEtherHost;
import nesting.node.ethernet.VlanEtherHostFullLoad;
import nesting.node.ethernet.VlanEtherSwitch;
import nesting.node.ethernet.VlanEtherSwitchPreemptable;

network Network_01
{
    parameters:

    types:
        channel C extends DatarateChannel
        {
            delay = 0.1us;
            datarate = 1Gbps;
        }
    submodules:
        switch: VlanEtherSwitch {
            parameters:
                @display("p=200,150");
            gates:
                ethg[3];
        }
        workstation1: VlanEtherHost {
            @display("p=100,200");
        }
        workstation2: VlanEtherHostFullLoad {
            @display("p=100,100");
        }
        backupServer: VlanEtherHost {
            @display("p=300,150");
        }
    connections:
        backupServer.ethg <--> C <--> switch.ethg[0];
        workstation1.ethg <--> C <--> switch.ethg[1];
        workstation2.ethg <--> C <--> switch.ethg[2];
}
