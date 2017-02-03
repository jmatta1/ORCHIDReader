/***************************************************************************//**
********************************************************************************
**
** @file OrchidFileReader.cpp
** @author James Till Matta
** @date 03 Feb, 2017
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the implementation of the class which reads the ORCHID files
**
********************************************************************************
*******************************************************************************/
#include"OrchidFileReader.h"
// includes for C system headers
// includes for C++ system headers
#include<iostream>
#include<fstream>
#include<ctime>
// includes from other libraries
// includes from ORCHIDReader

namespace Input 
{
template <class RetType>
RetType grabType(const int bInd, const char* buf)
{
    return reinterpret_cast<RetType*>(buf[bInd])[0];
}

OrchidFileReader::OrchidFileReader(InputParser::ConfigData* cData):confData(cData)
{
    //allocate the file read buffer to be 50% bigger than a buffer in the file
    buffer = new char[3*1024*1024];
    this->readListFile();
}

OrchidFileReader::~OrchidFileReader()
{
    delete[] buffer;
}

void OrchidFileReader::processFiles(Output::RootOutput* output)
{
    int numFiles = fileList.size();
    for(int i=0; i<numFiles; ++i)
    {
        time_t currTime;
        time(&currTime);
        char timeOutputArray[101];
        strftime(timeOutputArray, 100, "%Y-%b-%d %H:%M:%S", localtime(&currTime));
        std::cout<<timeOutputArray<<" Opening file: "<<fileList[i]<<"\n";
        //open the file and extract a few things
        std::ifstream infile(fileList[i].c_str(), std::ios::binary | std::ios::in);
        std::string runTitle;
        int seriesNumber;
        int runNumber;
        infile.read(buffer, 4096);
        procFileHeader(runTitle, runNumber, seriesNumber);
        if(i!=0)//skip this for the first file
        {
            fileEvent.oldFileLastBufferTime = lastBufferEndTime;
        }
    }
}

void OrchidFileReader::procFileHeader(std::string& title, int& rNum, int& sNum)
{
    int bInd = 26;
    title = std::string(buffer+bInd);
    bInd += 100;
    rNum = grabType<unsigned long>(bInd, buffer);
    bInd += 4;
    sNum = grabType<unsigned long>(bInd, buffer);
}

void OrchidFileReader::readListFile()
{
    std::string tempStr;
    std::ifstream input;
    input.open(confData->listFilePath);
    if(input.good())
    {
        std::getline(input,tempStr);
    }
    else
    {
        std::cout<<"Something is wrong with the file list"<<std::endl;
        throw std::runtime_error("Could not retrieve file data");
    }
    while(!input.eof() && input.good())
    {
        fileList.push_back(tempStr);
        std::getline(input,tempStr);
    }
    input.close();
    if(fileList.size()<1)
    {
        std::cout<<"No files in list"<<std::endl;
        throw std::runtime_error("No Files in input list");
    }
}

}

