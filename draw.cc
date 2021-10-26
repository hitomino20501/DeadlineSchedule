#include <string.h>
#include <omnetpp.h>
#include <math.h>
#include <vector>
#include <numeric>
#include "user.h"
#include "job.h"
#include "generate_job.h"

using namespace omnetpp;

class Draw : public cSimpleModule{
    private:
        std::vector<int>& hierarchy = GenerateJob::getInstance().getHierarchy();
        std::vector<std::vector<int>>& adj = GenerateJob::getInstance().getUserWorkflow();
    protected:
    // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Draw);

void Draw::initialize(){
    cModuleType *moduleType = cModuleType::get("Node");

    int jobIndex = 0;
    int totalNode = accumulate(hierarchy.begin(), hierarchy.end(), 0);
    int baseX = 750;
    int baseY = 150;
    int offsetX = 0;
    int offsetY = 0;
    /*cModule *module = moduleType->create("node", getParentModule(), 1, 0);
    module->getDisplayString().setTagArg("p", 0, 750);
    module->getDisplayString().setTagArg("p", 1, 200);
    module->finalizeParameters();
    module->buildInside();
    module->scheduleStart(simTime());*/
    jobIndex = 0;
    int end = 0;
    int count = 0;
    for(int i=0;i<hierarchy.size();i++){
        end = jobIndex+hierarchy[i];
        count = 0;
        if(hierarchy[i]%2==1){
            offsetX = baseX - floor(hierarchy[i]/2)*50;
            offsetY = baseY + i*70;
        }else{
            offsetX = baseX - (floor(hierarchy[i]/2)*50) + 25;
            offsetY = baseY + i*70;
        }
        for(int j=jobIndex;j<end;j++){
            cModule *module = moduleType->create("node", getParentModule(), totalNode, jobIndex);
            module->getDisplayString().setTagArg("p", 0, offsetX + count*50);
            module->getDisplayString().setTagArg("p", 1, offsetY);
            if(i==0){
                module->setGateSize("in", hierarchy[i+1]+1);
                module->setGateSize("out", hierarchy[i+1]+1);
            }else if(i==hierarchy.size()-1){
                module->setGateSize("in", hierarchy[i-1]+1);
                module->setGateSize("out", hierarchy[i-1]+1);
            }else{
                module->setGateSize("in", hierarchy[i+1]+hierarchy[i-1]+1);
                module->setGateSize("out", hierarchy[i+1]+hierarchy[i-1]+1);
            }
            //module->setGateSize("in", 2);
            //module->setGateSize("out", 2);
            module->finalizeParameters();
            module->buildInside();
            module->scheduleStart(simTime());
            jobIndex++;
            count++;
        }
    }
    // 連結各節點
    std::vector<int> port;
    for(int i=0;i<adj.size();i++){
        port.push_back(1);
    }
    cModule *node;
    cModule *dependencyNode;
    for(int i=0;i<adj.size();i++){
        for(int j=0;j<adj[i].size();j++){
            if(adj[i][j]==1){
                node = getParentModule()->getSubmodule("node", i);
                dependencyNode = getParentModule()->getSubmodule("node", j);
                node->gate("out", port[i])->connectTo(dependencyNode->gate("in", port[j]));
                port[i] = port[i] + 1;
                port[j] = port[j] + 1;
            }
        }
    }
}

void Draw::handleMessage(cMessage *msg){}
