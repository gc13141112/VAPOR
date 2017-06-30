//																	  *
//		   Copyright (C)  2016										*
//	 University Corporation for Atmospheric Research				  *
//		   All Rights Reserved										*
//																	  *
//************************************************************************/
//
//  File:	   Statistics.cpp
//
//  Author:	 Scott Pearse
//		  National Center for Atmospheric Research
//		  PO 3000, Boulder, Colorado
//
//  Date:	   August 2016
//
//  Description:	Implements the Statistics class.
//
#ifdef WIN32
#pragma warning(disable : 4100)
#endif
#include "vapor/glutil.h" // Must be included first!!!
#include "Statistics.h"

#include <QFileDialog>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cassert>
#include <cstdio>
#include <vapor/MyBase.h>
//#include "regionparams.h"
//#include "AnimationParams.h"

using namespace Wasp;
using namespace VAPoR;
using namespace std;

Statistics::Statistics(QWidget* parent) : 
QDialog(parent), Ui_StatsWindow(){
	
	_errMsg = NULL;
	_initialized = 0;
	_slidersInitialized = false;
	_regionInitialized = false;

	_xRange = NULL;
	_xMinSlider = NULL;
	_xMaxSlider = NULL;
	_xMinLineEdit = NULL;
	_xMaxLineEdit = NULL;
	_xSinglePointSlider = NULL;
	_xSinglePointLineEdit = NULL;
	_xCenterSlider = NULL;
	_xSizeSlider = NULL;
	_xCenterLineEdit = NULL;
	_xSizeLineEdit = NULL;
	_xMinCell = NULL;
	_xMaxCell = NULL;
	_minXMinLabel = NULL;
	_minXMaxLabel = NULL;
	_maxXMinLabel = NULL;
	_maxXMaxLabel = NULL;
	_centerXMinLabel = NULL;
	_centerXMaxLabel = NULL;
	_sizeXMinLabel = NULL;
	_sizeXMaxLabel = NULL;
	_spXMinLabel = NULL;
	_spXMaxLabel = NULL;
	
	_yRange = NULL;
	_yMinSlider = NULL;
	_yMaxSlider = NULL;
	_yMinLineEdit = NULL;
	_yMaxLineEdit = NULL;
	_ySinglePointSlider = NULL;
	_ySinglePointLineEdit = NULL;
	_yCenterSlider = NULL;
	_ySizeSlider = NULL;
	_yCenterLineEdit = NULL;
	_ySizeLineEdit = NULL;
	_yMinCell = NULL;
	_yMaxCell = NULL;
	_minYMinLabel = NULL;
	_minYMaxLabel = NULL;
	_maxYMinLabel = NULL;
	_maxYMaxLabel = NULL;
	_centerYMinLabel = NULL;
	_centerYMaxLabel = NULL;
	_sizeYMinLabel = NULL;
	_sizeYMaxLabel = NULL;
	_spYMinLabel = NULL;
	_spYMaxLabel = NULL;
	
	_zRange = NULL;
	_zMinSlider = NULL;
	_zMaxSlider = NULL;
	_zMinLineEdit = NULL;
	_zMaxLineEdit = NULL;
	_zSinglePointSlider = NULL;
	_zSinglePointLineEdit = NULL;
	_zCenterSlider = NULL;
	_zSizeSlider = NULL;
	_zCenterLineEdit = NULL;
	_zSizeLineEdit = NULL;
	_zMinCell = NULL;
	_zMaxCell = NULL;
	_minZMinLabel = NULL;
	_minZMaxLabel = NULL;
	_maxZMinLabel = NULL;
	_maxZMaxLabel = NULL;
	_centerZMinLabel = NULL;
	_centerZMaxLabel = NULL;
	_sizeZMinLabel = NULL;
	_sizeZMaxLabel = NULL;
	_spZMinLabel = NULL;
	_spZMaxLabel = NULL;

	_dm = NULL;
	_rGrid = NULL;
	
	setupUi(this);
	setWindowTitle("Statistics");
	adjustTables();
}

Statistics::~Statistics() {
	if (_errMsg) delete _errMsg;
	
	if (_zRange) delete _zRange;
	if (_zMinSlider) delete _zMinSlider;
	if (_zMaxSlider) delete _zMaxSlider;
	if (_zMinLineEdit) delete _zMinLineEdit;
	if (_zMaxLineEdit) delete _zMaxLineEdit;
	if (_zCenterSlider) delete _zCenterSlider;
	if (_zCenterLineEdit) delete _zCenterLineEdit;
	if (_zSizeSlider) delete _zSizeSlider;
	if (_zSizeLineEdit) delete _zSizeLineEdit;
	if (_zSinglePointSlider) delete _zSinglePointSlider;
	if (_zSinglePointLineEdit) delete _zSinglePointLineEdit;

	if (_yRange) delete _yRange;
	if (_yMinSlider) delete _yMinSlider;
	if (_yMaxSlider) delete _yMaxSlider;
	if (_yMinLineEdit) delete _yMinLineEdit;
	if (_yMaxLineEdit) delete _yMaxLineEdit;
	if (_yCenterSlider) delete _yCenterSlider;
	if (_yCenterLineEdit) delete _yCenterLineEdit;
	if (_ySizeSlider) delete _ySizeSlider;
	if (_ySizeLineEdit) delete _ySizeLineEdit;
	if (_ySinglePointSlider) delete _ySinglePointSlider;
	if (_ySinglePointLineEdit) delete _ySinglePointLineEdit;
	if (_xRange) delete _xRange;

	if (_xMinSlider) delete _xMinSlider;
	if (_xMaxSlider) delete _xMaxSlider;
	if (_xMinLineEdit) delete _xMinLineEdit;
	if (_xMaxLineEdit) delete _xMaxLineEdit;
	if (_xCenterSlider) delete _xCenterSlider;
	if (_xCenterLineEdit) delete _xCenterLineEdit;
	if (_xSizeSlider) delete _xSizeSlider;
	if (_xSizeLineEdit) delete _xSizeLineEdit;
	if (_xSinglePointSlider) delete _xSinglePointSlider;
	if (_xSinglePointLineEdit) delete _xSinglePointLineEdit;
}

int Statistics::initDataMgr(DataMgr* dm) {
	if (dm!=NULL) {
		_dm = dm;
	}
	else { return -1;}
	initialize();
	return 0;
}

int Statistics::initialize(){

	// This is a bitmask to define which statistics to calculate/display.
	// If a statistic variable is set to 0x00 or undefined, it will not
	// be applied.  The _calculations variable is used as a filter, and
	// is all-inclusive by default.
	//
	_MIN = 0x01;
	_MAX = 0x02;
	_MEAN = 0x04;
	_MEDIAN = 0x00;
	_SIGMA = 0x00;
	_calculations = 0xFF;

	if (_dm==NULL) return -1;
	if (_initialized==1) return 0;

	_errMsg = new sErrMsg;
	_autoUpdate = false;

	// for _regionSelection,
	// 0 = center/size, 1 = min/max, 2 = center/size
	//
	_regionSelection = 0;
	stackedSliderWidget->setCurrentIndex(0);

	generateTableColumns();

	_defaultVar = _dm->GetDataVarNames()[0];
	if (_defaultVar=="") return -1;
	initTimes();

	int rc = GetExtents(_extents);
	if (rc<0) return -1;
	_fullExtents = _extents;

	initVariables();
	initCRatios();
	initRefinement();
	initRegion();
	initRangeControllers();

	ExportButton->setEnabled(true);

	connect(MinTimestepSpinbox, SIGNAL(valueChanged(int)), this, SLOT(minTSChanged()));
	connect(MaxTimestepSpinbox, SIGNAL(valueChanged(int)), this, SLOT(maxTSChanged()));
	connect(UpdateCheckbox, SIGNAL(stateChanged(int)), this, SLOT(autoUpdateClicked()));
	connect(UpdateButton, SIGNAL(pressed()), this, SLOT(updateButtonPressed()));
	connect(RefCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(refinementChanged(int)));
	connect(CRatioCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(cRatioChanged(int)));
	connect(NewVarCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(newVarAdded(int)));
	connect(RestoreExtentsButton, SIGNAL(pressed()), this, SLOT(restoreExtents()));
	connect(RemoveVarCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(varRemoved(int)));
	connect(ExportButton, SIGNAL(clicked()), this, SLOT(exportText()));
	connect(regionSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(regionSlidersChanged()));
	connect(copyActiveRegionButton, SIGNAL(pressed()), this, SLOT(copyActiveRegion()));
	connect(addStatCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(addStatistic(int)));
	connect(removeStatCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(removeStatistic(int)));
	
	_initialized = 1;
	
	return 0;
}

void Statistics::addStatistic(int index) {
	if (index == 0) return;
	string statName = addStatCombo->currentText().toStdString();

	if (statName == "Min") _MIN = 0x01;
	if (statName == "Max") _MAX = 0x02;
	if (statName == "Mean") _MEAN = 0x04;
	if (statName == "Median") _MEDIAN = 0x10;
	if (statName == "StdDev") _SIGMA = 0x08;

	VariablesTable->resizeColumnsToContents();
	addStatCombo->setCurrentIndex(0);

	update();
}

void Statistics::removeStatistic(int index) {
	if (index == 0) return;
	string statName = removeStatCombo->currentText().toStdString();

	if (statName == "Min") _MIN = 0x00;
	if (statName == "Max") _MAX = 0x00;
	if (statName == "Mean") _MEAN = 0x00;
	if (statName == "Median") _MEDIAN = 0x00;
	if (statName == "StdDev") _SIGMA = 0x00;

	VariablesTable->resizeColumnsToContents();
	removeStatCombo->setCurrentIndex(0);

	update();
}

void Statistics::errReport(string msg) const {
	_errMsg->errorList->setText(QString::fromStdString(msg));
	_errMsg->show();
	_errMsg->raise();
	_errMsg->activateWindow();
}

void Statistics::initTimes() {
	_minTS = 0;
	_maxTS = 0;
	
	MinTimestepSpinbox->setMinimum(0);
	MinTimestepSpinbox->setMaximum(_dm->GetNumTimeSteps(_defaultVar)-1);
	MinTimestepSpinbox->setValue(0);

	MaxTimestepSpinbox->setMinimum(0);
	MaxTimestepSpinbox->setMaximum(_dm->GetNumTimeSteps(_defaultVar)-1);
	MaxTimestepSpinbox->setValue(0);
}

void Statistics::initRangeControllers() {

	// Set up x axis controllers
	//
	if (_xRange) delete _xRange;
	if (_xMinSlider) delete _xMinSlider;
	if (_xMaxSlider) delete _xMaxSlider;
	if (_xMinLineEdit) delete _xMinLineEdit;
	if (_xMaxLineEdit) delete _xMaxLineEdit;
	if (_xCenterSlider) delete _xCenterSlider;
	if (_xCenterLineEdit) delete _xCenterLineEdit;
	if (_xSizeSlider) delete _xSizeSlider;
	if (_xSizeLineEdit) delete _xSizeLineEdit;
	if (_xSinglePointSlider) delete _xSinglePointSlider;
	if (_xSinglePointLineEdit) delete _xSinglePointLineEdit;
	_xRange = new Range(_fullExtents[0],_fullExtents[3]);
	_xMinSlider = new MinMaxSlider(_xRange, minXSlider, 0);
	_xMaxSlider = new MinMaxSlider(_xRange, maxXSlider, 1);
	_xSinglePointSlider = new SinglePointSlider(_xRange, spXSlider);
	double xMid = (_fullExtents[0]+_fullExtents[3])/2.f;
	_xSinglePointLineEdit = new SinglePointLineEdit(_xRange, spXEdit,xMid);
	_xMinLineEdit = new MinMaxLineEdit(_xRange, minXEdit, 0);
	_xMaxLineEdit = new MinMaxLineEdit(_xRange, maxXEdit, 1);
	_xCenterSlider = new CenterSizeSlider(_xRange, centerXSlider, 0);
	_xSizeSlider = new CenterSizeSlider(_xRange, sizeXSlider, 1);
	_xCenterLineEdit = new CenterSizeLineEdit(_xRange, centerXEdit, 0);
	_xSizeLineEdit = new CenterSizeLineEdit(_xRange, sizeXEdit, 1);
	_xMinCell = new MinMaxTableCell(_xRange, RegionTable, 0, 0, 0);
	_xMaxCell = new MinMaxTableCell(_xRange, RegionTable, 1, 1, 0);
	_minXMinLabel = new MinMaxLabel(_xRange, minXMinLabel, 0);
	_minXMaxLabel = new MinMaxLabel(_xRange, minXMaxLabel, 1);
	_maxXMinLabel = new MinMaxLabel(_xRange, maxXMinLabel, 0);
	_maxXMaxLabel = new MinMaxLabel(_xRange, maxXMaxLabel, 1);
	_centerXMinLabel = new MinMaxLabel(_xRange, centerXMinLabel, 0);
	_centerXMaxLabel = new MinMaxLabel(_xRange, centerXMaxLabel, 1);
	_sizeXMinLabel = new SizeLabel(_xRange, sizeXMinLabel, 0);
	_sizeXMaxLabel = new SizeLabel(_xRange, sizeXMaxLabel, 1);
	_spXMinLabel = new MinMaxLabel(_xRange, spXMinLabel, 0);
	_spXMaxLabel = new MinMaxLabel(_xRange, spXMaxLabel, 1);
	_xRange->addObserver(_minXMinLabel);
	_xRange->addObserver(_minXMaxLabel);
	_xRange->addObserver(_maxXMinLabel);
	_xRange->addObserver(_maxXMaxLabel);
	_xRange->addObserver(_centerXMinLabel);
	_xRange->addObserver(_centerXMaxLabel);
	_xRange->addObserver(_sizeXMinLabel);
	_xRange->addObserver(_sizeXMaxLabel);
	_xRange->addObserver(_spXMinLabel);
	_xRange->addObserver(_spXMaxLabel);
	_xRange->addObserver(_xMinSlider);
	_xRange->addObserver(_xMaxSlider);
	_xRange->addObserver(_xSinglePointSlider);
	_xRange->addObserver(_xMinLineEdit);
	_xRange->addObserver(_xMaxLineEdit);
	_xRange->addObserver(_xCenterSlider);
	_xRange->addObserver(_xSizeSlider);
	_xRange->addObserver(_xCenterLineEdit);
	_xRange->addObserver(_xSizeLineEdit);
	_xRange->addObserver(_xMinCell);
	_xRange->addObserver(_xMaxCell);

	// Set up y axis controllers
	//
	if (_yRange) delete _yRange;
	if (_yMinSlider) delete _yMinSlider;
	if (_yMaxSlider) delete _yMaxSlider;
	if (_yMinLineEdit) delete _yMinLineEdit;
	if (_yMaxLineEdit) delete _yMaxLineEdit;
	if (_yCenterSlider) delete _yCenterSlider;
	if (_yCenterLineEdit) delete _yCenterLineEdit;
	if (_ySizeSlider) delete _ySizeSlider;
	if (_ySizeLineEdit) delete _ySizeLineEdit;
	if (_ySinglePointSlider) delete _ySinglePointSlider;
	if (_ySinglePointLineEdit) delete _ySinglePointLineEdit;
	_yRange = new Range(_fullExtents[1],_fullExtents[4]);
	_yMinSlider = new MinMaxSlider(_yRange, minYSlider, 0);
	_yMaxSlider = new MinMaxSlider(_yRange, maxYSlider, 1);
	_ySinglePointSlider = new SinglePointSlider(_yRange, spYSlider);
	double yMid = (_fullExtents[1]+_fullExtents[4])/2.f;
	_ySinglePointLineEdit = new SinglePointLineEdit(_yRange, spYEdit, yMid);
	_yMinLineEdit = new MinMaxLineEdit(_yRange, minYEdit, 0);
	_yMaxLineEdit = new MinMaxLineEdit(_yRange, maxYEdit, 1);
	_yCenterSlider = new CenterSizeSlider(_yRange, centerYSlider, 0);
	_ySizeSlider = new CenterSizeSlider(_yRange, sizeYSlider, 1);
	_yCenterLineEdit = new CenterSizeLineEdit(_yRange, centerYEdit, 0);
	_ySizeLineEdit = new CenterSizeLineEdit(_yRange, sizeYEdit, 1);
	_yMinCell = new MinMaxTableCell(_yRange, RegionTable, 0, 0, 1);
	_yMaxCell = new MinMaxTableCell(_yRange, RegionTable, 1, 1, 1);
	_minYMinLabel = new MinMaxLabel(_yRange, minYMinLabel, 0);
	_minYMaxLabel = new MinMaxLabel(_yRange, minYMaxLabel, 1);
	_maxYMinLabel = new MinMaxLabel(_yRange, maxYMinLabel, 0);
	_maxYMaxLabel = new MinMaxLabel(_yRange, maxYMaxLabel, 1);
	_centerYMinLabel = new MinMaxLabel(_yRange, centerYMinLabel, 0);
	_centerYMaxLabel = new MinMaxLabel(_yRange, centerYMaxLabel, 1);
	_sizeYMinLabel = new SizeLabel(_yRange, sizeYMinLabel, 0);
	_sizeYMaxLabel = new SizeLabel(_yRange, sizeYMaxLabel, 1);
	_spYMinLabel = new MinMaxLabel(_yRange, spYMinLabel, 0);
	_spYMaxLabel = new MinMaxLabel(_yRange, spYMaxLabel, 1);
	_yRange->addObserver(_minYMinLabel);
	_yRange->addObserver(_minYMaxLabel);
	_yRange->addObserver(_maxYMinLabel);
	_yRange->addObserver(_maxYMaxLabel);
	_yRange->addObserver(_centerYMinLabel);
	_yRange->addObserver(_centerYMaxLabel);
	_yRange->addObserver(_sizeYMinLabel);
	_yRange->addObserver(_sizeYMaxLabel);
	_yRange->addObserver(_spYMinLabel);
	_yRange->addObserver(_spYMaxLabel);
	_yRange->addObserver(_yMinSlider);
	_yRange->addObserver(_yMaxSlider);
	_yRange->addObserver(_ySinglePointSlider);
	_yRange->addObserver(_ySinglePointLineEdit);
	_yRange->addObserver(_yMinLineEdit);
	_yRange->addObserver(_yMaxLineEdit);
	_yRange->addObserver(_yCenterSlider);
	_yRange->addObserver(_ySizeSlider);
	_yRange->addObserver(_yCenterLineEdit);
	_yRange->addObserver(_ySizeLineEdit);
	_yRange->addObserver(_yMinCell);
	_yRange->addObserver(_yMaxCell);

	// Set up z axis controllers
	//
	if (_zRange) delete _zRange;
	if (_zMinSlider) delete _zMinSlider;
	if (_zMaxSlider) delete _zMaxSlider;
	if (_zMinLineEdit) delete _zMinLineEdit;
	if (_zMaxLineEdit) delete _zMaxLineEdit;
	if (_zCenterSlider) delete _zCenterSlider;
	if (_zCenterLineEdit) delete _zCenterLineEdit;
	if (_zSizeSlider) delete _zSizeSlider;
	if (_zSizeLineEdit) delete _zSizeLineEdit;
	if (_zSinglePointSlider) delete _zSinglePointSlider;
	if (_zSinglePointLineEdit) delete _zSinglePointLineEdit;
	_zRange = new Range(_fullExtents[2],_fullExtents[5]);
	_zMinSlider = new MinMaxSlider(_zRange, minZSlider, 0);
	_zMaxSlider = new MinMaxSlider(_zRange, maxZSlider, 1);
	_zSinglePointSlider = new SinglePointSlider(_zRange, spZSlider);
	double zMid = (_fullExtents[2]+_fullExtents[5])/2.f;
	_zSinglePointLineEdit = new SinglePointLineEdit(_zRange, spZEdit, zMid);
	_zMinLineEdit = new MinMaxLineEdit(_zRange, minZEdit, 0);
	_zMaxLineEdit = new MinMaxLineEdit(_zRange, maxZEdit, 1);
	_zCenterSlider = new CenterSizeSlider(_zRange, centerZSlider,0);
	_zSizeSlider = new CenterSizeSlider(_zRange, sizeZSlider ,1);
	_zCenterLineEdit = new CenterSizeLineEdit(_zRange, centerZEdit, 0);
	_zSizeLineEdit = new CenterSizeLineEdit(_zRange, sizeZEdit, 1);
	_zMinCell = new MinMaxTableCell(_zRange, RegionTable, 0, 0, 2);
	_zMaxCell = new MinMaxTableCell(_zRange, RegionTable, 1, 1, 2);
	_minZMinLabel = new MinMaxLabel(_zRange, minZMinLabel, 0);
	_minZMaxLabel = new MinMaxLabel(_zRange, minZMaxLabel, 1);
	_maxZMinLabel = new MinMaxLabel(_zRange, maxZMinLabel, 0);
	_maxZMaxLabel = new MinMaxLabel(_zRange, maxZMaxLabel, 1);
	_centerZMinLabel = new MinMaxLabel(_zRange, centerZMinLabel, 0);
	_centerZMaxLabel = new MinMaxLabel(_zRange, centerZMaxLabel, 1);
	_sizeZMinLabel = new SizeLabel(_zRange, sizeZMinLabel, 0);
	_sizeZMaxLabel = new SizeLabel(_zRange, sizeZMaxLabel, 1);
	_spZMinLabel = new MinMaxLabel(_zRange, spZMinLabel, 0);
	_spZMaxLabel = new MinMaxLabel(_zRange, spZMaxLabel, 1);
	_zRange->addObserver(_minZMinLabel);
	_zRange->addObserver(_minZMaxLabel);
	_zRange->addObserver(_maxZMinLabel);
	_zRange->addObserver(_maxZMaxLabel);
	_zRange->addObserver(_centerZMinLabel);
	_zRange->addObserver(_centerZMaxLabel);
	_zRange->addObserver(_sizeZMinLabel);
	_zRange->addObserver(_sizeZMaxLabel);
	_zRange->addObserver(_spZMinLabel);
	_zRange->addObserver(_spZMaxLabel);
	_zRange->addObserver(_zMinSlider);
	_zRange->addObserver(_zMaxSlider);
	_zRange->addObserver(_zSinglePointSlider);
	_zRange->addObserver(_zSinglePointLineEdit);
	_zRange->addObserver(_zMinLineEdit);
	_zRange->addObserver(_zMaxLineEdit);
	_zRange->addObserver(_zCenterSlider);
	_zRange->addObserver(_zSizeSlider);
	_zRange->addObserver(_zCenterLineEdit);
	_zRange->addObserver(_zSizeLineEdit);
	_zRange->addObserver(_zMinCell);
	_zRange->addObserver(_zMaxCell);

	_slidersInitialized = true;
}

void Statistics::initCRatios() {
	_cRatios = _dm->GetCRatios(_defaultVar);
	_cRatio = _cRatios.size()-1;

	for (std::vector<size_t>::iterator it = _cRatios.begin(); it != _cRatios.end(); ++it){
		CRatioCombo->addItem("1:"+QString::number(*it));
	}

	CRatioCombo->setCurrentIndex(_cRatio);
}

void Statistics::initRefinement() {
	_refLevel = _dm->GetNumRefLevels(_defaultVar);

	for (int i=0; i<=_refLevel; i++){
		RefCombo->addItem(QString::number(i));
	}
	RefCombo->setCurrentIndex(_refLevel);
}

void Statistics::copyActiveRegion(){
/*	RegionParams* rParams = VizWinMgr::getActiveRegionParams();
	AnimationParams* aParams = VizWinMgr::getActiveAnimationParams();
	int currentTS = aParams->GetCurrentTimestep();
	double min[3],max[3];
	for (int i = 0; i< 3; i++){
		min[i] = rParams->getLocalRegionMin(i,currentTS) + _fullExtents[i];
		max[i] = rParams->getLocalRegionMax(i,currentTS) + _fullExtents[i];
	}

	_xRange->setUserMin(min[0]);
	_xRange->setUserMax(max[0]);
	_yRange->setUserMin(min[1]);
	_yRange->setUserMax(max[1]);
	_zRange->setUserMin(min[2]);
	_zRange->setUserMax(max[2]);
*/
}

// In Vapor 2.x, the function call:
//
//     _extents = _dm->GetExtents(_minTS);
//
// would set our 6 element _extents vector.
// In Vapor 3.x, we receive two sets of thre
// element vectors instead, so we need to
// backfill these values into our old 6 element
// _extents vector.
//
int Statistics::GetExtents(vector<double>& extents) {
	vector<double> minExtents, maxExtents;
	int rc = _dm->GetVariableExtents(_minTS, _defaultVar, _refLevel,
									minExtents, maxExtents);
	if (rc<0) {
		string myErr;
		myErr = "Could not find minimum and maximun extents in current data set."; 
		errReport(myErr);
		return -1;
	}
	if (extents.size() < 6) {
		extents.push_back(minExtents[0]);
		extents.push_back(minExtents[1]);
		extents.push_back(minExtents[2]);
		extents.push_back(maxExtents[0]);
		extents.push_back(maxExtents[1]);
		extents.push_back(maxExtents[2]);

		_uCoordMin.push_back(minExtents[0]);
		_uCoordMin.push_back(minExtents[1]);
		_uCoordMin.push_back(minExtents[2]);
		_uCoordMax.push_back(maxExtents[0]);
		_uCoordMax.push_back(maxExtents[1]);
		_uCoordMax.push_back(maxExtents[2]);
	}
	else {
		extents[0] = minExtents[0];
		extents[1] = minExtents[1];
		extents[2] = minExtents[2];
		extents[3] = maxExtents[0];
		extents[4] = maxExtents[1];
		extents[5] = maxExtents[2];
	}
	return 1;
}

void Statistics::restoreExtents() {
	vector<double> extents;
	
	int rc = GetExtents(extents);
	if (!rc) return;

	_xRange->setUserMin(extents[0]);
	_xRange->setUserMax(extents[3]);
	_yRange->setUserMin(extents[1]);
	_yRange->setUserMax(extents[4]);
	_zRange->setUserMin(extents[2]);
	_zRange->setUserMax(extents[5]);
}

void Statistics::initRegion(bool activeRegion){
	// Save our old extents
	//
	vector<double> oldExtents(_fullExtents);

	// Get new extents of the minimum timestep
	// and apply them to our saved set of full
	// extents
	//
	int rc = GetExtents(_extents);
	if (!rc) return;
	_fullExtents = _extents;

	// If the extents for our new timestep 
	// are different from our old extents,
	// reset the region sliders.
	//
	if (((oldExtents[0] == _fullExtents[0]) &&
	(oldExtents[1] == _fullExtents[1]) &&
	(oldExtents[2] == _fullExtents[2]) &&
	(oldExtents[3] == _fullExtents[3]) &&
	(oldExtents[4] == _fullExtents[4]) &&
	(oldExtents[5] == _fullExtents[5]))) {
		return;
	}
	else if (_regionInitialized) setNewExtents();
	
	_resettingRegion = 1;

	QTableWidgetItem* twi;
	for (int i=0; i<2; i++){
		for (int j=0; j<3; j++){
			if (!_regionInitialized){	
				twi = new QTableWidgetItem(QString::number(_extents[i*3+j]));
				twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				Qt::ItemFlags flags = twi->flags();
				twi->setFlags(flags);
				RegionTable->setItem(i,j,twi);
			}
			else {
				twi = RegionTable->item(i,j);
				twi->setText(QString::number(_extents[i*3+j]));
			}
		}
	}
	_regionInitialized = 1;

	_resettingRegion = 0;
	if (_slidersInitialized) updateSliders();
	if (_autoUpdate) update();
	else (makeItRed());
}

void Statistics::setNewExtents() {
	_xRange->setDomainMin(_fullExtents[0]);
	_xRange->setDomainMin(_fullExtents[1]);
	_yRange->setDomainMin(_fullExtents[2]);
	_yRange->setDomainMin(_fullExtents[3]);
	_zRange->setDomainMin(_fullExtents[4]);
	_zRange->setDomainMin(_fullExtents[5]);
}

void Statistics::updateSliders() {
	_xRange->setUserMin(_extents[0]);
	_xRange->setUserMax(_extents[3]);
	_yRange->setUserMin(_extents[1]);
	_yRange->setUserMax(_extents[4]);
	_zRange->setUserMin(_extents[2]);
	_zRange->setUserMax(_extents[5]);
}

int Statistics::initVariables() {
	vector<string> vars;
	vars = _dm->GetDataVarNames();
	for (std::vector<string>::iterator it = vars.begin(); it != vars.end(); ++it){
		_vars.push_back(*it);
		_vars3d.push_back(*it);
	}

	sort(_vars.begin(), _vars.end());
	
	// Add variables to combo box
	//
	for (std::vector<string>::iterator it = _vars.begin(); it != _vars.end(); ++it){
		NewVarCombo->addItem(QString::fromStdString(*it));
	}

	return 0;
}

void Statistics::adjustTables(){
	VariablesTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	VariablesTable->verticalHeader()->setResizeMode(QHeaderView::Stretch);
	VariablesTable->resizeRowsToContents();
	VariablesTable->resizeColumnsToContents();

	RegionTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	RegionTable->verticalHeader()->setResizeMode(QHeaderView::Stretch);
	RegionTable->resizeRowsToContents();
	RegionTable->resizeColumnsToContents();
}

void Statistics::showMe() {
	show();
	raise();
	activateWindow();
	if (!_dm) return;
}

void Statistics::makeItRed() {
	size_t rows = VariablesTable->rowCount();
	size_t cols = VariablesTable->columnCount();
	QTableWidgetItem* twi;
	QBrush brush(QColor(255,0,0));

	for (size_t i=0; i<rows; i++){
		for (size_t j=0; j<cols; j++) {
			twi = VariablesTable->item(i,j);
			if (twi != NULL) twi->setForeground(brush);
		}
	}
}

void Statistics::maxTSChanged() {
	int min = MinTimestepSpinbox->value();
	int max = MaxTimestepSpinbox->value();

	if (max < min) {
		min = max;
		MinTimestepSpinbox->setValue(max);
	}

	if ((min != _minTS) || (max != _maxTS)) {
		_minTS = min;
		_maxTS = max;
	}
	if (_autoUpdate) update();
	else (makeItRed());
}

void Statistics::minTSChanged() {
	int min = MinTimestepSpinbox->value();
	int max = MaxTimestepSpinbox->value();

	if (min > max) {
		max = min;
		MaxTimestepSpinbox->setValue(min);
	}

	if ((min != _minTS) || (max != _maxTS)) {
		_minTS = min;
		_maxTS = max;
	}
	initRegion();
	if (_autoUpdate) update();
	else (makeItRed());
}

void Statistics::autoUpdateClicked() {
	if (UpdateCheckbox->isChecked()) _autoUpdate = true;
	else _autoUpdate = false;
	UpdateButton->setEnabled(!_autoUpdate);
	if (_autoUpdate) update();
	else (makeItRed());
}

void Statistics::refinementChanged(int index) {
	_refLevel = index;
	if (_autoUpdate) update();
	else (makeItRed());
}

void Statistics::cRatioChanged(int index) {
	_cRatio = index;
	if (_autoUpdate) update();
	else (makeItRed());
}

void Statistics::refreshTable() {
	VariablesTable->clear();
	VariablesTable->setRowCount(0);
	VariablesTable->setColumnCount(0);

	// First generate the layout of our VariablesTable
	//
	generateTableColumns();
}

void Statistics::update() {

	refreshTable();

	_uCoordMin[0] = _extents[0] = _xRange->getUserMin();
	_uCoordMin[1] = _extents[1] = _yRange->getUserMin();
	_uCoordMin[2] = _extents[2] = _zRange->getUserMin();
	_uCoordMax[0] = _extents[3] = _xRange->getUserMax();
	_uCoordMax[1] = _extents[4] = _yRange->getUserMax();
	_uCoordMax[2] = _extents[5] = _zRange->getUserMax();

	string varName;
	typedef std::map<string, _statistics>::iterator it_type;

	// Disable error reporting. Under VAPOR 2.x any errors result in
	// a callback that can trigger an infinite cascade of error msg
	// popups :-(
	//
	bool enable = MyBase::EnableErrMsg(false);
	bool success = true;
	for (it_type it = _stats.begin(); it!=_stats.end(); it++){
		varName = it->first;

		if (_calculations & _MEAN) {
			success &= calcMean(varName);
		}
		if (_calculations & _SIGMA) {
			success &= calcStdDev(varName);
		}
		if (_calculations & _MEDIAN) {
			success &= calcMedian(varName);
		}
	
		addCalculationToTable(varName);	
	}

	if (! success) {
		string myErr;
		myErr = "Warning: Not all requested variables and/or timesteps available.\n"
		"Some statistics may be incorrect!\n";
		errReport(myErr);
	}

	// Restore error reporting
	//
	MyBase::EnableErrMsg(enable);

	VariablesTable->resizeRowsToContents();
}

void Statistics::addCalculationToTable(string varName) {
	int rowCount = VariablesTable->rowCount();
	VariablesTable->insertRow(rowCount);
	VariablesTable->setVerticalHeaderItem(rowCount, new QTableWidgetItem(QString::fromStdString(varName)));

	QTableWidgetItem* twi;

	unsigned char calcCopy = _calculations;	
	int colCount = VariablesTable->columnCount();
	for (int i=0; i<colCount; i++) {
		if (calcCopy & _MIN) {
			twi = new QTableWidgetItem(QString::number(_stats[varName].min));
			VariablesTable->setItem(rowCount, i, twi);
			calcCopy = calcCopy - _MIN;
		}
		else if (calcCopy & _MAX) {
			twi = new QTableWidgetItem(QString::number(_stats[varName].max));
			VariablesTable->setItem(rowCount, i, twi);
			calcCopy = calcCopy - _MAX;
		}
		else if (calcCopy & _MEAN) {
			twi = new QTableWidgetItem(QString::number(_stats[varName].mean));
			VariablesTable->setItem(rowCount, i, twi);
			calcCopy = calcCopy - _MEAN;
		}
		else if (calcCopy & _MEDIAN) {
			twi = new QTableWidgetItem(QString::number(_stats[varName].median));
			VariablesTable->setItem(rowCount, i, twi);
			calcCopy = calcCopy - _MEDIAN;
		}
		else if (calcCopy & _SIGMA) {
			twi = new QTableWidgetItem(QString::number(_stats[varName].stddev));
			VariablesTable->setItem(rowCount, i, twi);
			calcCopy = calcCopy - _SIGMA;
		}
		twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	}
	VariablesTable->resizeRowToContents(rowCount);
	VariablesTable->setRowHeight(rowCount, 20);
}

void Statistics::generateTableColumns() {
	// Generate statistic columns in variables talbe
	//
	int colCount;


	if (_calculations & _MIN) {
		colCount = VariablesTable->columnCount();
		VariablesTable->insertColumn(colCount);
		VariablesTable->setHorizontalHeaderItem(colCount, new QTableWidgetItem(QString::fromStdString("Min")));
	}
	if (_calculations & _MAX) {
		colCount = VariablesTable->columnCount();
		VariablesTable->insertColumn(colCount);
		VariablesTable->setHorizontalHeaderItem(colCount, new QTableWidgetItem(QString::fromStdString("Max")));
	}
	if (_calculations & _MEAN) {
		colCount = VariablesTable->columnCount();
		VariablesTable->insertColumn(colCount);
		VariablesTable->setHorizontalHeaderItem(colCount, new QTableWidgetItem(QString::fromStdString("Mean")));
	}
	if (_calculations & _MEDIAN) {
		colCount = VariablesTable->columnCount();
		VariablesTable->insertColumn(colCount);
		VariablesTable->setHorizontalHeaderItem(colCount, new QTableWidgetItem(QString::fromStdString("Median")));
	}
	if (_calculations & _SIGMA) {
		colCount = VariablesTable->columnCount();
		VariablesTable->insertColumn(colCount);
		VariablesTable->setHorizontalHeaderItem(colCount, new QTableWidgetItem(QString::fromStdString("StdDev")));
	}
}

void Statistics::exportText() {
	_extents[0] = _xRange->getUserMin();
	_extents[1] = _yRange->getUserMin();
	_extents[2] = _zRange->getUserMin();
	_extents[3] = _xRange->getUserMax();
	_extents[4] = _yRange->getUserMax();
	_extents[5] = _zRange->getUserMax();

	QString fName = QFileDialog::getSaveFileName(this,"Select file to write statistics into:");
	ofstream file;
	file.open(fName.toStdString().c_str());

	file << "Variable Statistics\nVariable,Min,Max,Mean,StdDev" << endl;

	typedef std::map<string, _statistics>::iterator it_type;
	for (it_type it = _stats.begin(); it!=_stats.end(); it++){
		file << it->first << ",";
		file << it->second.min << ",";
		file << it->second.max << ",";
		file << it->second.mean << ",";
		file << it->second.stddev;
		file << endl;
	}

	file << endl;

	file << "Dependent Variable\nDimension,Min,Max" << endl;
	file << "X," << _extents[0] << "," << _extents[3] << endl;
	file << "Y," << _extents[1] << "," << _extents[4] << endl;
	file << "Z," << _extents[2] << "," << _extents[5] << endl;

	file << endl;

	file << "Temporal Extents\nStartTime,EndTime" << endl;
	file << _minTS << "," << _maxTS << endl;

	file.close();
}

void Statistics::varRemoved(int index) {
	if (index == 0) return;
	string varName = RemoveVarCombo->currentText().toStdString();
	_stats.erase(varName);
	RemoveVarCombo->setCurrentIndex(0);
	RemoveVarCombo->removeItem(index);
	update();
}

void Statistics::newVarAdded(int index) {
	if (index == 0) return;
	string varName = NewVarCombo->currentText().toStdString();

	typedef std::map<string, _statistics>::iterator it_type;
	for (it_type it = _stats.begin(); it!=_stats.end(); it++){
		if (it->first == varName) return;
	}

	_stats[varName] = _statistics();

	int rowCount = VariablesTable->rowCount();
	VariablesTable->insertRow(rowCount);
	VariablesTable->setVerticalHeaderItem(rowCount, new QTableWidgetItem(QString::fromStdString(varName)));

	QHeaderView *verticalHeader = VariablesTable->verticalHeader();
	verticalHeader->setResizeMode(QHeaderView::Fixed);
	verticalHeader->setDefaultSectionSize(20);

	int colCount = VariablesTable->columnCount();
	for (int j=0; j<colCount; j++){
		QTableWidgetItem* twi = new QTableWidgetItem("");
		twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		VariablesTable->setItem(rowCount,j,twi);
	}

	NewVarCombo->setCurrentIndex(0);

	RemoveVarCombo->addItem(QString::fromStdString(varName));
	VariablesTable->resizeRowsToContents();

	if (_autoUpdate) update();
	else (makeItRed());
}

void Statistics::regionSlidersChanged() {
	QString text = regionSelectorCombo->currentText();
	int index = regionSelectorCombo->currentIndex();
	stackedSliderWidget->setCurrentIndex(index);
	_regionSelection = index;
	
	_extents[0] = _xRange->getUserMin();
	_extents[1] = _yRange->getUserMin();
	_extents[2] = _zRange->getUserMin();
	_extents[3] = _xRange->getUserMax();
	_extents[4] = _xRange->getUserMax();
	_extents[5] = _xRange->getUserMax();

	if (_regionSelection == 0) {
		copyActiveRegionButton->setEnabled(true);
		RestoreExtentsButton->setEnabled(true);
	}
	if (_regionSelection == 1) {
		copyActiveRegionButton->setEnabled(true);
		RestoreExtentsButton->setEnabled(true);

	}
	if (_regionSelection == 2) {
		copyActiveRegionButton->setEnabled(false);
		RestoreExtentsButton->setEnabled(false);
	}
	else {
		copyActiveRegionButton->setEnabled(true);
		RestoreExtentsButton->setEnabled(true);
	}
}

void Statistics::rGridError(int ts, string varname) {
	std::ostringstream ss;
	ss << "Invalid grid specification at timestep ";
	ss << ts;
	ss << ", variable ";
	ss << varname;
	ss << ", refLevel ";
	ss << _refLevel;
	ss << ", cRatio 1:";
	ss << _cRatios[_cRatio];
	ss << ", voxelCoordMin ";
	ss << _vCoordMin[0];
	ss << " ";
	ss << _vCoordMin[1];
	ss << " ";
	ss << _vCoordMin[2];
	ss << ", voxelCoordMax ";
	ss << _vCoordMax[0];
	ss << " ";
	ss << _vCoordMax[1];
	ss << " ";
	ss << _vCoordMax[2];
	string myErr = ss.str();
	errReport(myErr);
}

bool Statistics::calcMean(string varname) {
	float mv;
	float val = 0;
	double sum = 0;
	double tsMean = 0;
	long count = 0;
	long missing = 0;
	float range[2];
	bool varIs3D = false;
	bool success = true;

	if (std::find(_vars3d.begin(), _vars3d.end(), varname) != _vars3d.end())
		varIs3D = true;

	int spMissing=0;
	for (int ts=_minTS; ts<=_maxTS; ts++){
		sum = 0;
		missing = 0;
		count = 0;

		_rGrid = _dm->GetVariable(ts, varname, _refLevel, _cRatio, _uCoordMin, _uCoordMax);

		if (!_rGrid) {
			success = false;
			continue;
		}

		mv = _rGrid->GetMissingValue();

		StructuredGrid::ForwardIterator<StructuredGrid> itr;
		double c = 0.0;

		size_t dims[3];
		_rGrid->GetDimensions(dims);

		// If _regionSelection==2, we are querying a single point.
		// So here we just call GetValue at that point.
		//
		if (_regionSelection==2){
			val = _rGrid->GetValue(_extents[0],_extents[1],_extents[2]);
			if (val != mv) {
				tsMean += val;
			}
			else {
				spMissing++;
			}

			// Generate min and max values for our single-point calculation
			// Initialize min and max with the first timestep, then overwrite
			// if necessary.
			if (ts == _minTS) {
				_stats[varname].min = val;
				_stats[varname].max = val;
			}
			else {
				if (val < _stats[varname].min) 
					_stats[varname].min = val;
				if (val > _stats[varname].max) 
					_stats[varname].max = val;
			}

			// If our missing value count is equal to the number of timesteps,
			// then all queries for this point have given us a missing value.
			// We must set the mean to mv, otherwise it will be set to its default of 0.
			//
			if (spMissing == _maxTS-_minTS+1) {
				tsMean = mv;
			}
		}

		// We are selecting a range of values, so we need to query each one.
		//
		else {
			int count=0;

			// Generate min and max values for our multi-point calculation
			//
			_rGrid->GetRange(range);
			if (ts == _minTS) {
				_stats[varname].min = range[0];
				_stats[varname].max = range[1];
			}
			else {
				if (range[0] < _stats[varname].min) 
					_stats[varname].min = range[0];
				if (range[1] > _stats[varname].max) 
					_stats[varname].max = range[1];
			}

			for (itr=_rGrid->begin(); itr!=_rGrid->end(); itr++) 
			{
				count++;
				val = *itr;
				if (val != mv) 
				{
					double y = val - c;
					double t = sum + y;
					c = t - sum - y;
					sum = t;
				}
				else missing++;
			}   

			count = _vCoordMax[0] - _vCoordMin[0] + 1;
			count *= (_vCoordMax[1] - _vCoordMin[1] + 1);
			if (std::find(_vars3d.begin(), _vars3d.end(), varname) != _vars3d.end()){
				count *= (_vCoordMax[2] - _vCoordMin[2] + 1);
			}

			count -= missing;
			assert (count >= 0);
			if (count == 0) tsMean = mv;
			else tsMean += sum/(double)count;
		}
	}

	// Subtracting spMissing in the denominator is a hack to accomodate
	// missing values that arise during the single-point calculation.
	// This is due to the fact that if we have a missing value during
	// single-point calculations, discarding that sample also means discarding
	// that entire timestep.  This must be accounted for when we average over time.
	// spMissing will always be 0 when we sample volumes of data.
	//
	_stats[varname].mean = tsMean / (double)(_maxTS - _minTS - spMissing + 1);
	return success;
}


bool Statistics::calcStdDev(string varname) {
	double deviations;
	double mean = _stats[varname].mean;
	double tsStdDev = 0;
	float mv;
	float val;
	long missing = 0;
	long globalCount = 0;
	bool varIs3D = false;
	bool success = true;
	int spMissing = 0;
	
	if (std::find(_vars3d.begin(), _vars3d.end(), varname) != _vars3d.end())
		varIs3D = true;

	for (int ts=_minTS; ts<=_maxTS; ts++){
		long count = 0;
		deviations = 0;
		missing = 0;
		if (_rGrid) delete _rGrid;
		_rGrid = _dm->GetVariable(ts, varname, _refLevel, _cRatio, _uCoordMin, _uCoordMax);

		// Invalid regular grid.  Use previous timesteps and return.
		if (!_rGrid) { 
			success = false;
			continue;
		}

		mv = _rGrid->GetMissingValue();

		// If _regionSelection==2, we are querying a single point.
		// So here we just call GetValue at that point.
		//
		if (_regionSelection==2){
			val = _rGrid->GetValue(_extents[0],_extents[1],_extents[2]);
			if (val != mv) {
				tsStdDev += (val-mean)*(val-mean);
			}
			else {
				spMissing++;
			}

			// If our missing value count is equal to the number of timesteps,
			// then all queries for this point have given us a missing value.
			// We must set the mean to mv, otherwise it will be set to its default of 0.
			//
			if (spMissing == _maxTS-_minTS+1) {
				tsStdDev = mv;
			}
			globalCount = _maxTS - _minTS - spMissing + 1;
			if (globalCount==0) tsStdDev = mv;
		}

		else {
//			RegularGrid::Iterator itr;
			StructuredGrid::ForwardIterator<StructuredGrid> itr;
			double c = 0.0;
			size_t dims[3];
			_rGrid->GetDimensions(dims);
			for (itr=_rGrid->begin(); itr!=_rGrid->end(); itr++) {
				val = *itr;
	
				if (val != mv) { //sum += val;
					double y = (val - mean) * (val - mean) - c;
					double t = deviations + y;
					c = t - deviations - y;
					deviations = t;
				}
				else 
					missing++;
			}
		 
			count = _vCoordMax[0] - _vCoordMin[0] + 1;
			count *= (_vCoordMax[1] - _vCoordMin[1] + 1);
	
			// If var is 3d, add third dimension to our count
			//
			if (std::find(_vars3d.begin(), _vars3d.end(), varname) != _vars3d.end())
				count *= (_vCoordMax[2] - _vCoordMin[2] + 1);
	
			count -= missing;
			assert(count >= 0);  
			if (count == 0) tsStdDev = mv;
			else tsStdDev += deviations;
			
			globalCount += count;
		}
	}
	
	_stats[varname].stddev = sqrt( tsStdDev / (double)(globalCount) );
	return success;
}

bool Statistics::calcMedian(string varname) {
	float mv;
	bool varIs3D = false;
	bool success = true;
	long globalCount = _vCoordMax[0] - _vCoordMin[0] + 1;
	globalCount *= (_vCoordMax[1] - _vCoordMin[1] + 1);
	// If var is 3d, add third dimension to our count
	if (std::find(_vars3d.begin(), _vars3d.end(), varname) != _vars3d.end())
	{
		varIs3D = true;
		globalCount *= (_vCoordMax[2] - _vCoordMin[2] + 1);
	}
	globalCount = globalCount * (_maxTS - _minTS + 1);
	std::vector<float> allValues;   // rGrid only returns floats
	try{
	  allValues.reserve( globalCount );
	}
	catch( exception &e )
	{
		std::ostringstream ss;
		ss << "Standard exception: " << e.what() << endl;
		ss << "  Memory allocation failed at median calculation" ;
		errReport(ss.str());
		success = false;
		return success;
	}

	for (int ts=_minTS; ts<=_maxTS; ts++){
		_rGrid = _dm->GetVariable(ts, varname, _refLevel, _cRatio, _uCoordMin, _uCoordMax);

		// Invalid regular grid.  Use previous timesteps and return.
		if (!_rGrid) { 
			success = false;
			continue;
		}

		float val;
		mv = _rGrid->GetMissingValue();
//		RegularGrid::Iterator itr;
		StructuredGrid::ForwardIterator<StructuredGrid> itr;
		// If _regionSelection==2, we are querying a single point.
		// So here we just call GetValue at that point.
		//
		if (_regionSelection==2){
			val = _rGrid->GetValue(_extents[0],_extents[1],_extents[2]);
			if (val != mv) {
				allValues.push_back( val );
			}
		}
		else {
			for (itr=_rGrid->begin(); itr!=_rGrid->end(); itr++) 
			{
				val = *itr;
	
				if ( val != mv) 
					allValues.push_back( val );
			}
		}
		if (_rGrid) delete _rGrid;
	}

	std::sort( allValues.begin(), allValues.end() );
	if (allValues.empty()) _stats[varname].median = mv;
	else _stats[varname].median = allValues.at( allValues.size() / 2 );
	
	return success;
}