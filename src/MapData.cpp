//============================================================================
// Name        : MapData.cpp
// Author      : Sunita Srinivasan
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "MapData.h"


void MapData::ParseInputData(const char* inputFile)
{
	fstream fs;
	string line;
	//MapData md;

	fs.open(inputFile);
	if (fs.is_open())
	{
	  while ( getline (fs,line) )
	  {
	    size_t pos = 0;
	    string delimiter = "|";
	    pos = line.find(delimiter);

	    const char* pop= line.substr(0,pos).c_str();
		int population=atoi(pop);
		line.erase(0,pos+1);

	    pos = line.find(delimiter);
	    string city = line.substr(0,pos);
	    line.erase(0,pos+1);

	    pos = line.find(delimiter);
	    string state = line.substr(0,pos);
	    line.erase(0,pos+1);

	    //There could be multiple cities with the same name in different states, so the identifier for a city can used used as [city name, state name]
	    city += ", " + state;

	    pos = line.find(delimiter);
	    string highways = line.substr(0,pos);
	    delimiter = ";";
	    vector<string> interStates;
	    while ((pos = highways.find(delimiter)) != std::string::npos)
	    {
	    	string curr_interstate = highways.substr(0, pos);
	    	interStates.push_back(curr_interstate);
	    	highways.erase(0, pos + delimiter.length());
	    }
	    interStates.push_back(highways);

	    for(size_t i=0;i<interStates.size();i++)
	    {
	    	int n = interStates[i].length();
	        const char* interstateNum= interStates[i].substr(2,n-2).c_str();
	    	int istateNumber=atoi(interstateNum);
	    	m_interstateCities[interStates[i]].push_back(city);
	    	m_interstateCity[istateNumber]++;
	    }

        m_cityState[city] = state;
	    m_cityInterstates[city] = interStates;
	    m_populationCity[population].push_back(city);
	  }

	  fs.close();
	}
	fs.close();
}

void MapData::GenerateCitiesByPopulation()
{
	ofstream ofs;
	ofs.open("Cities_By_Population.txt");
	for(map<int,vector<string> >::reverse_iterator iter=m_populationCity.rbegin();iter!=m_populationCity.rend();iter++)
	{
		ofs<<iter->first<<endl;
		vector<string> cities = iter->second;
		map<string,string> m_stateCity;
		//creating a new map here just for the sake of sorting based on state as opposed to city.
		for(size_t i=0;i<cities.size();i++)
		{
			string city = cities[i];
			string state = m_cityState[city];
			m_stateCity[state] = city;
		}

		for(map<string,string>::iterator iter=m_stateCity.begin();iter!=m_stateCity.end();iter++)
		{
			string city = iter->second;
			string state = iter->first;
			int pos = city.find(", ");
			ofs<<city.substr(0,pos)<<", "<<state<<endl;
			vector<string> interStates = m_cityInterstates[city];
			if(interStates.size() >0)
			{
				ofs<<"Interstates: ";
				for(size_t i=0;i<interStates.size();i++)
				{
					ofs<<interStates[i];
					//This check is only to avoid printing the comma after the last element
					if(i<interStates.size() - 1)
						ofs<<", ";
				}
			}
			ofs<<endl<<endl;
		}

	}

	ofs.close();
	cout<<"File Generated: Cities_By_Population.txt"<<endl;
}

void MapData::GenerateInterstatesByCity()
{
	ofstream ofs;
	ofs.open("Interstates_By_City.txt");
	for(map<int,int>::iterator iter=m_interstateCity.begin();iter!=m_interstateCity.end();iter++)
	{
		ofs<<"I-"<<iter->first<<" "<<iter->second<<endl;
	}

	ofs.close();
	cout<<"File Generated: Interstates_By_City.txt"<<endl;
}


void MapData::GenerateDegreesFromChicago()
{
	ofstream ofs;
	ofs.open("Degrees_From_Chicago.txt");
	//I will be performing this traversal starting with the city Chicago and doing a BFS, hence I will need a structure to mark
	//all the visited interstates as well as a queue to enqueue all cities that still need to be processed
	map<string, bool> visitedInterstate;
	queue<string> cityQueue;
	map<string,int> distance;
	//I'm creating a map to hold every single degree value to a vector of all the cities with that degree to Chicago
	//This is the format that the output needs to be displayed in.
	map<int,vector<string> > invDistance;

	cityQueue.push("Chicago, Illinois");
	distance["Chicago, Illinois"] = 0;
	invDistance[0].push_back("Chicago, Illinois");

	while(!cityQueue.empty())
	{
		string top = cityQueue.front();
		cityQueue.pop();
		vector<string> interstates = m_cityInterstates[top];
		for(size_t i=0;i<interstates.size();i++)
		{
			if(visitedInterstate.find(interstates[i]) == visitedInterstate.end())
			{
				vector<string> neighbors = m_interstateCities[interstates[i]];
				for(size_t j=0;j<neighbors.size();j++)
				{
					if(distance.find(neighbors[j]) == distance.end())
					{
						distance[neighbors[j]] = distance[top] + 1;
						cityQueue.push(neighbors[j]);
						invDistance[distance[top] + 1].push_back(neighbors[j]);
					}
				}
				visitedInterstate[interstates[i]] = true;
			}
		}
	}

	for(map<string,string>::iterator iter=m_cityState.begin();iter!=m_cityState.end();iter++)
	{
		if(distance.find(iter->first) == distance.end())
		{
			distance[iter->first] = -1;
			invDistance[-1].push_back(iter->first);
		}
	}

	for(map<int,vector<string> >::reverse_iterator iter = invDistance.rbegin();iter!=invDistance.rend();iter++)
	{
		vector<string> cities = iter->second;
		map<string,string> cityMap;
		for(size_t i=0;i<cities.size();i++)
		{
			string city = cities[i];
			string state = m_cityState[city];
			cityMap[city] = state;
		}
		for(map<string,string>::iterator iter2 = cityMap.begin();iter2!=cityMap.end();iter2++)
		{
			string city = iter2->first;
			int pos = city.find(", ");
			city = city.substr(0,pos); //parse out the state from the [city, state] key
			ofs<<iter->first<<" "<<city<<", "<<iter2->second<<endl;
		}

	}
	ofs.close();
	cout<<"File Generated: Degrees_From_Chicago.txt"<<endl;
}


int main(int argc, char* argv[]) {
	if(argc < 2)
	{
		cout<<"Need an input filename as command line parameter"<<endl;
		return 0;
	}
	//argv[0] is the name of this executable
	const char* inputFile = argv[1];
	int option;
	cout<<"Option1: Generate the [Cities By Population] and [Interstates by Cities] files"<<endl;
	cout<<"Option2: Generate one [Degrees from Chicago] file"<<endl;
	cout<<"Enter an option (1 or 2):"<<endl;
	cin>>option;
	MapData md;
	md.ParseInputData(inputFile);
	switch(option)
	{
	case 1:
		md.GenerateCitiesByPopulation();
		md.GenerateInterstatesByCity();
		break;
	case 2:
		md.GenerateDegreesFromChicago();
		break;
	default:
		cout<<"Invalid input"<<endl;
	}

	return 0;
}


