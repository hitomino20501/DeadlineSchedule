#include <omnetpp.h>
#include <queue>
#include <random>
#include <algorithm>
#include "generate_job.h"
#include "user.h"
#include "job.h"
#define totalUser 4
#define eachUserJob 10

using namespace omnetpp;

GenerateJob::GenerateJob(){
    generateColor();

    struct Job job;
    struct User user;
    userVector.reserve(totalUser);

    // ��������1 150slave 16user 20eachUserJob 25task
    /*int priorityGroup[3]={25, 20, 11};
    int userIndex = 0;
    for(int i=0;i<totalUser;i++){
        if(i<6){
            user.name = "User"+std::to_string(i);
            user.priority = priorityGroup[0];
            user.userIndex = userIndex;
            user.userWeight = (user.priority * PW)+(user.userErrorFrame * EW)+(0 * SW)+((user.userRenderingFrame - RB) * RW);
            user.userColor = colorQueue.front();
            userVector.push_back(user);
            userIndex++;
            colorQueue.push(colorQueue.front());
            colorQueue.pop();
        }
        else if(i>=6 && i<11){
            user.name = "User"+std::to_string(i);
            user.priority = priorityGroup[1];
            user.userIndex = userIndex;
            user.userWeight = (user.priority * PW)+(user.userErrorFrame * EW)+(0 * SW)+((user.userRenderingFrame - RB) * RW);
            user.userColor = colorQueue.front();
            userVector.push_back(user);
            userIndex++;
            colorQueue.push(colorQueue.front());
            colorQueue.pop();
        }
        else{
            user.name = "User"+std::to_string(i);
            user.priority = priorityGroup[2];
            user.userIndex = userIndex;
            user.userWeight = (user.priority * PW)+(user.userErrorFrame * EW)+(0 * SW)+((user.userRenderingFrame - RB) * RW);
            user.userColor = colorQueue.front();
            userVector.push_back(user);
            userIndex++;
            colorQueue.push(colorQueue.front());
            colorQueue.pop();
        }
    }

    std::random_shuffle(userVector.begin(), userVector.end());
    userIndex = 0;
    for (auto it = userVector.begin(); it != userVector.end(); ++it){
        (*it).userIndex = userIndex;
        userIndex++;
    }*/

    // ��������2 17slave 4user 3eachUserJob 25task
    int pr[totalUser]={20, 15, 11, 10};
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
    temJob.reserve(eachUserJob);
    jobVector.reserve(totalUser*eachUserJob);

    int jobIndex = 0;
    for(int i=0;i<totalUser;i++){
        EV<<userVector[i].name<<":"<<userVector[i].priority<<"\n";
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

    // �إ�workFlow
    //hierarchy:[1, 4, 2, 3]
    //workflowHierarchy:[[0], [1, 2, 3, 4], [5, 6], [7, 8, 9]]
    hierarchy = {1, 4, 2, 3};
    std::vector<int> temHierarchy;
    jobIndex = 0;
    int end = 0;
    for(int i=0;i<hierarchy.size();i++){
        end = jobIndex+hierarchy[i];
        for(int j=jobIndex;j<end;j++){
            temHierarchy.push_back(jobIndex);
            jobIndex++;
        }
        workflowHierarchy.push_back(temHierarchy);
        temHierarchy.clear();
    }
    /*for (auto it = workflowHierarchy.begin(); it != workflowHierarchy.end(); ++it){
        EV<<"Size:"<<it->size()<<"\n";
    }*/
    // ����adj
    for(int i=0;i<hierarchy.size();i++){
        if(i==0){
            // �Ĥ@�h
            for(int j=0;j<hierarchy[i];j++){
                for(int k=0;k<10;k++){
                    temHierarchy.push_back(0);
                }
                adj.push_back(temHierarchy);
                temHierarchy.clear();
            }
        }else{
            for(int j=0;j<hierarchy[i];j++){
                for(int k=0;k<10;k++){
                    if(std::binary_search(workflowHierarchy[i-1].begin(), workflowHierarchy[i-1].end(), k)){
                        temHierarchy.push_back(1);
                    }else{
                        temHierarchy.push_back(0);
                    }
                }
                adj.push_back(temHierarchy);
                temHierarchy.clear();
            }
        }
    }
    /*for (auto it = adj.begin(); it != adj.end(); ++it){
        //EV<<"Size:"<<it->size()<<"\n";
        for (auto it2 = it->begin(); it2 != it->end(); ++it2){
            EV<<*it2<<" ";
        }
        EV<<"\n";
    }*/
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