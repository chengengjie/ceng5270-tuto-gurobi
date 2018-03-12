#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <random>

using namespace std;

int main(int argc, char **argv) {
    int chipWidth = 50;
    int targetArea = chipWidth * chipWidth;
    default_random_engine generator;
    uniform_int_distribution<int> widthDistribution(10, 20);
    uniform_real_distribution<double> relHeightDistribution(0.5, 2);
    vector<int> blockWidths;
    vector<int> blockHeights;
    int area = 0;
    while (area < targetArea) {
        int width = widthDistribution(generator);
        int height = width * relHeightDistribution(generator);
        blockWidths.push_back(width);
        blockHeights.push_back(height);
        area += width * height;
    }

    string fileName = "sample.txt";
    ofstream ofs(fileName);
    cout << "Write into " << fileName << endl;
    ofs << "chipWidth : " << chipWidth << endl;
    ofs << "numBlocks : " << blockWidths.size() << endl;
    for (int i = 0; i < blockWidths.size(); ++i) {
        ofs << i << " : " << blockWidths[i] << " " << blockHeights[i] << endl;
    }
    return 0;
}