#ifndef JohnsonCalculator_H
#define JohnsonCalculator_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <contourcalculator.h>
using namespace cv;
using namespace std;
class JohnsonCalculator
{
public:
    JohnsonCalculator();
    ~JohnsonCalculator();
    void createM();
    void calculateIntensity(const int distance, vector<Point> gss, Mat img);
    //void setNormals(const vector<Point>  &value);
    vector<Point2f> getNormals() const;
    void clearNormals();
    void setNormalVecs(const int distance, vector<Point> gss );
    void calculateLightVector();
   // void drawNormalVecs(int distance);
    void setLightvector(Point L);
    Point getLightvector();
    void createMUsingPatches();



private:
    Mat M,I;
    vector<Point2f> normals;
    Point lightvector;

};

#endif // JohnsonCalculator_H
