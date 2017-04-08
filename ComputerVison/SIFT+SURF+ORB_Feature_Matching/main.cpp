#include <iostream>
#include <opencv2/opencv.hpp>

#include "SIFT.h"
#include "SURF_test.h"
#include "ORB_test.h"
#include "SIFT_test.h"
#include "RandomFerns.h"

using namespace std;
using namespace cv;

int main() {

/**
 * 1. SIFT from scratch
 * 2. SIFT detector using OpenCV
 */
//    SIFT* sift_test = new SIFT("/Users/poodar/Desktop/WechatIMG7.jpeg", 8, 2);
//
//    sift_test->DoSift();
//    sift_test->ShowAbsSigma();
//    sift_test->ShowKeypoints();
//
//    cvWaitKey(0);
//
//    delete sift_test;

//    SIFT_test();

/**
 * 3. SURF detector using OpenCV
 */
//    SURF_test();   // match object and object
//    SURT_test2();  // match object and scene

/**
 * 4. ORB detector using OpenCV
 */
//    ORB_test();

/**
 * 5. Random Ferns 目标跟踪
 * 参考：http://blog.csdn.net/stayfoolish_fan/article/details/50506906
 */


    return 0;
}