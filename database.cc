#include <string.h>
#include <omnetpp.h>
#include <queue>
#include "user.h"
#include "job.h"
#include "state.h"
#include "dispatch_m.h"

using namespace omnetpp;

struct cmp{
    bool operator() (Job a, Job b){
        return a.weight < b.weight;
    }
};

class Database : public cSimpleModule{
    private:
        std::priority_queue<Job, std::vector<Job>, cmp> jobQueue;
        int PW = 10;
        int EW = 0;
        int SW = 0;
        int RB = 0;
        int RW = -1;
    protected:
    // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Database);

void Database::initialize(){
    // �إ�User
    struct User userList[4] = {{10}, {20}, {30}, {40}};
    // ����Job
    // �Cuser 1��Job �CJob 20��Frame
    for(int i=0;i<4;i++){
        struct Job job;
        job.user = userList[i];
        int jobWeight = (job.user.priority * PW)+(job.errorFrame * EW)+(0 * SW)+((job.renderingFrame - RB) * RW);
        job.weight = jobWeight;
        jobQueue.push(job);
    }
}

void Database::handleMessage(cMessage *msg){
    // �B�z��worker���T��
    int dest = msg->getArrivalGate()->getIndex();
    EV<<"worker index: "<<dest<<"\n";

    int msgKind = msg->getKind();
    if(msgKind==WorkerState::REQUEST_JOB){
        // �R����worker��cMessage�T��
        delete msg;

        // �s�W�@��Dispatch�T��
        Dispatch *dispatchJob = new Dispatch("dispatchJob");
        dispatchJob->setKind(WorkerState::Dispatch_JOB);

        // ���Xqueue��weight�̤j��job
        struct Job job = jobQueue.top();
        // ��sjob���A
        job.renderingFrame = job.renderingFrame + 1;
        job.weight = (job.user.priority * PW)+(job.errorFrame * EW)+(0 * SW)+((job.renderingFrame - RB) * RW);
        dispatchJob->setJob(job);

        // ��spriority_queue pop��Apush
        jobQueue.pop();
        jobQueue.push(job);
        EV<<"weight:"<<job.weight<<"\n";

        send(dispatchJob, "out", dest);
    }
    else if(msgKind==WorkerState::FRAME_SUCCEEDED){
        Dispatch *dispatchJob = check_and_cast<Dispatch *>(msg);
        // ��sjob���A
        struct Job job = dispatchJob->getJob();
        EV<<"weight:"<<job.weight<<"\n";
        job.renderingFrame = job.renderingFrame - 1;
        job.finishFrame = job.finishFrame + 1;
        // �ˬdjob�O�_���� �����qqueue pop��job
        if(job.finishFrame!=job.totalFrame){
            job.weight = (job.user.priority * PW)+(job.errorFrame * EW)+(0 * SW)+((job.renderingFrame - RB) * RW);
            // ��spriority_queue pop��Apush
            jobQueue.pop();
            jobQueue.push(job);
        }else{
            jobQueue.pop();
        }

        job = jobQueue.top();
        // ��sjob���A
        job.renderingFrame = job.renderingFrame + 1;
        job.weight = (job.user.priority * PW)+(job.errorFrame * EW)+(0 * SW)+((job.renderingFrame - RB) * RW);
        dispatchJob->setJob(job);

        // ��spriority_queue pop��Apush
        jobQueue.pop();
        jobQueue.push(job);
        EV<<"weight:"<<job.weight<<"\n";

        send(dispatchJob, "out", dest);
    }

}
