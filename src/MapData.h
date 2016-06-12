/*
 * MapData.h
 *
 *  Created on: Jun 12, 2016
 *      Author: sunit
 */
#include <string>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <map>
#include <queue>
#ifndef MAPDATA_H_
#define MAPDATA_H_
using namespace std;

class MapData
{
	//I am using maps here because they are auto-sorted and we don't have to bother with sorting algorithms
	//The key for this map here is of the format [city name, state name] since there could be cities with the same name in multiple states
	map<string,string> m_cityState;
	map<string,vector<string> > m_cityInterstates;
	//I'm going to save a map of population to a vector of city names since multiple cities could have the same population
	//Since a map is ordered by default, we'll have a sorted map of cities based on population.
	map<int,vector<string> > m_populationCity;
	map<int,int> m_interstateCity;
	map<string,vector<string> > m_interstateCities;
public:
	void ParseInputData(const char* inputFile);
	void GenerateCitiesByPopulation();
	void GenerateInterstatesByCity();
	void GenerateDegreesFromChicago();
};



#endif /* MAPDATA_H_ */
