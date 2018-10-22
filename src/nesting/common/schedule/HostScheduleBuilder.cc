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

#include "HostScheduleBuilder.h"

namespace nesting {

HostSchedule<Ieee8021QCtrl>* HostScheduleBuilder::createHostScheduleFromXML(
        cXMLElement *xml, cXMLElement *rootXml) {
    HostSchedule<Ieee8021QCtrl>* schedule = new HostSchedule<Ieee8021QCtrl>();

    //extract cycle from second xml argument (xml root)
    int cycle = atoi(rootXml->getFirstChildWithTag("cycle")->getNodeValue());
    schedule->setCycle(cycle);

    vector<cXMLElement*> entries = xml->getChildrenByTagName("entry");
    for (cXMLElement* entry : entries) {
        // Get time
        const char* timeCString =
                entry->getFirstChildWithTag("start")->getNodeValue();
        unsigned int time = atoi(timeCString);

        // Get size
        const char* sizeCString =
                entry->getFirstChildWithTag("size")->getNodeValue();
        unsigned int size = atoi(sizeCString);

        // Get Ieee8021QCtrl

        Ieee8021QCtrl etherctrl = Ieee8021QCtrl();
        const char* queueCString =
                entry->getFirstChildWithTag("queue")->getNodeValue();
        etherctrl.setPCP(atoi(queueCString));

        const char* addressCString =
                entry->getFirstChildWithTag("dest")->getNodeValue();

        inet::MACAddress destination = inet::MACAddress(addressCString);
        etherctrl.setDestinationAddress(destination);

        etherctrl.setEtherType(0x8100);
        etherctrl.setTagged(true);
        etherctrl.setVID(0);
        etherctrl.setDEI(false);

        schedule->addEntry(time, size, etherctrl);
    }

    return schedule;
}

} // namespace nesting

