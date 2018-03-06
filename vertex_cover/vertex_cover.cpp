#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include "gurobi_c++.h"

using namespace std;

int main(int argc, char **argv) {
    auto start = chrono::system_clock::now();

    // read
    if (argc != 2) {
        cout << "Please specify the input file name." << endl;
        return 1;
    }
    string fileName(argv[1]);
    ifstream ifs(fileName);
    string prefix = fileName.substr(0, fileName.find_last_of('.'));
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

    // 0. init Gurobi env & model
    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env);

    // 1. add variables
    vector<GRBVar> vertexVars(numVertexes);
    for (int i = 0; i < numVertexes; ++i) {
        vertexVars[i] = model.addVar(0.0, 1.0, 1.0, GRB_BINARY, "vertex" + to_string(i));
    }

    // 2. add constraints
    for (int i = 0; i < numVertexes; ++i) {
        for (int j : adjLists[i]) {
            if (i < j) {
                model.addConstr(vertexVars[i] + vertexVars[j] >= 1, "edge" + to_string(i) + "_" + to_string(j));
            }
        }
    }

    // 3. solve & retrieve solutions
    // model.write(prefix + ".lp");
    model.optimize();
    cout << "Min vertex cover is " << model.getObjective().getValue() << endl;
    cout << "Selected vertexes are: ";
    for (int i = 0; i < numVertexes; ++i) {
        int value = vertexVars[i].get(GRB_DoubleAttr_X);
        if (value == 1) {
            cout << i << " ";
        }
    }
    cout << endl;

    // 4. check
    // model.write(prefix + ".sol");

    chrono::duration<double> duration = chrono::system_clock::now() - start;
    cout << "The program takes " << duration.count() << " seconds." << endl;

    return 0;
}