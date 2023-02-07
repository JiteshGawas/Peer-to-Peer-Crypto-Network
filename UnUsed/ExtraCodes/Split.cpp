#include <iostream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

int main()
{
    string a = "Gaurang Pays Jitesh 0.01 BTC";
    vector<string> splits;
    istringstream iss(a);
    string word;
    while (iss >> word)
    {
        splits.push_back(word);
    }
    for (const auto &w : splits)
    {
        cout << w << '\n';
    }
    return 0;
}