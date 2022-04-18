/*
 * generate_jobB.h
 *
 *  Created on: 2022¦~4¤ë18¤é
 *      Author: FoSky
 */

#ifndef GENERATE_JOBB_H_
#define GENERATE_JOBB_H_
#include <omnetpp.h>
#include <queue>
#include <vector>
#include "user.h"
#include "job.h"

class GenerateJobB{
public:
    static GenerateJobB& getInstance();
    std::vector<User>& getAllUser();
    std::vector<std::vector<Job>>& getAllJob();
    void clearVector();
private:
    std::queue<std::vector<Job>> jobQueue;
    std::queue<std::string> colorQueue;
    std::vector<User> userVector;
    std::vector<std::vector<Job>> jobVector;
    int PW = 1;
    int EW = 0;
    int SW = 0;
    int RB = 0;
    int RW = -1;
    int jobVectorIndex = 0;
    GenerateJobB();
    void generateColor();
};

#endif /* GENERATE_JOBB_H_ */
