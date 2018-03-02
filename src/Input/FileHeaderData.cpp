#include"FileHeaderData.h"

#include"Utility/GetData.h"

namespace Input
{

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
    runNumber = getData<unsigned int>(buffer, bInd);
    bInd += 4;
    //the next 4 bytes are the sequence number
    sequenceNumber = getData<unsigned int>(buffer, bInd);
    //the rest of the file header buffer is blank except for 8 bytes of F0F0...
    //at the very end of the file header buffer
}

}
