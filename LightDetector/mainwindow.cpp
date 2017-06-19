#include "mainwindow.h"
#include "QtGui"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <dlib/optimization.h>
#include "contourcalculator.h"

using namespace cv;
using namespace std;

QImage imageQT;
Mat maskImage, imageCV, imageCVwithContour;
vector<Point> normals;
Mat M;
QRect CroppedRect;
QPen redPen, redPenThick, whitePen, bluePen;
int posImageLableX = 0;
int posImageLableY = 0;
int distanceOfNormals=10;

bool isSelect;
bool isDrawing;
bool isCon1Active;

ContourCalculator* cc;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    hideVisual();
    ui->btm_Run->hide();
    ui->rad_Con1->hide();
    ui->btm_selection->hide();
    ui->btm_saveSelection->hide();
    ui->btm_deleteSelection->hide();
    ui->lbl_ListContours->hide();
    ui->btm_intensity->hide();
    isSelect=false;
    isCon1Active = false;
    redPen.setWidth(1);
    redPen.setColor(QColor(255,0,0));
    redPenThick.setWidth(14);
    redPenThick.setColor(QColor(255,0,0));
    whitePen.setWidth(3);
    whitePen.setColor(QColor(255,255,255));
    bluePen.setWidth(1);
    bluePen.setColor(QColor(0,0,255));
    cc = new ContourCalculator();
    }



MainWindow::~MainWindow()
{
    delete ui;
    delete cc;
}


void MainWindow::on_btm_image_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose the Image you want to analyse"), "Testbilder/mitMasken", tr("Images (*.jpg)"));
     std::string str = filename.toStdString();
     imageCV = imread(str.c_str(), CV_LOAD_IMAGE_COLOR);
     std::string filenameCV = str.substr(0,str.size()-4);
     filenameCV+="_mask.jpg";

   Mat maskImageLoaded = imread(filenameCV.c_str(), CV_8UC3);

    if (QString::compare(filename, QString()) !=0){
        bool valid = imageQT.load(filename);
        if (valid){
            imageQT=imageQT.scaled(ui->lbl_image->width(),ui->lbl_image->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
            ui->lbl_image->setPixmap(QPixmap::fromImage(imageQT));
            ui->centralWidget->activateWindow();
            ui->btm_restart->show();
            ui->lbl_mask->hide();
            ui->lbl_image->show();
            ui->btm_Run->show();
            ui->rad_Con1->hide();
            ui->btm_selection->hide();
            ui->btm_image->hide();
            cv::resize(maskImageLoaded, maskImage, Size(imageQT.width(), imageQT.height()));
            cv::resize(imageCV, imageCV, Size(imageQT.width(), imageQT.height()));
            cc->setImageCV(imageCV);
           }
    }
}


void MainWindow::on_btm_restart_clicked()
{
    ui->btm_image->show();
    ui->label_1->show();
    hideVisual();
    ui->btm_restart->hide();
    ui->btm_Run->hide();
    ui->rad_Con1->hide();
    ui->btm_selection->hide();
    ui->btm_deleteSelection->hide();
    ui->btm_saveSelection->hide();
    ui->lbl_ListContours->hide();
    ui->btm_intensity->hide();
    ui->btm_ShowN->hide();
    cc->clearContours();
}


void MainWindow::on_btm_ShowLV_clicked()
{
//    dlib::solve_least_squares_lm(objective_delta_stop_strategy(1e-7).be_verbose(),
//                                  residual,
//                                  residual_derivative,
//                                  data_samples,
//                                  x);
   CvLevMarq solver;
   solver.init	(	3,
                    normals.size(),
                    cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, DBL_EPSILON),
                    false
                    )	;
   printf("\n Solver initialisiert.");

//   bool proceed = solver.update( const CvMat *&param, CvMat *&J, CvMat *&err );
}

void MainWindow::on_btm_ShowN_clicked()
{
    setNormalVecs(distanceOfNormals);
    drawNormalVecs(distanceOfNormals);
    ui->btm_intensity->show();
    ui->btm_ShowN->hide();

}


void MainWindow::hideVisual(){
    ui->btm_ShowLV->hide();
    ui->btm_ShowN->hide();
    ui->btm_restart->hide();
    ui->lbl_mask->hide();
    ui->lbl_image->hide();
}

void MainWindow::showVisual(){
    ui->btm_ShowLV->show();
     ui->btm_ShowN->show();
}


void MainWindow::on_btm_Run_clicked(){

     cc->computeContours(maskImage);

     imageCVwithContour = imageCV.clone();
      for (int i=0; i<cc->getMainContour().size(); i++){
          drawContours( imageCVwithContour, cc->getMainContour(), i, Scalar (0, 255,0), 2, 8, cc->getHierarchy(), 0, Point() );

      }
       imshow("main contour", imageCVwithContour);
      //Display Contour as QImage
      imageQT= Mat2QImage(imageCVwithContour);
      ui->lbl_image->setPixmap(QPixmap::fromImage(imageQT));
      ui->btm_selection->show();
      ui->btm_Run->hide();
}

Mat MainWindow::QImage2Mat(QImage const& src){
    Mat tmp(src.height(), src.width(), CV_8UC3, (uchar*) src.bits(), src.bytesPerLine());
    Mat result;
    cvtColor(tmp, result, CV_BGR2RGB);
    return result;
}

QImage MainWindow::Mat2QImage(Mat const& src){
    Mat tmp;
       cvtColor(src, tmp, CV_BGR2RGB);
       QImage resultLarge((const uchar*) tmp.data, tmp.cols, tmp.rows, tmp.step, QImage::Format_RGB888);
       resultLarge.bits();
       //Bild so verkleinern, dass es passend in GUI angezeigt wird
       QImage result = resultLarge.scaled(imageQT.width(), imageQT.height(),Qt::KeepAspectRatio);
   //    printf("\n Bildgroesse Result width: %i und height: % i" , result.width(), result.height());
   //    printf("\n Bildgroesse Vorher width: %i und height: % i" , resultLarge.width(), resultLarge.height());
   return result;
}
// kann weg?
void MainWindow::cropContour(QRect rect){
    QImage croppedImage = imageQT.copy(rect);
    croppedImage.save("cropped_Image");
    ui->lbl_image->setPixmap(QPixmap::fromImage(croppedImage));
}


void MainWindow::on_btm_selection_clicked()
{
    isSelect = true;
//    ui->btm_saveSelection->show();
//    ui->btm_deleteSelection->show();
    ui->btm_selection->hide();
}

void MainWindow::mousePressEvent(QMouseEvent *event){
   if(isSelect){
      isDrawing = true;
      posImageLableX = ui->lbl_image->pos().x()+ui->centralWidget->pos().x()+ui->toggle_btm_showMask->pos().x();
      posImageLableY = ui->lbl_image->pos().y()+ ui->centralWidget->pos().y()+ui->toggle_btm_showMask->pos().y();
    //isSelect = false;
    //Set Both Points to the same Starvalue
    CroppedRect.setTopLeft(QPoint(event->pos().x()-posImageLableX, event->pos().y()-posImageLableY));
    paintStartPoint();
    markNrOfContour();
    printf("\n Cropped Rect top left x=%i und y=%i", CroppedRect.topLeft().x(), CroppedRect.topLeft().y());
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){


       if(event->type() == QEvent::MouseMove && isDrawing){
           posImageLableX = ui->lbl_image->pos().x()+ui->centralWidget->pos().x()+ui->toggle_btm_showMask->pos().x();
           posImageLableY = ui->lbl_image->pos().y()+ ui->centralWidget->pos().y()+ui->toggle_btm_showMask->pos().y();
          CroppedRect.setBottomRight(QPoint(event->pos().x()-posImageLableX, event->pos().y()-posImageLableY));
           paintRect();
       }
}

   void MainWindow::mouseReleaseEvent(QMouseEvent *event){
       paintRect();
       if(isDrawing){
       //When mouse is released update for the one last time
       isSelect = false;
       isDrawing = false;
       ui->btm_saveSelection->show();
       ui->btm_deleteSelection->show();
        }
   }

   void MainWindow::paintSubContour(){
       Mat temp = imageCV.clone();
       for (int i = 0; i < cc->getSubContour().size()-1; ++i) {
        line(temp,cc->getSubContour()[i],cc->getSubContour()[i+1],Scalar (0, 255,0), 2);
       }
       imageQT= Mat2QImage(temp);
       ui->lbl_image->setPixmap(QPixmap::fromImage(imageQT));
        }


   void MainWindow::paintRect(){
       imageQT= Mat2QImage(imageCVwithContour);
       QPainter painter(&imageQT);
      painter.setPen(redPen);
       if(isSelect && isDrawing){
           painter.drawRect(CroppedRect);
       }
       ui->lbl_image->setPixmap(QPixmap::fromImage(imageQT));
       painter.end();

       paintStartPoint();
       markNrOfContour();
   }

   void MainWindow::paintStartPoint(){
       QPainter painter(&imageQT);
      painter.setPen(redPenThick);
       if(isSelect && isDrawing){
           painter.drawPoint(CroppedRect.topLeft());
       }
       ui->lbl_image->setPixmap(QPixmap::fromImage(imageQT));
       painter.end();
   }
//Momentan wird nur eine 1 gesetzt, weil nur eine Kontur möglich (ausbaufähig)
void MainWindow::markNrOfContour(){
       QPainter painter(&imageQT);
      painter.setPen(whitePen);
       if(isSelect && isDrawing){
           painter.drawText(QPoint(CroppedRect.topLeft().rx()-2,CroppedRect.topLeft().ry()+3 ), "1");
       }
       ui->lbl_image->setPixmap(QPixmap::fromImage(imageQT));
       painter.end();
 }




void MainWindow::on_btm_saveSelection_clicked(){
  ui->btm_saveSelection->hide();
  ui->btm_deleteSelection->hide();
  ui->btm_ShowLV->show();
   ui->btm_ShowN->show();
  //cropContour(CroppedRect);
  cc->savePartOfContour(CroppedRect);
  if(cc->getSubContour().size()>=3){
      paintSubContour();
      ui->lbl_ListContours->show();
      ui->rad_Con1->setChecked(true);
      ui->rad_Con1->show();
  }
  else if(cc->getSubContour().size()<3){
     deleteDrawnSelection();
     QMessageBox::information(
                 this,
                 tr("Warning"),
                 tr("Selection was not saved, because its belonging Contour was too short for further calculations.") );

 }
  isCon1Active = true;
}

void MainWindow::deleteDrawnSelection(){
    //optische Löschung der Kontur


     ui->lbl_image->clear();
     imageQT = Mat2QImage(imageCV.clone());
     ui->lbl_image->setPixmap(QPixmap::fromImage(imageQT));


    //Löschen des Konturenvektors:
    //Löschen der Normalen und des Subkonturen falls wir Button auch verwenden wollen um nachträglich Konturen zu löschen :)
    cc->clearContours();
    normals.clear();
//    printf("\n \n Anzahl Main-Normale nach Loeschung: %i", cc->getMainContour().size());
//    printf("\n Anzahl Sub-Normalen nach Loeschung: %i", normals.size());
//    printf("\n Anzahl Subkonturen nach Loeschung: %i", cc->getSubContour().size());

}

void MainWindow::on_btm_deleteSelection_clicked()
{
   deleteDrawnSelection();
   ui->btm_selection->show();
   ui->btm_deleteSelection->hide();
   ui->btm_saveSelection->hide();
}

void MainWindow::on_rad_Con1_toggled(bool checked)
{
    if(checked){
        paintSubContour();
        isCon1Active = true;
    }
    else if(!checked){
        deleteDrawnSelection();
       isCon1Active= false;
    }
}

void MainWindow::setNormalVecs(int distance){
    for(int i = 0; i < cc->getSampledSubContour().size()-distance; i+=distance){
        Point startPos = cc->getSampledSubContour().at(i);
        Point endPos = cc->getSampledSubContour().at(i+distance);
        int dx = endPos.x - startPos.x;
        int dy = endPos.y - startPos.y;

        //Point normalOne = Point(-dy,dx); //Scheinen wir nicht zu brauchen, da diese Normale immer in dem Objekt liegt
        Point normalTwo = Point(dy,-dx);
        //TO DO: Länge der Normalen angleichen
        normals.push_back(normalTwo);
    }
    printf("\n Anzahl Normalen: %i" , normals.size());
    createM();

}

void MainWindow::drawNormalVecs(int distance){
      QPainter normalPainter(&imageQT);
      normalPainter.setPen(redPen);
      int counter = 0;
      int i=0;
      int endX, endY;
      while(counter < normals.size()){
        endX = cc->getSampledSubContour().at(i+distance/2).x - normals.at(counter).x;
        endY = cc->getSampledSubContour().at(i+distance/2).y - normals.at(counter).y;
        normalPainter.drawLine(cc->getSampledSubContour().at(i+distance/2).x,cc->getSampledSubContour().at(i+distance/2).y,endX,endY);
        counter ++;
        i +=distance;
      }
//      normalPainter.setPen(bluePen);
//      normalPainter.drawLine(SubContour.at(i-1+distance/2).x,SubContour.at(i-1+distance/2).y,endX,endY);// Letzte Normale der Kontur
//      normalPainter.setPen(whitePen);
//      int a = SubContour.at(0).x - normals.at(0).x;
//      int b = SubContour.at(0).y - normals.at(0).y;
//      normalPainter.drawLine(SubContour.at(0).x, SubContour.at(0).y,a,b ); //Erste Normale der Kontur

     ui->lbl_image->setPixmap(QPixmap::fromImage(imageQT));
     normalPainter.end();
}

//R ist Einheitszahl= 1
//3 Unbekannte: L.x, L.y und A (int oder float?)
//Ist die Frage ob wir die Intensity überhaupt berechnen müssen, weil wir ja eigentlich nur L rausbekommen wollen.
void MainWindow::calculateIntensity(int R, vector<Point> N, vector<Point> L, int A){
    int NL;

    for (int i=0; i<L.size(); i++){
    NL = N.at(i).x * L.at(i).x;
    NL+= N.at(i).y * L.at(i).y;
    int I = R * NL +A;
    printf("\n Die Intensitaet an Normale %i liegt bei: %i ",i, I);
}
}

void MainWindow::on_btm_intensity_clicked()
{
    vector<Point> L;
    L.push_back(Point(2,3));
     L.push_back(Point(3,1));
      L.push_back(Point(2,4));
    calculateIntensity(1, normals, L, 4);
    ui->btm_intensity->hide();
}


//  ( N0x, N0y, 1 )
//M=( N1x, N1y, 1 )
//  ( ........... )
void MainWindow::createM(){
    M =  Mat::zeros(normals.size(),3 , CV_8U);
    for(int i = 0; i< normals.size(); i++){
    M.at<int>(i,0) = normals.at(i).x ;
    M.at<int>(i,1) = normals.at(i).y ;
    M.at<int>(i,2) = 1 ;
    printf("\n Zeile %i von M: %i, %i, %i", i, M.at<int>(i,0), M.at<int>(i,1),  M.at<int>(i,2) );
    }


   imshow("Matrix M", M);
}
