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
#include<sstream>
// includes from other libraries
#include"root/TFile.h"
#include"root/TTree.h"
#include"root/TH1F.h"
#include"root/TH2F.h"
// includes from ORCHIDReader
#include"Output/OutputInterface.h"
#include"Config/DetData.h"
#include"Config/ConfigData.h"

namespace Output
{

struct BatchTreeData
{
    BatchTreeData(int numDet):numDetectors(numDet) {}
    ~BatchTreeData(){}
    void clearData();
    void setRates();
    void setAverages(int numEntries);

    int runNumber;
    unsigned long long startTime;
    unsigned long long stopTime;
    unsigned long long centerTime;
    double runTime;
    int numDetectors;
    float roughCorrection[32];
    int detNum[32];
    float avgChanVolt[32];
    float avgChanCurrent[32];
    float avgHVChanTemp[32];
    unsigned long long rawCounts[32];
    double rawRates[32];
};

class RootOutput // : public OutputInterface
{
public:
    RootOutput(InputParser::ConfigData* cData, InputParser::DetData* dData);
    virtual ~RootOutput();
    
    void slowControlsEvent(const Events::OrchidSlowControlsEvent& event);
    void dppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event);
    void inputFileSwitch(const Events::InputFileSwapEvent& event);
    void done();
    
private:
    //private member functions
    void prepTree();
    void closeRun();
    void prepTreeEntry();
    void doRoughDtCorrections();
    void initRun();
    
    //member data
    //data output members
    TFile* outfile;
    //2D Histograms
    TH2F** detRun2DHists;
    TH2F** detSum2DHists;
    //RunNum vs En projection are calculated when done() is called
    //1D Histograms per run
    TH1D** enProjWithCutoff;
    TH1D** enProjWithoutCutoff;
    TH1D** psdProjWithCutoff;
    TH1D** psdProjWithoutCutoff;
    TH1D** eventTimeHists;
    //1D Histograms sum
    TH1D** enProjWithCutoffSum;
    TH1D** enProjWithoutCutoffSum;
    TH1D** psdProjWithCutoffSum;
    TH1D** psdProjWithoutCutoffSum;
    //Tree pointer and struct
    TTree* batchTree;
    BatchTreeData treeData;
    
    //book keeping
    int numDetectors;
    unsigned long long runStartEpoch = 0;
    unsigned long long lastEpoch = 0;
    unsigned long long runEndEpochTarget = 600000000;
    unsigned long long runStartTimeStamp[32];
    unsigned long long lastTimeStamp[32];
    int runNumber = -1;
    int numSlowCtrls;
    bool firstEvent = false;
    int numTimeBin;
    float maxTimeEdge;
    //config data
    InputParser::ConfigData* confData;
    InputParser::DetData* detData;
};

}

#endif //ORCHIDREADER_SRC_OUTPUT_ROOTOUTPUT_H
