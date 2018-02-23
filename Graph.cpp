#include "Graph.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>

using namespace std;

void WeightedEdge::display(ostream &out) const{
  out << "(" << a << "," << b << "," << w << ")";
}

bool WeightedEdge::operator<(const WeightedEdge &lSide) {
  return w < lSide.w;
}

ostream & operator<<(ostream& out, const WeightedEdge & e) {
  e.display(out);
  return out;
} 


void Graph::addWeightedEdge(int a, int b, int w) {
  v.push_back(WeightedEdge(a,b,w));
}

void Graph::display(ostream &out) const {
  for (auto e: v) out << e << " ";
}


ostream & operator<<(ostream& out, const Graph & g) {
  g.display(out);
  return out;
} 

// function minSpanningTree:
// Implements Kruskal's minimum spanning tree algorithm and outputs 
// the list of edges of the min spanning tree.
void Graph::minSpanningTree() {
  
  // This will sort the edges in ascending weight order
  sort(v.begin(), v.end());
  set<int> xs;
  for (auto e : v)
  {
	xs.insert(e.a);
	xs.insert(e.b);
  }
  int node_size = xs.size();
  vector<WeightedEdge> es;
  vector<int> groups;
  groups.resize(node_size);
  for (int i = 0; i < node_size; i++)
	groups[i] = i;
  for (auto e : v)
  {
	if (groups[e.a] != groups[e.b])
	{

		int newGroup = groups[e.a];
		int oldGroup = groups[e.b];
		cout << "New " << newGroup << " Old " << oldGroup;
		for (int i = 0; i < node_size; i++)
		{
			if (groups[i] == oldGroup)
				groups[i] = newGroup;
		}
		cout << " Adding node " << e << endl;
		es.push_back(e);
	}
  }

  
  cout << "Min Span Tree: ";
  for (int i = 0; i < es.size(); i++)
  {
	cout << es[i] << " ";
  }
  cout << endl;
  
}
