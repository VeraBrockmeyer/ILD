#ifndef CONTOURCALCULATOR_H
#define CONTOURCALCULATOR_H
#include <opencv2/opencv.hpp>
#include "QtGui"

class ContourCalculator
{
public:
    ContourCalculator();
    ~ContourCalculator();
    void computePixelCoordsAlongContour();
    void runLineIterator(cv::Mat tempLineImg,cv::Point pt1, cv::Point pt2);
    void sortSubContour();
    void savePartOfContour(QRect CroppedRect);

    std::vector<std::vector<cv::Point> > getMainContour() const;
    void setMainContour(const std::vector<std::vector<cv::Point> > &value);

    std::vector<cv::Point> getSubContour() const;
    void setSubContour(const std::vector<cv::Point> &value);

    std::vector<cv::Point> getSampledSubContour() const;
    void setSampledSubContour(const std::vector<cv::Point> &value);
    void clearContours();
    void computeContours(cv::Mat maskImage);
    std::vector<cv::Vec4i> getHierarchy() const;

    void setImageCV(const cv::Mat &value);

private:
    std::vector<std::vector<cv::Point> > MainContour; //Muss wegen cv::findContour() von diesem Typ sein
    std::vector<cv::Point> SubContour; //Momentan kann zum Testen nur eine Subkontur erstellt werden
    std::vector<cv::Point> SampledSubContour;
    std::vector<cv::Vec4i> hierarchy;
    cv::Mat maskImage, imageCV;
};

#endif // CONTOURCALCULATOR_H
