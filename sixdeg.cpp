// Starter code for the six degrees engine.
// Reads the title.principals.csv file from beginning to end, splitting
// each line into strings.
// Measures the elapsed time.

#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
using namespace std;

struct edge
{
  int movieIndex;
  int toActorIndex;
};

typedef vector< vector<int> > Graph;

void split(const char *s, char c , vector<string> &res) {
  res.resize(0);
  do {
    const char *b = s;
    while(*s != c && *s) s++;
    res.push_back(string(b, s));
  } while (0 != *s++);
}

int getId(const string& s)
{
  string n = s;
  return stoi(n.erase(0, 2));
}

int findActorIndex(const vector<string>& actors, const string& actor)
{
  int a = -1;
  for (int i = 0; i < actors.size(); i++)
  {
    if (actors[i] == actor)
    {
      a = i;
      break;
    }
  }
  if (a == -1)
    cout << "Actor " << actor << " not found" << endl;
  return a;
}

pair< vector<string> , unordered_map<int, int> > loadDataFromFile(const string& path)
{
  ifstream file;
  string strLine;
  vector<string> tokens;
  vector<string> data;
  unordered_map<int, int> map;
  file.open(path);
  if (!file.is_open()) {
    cout << "Error reading file at " << path << endl;
  }

  int i = 0;
  while(getline(file, strLine)) {
    // remove the double quotes
    strLine.erase(remove(strLine.begin(), strLine.end(), '\"'), strLine.end());
    
    split(strLine.c_str(), ',', tokens); 
    int id = getId(tokens[0]);
    map[id] = i;
    // cout << tokens[1] << " " << map[id] << endl;
    data.push_back(tokens[1]);
    i++;
  }
  return make_pair(data, map);
}

void clock_it(const string& m, clock_t begin, clock_t end)
{
  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  cout << "\nElapsed time to " << m << " : " << elapsed_secs << " seconds\n";
}

int getDegreesOfSeparation(const Graph& actorGraph,
			   const Graph& titleGraph,
			   vector<edge>& visitedFrom,
			   int fromActor,
			   int toActor)
{
  vector<int> top;
  vector<int> neighbors;
  int depth = 0;
  top.push_back(fromActor);
  visitedFrom.resize(actorGraph.size());
  for (auto &v : visitedFrom)
    v.toActorIndex = -1;
  visitedFrom[fromActor].toActorIndex = 0;
  while (depth < 6)
  {
    for (auto actor : top)
    {
      for (auto t : actorGraph[actor])
      {
	for (auto costar : titleGraph[t])
	{
	  if (visitedFrom[costar].toActorIndex == -1)
	  {
	    visitedFrom[costar].toActorIndex = actor;
	    visitedFrom[costar].movieIndex = t;
	    if (costar == toActor)
	      return depth; // Found actor!
	    neighbors.push_back(costar);
	  }
	}
      }
    }
    top.clear();
    for (auto n : neighbors)
      top.push_back(n);
    depth++;
  }
  // COULD NOT FIND CONNECTION, YOU BROKE COMPUTER SCIENCE
  return 0;
}

void getActorsWithDepth(const Graph& actorGraph,
			const Graph& titleGraph,
			vector<edge>& visitedFrom,
			int fromActor)
{
}

int main() {

  clock_t begin;

  // Parse titles and actors
  vector<string> titles;
  vector<string> actors;
  unordered_map<int, int> actorsToIndex;
  unordered_map<int, int> titlesToIndex;

  begin = clock();

  auto p1 = loadDataFromFile("names.csv");
  // auto p1 = loadDataFromFile("names.small.csv");
  actors = p1.first;
  actorsToIndex = p1.second;

  clock_it("Parse Names", begin, clock());

  begin = clock();

  auto p2 = loadDataFromFile("title.basic.csv");
  // auto p2 = loadDataFromFile("title.small.csv");
  titles = p2.first;
  titlesToIndex = p2.second;

  clock_it("Parse Titles", begin, clock());

  ifstream principalsFile;
  principalsFile.open("title.principals.csv");
  // principalsFile.open("title.principals.small.csv");
  if (!principalsFile.is_open()) {
    cout << "Error reading file.\n";
  }

  // return 0;
  // Parse the Titles to Actors DB
  string strLine, stTitle;
  vector<string> tokens;
  unordered_map<int, unordered_set<int> > parsedTitles;
  unordered_map<int, unordered_set<int> > parsedActors;

  begin = clock();

  while(getline(principalsFile, strLine)) {
    strLine.erase(std::remove(strLine.begin(), strLine.end(), '\"'), strLine.end());
    split(strLine.c_str(), ',', tokens); 
    // cout << "Parsing line " << strLine << endl;
    int titleId = getId(tokens[0]);
    for (int j = 1; j < tokens.size(); j++)
    {
      parsedTitles[titleId].insert(getId(tokens[j]));
    }
    for (auto a : parsedTitles[titleId])
    {
      // cout << a << " " << titleId << endl;
      parsedActors[a].insert(titleId);
    }
  }

  clock_it("Create maps", begin, clock());

  Graph actorGraph;
  Graph titleGraph;

  actorGraph.resize(actorsToIndex.size());
  titleGraph.resize(titlesToIndex.size());
  // cout << actorsToIndex.size() << endl;

  begin = clock();


  for (auto pas : parsedActors)
  {
    int actorId = pas.first;
    int actorIndex = actorsToIndex[actorId];
    // cout << actors[actorIndex] << endl;
    for (auto tid : pas.second)
    {
      int ti = titlesToIndex[tid];
      // cout << "\t in " << titles[ti] << " with: " << endl;
      actorGraph[actorIndex].push_back(ti);
    }
  }

  clock_it("Create Actors Graph", begin, clock());
  begin = clock();

  for (auto pas : parsedTitles)
  {
    int titleId = pas.first;
    int titleIndex = titlesToIndex[titleId];
    // cout << actors[actorIndex] << endl;
    for (auto aid : pas.second)
    {
      int ai = actorsToIndex[aid];
      // cout << "\t in " << titles[mi] << " with: " << endl;
      titleGraph[titleIndex].push_back(ai);
    }
  }

  clock_it("Create Titles Graph", begin, clock());

  input_loop:

  cout << "Please choose A or B and press <Enter>" << endl;
  cout << "  A.) Get Actors with Nth degree of separation" << endl;
  cout << "  B.) Get degrees of separation between two actors" << endl;
  char option;
  cin >> option;

  if (cin == 'A')
  {
    string actor = "0";
    cout << "Enter an actor: "<< endl;
    getline(cin, actor);
    int a = findActorIndex(actors, actor);
    if (a != -1)
    {
      getActorsWithDepth(actorGraph, titleGraph, visitedFrom, actor);
    }
  }
  else if (cin == 'B')
  {
    string actor1 = "0";
    string actor2 = "0";
    vector<edge> visitedFrom;
    cout << endl;
    while (actor1.length() > 0 || actor2.length() > 0) {
      cout << "Enter two actors: "<< endl;
      getline(cin, actor1);
      getline(cin, actor2);

      // Currently validating the actor names is O(n), it can easily be made O(1)
      // with an unordered_set, but honestly it's fast enough anyway
      int a1 = findActorIndex(actors, actor1);
      int a2 = findActorIndex(actors, actor2);

      // Now the magic happens
      cout << endl;
      if (a1 != -1 && a2 != -1)
	{
	  begin = clock();
	  cout << " Searcing..\n" << endl;
	  int degrees = getDegreesOfSeparation(actorGraph, titleGraph, visitedFrom, a1, a2);
	  cout << "Degrees of separation " << (degrees + 1) << endl;
	  cout << "  " << actors[a2] << " was in \n\t";
	  cout << titles[visitedFrom[a2].movieIndex] << endl;
	  int current = visitedFrom[a2].toActorIndex;
	  for (int i = 0; i < 6; i++)
	    {
	      edge e = visitedFrom[current];
	      if (a1 != current)
		{
		  cout << "  with " << actors[current];
		  cout << " who was in \n\t" << titles[e.movieIndex] << endl;
		}
	      else
		{
		  cout << "  with " << actors[current] << endl;
		  break;
		}
	      current = e.toActorIndex;
	    }
	  cout << endl;

	  for (auto v : visitedFrom)
	    v.toActorIndex = -1;
	  clock_it("find the degrees of separation", begin, clock());
	  cout << endl;
	}
    }
  }
  else
    cout << "Error with input, please try again" << endl;
  // Honestly, I'd rather do a goto here so I can avoid yet another level of indentation
  // cause I also don't want to separate out into functions
  goto input_loop;
  
  return 0;
}
