#include <string.h>
#include <omnetpp.h>
#include <cmath>
#include <vector>
#include <sstream>
#include <numeric>
#include "state.h"
#include "dispatch_m.h"
#include "generate_jobB.h"
#include "user.h"
#include "job.h"
#define totalUser 4
#define eachUserJob 10

using namespace omnetpp;

class SlaveB : public cSimpleModule
{
    private:
        //struct Job* job;
        bool renderColor = false;
        std::string userColor;
        std::string userName;
        int limitSearchUser = 0;
        double denominator = 0.0;
        double totalSlave = 100.0;

        std::vector<User>& userVector = GenerateJobB::getInstance().getAllUser();
        std::vector<std::vector<Job>>& jobVector = GenerateJobB::getInstance().getAllJob();

        std::vector<User> balancedVector;
        std::vector<User> proportionVector;

        int PW = 1;
        int EW = 0;
        int SW = 0;
        int RB = 0;
        int RW = -1;

        bool isOverTotalFrame(Job *job);
        bool isAllJobFinisd(int userIndex);
        User& findDispatchUser();
        Job* findDispatchJob(User *user);
        void dispatchJob(User* user, Job* job);
    protected:
    // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void refreshDisplay() const override;
        virtual void finish() override;
};

Define_Module(SlaveB);

void SlaveB::initialize(){
    Dispatch *msg = new Dispatch("hello");
    msg->setKind(WorkerState::REQUEST_JOB);
    msg->setSchedulingPriority(5);
    // ���� 2��
    scheduleAt(10.0, msg);
}

void SlaveB::handleMessage(cMessage *msg){
    if(msg->isSelfMessage()){
        //EV<<"Slave got a message from itself: "<<simTime()<<"\n";
        // �B�z�ۤv��message
        int msgKind = msg->getKind();
        if(msgKind==WorkerState::FRAME_SUCCEEDED){
            //EV<<"Slave render finish: "<<simTime()<<"\n";
            renderColor = false;
            Dispatch *msg1 = check_and_cast<Dispatch *>(msg);
            int userIndex = msg1->getJob().user->userIndex;
            int jobVectorIndex = msg1->getJob().jobVectorIndex;
            int jobIndex = msg1->getJob().jobIndex;
            delete msg1;

            struct User* user;
            struct Job* job;
            user = &userVector.at(userIndex);
            job = &jobVector[jobVectorIndex].at(jobIndex);

            // ��suser
            user->userRenderingFrame = user->userRenderingFrame - 1;
            user->userFinishFrame = user->userFinishFrame + 1;
            user->userWeight = (user->priority * PW)+(user->userErrorFrame * EW)+(0 * SW)+((user->userRenderingFrame - RB) * RW);
            //user->userWeight = user->userWeight - 1;

            // ��sjob
            job->renderingFrame = job->renderingFrame - 1;
            job->finishFrame = job->finishFrame + 1;

            // �ˬdjob�O�_����
            if(job->finishFrame==job->totalFrame){
                job->isJobFinish = true;
                job->isActivate = false;
                user->renderingJob = user->renderingJob - 1;
                user->finishJob = user->finishJob + 1;
                //queueableJob--;
            }

            user = &findDispatchUser();
            job = findDispatchJob(user);
            if(job!=nullptr){
                //delete msg;
                dispatchJob(user, job);
            }else{
                //EV<<"Slave FRAME_SUCCEEDED but nullptr:\n";
                Dispatch *msg1 = new Dispatch("hello");
                msg1->setKind(WorkerState::REQUEST_JOB);
                msg1->setSchedulingPriority(5);
                scheduleAt(simTime()+1.0, msg1);
            }

        }else{
            //EV<<"Slave request a job: "<<simTime()<<"\n";
            //msg->setKind(WorkerState::REQUEST_JOB);
            //msg->setSchedulingPriority(5);

            struct User* user;
            struct Job* job;

            user = &findDispatchUser();
            job = findDispatchJob(user);
            if(job!=nullptr){
                delete msg;
                dispatchJob(user, job);
            }else{
                //EV<<"Slave REQUEST_JOB but nullptr:\n";
                msg->setKind(WorkerState::REQUEST_JOB);
                msg->setSchedulingPriority(5);
                scheduleAt(simTime()+1.0, msg);
            }
        }
    }
    else{
        // �B�z�Ӧ�server��message
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

User& SlaveB::findDispatchUser(){
    int index = 0;
    for (auto it = userVector.begin(); it != userVector.end(); ++it){
        /*if(index==limitSearchUser){
            break;
        }*/
        if(!isAllJobFinisd((*it).userIndex)){
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
        if(!isAllJobFinisd((*it).userIndex)){
            //EV<<"findDispatchUser:Weight: "<<(*it).userWeight<<"\n";
            if((*it).userWeight > max){
                max = (*it).userWeight;
                maxIndex = (*it).userIndex;
            }
        }
        index++;
    }

    // balance ��weight�@�� ��slave���t��render�ƶq�C��user
    index = 0;
    for (auto it = userVector.begin(); it != userVector.end(); ++it){
        /*if(index==limitSearchUser){
            break;
        }*/
        if(!isAllJobFinisd((*it).userIndex)){
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

bool SlaveB::isAllJobFinisd(int userIndex){
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

void SlaveB::dispatchJob(User* user, Job* job){
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

    // ��sjob���A
    job->renderingFrame = job->renderingFrame + 1;

    // start render
    userColor = job->user->userColor;
    userName = job->user->name;
    //EV<<"Slave start rendering: "<<simTime()<<"\n";
    renderColor = true;
    simtime_t renderTime = 10.0;
    //simtime_t renderTime = round(par("delayTime"));
    Dispatch *msg = new Dispatch("frameSucceed");
    msg->setKind(WorkerState::FRAME_SUCCEEDED);
    msg->setSchedulingPriority(5);
    msg->setJob(*job);
    scheduleAt(simTime()+renderTime, msg);
}

Job* SlaveB::findDispatchJob(User *user){
    // TODO:�᭱�[�JworkFlow
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

void SlaveB::refreshDisplay() const{
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

void SlaveB::finish() {
    //jobVector.clear();
    GenerateJobB::getInstance().clearVector();
    balancedVector.clear();
}