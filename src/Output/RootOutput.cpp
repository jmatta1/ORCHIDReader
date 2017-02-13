/***************************************************************************//**
********************************************************************************
**
** @file RootOutput.cpp
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
** @details Holds the implementation of the system for root output
**
********************************************************************************
*******************************************************************************/
#include"RootOutput.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
#include"root/TParameter.h"
// includes from ORCHIDReader

namespace Output
{

using std::ostringstream;
static const int NumEnChannels = 8192;
static const int NumPsdChannels = 1024;

void BatchTreeData::clearData()
{
    runNumber = 0;
    startTime = 0;
    stopTime = 0;
    centerTime = 0;
    runTime = 0.0;
    numDetectors = 0;
    for(int i=0; i<32; ++i)
    {
        rawCounts[i] = 0;
        rawRates[i] = 0.0;
        avgChanVolt[i] = 0.0;
        avgChanCurrent[i] = 0.0;
        avgHVChanTemp[i] = 0.0;
    }
}

void BatchTreeData::setRates()
{
    for(int i=0; i<32; ++i)
    {
        rawRates[i] = static_cast<double>(rawCounts[i])/runTime;
    }
}

void BatchTreeData::setAverages(int numEntries)
{
    float entries = static_cast<float>(numEntries);
    for(int i=0; i<32; ++i)
    {
        avgChanVolt[i] /= entries;
        avgChanCurrent[i] /= entries;
        avgHVChanTemp[i] /= entries;
    }
}

RootOutput::RootOutput(InputParser::ConfigData* cData, InputParser::DetData* dData):
    confData(cData), detData(dData), treeData(dData->detectorNum.size())
{
    //create the file
    outfile = new TFile(cData->rootFilePath.c_str(),"RECREATE");
    //set up the first level of arrays
    numDetectors = detData->detectorNum.size();
    detRun2DHists = new TH2F*[numDetectors];
    detSum2DHists = new TH2F*[numDetectors];
    enProjWithCutoff = new TH1D*[numDetectors];
    enProjWithoutCutoff = new TH1D*[numDetectors];
    psdProjWithCutoff = new TH1D*[numDetectors];
    psdProjWithoutCutoff = new TH1D*[numDetectors];
    eventTimeHists = new TH1D*[numDetectors];
    enProjWithCutoffSum = new TH1D*[numDetectors];
    enProjWithoutCutoffSum = new TH1D*[numDetectors];
    psdProjWithCutoffSum = new TH1D*[numDetectors];
    psdProjWithoutCutoffSum = new TH1D*[numDetectors];
    //prepare the root tree
    this->prepTree();
    //write constants for the batch out to the file
    TParameter<int>* tempInt = new TParameter<int>("numDet", numDetectors);
    tempInt->Write();
    TParameter<float>* arrayX = new TParameter<float>("arrayXPos", confData->arrayXPos);
    arrayX->Write();
    TParameter<float>* arrayY = new TParameter<float>("arrayYPos", confData->arrayYPos);
    arrayY->Write();
    outfile->Flush();
    delete tempInt;
    delete arrayX;
    delete arrayY;
    // store the per detector info in a TTree
    TTree* detTree = new TTree("detData", "Detector Data Tree");
    int detNum, dBrdNum, dChnNum, mBrdNum, mChnNum, detType;
    batchTree->Branch("detNum", &detNum,"detNum/I");
    batchTree->Branch("digiBrdNum", &dBrdNum,"digiBrdNum/I");
    batchTree->Branch("digiChanNum", &dChnNum,"digiChanNum/I");
    batchTree->Branch("mpodBrdNum", &mBrdNum,"mpodBrdNum/I");
    batchTree->Branch("mpodChanNum", &mChnNum,"mpodChanNum/I");
    batchTree->Branch("detType", &detType,"detType/I");
    float detXPos, detYPos, detZPos;
    batchTree->Branch("xpos", &detXPos,"xpos/I");
    batchTree->Branch("ypos", &detYPos,"ypos/I");
    batchTree->Branch("zpos", &detZPos,"zpos/I");
    for(int i=0; i<numDetectors; ++i)
    {
        detNum = detData->detectorNum[i];
        dBrdNum = detData->digiBoardNum[i];
        dChnNum = detData->digiChanNum[i];
        mBrdNum = detData->mpodBoardNum[i];
        mChnNum = detData->mpodChanNum[i];
        detXPos = (detData->detXOffset[i] + confData->arrayXPos);
        detYPos = (detData->detYOffset[i] + confData->arrayYPos);
        detZPos = detData->detZOffset[i];
        switch(detData->detType[i])
        {
        case InputParser::DetType::NaI:
            detType = 0;
            break;
        case InputParser::DetType::LS:
            detType = 1;
            break;
        case InputParser::DetType::HeMod:
            detType = 2;
            break;
        case InputParser::DetType::HeUnmod:
            detType = 3;
            break;
        case InputParser::DetType::CeBr3:
            detType = 4;
            break;
        default:
            detType = 50;
            break;
        }
        detTree->Fill();
    }
    detTree->Write();
    outfile->Flush();
    delete detTree;
    runNumber = 0;
    firstEvent = false;
    maxTimeEdge = (1.1f*confData->histIntegrationTime/0.001f);
    numTimeBin = static_cast<int>(maxTimeEdge/100.0f);
    histIntTimeUs = static_cast<unsigned long long>(1000000*confData->histIntegrationTime);
    for(int i=0; i<32; ++i)
    {
        firstDetEvent[i] = true;
        runStartTimeStamp[i] = 0;
        lastTimeStamp[i] = 0;
    }
}

RootOutput::~RootOutput()
{
    //since done was called before this, the first level of hist array should be
    //all that needs deleting wrt those
    delete[] detRun2DHists;
    delete[] detSum2DHists;
    delete[] enProjWithCutoff;
    delete[] enProjWithoutCutoff;
    delete[] psdProjWithCutoff;
    delete[] psdProjWithoutCutoff;
    delete[] eventTimeHists;
    delete[] enProjWithCutoffSum;
    delete[] enProjWithoutCutoffSum;
    delete[] psdProjWithCutoffSum;
    delete[] psdProjWithoutCutoffSum;
    
    //since done was called the batch tree was written
    delete batchTree;
    //since done was called the outfile was flushed
    delete outfile;
    //we do not own confData and detData, so don't try to delete them
}

void RootOutput::slowControlsEvent(const Events::OrchidSlowControlsEvent& event)
{
    if(firstEvent)
    {//drop slow controlls events that occur between runs, they should be a tiny
        //minority and would otherwise be a pain to take care of
        return;
    }//these two ifs are separated in case I want to change how something is handled
    if(event.appxTime > runEndEpochTarget)
    {//drop slow controlls events that occur between runs, they should be a tiny
        //minority and would otherwise be a pain to take care of
        return;
    }
    //if we are here the event is during an active run
    for(int i=0; i<event.numVoltageChannels; ++i)
    {
        int detNum = detData->mpodToDet(event.boardNumber[i],event.channelNumber[i]);
        int detInd = detData->detToInd(detNum);
        treeData.avgChanVolt[detInd] += event.terminalVoltage[i];
        treeData.avgChanCurrent[detInd] += event.current[i];
        treeData.avgHVChanTemp[detInd] += event.temperature[i];
    }
    ++numSlowCtrls;
}

void RootOutput::dppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event)
{
    int detNum = detData->digiToDet(event.boardNumber,event.channelNumber);
    int detInd = detData->detToInd(detNum);
    //handle the first event of the batch
    if(firstEvent)
    {
        this->initSums();
        this->initRun();
        firstEvent = false;
        runStartEpoch = event.appxTime;
        runEndEpochTarget = runStartEpoch + histIntTimeUs;
        std::cout << runEndEpochTarget << std::endl;
    }
    //handle the first event of the detector in question
    if(firstDetEvent[detInd])
    {
        runStartTimeStamp[detInd] = event.timeStamp;
        firstDetEvent[detInd] = false;
    }
    //check to see if this event belongs to the next run
    if(event.appxTime > runEndEpochTarget)
    {
        this->closeRun();
        runStartEpoch = lastEpoch;
        runEndEpochTarget = runStartEpoch + histIntTimeUs;
        for(int i=0; i<numDetectors; ++i)
        {
            runStartTimeStamp[i] = lastTimeStamp[i];
        }
        this->initRun();
    }
    //now proceed to handle event normally
    lastTimeStamp[detInd] = event.timeStamp;
    lastEpoch = event.appxTime;
    double longGate = static_cast<double>(event.longIntegral);
    double psd = ((longGate-static_cast<double>(event.shortIntegral))/longGate);
    detRun2DHists[detInd]->Fill(longGate,psd);
    detSum2DHists[detInd]->Fill(longGate,psd);
    enProjWithoutCutoff[detInd]->Fill(longGate);
    enProjWithoutCutoffSum[detInd]->Fill(longGate);
    psdProjWithoutCutoff[detInd]->Fill(psd);
    psdProjWithoutCutoffSum[detInd]->Fill(psd);
    treeData.rawCounts[detInd] += 1;
    if(longGate < detData->psdProjEnThresh[detInd])
    {
        psdProjWithCutoff[detInd]->Fill(psd);
        psdProjWithCutoffSum[detInd]->Fill(psd);
    }
    if(psd < detData->enProjPsdThresh[detInd])
    {
        enProjWithoutCutoff[detInd]->Fill(longGate);
        enProjWithoutCutoffSum[detInd]->Fill(longGate);
    }
    double evTimeMs = static_cast<double>(event.timeStamp - runStartTimeStamp[detInd])/1000.0;
    eventTimeHists[detInd]->Fill(evTimeMs);
}

void RootOutput::inputFileSwitch(const Events::InputFileSwapEvent& event)
{
    if(!event.sameRun)
    {
        this->closeRun();
        runStartEpoch = event.newFileFirstBufferTime;
        runEndEpochTarget = runStartEpoch + histIntTimeUs;
        for(int i=0; i<numDetectors; ++i)
        {
            runStartTimeStamp[i] = 0;
            lastTimeStamp[i] = 0;
            firstDetEvent[i] = true;
        }
        this->initRun();
        return;
    }
    if((event.newFileFirstBufferTime - event.oldFileLastBufferTime) > 7000000)
    {
        this->closeRun();
        runStartEpoch = event.newFileFirstBufferTime;
        runEndEpochTarget = runStartEpoch + histIntTimeUs;
        for(int i=0; i<numDetectors; ++i)
        {
            runStartTimeStamp[i] = 0;
            lastTimeStamp[i] = 0;
            firstDetEvent[i] = true;
        }
        this->initRun();
        return;
    }
}

void RootOutput::done()
{
    //first close out this final run
    this->closeRun();
    //now close out the sum spectra
    this->closeSums();
    //now ensure the tree is written
    batchTree->Write();
    outfile->Flush();
    //now construct the 2D runnum vs en proj and runnum vs psd proj spectra
    this->constructTimeSeriesSpectra();
    //now we are done
}

void RootOutput::constructTimeSeriesSpectra()
{
    int numRuns = runNumber + 1;
    //create the histogram arrays
    TH2F** enProjTimeSeriesWithThresh = new TH2F*[numDetectors];
    TH2F** psdProjTimeSeriesWithThresh = new TH2F*[numDetectors];
    TH2F** enProjTimeSeriesWithoutThresh = new TH2F*[numDetectors];
    TH2F** psdProjTimeSeriesWithoutThresh = new TH2F*[numDetectors];
    //create the histograms
    for(int i=0; i<numDetectors; ++i)
    {
        ostringstream histNamer;
        histNamer << "Det_" << detData->detectorNum[i] << "_px_thresh_timeseries";
        enProjTimeSeriesWithThresh[i] = new TH2F(histNamer.str().c_str(),"En Projection With Cutoff Time Series",NumEnChannels,0,65536,numRuns,0.0,numRuns);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_py_thresh_timeseries";
        psdProjTimeSeriesWithThresh[i] = new TH2F(histNamer.str().c_str(),"Psd Projection With Cutoff Time Series",NumPsdChannels,0.0,1.0,numRuns,0.0,numRuns);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_px_timeseries";
        enProjTimeSeriesWithoutThresh[i] = new TH2F(histNamer.str().c_str(),"En Projection Time Series",NumEnChannels,0,65536,numRuns,0.0,numRuns);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_py_timeseries";
        psdProjTimeSeriesWithoutThresh[i] = new TH2F(histNamer.str().c_str(),"Psd Projection Time Series",NumPsdChannels,0.0,1.0,numRuns,0.0,numRuns);
    }
    
    //now fill the time series histograms
    //first iterate over run number
    for(int i = 0; i < numRuns; ++i)
    {
        //pull the run information from the tree
        batchTree->GetEntry(i);
        //now iterate over detector number
        for(int j=0; j<numDetectors; ++j)
        {
            ostringstream hNamer;
            hNamer << "Det_" << detData->detectorNum[j] << "_Run_" << i << "_px_thresh";
            TH1D* enProjThresh = (TH1D*)outfile->Get(hNamer.str().c_str());
            hNamer.str("");
            hNamer.clear();
            hNamer << "Det_" << detData->detectorNum[j] << "_Run_" << i << "_px";
            TH1D* enProj = (TH1D*)outfile->Get(hNamer.str().c_str());
            hNamer.str("");
            hNamer.clear();
            hNamer << "Det_" << detData->detectorNum[j] << "_Run_" << i << "_py_thresh";
            TH1D* psdProjThresh = (TH1D*)outfile->Get(hNamer.str().c_str());
            hNamer.str("");
            hNamer.clear();
            hNamer << "Det_" << detData->detectorNum[j] << "_Run_" << i << "_py";
            TH1D* psdProj = (TH1D*)outfile->Get(hNamer.str().c_str());
            //iterate over energy channel number to fill that row in this time series histogram, divide by the run time to normalize things
            for(int k=1; k<=NumEnChannels; ++k)
            {
                enProjTimeSeriesWithThresh[j]->SetBinContent(i+1,k,enProjThresh->GetBinContent(k)/treeData.runTime);
                psdProjTimeSeriesWithThresh[j]->SetBinContent(i+1,k,psdProjThresh->GetBinContent(k)/treeData.runTime);
                enProjTimeSeriesWithoutThresh[j]->SetBinContent(i+1,k,enProj->GetBinContent(k)/treeData.runTime);
                psdProjTimeSeriesWithoutThresh[j]->SetBinContent(i+1,k,psdProj->GetBinContent(k)/treeData.runTime);
            }
            //delete the retrieved histograms from memory
            delete enProjThresh;
            delete enProj;
            delete psdProjThresh;
            delete psdProj;
        }
    }
    
    //write and delete the histograms
    for(int i=0; i<numDetectors; ++i)
    {
        enProjTimeSeriesWithThresh[i]->Write();
        psdProjTimeSeriesWithThresh[i]->Write();
        enProjTimeSeriesWithoutThresh[i]->Write();
        psdProjTimeSeriesWithoutThresh[i]->Write();
        delete enProjTimeSeriesWithThresh[i];
        delete psdProjTimeSeriesWithThresh[i];
        delete enProjTimeSeriesWithoutThresh[i];
        delete psdProjTimeSeriesWithoutThresh[i];
    }
    //delete the histogram arrays
    delete[] enProjTimeSeriesWithThresh;
    delete[] psdProjTimeSeriesWithThresh;
    delete[] enProjTimeSeriesWithoutThresh;
    delete[] psdProjTimeSeriesWithoutThresh;
}

void RootOutput::initSums()
{
    for(int i=0; i<numDetectors; ++i)
    {
        ostringstream histNamer;
        histNamer << "Det_" << detData->detectorNum[i] << "_2D";
        detSum2DHists[i] = new TH2F(histNamer.str().c_str(),"PSD Vs lGate Batch Sum",NumEnChannels,0,65536,NumPsdChannels,0.0,1.0);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_px_thresh";
        enProjWithCutoffSum[i] = new TH1D(histNamer.str().c_str(),"lGate Projection With Cutoff Batch Sum",NumEnChannels,0,65536);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_px";
        enProjWithoutCutoffSum[i] = new TH1D(histNamer.str().c_str(),"lGate Projection Batch Sum",NumEnChannels,0,65536);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_py_thresh";
        psdProjWithCutoffSum[i] = new TH1D(histNamer.str().c_str(),"PSD Projection With Cutoff Batch Sum",NumPsdChannels,0.0,1.0);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_py";
        psdProjWithoutCutoffSum[i] = new TH1D(histNamer.str().c_str(),"PSD Projection Batch Sum",NumPsdChannels,0.0,1.0);
    }
}

void RootOutput::initRun()
{
    numSlowCtrls = 0;
    ++runNumber;
    for(int i=0; i<numDetectors; ++i)
    {
        ostringstream histNamer;
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_2D";
        detRun2DHists[i] = new TH2F(histNamer.str().c_str(),"PSD Vs lGate",NumEnChannels,0,65536,NumPsdChannels,0.0,1.0);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_px_thresh";
        enProjWithCutoff[i] = new TH1D(histNamer.str().c_str(),"lGate Projection With Cutoff",NumEnChannels,0,65536);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_px";
        enProjWithoutCutoff[i] = new TH1D(histNamer.str().c_str(),"lGate Projection",NumEnChannels,0,65536);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_py_thresh";
        psdProjWithCutoff[i] = new TH1D(histNamer.str().c_str(),"PSD Projection With Cutoff",NumPsdChannels,0.0,1.0);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_py";
        psdProjWithoutCutoff[i] = new TH1D(histNamer.str().c_str(),"PSD Projection",NumPsdChannels,0.0,1.0);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_times";
        eventTimeHists[i] = new TH1D(histNamer.str().c_str(),"EventTimes",numTimeBin,0.0,maxTimeEdge);
        runStartTimeStamp[i] = 0;
        lastTimeStamp[i] = 0;
    }
}

void RootOutput::closeRun()
{
    outfile->cd();
    //now make prep the runs entry into the tree
    this->prepTreeEntry();
    batchTree->Fill();
    for(int i=0; i<numDetectors; ++i)
    {
        //write the per run histograms
        detRun2DHists[i]->Write();
        enProjWithCutoff[i]->Write();
        enProjWithoutCutoff[i]->Write();
        psdProjWithCutoff[i]->Write();
        psdProjWithoutCutoff[i]->Write();
        eventTimeHists[i]->Write();
        //delete the per run histograms
        delete detRun2DHists[i];
        delete enProjWithCutoff[i];
        delete enProjWithoutCutoff[i];
        delete psdProjWithCutoff[i];
        delete psdProjWithoutCutoff[i];
        delete eventTimeHists[i];
    }
    treeData.clearData();
    //flush the file to make sure hists are sent to disk
    outfile->Flush();
}

void RootOutput::closeSums()
{
    outfile->cd();
    for(int i=0; i<numDetectors; ++i)
    {
        //write the per run histograms
        detSum2DHists[i]->Write();
        enProjWithCutoffSum[i]->Write();
        enProjWithoutCutoffSum[i]->Write();
        psdProjWithCutoffSum[i]->Write();
        psdProjWithoutCutoffSum[i]->Write();
        //delete the per run histograms
        delete detSum2DHists[i];
        delete enProjWithCutoffSum[i];
        delete enProjWithoutCutoffSum[i];
        delete psdProjWithCutoffSum[i];
        delete psdProjWithoutCutoffSum[i];
    }
    //flush the file to make sure hists are sent to disk
    outfile->Flush();
}

void RootOutput::prepTree()
{
    //create the tree
    batchTree = new TTree("BatchTree","Per Run Data");
    //now create the scalar branches
    batchTree->Branch("runN", &treeData.runNumber,"runN/I");
    batchTree->Branch("startT", &treeData.startTime,"startT/l");
    batchTree->Branch("stopT", &treeData.stopTime,"stopT/l");
    batchTree->Branch("centerT", &treeData.centerTime,"centerT/l");
    batchTree->Branch("runT", &treeData.runTime,"runT/D");
    //now create the vector branches
    std::ostringstream leafNamer;
    leafNamer << "roughDtCorr["<<numDetectors<<"]/F";
    batchTree->Branch("roughDtCorr", treeData.roughCorrection, leafNamer.str().c_str());
    leafNamer.str("");
    leafNamer.clear();
    leafNamer << "avgChanVolt["<<numDetectors<<"]/F";
    batchTree->Branch("avgChanVolt", treeData.avgChanVolt, leafNamer.str().c_str());
    leafNamer.str("");
    leafNamer.clear();
    leafNamer << "avgChanCurr["<<numDetectors<<"]/F";
    batchTree->Branch("avgChanCurr", treeData.avgChanCurrent, leafNamer.str().c_str());
    leafNamer.str("");
    leafNamer.clear();
    leafNamer << "avgHVChanTemp["<<numDetectors<<"]/F";
    batchTree->Branch("avgHVChanTemp", treeData.avgHVChanTemp, leafNamer.str().c_str());
    leafNamer.str("");
    leafNamer.clear();
    leafNamer << "rawCounts["<<numDetectors<<"]/l";
    batchTree->Branch("rawCounts", treeData.avgHVChanTemp, leafNamer.str().c_str());
    leafNamer.str("");
    leafNamer.clear();
    leafNamer << "rawRates["<<numDetectors<<"]/D";
    batchTree->Branch("rawRates", treeData.avgHVChanTemp, leafNamer.str().c_str());
}

void RootOutput::prepTreeEntry()
{
    treeData.runNumber = runNumber;
    treeData.startTime = runStartEpoch;
    treeData.stopTime = lastEpoch;
    treeData.centerTime = (treeData.stopTime + treeData.startTime)/2000000;
    treeData.runTime = ((static_cast<double>(treeData.stopTime) - static_cast<double>(treeData.startTime))/1000000.0);
    treeData.setAverages(numSlowCtrls);
    treeData.setRates();
    this->doRoughDtCorrections();
}

void RootOutput::doRoughDtCorrections()
{
    for(int i=0; i< numDetectors; ++i)
    {
        //from the first bin of the time spectra to the last written bin, find the number of empty bins
        if(lastTimeStamp[i] < runStartTimeStamp[i])
        {
            lastTimeStamp[i] += 140737488355328ULL;
        }
        int lastBin = eventTimeHists[i]->FindBin((lastTimeStamp[i]-runStartTimeStamp[i])/(500000));
        float emptyBins = 0;
        float fullBins = 0;
        for(int j=1; j<lastBin; ++j)
        {
            double binContent = eventTimeHists[i]->GetBinContent(j);
            if(binContent < 1.0)
            {
                emptyBins += 1.0f;
            }
            else
            {
                fullBins += 1.0f;
            }
        }
        treeData.roughCorrection[i] = (fullBins/(fullBins+emptyBins));
    }
}

}
