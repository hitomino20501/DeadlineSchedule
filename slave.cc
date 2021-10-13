#include <string.h>
#include <omnetpp.h>
#include <cmath>
#include "state.h"
#include "dispatch_m.h"

using namespace omnetpp;

class Slave : public cSimpleModule{
  protected:
    // The following redefined virtual function holds the algorithm.
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Slave);

void Slave::initialize(){
    Dispatch *msg = new Dispatch("hello");
    msg->setKind(WorkerState::REQUEST_JOB);
    // ���� 2��
    scheduleAt(2.0, msg);
}

void Slave::handleMessage(cMessage *msg){
    if(msg->isSelfMessage()){
        // �B�z�ۤv��message
        send(msg, "out");
    }
    else{
        // �B�z�Ӧ�server��message
        EV<<"worker log: get a msg\n";
        Dispatch *dispatchJob = check_and_cast<Dispatch *>(msg);

        int msgKind = dispatchJob->getKind();
        if(msgKind==WorkerState::Dispatch_JOB){
            EV<<"worker log kind:"<<msgKind<<"\n";
            struct Job job = dispatchJob->getJob();
            EV<<"worker log weight:"<<job.weight<<"\n";
            // �]�wrender�ɶ�
            simtime_t renderTime = round(par("delayTime"));
            // ����render �o�e�T����server
            dispatchJob->setKind(WorkerState::FRAME_SUCCEEDED);
            scheduleAt(simTime()+renderTime, dispatchJob);
        }
    }
}
