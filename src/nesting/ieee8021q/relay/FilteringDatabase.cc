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

#include "FilteringDatabase.h"

namespace nesting {

Define_Module(FilteringDatabase);

FilteringDatabase::FilteringDatabase() {
  this->agingActive = false;
  this->agingThreshold = 0;
}

FilteringDatabase::FilteringDatabase(bool agingActive, simtime_t agingThreshold) {
  this->agingActive = agingActive;
  this->agingThreshold = agingThreshold;
}

FilteringDatabase::~FilteringDatabase() {
}

void FilteringDatabase::clearAdminFdb() {
  adminFdb.clear();
}
void FilteringDatabase::initialize(int stage) {
  if (stage == INITSTAGE_LOCAL) {
    cXMLElement* fdb = par("database");
    cXMLElement* cycleXml = par("cycle");
    cycle = atoi(cycleXml->getFirstChildWithTag("cycle")->getNodeValue());
    loadDatabase(fdb, cycle);

    cModule* clockModule = getModuleByPath(par("clockModule"));
    clock = check_and_cast<IClock*>(clockModule);

//    WATCH_MAP(fdb)
  } else if (stage == INITSTAGE_LINK_LAYER) {
    clock->subscribeTick(this, 0);
  }
}

int FilteringDatabase::numInitStages() const {
  return INITSTAGE_LINK_LAYER + 1;
}

void FilteringDatabase::loadDatabase(cXMLElement* xml, int cycle) {
  newCycle = cycle;

  string switchName = this->getModuleByPath(par("switchModule"))->getFullName();
  cXMLElement* fdb;
  //TODO this bool can probably be refactored to a nullptr check
  bool databaseFound = false;
  //try to extract the part of the filteringDatabase xml belonging to this module
  for (cXMLElement* host : xml->getChildren()) {
    if (host->hasAttributes() && host->getAttribute("id") == switchName) {
      fdb = host;
      databaseFound = true;
      break;
    }
  }

  //only continue if a filtering database was found for this switch
  if (!databaseFound) {
    return;
  }

  // Get static rules from XML file
  cXMLElement* staticRules = fdb->getFirstChildWithTag("static");

  if (staticRules != nullptr) {
    clearAdminFdb();

    cXMLElement* forwardingXml = staticRules->getFirstChildWithTag("forward");
    if (forwardingXml != nullptr) {
      this->parseEntries(forwardingXml);
    }

    changeDatabase = true;
  }

}

void FilteringDatabase::parseEntries(cXMLElement* xml) {
  // If present get rules from XML file
  if (xml == nullptr) {
    throw new cRuntimeError("Illegal xml input");
  }
  // Rules for individual addresses
  cXMLElementList individualAddresses = xml->getChildrenByTagName("individualAddress");

  for (auto individualAddress : individualAddresses) {

    string macAddressStr = string(individualAddress->getAttribute("macAddress"));
    if (macAddressStr.empty()) {
      throw cRuntimeError("individualAddress tag in forwarding database XML must have an "
          "macAddress attribute");
    }

    if (!individualAddress->getAttribute("port")) {
      throw cRuntimeError("individualAddress tag in forwarding database XML must have an "
          "port attribute");
    }

    int port = atoi(individualAddress->getAttribute("port"));

    uint8_t vid = 0;
    if (individualAddress->getAttribute("vid"))
      vid = static_cast<uint8_t>(atoi(individualAddress->getAttribute("vid")));

    // Create and insert entry for different individual address types
    if (vid == 0) {
      MACAddress macAddress;
      if (!macAddress.tryParse(macAddressStr.c_str())) {
        throw new cRuntimeError("Cannot parse invalid MAC address.");
      }
      adminFdb.insert( { macAddress, pair<simtime_t, int>(0, port) });
    } else {
      // TODO
      throw cRuntimeError("Individual address rules with VIDs aren't supported yet");
    }
  }
}

void FilteringDatabase::tick(IClock *clock) {
  if (changeDatabase) {
    operFdb.swap(adminFdb);
    cycle = newCycle;
    clearAdminFdb();

    EV_INFO << getFullPath() << ": Loading filtering database at time " << clock->getTime().inUnit(SIMTIME_US) << endl;

    changeDatabase = false;
  }
  clock->subscribeTick(this, cycle);
}

void FilteringDatabase::handleMessage(cMessage *msg) {
  throw cRuntimeError("Must not receive messages.");
}

void FilteringDatabase::insert(MACAddress macAddress, simtime_t curTS, int port) {
  operFdb[macAddress] = std::pair<simtime_t, int>(curTS, port);
}

int FilteringDatabase::getPort(MACAddress macAddress, simtime_t curTS) {
  simtime_t ts;
  int port;

  auto it = operFdb.find(macAddress);

  //is element available?
  if (it != operFdb.end()) {
    ts = it->second.first;
    port = it->second.second;
    // static entries (ts == 0) do not age
    if (!agingActive || (ts == 0 || curTS - ts < agingThreshold)) {
      operFdb[macAddress] = std::pair<simtime_t, int>(curTS, port);
      return port;
    } else {
      operFdb.erase(macAddress);
    }
  }

  return -1;
}

} // namespace nesting
