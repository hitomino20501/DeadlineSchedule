/*
 * job.h
 *
 *  Created on: 2021¦~10¤ë13¤é
 *      Author: FoSky
 */

#ifndef JOB_H_
#define JOB_H_
#include "task.h"

struct Job{
    struct User* user;
    int jobIndex = 0;
    int totalFrame = 100;
    std::vector<Task> taskVector;
    int renderingFrame = 0;
    int errorFrame = 0;
    int finishFrame = 0;
    int weight = 0;
    int jobVectorIndex = 0;
    bool isJobFinish = false;
    bool isActivate = false;
    std::string jobColor;
    omnetpp::simtime_t startTime = 0;
    omnetpp::simtime_t finisdTime = 0;
};

#endif /* JOB_H_ */
