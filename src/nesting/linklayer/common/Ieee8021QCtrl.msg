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

//
// The generated class by this message file is used to represent the data
// fields extracted from Ethernet1Q tags. After a packet is decapsulated by the
// VLAN sublayer of the switch, a control information object of this type is
// attached to it.
//

cplusplus {{
#include "inet/linklayer/common/Ieee802Ctrl.h"
}}
 
class noncobject inet::Ieee802Ctrl;
 
namespace nesting;
 
class Ieee8021QCtrl extends inet::Ieee802Ctrl {
    bool tagged @getter(isTagged);	// True if the frame is tagged, false otherwise
    int PCP;						// Priority Code point
    bool DEI;						// Drop Eligible Indicator
    int VID;						// VLAN Identifier
}