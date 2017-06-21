#ifndef LIGHTCALCULATOR_H
#define LIGHTCALCULATOR_H
#include <opencv2/opencv.hpp>

class LightCalculator
{
public:
    LightCalculator();
    void runLightCalculator(cv::Mat M, cv::Mat I);
};

#endif // LIGHTCALCULATOR_H
