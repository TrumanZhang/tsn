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

@namespace();

import nesting.ieee8021q.queue.Queuing;
import nesting.linklayer.ethernet.VLANEncap;
import nesting.linklayer.framePreemption.FrameForward;
import nesting.node.ethernet.SinkInOut;
import nesting.linklayer.framePreemption.EtherMACFullDuplexPreemptable;
import inet.linklayer.contract.IEtherMac;
import inet.linklayer.contract.IEthernetInterface;
import inet.linklayer.ethernet.IEtherEncap;
//import inet.common.queue.Sink;


module VLANEthernetInterfaceSwitch like IEthernetInterface
{
    parameters:
        @class(inet::InterfaceEntry);
        string interfaceTableModule;
        string fcsMode @enum("declared","computed") = default("declared");
        @display("i=block/ifcard;bgl=2");
        *.interfaceTableModule = default(absPath(interfaceTableModule));
        *.fcsMode = fcsMode;
    gates:
        input upperLayerIn;
        output upperLayerOut;
        inout phys @labels(EtherFrame);
    submodules:
        mac: EtherMACFullDuplexPreemptable {
            @display("p=90,390");
        }
        queue: Queuing {
            parameters:
                @display("p=202,61;q=l2queue");
        }
        vlanEncapE: VLANEncap {
            @display("p=90,221");
        }
        etherEncapE: <default("EtherEncap")> like IEtherEncap {
            parameters:
                @display("p=90,301");
        }
        vlanEncapP: VLANEncap {
            @display("p=202,221");
        }
        etherEncapP: <default("EtherEncap")> like IEtherEncap {
            parameters:
                @display("p=202,301");
        }
        frameForward: FrameForward {
            @display("p=90,61");
        }
        sink: SinkInOut {
            @display("p=202,390");
        }
    connections:
        upperLayerIn --> queue.in;
        frameForward.upperLayerOut --> upperLayerOut;

        vlanEncapE.upperLayerOut --> frameForward.lowerLayerInE;
        vlanEncapP.upperLayerOut --> frameForward.lowerLayerInP;

        queue.eOut --> vlanEncapE.upperLayerIn;
        queue.pOut --> vlanEncapP.upperLayerIn;

        etherEncapE.lowerLayerOut --> mac.upperLayerIn;
        mac.upperLayerOut --> etherEncapE.lowerLayerIn;

        etherEncapP.lowerLayerOut --> sink.in++;
        sink.out++ --> etherEncapP.lowerLayerIn;

        sink.out++ --> mac.upperLayerPreemptableIn;
        mac.upperLayerPreemptableOut --> sink.in++;

        vlanEncapE.lowerLayerOut --> etherEncapE.upperLayerIn;
        etherEncapE.upperLayerOut --> vlanEncapE.lowerLayerIn;

        vlanEncapP.lowerLayerOut --> etherEncapP.upperLayerIn;
        etherEncapP.upperLayerOut --> vlanEncapP.lowerLayerIn;

        mac.phys <--> { @display("m=s"); } <--> phys;
}
