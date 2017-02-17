/***************************************************************************//**
********************************************************************************
**
** @file RunData.h
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
** @details Holds the definition of the RunData class
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_OUTPUT_RUNDATA_H
#define ORCHIDREADER_SRC_OUTPUT_RUNDATA_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHIDReader
namespace Output
{

struct RunData
{
    RunData(int numDet);
    ~RunData();
    void closeRun(int numSlowCtrlsEvents);
    void resetCounters();
    void calcRates();
    void calcAvgs(int numScEvents);
    
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

}
#endif //ORCHIDREADER_SRC_OUTPUT_RUNDATA_H
