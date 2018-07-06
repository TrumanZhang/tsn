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

package nesting.linklayer;

import inet.linklayer.ethernet.EtherEncap;
import nesting.linklayer.framePreemption.EtherMACFullDuplexPreemptable;
import nesting.linklayer.ethernet.VLANEncap;
import nesting.linklayer.framePreemption.FrameForward;

//
// This module bundles the lower layer of the ~EthernetSwitch module.
//
// @see ~EtherMACFullDuplex, ~EtherEncap, ~VLANEncap
//
module LowerLayer
{
    parameters:
        @display("i=block/rxtx;bgb=397,400");
    gates:
        inout phys;
        input upperLayerPIn;
        input upperLayerEIn;
        output upperLayerOut;
    types:
    submodules:
        etherEncapE: EtherEncap {
            useSNAP = false; // encapsulate all packets into Ethernet2 frames.
            @display("p=130,202");
        }
        vlanEncapE: VLANEncap {
            @display("p=130,128");
        }
        vlanEncapP: VLANEncap {
            @display("p=261,128");
        }
        etherEncapP: EtherEncap {
            useSNAP = false; // encapsulate all packets into Ethernet2 frames.
            @display("p=261,202");
        }
        mac: EtherMACFullDuplexPreemptable {
            promiscuous = true;
            @display("p=195,340");
        }
        frameForward: FrameForward {
            @display("p=195,43");
        }
    connections:
        upperLayerEIn --> vlanEncapE.upperLayerIn;
        upperLayerPIn --> vlanEncapP.upperLayerIn;

        frameForward.upperLayerOut --> upperLayerOut;
        vlanEncapE.upperLayerOut --> frameForward.lowerLayerInE;
        vlanEncapP.upperLayerOut --> frameForward.lowerLayerInP;

        vlanEncapE.lowerLayerOut --> etherEncapE.upperLayerIn;
        etherEncapE.upperLayerOut --> vlanEncapE.lowerLayerIn;

        vlanEncapP.lowerLayerOut --> etherEncapP.upperLayerIn;
        etherEncapP.upperLayerOut --> vlanEncapP.lowerLayerIn;

        mac.phys <--> phys;
        etherEncapE.lowerLayerOut --> mac.upperLayerIn;
        mac.upperLayerOut --> etherEncapE.lowerLayerIn;
        etherEncapP.lowerLayerOut --> mac.upperLayerPreemptableIn;
        mac.upperLayerPreemptableOut --> etherEncapP.lowerLayerIn;
}
