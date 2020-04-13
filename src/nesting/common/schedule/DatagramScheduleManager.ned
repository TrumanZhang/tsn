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

package nesting.common.schedule;

simple DatagramScheduleManager
{
    parameters:
        @display("i=block/table2");
        string clockModule; // Path to clock module implementing IClock2 interface
        bool enabled = default(true); // If enabled is true, then sent datagram events will be scheduled.
        xml initialAdminSchedule = default(xml("<schedule CycleTime=\"1h\"></schedule>")); // schedule that will be loaded initially if enabled is true.
}
