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
import nesting.application.ethernet.EtherVLANTaggedTrafGen;
import nesting.linklayer.ethernet.VLANEncap;


//
// Simple Host with the capability to create and receive VLAN tagged ethernet
// traffic. Mainly used for test purposes.
//
// @see ~EtherVLANTaggedTrafGen
//
module VLANHost extends VLANHostBase
{
    parameters:
        @display("bgb=483,473");
        @networkNode();
    submodules:
        app: EtherVLANTaggedTrafGen {
            @display("p=171,415");
        }
    connections:
        app.out --> vlanEncap.upperLayerIn;
        vlanEncap.upperLayerOut --> app.in;
}
