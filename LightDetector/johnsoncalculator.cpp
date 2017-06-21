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
    int patchSize =2;
    int normalsUsed = normals.size();
    while(normalsUsed%patchSize !=0){
        normalsUsed--;
    }
    int ncols= normalsUsed/patchSize+1;
    int i=0;
    int r = 0;

    //printf("\n Laenge Normalen: %i genutzte Normalen:%i", normals.size(), normalsUsed);

    Mat Mp =  Mat::zeros(normalsUsed, ncols , CV_32F);

//    for(int c = 0; c< ncols-2; c+=2){
//        for(int r=0; r<normalsUsed-patchSize; r+=patchSize){
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

    printf("\n %f, %f, %f, %f, %f, %f, %f, %f ... %f" , Mp.at<float>(0,0) , Mp.at<float>(0,1) , Mp.at<float>(0,2) , Mp.at<float>(0,3) , Mp.at<float>(0,4) , Mp.at<float>(0,5) , Mp.at<float>(0,6) , Mp.at<float>(0,7),  Mp.at<float>(0,ncols-1));
    printf("\n %f, %f, %f, %f, %f, %f, %f, %f  ... %f" , Mp.at<float>(1,0) , Mp.at<float>(1,1) , Mp.at<float>(1,2) , Mp.at<float>(1,3) , Mp.at<float>(1,4) , Mp.at<float>(1,5) , Mp.at<float>(1,6) , Mp.at<float>(1,7),  Mp.at<float>(1,ncols-1));
    printf("\n %f, %f, %f, %f, %f, %f, %f, %f  ... %f" , Mp.at<float>(2,0) , Mp.at<float>(2,1) , Mp.at<float>(2,2) , Mp.at<float>(2,3) , Mp.at<float>(2,4) , Mp.at<float>(2,5) , Mp.at<float>(2,6) , Mp.at<float>(2,7),  Mp.at<float>(2,ncols-1));
    printf("\n %f, %f, %f, %f, %f, %f, %f, %f  ... %f" , Mp.at<float>(3,0) , Mp.at<float>(3,1) , Mp.at<float>(3,2) , Mp.at<float>(3,3) , Mp.at<float>(3,4) , Mp.at<float>(3,5) , Mp.at<float>(3,6) , Mp.at<float>(3,7),  Mp.at<float>(3,ncols-1));
    printf("\n %f, %f, %f, %f, %f, %f, %f, %f  ... %f" , Mp.at<float>(4,0) , Mp.at<float>(4,1) , Mp.at<float>(4,2) , Mp.at<float>(4,3) , Mp.at<float>(4,4) , Mp.at<float>(4,5) , Mp.at<float>(4,6) , Mp.at<float>(4,7),  Mp.at<float>(4,ncols-1));
    printf("\n %f, %f, %f, %f, %f, %f, %f, %f  ... %f" , Mp.at<float>(5,0) , Mp.at<float>(5,1) , Mp.at<float>(5,2) , Mp.at<float>(5,3) , Mp.at<float>(5,4) , Mp.at<float>(5,5) , Mp.at<float>(5,6) , Mp.at<float>(5,7),  Mp.at<float>(5,ncols-1));
    //imshow("Matrix M", M);
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
