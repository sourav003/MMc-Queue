/*
 * source.cc
 *
 *  Created on: 27 Sep 2020
 *      Author: smondal
 */

#include "Header.h"
#include <math.h>

using namespace std;
using namespace omnetpp;

class SourceMMc : public cSimpleModule
{
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(SourceMMc);

void SourceMMc::initialize()
{
    cMessage *Arrival = new cMessage("Arrival");
    scheduleAt(simTime(), Arrival);
}

void SourceMMc::handleMessage(cMessage *msg)
{
    if(strcmp(msg->getName(),"Arrival")==0)
    {
        // Generate and send job request to the Queue
        cMessage *Job = new cMessage("Job request");
        send(Job,"out");

        // schedule next call
        scheduleAt(simTime()+exponential(1.0/ArrivalRate), msg);
    }
}
