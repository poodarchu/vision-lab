//
// Created by Poodar Chu on 4/8/17.
//

#ifndef OPENCV_BASICS_KEYPOINT_H
#define OPENCV_BASICS_KEYPOINT_H

#include <iostream>
#include <vector>

using namespace std;

class Keypoint {
public:
    float xi;
    float yi;
    vector<double> mag;
    vector<double> orien;
    unsigned int scale;

    Keypoint() {

    }

    Keypoint(float x, float y) {
        xi = x;
        yi = y;
    }

    Keypoint(float x, float y, vector<double> const& m, vector<double> const& o, unsigned int s) {
        xi = x;
        yi = y;
        mag = m;
        orien = o;
        scale = s;
    }
};

#endif //OPENCV_BASICS_KEYPOINT_H
