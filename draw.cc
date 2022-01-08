#include <string.h>
#include <omnetpp.h>
#include <math.h>
#include <vector>
#include <numeric>
#include "user.h"
#include "job.h"
#include "generate_job.h"

using namespace omnetpp;

class Draw : public cSimpleModule{
    protected:
    // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Draw);

void Draw::initialize(){}

void Draw::handleMessage(cMessage *msg){}
