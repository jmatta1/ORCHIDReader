# ORCHIDReader
Initial processing of raw ORCHID data into root histogram time series (and root trees).

## Overview
This code reads some configuration data, a detector array configuration, and a list of the raw input files to process. It then produces a time series 1D and 2D root histograms from that data as well and a small root tree containing the run statistics. Run and batch statistics are written to two csv files that can be dumped into the per batch run database and the overall batch database respectively. Currently the only optional module implemented is a module to generate a root tree in a seperate file that contains all the event data, one event per tree entry.

## Input
There are three input files for ORCHIDReader. The primary input file, passed to ORCHIDReader on the command line is a simple `Parameter = Value` format. The file in the root directory of this repository labeled *config_file* is a well commented (and runable) example of it. This file supports comments that start with a #, like comments in python

The other two files are 'pointed at' by values in the primary input file.

The file pointed to by the value of `ListFilePath` contains the list of raw data files to be processed. One file per line, the line can contain either the absolute path of the raw file or the path of the raw file relative to the directory that ORCHIDReader is executing in. This file does not support comments.

The file pointed to by the value of `ArrayDataPath` is a csv file (which supports comments that start with a # sign). It contains columns for the following data about the detectors in the array.
  - Unique detector ID number
  - Digitizer module number
  - Digitizer channel number
  - MPOD HV system module number
  - MPOD HV system channel number
  - X coordinate offset of the detector relative to the array X, Y position
  - Y coordinate offset of the detector relative to the array X, Y position
  - Z coordinate offset of the detector relative to Z = 0
  - Detector type, supported values are: NaI, LS, CeBr3, HeMod, HeUnmod
  - Energy threshold to apply to the projection onto the Pulse Shape Discrimination (PSD) axis
  - PSD threshold to apply to the projection onto the energy axis


## Extending ORCHIDReader

ORCHIDReader provides a modular processing system for handling data output from ORCHID. To add a new form of processing one need only write a class that inherits from the `OutputInterface` class, instantiate that class in `int main(int argc, int argv)`, and pass that instance to the `OutputSystem` class instance using the `addOutputClass` member function.

The `OutputInterface` class provides 5 abstract member functions to be overridden by its inheritors. Theyare listed below.
```c++
//Individual event processing functions
virtual void slowControlsEvent(const Events::OrchidSlowControlsEvent& event) = 0;
virtual void dppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event) = 0;
//larger scale processing
virtual void newRun(int runNum, unsigned long long startT) = 0;
virtual void endRun(const RunData& runData) = 0;
virtual void done() = 0;
```

The function `slowControlsEvent(const Events::OrchidSlowControlsEvent& event)` is used by `OutputSystem` to pass an `OrchidSlowControlsEvent` structure (containing information read from the MPOD HV system) to the output classes that have been added to `OutputSystem`.

The function `dppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event)` is used by `OutputSystem` to pass a `DppPsdIntegralEvent` structure (containing time stamps, integrals of the digitized pulse, and flags) to the output classes that have been added to `OutputSystem`.

`newRun(int runNum, unsigned long long startT)` is used to tell the output classes that have been added to the `OutputSystem` that they need to start a new run of processing with run number `runNum`, and time of the start of the run since epoch (1970 Jan 1 12:00am) in microseconds equal to `startT`.

`endRun(const RunData& runData)` is the function that `OutputSystem` uses to tell the classes it is managing that they need to end their current run. `runData` is a reference to a `RunData` struct that holds accumulated statistics about the run that was just finished.

The function `done()` is used to tell the `OutputInterface` subclasses registered with `OutputSystem` that there is no more data and that any processing they need to finalize needs to be completed.
