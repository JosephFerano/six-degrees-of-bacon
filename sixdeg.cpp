// Starter code for the six degrees engine.
// Reads the title.principals.csv file from beginning to end, splitting
// each line into strings.
// Measures the elapsed time.

#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include "Graph.h"
using namespace std;


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

  int i;
  while(getline(file, strLine)) {
    // remove the double quotes
    strLine.erase(remove(strLine.begin(), strLine.end(), '\"'), strLine.end());
    
    split(strLine.c_str(), ',', tokens); 
    int id = getId(tokens[0]);
    map[id] = i;
    data.push_back(tokens[1]);
    i++;
  }
  return make_pair(data, map);
}


int main() {

  double elapsed_secs;
  clock_t begin;
  clock_t end;

  // Parse titles and actors
  vector<string> titles;
  vector<string> actors;
  unordered_map<int, int> actorsToIndex;
  unordered_map<int, int> titlesToIndex;

  begin = clock();
  auto p1 = loadDataFromFile("names.csv");
  actors = p1.first;
  actorsToIndex = p1.second;
  end = clock();
  elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  cout << "\nElapsed time to load and parse Actors file: " << elapsed_secs << " seconds\n";

  begin = clock();
  auto p2 = loadDataFromFile("title.basic.csv");
  titles = p2.first;
  titlesToIndex = p2.second;
  end = clock();
  elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  cout << "\nElapsed time to load and parse Titles file: " << elapsed_secs << " seconds\n";

  ifstream principalsFile;
  principalsFile.open("title.principals.csv");
  if (!principalsFile.is_open()) {
    cout << "Error reading file.\n";
  }

  // vector<string> titles;
  // vector<string> actors;
  // unordered_map<int, int> actorsToIndex;
  // unordered_map<int, int> titlesToIndex;

  // Parse the Titles to Actors DB
  string strLine, stTitle;
  vector<string> tokens;
  unordered_map<int, unordered_set<int> > parsedTitles;
  unordered_map<int, unordered_set<int> > parsedActors;
  vector< vector<int> > titleToActors;
  vector< vector<int> > actorToTitles;

  titleToActors.resize(parsedTitles.size());
  actorToTitles.resize(parsedActors.size());

  begin = clock();

  int i = 0;
  while(getline(principalsFile, strLine)) {
    // remove the double quotes
    strLine.erase(std::remove(strLine.begin(), strLine.end(), '\"'), strLine.end());
    
    split(strLine.c_str(), ',', tokens); 
    
    int titleId = getId(tokens[0]);
    for (int j = 1; j < tokens.size(); j++)
      parsedTitles[titleId].insert(getId(tokens[j]));
    for (auto a : parsedTitles[titleId])
      parsedActors[a].insert(titleId);
  }

  end = clock();
  elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  cout << "\nElapsed time to parse titles to actors: " << elapsed_secs << " seconds\n";

  for (auto t : titlesToIndex)
  {
    int titleIndex = t.second;
    for (auto edges : parsedTitles[t.first])
    {
      cout << "Title Index: " << titleIndex << " " << endl;
      cout << "Edge: " << edges << " " << endl;
      cout << "titleToActors size: " << titleToActors.size() << " " << endl;
      auto t2a = titleToActors[titleIndex];
      cout << "Getting t2: " << endl;
      cout << "Edge: " << t2a.size() << " " << endl;
      t2a.push_back(actorsToIndex[edges]);
    }
  }

  for (auto a : actorsToIndex)
  {
    int actorIndex = a.second;
    for (auto edge : parsedActors[a.first])
      actorToTitles[actorIndex].push_back(titlesToIndex[edge]);
  }

  cout << actorToTitles.size() << endl;
  for (int i = 0; i < actorToTitles.size(); i++)
  {
    cout << "Actor at " << i << " has " << actorToTitles[i].size() << " movies. They are; " << endl;
    for (int j = 0; j < actorToTitles.size(); j++)
      cout << actorToTitles[i][j] << endl;

  }
  // stop the chronometer
  end = clock();
  elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  cout << "\nElapsed time to read file: " << elapsed_secs << " seconds\n";
    
  while (1) {}
  return 0;
}
