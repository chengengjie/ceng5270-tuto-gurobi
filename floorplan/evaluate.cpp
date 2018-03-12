#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char **argv) {
    // read
    if (argc < 3) {
        cout << "Usage: ./evaluate <input_problem_file> <solution_file>" << endl;
        return 1;
    }
    string fileName(argv[1]);
    ifstream ifs(fileName);
    if (ifs.fail()) {
        cout << "Cannot open " << fileName << endl;
        return 1;
    }
    string fileName2(argv[2]);
    ifstream ifs2(fileName2);
    if (ifs2.fail()) {
        cout << "Cannot open " << fileName2 << endl;
        return 1;
    }

    // read input file
    string buf;
    double chipWidth, numBlocks;
    ifs >> buf >> buf >> chipWidth;  // chipWidth : xxx
    ifs >> buf >> buf >> numBlocks;  // numBlocks : xxx
    getline(ifs, buf);
    vector<double> widths(numBlocks);
    vector<double> heights(numBlocks);
    for (int i = 0; i < numBlocks; ++i) {
        getline(ifs, buf);
        istringstream iss(buf);
        iss >> buf >> buf >> widths[i] >> heights[i];  // i : yyy
    }

    // read solution file
    double chipHeight = 0;
    vector<double> x(numBlocks);
    vector<double> y(numBlocks);
    bool rotate;
    int idx;
    for (int i = 0; i < numBlocks; ++i) {
        ifs2 >> idx >> buf >> x[i] >> y[i] >> rotate;
        if (idx != i || ifs.fail()) {
            cout << "ERROR: Incorrect solution file format" << endl;
            return 1;
        }
        if (rotate) {
            swap(widths[i], heights[i]);
        }
        // check range
        if (x[i] < 0 || x[i] + widths[i] > chipWidth) {
            cout << "ERROR: Block " << i << " is out of chip width range" << endl;
            return 1;
        }
        if (y[i] < 0) {
            cout << "ERROR: Block " << i << " is out of chip height range" << endl;
            return 1;
        }
        chipHeight = max(chipHeight, y[i] + heights[i]);
    }
    // check overlap
    for (int i = 0; i < numBlocks; ++i) {
        for (int j = i + 1; j < numBlocks; ++j) {
            if (x[i] + widths[i] > x[j] && x[j] + widths[j] > x[i] &&   // x ovlp
                y[i] + heights[i] > y[j] && y[j] + heights[j] > y[i]) { // y ovlp
                cout << "ERROR: Block " << i << " and Block " << j << " overlap with each other" << endl;
                return 1;
            }
        }
    }
    cout << "Pass the check. Chip height is " << chipHeight << "." << endl;
    return 0;
}