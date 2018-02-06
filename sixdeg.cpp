// Starter code for the six degrees engine.
// Reads the title.principals.csv file from beginning to end, splitting
// each line into strings.
// Measures the elapsed time.

#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <fstream>
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

int main() {

  ifstream principalsFile;
  string strLine, stTitle;
  vector<string> tokens;
  double elapsed_secs;

  principalsFile.open("title.principals.csv");
  if (!principalsFile.is_open()) {
    cout << "Error reading file.\n";
  }

  // start the chronometer
  clock_t begin = clock();

  while(getline(principalsFile,strLine)) {
    // remove the double quotes
    strLine.erase(std::remove(strLine.begin(), strLine.end(), '\"'), strLine.end());
    
    cout << "The line: " << strLine << endl;
    
    split(strLine.c_str(), ',', tokens); 
    cout << "The tokens: " << endl;  
    for (auto e: tokens) 
      cout << "-- " << e << endl;

  }

  // stop the chronometer
  clock_t end = clock();
  elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  cout << "\nElapsed time to read file: " << elapsed_secs << " seconds\n";
    
  return 0;
}
