#include "johnsoncalculator.h"


JohnsonCalculator::JohnsonCalculator()
{

}

JohnsonCalculator::~JohnsonCalculator(){

}

void JohnsonCalculator::calculateLightVector(){
    Mat v ;
    cv::solve(M,I,v, DECOMP_SVD );
    printf("\n v cols: %i, rows: %i" , v.cols, v.rows);
    printf("\n v (%f,%f)" , v.at<float>(0,0), v.at<float>(1,0));
    Point L;
    int Lx =  v.at<float>(0,0);
    int Ly =  v.at<float>(1,0);
    L = Point(Lx,Ly);
    setLightvector(L);
    printf("Lightvector (%i,%i)" , Lx,Ly);
}

void JohnsonCalculator::setLightvector(Point L){
    lightvector = L;
}

Point JohnsonCalculator::getLightvector(){
    return lightvector;
}


//  ( N0x, N0y, 1 )
//M=( N1x, N1y, 1 )
//  ( ........... )
void JohnsonCalculator::createM(){
    M =  Mat::zeros(4,3 , CV_32F);
    for(int i = 0; i< 4; i++){
        M.at<float>(i,0) = normals.at(i).x ;
        M.at<float>(i,1) = normals.at(i).y ;
        M.at<float>(i,2) = 1.f ;
        printf("\n Zeile %i von M: %f, %f, %f", i, M.at<float>(i,0), M.at<float>(i,1),  M.at<float>(i,2) );
    }

    //imshow("Matrix M", M);
}


void JohnsonCalculator::createMUsingPatches(){
    int patchSize =4;
    int normalsUsed = normals.size();
    while(normalsUsed%patchSize !=0){
        normalsUsed--;
    }
    int ncols= (normalsUsed/patchSize)*2+1;
    int i=0;
    int r = 0;

    printf("\n Laenge Normalen: %i genutzte Normalen:%i ncols:%i", normals.size(), normalsUsed, ncols);

    Mat Mp =  Mat::zeros(normalsUsed, ncols , CV_32F);

    for(int c = 0; c< ncols-2; c+=2){
        for(int p =0; p<patchSize; p++){
            Mp.at<float>(r+p,c) = normals.at(i).x ;
            Mp.at<float>(r+p,c+1) = normals.at(i).y ;i++;
        }
        r+=patchSize;
    }

    for(int r=0; r<normalsUsed; r++){
         Mp.at<float>(r,ncols-1) = 1;
    }
      printf("\n");
      for(int r = 0; r< Mp.size().height; r++){
          for(int c = 0; c< Mp.size().width; c++){
          printf(" %f,  ", Mp.at<float>(r,c) );
          }
          printf(" \n \n" );
      }

    imshow("Matrix Mp", Mp);
}


void JohnsonCalculator::calculateIntensity(const int distance, vector<Point> gss, Mat img){
    I =  Mat::zeros(4,1 , CV_32F);
    printf("Size of I: %i ; Size of normals: %i", I.size().height, normals.size());
  int  counter=0;
    for(int i = 0; i < gss.size()-distance; i+=distance){
        counter ++;
        Point p = gss.at(i);
        Vec3b intensity = img.at<Vec3b>(p.y, p.x);
        uchar blue = intensity.val[0];
        uchar green = intensity.val[1];
        uchar red = intensity.val[2];
        float val = 0.299*red + 0.587*green + 0.114*blue;
        I.at<float>(i, 0) = val;
        printf("/n Intensity: %f",  I.at<float>(i, 0));
        if(counter==4)break;
    }
    //imshow("I",I);
}

void JohnsonCalculator::clearNormals(){
    normals.clear();
}

void JohnsonCalculator::setNormalVecs(const int distance, vector<Point> gss )
{
    for(int i = 0; i < gss.size()-distance; i+=distance){
        Point startPos = gss.at(i);
        Point endPos = gss.at(i+distance);
        float dx = endPos.x - startPos.x;
        float dy = endPos.y - startPos.y;
//        float l = sqrt(dx*dx+dy*dy);
//        dx/=l;
//        dy/=l;
        //Point normalOne = Point(dy,-dx);
        Point2f normalAsPoint = Point(-dy,dx);

        normals.push_back(normalAsPoint);
    }
    //printf("\n Anzahl Normalen: %i" , normals.size());
    createM();
    createMUsingPatches();
}

vector<Point2f> JohnsonCalculator::getNormals() const
{
    return normals;
}
