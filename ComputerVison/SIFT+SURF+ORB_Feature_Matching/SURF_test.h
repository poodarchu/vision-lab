//
// Created by Poodar Chu on 4/8/17.
//

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/gpu/gpu.hpp>

#include <iostream>

using namespace cv;
using namespace std;
using namespace cv::gpu;

//read an image in global.
Mat sceneP = imread("/Users/poodar/Developments/Cpp/ClionProjects/OpenCV_Basics/images/hdd_rotated.jpg");
Mat outImg;

void findObject ( Mat objectP, int minHessian, Scalar color );

void SURF_test() {

    Mat find = imread("/Users/poodar/Developments/Cpp/ClionProjects/OpenCV_Basics/images/hdd.jpg");
    Mat label = imread("/Users/poodar/Developments/Cpp/ClionProjects/OpenCV_Basics/images/label1.jpg");
    Mat label2 = imread("/Users/poodar/Developments/Cpp/ClionProjects/OpenCV_Basics/images/label2.jpg");

    outImg = sceneP;


    if(find.empty() && sceneP.empty()) {
        cout << "Could not load the image!" << endl;
        exit(0);
    } else {
        cout << "Images loaded successfully" << endl;
    }

    findObject( label, 2500, Scalar(255,0,0) );
    findObject( label2, 1500, Scalar(0,255,0) );
    findObject ( find, 1500, Scalar(0,0,255) );

    namedWindow("Match");
    imshow("Match",outImg);


    waitKey(0);
}

void findObject( Mat objectP, int minHessian, Scalar color ) {

    // vector of key points
    vector<cv::KeyPoint> keypointsO;
    vector<cv::KeyPoint> keypointsS;
    cout << "Looking for object...\n";

    // Start the timer
    // double exec_time = (double)getTickCount();
    // do something ...
    // exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();
    double exec_time;
    exec_time = static_cast<double>(cv::getTickCount());



    SurfFeatureDetector surf(minHessian);
    surf.detect(objectP,keypointsO);
    surf.detect(sceneP,keypointsS);


    // Step 2: Calculate descriptors (feature vectors)
    SurfDescriptorExtractor extractor;

    Mat descriptors_object, descriptors_scene;

    extractor.compute( objectP, keypointsO, descriptors_object );
    extractor.compute( sceneP, keypointsS, descriptors_scene );



    // Step 3: Matching descriptor vectors using FLANN matcher
    FlannBasedMatcher matcher;
    // BFMatcher matcher(NORM_L1);
    std::vector< DMatch > matches;
    matcher.match( descriptors_object, descriptors_scene, matches );

    double max_dist = 0; double min_dist = 150;
    double dist;

    // Quick calculation of min and max distances between key points
    for(int i=0; i<descriptors_object.rows; i++)
    {
        dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }

    vector< DMatch > good_matches;

    for(int i = 0; i < descriptors_object.rows; i++)
    {
        if( matches[i].distance < 3*min_dist)
            good_matches.push_back( matches[i] );
    }

    // Localize the object
    if( good_matches.size() >=8 && good_matches.size() <= 30)
    {
        cout << "OBJECT FOUND!" << endl;
        std::vector<Point2f> obj;
        std::vector<Point2f> scene;

        for( unsigned int i = 0; i < good_matches.size(); i++ )
        {
            // Get the keypoints from the good matches
            obj.push_back( keypointsO[ good_matches[i].queryIdx ].pt );
            scene.push_back( keypointsS[ good_matches[i].trainIdx ].pt );
        }

        Mat H = findHomography( obj, scene, CV_RANSAC );



        // Get the corners from the image_1 ( the object to be "detected" )
        std::vector<Point2f> obj_corners(4);
        obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( objectP.cols, 0 );
        obj_corners[2] = cvPoint( objectP.cols, objectP.rows ); obj_corners[3] = cvPoint( 0, objectP.rows );
        std::vector<Point2f> scene_corners(4);

        perspectiveTransform( obj_corners, scene_corners, H);

        // Draw lines between the corners (the mapped object in the scene - image_2 )
        line( outImg, scene_corners[0] , scene_corners[1], Scalar(255,255,0), 2 ); //TOP line
        line( outImg, scene_corners[1] , scene_corners[2], color, 2 );
        line( outImg, scene_corners[2] , scene_corners[3], color, 2 );
        line( outImg, scene_corners[3] , scene_corners[0] , color, 2 );
    }
    else cout << "OBJECT NOT FOUND!" << endl;

    exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();

    cout << "Good matches found: " << good_matches.size() << endl;
    cout << "Algorithm duration: " << exec_time << endl << "--------------------------------------" << endl;


    // drawing the results
    namedWindow("matches");
    Mat img_matches;
    drawMatches(objectP, keypointsO, sceneP, keypointsS, good_matches, img_matches);
    imshow("matches", img_matches);

    waitKey(100);
}

void SURT_test2() {

    // vector of key points
    vector<cv::KeyPoint> keypointsO;
    vector<cv::KeyPoint> keypointsS;

    Mat objectP = imread("/Users/poodar/Developments/Cpp/ClionProjects/OpenCV_Basics/images/hdd_sm.jpg");
    Mat sceneP = imread("/Users/poodar/Developments/Cpp/ClionProjects/OpenCV_Basics/images/scene.jpg");
    Mat outImg;

    gpu::GpuMat dst, src, src2;

    if(objectP.empty())
    {
        cout << "Could not load the image!" << endl;
    }
    else
    {
        cout << "Image loaded succesfully" << endl;
    }

    // Start the timer
    double duration = static_cast<double>(cv::getTickCount());

    SurfFeatureDetector surf(2500);
    surf.detect(objectP,keypointsO);
    surf.detect(sceneP,keypointsS);


    // Step 2: Calculate descriptors (feature vectors)
    SurfDescriptorExtractor extractor;

    Mat descriptors_object, descriptors_scene;

    extractor.compute( objectP, keypointsO, descriptors_object );
    extractor.compute( sceneP, keypointsS, descriptors_scene );

    // Step 3: Matching descriptor vectors using FLANN matcher
    FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher.match( descriptors_object, descriptors_scene, matches );


    // reduces the count of matches to 25
    nth_element(matches.begin(),matches.begin()+24,matches.end());
    matches.erase(matches.begin()+25,matches.end());

    drawMatches(objectP,keypointsO,sceneP,keypointsS,matches,outImg,Scalar::all(-1), Scalar::all(-1),vector<char>(),DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    // Localize the object
    std::vector<Point2f> obj;
    std::vector<Point2f> scene;

    for( int i = 0; i < matches.size(); i++ )
    {
        // Get the keypoints from the good matches
        obj.push_back( keypointsO[ matches[i].queryIdx ].pt );
        scene.push_back( keypointsS[ matches[i].trainIdx ].pt );
    }

    Mat H = findHomography( obj, scene, CV_RANSAC );

    // Get the corners from the image_1 ( the object to be "detected" )
    std::vector<Point2f> obj_corners(4);
    obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( objectP.cols, 0 );
    obj_corners[2] = cvPoint( objectP.cols, objectP.rows ); obj_corners[3] = cvPoint( 0, objectP.rows );
    std::vector<Point2f> scene_corners(4);

    perspectiveTransform( obj_corners, scene_corners, H);

    // Draw lines between the corners (the mapped object in the scene - image_2 )
    line( outImg, scene_corners[0] + Point2f( objectP.cols, 0), scene_corners[1] + Point2f( objectP.cols, 0), Scalar(0, 255, 0), 4 );
    line( outImg, scene_corners[1] + Point2f( objectP.cols, 0), scene_corners[2] + Point2f( objectP.cols, 0), Scalar( 0, 255, 0), 4 );
    line( outImg, scene_corners[2] + Point2f( objectP.cols, 0), scene_corners[3] + Point2f( objectP.cols, 0), Scalar( 0, 255, 0), 4 );
    line( outImg, scene_corners[3] + Point2f( objectP.cols, 0), scene_corners[0] + Point2f( objectP.cols, 0), Scalar( 0, 255, 0), 4 );


    duration = static_cast<double>(cv::getTickCount());
    duration /= cv::getTickFrequency();

    namedWindow("Match");
    imshow("Match",outImg);
    cout << "Macthes found: " << matches.size() << endl;
    cout << "Algorithm duration: " << duration << endl;

    waitKey(0);
}
