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

package nesting.common.time;

//
// @deprecated
//
// This ideal clock implementation provides the ability to subscribe to clock
// ticks and has no clock drift or any failure.
//
// This module is deprecated. Use instances of the ~IClock2 or ~IOscillator
// modules instead.
//
// @see ~IClock
//
simple LegacyClock like IClock {
    parameters:
        @display("i=block/timer");
        @class(LegacyClock);
        string oscillatorModule;
}
