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
    // 建立User
    struct User userList[4] = {{10}, {20}, {30}, {40}};
    // 產生Job
    // 每user 1個Job 每Job 20個Frame
    for(int i=0;i<4;i++){
        struct Job job;
        job.user = userList[i];
        int jobWeight = (job.user.priority * PW)+(job.errorFrame * EW)+(0 * SW)+((job.renderingFrame - RB) * RW);
        job.weight = jobWeight;
        jobQueue.push(job);
    }
}

void Database::handleMessage(cMessage *msg){
    // 處理自worker的訊息
    int dest = msg->getArrivalGate()->getIndex();
    EV<<"worker index: "<<dest<<"\n";

    int msgKind = msg->getKind();
    if(msgKind==WorkerState::REQUEST_JOB){
        // 刪除自worker的cMessage訊息
        delete msg;

        // 新增一個Dispatch訊息
        Dispatch *dispatchJob = new Dispatch("dispatchJob");
        dispatchJob->setKind(WorkerState::Dispatch_JOB);

        // 取出queue中weight最大的job
        struct Job job = jobQueue.top();
        // 更新job狀態
        job.renderingFrame = job.renderingFrame + 1;
        job.weight = (job.user.priority * PW)+(job.errorFrame * EW)+(0 * SW)+((job.renderingFrame - RB) * RW);
        dispatchJob->setJob(job);

        // 更新priority_queue pop後再push
        jobQueue.pop();
        jobQueue.push(job);
        EV<<"weight:"<<job.weight<<"\n";

        send(dispatchJob, "out", dest);
    }
    else if(msgKind==WorkerState::FRAME_SUCCEEDED){
        Dispatch *dispatchJob = check_and_cast<Dispatch *>(msg);
        // 更新job狀態
        struct Job job = dispatchJob->getJob();
        EV<<"weight:"<<job.weight<<"\n";
        job.renderingFrame = job.renderingFrame - 1;
        job.finishFrame = job.finishFrame + 1;
        // 檢查job是否完成 完成從queue pop此job
        if(job.finishFrame!=job.totalFrame){
            job.weight = (job.user.priority * PW)+(job.errorFrame * EW)+(0 * SW)+((job.renderingFrame - RB) * RW);
            // 更新priority_queue pop後再push
            jobQueue.pop();
            jobQueue.push(job);
        }else{
            jobQueue.pop();
        }

        job = jobQueue.top();
        // 更新job狀態
        job.renderingFrame = job.renderingFrame + 1;
        job.weight = (job.user.priority * PW)+(job.errorFrame * EW)+(0 * SW)+((job.renderingFrame - RB) * RW);
        dispatchJob->setJob(job);

        // 更新priority_queue pop後再push
        jobQueue.pop();
        jobQueue.push(job);
        EV<<"weight:"<<job.weight<<"\n";

        send(dispatchJob, "out", dest);
    }

}
