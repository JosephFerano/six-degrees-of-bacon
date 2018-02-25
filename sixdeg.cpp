// By Jose Perez Perez Perez Perez Perez Perez Perez Perez
//
// High level explanation of the design:
// I'm aiming for as many O(1) operations as possible. This led to
// certain design decisions such as a heavy parsing period at the
// start of the program and a very large load on memory. Which further
// led to the heavy use of maps all over the place. In particular,
// verifying whether a title/actor name exists in the database is O(1)
// and so is getting the edges of a title/actor. I also try to avoid
// using strings as much as possible. The ids in particular were a bit
// inefficient since they would be structured like "nm0000524",
// basically wasting time with all those comparisons. So I parse
// everything to use ints instead.
//
// There are potential performance improvements for these but they are
// out of scope for this assignment. The former can be ameliorated by
// serializing the new parsed data into a format that is much more
// friendlier to parse at runtime and saving that to disk. The latter
// can be fixed by by using a traditional database, although I'm not
// sure what the performance hit would be when querying.
// 
// Data orientation for cache friendliness was also emphasized and
// this led to the constant use of vectors and all over the place,
// although I'm not sure how much better raw C arrays would've been
// but I do know I didn't want to deal with that
// headache. Particularly useful was a vector<edge> data structure
// that is used to mark whether nodes are visited or not. Not only did
// it serve for this purpose, but it was also used to trace the path
// back from the target actor to the base actor in the
// getDegreesOfSeparation function. As a vector though, the memory is
// contiguous and although we only access one of the two integers in
// the edge struct, we should still see a speed increase despite a
// theoretical duplication of cache misses.
//
// Some words on style; first I don't like to riddle code with
// comments, i'd rather use descriptive names as self-documentation,
// and suppliment that with paragraphs like these. However, if the
// scope of the variable is very small, I tend to go with single
// letter variables a lot of the time.
//
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

// These maps help us do a lot of O(1) operations. However, idToIndex,
// is a temporary map who use is just to facilitate constructing the
// graph. Once the graph is completed, we throw it away basically.

// pair< vector<string> , unordered_map<int, int> > loadDataFromFile(const string& path)
void loadAndParseDataFromFile(const string& path,
			      vector<string>& indexToName,
			      unordered_map<int, int>& idToIndex,
			      unordered_map<string, int>& nameToIndex)
{
  ifstream file;
  string strLine;
  vector<string> tokens;
  // vector<string> data;
  // unordered_map<int, int> map;
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
    // map[id] = i;
    // // cout << tokens[1] << " " << map[id] << endl;
    // data.push_back(tokens[1]);
    idToIndex[id] = i;
    nameToIndex[tokens[1]] = i;
    indexToName.push_back(tokens[1]);

    i++;
    // return make_pair(data, map);
  }
}

void clock_it(const string& m, clock_t begin, clock_t end)
{
  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  cout << "\nElapsed time to " << m << " : " << elapsed_secs << " seconds\n";
}

// Here I'm using a breadth first search. The nested loops are a
// little funky but at least we avoid recursion. We don't use a queue
// because honestly we only care about the depths, rather than
// guaranteed left-to-right traversal. By using two pseudo queues, we
// are able to easily separate groups of actors by their depth.  Also
// note the use of vector<edge>. We save it even when we find the
// actor, because this helps the caller go through the vector to
// traverse back to the base actor.
int getDegreesOfSeparation(const Graph& actorGraph,
			   const Graph& titleGraph,
			   vector<edge>& visitedFrom,
			   int fromActor,
			   int toActor)
{
  vector<int> top;
  vector<int> neighbors;
  int depth = 1;
  top.push_back(fromActor);
  visitedFrom.resize(actorGraph.size());
  for (auto &v : visitedFrom)
    v.toActorIndex = -1;
  visitedFrom[fromActor].toActorIndex = 0;
  while (depth <= 6)
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
    neighbors.clear();
    depth++;
  }
  // COULD NOT FIND EDGE, YOU BROKE COMPUTER SCIENCE
  return 0;
}

// Here I'm using the same breadth first algorithm from getDegreesOfSeparation
// since I believe this is the fastest way to get valid actors. The safest way
// to assure that the actors we're considering to be equal to the maxDepth
// is to eliminate all actors at each depth. If not we run the risk of an actor
// having a closer degree than that of the movie, through another movie/actor path.
// This should guarantee that they are indeed of the desired degree of separation.
// A verification could be added where we go through all actors returned and assure
// ourselves that they are valid by calling getDegreesOfSeparation on each.
vector<int> getActorsWithDepth(const Graph& actorGraph,
			       const Graph& titleGraph,
			       int fromActor,
			       int maxDepth)
{
  vector<int> top;
  vector<int> neighbors;
  vector<edge> visitedFrom;
  int depth = 1;
  top.push_back(fromActor);
  visitedFrom.resize(actorGraph.size());
  for (auto &v : visitedFrom)
    v.toActorIndex = -1;
  visitedFrom[fromActor].toActorIndex = 0;
  while (depth <= 6)
  {
    for (auto actor : top)
    {
      for (auto t : actorGraph[actor])
      {
	for (auto costar : titleGraph[t])
	{
	  if (visitedFrom[costar].toActorIndex == -1)
	  {
	    visitedFrom[costar].toActorIndex = 0;
	    neighbors.push_back(costar);
	  }
	}
      }
    }
    top.clear();
    for (auto n : neighbors)
      top.push_back(n);
    neighbors.clear();
    if (depth == maxDepth)
    {
      vector<int> as;
      int min = top.size() < 20  ? top.size() : 20;
      as.resize(min);
      for (int i = 0; i < min; i++)
      {
	as[i] = top[i];
      }
      return as;
    }
    depth++;
  }
  // COULD NOT FIND EDGE, YOU BROKE COMPUTER SCIENCE
  return top;
}

int main() {

  clock_t begin;

  // Parse titles and actors
  vector<string> titles;
  vector<string> actors;
  unordered_map<string, int> actorNamesToIndex;
  unordered_map<string, int> titleNamesToIndex;
  unordered_map<int, int> actorsToIndex;
  unordered_map<int, int> titlesToIndex;

  begin = clock();

  // auto p1 = loadDataFromFile("names.csv");
  // actors = p1.first;
  // actorsToIndex = p1.second;

  loadAndParseDataFromFile("names.csv", actors, actorsToIndex, actorNamesToIndex);
  clock_it("Parse Names", begin, clock());
  cout << actorNamesToIndex.size() << endl;
  // for (auto n : actorNamesToIndex)
  //   cout << n.first << " " << n.second << endl;

  begin = clock();

//   auto p2 = loadDataFromFile("title.basic.csv");
//   titles = p2.first;
//   titlesToIndex = p2.second;

  loadAndParseDataFromFile("title.basic.csv", titles, titlesToIndex, titleNamesToIndex);
  clock_it("Parse Titles", begin, clock());

  ifstream principalsFile;
  principalsFile.open("title.principals.csv");
  if (!principalsFile.is_open()) {
    cout << "Error reading file.\n";
  }

  // Parse the Titles to Actors DB
  string strLine, stTitle;
  vector<string> tokens;
  unordered_map<int, unordered_set<int> > parsedTitles;
  unordered_map<int, unordered_set<int> > parsedActors;

  begin = clock();

  while(getline(principalsFile, strLine)) {
    strLine.erase(std::remove(strLine.begin(), strLine.end(), '\"'), strLine.end());
    split(strLine.c_str(), ',', tokens); 
    int titleId = getId(tokens[0]);
    for (int j = 1; j < tokens.size(); j++)
    {
      parsedTitles[titleId].insert(getId(tokens[j]));
    }
    for (auto a : parsedTitles[titleId])
    {
      parsedActors[a].insert(titleId);
    }
  }

  clock_it("Create maps", begin, clock());

  Graph actorGraph;
  Graph titleGraph;

  actorGraph.resize(actorsToIndex.size());
  titleGraph.resize(titlesToIndex.size());

  begin = clock();


  for (auto pas : parsedActors)
  {
    int actorId = pas.first;
    int actorIndex = actorsToIndex[actorId];
    for (auto tid : pas.second)
    {
      int ti = titlesToIndex[tid];
      actorGraph[actorIndex].push_back(ti);
    }
  }

  clock_it("Create Actors Graph", begin, clock());
  begin = clock();

  for (auto pas : parsedTitles)
  {
    int titleId = pas.first;
    int titleIndex = titlesToIndex[titleId];
    for (auto aid : pas.second)
    {
      int ai = actorsToIndex[aid];
      titleGraph[titleIndex].push_back(ai);
    }
  }

  clock_it("Create Titles Graph", begin, clock());

  // We don't need these anymore
  parsedActors.clear();
  parsedTitles.clear();

  input_loop:

  cout << endl;
  cout << "Please choose A or B and press <Enter>" << endl;
  cout << "  A.) Get Actors with Nth degree of separation" << endl;
  cout << "  B.) Get degrees of separation between two actors" << endl;
  string option;
  getline(cin, option);

  if (option[0] == 'A')
  {
    string actorInput = "Huh";
    string depthInput;
    int depth;
    cout << "Enter an actor: "<< endl;
    getline(cin, actorInput);
    // int a = findActorIndex(actors, actor);
    int a = -1;
    auto it = actorNamesToIndex.find(actorInput);
    if (it != actorNamesToIndex.end())
      a = it->second;
    if (a != -1)
    {
      cout << "Now enter a depth: (max 6)"<< endl;
      getline(cin, depthInput);
      depth = stoi(depthInput);
      cout << endl;
      begin = clock();
      cout << " Searcing..\n" << endl;
      auto cs = getActorsWithDepth(actorGraph, titleGraph, a, depth);
      cout << "Actors with " << depth << " degree of separation" << endl;
      for (auto c : cs)
	cout << "  " << actors[c] << endl;
      clock_it("find actors to the Nth degree", begin, clock());
    }
  }
  else if (option[0] == 'B')
  {
    string actor1 = "0";
    string actor2 = "0";
    vector<edge> visitedFrom;
    cout << endl;
    cout << "Enter actor one: "<< endl;
    getline(cin, actor1);
    cout << "Enter actor two: "<< endl;
    getline(cin, actor2);

    // Currently validating the actor names is O(n), it can easily be made O(1)
    // with an unordered_set, but honestly it's fast enough anyway
    // int a1 = findActorIndex(actors, actor1);
    // int a2 = findActorIndex(actors, actor2);

    int a1 = -1;
    auto it1 = actorNamesToIndex.find(actor1);
    if (it1 != actorNamesToIndex.end())
      a1 = it1->second;
    int a2 = -1;
    auto it2 = actorNamesToIndex.find(actor2);
    if (it2 != actorNamesToIndex.end())
      a2 = it2->second;

    // Now the magic happens
    cout << endl;
    if (a1 != -1 && a2 != -1)
    {
      begin = clock();
      cout << " Searcing..\n" << endl;
      int degrees = getDegreesOfSeparation(actorGraph, titleGraph, visitedFrom, a1, a2);
      cout << degrees << " degrees of separation" << endl;
      cout << "---------------------------" << endl;
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
  else
    cout << "Error with input, please try again" << endl;
  // Honestly, I'd rather do a goto here so I can avoid yet another level of indentation
  // cause I also don't want to separate out into functions
  goto input_loop;
  
  return 0;
}
