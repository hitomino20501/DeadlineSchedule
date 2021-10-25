#include <omnetpp.h>
#include <queue>
#include <random>
#include <algorithm>
#include "generate_job.h"
#include "user.h"
#include "job.h"
#define totalUser 4
#define eachUserJob 3


GenerateJob::GenerateJob(){
    generateColor();

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

    //std::random_shuffle(userVector.begin(), userVector.end());

    std::vector<Job> temJob;
    temJob.reserve(eachUserJob);
    jobVector.reserve(100);

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
        jobVector.push_back(temJob);
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

void GenerateJob::clearVector(){
    userVector.clear();
    jobVector.clear();
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
