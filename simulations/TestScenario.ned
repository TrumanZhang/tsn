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

package nesting.simulations;

import ned.DatarateChannel;
import nesting.node.ethernet.VEtherSwitchPreemptable;
import nesting.node.ethernet.VEtherHostSched;
import nesting.node.ethernet.VEtherHostFullLoad;
import nesting.node.ethernet.VEtherHost;


network TestScenario
{
    types:
        channel C extends DatarateChannel
        {
            delay = 0.1us;
            datarate = 1Gbps;
        }
    submodules:
        switchA: VEtherSwitchPreemptable {
            parameters:
                @display("p=188,127");
            gates:
                ethg[4];
        }
        switchB: VEtherSwitchPreemptable {
            parameters:
                @display("p=327,127");
            gates:
                ethg[3];
        }
        workstation1: VEtherHostFullLoad {
            @display("p=79,62");
        }
        workstation2: VEtherHostFullLoad {
            @display("p=172,29");
        }
        backupServer: VEtherHost {
            @display("p=446,54");
        }
        robotController: VEtherHostSched {
            @display("p=79,202");
        }
        roboticArm: VEtherHost {
            @display("p=440,202");
        }
    connections:
        robotController.ethg <--> C <--> switchA.ethg[0];
        workstation1.ethg <--> C <--> switchA.ethg[1];
        workstation2.ethg <--> C <--> switchA.ethg[2];

        roboticArm.ethg <--> C <--> switchB.ethg[0];
        backupServer.ethg <--> C <--> switchB.ethg[1];

        switchA.ethg[3] <--> C <--> switchB.ethg[2];
}
