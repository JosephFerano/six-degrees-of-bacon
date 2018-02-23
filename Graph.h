#include <iostream>
#include <vector>

using namespace std;

class WeightedEdge{
public:
  int a, b, w;
  WeightedEdge()  {}
  WeightedEdge(int aa, int bb, int ww) : a(aa),b(bb),w(ww) {}
  void set(int aa, int bb, int ww) { a = aa; b = bb; w = ww; }
  void display(ostream &out) const;
  bool operator<(const WeightedEdge &lSide);

};

ostream & operator<<(ostream& out, const WeightedEdge & e);

class Graph {
private:
  vector<WeightedEdge> v;
  
public:

  Graph() { v.resize(0); }
  void addWeightedEdge(int aa, int bb, int w);
  void display(ostream &out) const;
  
  void minSpanningTree();
};

  
ostream & operator<<(ostream& out, const Graph & L);
