#include "mainwindow.h"
#include "QtGui"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <dlib/optimization.h>
#include "contourcalculator.h"
#include "johnsoncalculator.h"

using namespace cv;
using namespace std;

//Property Settings
bool usePatches = true; //false = basic version und true = version with patches
bool useHighestIntensity = true; //has only impact, if usePatches is true
int distanceOfNormals=10;

QImage imageQT;
Mat maskImage, imageCV, imageCVwithContour;
QRect CroppedRect;
QPen redPen, redPenThick, whitePen, bluePen;
int posImageLableX = 0;
int posImageLableY = 0;

bool isSelect;
bool isDrawing;
bool runRectMode = false;

ContourCalculator* cc;
JohnsonCalculator* jc;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    hideVisual();
    ui->btm_Run->hide();
    ui->btm_selection->hide();
    ui->btm_saveSelection->hide();
    ui->btm_deleteSelection->hide();
    ui->btm_intensity->hide();
    isSelect=false;
    redPen.setWidth(1);
    redPen.setColor(QColor(255,0,0));
    redPenThick.setWidth(14);
    redPenThick.setColor(QColor(255,0,0));
    whitePen.setWidth(1);
    whitePen.setColor(QColor(255,255,255));
    bluePen.setWidth(2);
    bluePen.setColor(QColor(0,0,255));
    cc = new ContourCalculator();
    jc = new JohnsonCalculator();
}



MainWindow::~MainWindow()
{
    delete ui;
    delete cc;
    delete jc;
}


void MainWindow::on_btm_image_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose the Image you want to analyse"), "Testbilder/mitMasken", tr("Images (*.jpg *.tif)"));
    if( filename.isNull() )
    {
        printf(" no filename \n");
    }
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
    ui->btm_selection->hide();
    ui->btm_deleteSelection->hide();
    ui->btm_saveSelection->hide();
    ui->btm_ShowN->hide();
    cc->clearContours();
    jc->clearAll();
}


void MainWindow::on_btm_ShowLV_clicked()
{
    if(!usePatches){
        jc->calculateIntensity(distanceOfNormals, cc->getSampledSubContour(), imageCV);
        jc->calculateLightVector();
        drawLV();
    }
    else if(usePatches){
        jc->calculateIntensityUsingPatches(distanceOfNormals, cc->getSampledSubContour(), imageCV);
        jc->calculateLightVectorUsingPatches();
        drawLVUsingPatches();

        if(!useHighestIntensity){
        drawFinalLightvector();
        }
        else if(useHighestIntensity){
            drawFinalLightvectorOnHighestIntensity();
        }
    }
    ui->btm_ShowLV->hide();
}


void MainWindow::on_btm_ShowN_clicked()
{
    jc->setNormalVecs(distanceOfNormals, cc->getSampledSubContour(),maskImage);
    drawNormalVecs(distanceOfNormals);
    ui->btm_ShowLV->show();
    ui->btm_ShowN->hide();
}


void MainWindow::hideVisual(){
    ui->btm_ShowLV->hide();
    ui->btm_ShowN->hide();
    ui->btm_restart->hide();
    ui->lbl_mask->hide();
    ui->lbl_image->hide();
}




void MainWindow::on_btm_Run_clicked(){

    cc->computeContours(maskImage);
    cc->setImageCV(imageCV);
    imageCVwithContour = imageCV.clone();
    for (int i=0; i<cc->getMainContour().size(); i++){
        drawContours( imageCVwithContour, cc->getMainContour(), i, Scalar (0, 255,0), 1, 8, cc->getHierarchy(), 0, Point() );

    }
    imageQT= Mat2QImage(imageCVwithContour);
    ui->lbl_image->setPixmap(QPixmap::fromImage(imageQT));
    //  paintMainContour();
    ui->btm_selection->show();
    ui->btm_Run->hide();
    runRectMode = true;
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
    return result;
}


void MainWindow::on_btm_selection_clicked()
{
    isSelect = true;
    ui->btm_selection->hide();
}

void MainWindow::mousePressEvent(QMouseEvent *event){
    if(isSelect){
        isDrawing = true;
        posImageLableX = ui->lbl_image->pos().x()+ui->centralWidget->pos().x()+ui->toggle_btm_showMask->pos().x();
        posImageLableY = ui->lbl_image->pos().y()+ ui->centralWidget->pos().y()+ui->toggle_btm_showMask->pos().y();
        //Set Both Points to the same Starvalue
        CroppedRect.setTopLeft(QPoint(event->pos().x()-posImageLableX, event->pos().y()-posImageLableY));
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
    if(runRectMode){
        paintRect();
        if(isDrawing){
            //When mouse is released update for the one last time
            isSelect = false;
            isDrawing = false;
            ui->btm_saveSelection->show();
            ui->btm_deleteSelection->show();
        }
    }
}

void MainWindow::paintSubContour(){
    Mat temp = imageCV.clone();
    //    int color = 0;

    for (int i = 0; i < cc->getSubContour().size()-1; ++i) {
        //      color++;
        line(temp,cc->getSubContour()[i],cc->getSubContour()[i+1],Scalar (0, 255,0), 1);

        //        if(color < 255){
        //        line(temp,cc->getSubContour()[i],cc->getSubContour()[i+1],Scalar (color, 0,0), 3);
        //        }
        //         else if (color <2*255){
        //            line(temp,cc->getSubContour()[i],cc->getSubContour()[i+1],Scalar ((double)255- color,0), 3);
        //        }
    }
    imageQT= Mat2QImage(temp);
    ui->lbl_image->setPixmap(QPixmap::fromImage(imageQT));
}

void MainWindow::paintMainContour(){
    Mat temp = imageCV.clone();
    int color = 0;
    for (int i = 0; i < cc->getMainContour()[0].size()-1; ++i) {
        color++;
        if(color < 255){
            line(temp,cc->getMainContour()[0][i],cc->getMainContour()[0][i+1],Scalar (color, 0,0), 1);
        }
        else if (color <2*255){
            line(temp,cc->getMainContour()[0][i],cc->getMainContour()[0][i+1],Scalar (0, (double) color-255,0), 1);
        }
        else if (color <2*255){
            line(temp,cc->getMainContour()[0][i],cc->getMainContour()[0][i+1],Scalar (0,0, (double)color-(2*255)), 1);
        }
    }
    circle(temp,cc->getMainContour()[0][0],3, Scalar(255,255,255),2);
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
}



void MainWindow::on_btm_saveSelection_clicked(){
    ui->btm_saveSelection->hide();
    ui->btm_deleteSelection->hide();
    ui->btm_ShowN->show();
    //cropContour(CroppedRect);
    cc->savePartOfContour(CroppedRect);
    if(cc->getSubContour().size()>=3){
        paintSubContour();
    }
    else if(cc->getSubContour().size()<3){
        deleteDrawnSelection();
        QMessageBox::information(
                    this,
                    tr("Warning"),
                    tr("Selection was not saved, because its belonging Contour was too short for further calculations.") );

    }
    runRectMode = false;
}

void MainWindow::deleteDrawnSelection(){
    //optische Löschung der Kontur
    ui->lbl_image->clear();
    imageQT = Mat2QImage(imageCV.clone());
    ui->lbl_image->setPixmap(QPixmap::fromImage(imageQT));
    //Löschen des Konturenvektors:
    cc->clearCurrentSelction();
    jc->clearAll();
}

void MainWindow::on_btm_deleteSelection_clicked()
{
    deleteDrawnSelection();
    ui->btm_selection->show();
    ui->btm_deleteSelection->hide();
    ui->btm_saveSelection->hide();
}



void MainWindow::drawNormalVecs(int distance){
    QPainter normalPainter(&imageQT);
    normalPainter.setPen(redPen);
    int counter = 0;
    int i=0;
    int endX, endY;
    while(counter < jc->getNormals().size()){
        endX = cc->getSampledSubContour().at(i+distance/2).x - jc->getNormals().at(counter).x;
        endY = cc->getSampledSubContour().at(i+distance/2).y - jc->getNormals().at(counter).y;
        normalPainter.drawLine(cc->getSampledSubContour().at(i+distance/2).x,cc->getSampledSubContour().at(i+distance/2).y,endX,endY);
        counter ++;
        i +=distance;
    }

    ui->lbl_image->setPixmap(QPixmap::fromImage(imageQT));
    normalPainter.end();
}


void MainWindow::drawLV(){
    QPainter LVPainter(&imageQT);
    LVPainter.setPen(whitePen);
    Point LV = jc->getLightvector();
    int middleOfContour;
    int cLength = cc->getSampledSubContour().size(); //length of contour
    if(cLength%2 == 0){
        middleOfContour = (cLength-1)/2;
    }
    else if(cLength%2 == 1){
        middleOfContour = (cLength-2)/2;
    }
    Point Pos = cc->getSampledSubContour().at(middleOfContour);
    int factor=10;
    LV.x = LV.x*factor;
    LV.y = LV.y*factor;
    Point imageLV = Pos+LV;

    LVPainter.drawLine(Pos.x, Pos.y,imageLV.x, imageLV.y);

    ui->lbl_image->setPixmap(QPixmap::fromImage(imageQT));
    LVPainter.end();
    saveResults();
}


void MainWindow::drawLVUsingPatches(){
    QPainter LVPainter(&imageQT);
    LVPainter.setPen(whitePen);
    vector<float> LVs = jc->getLightvectorsUsingPatches();
    int startX, startY,endX, endY, c=0;
    int factor = 12;
    for (int i=0; i<LVs.size()-1 ; i+=2){
        startX = cc->getSampledSubContour().at(c*jc->patchSize*distanceOfNormals+jc->patchSize*distanceOfNormals/2).x;
        startY = cc->getSampledSubContour().at(c*jc->patchSize*distanceOfNormals+jc->patchSize*distanceOfNormals/2).y;
        endX = startX + LVs.at(i)*factor;
        endY = startY + LVs.at(i+1)*factor;
        c++;
        LVPainter.drawLine(startX, startY,endX, endY);
    }
    ui->lbl_image->setPixmap(QPixmap::fromImage(imageQT));
    LVPainter.end();

}


void MainWindow::drawFinalLightvector(){
    QPainter LVPainter(&imageQT);
    LVPainter.setPen(bluePen);
    vector<float> LVs = jc->getLightvectorsUsingPatches();
    float x,y;
    float px,py;
    float counter =0;
    for (int i= 0; i<LVs.size()-2; i+=2){
        px=LVs.at(i);
        py=LVs.at(i+1);
        float vLenght = px*px + py*py;
        px/=vLenght;
        py/=vLenght;
        x+=px;
        y+=py;
        counter++;
    }

    x/=counter;
    y/=counter;

    int factor=450;
    x*=factor;
    y*=factor;
    finalLV = Point2f(x,y);
    int middleOfContour;
    int cLength = cc->getSampledSubContour().size(); //length of contour
    if(cLength % 2 == 0){
        middleOfContour = (cLength-1)/2;
    }
    else if(cLength%2 == 1){
        middleOfContour = (cLength-2)/2;
    }
    Point Pos = cc->getSampledSubContour().at(middleOfContour);

    LVPainter.drawLine(Pos.x, Pos.y,Pos.x+x, Pos.y+y);

    ui->lbl_image->setPixmap(QPixmap::fromImage(imageQT));
    LVPainter.end();
    saveResults();
}

void MainWindow::saveResults(){
    QString str = "Result_.jpg";
    imageQT.save(str,"JPEG");
    cv::FileStorage fs("ResultMats_.yml", cv::FileStorage::WRITE);
    if (fs.isOpened()){

        if(!usePatches){
            fs << "M" << jc->getM();
            fs<< "I" << jc->getI();
            fs << "LV" << jc->getLightvector();
        }
        else{
            fs << "Mp" << jc->getMp();
            fs << "Ip" << jc->getIp();
            fs << "LV" << 0;
            for (int i = 0; i < jc->getLightvectorsUsingPatches().size()-1; i+=2) {
                Point2f p = Point2f(jc->getLightvectorsUsingPatches()[i],jc->getLightvectorsUsingPatches()[i+1]);
                fs << "p" << p;
            }
            fs <<"final LV" << finalLV;
        }
    }
}

void MainWindow::drawFinalLightvectorOnHighestIntensity(){
    QPainter LVPainter(&imageQT);
    LVPainter.setPen(bluePen);
    Point LV = jc->findLVofHighestIntensity();
    int middleOfContour;
    int cLength = cc->getSampledSubContour().size(); //length of contour
    if(cLength%2 == 0){
        middleOfContour = (cLength-1)/2;
    }
    else if(cLength%2 == 1){
        middleOfContour = (cLength-2)/2;
    }
    Point Pos = cc->getSampledSubContour().at(middleOfContour);
    int factor=5;
    LV.x = LV.x*factor;
    LV.y = LV.y*factor;
    Point imageLV = Pos+LV;

    LVPainter.drawLine(Pos.x, Pos.y,imageLV.x, imageLV.y);

   //printf("\n Eingezeichneter Lichtvektor für Patch mit der größten Intensitaet (%f, %f)", LV.x, LV.y);
    ui->lbl_image->setPixmap(QPixmap::fromImage(imageQT));
    LVPainter.end();
    saveResults();
}


