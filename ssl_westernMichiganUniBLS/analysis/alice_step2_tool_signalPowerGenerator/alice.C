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

#include "ToolNodesGenerator.hh"
#include "ToolMapGenerator.hh"
#include "ToolSignalPowerGenerator.hh"

using namespace std;

vector<double> rssis, dists;

void RaisdualTest();

int main()
{
	cout<<"Hello "<<endl;

	// step 0 : read
	string filename = "../data_step1_observations.txt";
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
	}
	read.close();



	// debug
	RaisdualTest();

	// step 1 : path loss exponent estimation
	//
	// Optimization 
	//
	int observationsSize = 2;
	int residualSize = 1;
	int varialbeSize = 2;

	UserOptimizationManager * manager = new NewtonsOptimizationManager("NewtonsMethod",observationsSize,varialbeSize,residualSize);

	// set variables
	vector<double> variables;
	variables.push_back(-0.);
	variables.push_back(0.);
	manager->SetUserInitialization(variables);

	// set cost function
	UserCostFunction* costFunction = new NewtonsCostFunction("costFunction",observationsSize,varialbeSize,residualSize);

	//  observations
	//for(int i=0;i<rssis.size();i++)
	for(int i=0;i<150;i++)
	{
		int ID = i;
		vector<double> observation_current;
		observation_current.push_back(rssis[ID]);
		observation_current.push_back(dists[ID]);
		costFunction->AddResidualBlock(observation_current);
	}

	//
	cout<<" "<<endl;
	cout<<"alice SetUserInitialization"<<endl;
	manager->SetUserInitialization(costFunction);

	//
	double UserReferencedLength = 70.;
	manager->SetUserReferencedLength(UserReferencedLength);

	// 
	double UserReferencedEpsilon = 1e-2;
	manager->SetUserEpsilonForTerminating(UserReferencedEpsilon);

	// initialize
	cout<<" "<<endl;
	cout<<"Initialize "<<endl;
	manager->Initialize();

	return 1;
}

void RaisdualTest()
{
	int observationsSize = 2;
	int residualSize = 1;
	int varialbeSize = 2;

	vector<double> observation_current;
	observation_current.push_back(rssis[0]);
	observation_current.push_back(dists[0]);

	UserResidualBlockFunction *poly = new PolyResidualBlockFunction("rssi",observation_current,observationsSize,varialbeSize,residualSize);

	vector<double> variables, residual;
	variables.push_back(0);
	variables.push_back(0);

	poly->ResidualFunction(variables,residual);

}
