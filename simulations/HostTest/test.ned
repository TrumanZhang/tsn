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

package nesting.simulations.HostTest;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.StandardHost;
import ned.DatarateChannel;
import nesting.ieee8021q.clock.IClock;


network Test
{
    @display("bgb=501,311");
    types:
        channel C extends DatarateChannel
        {
            delay = 0.1us;
            datarate = 1Gbps;
        }
        module ScheduledTrafficHost extends StandardHost
        {
            parameters:
                @display("bgb=1255.2749,738.675");
                numApps = 1;
                app[0].typename = "UdpScheduledTrafficApp";
                app[0].clockModule = "^.clock";
                app[0].interfaceTableModule = "^.interfaceTable";
            submodules:
                clock: <"IdealClock"> like IClock {
                    @display("is=s;p=124.424995,647.32495");
                }
        }
    submodules:
        h1: ScheduledTrafficHost {
            @display("p=196,62");
        }
        h2: ScheduledTrafficHost {
            @display("p=411,62");
        }
        h3: ScheduledTrafficHost {
            @display("p=299,233");
        }
        s1: EtherSwitch {
            @display("p=299,132");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("is=s;p=60,31");
        }
    connections:
        h1.ethg++ <--> C <--> s1.ethg++;
        h2.ethg++ <--> C <-->  s1.ethg++;
        h3.ethg++ <--> C <-->  s1.ethg++;
}
