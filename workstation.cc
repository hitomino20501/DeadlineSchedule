#include <string.h>
#include <omnetpp.h>
#include <queue>
#include <random>
#include <algorithm>
#include "user.h"
#include "job.h"
#include "state.h"
#include "dispatch_m.h"
#include "generate_job.h"
#define totalUser 4
#define eachUserJob 3

using namespace omnetpp;

class Workstation : public cSimpleModule{
    private:
        //std::queue<std::vector<Job>> jobQueue;
        //std::queue<std::string> colorQueue;
        std::vector<User>& userVector = GenerateJob::getInstance().getAllUser();
        /*int PW = 1;
        int EW = 0;
        int SW = 0;
        int RB = 0;
        int RW = -1;*/
        int countUser = 0;
        //void generateColor();
        //void generateJob(User &user);
    protected:
    // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Workstation);

void Workstation::initialize(){
    // 改成singleton模式
    /*generateColor();

    struct Job job;
    struct User user;
    userVector.reserve(totalUser);
    int pr[totalUser]={10, 15, 11, 20};
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
            userVector[i].totalJob = userVector[i].totalJob+1;
        }
        jobQueue.push(temJob);
        jobIndex = 0;
        temJob.clear();
    }*/

    // 手動產生user job
    // 產生4個user
    /*int pr[totalUser]={15, 11, 10, 20};
    //int userPriority = 10;
    User userList[totalUser];
    for(int i=0;i<totalUser;i++){
        userList[i].name = "User"+std::to_string(i);
        userList[i].priority = pr[i];
        //userList[i].priority = userPriority;
        //userPriority = userPriority+5;
    }

    generateJob(userList[0]);
    generateJob(userList[1]);
    generateJob(userList[2]);
    generateJob(userList[3]);*/

    // 實驗環境:設定X台slave Y個user同時執行的時候剛好拿滿X台
    /*int priorityGroup[3]={25, 20, 11};
    std::vector<User> userVector;
    userVector.reserve(16);
    User user;
    for(int i=0;i<16;i++){
        if(i<6){
            user.name = "User"+std::to_string(i);
            user.priority = priorityGroup[0];
            userVector.push_back(user);
        }
        else if(i>=6 && i<11){
            user.name = "User"+std::to_string(i);
            user.priority = priorityGroup[1];
            userVector.push_back(user);
        }
        else{
            user.name = "User"+std::to_string(i);
            user.priority = priorityGroup[2];
            userVector.push_back(user);
        }
    }

    std::random_shuffle(userVector.begin(), userVector.end());

    for (auto it = userVector.begin(); it != userVector.end(); ++it){
        EV<<(*it).name<<":"<<(*it).priority<<"\n";
        generateJob(*it);
    }

    userVector.clear();*/

    Dispatch *msg = new Dispatch("hello");
    msg->setKind(WorkerState::SUBMIT_JOB);
    scheduleAt(1.0, msg);

    // 產生job
    /*int jobIndex = 0;
    for(int i=0;i<totalUser;i++){
        for(int j=0;j<eachUserJob;j++){
            job.user = userList[i];
            job.jobIndex = jobIndex;
            job.weight = (job.user.priority * PW)+(job.errorFrame * EW)+(0 * SW)+((job.renderingFrame - RB) * RW);
            job.jobColor = colorQueue.front();
            jobIndex++;
            jobQueue.push(job);
        }
        colorQueue.push(colorQueue.front());
        colorQueue.pop();
    }*/
}

void Workstation::handleMessage(cMessage *msg){
    if(msg->isSelfMessage()){
        EV<<"Workstation got a message from itself: "<<simTime()<<"\n";
        int msgKind = msg->getKind();
        if(msgKind==WorkerState::SUBMIT_JOB){
            EV<<"Workstation submit a job: "<<simTime()<<"\n";
            /*struct Workflow workflow;
            workflow.user = &userVector[countUser];
            Submit *submitJob = new Submit("submitJob");
            submitJob->setKind(WorkerState::SUBMIT_JOB);
            submitJob->setSchedulingPriority(1);
            submitJob->setWorkflow(workflow);
            send(submitJob, "out");
            countUser++;*/
            struct Job job;
            if(countUser<userVector.size()){
                job.user = &userVector[countUser];
            }
            else if(countUser==userVector.size()){
                job.user = &userVector[0];
            }
            Dispatch *submitJob = new Dispatch("submitJob");
            submitJob->setKind(WorkerState::SUBMIT_JOB);
            submitJob->setSchedulingPriority(1);
            submitJob->setJob(job);
            send(submitJob, "out");
            countUser++;
            /*for (auto it = userVector.begin(); it != userVector.end(); ++it){
                EV<<"{";
                EV<<"'simTime':"<<simTime()<<",";
                EV<<"'userName':'"<<(*it).name<<"',";
                EV<<"'renderingFrame':"<<(*it).userRenderingFrame<<",";
                EV<<"'weight':"<<(*it).userWeight<<",";
                EV<<"'total':"<<(*it).userFinishFrame;
                EV<<"}\n";
            }*/
            /*struct Workflow workflow;
            workflow.userJobs = jobQueue.front();
            jobQueue.pop();
            Submit *submitJob = new Submit("submitJob");
            submitJob->setKind(WorkerState::SUBMIT_JOB);
            submitJob->setSchedulingPriority(1);
            submitJob->setWorkflow(workflow);
            send(submitJob, "out");*/
            /*job = jobQueue.front();
            jobQueue.pop();
            Dispatch *submitJob = new Dispatch("submitJob");
            submitJob->setKind(WorkerState::SUBMIT_JOB);
            msg->setSchedulingPriority(1);
            submitJob->setJob(job);
            send(submitJob, "out");*/
        }
        if(countUser<userVector.size()){
            msg->setSchedulingPriority(1);
            scheduleAt(simTime()+5.0, msg);
        }
        else if(countUser==userVector.size()){
            msg->setSchedulingPriority(1);
            scheduleAt(simTime()+100.0, msg);
        }
        else{
            cancelAndDelete(msg);
        }
        /*if(!jobQueue.empty()){
            msg->setSchedulingPriority(1);
            scheduleAt(simTime()+1.0, msg);
        }else{
            cancelAndDelete(msg);
        }*/
        // 延遲最後一個工作
        /*if(!jobQueue.empty()){
            if(jobQueue.size()==1){
                msg->setSchedulingPriority(1);
                scheduleAt(simTime()+15.0, msg);
            }else{
                msg->setSchedulingPriority(1);
                scheduleAt(simTime()+1.0, msg);
            }
        }else{
            cancelAndDelete(msg);
        }*/
    }
}

/*void Workstation::generateJob(User &user){
    job.user = user;
    job.jobIndex = jobIndex;
    job.weight = (job.user.priority * PW)+(job.errorFrame * EW)+(0 * SW)+((job.renderingFrame - RB) * RW);
    job.jobColor = colorQueue.front();
    jobQueue.push(job);
    jobIndex++;
    colorQueue.push(colorQueue.front());
    colorQueue.pop();
}*/

/*void Workstation::generateColor(){
    colorQueue.push("darkorange");
    colorQueue.push("magenta");
    colorQueue.push("lightskyblue");
    colorQueue.push("blue");
    colorQueue.push("aqua");
    colorQueue.push("darkgreen");
    colorQueue.push("blueviolet");
    colorQueue.push("brown");
    colorQueue.push("darkcyan");
    colorQueue.push("deeppink");
    colorQueue.push("gold");
    colorQueue.push("hotpink");
    colorQueue.push("khaki");
    colorQueue.push("aquamarine");
    colorQueue.push("yellow");
    colorQueue.push("yellowgreen");
    colorQueue.push("violet");
    colorQueue.push("tomato");
    colorQueue.push("tan");
    colorQueue.push("indianred");
    colorQueue.push("sandybrown");
    colorQueue.push("red");
    colorQueue.push("purple");
    colorQueue.push("mediumblue");
    colorQueue.push("peru");
    colorQueue.push("pink");
    colorQueue.push("olivedrab");
    colorQueue.push("orange");
    colorQueue.push("navy");
    colorQueue.push("indigo");
}*/
