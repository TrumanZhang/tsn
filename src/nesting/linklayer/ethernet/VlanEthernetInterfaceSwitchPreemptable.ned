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

package nesting.linklayer.ethernet;

import inet.linklayer.contract.IEthernetInterface;
import inet.linklayer.ethernet.IEtherEncap;
import inet.linklayer.ieee8021q.IIeee8021qEncap;
import inet.networklayer.common.InterfaceEntry;
import nesting.ieee8021q.queue.Queuing;
import nesting.linklayer.framePreemption.EtherMACFullDuplexPreemptable;
import nesting.linklayer.framePreemption.FrameForward;

@namespace();

//
// This module implements an ethernet interface that supports frame preemption.
//
module VlanEthernetInterfaceSwitchPreemptable extends InterfaceEntry like IEthernetInterface
{
    parameters:
        @display("i=block/ifcard;bgl=2");
        @class(inet::InterfaceEntry);
        string address = default("auto"); // MAC address
        string fcsMode @enum("declared","computed") = default("declared");
        *.fcsMode = fcsMode;
        bool framePreemptionDisabled = default(true);
        string interfaceTableModule;
    gates:
        input upperLayerIn;
        output upperLayerOut;
        inout phys @labels(EtherFrame);
    submodules:
        // mac supporting frame preemption
        mac: EtherMACFullDuplexPreemptable {
            @display("p=139,478");
        }
        queuing: Queuing {
            parameters:
                @display("p=202,61;q=l2queue");
        }
        encapE: <default("EtherEncap")> like IEtherEncap {
            parameters:
                @display("p=90,202");
        }
        encapP: <default("EtherEncap")> like IEtherEncap {
            parameters:
                @display("p=202,202");
        }
        qEncapE: <default("Ieee8021qEncap")> like IIeee8021qEncap {
            parameters:
                @display("p=90,333");
        }
        qEncapP: <default("Ieee8021qEncap")> like IIeee8021qEncap {
            @display("p=202,333");
        }
        frameForward: FrameForward {
            @display("p=90,61");
        }
    connections:
        upperLayerIn --> queuing.in;
        frameForward.upperLayerOut --> upperLayerOut;

        encapE.upperLayerOut --> frameForward.lowerLayerInE;
        encapP.upperLayerOut --> frameForward.lowerLayerInP;

        queuing.eOut --> encapE.upperLayerIn;
        queuing.pOut --> encapP.upperLayerIn;

        qEncapE.lowerLayerOut --> mac.upperLayerIn;
        mac.upperLayerOut --> qEncapE.lowerLayerIn;

        qEncapP.lowerLayerOut --> mac.upperLayerPreemptableIn;
        mac.upperLayerPreemptableOut --> qEncapP.lowerLayerIn;

        encapE.lowerLayerOut --> qEncapE.upperLayerIn;
        qEncapE.upperLayerOut --> encapE.lowerLayerIn;

        encapP.lowerLayerOut --> qEncapP.upperLayerIn;
        qEncapP.upperLayerOut --> encapP.lowerLayerIn;

        mac.phys <--> { @display("m=s"); } <--> phys;
}
