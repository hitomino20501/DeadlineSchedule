/*
 * generate_job.h
 *
 *  Created on: 2021¦~10¤ë25¤é
 *      Author: FoSky
 */

#ifndef GENERATE_JOB_H_
#define GENERATE_JOB_H_
#include <omnetpp.h>
#include <queue>
#include <vector>
#include "user.h"
#include "job.h"

class GenerateJob{
public:
    static GenerateJob& getInstance();
    std::vector<User>& getAllUser();
    std::vector<std::vector<Job>>& getAllJob();
    std::vector<int>& getHierarchy();
    std::vector<std::vector<int>>& getWorkflowHierarchy();
    std::vector<std::vector<int>>& getUserWorkflow();
    void clearVector();
    std::vector<int>& getWorkerState();
private:
    std::queue<std::vector<Job>> jobQueue;
    std::queue<std::string> colorQueue;
    std::vector<User> userVector;
    std::vector<std::vector<Job>> jobVector;
    std::vector<int> hierarchy;
    std::vector<std::vector<int>> workflowHierarchy;
    std::vector<std::vector<int>> adj;
    std::vector<int> workerState;
    int PW = 1;
    int EW = 0;
    int SW = 0;
    int RB = 0;
    int RW = -1;
    int jobVectorIndex = 0;
    GenerateJob();
    void generateColor();
};

#endif /* GENERATE_JOB_H_ */
