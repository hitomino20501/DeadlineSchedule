#include <omnetpp.h>
#include <queue>
#include <random>
#include <algorithm>
#include "generate_job.h"
#include "user.h"
#include "job.h"
#include "task.h"
#define totalUser 4
#define eachUserJob 1
#define totalFarm 2
#define totalSlave 100

using namespace omnetpp;

GenerateJob::GenerateJob(){
    generateColor();

    struct Job job;
    struct User user;
    struct Task task;
    userVector.reserve(totalUser);

    farmCredit={0, 0};
    slaveState.resize(totalSlave);
    std::fill(slaveState.begin(), slaveState.end(), -1);

    // πÍ≈Á¿Ùπ“2 17slave 4user 3eachUserJob 25task
    int pr[totalUser]={20, 15, 11, 10};
    int pro[totalUser]={6, 2, 1, 1};
    int userIndex = 0;
    for(int i=0;i<totalUser;i++){
        user.name = "UserA"+std::to_string(i);
        user.priority = pr[i];
        user.proportion = pro[i];
        user.userIndex = userIndex;
        user.userWeight = (user.priority * PW)+(user.userErrorFrame * EW)+(0 * SW)+((user.userRenderingFrame - RB) * RW);
        user.userColor = colorQueue.front();
        userVector.push_back(user);
        userIndex++;
        colorQueue.push(colorQueue.front());
        colorQueue.pop();
    }

    std::vector<Job> temJob;
    //temJob.reserve(eachUserJob);
    //jobVector.reserve(totalUser*eachUserJob);

    int jobIndex = 0;
    for(int i=0;i<totalUser;i++){
        EV<<userVector[i].name<<":"<<userVector[i].priority<<"\n";
        if(i==0){
            for(int j=0;j<2;j++){
                job.farm="A";
                if(j==0){
                    job.totalFrame = 60;
                    job.renderTime = 100.0;
                }
                else if(j==1){
                    job.totalFrame = 100;
                    job.renderTime = 100.0;
                }
                //job.totalFrame = 10;
                job.taskVector.reserve(job.totalFrame);
                job.user = &userVector[i];
                job.jobIndex = jobIndex;
                jobIndex++;
                job.jobVectorIndex = jobVectorIndex;
                for(int k=0;k<job.totalFrame;k++){
                    task.taskIndex = k;
                    job.taskVector.push_back(task);
                }
                temJob.push_back(job);
                job.taskVector.clear();
                //userVector[i].totalJob = userVector[i].totalJob+1;
            }
        }
        else if(i==1){
            for(int j=0;j<eachUserJob;j++){
                job.farm="A";
                job.totalFrame = 20;
                job.renderTime = 40.0;
                //job.totalFrame = 10;
                job.taskVector.reserve(job.totalFrame);
                job.user = &userVector[i];
                job.jobIndex = jobIndex;
                jobIndex++;
                job.jobVectorIndex = jobVectorIndex;
                for(int k=0;k<job.totalFrame;k++){
                    task.taskIndex = k;
                    job.taskVector.push_back(task);
                }
                temJob.push_back(job);
                job.taskVector.clear();
                //userVector[i].totalJob = userVector[i].totalJob+1;
            }
        }
        else if(i==2){
            for(int j=0;j<eachUserJob;j++){
                job.farm="A";
                job.totalFrame = 10;
                job.renderTime = 40.0;
                //job.totalFrame = 10;
                job.taskVector.reserve(job.totalFrame);
                job.user = &userVector[i];
                job.jobIndex = jobIndex;
                jobIndex++;
                job.jobVectorIndex = jobVectorIndex;
                for(int k=0;k<job.totalFrame;k++){
                    task.taskIndex = k;
                    job.taskVector.push_back(task);
                }
                temJob.push_back(job);
                job.taskVector.clear();
                //userVector[i].totalJob = userVector[i].totalJob+1;
            }
        }
        else if(i==3){
            for(int j=0;j<eachUserJob;j++){
                job.farm="A";
                job.totalFrame = 10;
                job.renderTime = 40.0;
                //job.totalFrame = 10;
                job.taskVector.reserve(job.totalFrame);
                job.user = &userVector[i];
                job.jobIndex = jobIndex;
                jobIndex++;
                job.jobVectorIndex = jobVectorIndex;
                for(int k=0;k<job.totalFrame;k++){
                    task.taskIndex = k;
                    job.taskVector.push_back(task);
                }
                temJob.push_back(job);
                job.taskVector.clear();
                //userVector[i].totalJob = userVector[i].totalJob+1;
            }
        }

        jobQueue.push(temJob);
        jobVector.push_back(temJob);
        jobVectorIndex++;
        jobIndex = 0;
        temJob.clear();
    }
}

GenerateJob& GenerateJob::getInstance(){
    static GenerateJob instance;
    return instance;
}

std::vector<User>& GenerateJob::getAllUser(){
    return userVector;
}

std::vector<std::vector<Job>>& GenerateJob::getAllJob(){
    return jobVector;
}

std::vector<int>& GenerateJob::getHierarchy(){
    return hierarchy;
}

std::vector<std::vector<int>>& GenerateJob::getWorkflowHierarchy(){
    return workflowHierarchy;
}

std::vector<std::vector<int>>& GenerateJob::getUserWorkflow(){
    return adj;
}

std::vector<int>& GenerateJob::getFarmCredit(){
    return farmCredit;
}

std::vector<int>& GenerateJob::getSlaveState(){
    return slaveState;
}

void GenerateJob::clearVector(){
    userVector.clear();
    jobVector.clear();
    hierarchy.clear();
    workflowHierarchy.clear();
    adj.clear();
}

void GenerateJob::generateColor(){
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
}
