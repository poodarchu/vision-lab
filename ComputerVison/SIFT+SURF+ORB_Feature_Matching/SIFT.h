//
// Created by Poodar Chu on 4/8/17.
//

#ifndef OPENCV_BASICS_SIFT_H
#define OPENCV_BASICS_SIFT_H

/**
 * 参考 http://aishack.in/tutorials/sift-scale-invariant-feature-transform-introduction/ 上详细的 SIFT 原理与实现
 */

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "KeyPoint.h"
#include "Descriptor.h"

class SIFT {
public:
    SIFT(IplImage* img, int octaves, int intervals);
    SIFT(const char* fileName, int octaves, int intervals);
    ~SIFT();

    void DoSift();

    void ShowKeypoints();
    void ShowAbsSigma();

private:
    void GenerateLists();
    void BuildScaleSpace();
    void DetectExtrema();
    void AssignOrientations();
    void ExtractKeypointDescriptors();

    unsigned int GetKernelSize(double sigma, double cut_off=0.001);
    CvMat* BuildInterpolatedGaussianTable(unsigned int size, double sigma);
    double gaussian2D(double x, double y, double sigma);

private:
    IplImage* m_srcImage;   // the image applied.
    unsigned int m_numOctaves; // the desired number of octaves
    unsigned int m_numIntervals; // the desired number of intervals
    unsigned int m_numKeypoints;  //the number of key points detected

    IplImage*** m_gList;  //A 2D array to hold the different gaussian blurred images
    IplImage*** m_dogList;  // A 2D array to hold the different Dog images
    IplImage*** m_extrema;  // A 2D array to hold binary images. In the binary image, 1 = extrema, 0 = not extrema
    double** m_absSigma;  //A 2D array to hold the sigma used to blur a particular image

    vector<Keypoint> m_keyPoints;  // Hold each key point's basic info
    vector<Descriptor> m_keyDescs;  // Hold each key point's descriptor

};


#endif //OPENCV_BASICS_SIFT_H
