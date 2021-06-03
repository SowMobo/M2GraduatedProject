/////////////////////////////////////////////////////////GOAL/////////////////////////////////////////////////////////////////////////////
/*The goal of this program is to segment a color image and then recognize
  a specific color based on threshols value
 */
/**************First, capture and display images from a camera***********************************/
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

// Trackbar values
int low[]{50, 50, 50};
int high[]{250, 250, 250};

int main(int argc, char *argv[])
{
    printf("Hit ESC key to quit...\n");
    
    std::string windowNames[]{"band 0", "band 1", "band 2"};
    
    cv::VideoCapture cap(0); // open the default camera
    if(!cap.isOpened()) // check if we succeeded
    {
        printf("error - can't open the camera\n");
        system("PAUSE");
        return -1;
    }
    
    double WIDTH  = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    double HEIGHT  = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    printf("Image width%f, height%f\n", WIDTH, HEIGHT);
    
    // Create image windows. Meaning of flags:
    // CV_WINDOW_NORMAL enables manual resizing; cv_WINDOW_AUTOSIZE is automatic
    // We can "or" the above choice with CV_WINDOW_KEEPRATIO, which keeps aspect ratio
    cv::namedWindow("Input image", cv::WINDOW_AUTOSIZE);
    for(int i{0}; i < 3; ++i)
        cv::namedWindow(windowNames[i], cv::WINDOW_AUTOSIZE);
    
    //  create trackbars
    for(int i{0}; i < 3; ++i)
    {
        cv::createTrackbar("low", windowNames[i], &low[i], 255, NULL);
        cv::createTrackbar("high", windowNames[i], &high[i], 255, NULL);
    }
    
    // Run an infinite loop until user hits the ESC key
    while(true)
    {
        cv::Mat imgInput;
        cap >> imgInput; // get image from camera
        cv::imshow("Input image", imgInput);
        
        // convert to HSV
        cv::Mat imgHSV;
        cv::cvtColor(imgInput, imgHSV, cv::COLOR_BGR2HSV);
        
        // split into planes
        cv::Mat planes[3];
        cv::split(imgHSV, planes);
        
        // Trheshol
        for(int i{0}; i < 3; ++i)
        {
            cv::Mat imgThreshLow, imgThreshHigh;
            cv::threshold(planes[i],
                         imgThreshLow, // output threshold imahe
                         low[i],        // value to use for threshold
                         255,           // output value
                         cv::THRESH_BINARY); // threshold type
            cv::threshold(planes[i],
                         imgThreshHigh, // output threshold imahe
                         high[i],        // value to use for threshold
                         255,           // output value
                         cv::THRESH_BINARY_INV); // threshold type
            cv::bitwise_and(imgThreshLow, imgThreshHigh, planes[i]);
        }
        
        //show images in the window
        for(int i{0}; i < 3; ++i)
        {
            cv::imshow(windowNames[i], planes[i]);
        }
        
        // finally, AND all the threshold images together
        cv::Mat imgResult{planes[0]};
        
        for(int i{1}; i < 3; ++i)
            cv::bitwise_and(imgResult, planes[i], imgResult);
        
        // clean up binary image using morphological operatorss
        cv::Mat structuringElmt(7,7, CV_8U, cv::Scalar(1));
        cv::morphologyEx(imgResult, imgResult, cv::MORPH_CLOSE, structuringElmt);
        
        cv::imshow("Binary result", imgResult);
        
        // Find connected components contours
        std::vector<std::vector<cv::Point> > contours;
        cv::findContours(
                 imgResult,             // input image (is destroyed)
                 contours,              // output vector of contours
                 cv::RETR_LIST,          // retrieve all contours
                 cv::CHAIN_APPROX_NONE); // all pixels of each contours
        
        // Draw contours on original image
        cv::drawContours(imgInput, contours,
                    -1,      // contour number to draw (-1 means draw all)
                    cv::Scalar(255, 255, 255),  // color
                    2,                         //thickness (-1 means fill)
                    8);                        // line connectivity
        
        cv::imshow("Overlay", imgInput);
        
        // wait for x ms (0 means wait until a keypress)
        if(cv::waitKey(33) == 27)
            break; // ESC is ascii 27
    }
    
    return EXIT_SUCCESS;
}

