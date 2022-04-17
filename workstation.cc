#include <string.h>
#include <omnetpp.h>
#include <queue>
#include <random>
#include <algorithm>
#include "user.h"
#include "job.h"
#include "state.h"
#include "dispatch_m.h"
#include "generate_job.h"
#define totalUser 4
#define eachUserJob 10

using namespace omnetpp;

class Workstation : public cSimpleModule{
    private:
        //std::queue<std::vector<Job>> jobQueue;
        //std::queue<std::string> colorQueue;
        std::vector<User>& userVector = GenerateJob::getInstance().getAllUser();
        /*int PW = 1;
        int EW = 0;
        int SW = 0;
        int RB = 0;
        int RW = -1;*/
        int countUser = 0;
        //void generateColor();
        //void generateJob(User &user);
    protected:
    // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Workstation);

void Workstation::initialize(){
    // §ï¦¨singleton¼Ò¦¡

    Dispatch *msg = new Dispatch("hello");
    msg->setKind(WorkerState::SUBMIT_JOB);
    scheduleAt(0, msg);
}

void Workstation::handleMessage(cMessage *msg){
    if(msg->isSelfMessage()){
        //EV<<"Workstation got a message from itself: "<<simTime()<<"\n";
        int msgKind = msg->getKind();
        if(msgKind==WorkerState::SUBMIT_JOB){
            //EV<<"Workstation submit a job: "<<simTime()<<"\n";

            //struct Job job;
            struct User* user;

            if(countUser<totalUser*eachUserJob){
                user = &userVector[countUser%totalUser];
                user->totalJob = user->totalJob + 1;
            }
            countUser++;
        }
        if(countUser<totalUser*eachUserJob){
            msg->setSchedulingPriority(1);
            scheduleAt(simTime()+0.5, msg);
        }
        else{
            cancelAndDelete(msg);
        }
    }
}
