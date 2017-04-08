//
// Created by Poodar Chu on 4/8/17.
//

#ifndef OPENCV_BASICS_DESCRIPTOR_H
#define OPENCV_BASICS_DESCRIPTOR_H

#include <iostream>
#include <vector>

using namespace std;

class Descriptor {
public:
    float xi, yi;  // location
    vector<double> fv; // feature vector: an image is divided into N*N blocks. every block has a magnitude and orientation.

    Descriptor() {

    }

    Descriptor(float x, float y, vector<double> const& f) {
        xi = x;
        yi = y;
        fv= f;
    }
};

#endif //OPENCV_BASICS_DESCRIPTOR_H
