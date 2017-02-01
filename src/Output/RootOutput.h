/***************************************************************************//**
********************************************************************************
**
** @file RootOutput.h
** @author James Till Matta
** @date 31 Jan, 2017
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition of the system for root output
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_OUTPUT_ROOTOUTPUT_H
#define ORCHIDREADER_SRC_OUTPUT_ROOTOUTPUT_H
// includes for C system headers
// includes for C++ system headers
#include<string>
// includes from other libraries
#include"root/TFile.h"
#include"root/TTree.h"
#include"root/TH1F.h"
#include"root/TH2F.h"
// includes from ORCHIDReader
#include"Output/OutputInterface.h"
#include"Config/DetData.h"
#include"Events/OrchidSlowControlsEvent.h"
#include"Events/DppPsdIntegralEvent.h"

namespace Output
{

struct BatchTreeData
{
    BatchTreeData(int numDet):numDetectors(numDet) {}
    ~BatchTreeData(){}
    void clearRates();
    void setRates();

    unsigned long long startTime;
    unsigned long long stopTime;
    unsigned long long centerTime;
    double runTime;
    float rouchCorrection;
    float avgChanVolt;
    float avgChanCurrent;
    float avgHVChanTemp;
    int numDetectors;
    unsigned long long rawCounts[32];
    double rawRates[32];
};

class RootOutput : public OutputInterface
{
public:
    RootOutput(const std::string& outFileName, InputParser::DetData* dData);
    virtual ~RootOutput();
    
    virtual void slowControlsEvent(const Events::OrchidSlowControlsEvent& event);
    virtual void dppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event);
    virtual void done();
    
private:
    //private member functions
    bool closeRun();
    bool initRun();
    
    //member data
    //data output members
    TFile* outfile;
    //2D Histograms
    TH2F** detRun2DHists;
    TH2F** detSum2DHists;
    //RunNum vs En projection are calculated when done() is called
    //1D Histograms per run
    TH1F** enProjWithCutoff;
    TH1F** enProjWithoutCutoff;
    TH1F** psdProjWithCutoff;
    TH1F** psdProjWithoutCutoff;
    //1D Histograms sum
    TH1F** enProjWithCutoffSum;
    TH1F** enProjWithoutCutoffSum;
    TH1F** psdProjWithCutoffSum;
    TH1F** psdProjWithoutCutoffSum;
    TH1F** eventTimeHistsSum;
    //Tree pointer and struct
    TTree* batchTree;
    BatchTreeData treeData;
    
    //book keeping
    int numDetectors;
    unsigned long long runStartEpoch = 0;
    unsigned long long runEndEpochTarget = 600000000;
    unsigned long long runStartTimeStamp = 0;
    unsigned runNumber = 0;
    
    
    InputParser::DetData* detData;
};

}

#endif //ORCHIDREADER_SRC_OUTPUT_ROOTOUTPUT_H
