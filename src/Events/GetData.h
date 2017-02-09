/***************************************************************************//**
********************************************************************************
**
** @file DppPsdIntegralEvent.h
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
** @details Holds the definition of the class for DPP-PSD events without
** waveforms
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_EVENTS_GETDATA_H
#define ORCHIDREADER_SRC_EVENTS_GETDATA_H

namespace Events
{

template<class Type>
Type getData(char* buffer, int index)
{
    return *(reinterpret_cast<Type*>(buffer+index));
}

}
#endif //ORCHIDREADER_SRC_EVENTS_GETDATA_H
