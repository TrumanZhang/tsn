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

package nesting.node.ethernet.host;

import inet.linklayer.ethernet.EtherMACFullDuplex;
import inet.linklayer.ethernet.IEtherEncap;

//
// Base module for the further implementation of ethernet capable hosts.
//
module EtherHostBase {
    parameters:
        *.interfaceTableModule = default("");
        @networkNode();
        @labels(node,ethernet-node);
        @display("i=device/pc2");
    gates:
        inout ethg @labels(EtherFrame-conn);
    submodules:
        etherEncap: <default("EtherEncap")> like IEtherEncap {
            @display("p=171,147");
        }
        mac: EtherMACFullDuplex {
            @display("p=171,58");
        }
    connections:
        mac.phys <--> ethg;
        mac.upperLayerOut --> etherEncap.lowerLayerIn;
        mac.upperLayerIn <-- etherEncap.lowerLayerOut;
}
