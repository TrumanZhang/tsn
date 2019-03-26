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

import inet.common.queue.Sink;
import nesting.common.queue.EtherTrafGenQueue;
import nesting.linklayer.ethernet.VLANEncap;

//
// VLAN capable host module, that sends packets with the maximum throughput the
// physical medium allows. This module's main use case is to simulate a
// congestion scenarios on a switch.
//
module VEtherHostFullLoad extends VEtherHostBase
{
    parameters:
        @display("bgb=363,398");
        @networkNode();
        eth.mac.queueModule = "^.^.trafGenQueue";
    submodules:
        trafGenQueue: EtherTrafGenQueue {
            @display("p=192,69");
        }
        sink: Sink {
            @display("p=84,69");
        }
        vlanEncap: VLANEncap {
            @display("p=142,188");
        }
    connections:
        trafGenQueue.out --> vlanEncap.upperLayerIn;
        vlanEncap.upperLayerOut --> sink.in++;
        
        vlanEncap.lowerLayerOut --> eth.upperLayerIn;
        eth.upperLayerOut --> vlanEncap.lowerLayerIn;
}
