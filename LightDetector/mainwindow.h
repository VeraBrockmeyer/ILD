#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <opencv2/opencv.hpp>
using std::vector;
namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btm_image_clicked();

    void on_btm_restart_clicked();

    void on_btm_ShowLV_clicked();

    void on_btm_Run_clicked();

    void on_btm_selection_clicked();

    void on_btm_saveSelection_clicked();

    void on_btm_deleteSelection_clicked();

    void on_rad_Con1_toggled(bool checked);

    void on_btm_intensity_clicked();

    void on_btm_ShowN_clicked();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    Ui::MainWindow *ui;
    void showVisual();
    void hideVisual();
    cv::Mat QImage2Mat(QImage const& src);
    QImage Mat2QImage(cv::Mat const& src);
    void paintRect();
    void paintStartPoint();
    void markNrOfContour();
    void deleteDrawnSelection();
    void paintSubContour();
    void setNormalVecs(int distance);
    void drawNormalVecs(int distance);
    void calculateIntensity(int R, vector<cv::Point> N, vector<cv::Point> L, int A);
    void createM();
    void cropContour(QRect rect);


};

#endif // MAINWINDOW_H
