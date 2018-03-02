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
#include<iomanip>
#include<fstream>
#include<ctime>
// includes from other libraries
// includes from ORCHIDReader



namespace Input 
{

static const unsigned long long FileHeaderSize = 4096;
static const unsigned long long BufferSize = 2*1024*1024;
static const unsigned long long BufferDataSize = (2*1024*1024-8192);
static const unsigned long long BufferDataOffset = 8192;

OrchidFileReader::OrchidFileReader(InputParser::ConfigData* cData, int numDet):
    confData(cData), scEvent(numDet)
{
    //allocate the file read buffer to be 50% bigger than a buffer in the file
    buffer = new char[3*1024*1024];
    this->readListFile();
}

OrchidFileReader::~OrchidFileReader()
{
    delete[] buffer;
}

void OrchidFileReader::processFiles(Output::OutputSystem* output)
{
    int numFiles = fileList.size();
    int skipCount = 0;
    for(int i=0; i<numFiles; ++i)
    {
        //output the time and file
        time_t currTime;
        time(&currTime);
        char timeOutputArray[101];
        strftime(timeOutputArray, 100, "%Y-%b-%d %H:%M:%S", localtime(&currTime));
        std::cout<<timeOutputArray<<" Opening file: "<<fileList[i]<<"\n";
        //open the file
        std::ifstream infile(fileList[i].c_str(), std::ios::binary | std::ios::in);
        if(!infile.good())
        {
            std::cout<<"Error in opening file, skipping"<<std::endl;
            ++skipCount;
            continue;
        }
        //get the file's size
        infile.seekg(0,std::ios_base::end);
        currentFileSize = infile.tellg();
        infile.seekg(0,std::ios_base::beg);
        unsigned long long fileOffset = 0;
        long long lastBuffer = (currentFileSize-BufferSize);
        //handle the weird bug that previous versions had that sometimes wrote
        //a just the header of a data buffer to the beginning of a file, before
        //the file header
        long long remainder = ((currentFileSize-FileHeaderSize)%BufferSize);
        if(remainder != 0)
        {//if this is the case then there is an incomplete buffer somewhere,
            //it seems that whenever this happens the incomplete buffer is at
            //the begining of the data stream, at it seems to be the 8192B
            //dataBuffer header for an empty buffer, fix this by simply skipping
            //that
            if(remainder >= 8192)
            {// if the extra size is greater than or equal to 8192, it could be
                // a broken buffer, a leading buffer header, *or* both, so first
                //read the leading 8192 bytes and see if it is a buffer header
                //if it isn't a buffer header, treat all the remainder as a broken
                //trailing buffer. If there is a buffer header at the start, then
                //see if there is more that we need to account for, if there is,
                //attribute the remaining excess size to a trailing broken buffer
                std::cout<<"Initial Buffer Fragment! Offsetting start!"<<std::endl;
                fileOffset += BufferDataOffset;
                infile.read(buffer, BufferDataOffset);
                //check to make sure that the chunk read was indeed the weird buffer header
                unsigned int firstInt = reinterpret_cast<int*>(buffer)[0];
                if(firstInt != 0xf0f0f0f0)
                { //there was no leading buffer header, undo our  changes
                    //and attribute all the remainder to a broken trailing buffer
                    fileOffset = 0;
                    infile.seekg(0,std::ios_base::beg);
                    currentFileSize -= remainder;
                    lastBuffer = (currentFileSize-BufferSize);
                }
                else if(remainder > 8192)
                {//there was a leading buffer *and* a broken trailing buffer
                    currentFileSize -= (remainder-8192);
                    lastBuffer = (currentFileSize-BufferSize);
                }
            }
            else
            {//if the extra size is less than 8192 bytes it cannot be the
                //wierd buffer thing so assume it is a broken buffer at end
                currentFileSize -= remainder;
                lastBuffer = (currentFileSize-BufferSize);
            }
        }
        if((currentFileSize-FileHeaderSize) < BufferSize)
        {
            std::cout<<"File has no buffers! Skipping"<<std::endl;
            ++skipCount;
            continue;
        }
        //read and process the file header
        infile.read(buffer, FileHeaderSize);
        fileOffset += FileHeaderSize;
        FileHeaderData tempFileHeader;
        tempFileHeader.readFromBuffer(buffer);
        //read the first buffer and process its header
        infile.read(buffer, BufferSize);
        fileOffset += BufferSize;
        if(!confData->processFirstBuffer && i==0)
        {//this is the first file of the batch and we do not want to process the
            //first buffer, since we just read the first buffer, simply read
            //the next buffer immediately
            infile.read(buffer, BufferSize);
            fileOffset += BufferSize;
        }
        BufferHeaderData tempBufferData;
        int buffInd = tempBufferData.readFromBuffer(buffer);
        //make a new file event
        if(i==skipCount)
        {
            fileEvent.firstFile = true;
            fileEvent.sameRun = true; //kinda by default here
            fileEvent.previousFileLastBufferEndTime = 0;
            fileEvent.newFileHeaderTime = tempFileHeader.fileStartTime;
            fileEvent.newFileFirstBufferStartTime = tempBufferData.bufferStartTime;
        }
        else
        {
            fileEvent.firstFile = false;
            fileEvent.previousFileLastBufferEndTime = this->currBufferData.bufferStopTime;
            fileEvent.newFileHeaderTime = tempFileHeader.fileStartTime;
            fileEvent.newFileFirstBufferStartTime = tempBufferData.bufferStartTime;
            //figure out if we are in the same run
            fileEvent.sameRun = true;
            if(tempFileHeader.runTitle != currFileData.runTitle ||
                    tempFileHeader.runNumber != currFileData.runNumber ||
                    tempFileHeader.sequenceNumber != (currFileData.sequenceNumber + 1))
            {
                std::cout<<"First clause\n"<< tempFileHeader.runTitle<< ", "<<currFileData.runTitle<<"\n"
                         <<tempFileHeader.runNumber<<", "<<currFileData.runNumber<<"   |   "
                         <<tempFileHeader.sequenceNumber<<", "<<currFileData.sequenceNumber<<"   |   "
                         <<tempFileHeader.fileStartTime<<", "<<currBufferData.bufferStopTime<<std::endl;
                fileEvent.sameRun = false;
            }
            else if(tempFileHeader.fileStartTime > (currBufferData.bufferStopTime + 10000000))
            {//if this is the case then it has been more than 10 seconds since the last file was started so it was probably a pause
                std::cout<<"Second clause\n"<< tempFileHeader.runTitle<< ", "<<currFileData.runTitle<<"\n"
                         <<tempFileHeader.runNumber<<", "<<currFileData.runNumber<<"   |   "
                         <<tempFileHeader.sequenceNumber<<", "<<currFileData.sequenceNumber<<"   |   "
                         <<tempFileHeader.fileStartTime<<", "<<currBufferData.bufferStopTime<<std::endl;
                fileEvent.sameRun = false;
            }
        }
        output->newFileEvent(fileEvent);
        //swap the old data out with the new data
        this->currFileData = tempFileHeader;
        this->currBufferData = tempBufferData;
        //now process the rest of the buffer we read the header of
        processDataBuffer(output, buffInd);
        //now loop through the rest of the file
        while(fileOffset <= lastBuffer)
        {
            infile.read(buffer, BufferSize);
            fileOffset += BufferSize;
            buffInd = tempBufferData.readFromBuffer(buffer);
            this->currBufferData = tempBufferData;
            processDataBuffer(output, buffInd);
        }
    }
    time_t currTime;
    time(&currTime);
    char timeOutputArray[101];
    strftime(timeOutputArray, 100, "%Y-%b-%d %H:%M:%S", localtime(&currTime));
    std::cout<<timeOutputArray<<" Done Processing Files"<<std::endl;
}

void OrchidFileReader::processDataBuffer(Output::OutputSystem* output, int startInd)
{
    int bInd = startInd;
    unsigned long long aprxTime;
    int eventCount = 0;
    unsigned long long timeDiff = currBufferData.bufferStopTime-currBufferData.bufferStartTime;
    while(bInd < BufferSize)
    {
        if(buffer[bInd] == 15 && buffer[bInd+1] == 2)
        {//DPP PSD event
            aprxTime = ((timeDiff*eventCount/currBufferData.eventCount)+currBufferData.bufferStartTime);
            bInd += dppEvent.readEvent(buffer + bInd, aprxTime);
            output->dppPsdIntegralEvent(dppEvent);
            eventCount += 1;
        }
        else if(buffer[bInd] != 0 && buffer[bInd+2] == 1 && buffer[bInd+3] == 0)
        {// the 2 0 check makes sure a 2 byte value of 2 is written slow controls event
            aprxTime = ((timeDiff*eventCount/currBufferData.eventCount)+currBufferData.bufferStartTime);
            bInd += scEvent.readEvent(buffer + bInd, aprxTime);
            output->slowControlsEvent(scEvent);
            eventCount += 1;
        }
        else if(buffer[bInd] == 0)
        {
            //the rest of the buffer is blank, skip past the end to get to the next buffer
            bInd = (BufferSize + 10);
        }
        else
        {
            std::cout << "Unexpected event issue in ORCHID File Reader" << std::endl;
            throw std::runtime_error("Bad event code");
        }
    }
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

