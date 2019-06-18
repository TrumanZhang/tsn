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

package nesting.simulations.s_10h_1496B_5ms_5burst_5msol_5msoh;

import nesting.node.ethernet.VlanEtherSwitchPreemptable;
import nesting.node.ethernet.VlanEtherHostSched;


network s_10h_1496B_5ms_5burst_5msol_5msoh
{
    submodules:
    
    
        s7: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=236,186");
            gates:
                ethg[3]; 
        }
        h10: VlanEtherHostSched{
            @display("p=299,900");
        }
    
        h6: VlanEtherHostSched{
            @display("p=438,-50");
        }
    
        s4: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=722,280");
            gates:
                ethg[3]; 
        }
        s6: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=428,88");
            gates:
                ethg[3]; 
        }
        h2: VlanEtherHostSched{
            @display("p=572,720");
        }
    
        s9: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=100,600");
            gates:
                ethg[3]; 
        }
        h7: VlanEtherHostSched{
            @display("p=159,76");
        }
    
        s1: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=426,560");
            gates:
                ethg[2]; 
        }
        s2: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=562,588");
            gates:
                ethg[3]; 
        }
        h3: VlanEtherHostSched{
            @display("p=579,474");
        }
    
        s3: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=663,447");
            gates:
                ethg[3]; 
        }
        h9: VlanEtherHostSched{
            @display("p=-11,674");
        }
    
        s8: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=114,377");
            gates:
                ethg[3]; 
        }
        s5: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=625,126");
            gates:
                ethg[3]; 
        }
        s10: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=185,777");
            gates:
                ethg[2]; 
        }
        h8: VlanEtherHostSched{
            @display("p=-17,346");
        }
    
        h4: VlanEtherHostSched{
            @display("p=854,297");
        }
    
        h1: VlanEtherHostSched{
            @display("p=323,487");
        }
    
        h5: VlanEtherHostSched{
            @display("p=742,48");
        }
    
    connections:
    
    
    
    s7.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s6.ethg$i[0];
    
    
    
    
    
    s7.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h7.ethg$i;
    
    
    
    
    
    s7.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s8.ethg$i[0];
    
    
    
    
    
    h10.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s10.ethg$i[0];
    
    
    
    
    
    h6.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s6.ethg$i[1];
    
    
    
    
    
    s4.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s3.ethg$i[0];
    
    
    
    
    
    s4.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h4.ethg$i;
    
    
    
    
    
    s4.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s5.ethg$i[0];
    
    
    
    
    
    s6.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s7.ethg$i[0];
    
    
    
    
    
    s6.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h6.ethg$i;
    
    
    
    
    
    s6.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s5.ethg$i[1];
    
    
    
    
    
    h2.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s2.ethg$i[1];
    
    
    
    
    
    s9.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h9.ethg$i;
    
    
    
    
    
    s9.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s8.ethg$i[1];
    
    
    
    
    
    s9.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s10.ethg$i[1];
    
    
    
    
    
    h7.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s7.ethg$i[1];
    
    
    
    
    
    s1.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s2.ethg$i[2];
    
    
    
    
    
    s1.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h1.ethg$i;
    
    
    
    
    
    s2.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s3.ethg$i[1];
    
    
    
    
    
    s2.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h2.ethg$i;
    
    
    
    
    
    s2.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s1.ethg$i[0];
    
    
    
    
    
    h3.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s3.ethg$i[2];
    
    
    
    
    
    s3.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s4.ethg$i[0];
    
    
    
    
    
    s3.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s2.ethg$i[0];
    
    
    
    
    
    s3.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h3.ethg$i;
    
    
    
    
    
    h9.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s9.ethg$i[0];
    
    
    
    
    
    s8.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s7.ethg$i[2];
    
    
    
    
    
    s8.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s9.ethg$i[1];
    
    
    
    
    
    s8.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h8.ethg$i;
    
    
    
    
    
    s5.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s4.ethg$i[2];
    
    
    
    
    
    s5.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s6.ethg$i[2];
    
    
    
    
    
    s5.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h5.ethg$i;
    
    
    
    
    
    s10.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h10.ethg$i;
    
    
    
    
    
    s10.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s9.ethg$i[2];
    
    
    
    
    
    h8.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s8.ethg$i[2];
    
    
    
    
    
    h4.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s4.ethg$i[1];
    
    
    
    
    
    h1.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s1.ethg$i[1];
    
    
    
    
    
    h5.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s5.ethg$i[2];
    
    
    
}