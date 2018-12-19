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

package nesting.linklayer.framePreemption;

//
// This module implements the MAC Merge sublayer as specified by IEEE Standard
// for Ethernet 802.3 (p. 237). It supports interspersing express traffic with 
// preemptible traffic.
// 
// Expected Environment:
//  - eUpperPhys and pUpperPhys should be connected to an eMac and pMax respectively.
//
// Functionality:
//  - t.b.d.
//

simple EtherMacMerge
{
    parameters:
        @signal[preemptCurrentFrameSignal](type=inet::Packet);
        @signal[transmittedExpressFrameSignal](type=inet::Packet);
        @signal[transmittedPreemptableFrameSignal](type=inet::Packet);
        @signal[transmittedPreemptableFramePartSignal](type=inet::Packet);
        @signal[transmittedPreemptableNonFinalSignal](type=inet::Packet);
        @signal[transmittedPreemptableFinalSignal](type=inet::Packet);
        @signal[transmittedPreemptableFullSignal](type=inet::Packet);
        @signal[expressFrameEnqueuedWhileSendingPreemptableSignal];

        @statistic[preemptions](title="preemptions"; source=preemptCurrentFrameSignal; record=count; interpolationmode=none);
        @statistic[expressFrames](title="expressFrames"; source=transmittedExpressFrameSignal; record=count; interpolationmode=none);
        @statistic[preemptableFrames](title="preemptableFrames"; source=transmittedPreemptableFrameSignal; record=count; interpolationmode=none);
        @statistic[mPackets](title="mPackets"; source=transmittedPreemptableFramePartSignal; record=count; interpolationmode=none);
        @statistic[mPacketsNonFinal](title="mPackets non-final"; source=transmittedPreemptableNonFinalSignal; record=count; interpolationmode=none);
        @statistic[mPacketsFinal](title="mPackets final"; source=transmittedPreemptableFinalSignal; record=count; interpolationmode=none);
        @statistic[mPacketsFull](title="mpackets full"; source=transmittedPreemptableFullSignal; record=count; interpolationmode=none);
        @statistic[expressFramesEnqueued](title="express frames enqueued"; source=expressFrameEnqueuedWhileSendingPreemptableSignal);
    gates:
        input eUpperLayerIn; // from Queueing
        output eLowerLayerOut; // to eMac
        input pUpperLayerIn; // from Queueing
        output pLowerLayerout; // to pMac
}
