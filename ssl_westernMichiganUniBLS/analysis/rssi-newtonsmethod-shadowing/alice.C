#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "UserCostFunction.hh"
#include "SteepestCostFunction.hh"
#include "NewtonsCostFunction.hh"

#include "UserResidualBlockFunction.hh"
#include "PolyResidualBlockFunction.hh"

#include "UserOptimizationManager.hh"
#include "SteepestOptimizationManager.hh"
#include "NewtonsOptimizationManager.hh"

#include "ToolMapGenerator.hh"
#include "ToolSignalPowerGenerator.hh"


using namespace std;

double Power_ref;
double PathLossExponent;
double d0;

bool PowerRSSI_residualTest();
bool PowerRSSI();
int RSSI();

int main()
{
	cout<<"Hello "<<endl;

	//
	string filename = "../Data_PathLossExponent.txt";
	ifstream file(filename.c_str());

	if(file.fail())
	{
		cout<<"Can not find the file \" "<<filename<<" \""<<endl;
		return 0;
	}

	string temp;

	file>>temp>>Power_ref;
	file>>temp>>PathLossExponent;
	file>>temp>>d0;

	cout<<"Power_ref "<<Power_ref<<endl;
	cout<<"PathLossExponent "<<PathLossExponent<<endl;
	cout<<"d0 "<<d0<<endl;

	//
	//bool isBuild1Good = PowerRSSI_residualTest();
	bool isBuildGood = PowerRSSI();

	return 1;
}

bool PowerRSSI_residualTest()
{
	int NumberNodes = 2;
	int observationsSize = 4+2*NumberNodes;
	int varialbeSize = 2*NumberNodes;
	int residualSize = 1;

	vector<double> obs;
	obs.push_back(-38.4748);
	obs.push_back(Power_ref);
	obs.push_back(-44.8991);
	obs.push_back(Power_ref);
	obs.push_back(PathLossExponent);
	obs.push_back(d0);
	obs.push_back(-45.3566);
	obs.push_back(84.0258);

	vector<double> variables;
	variables.push_back(-93.8696);
	variables.push_back(27.0384);
	variables.push_back(46.2762);
	variables.push_back(-43.2151);

	vector<double> residual;

	PolyResidualBlockFunction *poly = new PolyResidualBlockFunction("poly",obs,observationsSize,varialbeSize,residualSize);
	bool isPolyGood = poly->ResidualFunction(variables,residual);

	cout<<"Residual "<<residual[0]<<endl;

	return true;
}

bool PowerRSSI()
{

	// step 0 : read
	vector<double> rssis, dists;
	vector<double> tarxs, tarys;
	vector<double> ancxs, ancys;

	string filename = "../../data_step1_observations.txt";
	ifstream read(filename);

	if(read.fail())
	{
		cout<<"file "<<filename<<" dose not exist"<<endl;
		return 1;
	}

	string locLabel;
	double tarx, tary;
	double rssi;
	double ancx, ancy;
	double dist;

	while(!read.eof())
	{
		read>>locLabel>>tarx>>tary>>rssi>>ancx>>ancy>>dist;

		if(read.eof()) break;

		if(rssi<-199) continue;

		rssis.push_back(rssi);
		dists.push_back(dist);
		tarxs.push_back(tarx);
		tarys.push_back(tary);
		ancxs.push_back(ancx);
		ancys.push_back(ancy);
	}
	read.close();

	//
	// Optimization 
	//
	int observationsSize = 6;
	int residualSize = 1;
	int varialbeSize = 2;

	UserOptimizationManager * manager = new NewtonsOptimizationManager("NewtonsMethod",observationsSize,varialbeSize,residualSize);

	// set variables
	vector<double> variables;
	for(int i=0;i<varialbeSize;i++)
	{
		variables.push_back(0.);
	}
	manager->SetUserInitialization(variables);

	// set cost function
	UserCostFunction* costFunction = new NewtonsCostFunction("costFunction",observationsSize,varialbeSize,residualSize);

	// get observations
	//for(int i=0;i<rssis.size();i++)
	for(int i=0;i<13*20;i++)
	{
		int ID = i;

		if(rssis[ID]<-199) continue;

		vector<double> observation_current;
		observation_current.push_back(rssis[ID]);
		observation_current.push_back(Power_ref);
		observation_current.push_back(PathLossExponent);
		observation_current.push_back(d0);
		observation_current.push_back(ancxs[ID]);
		observation_current.push_back(ancys[ID]);
		costFunction->AddResidualBlock(observation_current);

		// debug
		cout<<"ID "<<ID<<": rssis[ID] "<<rssis[ID]<<", Power_ref "<<Power_ref<<", PathLossExponent "<<PathLossExponent<<", d0 "<<d0<<", ancxs[ID] "<<ancxs[ID]<<", ancys[ID] "<<ancys[ID]<<endl;
	}

	//
	cout<<" "<<endl;
	cout<<"alice SetUserInitialization"<<endl;
	manager->SetUserInitialization(costFunction);

	//
	double UserReferencedLength = 70.;
	manager->SetUserReferencedLength(UserReferencedLength);

	// 
	double UserReferencedEpsilon = 1e-7;
	manager->SetUserEpsilonForTerminating(UserReferencedEpsilon);

	// initialize
	cout<<" "<<endl;
	cout<<"Initialize "<<endl;
	manager->Initialize();


	return true;
}

int RSSI()
{
	string filename = "../observations.txt";
	ifstream file(filename.c_str());

	if(file.fail())
	{
		cout<<"Can not find the file \" "<<filename<<" \""<<endl;
		return 0;
	}

	int NumberAnchors = 0;
	int NumberNodes = 0;
	string temp;
	file>>temp>>NumberAnchors>>temp>>NumberNodes;
	cout<<"NumberAnchors "<<NumberAnchors<<" ; NumberNodes "<<NumberNodes<<endl;

	int AnchorID = 0;
	int NodeID = 0;
	double distanceSquared = 0;
	double ax = 0; 
	double ay = 0; 
	double xx = 0;
	double xy = 0;

	vector<int> AnchorIDs;
	vector<int> NodeIDs;
	vector<double> distancesSquared;
	vector<double> axs, ays, xxs, xys;

	while(!file.eof())
	{
		file>>AnchorID>>ax>>ay>>NodeID>>xx>>xy>>distanceSquared;

		if(file.eof()) break;

		cout<<"Distance squared "<<distanceSquared<<": Anchor ID "<<AnchorID<<", loc "<<ax<<", "<<ay<<"; Node ID "<<NodeID<<", loc "<<xx<<", "<<xy<<endl;

		AnchorIDs.push_back(AnchorID);
		NodeIDs.push_back(NodeID);
		distancesSquared.push_back(distanceSquared);
		axs.push_back(ax);
		ays.push_back(ay);
		xxs.push_back(xx);
		xys.push_back(xy);
	}

	file.close();

	//// test 
	//int observationsSize = 2 + NumberNodes;
	//int residualSize = 1;
	//int varialbeSize = 2*NumberNodes;

	//vector<double> variables;
	//variables.push_back(1.);
	//variables.push_back(2.);
	//variables.push_back(3.);
	//variables.push_back(4.);

	//vector<double> observations;
	//observations.push_back(1307.87);
	//observations.push_back(3725.57);
	//observations.push_back(62.4517);
	//observations.push_back(6.97365);
	//UserResidualBlockFunction * resi = new PolyResidualBlockFunction("test",observations,observationsSize,varialbeSize,residualSize);

	//vector<double> resiValue;
	//resi -> ResidualFunction(variables, resiValue);

	//
	// Optimization 
	//
	int observationsSize = 2 + NumberNodes;
	int residualSize = 1;
	int varialbeSize = 2*NumberNodes;

	UserOptimizationManager * manager = new NewtonsOptimizationManager("NewtonsMethod",observationsSize,varialbeSize,residualSize);

	// set variables
	vector<double> variables;
	for(int i=0;i<varialbeSize;i++)
	{
		variables.push_back(0.);
	}
	manager->SetUserInitialization(variables);

	// set cost function
	UserCostFunction* costFunction = new NewtonsCostFunction("costFunction",observationsSize,varialbeSize,residualSize);

	// get observations
	for(int i=0;i<NumberAnchors;i++)
	{
		int AnchorID = i;
		vector<double> observation_current;
		for(int j=0;j<NumberNodes;j++)
		{
			int NodeID = j;
			int disID = AnchorID*NumberNodes + NodeID;
			observation_current.push_back(distancesSquared[disID]);
		}
		int ID = AnchorID*NumberNodes + 0 ;
		observation_current.push_back(axs[ID]);
		observation_current.push_back(ays[ID]);
		costFunction->AddResidualBlock(observation_current);

		/*
		// debug
		for(int k=0;k<2+NumberNodes;k++)
		{
			cout<<"Observation ID "<<k<<"; ";
			cout<<observation_current[k]<<", ";
		}
		cout<<endl;

		UserResidualBlockFunction * resi = new PolyResidualBlockFunction("test",observation_current,observationsSize,varialbeSize,residualSize);
		vector<double> resiValue;
		resi -> ResidualFunction(variables, resiValue);
		*/
	}

	//
	cout<<" "<<endl;
	cout<<"alice SetUserInitialization"<<endl;
	manager->SetUserInitialization(costFunction);

	//
	double UserReferencedLength = 70.;
	manager->SetUserReferencedLength(UserReferencedLength);

	// 
	double UserReferencedEpsilon = 5e3;
	manager->SetUserEpsilonForTerminating(UserReferencedEpsilon);


	// initialize
	cout<<" "<<endl;
	cout<<"Initialize "<<endl;
	manager->Initialize();


	return 1;
}
