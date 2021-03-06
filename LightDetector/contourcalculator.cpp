#include "contourcalculator.h"
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

ContourCalculator::ContourCalculator()
{
}

ContourCalculator::~ContourCalculator(){

}
void ContourCalculator::computeContours(cv::Mat maskImage){
    ContourCalculator::maskImage=maskImage;
    // Dilation to make sure that the Object is bigger than its mask
    Mat imageAfterMorph;
    int erosion_size = 1;
    cv::Mat element = getStructuringElement(cv::MORPH_ELLIPSE,
                                            cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
                                            cv::Point(erosion_size, erosion_size));
    morphologyEx(maskImage, imageAfterMorph, cv::MORPH_ERODE, element, cv::Point(-1, -1));
    //imshow("after Morph",imageAfterMorph);
    Mat imageCanny;
    // Canny to detect edges
    Canny(imageAfterMorph, imageCanny, 0, 1200 , 5);
    //imshow("canny",imageCanny);
    //   erosion_size = 1;
    //   element = getStructuringElement(cv::MORPH_ELLIPSE,
    //                                            cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
    //                                            cv::Point(erosion_size, erosion_size));
    //    morphologyEx(imageCanny, imageCanny, cv::MORPH_DILATE, element, cv::Point(-1, -1));
    //imshow("canny after",imageCanny);
    // Find associated Contours and draw them (in our case it) into the Mat imageWithContours
    findContours(imageCanny, MainContour, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0,0));
    // printf("\n Anzahl Konturen: %i " , MainContour.size());

}

std::vector<cv::Vec4i> ContourCalculator::getHierarchy() const
{
    return hierarchy;
}

void ContourCalculator::setImageCV(const cv::Mat &value)
{
    imageCV = value;
}

void ContourCalculator::clearCurrentSelction(){
    SubContour.clear();
    SampledSubContour.clear();
    hierarchy.clear();
    hasHighestPoint = false;
}
void ContourCalculator::clearContours(){
    SubContour.clear();
    MainContour.clear();
    SampledSubContour.clear();
    hierarchy.clear();
    hasHighestPoint = false;
}
void ContourCalculator:: sortSubContour(){
    // apply it to the contours:
    vector<Point> SortedSubContour;
    int mostLeftPos = 0;
    int mostRightPos = 0;
    Point mostLeftDown = Point(maskImage.size().width,0);
    Point mostRightDown = Point(0,0);

    for (int i = 0; i < SubContour.size()-1; ++i) {
        Point p = SubContour[i];
        if(p.x<=mostLeftDown.x && p.y >= mostLeftDown.y){
            mostLeftDown = p;
            mostLeftPos=i;
        }
        if(p.x>=mostRightDown.x && p.y >= mostRightDown.y){
            mostRightDown = p;
            mostRightPos=i;
        }
    }


    if (hasHighestPoint && mostRightPos>mostLeftPos){
        for (int i = mostLeftPos; i >= 0; i--) {
            SortedSubContour.push_back(SubContour[i]);
        }
        if(mostRightPos != 0 && mostRightPos != SubContour.size()-1){
            for (int i = SubContour.size()-1; i >= mostRightPos; i--) {
                SortedSubContour.push_back(SubContour[i]);
            }
        }
    }
    else{
        for (int i = SubContour.size()-1; i >= 0; i--) {
            SortedSubContour.push_back(SubContour[i]);
        }
    }
    SubContour.clear();
    SubContour= SortedSubContour;
}

std::vector<std::vector<cv::Point> > ContourCalculator::getMainContour() const
{
    return MainContour;
}

void ContourCalculator::setMainContour(const std::vector<std::vector<cv::Point> > &value)
{
    MainContour = value;
}

std::vector<cv::Point> ContourCalculator::getSubContour() const
{
    return SubContour;
}

void ContourCalculator::setSubContour(const std::vector<cv::Point> &value)
{
    SubContour = value;
}

std::vector<cv::Point> ContourCalculator::getSampledSubContour() const
{
    return SampledSubContour;
}

void ContourCalculator::setSampledSubContour(const std::vector<cv::Point> &value)
{
    SampledSubContour = value;
}

void ContourCalculator::runLineIterator(Mat tempLineImg,Point pt1, Point pt2){
    // grabs pixels along the line (pt1, pt2)
    // from 8-bit 3-channel image to the buffer
    LineIterator it(tempLineImg, pt1, pt2, 8, true);
    for(int i = 0; i < it.count; i++, ++it){
        Point buffer = it.pos();
        SampledSubContour.push_back(buffer);
    }
}

void  ContourCalculator::computePixelCoordsAlongContour(){
    for (int i = 0; i < SubContour.size()-1; ++i) {
        Mat tempLineImg = maskImage.clone();
        tempLineImg= Mat::zeros(tempLineImg.size(),tempLineImg.type());
        line(tempLineImg,SubContour[i],SubContour[i+1],Scalar(255,255,255));
        if (i==SubContour.size()-2) {
            //imshow("lastLine",tempLineImg);
        }
        runLineIterator(tempLineImg, SubContour[i],SubContour[i+1]);
    }
    Mat debug = imageCV.clone();
    circle(debug,SampledSubContour[0],5,Scalar(255,0,0));
    circle(debug,SampledSubContour[SampledSubContour.size()-1],5,Scalar(255,0,0));

    foreach (Point c, SampledSubContour) {
        circle(debug,c,1,Scalar(0,0,255));
    }
    //imshow("pixel Contour",debug);

}

void ContourCalculator::savePartOfContour(QRect CroppedRect){
    int xBoxHigh = CroppedRect.topRight().x();
    int xBoxLow = CroppedRect.topLeft().x();
    int yBoxHigh = CroppedRect.bottomRight().y();
    int yBoxLow = CroppedRect.topLeft().y();


    Mat imageDebug = Mat::zeros(imageCV.size(), CV_8UC3);
    Rect r = Rect(Point(xBoxHigh,yBoxHigh),Point(xBoxLow,yBoxLow));
    rectangle(imageDebug,r,Scalar(0,0,255));


    for (int i=0; i<MainContour.at(0).size(); i++){
        int xCon= MainContour.at(0).at(i).x;
        int yCon= MainContour.at(0).at(i).y;

        if (CroppedRect.contains(xCon, yCon)){
            if(i==0)
                hasHighestPoint=true;
            SubContour.push_back(Point(xCon,yCon));
            circle(imageDebug,Point(xCon,yCon),1,Scalar(0,255,0));

        }
        else{
            circle(imageDebug,Point(xCon,yCon),1,Scalar(255,0,0));

        }
    }
    //printf("\n Laenge der Subkontur nach Selektierung: %i (Soll Anzahl aller Konturenpunkte innerhalb der Auswahl zaehlen)" , SubContour.size());
    if(SubContour.size()>=3){
        sortSubContour();
        //imshow("Debug", imageDebug);
        computePixelCoordsAlongContour();
        //printf("\n Laenge der SampledSubKontur insgesamt: %i " , SampledSubContour.size());

    }

}

