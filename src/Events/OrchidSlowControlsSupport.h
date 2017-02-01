/***************************************************************************//**
********************************************************************************
**
** @file OrchidSlowControlsSupport.h
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
** @details Holds helper class definitions for ORCHID Slow controls data
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_EVENTS_ORCHIDSLOWCONTROLSSUPPORT_H
#define ORCHIDREADER_SRC_EVENTS_ORCHIDSLOWCONTROLSSUPPORT_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHIDReader

namespace Events
{

namespace CrateMasks 
{
static const unsigned int MaskMainOn=0x8000,        MaskMainInhibit=0x4000,    MaskLocalControlOnly=0x2000;
static const unsigned int MaskInputFailure=0x1000,  MaskOutputFailure=0x0800,  MaskFantrayFailue=0x4000;
static const unsigned int MaskSensorFailure=0x0200, MaskVmeSysFailure=0x0100,  MaskPlugAndPlayIncompatible=0x0080;
static const unsigned int MaskBusReset=0x0040,      MaskSupplyDerating=0x0020, MaskSupplyFailure=0x0010;
}

class CrateStatus
{
public:
    CrateStatus();
    bool mainOn                  :1;//bit0      (MSB)
    bool mainInhibit             :1;//bit1
    bool localControlOnly        :1;//bit2
    bool inputFailure            :1;//bit3
    bool outputFailure           :1;//bit4
    bool fantrayFailure          :1;//bit5
    bool sensorFailure           :1;//bit6
    bool vmeSysFailure           :1;//bit7
    bool plugAndPlayIncompatible :1;//bit8
    bool busReset                :1;//bit9
    bool supplyDerating          :1;//bit10
    bool supplyFailure           :1;//bit11     (LSB)
    
    std::string getStatusString();
    void loadFromValue(unsigned int value);
    CrateStatus& operator=(const CrateStatus& rhs);
    unsigned int giveIntRepresentation();
};

namespace ChannelMasks 
{
static const unsigned int MaskOutputOn=0x800000,                         MaskOutputInhibit=0x400000;
static const unsigned int MaskOutputFailureMinSenseVoltage=0x200000,     MaskOutputFailureMaxSenseVoltage=0x100000;
static const unsigned int MaskOutputFailureMaxTerminalVoltage=0x080000,  MaskOutputFailureMaxCurrent=0x040000;
static const unsigned int MaskOutputFailureMaxTemperature=0x020000,      MaskOutputFailureMaxPower=0x010000;
static const unsigned int MaskOutputFailureTimeout=0x004000,             MaskOutputCurrentLimited=0x002000;
static const unsigned int MaskOutputRampUp=0x001000,                     MaskOutputRampDown=0x000800;
static const unsigned int MaskOutputEnableKill=0x000400,                 MaskOutputEmergencyOff=0x000200;
static const unsigned int MaskOutputAdjusting=0x000100,                  MaskOutputConstantVoltage=0x000080;
static const unsigned int MaskOutputCurrentBoundsExceeded=0x000020,      MaskOutputFailureCurrentLimit=0x000010;
}

class ChannelStatus
{
public:
    ChannelStatus();
    bool outputOn                        :1;//bit0      (MSB)
    bool outputInhibit                   :1;//bit1
    bool outputFailureMinSenseVoltage    :1;//bit2
    bool outputFailureMaxSenseVoltage    :1;//bit3
    bool outputFailureMaxTerminalVoltage :1;//bit4
    bool outputFailureMaxCurrent         :1;//bit5
    bool outputFailureMaxTemperature     :1;//bit6
    bool outputFailureMaxPower           :1;//bit7
    bool reserved1                       :1;//bit8
    bool outputFailureTimeout            :1;//bit9
    bool outputCurrentLimited            :1;//bit10
    bool outputRampUp                    :1;//bit11
    bool outputRampDown                  :1;//bit12
    bool outputEnableKill                :1;//bit13
    bool outputEmergencyOff              :1;//bit14
    bool outputAdjusting                 :1;//bit15
    bool outputConstantVoltage           :1;//bit16
    bool reserved2                       :1;//bit17
    bool outputCurrentBoundsExceeded     :1;//bit18
    bool outputFailureCurrentLimit       :1;//bit19     (LSB)

    std::string getStatusString();
    void loadFromValue(unsigned int value);
    ChannelStatus& operator=(const ChannelStatus& rhs);
    unsigned int giveIntRepresentation();
};

}
#endif //ORCHIDREADER_SRC_EVENTS_ORCHIDSLOWCONTROLSSUPPORT_H
