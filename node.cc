#include <string.h>
#include <omnetpp.h>
#include "user.h"
#include "job.h"
#include "state.h"
#include "generate_job.h"

using namespace omnetpp;

class Node : public cSimpleModule{
    private:
        std::vector<std::vector<Job>>& jobVector = GenerateJob::getInstance().getAllJob();
        bool renderColor = false;
        bool finishColor = false;
    protected:
    // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void refreshDisplay() const override;
};

Define_Module(Node);

void Node::initialize(){
    /*cMessage *msg = new cMessage("drawNode");
    scheduleAt(3.0, msg);*/
}

void Node::handleMessage(cMessage *msg){
    if(msg->isSelfMessage()){
        int index = 0;
        for (auto it = jobVector[0].begin(); it != jobVector[0].end(); ++it){
            if((*it).isActivate){
                if(getIndex()==index){
                    renderColor = true;
                }
            }
            index++;
        }
        scheduleAt(simTime()+3.0, msg);
    }else{
        //EV<<"Node got a message\n";
        int msgKind = msg->getKind();
        if(msgKind==WorkerState::JOB_START){
            renderColor = true;
        }else if(msgKind==WorkerState::JOB_FINISH){
            renderColor = false;
            finishColor = true;
        }
        delete(msg);
    }

}

void Node::refreshDisplay() const{
    if(renderColor){
        getDisplayString().setTagArg("i", 1, "orange");
        getDisplayString().setTagArg("i", 2, 50);
    }else if(finishColor){
        getDisplayString().setTagArg("i", 1, "green");
        getDisplayString().setTagArg("i", 2, 50);
    }
}
