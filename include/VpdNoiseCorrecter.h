#ifndef VPD_NOISE_CORRECTER_H
#define VPD_NOISE_CORRECTER_H

// STL
#include <memory>

// ROOT
#include "TTree.h"
#include "TFile.h"


#include "TreeAnalyzer.h"
using namespace jdb;


class VpdNoiseCorrecter : public TreeAnalyzer
{
protected:
	
	map<int, int> pulserMap;
	vector<int> channelMaskEast;
	vector<int> channelMaskWest;
	map<int, int> centerEast;
	map<int, int> centerWest;



public:
	virtual const char* classname() const { return "VpdNoiseCorrecter"; }

	VpdNoiseCorrecter() {

	}
	~VpdNoiseCorrecter() {

	}

	virtual void initialize(){
	
		channelMaskEast = config.getIntVector( nodePath + ".ChannelMaskEast", 1, 16 );
		channelMaskWest = config.getIntVector( nodePath + ".ChannelMaskWest", 1, 16 );
		pulserMap = {{0,0}, { 1,0 }, {2,0}, {3,0}, {4,4}, {5,4}, {6,4}, {7,4}, {8,8}, {9,8}, {10,8}, {11,8}, {12,12}, {13,12}, {14,12}, {15,12} };

	}

	virtual void preEventLoop();


	virtual bool keepEvent( ){
		return true;
	}

	virtual void analyzeEvent();

};




#endif