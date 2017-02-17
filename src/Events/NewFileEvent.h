/***************************************************************************//**
********************************************************************************
**
** @file NewFileEvent.h
** @author James Till Matta
** @date 02 Feb, 2017
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition of the class for file swaps
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_EVENTS_NEWFILEEVENT_H
#define ORCHIDREADER_SRC_EVENTS_NEWFILEEVENT_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHIDReader

namespace Events
{

class NewFileEvent
{
public:
    unsigned long long previousFileLastBufferEndTime;
    unsigned long long newFileHeaderTime;
    unsigned long long newFileFirstBufferStartTime;
    bool firstFile;
    bool sameRun;
};

}

#endif //ORCHIDREADER_SRC_EVENTS_NEWFILEEVENT_H
