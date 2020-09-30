#include "Header.h"

int const NumServer = 5;                            // number of servers
double ArrivalRate = 800;                           // arrival rate in jobs/s
double ObservationTime = 5.0/1000;                   // observe the processing latency at every 5 ms
double ServiceRateVec[] = {[0 ... NumServer-1]=250};




