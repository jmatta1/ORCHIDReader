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
// includes from ORCHIDReader
#include"Output/OutputInterface.h"

namespace InputParser
{
    class ConfigData;
    class DetData;
}

class TFile;
class TTree;
class TH1D;
class TH2F;

namespace Output
{

struct BatchTreeData
{
    BatchTreeData(int numDet):numDetectors(numDet)
    {
        detNum = new int[numDet];
        avgChanVolt = new float[numDet];
        avgChanCurrent = new float[numDet];
        avgHVChanTemp = new float[numDet];
        rawCounts = new unsigned long long[numDet];
        rawRates = new double[numDet];
    }
    ~BatchTreeData()
    {
        delete[] detNum;
        delete[] avgChanVolt;
        delete[] avgChanCurrent;
        delete[] avgHVChanTemp;
        delete[] rawCounts;
        delete[] rawRates;
    }

    int runNumber;
    unsigned long long startTime;
    unsigned long long stopTime;
    unsigned long long centerTime;
    double runTime;
    int numDetectors;
    int* detNum;
    float* avgChanVolt;
    float* avgChanCurrent;
    float* avgHVChanTemp;
    unsigned long long* rawCounts;
    double* rawRates;
};

class RootOutput : public OutputInterface
{
public:
    RootOutput(InputParser::ConfigData* cData, InputParser::DetData* dData);
    virtual ~RootOutput();
    
    virtual void slowControlsEvent(const Events::OrchidSlowControlsEvent& event) final;
    virtual void dppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event) final;
    
    virtual void newRun(int runNum, unsigned long long startT) final;
    virtual void endRun(const RunData& runData) final;
    virtual void done() final;
    
private:
    //private member functions
    void prepTree();
    void initRun();
    void initSums();
    void closeSums();
    void constructTimeSeriesSpectra();
    
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
    bool* firstDetEventOfRun;
    unsigned long long* runStartTimeStamp;
    unsigned long long* lastTimeStamp;
    int numTimeBin;
    float maxTimeEdge;
    int runNumber;
    unsigned long long startTime;
    //config data
    InputParser::ConfigData* confData;
    InputParser::DetData* detData;
};

}

#endif //ORCHIDREADER_SRC_OUTPUT_ROOTOUTPUT_H
