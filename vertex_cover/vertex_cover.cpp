#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char **argv) {
    // read
    if (argc != 2) {
        cout << "Please specify the input file name." << endl;
        return 1;
    }
    string fileName(argv[1]);
    ifstream ifs(fileName);
    if (ifs.fail()) {
        cout << "Cannot open " << fileName << endl;
    }
    string buf;
    int numVertexes;
    ifs >> buf >> buf >> numVertexes;  // numVertexes : xxx
    getline(ifs, buf);
    vector<vector<int>> adjLists(numVertexes);

    for (int i = 0; i < numVertexes; ++i) {
        getline(ifs, buf);
        istringstream iss(buf);
        iss >> buf >> buf;  // i : yyy
        int adj;
        while (iss >> adj) {
            adjLists[i].push_back(adj);
        }
    }

    return 0;
}