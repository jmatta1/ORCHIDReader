/***************************************************************************//**
********************************************************************************
**
** @file DetData.cpp
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
** @details Holds the implementation of the data structure that holds detector
** setup data
**
********************************************************************************
*******************************************************************************/
#include"DetData.h"
// includes for C system headers
// includes for C++ system headers
#include<iomanip>
// includes from other libraries
// includes from ORCHIDReader
#include"Utility/SortPermutation.h"

namespace InputParser
{
//stream operator to output the DetType in a pretty way
std::ostream& operator<<(std::ostream& os, const DetType& value)
{
    switch(value)
    {
    case DetType::NaI:
        return os << "NaI";
    case DetType::LS:
        return os << "LS";
    case DetType::CeBr3:
        return os << "CeBr3";
    case DetType::HeMod:
        return os << "HeMod";
    case DetType::HeUnmod:
        return os << "HeUnmod";
    }
}

void DetData::addDetectorNum(const int& input)
{
    detectorNum.push_back(input);
}

void DetData::addDigiBoardNum(const int& input)
{
    digiBoardNum.push_back(input);
}

void DetData::addDigiChanNum(const int& input)
{
    digiChanNum.push_back(input);
}

void DetData::addMpodBoardNum(const int& input)
{
    mpodBoardNum.push_back(input);
}

void DetData::addMpodChanNum(const int& input)
{
    mpodChanNum.push_back(input);
}

void DetData::addDetXOffset(const float& input)
{
    detXOffset.push_back(input);
}

void DetData::addDetYOffset(const float& input)
{
    detYOffset.push_back(input);
}

void DetData::addDetZOffset(const float& input)
{
    detZOffset.push_back(input);
}

void DetData::addDetType(const DetType& input)
{
    detType.push_back(input);
}

void DetData::addPsdProjEnThresh(const float& input)
{
    psdProjEnThresh.push_back(input);
}

void DetData::addEnProjPsdThresh(const float& input)
{
    enProjPsdThresh.push_back(input);
}

bool DetData::validate()
{
    bool retVal = true;
    if(!((detectorNum.size() >= 1) &&
         (digiBoardNum.size() >= 1) &&
         (digiChanNum.size() >= 1) &&
         (mpodBoardNum.size() >= 1) &&
         (mpodChanNum.size() >= 1) &&
         (detXOffset.size() >= 1) &&
         (detYOffset.size() >= 1) &&
         (detZOffset.size() >= 1) &&
         (detType.size() >= 1) &&
         (psdProjEnThresh.size() >= 1) &&
         (psdProjEnThresh.size() >= 1)))
    {
        retVal = false;
    }
    int rows = detectorNum.size();
    if(!((rows == detectorNum.size()) &&
         (rows == digiBoardNum.size()) &&
         (rows == digiChanNum.size()) &&
         (rows == mpodBoardNum.size()) &&
         (rows == mpodChanNum.size()) &&
         (rows == detXOffset.size()) &&
         (rows == detYOffset.size()) &&
         (rows == detZOffset.size()) &&
         (rows == detType.size()) &&
         (rows == psdProjEnThresh.size()) &&
         (rows == psdProjEnThresh.size())))
    {
        retVal = false;
    }
    //in theory we should be checking here for repeated detector numbers,
    //repeated digitizer board and channel pairs, and repeated mpod board and
    //channel pairs, for now we wont because it is troublesome
    //TODO better validation checks
    return retVal;
}

void DetData::printValidationErrors()
{
    if(!((detectorNum.size() >= 1) &&
         (digiBoardNum.size() >= 1) &&
         (digiChanNum.size() >= 1) &&
         (mpodBoardNum.size() >= 1) &&
         (mpodChanNum.size() >= 1) &&
         (detXOffset.size() >= 1) &&
         (detYOffset.size() >= 1) &&
         (detZOffset.size() >= 1) &&
         (detType.size() >= 1) &&
         (psdProjEnThresh.size() >= 1) &&
         (psdProjEnThresh.size() >= 1)))
    {
        std::cout<<"There needs to be at least one detector line entry"<<std::endl;
    }
    int rows = detectorNum.size();
    if(!((rows == detectorNum.size()) &&
         (rows == digiBoardNum.size()) &&
         (rows == digiChanNum.size()) &&
         (rows == mpodBoardNum.size()) &&
         (rows == mpodChanNum.size()) &&
         (rows == detXOffset.size()) &&
         (rows == detYOffset.size()) &&
         (rows == detZOffset.size()) &&
         (rows == detType.size()) &&
         (rows == psdProjEnThresh.size()) &&
         (rows == psdProjEnThresh.size())))
    {
        std::cout<<"There needs to be an equal number of entries for every"
                <<" detector line column"<<std::endl;
    }
    //in theory we should be checking here for repeated detector numbers,
    //repeated digitizer board and channel pairs, and repeated mpod board and
    //channel pairs, for now we wont because it is troublesome
    //TODO: better validation checks
}

void DetData::sort()
{
    Utility::OneKeyIntCompare comp;
    std::vector<std::size_t> permutation = Utility::oneKeySortPermutation(detectorNum, comp);
    detectorNum = Utility::applyPermutation(detectorNum, permutation);
    digiBoardNum = Utility::applyPermutation(digiBoardNum, permutation);
    digiChanNum = Utility::applyPermutation(digiChanNum, permutation);
    mpodBoardNum = Utility::applyPermutation(mpodBoardNum, permutation);
    mpodChanNum = Utility::applyPermutation(mpodChanNum, permutation);
    detXOffset = Utility::applyPermutation(detXOffset, permutation);
    detYOffset = Utility::applyPermutation(detYOffset, permutation);
    detZOffset = Utility::applyPermutation(detZOffset, permutation);
    detType = Utility::applyPermutation(detType, permutation);
    psdProjEnThresh = Utility::applyPermutation(psdProjEnThresh, permutation);
    enProjPsdThresh = Utility::applyPermutation(enProjPsdThresh, permutation);
}


std::ostream& operator<<(std::ostream& os, const DetData& dd)
{
    using std::setw;
    os<<"DetNum | DigiBrdNum | DigiChnNum | MpodBrdNum | MpodChnNum | XOffset | YOffset | ZOffset | DetType | ProjEnThresh | ProjPsdThresh"<<std::endl;
    os<<"=================================================================================================================================";
    for(int i=0; i<dd.detectorNum.size(); ++i)
    {
        os<<"\n"<<setw(6)<<dd.detectorNum[i] << " | "<<setw(10)<<dd.digiBoardNum[i];
        os<<" | "<<setw(10)<<dd.digiChanNum[i] << " | "<<setw(10)<<dd.mpodBoardNum[i];
        os<<" | "<<setw(10)<<dd.mpodChanNum[i] << " | "<<setw(7)<<dd.detXOffset[i];
        os<<" | "<<setw(7)<<dd.detYOffset[i] << " | "<<setw(7)<<dd.detZOffset[i];
        os<<" | "<<setw(7)<<dd.detType[i] << " | "<<setw(12)<<dd.psdProjEnThresh[i];
        os<<" | "<<setw(13)<<dd.enProjPsdThresh[i];
    }
    os<<"\n"<<std::endl;
    return os;
}

}
