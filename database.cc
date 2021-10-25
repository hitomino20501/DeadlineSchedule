#include <string.h>
#include <omnetpp.h>
#include <queue>
#include <vector>
#include <sstream>
#include "user.h"
#include "job.h"
#include "state.h"
#include "dispatch_m.h"
#include "generate_job.h"
#define totalUser 4
#define eachUserJob 3

using namespace omnetpp;

class Database : public cSimpleModule{
    private:
        std::vector<std::vector<Job>>& jobVector = GenerateJob::getInstance().getAllJob();
        std::vector<User> balancedVector;
        std::queue<int> destQueue;
        std::queue<std::string> colorQueue;
        //std::vector<User> userVector;
        std::vector<User>& userVector = GenerateJob::getInstance().getAllUser();
        int PW = 1;
        int EW = 0;
        int SW = 0;
        int RB = 0;
        int RW = -1;
        int queueableJob = 0;//�n��
        //int queueableJob = totalUser * eachUserJob;
        int logFlag = 0;
        int limitSearchUser = 0;
        User& findDispatchUser();
        Job* findDispatchJob(User *user);
        void generateColor(std::queue<std::string> *colorQueue);
        bool isQueueHasJob();
        bool isOverTotalFrame(Job *job);
        bool isAllJobFinisd(int userIndex);
        void dispatchJob(User* user, Job* job);
    protected:
    // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void finish() override;
        virtual void refreshDisplay() const override;
};

Define_Module(Database);

void Database::initialize(){
    jobVector.reserve(100);
    balancedVector.reserve(100);

    /*struct Job job;
    struct User user;
    generateColor(&colorQueue);
    userVector.reserve(totalUser);
    int pr[totalUser]={10, 11, 15, 20};
    int userIndex = 0;
    for(int i=0;i<totalUser;i++){
        user.name = "User"+std::to_string(i);
        user.priority = pr[i];
        user.userIndex = userIndex;
        user.userWeight = (user.priority * PW)+(user.userErrorFrame * EW)+(0 * SW)+((user.userRenderingFrame - RB) * RW);
        user.userColor = colorQueue.front();
        userVector.push_back(user);
        userIndex++;
        colorQueue.push(colorQueue.front());
        colorQueue.pop();
    }

    std::vector<Job> temJob;

    int jobIndex = 0;
    for(int i=0;i<totalUser;i++){
        for(int j=0;j<eachUserJob;j++){
            job.user = &userVector[i];
            job.jobIndex = jobIndex;
            jobIndex++;
            temJob.push_back(job);
            queueableJob++;
            userVector[i].totalJob = userVector[i].totalJob+1;
        }
        jobVector.push_back(temJob);
        jobIndex = 0;
        temJob.clear();
    }*/

    Dispatch *msg = new Dispatch("log");
    msg->setKind(WorkerState::LOG_TIMER);
    msg->setSchedulingPriority(10);
    scheduleAt(1.5, msg);

    // �H�U�ٲ� ���workstation�e�X�u�@ �ª� �H�o��
    /*queueableJob = totalUser * eachUserJob;
    jobVector.reserve(queueableJob);

    generateColor(&colorQueue);

    // �s�WUser Job
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
    int msgKind = msg->getKind();
    if(msg->isSelfMessage()){
        if(msgKind==WorkerState::LOG_TIMER){
            //EV<<"just print\n";
            int renderingFrameZero = 0;
            int index = 0;
            for (auto it = userVector.begin(); it != userVector.end(); ++it){
                if(index==limitSearchUser){
                    break;
                }
                EV<<"{";
                EV<<"'simTime':"<<simTime()<<",";
                EV<<"'userName':'"<<(*it).name<<"',";
                EV<<"'renderingFrame':"<<(*it).userRenderingFrame<<",";
                EV<<"'weight':"<<(*it).userWeight<<",";
                EV<<"'total':"<<(*it).userFinishFrame;
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
                scheduleAt(simTime()+1.0, msg);
            }else{
                cancelAndDelete(msg);
            }

        }else{
            EV<<"Database receive a message from itself"<<simTime()<<"\n";
            EV<<"Finished the request: "<<simTime()<<"\n";
            /*EV<<"Dispatch job info:\n";
            EV<<"  user: "<<job.user.name<<"\n";
            EV<<"  priority: "<<job.user.priority<<"\n";
            EV<<"  jobIndex:"<<job.jobIndex<<"\n";
            EV<<"  weight:"<<job.weight<<"\n";*/
            send(msg, "out", destQueue.front());
            destQueue.pop();
        }
    }else{
        // �B�z��worker/workstation���T��
        int dest = msg->getArrivalGate()->getIndex();
        if(msgKind==WorkerState::REQUEST_JOB){
            EV<<"Database receive a message from worker["<<dest<<"]: "<<simTime()<<"\n";
            EV<<"Message type: REQUEST_JOB\n";
            destQueue.push(dest);
            // �R����worker��cMessage�T��
            delete msg;

            if(isQueueHasJob()){
                // ���Xvector��weight�̤j��job
                // �令���Xvector��weight�̤j��user
                struct User* user;
                struct Job* job;
                user = &findDispatchUser();
                job = findDispatchJob(user);
                if(job!=nullptr){
                    dispatchJob(user, job);
                }else{
                    Dispatch *noDispatchJob = new Dispatch("noDispatchJob");
                    noDispatchJob->setKind(WorkerState::NO_Dispatch_JOB);
                    scheduleAt(simTime()+0.5, noDispatchJob);
                }
            }else{
                if(logFlag<4){
                    Dispatch *noDispatchJob = new Dispatch("noDispatchJob");
                    noDispatchJob->setKind(WorkerState::NO_Dispatch_JOB);
                    scheduleAt(simTime()+0.5, noDispatchJob);
                }else{
                    // ����slave
                    Dispatch *shutDown = new Dispatch("shutDown");
                    shutDown->setKind(WorkerState::SHUT_DOWN_SLAVE);
                    scheduleAt(simTime()+0.5, shutDown);
                }
            }
        }
        else if(msgKind==WorkerState::FRAME_SUCCEEDED){
            EV<<"Database receive a message from worker["<<dest<<"]: "<<simTime()<<"\n";
            // ���ojobIndex ��svector��job���A
            EV<<"Message type: FRAME_SUCCEEDED\n";
            destQueue.push(dest);
            Dispatch *receiveJob = check_and_cast<Dispatch *>(msg);
            int userIndex = receiveJob->getJob().user->userIndex;
            int jobIndex = receiveJob->getJob().jobIndex;
            delete receiveJob;

            struct User* user;
            struct Job* job;
            user = &userVector.at(userIndex);
            job = &jobVector[userIndex].at(jobIndex);

            // ��suser
            user->userRenderingFrame = user->userRenderingFrame - 1;
            user->userFinishFrame = user->userFinishFrame + 1;
            user->userWeight = (user->priority * PW)+(user->userErrorFrame * EW)+(0 * SW)+((user->userRenderingFrame - RB) * RW);

            // ��sjob
            job->renderingFrame = job->renderingFrame - 1;
            job->finishFrame = job->finishFrame + 1;

            // �ˬdjob�O�_����
            if(job->finishFrame==job->totalFrame){
                job->isJobFinish = true;
                job->isActivate = false;
                user->renderingJob = user->renderingJob - 1;
                queueableJob--;
            }

            // �A�qqueue����ܤ@�ӦX�A���u�@
            if(isQueueHasJob()){
                user = &findDispatchUser();
                job = findDispatchJob(user);
                if(job!=nullptr){
                    dispatchJob(user, job);
                }
            }
        }else if(msgKind==WorkerState::SUBMIT_JOB){
            EV<<"Database receive a message from workstation: "<<simTime()<<"\n";
            EV<<"Message type: SUBMIT_JOB\n";
            /*Submit *submitJob = check_and_cast<Submit *>(msg);
            userVector.push_back(*(submitJob->getWorkflow().user));
            queueableJob = queueableJob + jobVector[limitSearchUser].size();
            limitSearchUser++;
            delete submitJob;*/
            queueableJob = queueableJob + jobVector[limitSearchUser].size();
            limitSearchUser++;
            delete msg;
            /*Submit *submitJob = check_and_cast<Submit *>(msg);
            jobVector.push_back(submitJob->getWorkflow().userJobs);
            queueableJob = queueableJob + submitJob->getWorkflow().userJobs.size();
            userVector.push_back(*(submitJob->getWorkflow().userJobs[0].user));
            delete submitJob;*/
            /*Dispatch *submitJob = check_and_cast<Dispatch *>(msg);
            jobVector.push_back(submitJob->getJob());
            queueableJob++;*/
        }
    }

}

void Database::finish() {
    //jobVector.clear();
    GenerateJob::getInstance().clearVector();
    balancedVector.clear();
}

void Database::refreshDisplay() const{
    cCanvas *canvas = getSystemModule()->getCanvas();
    canvas->setAnimationSpeed(1.5, this);
}

/* �Ƶ{�޿� Weighted, Balanced
 * Weighted:A weighted system that takes priority, submission time, number of
 * rendering tasks, and number of job errors into account, but does
 * not take pools into account.
 * Balanced: Job order will be balanced so that each job has the same
 * number of Workers rendering them at a time.
 * */
User& Database::findDispatchUser(){
    int index = 0;
    int max = -1000;
    int maxIndex = 0;
    for (auto it = userVector.begin(); it != userVector.end(); ++it){
        if(index==limitSearchUser){
            break;
        }
        if(!isAllJobFinisd((*it).userIndex)){
            EV<<"Weight: "<<(*it).userWeight<<"\n";
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
        if(index==limitSearchUser){
            break;
        }
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

Job* Database::findDispatchJob(User *user){
    // TODO:�᭱�[�JworkFlow
    int index = 0;
    for (auto it = jobVector[(*user).userIndex].begin(); it != jobVector[(*user).userIndex].end(); ++it){
        if((!(*it).isJobFinish) && ((*it).finishFrame+(*it).renderingFrame<(*it).totalFrame)){
            index = (*it).jobIndex;
            return &(jobVector[(*user).userIndex].at(index));
        }
    }

    return nullptr;
}

void Database::dispatchJob(User* user, Job* job){
    // ��suser���A
    if(!job->isActivate){
        user->renderingJob = user->renderingJob + 1;
        job->isActivate = true;
    }
    user->userRenderingFrame = user->userRenderingFrame+1;
    user->userWeight = (user->priority * PW)+(user->userErrorFrame * EW)+(0 * SW)+((user->userRenderingFrame - RB) * RW);

    // ��sjob���A
    job->renderingFrame = job->renderingFrame + 1;

    // �s�W�@��Dispatch�T��
    Dispatch *dispatchJob = new Dispatch("dispatchJob");
    dispatchJob->setKind(WorkerState::Dispatch_JOB);
    dispatchJob->setJob(*job);

    // �����B�z�ШD�ɶ�
    scheduleAt(simTime()+0.5, dispatchJob);
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

bool Database::isAllJobFinisd(int userIndex){
    for (auto it = jobVector[userIndex].begin(); it != jobVector[userIndex].end(); ++it){
        if(!(*it).isJobFinish){
            if((*it).finishFrame+(*it).renderingFrame<(*it).totalFrame){
                return false;
            }
        }
    }
    return true;
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
