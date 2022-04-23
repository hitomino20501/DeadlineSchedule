#include <string.h>
#include <omnetpp.h>
#include <cmath>
#include <vector>
#include <sstream>
#include <numeric>
#include "state.h"
#include "dispatch_m.h"
#include "generate_job.h"
#include "generate_jobB.h"
#include "user.h"
#include "job.h"
#define totalUser 4
#define eachUserJob 1

using namespace omnetpp;

class Slave : public cSimpleModule{
    private:
        //struct Job* job;
        bool renderColor = false;
        std::string userColor;
        std::string userName;
        int limitSearchUser = 0;
        double denominator = 0.0;
        double totalSlave = 100.0;

        std::vector<User>& userVectorA = GenerateJob::getInstance().getAllUser();
        std::vector<std::vector<Job>>& jobVectorA = GenerateJob::getInstance().getAllJob();
        std::vector<int>& farmCreditA = GenerateJob::getInstance().getFarmCredit();
        std::vector<User>& userVectorB = GenerateJobB::getInstance().getAllUser();
        std::vector<std::vector<Job>>& jobVectorB = GenerateJobB::getInstance().getAllJob();
        std::vector<int>& farmCreditB = GenerateJobB::getInstance().getFarmCredit();

        std::vector<int>& slaveStateA = GenerateJob::getInstance().getSlaveState();
        std::vector<int>& slaveStateB = GenerateJobB::getInstance().getSlaveState();

        std::vector<User> balancedVector;
        std::vector<User> proportionVector;

        int PW = 1;
        int EW = 0;
        int SW = 0;
        int RB = 0;
        int RW = -1;

        bool isOverTotalFrame(Job *job);
        bool isAllJobFinisd(int userIndex, std::vector<User>& userVector, std::vector<std::vector<Job>>& jobVector);
        User& findDispatchUser(std::vector<User>& userVector, std::vector<std::vector<Job>>& jobVector);
        Job* findDispatchJob(User *user, std::vector<std::vector<Job>>& jobVector);
        void dispatchJob(User* user, Job* job);
        void printFarmCredit(std::vector<int> &credit);
        std::string findFarmCredit(std::vector<int> &credit);
        bool isFarmIdleSlave(std::vector<int>& slaveState);
        int calRenderSlave(int farmIndex, std::vector<int>& slaveState);
    protected:
    // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void refreshDisplay() const override;
        virtual void finish() override;
};

Define_Module(Slave);

void Slave::initialize(){
    Dispatch *msg = new Dispatch("hello");
    msg->setKind(WorkerState::REQUEST_JOB);
    msg->setSchedulingPriority(5);
    // 延遲 2秒
    scheduleAt(10.0, msg);
}

void Slave::handleMessage(cMessage *msg){
    if(msg->isSelfMessage()){
        //EV<<"Slave got a message from itself: "<<simTime()<<"\n";
        // 處理自己的message
        int msgKind = msg->getKind();
        if(msgKind==WorkerState::FRAME_SUCCEEDED){
            //EV<<"Slave render finish: "<<simTime()<<"\n";
            renderColor = false;
            Dispatch *msg1 = check_and_cast<Dispatch *>(msg);
            int userIndex = msg1->getJob().user->userIndex;
            int jobVectorIndex = msg1->getJob().jobVectorIndex;
            int jobIndex = msg1->getJob().jobIndex;
            std::string farmName = msg1->getJob().farm;
            delete msg1;

            struct User* user;
            struct Job* job;
            if(farmName=="A"){
                user = &userVectorA.at(userIndex);
                job = &jobVectorA[jobVectorIndex].at(jobIndex);
            }
            else if(farmName=="B"){
                user = &userVectorB.at(userIndex);
                job = &jobVectorB[jobVectorIndex].at(jobIndex);
            }

            // 更新user
            user->userRenderingFrame = user->userRenderingFrame - 1;
            user->userFinishFrame = user->userFinishFrame + 1;
            user->userWeight = (user->priority * PW)+(user->userErrorFrame * EW)+(0 * SW)+((user->userRenderingFrame - RB) * RW);
            //user->userWeight = user->userWeight - 1;

            // 更新job
            job->renderingFrame = job->renderingFrame - 1;
            job->finishFrame = job->finishFrame + 1;

            // 檢查job是否完成
            if(job->finishFrame==job->totalFrame){
                job->isJobFinish = true;
                job->isActivate = false;
                user->renderingJob = user->renderingJob - 1;
                user->finishJob = user->finishJob + 1;
                //queueableJob--;
            }

            // 更新slave
            slaveStateA[getIndex()] = -1;

            Dispatch *msg2 = new Dispatch("hello");
            msg2->setKind(WorkerState::REQUEST_JOB);
            msg2->setSchedulingPriority(5);
            scheduleAt(simTime()+1.0, msg2);
            /*user = &findDispatchUser(userVectorA, jobVectorA);
            job = findDispatchJob(user, jobVectorA);
            if(job!=nullptr){
                //delete msg;
                dispatchJob(user, job);
            }else{
                //EV<<"Slave FRAME_SUCCEEDED but nullptr:\n";
                Dispatch *msg1 = new Dispatch("hello");
                msg1->setKind(WorkerState::REQUEST_JOB);
                msg1->setSchedulingPriority(5);
                scheduleAt(simTime()+1.0, msg1);
            }*/

        }else{
            //EV<<"Slave request a job: "<<simTime()<<"\n";
            //msg->setKind(WorkerState::REQUEST_JOB);
            //msg->setSchedulingPriority(5);

            struct User* user;
            struct Job* job;

            /*if(getIndex()==1){
                EV<<"getIndex1: \n";
                farmCredit = GenerateJobB::getInstance().getFarmCredit();
            }
            else{
                EV<<"getIndex2: \n";
                farmCredit = GenerateJob::getInstance().getFarmCredit();
            }*/

            user = &findDispatchUser(userVectorA, jobVectorA);
            job = findDispatchJob(user, jobVectorA);
            if(job!=nullptr){
                delete msg;
                dispatchJob(user, job);
            }else{
                // 找一個Credit最大的農場
                //EV<<"find job from other farm\n";
                std::string farm = findFarmCredit(farmCreditA);
                //EV<<farm<<"\n";
                if(farm=="B"){
                    user = &findDispatchUser(userVectorB, jobVectorB);
                    job = findDispatchJob(user, jobVectorB);
                    if(job!=nullptr){
                        delete msg;
                        dispatchJob(user, job);
                    }
                    else{
                        msg->setKind(WorkerState::REQUEST_JOB);
                        msg->setSchedulingPriority(5);
                        scheduleAt(simTime()+1.0, msg);
                    }
                }
                else if(farm=="N"){
                    msg->setKind(WorkerState::REQUEST_JOB);
                    msg->setSchedulingPriority(5);
                    scheduleAt(simTime()+10.0, msg);
                }
            }
        }
    }
    else{
        // 處理來自server的message
        //EV<<"Slave got a message(job) from database: "<<simTime()<<"\n";
        int msgKind = msg->getKind();
        int senderGate = msg->getSenderGate()->getIndex();
        //EV<<"Slave got a message(job) from: "<<test<<" gate\n";
        if(msgKind==WorkerState::Dispatch_JOB){
            // Log
        }else if(msgKind==WorkerState::NO_Dispatch_JOB){
            //EV<<"Database no pending job: "<<simTime()<<"\n";
            scheduleAt(simTime()+1.0, msg);
        }else{
            //EV<<"Shut down slave: "<<simTime()<<"\n";
            cancelAndDelete(msg);
        }
    }
}

std::string Slave::findFarmCredit(std::vector<int> &credit){
    std::vector<int> balancedCreditVector;
    int index = 0;
    int max = -100000;
    int maxIndex = -1;
    for (auto it = credit.begin(); it != credit.end(); ++it){
        //EV<<*it<<"\n";
        // 跳過自己農場index
        if(index==0){
            index++;
            continue;
        }
        else if(index==1){
            // B農場
            if(!isFarmIdleSlave(slaveStateB)){
                if((*it) > max){
                    max = (*it);
                    maxIndex = index;
                }
            }
        }

        index++;
    }
    if(maxIndex==-1){
        return "N";
    }
    index = 0;
    for (auto it = credit.begin(); it != credit.end(); ++it){
        // 跳過自己農場index
        if(index==0){
            index++;
            continue;
        }
        else if(index==1){
            // B農場
            if(!isFarmIdleSlave(slaveStateB)){
                if((*it) == max){
                    balancedCreditVector.push_back(index);
                }
            }
        }

        index++;
    }

    if(balancedVector.size()>1){
        //EV<<"Invoke balanced:\n";
        int minRender = -1;
        for (auto it = balancedCreditVector.begin(); it != balancedCreditVector.end(); ++it){
            int temCel = calRenderSlave((*it), slaveStateA);
            if(minRender==-1){
                minRender = temCel;
                maxIndex = (*it);
            }else{
                if(temCel<minRender){
                    minRender = temCel;
                    maxIndex = (*it);
                }
            }
        }
        balancedCreditVector.clear();
    }

    if(maxIndex==1){
        return "B";
    }

    return "N";
}

bool Slave::isFarmIdleSlave(std::vector<int>& slaveState){
    //EV<<"slaveStateB\n";
    for (auto it = slaveState.begin(); it != slaveState.end(); ++it){
        //EV<<"slaveStateB"<<*it<<"\n";
        // slaveState -1 代表slave idle
        if((*it)==-1){
            return true;
        }
    }
    return false;
}

int Slave::calRenderSlave(int farmIndex, std::vector<int>& slaveState){
    int cal = 0;
    for (auto it = slaveState.begin(); it != slaveState.end(); ++it){
        if((*it)==farmIndex){
            cal++;
        }
    }
    return cal;
}

User& Slave::findDispatchUser(std::vector<User>& userVector, std::vector<std::vector<Job>>& jobVector){
    int index = 0;
    for (auto it = userVector.begin(); it != userVector.end(); ++it){
        /*if(index==limitSearchUser){
            break;
        }*/
        if(!isAllJobFinisd((*it).userIndex, userVector, jobVector)){
            proportionVector.push_back(*it);
        }
        index++;
    }
    denominator = 0;
    for (auto it = proportionVector.begin(); it != proportionVector.end(); ++it){
        if((*it).finishJob != (*it).totalJob){
            denominator = denominator + (*it).proportion;
        }
    }
    //EV<<"denominator: "<<denominator<<"\n";
    double tempWeight = 0.0;
    for (auto it = proportionVector.begin(); it != proportionVector.end(); ++it){
        //EV<<(*it).name<<":proportion: "<<(*it).proportion<<"\n";
        //EV<<(*it).name<<":cal: "<<(*it).proportion/denominator<<"\n";
        tempWeight = totalSlave * ((*it).proportion/denominator);
        //EV<<(*it).name<<":tempWeight: "<<tempWeight<<"\n";
        if((*it).limitUserWeight == -1){
            (*it).limitUserWeight = (int)round(tempWeight);
            //(*it).userWeight = (int)round(tempWeight);
            (*it).priority = (int)round(tempWeight);
            (*it).userWeight = ((*it).priority * PW)+((*it).userErrorFrame * EW)+(0 * SW)+(((*it).userRenderingFrame - RB) * RW);
        }
        else{
            //(*it).userWeight = (*it).userWeight + ((int)round(tempWeight)-(*it).limitUserWeight);
            (*it).priority = (*it).priority + ((int)round(tempWeight)-(*it).limitUserWeight);
            (*it).userWeight = ((*it).priority * PW)+((*it).userErrorFrame * EW)+(0 * SW)+(((*it).userRenderingFrame - RB) * RW);
            (*it).limitUserWeight = (int)round(tempWeight);
        }
        //EV<<(*it).name<<": "<<(*it).userWeight<<"\n";
    }
    for (auto it = proportionVector.begin(); it != proportionVector.end(); ++it){
        userVector[(*it).userIndex] = (*it);
    }
    proportionVector.clear();

    index = 0;
    int max = -1000;
    int maxIndex = 0;
    for (auto it = userVector.begin(); it != userVector.end(); ++it){
        /*if(index==limitSearchUser){
            break;
        }*/
        if(!isAllJobFinisd((*it).userIndex, userVector, jobVector)){
            //EV<<"findDispatchUser:Weight: "<<(*it).userWeight<<"\n";
            if((*it).userWeight > max){
                max = (*it).userWeight;
                maxIndex = (*it).userIndex;
            }
        }
        index++;
    }

    // balance 當weight一樣 把slave分配給render數量低的user
    index = 0;
    for (auto it = userVector.begin(); it != userVector.end(); ++it){
        /*if(index==limitSearchUser){
            break;
        }*/
        if(!isAllJobFinisd((*it).userIndex, userVector, jobVector)){
            if((*it).userWeight == max){
                balancedVector.push_back(*it);
            }
        }
        index++;
    }

    if(!balancedVector.empty()){
        //EV<<"Invoke balanced:\n";
        int minRender = -1;
        for (auto it = balancedVector.begin(); it != balancedVector.end(); ++it){
            if(minRender==-1){
                minRender = (*it).userRenderingFrame;
                maxIndex = (*it).userIndex;
            }else{
                if((*it).userRenderingFrame<minRender){
                    minRender = (*it).userRenderingFrame;
                    maxIndex = (*it).userIndex;
                }
            }
        }
        balancedVector.clear();
    }
    //EV<<"maxIndex:"<<maxIndex<<"\n";
    return userVector.at(maxIndex);
}

bool Slave::isAllJobFinisd(int userIndex, std::vector<User>& userVector, std::vector<std::vector<Job>>& jobVector){
    int i = 0;
    for (auto it = jobVector[userIndex].begin(); it != jobVector[userIndex].end(); ++it){
        if(i==userVector[userIndex].totalJob){
            break;
        }
        i++;
        if(!(*it).isJobFinish){
            if((*it).finishFrame+(*it).renderingFrame<(*it).totalFrame){
                return false;
            }
        }
    }
    return true;
}

void Slave::dispatchJob(User* user, Job* job){
    if(!job->isActivate){
        user->renderingJob = user->renderingJob + 1;
        job->isActivate = true;
        /*if(user->userIndex==0){
            cMessage *drawNode = new cMessage("drawNode");
            drawNode->setKind(WorkerState::JOB_START);
            cModule *node = getParentModule()->getSubmodule("node", job->jobIndex);
            sendDirect(drawNode, node, "in", 0);
        }*/
    }
    user->userRenderingFrame = user->userRenderingFrame+1;
    user->userWeight = (user->priority * PW)+(user->userErrorFrame * EW)+(0 * SW)+((user->userRenderingFrame - RB) * RW);

    // 更新job狀態
    job->renderingFrame = job->renderingFrame + 1;

    // 更新slave狀態
    if(job->farm=="A"){
        slaveStateA[getIndex()] = 0;
    }
    else if(job->farm=="B"){
        slaveStateA[getIndex()] = 1;
        farmCreditA[1] = farmCreditA[1] + 1;
        farmCreditB[0] = farmCreditB[0] - 1;
        /*EV<<"farmCreditA[1]"<<farmCreditA[1]<<"\n";
        EV<<"farmCreditB[0]"<<farmCreditB[0]<<"\n";*/
    }

    // start render
    userColor = job->user->userColor;
    userName = job->user->name;
    //EV<<"Slave start rendering: "<<simTime()<<"\n";
    renderColor = true;
    simtime_t renderTime = 10.0;
    //simtime_t renderTime = round(par("delayTime"));
    Dispatch *msg = new Dispatch("frameSucceed");
    msg->setKind(WorkerState::FRAME_SUCCEEDED);
    msg->setSchedulingPriority(3);
    msg->setJob(*job);
    scheduleAt(simTime()+renderTime, msg);
}

Job* Slave::findDispatchJob(User *user, std::vector<std::vector<Job>>& jobVector){
    // TODO:後面加入workFlow
    int index = 0;
    int i = 0;
    for (auto it = jobVector[(*user).userIndex].begin(); it != jobVector[(*user).userIndex].end(); ++it){
        if(i==(*user).totalJob){
            break;
        }
        i++;
        if((!(*it).isJobFinish) && ((*it).finishFrame+(*it).renderingFrame<(*it).totalFrame)){
            index = (*it).jobIndex;
            return &(jobVector[(*user).userIndex].at(index));
        }
    }

    return nullptr;
}

void Slave::refreshDisplay() const{
    if(renderColor){
        auto c_jobColor = userColor.c_str();
        getDisplayString().setTagArg("i", 1, c_jobColor);
        getDisplayString().setTagArg("i", 2, 70);
        auto c_userName = userName.c_str();
        getDisplayString().setTagArg("t", 0, c_userName);
    }else{
        getDisplayString().setTagArg("i", 1, "snow");
    }
}

void Slave::printFarmCredit(std::vector<int> &credit){
    EV<<"farmCredit: "<<credit[0]<<"\n";
}

void Slave::finish() {
    //jobVector.clear();
    GenerateJob::getInstance().clearVector();
    balancedVector.clear();
}
