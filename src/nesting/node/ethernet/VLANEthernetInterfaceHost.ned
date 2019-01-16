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

@namespace();

import inet.linklayer.ethernet.EtherMacFullDuplex;
import inet.linklayer.ethernet.EtherEncap;
import inet.linklayer.contract.IEthernetInterface;

//
// Ethernet network interface, used in Host modules in nesting project.
//
module VLANEthernetInterfaceHost like IEthernetInterface
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
        mac: EtherMacFullDuplex {
            @display("p=141,193");
        }
        encap: EtherEncap {
            parameters:
                @display("p=141,77");
        }
    connections:
        mac.phys <--> { @display("m=s"); } <--> phys;

        mac.upperLayerOut --> encap.lowerLayerIn;
        encap.lowerLayerOut --> mac.upperLayerIn;

        encap.upperLayerOut --> { @display("m=n"); } --> upperLayerOut;
        upperLayerIn --> { @display("m=n"); } --> encap.upperLayerIn;

}

