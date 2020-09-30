/*
 * Queue.cc
 *
 *  Created on: 27 Sep 2020
 *      Author: smondal
 */

#include "Header.h"
#include <math.h>

using namespace std;
using namespace omnetpp;

class QueueMMc : public cSimpleModule
{
    private:
    /* Queue structure:
     * Corresponding to server i there is a ServerQ[i] that can contain maximum one message.
     * Arrivals try to first fill up these buffers.
     * If all of them are full, then arrivals are stored in ExcessQVector.
     * Messages arriving at ServerQ immediately start service.
     * When a ServBuffer becomes empty, messages, if any, are imported from ExcessQVector.
     */
        vector<double> ServerQ;
        vector<double> ExcessQ;
        vector<int> IdleServerIndex; /* Indices of the servers that are idle. */
                                     /* ServerIndex is actually the index of the associated QGate */
                                     /* It is the index as seen from QueueNode */
                                     /* Not the actual index of the server */

        int SumQLength;
        double CumDelay;
        int NumObservInstances;
        int NumServedMsg;
        int StateOccurence;
        cOutVector ProcessingLatency;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        void finish() override;
};

Define_Module(QueueMMc);

void QueueMMc::initialize()
{
    StateOccurence = 0;
    SumQLength = 0;
    CumDelay = 0;
    NumObservInstances = 0;
    NumServedMsg = 0;

    for (int i=0; i< NumServer; i++)
    {
        ServerQ.push_back(0);          /* Initially all ServerQs contain no message */
        IdleServerIndex.push_back(i);  /* All servers are idle */
    }

    cMessage *Observation = new cMessage("Observation");
    scheduleAt(simTime()+ObservationTime, Observation);
}

void QueueMMc::handleMessage(cMessage *msg)
{
    if(strcmp(msg->getName(),"Job request")==0)
    {
        if(IdleServerIndex.size()==0)    /* No Server is idle */
        {
            ExcessQ.push_back(simTime().dbl()); /* Store in ExcessQ */
        }
        else
        {
            int ServerIndex = IdleServerIndex[0];           /* Choose an idle server */
            ServerQ[ServerIndex] = simTime().dbl();         /* Assign arrival to ServerQ */
            cMessage *Transmission = new cMessage("Transmission");    /* Start transmission */
            send(Transmission,"inSrv$o",ServerIndex);
            IdleServerIndex.erase(IdleServerIndex.begin()); /* Server becomes busy */
        }
        delete msg;
    }
    else if (strcmp(msg->getName(), "ServiceIsComplete")==0)
    {
        int ServerIndex = msg->getArrivalGate()->getIndex();
        NumServedMsg++;
        CumDelay+=simTime().dbl() - ServerQ[ServerIndex];

        if(ExcessQ.size()>0)
        {
            ServerQ[ServerIndex] = ExcessQ[0];              /* Import from ExcessQ */
            ExcessQ.erase(ExcessQ.begin());

            cMessage *Transmission = new cMessage("Transmission");    /* Start transmission */
            send(Transmission,"inSrv$o",ServerIndex);
        }
        else
        {
            ServerQ[ServerIndex] = 0;
            IdleServerIndex.push_back(ServerIndex); /* Server becomes idle */
        }
        delete msg;
    }
    else if (strcmp(msg->getName(), "Observation")==0)
    {
        scheduleAt(simTime()+ObservationTime, msg);
        NumObservInstances++;
        int CurrentQLength = ExcessQ.size() + NumServer - IdleServerIndex.size();
        SumQLength += CurrentQLength;
        StateOccurence += (CurrentQLength==0);
        // recording processing latency over each observation slot
        double latency = 1.0*CumDelay/NumServedMsg;
        ProcessingLatency.record(latency);
    }
}

void QueueMMc::finish()
{
    cout<<"Simulation Values: "<<endl;
    cout<<"Average Queue Length: "<< 1.0*SumQLength/NumObservInstances<<endl;
    cout<<"Average Delay: "<< 1.0*CumDelay/NumServedMsg<<endl;
    cout<<"State 0 probability: "<<1.0*StateOccurence/NumObservInstances<<endl;
    cout<<endl;
    cout<<"Theoretical Values: From Wikipedia."<<endl;
    cout<<"Applies when all service rates are equal."<<endl;

    double Rho = ArrivalRate/(NumServer*ServiceRateVec[0]);
    double cRho = NumServer*Rho;
    double TermA = 1;
    double Denom = 1;

    for (int k =1; k< NumServer; k++)
    {
        TermA *= cRho/k;
        Denom += TermA;
    }

    TermA *= (cRho/NumServer)/(1-Rho);

    double TermC = TermA/(Denom+TermA);
    double AverageQLength = (Rho/(1-Rho))*TermC + (NumServer*Rho);
    double Prob0 = 1/(Denom+TermA);
    double AverageDelay = TermC/(NumServer*ServiceRateVec[0]-ArrivalRate) + (1/ServiceRateVec[0]);

    cout<<"Average Queue Length: "<<AverageQLength<<endl;
    cout<<"Average Delay: "<<AverageDelay<<endl;
    cout<<"State 0 Probability: "<<Prob0<<endl;
}


