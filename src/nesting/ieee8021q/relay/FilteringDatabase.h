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

#include "inet/linklayer/common/MACAddress.h"
#include "../../linklayer/common/Ieee8021QCtrl_m.h"
#include "../clock/IClockListener.h"

using namespace omnetpp;
using namespace inet;
using namespace std;

//added hash function for MACAddress (required for map)
namespace std
{
    template<> struct hash<MACAddress>
    {
        size_t operator()(MACAddress const& mac) const noexcept
        {
            return std::hash<string>{}(mac.str());
        }
    };
}

namespace nesting {

/**
 * See the NED file for a detailed description
 */
class FilteringDatabase : public cSimpleModule, public IClockListener {
private:
  unordered_map<MACAddress, pair<simtime_t, int>> adminFdb;
  unordered_map<MACAddress, pair<simtime_t, int>> operFdb;

  bool changeDatabase = false;

  /**
   * Clock reference, needed to subscribe notifications of cycle iterations
   */
  IClock* clock;

  int cycle = 100;
  int newCycle = 100;

  bool agingActive = false;
  simtime_t agingThreshold;

  void parseEntries(cXMLElement* xml);
  void clearAdminFdb();

protected:
  virtual void initialize(int stage) override;

  virtual void handleMessage(cMessage* msg);

  virtual int numInitStages() const override;

public:
  FilteringDatabase(bool agingActive, simtime_t agingTreshold);
  FilteringDatabase();
  virtual ~FilteringDatabase();

  /** @see IClockListener::tick(IClock*) */
  virtual void tick(IClock *clock) override;

  virtual void loadDatabase(cXMLElement* fdb, int cycle);

  virtual int getPort(MACAddress macAddress, simtime_t curTS);

  void insert(MACAddress macAddress, simtime_t curTS, int port);
};

} // namespace nesting

#endif
