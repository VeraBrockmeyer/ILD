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
    void calculateIntensity(int R, vector<Point> N, vector<Point> L, int A);
    //void setNormals(const vector<Point>  &value);
    vector<Point> getNormals() const;
    void clearNormals();
    void setNormalVecs(const int distance, vector<Point> gss );
    void calculateLightVector();
   // void drawNormalVecs(int distance);



private:
    Mat M;
    vector<Point> normals;

};

#endif // JohnsonCalculator_H
