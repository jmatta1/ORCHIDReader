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
#include"Utility/GetData.h"


namespace Input 
{

static const unsigned long long FileHeaderSize = 4096;
static const unsigned long long BufferSize = 2*1024*1024;
static const unsigned long long BufferDataSize = (2*1024*1024-8192);
static const unsigned long long BufferDataOffset = 8192;

BufferHeaderData& BufferHeaderData::operator=(const BufferHeaderData& rhs)
{
    eventCount      = rhs.eventCount;
    bufferNumber    = rhs.bufferNumber;
    bufferStartTime = rhs.bufferStartTime;
    bufferStopTime  = rhs.bufferStopTime;
}

int BufferHeaderData::readFromBuffer(char* buffer)
{
    //the first 8 bytes of a buffer header are 0xF0F0F0F000000002
    //i.e. the 4 bytes separator value followed bye the 4 bytes buffer id number
    //thus we ignore them
    int bInd = 8;
    //the next 4 bytes are the number of events, regular and slow controls, that are in the buffer
    eventCount = getData<unsigned int>(buffer, bInd);
    bInd += 4;
    //the next 4 bytes is the sequence number of this buffer within the file
    bufferNumber = getData<unsigned int>(buffer, bInd);
    bInd += 4;
    //the next 8 bytes are the start time, in microseconds since unix epoch of writing the buffer
    bufferStartTime = getData<unsigned long long>(buffer, bInd);
    bInd += 8;
    //the next 8 bytes are the stop time, in microseconds since unix epoch of writing the buffer
    bufferStopTime= getData<unsigned long long>(buffer, bInd);
    bInd += 8;
    //the rest of the buffer header contains CRC checks of the buffer contents.
    //in future versions perform the check and mark bad sections in a bit array to
    //added to the class in the future
    
    //now return the size of the buffer header in bytes
    return 8192;
}

FileHeaderData& FileHeaderData::operator=(const FileHeaderData& rhs)
{
    orchidMajorVer      = rhs.orchidMajorVer;
    orchidMinorVer      = rhs.orchidMinorVer;
    orchidPatchVer      = rhs.orchidPatchVer;
    fileStartTime       = rhs.fileStartTime;
    fileStartTimeString = rhs.fileStartTimeString;
    runTitle            = rhs.runTitle;
    runNumber           = rhs.runNumber;
    sequenceNumber      = rhs.sequenceNumber;
}

void FileHeaderData::readFromBuffer(char* buffer)
{
    //skip the byte ordering long and the buffer type identifier
    int bInd = 12;
    //the next 3 sets of two bytes contain the orchid major, minor, and patch versions
    orchidMajorVer = getData<unsigned short>(buffer, bInd);
    bInd += 2;
    orchidMinorVer = getData<unsigned short>(buffer, bInd);
    bInd += 2;
    orchidPatchVer = getData<unsigned short>(buffer, bInd);
    bInd += 2;
    //the next 8 bytes are the posix trime in microseconds
    fileStartTime = getData<unsigned long long>(buffer, bInd);
    bInd += 8;
    //the next 30 bytes are effectively a cstring with the iso extended time string (26 chars) followed by padding /0 chars
    fileStartTimeString = std::string(buffer + bInd);
    bInd += 30;
    //the next 100 bytes are up to 99 characters of the run title followed by padding /0 characters
    runTitle = std::string(buffer+bInd);
    bInd += 100;
    //the next 4 bytes are the run number
    runNumber = getData<unsigned long>(buffer, bInd);
    bInd += 4;
    //the next 4 bytes are the sequence number
    sequenceNumber = getData<unsigned long>(buffer, bInd);
    //the rest of the file header buffer is blank except for 8 bytes of F0F0...
    //at the very end of the file header buffer
}

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
        //get the file's size
        infile.seekg(0,std::ios_base::end);
        currentFileSize = infile.tellg();
        infile.seekg(0,std::ios_base::beg);
        unsigned long long fileOffset = 0;
        long long lastBuffer = (currentFileSize-BufferSize);
        //handle the weird bug that previous versions had that sometimes wrote
        //a just the header of a data buffer to the beginning of a file, before
        //the file header
        if(((currentFileSize-FileHeaderSize)%BufferSize) != 0)
        {//if this is the case then there is an incomplete buffer somewhere,
            //it seems that whenever this happens the incomplete buffer is at
            //the begining of the data stream, at it seems to be the 8192B
            //dataBuffer header for an empty buffer, fix this by simply skipping
            //that
            std::cout<<"Initial Buffer Fragment! Offsetting start!"<<std::endl;
            fileOffset += BufferDataOffset;
            infile.read(buffer, BufferDataOffset);
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
        if(i==0)
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
            fileEvent.previousFileLastBufferEndTime = 0;
            fileEvent.newFileHeaderTime = tempFileHeader.fileStartTime;
            fileEvent.newFileFirstBufferStartTime = tempBufferData.bufferStartTime;
            //figure out if we are in the same run
            fileEvent.sameRun = true;
            if(tempFileHeader.runTitle != currFileData.runTitle ||
                    tempFileHeader.runNumber != currFileData.runNumber ||
                    tempFileHeader.sequenceNumber != (currFileData.sequenceNumber + 1))
            {
                fileEvent.sameRun = false;
            }
            else if(tempFileHeader.fileStartTime > (currBufferData.bufferStopTime + 10000000))
            {//if this is the case then it has been more than 10 seconds since the last file was started so it was probably a pause
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

