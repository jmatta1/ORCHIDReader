/***************************************************************************//**
********************************************************************************
**
** @file DetData.h
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
** @details Holds the definition of the data structure that holds detector
** setup data
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_CONFIG_DETDATA_H
#define ORCHIDREADER_SRC_CONFIG_DETDATA_H
// includes for C system headers
// includes for C++ system headers
#include<vector>
#include<iostream>
// includes from other libraries
// includes from ORCHIDReader

namespace InputParser
{

enum class DetType : char {NaI, LS, CeBr3, HeMod, HeUnmod};
// stream operator to output the DetType enum in a pretty way
std::ostream& operator<<(std::ostream& os, const DetType& value);

class DetData
{
public:
    std::vector<int> detectorNum;
    std::vector<int> digiBoardNum;
    std::vector<int> digiChanNum;
    std::vector<int> mpodBoardNum;
    std::vector<int> mpodChanNum;
    std::vector<float> detXOffset;
    std::vector<float> detYOffset;
    std::vector<float> detZOffset;
    std::vector<DetType> detType;
    std::vector<float> psdProjEnThresh;
    std::vector<float> enProjPsdThresh;
    
    void addDetectorNum(const int& input);
    void addDigiBoardNum(const int& input);
    void addDigiChanNum(const int& input);
    void addMpodBoardNum(const int& input);
    void addMpodChanNum(const int& input);
    void addDetXOffset(const float& input);
    void addDetYOffset(const float& input);
    void addDetZOffset(const float& input);
    void addDetType(const DetType& input);
    void addPsdProjEnThresh(const float& input);
    void addEnProjPsdThresh(const float& input);
    
    bool validate();
    void printValidationErrors();
    
    void sort();
    
    friend std::ostream& operator<<(std::ostream& os, const DetData& dd);
    
private:
};


}

#endif //ORCHIDREADER_SRC_CONFIG_DETDATA_H
