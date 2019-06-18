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

package nesting.simulations.s_10h_1000B_5ms_5burst_5msol_5msoh;

import nesting.node.ethernet.VlanEtherSwitchPreemptable;
import nesting.node.ethernet.VlanEtherHostSched;


network s_10h_1000B_5ms_5burst_5msol_5msoh
{
    submodules:
    
    
        s5: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=252,533");
            gates:
                ethg[3]; 
        }
        h9: VlanEtherHostSched{
            @display("p=644,122");
        }
    
        h7: VlanEtherHostSched{
            @display("p=201,183");
        }
    
        s4: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=387,593");
            gates:
                ethg[3]; 
        }
        s2: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=387,655");
            gates:
                ethg[3]; 
        }
        s8: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=409,135");
            gates:
                ethg[3]; 
        }
        s9: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=559,91");
            gates:
                ethg[3]; 
        }
        s1: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=388,796");
            gates:
                ethg[2]; 
        }
        s10: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=667,22");
            gates:
                ethg[2]; 
        }
        h8: VlanEtherHostSched{
            @display("p=385,39");
        }
    
        h4: VlanEtherHostSched{
            @display("p=482,665");
        }
    
        h1: VlanEtherHostSched{
            @display("p=389,900");
        }
    
        h3: VlanEtherHostSched{
            @display("p=515,473");
        }
    
        h5: VlanEtherHostSched{
            @display("p=172,611");
        }
    
        h2: VlanEtherHostSched{
            @display("p=341,570");
        }
    
        h10: VlanEtherHostSched{
            @display("p=750,-33");
        }
    
        s7: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=281,241");
            gates:
                ethg[3]; 
        }
        s3: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=462,569");
            gates:
                ethg[3]; 
        }
        h6: VlanEtherHostSched{
            @display("p=113,443");
        }
    
        s6: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=215,393");
            gates:
                ethg[3]; 
        }
    connections:
    
    
    
    s5.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s6.ethg$i[1];
    
    
    
    
    
    s5.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h5.ethg$i;
    
    
    
    
    
    s5.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s4.ethg$i[0];
    
    
    
    
    
    h9.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s9.ethg$i[1];
    
    
    
    
    
    h7.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s7.ethg$i[1];
    
    
    
    
    
    s4.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s5.ethg$i[2];
    
    
    
    
    
    s4.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h4.ethg$i;
    
    
    
    
    
    s4.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s3.ethg$i[2];
    
    
    
    
    
    s2.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s3.ethg$i[0];
    
    
    
    
    
    s2.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h2.ethg$i;
    
    
    
    
    
    s2.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s1.ethg$i[0];
    
    
    
    
    
    s8.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h8.ethg$i;
    
    
    
    
    
    s8.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s7.ethg$i[2];
    
    
    
    
    
    s8.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s9.ethg$i[0];
    
    
    
    
    
    s9.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s8.ethg$i[2];
    
    
    
    
    
    s9.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h9.ethg$i;
    
    
    
    
    
    s9.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s10.ethg$i[0];
    
    
    
    
    
    s1.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s2.ethg$i[2];
    
    
    
    
    
    s1.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h1.ethg$i;
    
    
    
    
    
    s10.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s9.ethg$i[2];
    
    
    
    
    
    s10.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h10.ethg$i;
    
    
    
    
    
    h8.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s8.ethg$i[0];
    
    
    
    
    
    h4.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s4.ethg$i[1];
    
    
    
    
    
    h1.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s1.ethg$i[1];
    
    
    
    
    
    h3.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s3.ethg$i[1];
    
    
    
    
    
    h5.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s5.ethg$i[1];
    
    
    
    
    
    h2.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s2.ethg$i[1];
    
    
    
    
    
    h10.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s10.ethg$i[1];
    
    
    
    
    
    s7.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s6.ethg$i[0];
    
    
    
    
    
    s7.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h7.ethg$i;
    
    
    
    
    
    s7.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s8.ethg$i[1];
    
    
    
    
    
    s3.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s2.ethg$i[0];
    
    
    
    
    
    s3.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h3.ethg$i;
    
    
    
    
    
    s3.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s4.ethg$i[2];
    
    
    
    
    
    h6.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s6.ethg$i[2];
    
    
    
    
    
    s6.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s7.ethg$i[0];
    
    
    
    
    
    s6.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s5.ethg$i[0];
    
    
    
    
    
    s6.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h6.ethg$i;
    
    
    
}