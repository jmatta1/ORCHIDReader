/***************************************************************************//**
********************************************************************************
**
** @file CsvOutput.h
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
** @details Holds the implementation of the class which outputs batch and run
** data into csv files
**
********************************************************************************
*******************************************************************************/
#include"CsvOutput.h"
// includes for C system headers
// includes for C++ system headers

// includes from other libraries
// includes from ORCHIDReader
#include"Events/OrchidSlowControlsEvent.h"
#include"Events/DppPsdIntegralEvent.h"
#include"Output/RunData.h"
#include"Config/DetData.h"
#include"Config/ConfigData.h"

namespace Output
{

//StartEpoch, StartDate, StartTime, EndEpoch, EndData, EndTime, NumRuns
CsvOutput::CsvOutput(InputParser::ConfigData* cData, InputParser::DetData* dData)
{
    int numDet = dData->detectorNum.size();
    //open the batch data file
    batchCsv.open(cData->batchMetaDataPath.c_str());
    //write what we can for the batch data file
    batchCsv << "Number of Detectors, Array X_Pos, Array Y_Pos, ";
    batchCsv << "Histogram Integration Time, RootOutput, Run Data Csv Path, ";
    batchCsv << "DetDataCsv, StartEpoch, StartDate, StartTime, EndEpoch, ";
    batchCsv << "EndData, EndTime, NumRuns" << std::endl;
    batchCsv << numDet << ", " << cData->arrayXPos << ", ";
    batchCsv << cData->arrayYPos << ", " << cData->histIntegrationTime << ", ";
    batchCsv << cData->rootFilePath << ", " << cData->runCsvPath << ", ";
    batchCsv << cData->detMetaDataPath << ", ";

    //open, write, and close the full detector data file
    std::ofstream detData(cData->detMetaDataPath.c_str());
    detData << "Det Number, Digitizer Board Number, Digitizer Channel Number, ";
    detData << "MPOD Board Num, MPOD Channel Num, Detector Type, X Offset, ";
    detData << "X Position, Y Offset, Y Position, Z Offset, Z Position" << std::endl;
    for(int i=0; i<numDet; ++i)
    {
        float detXPos = (dData->detXOffset[i] + cData->arrayXPos);
        float detYPos = (dData->detYOffset[i] + cData->arrayYPos);
        float detZPos = dData->detXOffset[i];
        detData << dData->detectorNum[i] << ", " << dData->digiBoardNum[i] << ", ";
        detData << dData->digiChanNum[i] << ", " << dData->mpodBoardNum[i] << ", ";
        detData << dData->mpodChanNum[i] << ", " << dData->detType[i] << ", ";
        detData << dData->detXOffset[i]  << ", " << detXPos << ", ";
        detData << dData->detYOffset[i]  << ", " << detYPos << ", ";
        detData << dData->detZOffset[i]  << ", " << detZPos << std::endl;
    }
    detData.close();
    
    //open the run data file
    runDataFile.open(cData->runCsvPath.c_str());
    
}
/*
    //create the tree
    batchTree = new TTree("BatchTree","Per Run Data");
    //now create the scalar branches
    batchTree->Branch("runN", &treeData.runNumber,"runN/I");
    batchTree->Branch("startT", &treeData.startTime,"startT/l");
    batchTree->Branch("stopT", &treeData.stopTime,"stopT/l");
    batchTree->Branch("centerT", &treeData.centerTime,"centerT/l");
    batchTree->Branch("runT", &treeData.runTime,"runT/D");
    //now create the vector branches
    std::ostringstream leafNamer;
    leafNamer << "avgChanVolt["<<numDetectors<<"]/F";
    batchTree->Branch("avgChanVolt", treeData.avgChanVolt, leafNamer.str().c_str());
    leafNamer.str("");
    leafNamer.clear();
    leafNamer << "avgChanCurr["<<numDetectors<<"]/F";
    batchTree->Branch("avgChanCurr", treeData.avgChanCurrent, leafNamer.str().c_str());
    leafNamer.str("");
    leafNamer.clear();
    leafNamer << "avgHVChanTemp["<<numDetectors<<"]/F";
    batchTree->Branch("avgHVChanTemp", treeData.avgHVChanTemp, leafNamer.str().c_str());
    leafNamer.str("");
    leafNamer.clear();
    leafNamer << "rawCounts["<<numDetectors<<"]/l";
    batchTree->Branch("rawCounts", treeData.rawCounts, leafNamer.str().c_str());
    leafNamer.str("");
    leafNamer.clear();
    leafNamer << "rawRates["<<numDetectors<<"]/D";
    batchTree->Branch("rawRates", treeData.rawRates, leafNamer.str().c_str());
*/

CsvOutput::~CsvOutput()
{
    
}

// processing for individual events
void CsvOutput::slowControlsEvent(const Events::OrchidSlowControlsEvent& event)
{
    
}

void CsvOutput::dppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event)
{
    
}

//larger scale processing
void CsvOutput::newRun(int runNum, unsigned long long startT)
{
    
}

void CsvOutput::endRun(RunData* runData)
{
    
}

void CsvOutput::done()
{
    
}

}
