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
    if (ifs.fail()) {
        cout << "Cannot open " << fileName << endl;
    }
    string prefix = fileName.substr(0, fileName.find_last_of('.'));
    string buf;
    double chipWidth, numBlocks;
    ifs >> buf >> buf >> chipWidth;  // chipWidth : xxx
    ifs >> buf >> buf >> numBlocks;  // numBlocks : xxx
    getline(ifs, buf);
    vector<double> widths(numBlocks);
    vector<double> heights(numBlocks);
    double chipHeightUB;
    for (int i = 0; i < numBlocks; ++i) {
        double width, height;
        getline(ifs, buf);
        istringstream iss(buf);
        iss >> buf >> buf >> width >> height;  // i : yyy
        widths[i] = width;
        heights[i] = height;
        chipHeightUB += height;
    }

    // 0. init Gurobi env & model
    GRBEnv env = GRBEnv();
    // env.set(GRB_IntParam_LogToConsole, 0);  // make the console silent
    GRBModel model = GRBModel(env);

    // 1. variables
    vector<GRBVar> xVars(numBlocks);    // lower x
    vector<GRBVar> yVars(numBlocks);    // lower y
    for (int i = 0; i < numBlocks; ++i) {
        xVars[i] = model.addVar(0.0, chipWidth - widths[i], 0.0, GRB_CONTINUOUS, "x" + to_string(i));
        yVars[i] = model.addVar(0.0, chipHeightUB, 0.0, GRB_CONTINUOUS, "y" + to_string(i));
    }
    GRBVar chipHeightVar = model.addVar(0.0, chipHeightUB, 1.0, GRB_CONTINUOUS, "H");

    // 2. constraints
    // 2.1 relate heights with chipHeightVar
    for (int i = 0; i < numBlocks; ++i) {
        model.addConstr(yVars[i] + heights[i] <= chipHeightVar, "y" + to_string(i) + "_H");
    }
    // 2.2 no overlap
    for (int i = 0; i < numBlocks; ++i) {
        for (int j = i + 1; j < numBlocks; ++j) {
            string pairStr = to_string(i) + "_" + to_string(j);
            // x
            GRBVar xNoOvlp = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "xNoOvlp" + pairStr);
            GRBVar xLowHighVars[2];
            xLowHighVars[0] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "xLow" + pairStr);
            xLowHighVars[1] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "xHigh" + pairStr);
            model.addGenConstrIndicator(xLowHighVars[0], 1.0, xVars[i] + widths[i] <= xVars[j]);
            model.addGenConstrIndicator(xLowHighVars[1], 1.0, xVars[j] + widths[j] <= xVars[i]);
            model.addGenConstrOr(xNoOvlp, xLowHighVars, 2);
            // y
            GRBVar yNoOvlp = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "yNoOvlp" + pairStr);
            GRBVar yLowHighVars[2];
            yLowHighVars[0] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "yLow" + pairStr);
            yLowHighVars[1] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "yHigh" + pairStr);
            model.addGenConstrIndicator(yLowHighVars[0], 1.0, yVars[i] + heights[i] <= yVars[j]);
            model.addGenConstrIndicator(yLowHighVars[1], 1.0, yVars[j] + heights[j] <= yVars[i]);
            model.addGenConstrOr(yNoOvlp, yLowHighVars, 2);
            // together
            model.addConstr(xNoOvlp + yNoOvlp >= 1, "noOvlp" + pairStr);
        }
    }

    // 3. solve
    // model.write(prefix + ".lp");
    model.optimize();
    cout << "Min chip height is " << model.getObjective().getValue() << endl;
    cout << "Block locations are: " << endl;
    ofstream ofs(prefix + "_solution.txt");
    for (int i = 0; i < numBlocks; ++i) {
        double x = xVars[i].get(GRB_DoubleAttr_X);
        double y = yVars[i].get(GRB_DoubleAttr_X);
        cout << i << " : " << x << " " << y << " " << widths[i] << " " << heights[i] << endl;
        ofs << i << " : " << x << " " << y << endl;
    }
    cout << endl;

    // 4. debug
    // model.write(prefix + ".sol");

    chrono::duration<double> duration = chrono::system_clock::now() - start;
    cout << "The program takes " << duration.count() << " seconds." << endl;

    return 0;
}