#include <iostream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

int main()
{
    string a = "Gaurang Pays Jitesh 0.01 BTC";
    vector<string> splits;
    stringstream iss(a);
    string word;
    while (iss >> word)
    {
        splits.push_back(word);
    }
    // for (const auto &w : splits)
    // {
    //     cout << w << '\n';
    // }
    int ac = 1, b = 25;
    string word2 = to_string(ac) + "Thereer ia am" + to_string(b) + "Chup Be";
    cout << word2 << endl;
    return 0;
}