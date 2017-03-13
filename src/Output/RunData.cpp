/***************************************************************************//**
********************************************************************************
**
** @file RunData.cpp
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
** @details Holds the implementation of the RunData class
**
********************************************************************************
*******************************************************************************/
#include"Output/RunData.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHIDReader

namespace Output
{

RunData::RunData(int numDet):numDetectors(numDet)
{
    detNum = new int[numDetectors];
    avgChanVolt = new float[numDetectors];
    avgChanCurrent = new float[numDetectors];
    avgHVChanTemp = new float[numDetectors];
    rawCounts = new unsigned long long[numDetectors];
    rawRates = new double[numDetectors];
}

RunData::~RunData()
{
    delete[] detNum;
    delete[] avgChanVolt;
    delete[] avgChanCurrent;
    delete[] avgHVChanTemp;
    delete[] rawCounts;
    delete[] rawRates;
}

void RunData::resetCounters()
{
    for(int i=0; i<numDetectors; ++i)
    {
        avgChanVolt[i] = 0.0f;
        avgChanCurrent[i] = 0.0f;
        avgHVChanTemp[i] = 0.0f;
        rawCounts[i] = 0;
        rawRates[i] = 0.0;
    }
}

void RunData::calcRates()
{
    for(int i=0; i<numDetectors; ++i)
    {
        rawRates[i] = static_cast<double>(rawCounts[i])/runTime;
    }
}

void RunData::calcAvgs(int numScEvents)
{
    float numEvents = static_cast<float>(numScEvents);
    for(int i=0; i<numDetectors; ++i)
    {
        avgChanVolt[i] /= numEvents;
        avgChanCurrent[i] /= numEvents;
        avgHVChanTemp[i] /= numEvents;
    }
}

void RunData::closeRun(int numSlowCtrlsEvents)
{
    centerTime = ((startTime + stopTime)/2);
    runTime = (static_cast<double>(stopTime - startTime)/1000000.0);
    calcRates();
    calcAvgs(numSlowCtrlsEvents);
}

}
