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

package nesting.simulations.s_10h_300B_10ms_20burst_5msol_5msoh;

import nesting.node.ethernet.VlanEtherSwitchPreemptable;
import nesting.node.ethernet.VlanEtherHostSched;


network s_10h_300B_10ms_20burst_5msol_5msoh
{
    submodules:
    
    
        s3: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=462,100");
            gates:
                ethg[3]; 
        }
        s9: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=490,743");
            gates:
                ethg[3]; 
        }
        h9: VlanEtherHostSched{
            @display("p=597,703");
        }
    
        h2: VlanEtherHostSched{
            @display("p=613,-85");
        }
    
        s7: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=231,616");
            gates:
                ethg[3]; 
        }
        h5: VlanEtherHostSched{
            @display("p=149,350");
        }
    
        s8: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=361,691");
            gates:
                ethg[3]; 
        }
        s1: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=725,-1");
            gates:
                ethg[2]; 
        }
        h8: VlanEtherHostSched{
            @display("p=411,744");
        }
    
        h3: VlanEtherHostSched{
            @display("p=511,165");
        }
    
        s5: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=227,321");
            gates:
                ethg[3]; 
        }
        h7: VlanEtherHostSched{
            @display("p=149,687");
        }
    
        s4: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=318,179");
            gates:
                ethg[3]; 
        }
        h1: VlanEtherHostSched{
            @display("p=824,-29");
        }
    
        h10: VlanEtherHostSched{
            @display("p=383,900");
        }
    
        s10: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=452,839");
            gates:
                ethg[2]; 
        }
        h4: VlanEtherHostSched{
            @display("p=255,96");
        }
    
        s6: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=178,474");
            gates:
                ethg[3]; 
        }
        h6: VlanEtherHostSched{
            @display("p=68,488");
        }
    
        s2: VlanEtherSwitchPreemptable{
            parameters:
                @display("p=595,17");
            gates:
                ethg[3]; 
        }
    connections:
    
    
    
    s3.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h3.ethg$i;
    
    
    
    
    
    s3.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s4.ethg$i[0];
    
    
    
    
    
    s3.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s2.ethg$i[0];
    
    
    
    
    
    s9.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s8.ethg$i[0];
    
    
    
    
    
    s9.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s10.ethg$i[0];
    
    
    
    
    
    s9.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h9.ethg$i;
    
    
    
    
    
    h9.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s9.ethg$i[2];
    
    
    
    
    
    h2.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s2.ethg$i[2];
    
    
    
    
    
    s7.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s8.ethg$i[2];
    
    
    
    
    
    s7.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s6.ethg$i[2];
    
    
    
    
    
    s7.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h7.ethg$i;
    
    
    
    
    
    h5.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s5.ethg$i[0];
    
    
    
    
    
    s8.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s9.ethg$i[0];
    
    
    
    
    
    s8.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h8.ethg$i;
    
    
    
    
    
    s8.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s7.ethg$i[0];
    
    
    
    
    
    s1.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h1.ethg$i;
    
    
    
    
    
    s1.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s2.ethg$i[1];
    
    
    
    
    
    h8.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s8.ethg$i[1];
    
    
    
    
    
    h3.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s3.ethg$i[0];
    
    
    
    
    
    s5.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h5.ethg$i;
    
    
    
    
    
    s5.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s6.ethg$i[0];
    
    
    
    
    
    s5.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s4.ethg$i[2];
    
    
    
    
    
    h7.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s7.ethg$i[2];
    
    
    
    
    
    s4.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s3.ethg$i[1];
    
    
    
    
    
    s4.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h4.ethg$i;
    
    
    
    
    
    s4.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s5.ethg$i[2];
    
    
    
    
    
    h1.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s1.ethg$i[0];
    
    
    
    
    
    h10.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s10.ethg$i[1];
    
    
    
    
    
    s10.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s9.ethg$i[1];
    
    
    
    
    
    s10.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h10.ethg$i;
    
    
    
    
    
    h4.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s4.ethg$i[1];
    
    
    
    
    
    s6.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s5.ethg$i[1];
    
    
    
    
    
    s6.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h6.ethg$i;
    
    
    
    
    
    s6.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s7.ethg$i[1];
    
    
    
    
    
    h6.ethg$o --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s6.ethg$i[1];
    
    
    
    
    
    s2.ethg$o[0] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s3.ethg$i[2];
    
    
    
    
    
    s2.ethg$o[1] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> s1.ethg$i[1];
    
    
    
    
    
    s2.ethg$o[2] --> {delay = 1e-06s; datarate = 1000000000.0bps;} --> h2.ethg$i;
    
    
    
}