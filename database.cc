#include <string.h>
#include <omnetpp.h>
#include <queue>
#include <vector>
#include <sstream>
#include "user.h"
#include "job.h"
#include "state.h"
#include "dispatch_m.h"
#define totalUser 4
#define eachUserJob 1

using namespace omnetpp;

class Database : public cSimpleModule{
    private:
        std::vector<Job> jobVector;
        std::queue<int> destQueue;
        std::queue<std::string> colorQueue;
        struct Job job;
        int PW = 1;
        int EW = 0;
        int SW = 0;
        int RB = 0;
        int RW = -1;
        int queueableJob = 0;
        Job findDispatchJob(std::vector<Job> *jobVector);
        void generateColor(std::queue<std::string> *colorQueue);
        bool isQueueHasJob();
        bool isOverTotalFrame(Job *job);
    protected:
    // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Database);

void Database::initialize(){
    jobVector.reserve(50);
    // 以下省略 改用workstation送出工作
    /*queueableJob = totalUser * eachUserJob;
    jobVector.reserve(queueableJob);

    generateColor(&colorQueue);

    // 新增User Job
    // User userList[totalUser] = {{"A",10}, {"B",20}, {"C",30}, {"D",40}};
    int pr[4]={20, 15, 11, 10};
    User userList[totalUser];
    int userPriority = 10;
    for(int i=0;i<totalUser;i++){
        userList[i].name = "User"+std::to_string(i);
        userList[i].priority = pr[i];
        //userList[i].priority = userPriority;
        //userPriority = userPriority+5;
    }

    int jobIndex = 0;
    for(int i=0;i<totalUser;i++){
        for(int j=0;j<eachUserJob;j++){
            job.user = userList[i];
            job.jobIndex = jobIndex;
            job.weight = (job.user.priority * PW)+(job.errorFrame * EW)+(0 * SW)+((job.renderingFrame - RB) * RW);
            job.jobColor = colorQueue.front();
            jobIndex++;
            jobVector.push_back(job);
        }
        EV<<"color: "<<colorQueue.front()<<"\n";
        colorQueue.push(colorQueue.front());
        colorQueue.pop();
    }*/
}

void Database::handleMessage(cMessage *msg){
    if(msg->isSelfMessage()){
        EV<<"Database receive a message from itself"<<simTime()<<"\n";
        EV<<"Finished the request: "<<simTime()<<"\n";
        /*EV<<"Dispatch job info:\n";
        EV<<"  user: "<<job.user.name<<"\n";
        EV<<"  priority: "<<job.user.priority<<"\n";
        EV<<"  jobIndex:"<<job.jobIndex<<"\n";
        EV<<"  weight:"<<job.weight<<"\n";*/
        send(msg, "out", destQueue.front());
        destQueue.pop();
    }else{
        // 處理自worker/workstation的訊息
        int msgKind = msg->getKind();
        int dest = msg->getArrivalGate()->getIndex();
        if(msgKind==WorkerState::REQUEST_JOB){
            EV<<"Database receive a message from worker["<<dest<<"]: "<<simTime()<<"\n";
            EV<<"Message type: REQUEST_JOB\n";
            destQueue.push(dest);
            // 刪除自worker的cMessage訊息
            delete msg;

            if(isQueueHasJob()){
                // 取出vector中weight最大的job
                job = findDispatchJob(&jobVector);
                // 更新job狀態
                job.renderingFrame = job.renderingFrame + 1;
                job.weight = (job.user.priority * PW)+(job.errorFrame * EW)+(0 * SW)+((job.renderingFrame - RB) * RW);
                jobVector.at(job.jobIndex) = job;

                // 新增一個Dispatch訊息
                Dispatch *dispatchJob = new Dispatch("dispatchJob");
                dispatchJob->setKind(WorkerState::Dispatch_JOB);
                dispatchJob->setJob(job);
                // 模擬處理請求時間
                scheduleAt(simTime()+0.5, dispatchJob);
            }else{
                Dispatch *noDispatchJob = new Dispatch("noDispatchJob");
                noDispatchJob->setKind(WorkerState::NO_Dispatch_JOB);
                scheduleAt(simTime()+0.5, noDispatchJob);
            }
            //TODO:isQueueHasNoJob 回傳
        }
        else if(msgKind==WorkerState::FRAME_SUCCEEDED){
            EV<<"Database receive a message from worker["<<dest<<"]: "<<simTime()<<"\n";
            // 取得jobIndex 更新vector中job狀態
            EV<<"Message type: FRAME_SUCCEEDED\n";
            destQueue.push(dest);
            Dispatch *receiveJob = check_and_cast<Dispatch *>(msg);
            int jobIndex = receiveJob->getJob().jobIndex;
            delete receiveJob;

            job = jobVector.at(jobIndex);
            job.renderingFrame = job.renderingFrame - 1;
            job.finishFrame = job.finishFrame + 1;
            job.weight = (job.user.priority * PW)+(job.errorFrame * EW)+(0 * SW)+((job.renderingFrame - RB) * RW);
            // 檢查job是否完成
            if(job.finishFrame==job.totalFrame){
                job.isJobFinish = true;
                queueableJob--;
            }
            jobVector.at(jobIndex) = job;

            if(isQueueHasJob()){
                job = findDispatchJob(&jobVector);
                if(!isOverTotalFrame(&job)){
                    job.renderingFrame = job.renderingFrame + 1;
                    job.weight = (job.user.priority * PW)+(job.errorFrame * EW)+(0 * SW)+((job.renderingFrame - RB) * RW);
                    jobVector.at(job.jobIndex) = job;

                    // 新增一個Dispatch訊息
                    Dispatch *dispatchJob = new Dispatch("dispatchJob");
                    dispatchJob->setKind(WorkerState::Dispatch_JOB);
                    dispatchJob->setJob(job);
                    scheduleAt(simTime()+0.5, dispatchJob);
                }
            }
        }else if(msgKind==WorkerState::SUBMIT_JOB){
            EV<<"Database receive a message from workstation: "<<simTime()<<"\n";
            EV<<"Message type: SUBMIT_JOB\n";
            Dispatch *submitJob = check_and_cast<Dispatch *>(msg);
            jobVector.push_back(submitJob->getJob());
            queueableJob++;
        }
    }

}

Job Database::findDispatchJob(std::vector<Job> *jobVector){
    int index = 0;
    int max = 0;
    int maxIndex = 0;
    for (auto it = jobVector->begin(); it != jobVector->end(); ++it){
        if((!(*it).isJobFinish) && ((*it).finishFrame+(*it).renderingFrame!=(*it).totalFrame)){
            // TODO:加入balance
            // 當vector中有一樣weight時
            // >表示對index越前面的越有利
            // >=表示對index越後面的越有利
            if((*it).weight >= max){
                max = (*it).weight;
                maxIndex = index;
            }
        }
        index++;
    }
    //EV<<"maxIndex:"<<maxIndex<<"\n";
    return jobVector->at(maxIndex);
}

bool Database::isQueueHasJob(){
    if(queueableJob>0){
        return true;
    }
    return false;
}

bool Database::isOverTotalFrame(Job *job){
    if(job->finishFrame+job->renderingFrame==job->totalFrame){
        return true;
    }
    return false;
}

void Database::generateColor(std::queue<std::string> *colorQueue){
    colorQueue->push("darkorange");
    colorQueue->push("magenta");
    colorQueue->push("lightskyblue");
    colorQueue->push("blue");
    colorQueue->push("aqua");
    colorQueue->push("darkgreen");
    colorQueue->push("blueviolet");
    colorQueue->push("brown");
    colorQueue->push("darkcyan");
    colorQueue->push("deeppink");
    colorQueue->push("gold");
    colorQueue->push("hotpink");
    colorQueue->push("khaki");
    colorQueue->push("aquamarine");
    colorQueue->push("yellow");
    colorQueue->push("yellowgreen");
    colorQueue->push("violet");
    colorQueue->push("tomato");
    colorQueue->push("tan");
    colorQueue->push("indianred");
    colorQueue->push("sandybrown");
    colorQueue->push("red");
    colorQueue->push("purple");
    colorQueue->push("mediumblue");
    colorQueue->push("peru");
    colorQueue->push("pink");
    colorQueue->push("olivedrab");
    colorQueue->push("orange");
    colorQueue->push("navy");
    colorQueue->push("indigo");
}
