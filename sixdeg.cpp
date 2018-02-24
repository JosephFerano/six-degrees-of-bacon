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

void bfs(const Graph& graph, int actorIndex, int targetIndex);

struct edge
{
  int movieIndex;
  int toActorIndex;
};

typedef vector< vector<edge> > Graph;

// funtion split:
//   Given:
//     - s, a c-style string
//     - c, a separator character
//     - res, a reference to a vector of strings
//   Splits the string using the separator and returns the result
//   in the res vector. 
//   
//   For example: Given s = "a man a horse", c = ' '
//   Returns a vector with {"a", "man", "a", "horse"};

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

  // for (auto pt : parsedTitles)
  // {
  //   cout << titles[titlesToIndex[pt.first]] << " \n";
  //   for (auto a : pt.second)
  //     cout << '\t' << actors[actorsToIndex[a]] << endl;
  // }

  // for (auto pa : parsedActors)
  // {
  //   cout << actors[actorsToIndex[pa.first]] << " \n";
  //   for (auto t : pa.second)
  //     cout << '\t' << titles[titlesToIndex[t]] << endl;
  // }

  clock_it("Create maps", begin, clock());
  Graph graph;
  Graph movieGraph;

  graph.resize(actorsToIndex.size());
  movieGraph.resize(titlesToIndex.size());
  // cout << actorsToIndex.size() << endl;

  begin = clock();

  for (auto pas : parsedActors)
  {
    int actorId = pas.first;
    int actorIndex = actorsToIndex[actorId];
    // cout << actors[actorIndex] << endl;
    for (auto tid : pas.second)
    {
      int mi = titlesToIndex[tid];
      // cout << "\t in " << titles[mi] << " with: " << endl;
      for (auto aid : parsedTitles[tid])
      {
	if (aid == actorId)
	  continue;
	edge e;
	e.movieIndex = mi;
	e.toActorIndex = actorsToIndex[aid];
	graph[actorIndex].push_back(e);
	// cout << "\t\t " << actors[e.toActorIndex] << endl;
      }
    }
  }

  for (auto pas : parsedActors)
  {
    int actorId = pas.first;
    int actorIndex = actorsToIndex[actorId];
    // cout << actors[actorIndex] << endl;
    for (auto tid : pas.second)
    {
      int mi = titlesToIndex[tid];
      // cout << "\t in " << titles[mi] << " with: " << endl;
      for (auto aid : parsedTitles[tid])
      {
	if (aid == actorId)
	  continue;
	edge e;
	e.movieIndex = mi;
	e.toActorIndex = actorsToIndex[aid];
	graph[actorIndex].push_back(e);
	// cout << "\t\t " << actors[e.toActorIndex] << endl;
      }
    }
  }

  // for (int i = 0; i < 10; i++)
  // {
  //   cout << actors[i] << " friends " << graph[i].size() << endl;
  // }
  // unordered_set<string> s;
  // for (int i = 0; i < graph[0].size(); i++)
  // {
  //   string conc = to_string(graph[0][i].toActorIndex) + "-"+ to_string(graph[0][i].movieIndex);
  //   auto it = s.find(conc);
  //   if (it != s.end())
  //     cout << titles[graph[0][i].movieIndex] << " " << actors[graph[0][i].toActorIndex] << endl;
  //   s.insert(conc);
  // }
  // cout << graph[0].size() << " " << s.size() << endl;

  clock_it("Create graph", begin, clock());

  string actor1 = "0";
  string actor2 = "0";
  while (actor1.length() > 0 || actor2.length() > 0)
  {
    cout << "Enter two actors: "<<endl;

    getline(cin, actor1);
    getline(cin, actor2);

    int a1 = -1;
    for (int i = 0; i < actors.size(); i++)
      if (actors[i] == actor1)
	a1 = i;
    int a2 = -1;
    for (int i = 0; i < actors.size(); i++)
      if (actors[i] == actor2)
	a2 = i;
    if (a1 == -1)
      cout << "Actor " << actor1 << "not found" << endl;
    if (a2 == -1)
      cout << "Actor " << actor2 << "not found" << endl;

    if (a1 != -1 && a2 != -1)
    {
	cout << "Actors are " << actor1 << "  " << actor2 <<endl;
	queue<int> q;

	
    }
  }

    
  return 0;
}

queue<edge> getCostars(vector<bool> visited, int movieIndex, Graph g) 
{
  queue<edge> costars;
  for (auto n : g[actorIndex])
  {
    if (!visited[n.toActorIndex])
    {
      costars.push(n);
      visited[n.toActorIndex] = true;
    }
  }
  return costars;
}

vector<edge> bfs(const Graph& graph, int actorIndex, int targetIndex)
{
  queue<edge> top;
  queue<edge> sub;
  int depth = 0;
  edge visitedFrom[graph.size()];
  edge root;
  root.toActorIndex = actorIndex;
  top.push(actor)
  while (!top.empty())
  {
    for (auto e in top)
    {
      for (auto m in e.movieIndex)
      {
	for (auto a in m)
        {
	  if (a == targetIndex)
	  {
	    return a;
          }
	  else
	  {
	    if (a.from == -1)
            {
	      a.from = (m , e.toActorIndex)
	      sub.push(a);
            }
          }
        }
      }
    }
    top = sub;
    depth++;
  }

}
