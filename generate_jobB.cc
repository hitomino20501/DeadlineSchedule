#include <omnetpp.h>
#include <queue>
#include <random>
#include <algorithm>
#include "generate_jobB.h"
#include "user.h"
#include "job.h"
#include "task.h"
#define totalUser 4
#define eachUserJob 1
#define totalFarm 2
#define totalSlave 100

using namespace omnetpp;

GenerateJobB::GenerateJobB(){
    generateColor();

    struct Job job;
    struct User user;
    struct Task task;
    userVector.reserve(totalUser);

    farmCredit={-10, 0};
    slaveState.resize(totalSlave);
    std::fill(slaveState.begin(), slaveState.end(), 0);

    // πÍ≈Á¿Ùπ“2 17slave 4user 3eachUserJob 25task
    int pr[totalUser]={20, 15, 11, 10};
    int pro[totalUser]={6, 2, 1, 1};
    int userIndex = 0;
    for(int i=0;i<totalUser;i++){
        user.name = "User"+std::to_string(i);
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
    temJob.reserve(eachUserJob);
    jobVector.reserve(totalUser*eachUserJob);

    int jobIndex = 0;
    for(int i=0;i<totalUser;i++){
        EV<<userVector[i].name<<":"<<userVector[i].priority<<"\n";
        for(int j=0;j<eachUserJob;j++){
            job.farm="B";
            job.totalFrame = 10;
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
        jobQueue.push(temJob);
        jobVector.push_back(temJob);
        jobVectorIndex++;
        jobIndex = 0;
        temJob.clear();
    }
}

GenerateJobB& GenerateJobB::getInstance(){
    static GenerateJobB instance;
    return instance;
}

std::vector<User>& GenerateJobB::getAllUser(){
    return userVector;
}

std::vector<std::vector<Job>>& GenerateJobB::getAllJob(){
    return jobVector;
}

std::vector<int>& GenerateJobB::getFarmCredit(){
    return farmCredit;
}

std::vector<int>& GenerateJobB::getSlaveState(){
    return slaveState;
}

void GenerateJobB::clearVector(){
    userVector.clear();
    jobVector.clear();
}

void GenerateJobB::generateColor(){
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

