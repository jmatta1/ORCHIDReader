/***************************************************************************//**
********************************************************************************
**
** @file CsvOutput.h
** @author James Till Matta
** @date 10 Mar, 2017
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
#include<ctime>
#include<sstream>
#include<iomanip>
// includes from other libraries
// includes from ORCHIDReader
#include"Events/OrchidSlowControlsEvent.h"
#include"Events/DppPsdIntegralEvent.h"
#include"Output/RunData.h"
#include"Config/DetData.h"
#include"Config/ConfigData.h"

namespace Output
{

CsvOutput::CsvOutput(InputParser::ConfigData* cData, InputParser::DetData* dData)
{
    //get the number of detectors
    int numDet = dData->detectorNum.size();
    //open the batch data file
    batchCsv.open(cData->batchMetaDataPath.c_str());
    //write what we can for the batch data file
    batchCsv << "Number of Detectors, Array X_Pos, Array Y_Pos, ";
    batchCsv << "Histogram Integration Time, RootOutput, Run Data Csv Path, ";
    batchCsv << "DetDataCsv, StartEpoch, Start DateTime, EndEpoch, ";
    batchCsv << "End DateTime, NumRuns" << std::endl;
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
    
    // open the run data file
    runDataFile.open(cData->runCsvPath.c_str());
    runDataFile << "Run Num, Start Epoch (us), Start DateTime, Stop Epoch (us), ";
    runDataFile << "Stop Datetime, Center Epoch (us), Center DateTime, Run Time (us)";
    // now write out the vector data headers
    for(int i=0; i<numDet; ++i)
    {
        int detNum = dData->detectorNum[i];
        runDataFile << ", " << " Avg Voltage Det" << detNum;
        runDataFile << ", " << " Avg Current Det" << detNum;
        runDataFile << ", " << " Avg HV Temp Det" << detNum;
        runDataFile << ", " << " Raw Counts Det" << detNum;
        runDataFile << ", " << " Raw Rate Det" << detNum;
    }
    runDataFile << std::endl;
}

CsvOutput::~CsvOutput()
{
    //nothing to do, the call to done closed the files
}


void CsvOutput::slowControlsEvent(const Events::OrchidSlowControlsEvent& event)
{   //do nothing, this class does not handle individual events
    return;
}

void CsvOutput::dppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event)
{   //do nothing, this class does not handle individual events
    return;
}

void CsvOutput::newRun(int runNum, unsigned long long startT)
{
    if(numRuns==0)
    {//if this is the first run, write the start of batch times
        writeEpochDateTimePairToStream(startT, batchCsv);
    }
    //write the start of run info, namely, number and time
    runDataFile << runNum << ", ";
    writeEpochDateTimePairToStream(startT, runDataFile);
    ++numRuns;
}

void CsvOutput::endRun(const RunData& runData)
{
    //store this end of run so that we have it if this is the last run
    lastEndOfRun = runData.stopTime;
    //first generate and write the stop time of the run
    writeEpochDateTimePairToStream(runData.stopTime, runDataFile);
    //now generate and write the center time stuff
    writeEpochDateTimePairToStream(runData.centerTime, runDataFile);
    //now write the run time in us
    runDataFile << (runData.stopTime - runData.startTime);
    //runData's detector numbers should be in the same order as the header written
    for(int i=0; i<runData.numDetectors; ++i)
    {
        runDataFile << ", " << runData.avgChanVolt[i];
        runDataFile << ", " << runData.avgChanCurrent[i];
        runDataFile << ", " << runData.avgHVChanTemp[i];
        runDataFile << ", " << runData.rawCounts[i];
        runDataFile << ", " << runData.rawRates[i];
    }
    runDataFile << std::endl;
}

/*
 EndEpoch, End DateTime, NumRuns
*/
void CsvOutput::done()
{
    //here we end things
    //first close the run data file, we have nothing more to add to it
    runDataFile.close();
    writeEpochDateTimePairToStream(lastEndOfRun, batchCsv);
    batchCsv << numRuns;
    batchCsv.close();
}

void CsvOutput::writeEpochDateTimePairToStream(unsigned long long epochDuration, std::ostream& out)
{
    //I hate using the default c++ time libraries, boost does it so well, but
    //since the boost installation I have access to on the cluster is header
    //only, and I think that the date time component is compiled, I have to
    //instead come up with a muddled and probably sub optimal work around with
    //the chrono and ctime libraries. There must be an easier way to do this
    //but I cannot see it right now
    //now write the stop time and the final number of runs to the batch metadata file
    int fracSeconds = (epochDuration % 1000000ULL);
    std::chrono::system_clock::time_point timePt(std::chrono::seconds(epochDuration/1000000ULL));
    std::time_t timet = std::chrono::system_clock::to_time_t(timePt);
    char timeOutputArray[101];
    strftime(timeOutputArray, 100, "%Y-%b-%d %H:%M:%S", localtime(&timet));
    out << epochDuration << ", ";
    out << timeOutputArray << "." << std::setw(6) << std::setfill('0')
        << fracSeconds << std::setfill(' ') << std::setw(0) << ", ";
}

}
