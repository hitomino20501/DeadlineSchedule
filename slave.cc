#include <string.h>
#include <omnetpp.h>
#include <cmath>
#include "state.h"
#include "dispatch_m.h"

using namespace omnetpp;

class Slave : public cSimpleModule{
    private:
        struct Job job;
        bool renderColor = false;
        std::string jobColor;
        std::string userName;
    protected:
    // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void refreshDisplay() const override;
};

Define_Module(Slave);

void Slave::initialize(){
    Dispatch *msg = new Dispatch("hello");
    msg->setKind(WorkerState::REQUEST_JOB);
    // 延遲 2秒
    scheduleAt(2.0, msg);
}

void Slave::handleMessage(cMessage *msg){
    if(msg->isSelfMessage()){
        EV<<"Slave got a message from itself: "<<simTime()<<"\n";
        // 處理自己的message
        int msgKind = msg->getKind();
        if(msgKind==WorkerState::Dispatch_JOB){
            EV<<"Slave render finish: "<<simTime()<<"\n";
            msg->setKind(WorkerState::FRAME_SUCCEEDED);
            msg->setSchedulingPriority(5);
            renderColor = false;
            send(msg, "out");
        }else{
            EV<<"Slave request a job: "<<simTime()<<"\n";
            msg->setKind(WorkerState::REQUEST_JOB);
            msg->setSchedulingPriority(5);
            send(msg, "out");
        }
    }
    else{
        // 處理來自server的message
        EV<<"Slave got a message(job) from database: "<<simTime()<<"\n";
        int msgKind = msg->getKind();
        if(msgKind==WorkerState::Dispatch_JOB){
            // Log
            Dispatch *dispatchJob = check_and_cast<Dispatch *>(msg);
            job = dispatchJob->getJob();
            jobColor = job.jobColor;
            userName = job.user.name;
            /*EV<<"Job info:\n";
            EV<<"  jobIndex:"<<job.jobIndex<<"\n";
            EV<<"  userName:"<<job.user.name<<"\n";
            EV<<"  renderingFrame:"<<job.renderingFrame<<"\n";
            EV<<"  finishFrame:"<<job.finishFrame<<"\n";
            EV<<"  weight:"<<job.weight<<"\n";*/
            EV<<"Slave start rendering: "<<simTime()<<"\n";

            // 設定render時間
            // 完成render 發送訊息給server
            renderColor = true;
            simtime_t renderTime = round(par("delayTime"));
            scheduleAt(simTime()+renderTime, msg);
        }else{
            EV<<"Database no pending job: "<<simTime()<<"\n";
            scheduleAt(simTime()+0.5, msg);
        }
    }
}

void Slave::refreshDisplay() const{
    if(renderColor){
        auto c_jobColor = jobColor.c_str();
        getDisplayString().setTagArg("i", 1, c_jobColor);
        getDisplayString().setTagArg("i", 2, 70);
        auto c_userName = userName.c_str();
        getDisplayString().setTagArg("t", 0, c_userName);
    }else{
        getDisplayString().setTagArg("i", 1, "snow");
    }
}
