/*
 * server.cc
 *
 *  Created on: 27 Sep 2020
 *      Author: smondal
 */

#include "Header.h"

using namespace std;
using namespace omnetpp;

class ServerMMc : public cSimpleModule
{
    private:
        double ServiceRate;
        bool IsBusy;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(ServerMMc);

void ServerMMc::initialize()
{
    IsBusy = 0; /* Initially the server is idle */
    ServiceRate = ServiceRateVec[getIndex()]; /* getIndex() == Index of the server */
}

void ServerMMc::handleMessage(cMessage *msg)
{
    if(IsBusy == 0)  /* Service request arrives from queue */
    {
        IsBusy = 1;
        scheduleAt(simTime()+exponential(1.0/ServiceRate), msg);
    }
    else
    {
        IsBusy = 0; /*Service completes*/
        delete msg;

        cMessage *ServACK = new cMessage("ServiceIsComplete");
        send(ServACK,"inQue$o");
    }

}


