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
    parameters:
        @display("bgb=509,232");
        int numHosts = default(3);
    types:
        channel C extends DatarateChannel
        {
            delay = 0.1us;
            datarate = 1Gbps;
        }
        module ScheduledTrafficHost extends StandardHost
        {
            parameters:
                numApps = 1;
                app[0].typename = "UdpScheduledTrafficApp";
                app[0].clockModule = "^.clock";
                app[0].interfaceTableModule = "^.interfaceTable";
            submodules:
                clock: <"IdealClock"> like IClock {
                    @display("is=s;p=452.4,75.4");
                }
        }
    submodules:
        host[numHosts]: ScheduledTrafficHost {
            @display("p=209,146,r,100");
        }
        s1: EtherSwitch {
            @display("p=209,46");
            numEthInterfaces = numHosts;
        }
        configurator: Ipv4NetworkConfigurator {
            @display("is=s;p=60,31");
        }
    connections:
        for i=0..numHosts-1 {
            host[i].ethg++ <--> C <--> s1.ethg[i];
        }
}
