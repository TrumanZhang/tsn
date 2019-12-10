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

#ifndef __MAIN_FILTERINGDATABASE_H_
#define __MAIN_FILTERINGDATABASE_H_

#include <omnetpp.h>
#include <tuple>
#include <unordered_map>

#include "inet/linklayer/common/MacAddress.h"
#include "inet/networklayer/contract/IInterfaceTable.h"

#include "nesting/common/time/IClockListener.h"

using namespace omnetpp;
using namespace inet;

//added hash function for MacAddress (required for map)
namespace std {
template<> struct hash<MacAddress> {
    size_t operator()(MacAddress const& mac) const noexcept
    {
        return std::hash<string> { }(mac.str());
    }
};
}

namespace nesting {

/**
 * See the NED file for a detailed description
 */
class FilteringDatabase: public cSimpleModule, public IClockListener {
private:
    std::unordered_map<MacAddress, std::pair<simtime_t, std::vector<int>>> adminFdb;
    std::unordered_map<MacAddress, std::pair<simtime_t, std::vector<int>>> operFdb;

    bool changeDatabase = false;

    /**
     * Clock reference, needed to subscribe notifications of cycle iterations
     */
    IClock* clock;

    simtime_t cycle = simTime().parse("100us");
    simtime_t newCycle = simTime().parse("100us");;

    bool agingActive = false;
    simtime_t agingThreshold;

    IInterfaceTable* ifTable;

protected:
    virtual void initialize(int stage) override;

    virtual void handleMessage(cMessage* msg);

    virtual int numInitStages() const override;

    void parseEntries(cXMLElement* xml);

    void clearAdminFdb();

public:
    FilteringDatabase(bool agingActive, simtime_t agingTreshold);
    FilteringDatabase();
    virtual ~FilteringDatabase();

    /** @see IClockListener::tick(IClock*) */
    virtual void tick(IClock *clock, short kind) override;

    virtual void loadDatabase(cXMLElement* fdb, simtime_t cycle);

    virtual int getDestInterfaceId(MacAddress macAddress, simtime_t curTS);

    virtual std::vector<int> getDestInterfaceIds(MacAddress macAddress, simtime_t curTS);

    void insert(MacAddress macAddress, simtime_t curTS, int interfaceId);
};

} // namespace nesting

#endif
