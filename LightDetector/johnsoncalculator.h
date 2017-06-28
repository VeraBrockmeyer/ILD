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
    int patchSize =4; //must be divided by 2
    JohnsonCalculator();
    ~JohnsonCalculator();
    void createM();
    void calculateIntensity(const int distance, vector<Point> gss, Mat img);
    vector<Point2f> getNormals() const;
    void clearAll();
    void setNormalVecs(const int distance, vector<Point> gss , Mat maskImage);
    void calculateLightVector();
    void setLightvector(Point L);
    Point getLightvector();
    void setLightvectorsUsingPatches(vector<float> L);
    vector<float> getLightvectorsUsingPatches();
    void createMUsingPatches();
    void createC();
    int getNrOfPatches();
    void calculateIntensityUsingPatches(const int distance, vector<Point> gss, Mat img);
    void calculateLightVectorUsingPatches();



    Mat getM() const;

    Mat getI() const;

    Mat getMp() const;

    Mat getIp() const;

private:
    Mat M,I, Mp, C, Ip;
    vector<Point2f> normals;
    Point lightvector;
    vector<float> lightvectorsUsingPatches;
    int normalsUsed;

};

#endif // JohnsonCalculator_H
