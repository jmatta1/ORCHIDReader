#include"BufferHeaderData.h"

#include"Utility/GetData.h"

namespace Input
{

BufferHeaderData& BufferHeaderData::operator=(const BufferHeaderData& rhs)
{
    eventCount      = rhs.eventCount;
    bufferNumber    = rhs.bufferNumber;
    bufferStartTime = rhs.bufferStartTime;
    bufferStopTime  = rhs.bufferStopTime;
}

int BufferHeaderData::readFromBuffer(char* buffer)
{
    //the first 8 bytes of a buffer header are 0xF0F0F0F0 00000002
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

}
