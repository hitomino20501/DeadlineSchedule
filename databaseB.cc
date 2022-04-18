#include <string.h>
#include <omnetpp.h>
#include <queue>
#include <vector>
#include <sstream>
#include <numeric>
#include "user.h"
#include "job.h"
#include "state.h"
#include "dispatch_m.h"
#include "generate_jobB.h"
#define totalUser 4
#define eachUserJob 10

using namespace omnetpp;

class DatabaseB : public cSimpleModule{
    private:
        int logFlag = 0;
        std::vector<std::vector<Job>>& jobVector = GenerateJobB::getInstance().getAllJob();
        std::vector<User>& userVector = GenerateJobB::getInstance().getAllUser();
    protected:
    // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(DatabaseB);

void DatabaseB::initialize(){
    Dispatch *msg = new Dispatch("log");
    msg->setKind(WorkerState::LOG_TIMER);
    msg->setSchedulingPriority(10);
    scheduleAt(0.0, msg);
}

void DatabaseB::handleMessage(cMessage *msg){
    int msgKind = msg->getKind();
    if(msg->isSelfMessage()){
        if(msgKind==WorkerState::LOG_TIMER){
            //EV<<"just print\n";
            int renderingFrameZero = 0;
            int index = 0;
            for (auto it = userVector.begin(); it != userVector.end(); ++it){
                /*if(index==limitSearchUser){
                    break;
                }*/
                EV<<"{";
                EV<<"'simTimeB':"<<simTime()<<",";
                EV<<"'userNameB':'"<<(*it).name<<"',";
                EV<<"'renderingFrameB':"<<(*it).userRenderingFrame<<",";
                EV<<"'weightB':"<<(*it).userWeight<<",";
                EV<<"'totalB':"<<(*it).userFinishFrame;
                EV<<"}\n";
                if((*it).userRenderingFrame==0){
                    renderingFrameZero++;
                }
                index++;
            }
            if(renderingFrameZero==userVector.size()){
                logFlag++;
            }
            if(logFlag<4){
                scheduleAt(simTime()+5.0, msg);
            }else{
                cancelAndDelete(msg);
            }

        }
    }
}

