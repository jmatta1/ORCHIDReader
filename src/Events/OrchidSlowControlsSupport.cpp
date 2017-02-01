/***************************************************************************//**
********************************************************************************
**
** @file OrchidSlowControlsSupport.cpp
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
** @details Holds helper class implementations for ORCHID Slow controls data
**
********************************************************************************
*******************************************************************************/
#include"Events/OrchidSlowControlsSupport.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHIDReader
namespace Events
{

CrateStatus::CrateStatus():         mainOn(false),          mainInhibit(false),
    localControlOnly(false),        inputFailure(false),    outputFailure(false),
    fantrayFailure(false),          sensorFailure(false),   vmeSysFailure(false),
    plugAndPlayIncompatible(false), busReset(false),        supplyDerating(false),
    supplyFailure(false) {}

std::string CrateStatus::getStatusString()
{
    std::ostringstream output;
    if(mainOn)
    {
        output << "On ";
    }
    if(mainInhibit)
    {
        output << "Inhibit ";
    }
    if(localControlOnly)
    {
        output << "Lcl_Ctrl_Only ";
    }
    if(inputFailure || outputFailure || fantrayFailure || sensorFailure || vmeSysFailure || plugAndPlayIncompatible || supplyDerating || supplyFailure)
    {
        output << "Failure: ( ";
        if(inputFailure) output << "input ";
        if(outputFailure) output << "output ";
        if(fantrayFailure) output << "fantray ";
        if(sensorFailure) output << "sensor ";
        if(vmeSysFailure) output << "VME_Sys ";
        if(plugAndPlayIncompatible) output << "PnP_Incompatibility ";
        if(supplyDerating) output << "PS_derating ";
        if(supplyFailure) output << "PS_failure ";
        output << ") ";
        
    }
    if(busReset)
    {
        output << "Bus_Reset ";
    }
    return output.str();
}

void CrateStatus::loadFromValue(unsigned int value)
{
    this->mainOn                  = ((value & CrateMasks::MaskMainOn)                  != 0);
    this->mainInhibit             = ((value & CrateMasks::MaskMainInhibit)             != 0);
    this->localControlOnly        = ((value & CrateMasks::MaskLocalControlOnly)        != 0);
    this->inputFailure            = ((value & CrateMasks::MaskInputFailure)            != 0);
    this->outputFailure           = ((value & CrateMasks::MaskOutputFailure)           != 0);
    this->fantrayFailure          = ((value & CrateMasks::MaskFantrayFailue)           != 0);
    this->sensorFailure           = ((value & CrateMasks::MaskSensorFailure)           != 0);
    this->vmeSysFailure           = ((value & CrateMasks::MaskVmeSysFailure)           != 0);
    this->plugAndPlayIncompatible = ((value & CrateMasks::MaskPlugAndPlayIncompatible) != 0);
    this->busReset                = ((value & CrateMasks::MaskBusReset)                != 0);
    this->supplyDerating          = ((value & CrateMasks::MaskSupplyDerating)          != 0);
    this->supplyFailure           = ((value & CrateMasks::MaskSupplyFailure)           != 0);
}

CrateStatus& CrateStatus::operator=(const CrateStatus& rhs)
{
    this->mainOn                  = rhs.mainOn;
    this->mainInhibit             = rhs.mainInhibit;
    this->localControlOnly        = rhs.localControlOnly;
    this->inputFailure            = rhs.inputFailure;
    this->outputFailure           = rhs.outputFailure;
    this->fantrayFailure          = rhs.fantrayFailure;
    this->sensorFailure           = rhs.sensorFailure;
    this->vmeSysFailure           = rhs.vmeSysFailure;
    this->plugAndPlayIncompatible = rhs.plugAndPlayIncompatible;
    this->busReset                = rhs.busReset;
    this->supplyDerating          = rhs.supplyDerating;
    this->supplyFailure           = rhs.supplyFailure;
    return *this;
}

unsigned int CrateStatus::giveIntRepresentation()
{
    unsigned int output = 0x00000000;
    output |= ((this->mainOn)                   ?CrateMasks::MaskMainOn                 :0x00000000);
    output |= ((this->mainInhibit)              ?CrateMasks::MaskMainInhibit            :0x00000000);
    output |= ((this->localControlOnly)         ?CrateMasks::MaskLocalControlOnly       :0x00000000);
    output |= ((this->inputFailure)             ?CrateMasks::MaskInputFailure           :0x00000000);
    output |= ((this->outputFailure)            ?CrateMasks::MaskOutputFailure          :0x00000000);
    output |= ((this->fantrayFailure)           ?CrateMasks::MaskFantrayFailue          :0x00000000);
    output |= ((this->sensorFailure)            ?CrateMasks::MaskSensorFailure          :0x00000000);
    output |= ((this->vmeSysFailure)            ?CrateMasks::MaskVmeSysFailure          :0x00000000);
    output |= ((this->plugAndPlayIncompatible)  ?CrateMasks::MaskPlugAndPlayIncompatible:0x00000000);
    output |= ((this->busReset)                 ?CrateMasks::MaskBusReset               :0x00000000);
    output |= ((this->supplyDerating)           ?CrateMasks::MaskSupplyDerating         :0x00000000);
    output |= ((this->supplyFailure)            ?CrateMasks::MaskSupplyFailure          :0x00000000);
    return output;
}


ChannelStatus::ChannelStatus():          outputOn(false),
    outputInhibit(false),                outputFailureMinSenseVoltage(false),
    outputFailureMaxSenseVoltage(false), outputFailureMaxTerminalVoltage(false),
    outputFailureMaxCurrent(false),      outputFailureMaxTemperature(false),
    outputFailureMaxPower(false),        reserved1(false),
    outputFailureTimeout(false),         outputCurrentLimited(false),
    outputRampUp(false),                 outputRampDown(false),
    outputEnableKill(false),             outputEmergencyOff(false),
    outputAdjusting(true),               outputConstantVoltage(false),
    reserved2(false),                    outputCurrentBoundsExceeded(false),
    outputFailureCurrentLimit(false) {}

std::string ChannelStatus::getStatusString()
{
    std::ostringstream output;
    if(outputOn) output << "On ";
    if(outputInhibit) output << "Inhibit ";
    if(outputFailureMinSenseVoltage || outputFailureMaxSenseVoltage ||
            outputFailureMaxTerminalVoltage || outputFailureMaxCurrent ||
            outputFailureMaxTemperature || outputFailureMaxPower ||
            outputFailureTimeout || outputEmergencyOff ||
            outputCurrentBoundsExceeded || outputFailureCurrentLimit)
    {
        output << "Failure: ( ";
        if(outputFailureMinSenseVoltage) output << "MinSense ";
        if(outputFailureMaxSenseVoltage) output << "MaxSense ";
        if(outputFailureMaxTerminalVoltage) output << "MaxTerm ";
        if(outputFailureMaxCurrent) output << "MaxCurr ";
        if(outputFailureMaxTemperature) output << "MaxTemp ";
        if(outputFailureMaxPower) output << "MaxPower ";
        if(outputFailureTimeout) output << "FailTimout ";
        if(outputEmergencyOff) output << "EmerOff ";
        if(outputCurrentBoundsExceeded) output << "OutOfCurrBounds ";
        if(outputFailureCurrentLimit) output << "CurrLimFail ";
        output << ") ";
        
    }
    if(outputCurrentLimited) output << "OutCurLim ";
    if(outputRampUp) output << "RampUp ";
    if(outputRampDown) output << "RampDown ";
    if(outputAdjusting) output << "FineAdj ";
    if(outputConstantVoltage) output << "ConstVoltage ";
    return output.str();
}


void ChannelStatus::loadFromValue(unsigned int value)
{
    this->outputOn                        = ((value & ChannelMasks::MaskOutputOn)                        != 0);
    this->outputInhibit                   = ((value & ChannelMasks::MaskOutputInhibit)                   != 0);
    this->outputFailureMinSenseVoltage    = ((value & ChannelMasks::MaskOutputFailureMinSenseVoltage)    != 0);
    this->outputFailureMaxSenseVoltage    = ((value & ChannelMasks::MaskOutputFailureMaxSenseVoltage)    != 0);
    this->outputFailureMaxTerminalVoltage = ((value & ChannelMasks::MaskOutputFailureMaxTerminalVoltage) != 0);
    this->outputFailureMaxCurrent         = ((value & ChannelMasks::MaskOutputFailureMaxCurrent)         != 0);
    this->outputFailureMaxTemperature     = ((value & ChannelMasks::MaskOutputFailureMaxTemperature)     != 0);
    this->outputFailureMaxPower           = ((value & ChannelMasks::MaskOutputFailureMaxPower)           != 0);
    this->outputFailureTimeout            = ((value & ChannelMasks::MaskOutputFailureTimeout)            != 0);
    this->outputCurrentLimited            = ((value & ChannelMasks::MaskOutputCurrentLimited)            != 0);
    this->outputRampUp                    = ((value & ChannelMasks::MaskOutputRampUp)                    != 0);
    this->outputRampDown                  = ((value & ChannelMasks::MaskOutputRampDown)                  != 0);
    this->outputEnableKill                = ((value & ChannelMasks::MaskOutputEnableKill)                != 0);
    this->outputEmergencyOff              = ((value & ChannelMasks::MaskOutputEmergencyOff)              != 0);
    this->outputAdjusting                 = ((value & ChannelMasks::MaskOutputAdjusting)                 != 0);
    this->outputConstantVoltage           = ((value & ChannelMasks::MaskOutputConstantVoltage)           != 0);
    this->outputCurrentBoundsExceeded     = ((value & ChannelMasks::MaskOutputCurrentBoundsExceeded)     != 0);
    this->outputFailureCurrentLimit       = ((value & ChannelMasks::MaskOutputFailureCurrentLimit)       != 0);
}

ChannelStatus& ChannelStatus::operator=(const ChannelStatus& rhs)
{
    this->outputOn                        = rhs.outputOn;
    this->outputInhibit                   = rhs.outputInhibit;
    this->outputFailureMinSenseVoltage    = rhs.outputFailureMinSenseVoltage;
    this->outputFailureMaxSenseVoltage    = rhs.outputFailureMaxSenseVoltage;
    this->outputFailureMaxTerminalVoltage = rhs.outputFailureMaxTerminalVoltage;
    this->outputFailureMaxCurrent         = rhs.outputFailureMaxCurrent;
    this->outputFailureMaxTemperature     = rhs.outputFailureMaxTemperature;
    this->outputFailureMaxPower           = rhs.outputFailureMaxPower;
    this->outputFailureTimeout            = rhs.outputFailureTimeout;
    this->outputCurrentLimited            = rhs.outputCurrentLimited;
    this->outputRampUp                    = rhs.outputRampUp;
    this->outputRampDown                  = rhs.outputRampDown;
    this->outputEnableKill                = rhs.outputEnableKill;
    this->outputEmergencyOff              = rhs.outputEmergencyOff;
    this->outputAdjusting                 = rhs.outputAdjusting;
    this->outputConstantVoltage           = rhs.outputConstantVoltage;
    this->outputCurrentBoundsExceeded     = rhs.outputCurrentBoundsExceeded;
    this->outputFailureCurrentLimit       = rhs.outputFailureCurrentLimit;
    return *this;
}

unsigned int ChannelStatus::giveIntRepresentation()
{
    unsigned int output = 0x00000000;
    output |= ((this->outputOn)                         ?ChannelMasks::MaskOutputOn                         :0x00000000);
    output |= ((this->outputInhibit)                    ?ChannelMasks::MaskOutputInhibit                    :0x00000000);
    output |= ((this->outputFailureMinSenseVoltage)     ?ChannelMasks::MaskOutputFailureMinSenseVoltage     :0x00000000);
    output |= ((this->outputFailureMaxSenseVoltage)     ?ChannelMasks::MaskOutputFailureMaxSenseVoltage     :0x00000000);
    output |= ((this->outputFailureMaxTerminalVoltage)  ?ChannelMasks::MaskOutputFailureMaxTerminalVoltage  :0x00000000);
    output |= ((this->outputFailureMaxCurrent)          ?ChannelMasks::MaskOutputFailureMaxCurrent          :0x00000000);
    output |= ((this->outputFailureMaxTemperature)      ?ChannelMasks::MaskOutputFailureMaxTemperature      :0x00000000);
    output |= ((this->outputFailureMaxPower)            ?ChannelMasks::MaskOutputFailureMaxPower            :0x00000000);
    output |= ((this->outputFailureTimeout)             ?ChannelMasks::MaskOutputFailureTimeout             :0x00000000);
    output |= ((this->outputCurrentLimited)             ?ChannelMasks::MaskOutputCurrentLimited             :0x00000000);
    output |= ((this->outputRampUp)                     ?ChannelMasks::MaskOutputRampUp                     :0x00000000);
    output |= ((this->outputRampDown)                   ?ChannelMasks::MaskOutputRampDown                   :0x00000000);
    output |= ((this->outputEnableKill)                 ?ChannelMasks::MaskOutputEnableKill                 :0x00000000);
    output |= ((this->outputEmergencyOff)               ?ChannelMasks::MaskOutputEmergencyOff               :0x00000000);
    output |= ((this->outputAdjusting)                  ?ChannelMasks::MaskOutputAdjusting                  :0x00000000);
    output |= ((this->outputConstantVoltage)            ?ChannelMasks::MaskOutputConstantVoltage            :0x00000000);
    output |= ((this->outputCurrentBoundsExceeded)      ?ChannelMasks::MaskOutputCurrentBoundsExceeded      :0x00000000);
    output |= ((this->outputFailureCurrentLimit)        ?ChannelMasks::MaskOutputFailureCurrentLimit        :0x00000000);
    return output;
}

}
