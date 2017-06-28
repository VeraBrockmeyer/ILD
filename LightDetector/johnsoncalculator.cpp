#include "johnsoncalculator.h"


JohnsonCalculator::JohnsonCalculator()
{

}

JohnsonCalculator::~JohnsonCalculator(){

}

void JohnsonCalculator::calculateLightVector(){
    Mat v;
    cv::solve(M,I,v, DECOMP_SVD );
    Point L;
    int Lx =  v.at<float>(0,0);
    int Ly =  v.at<float>(1,0);
    L = Point(Lx,Ly);
    setLightvector(L);
    for(int i = 0; i<v.size().height-2; i+=2){
        // printf("%f " , v.at<float>(i,0));
        //printf("%f \n " , v.at<float>(i+1,0));
    }
}

void JohnsonCalculator::calculateLightVectorUsingPatches(){
    Mat MpT = Mp.t();
    Mat CT = C.t();
    Mat bracket = (MpT*Mp)  + (0.5f*CT*C);
    Mat bracketInvert = bracket.inv(DECOMP_SVD);
    Mat bvec = MpT*Ip;
    //Mat v = bracket *bvec;
    Mat v = bracketInvert *bvec;


    vector<float> L;
    // printf("\n \n Lightvectors \n");
    for(int i = 0; i<v.size().height-2; i+=2){
        L.push_back(v.at<float>(i,0));
        L.push_back(v.at<float>(i+1,0));
        // printf("%f " , v.at<float>(i,0));
        // printf("%f \n " , v.at<float>(i+1,0));
    }
    setLightvectorsUsingPatches(L);
}


Mat JohnsonCalculator::getM() const
{
    return M;
}

Mat JohnsonCalculator::getI() const
{
    return I;
}

Mat JohnsonCalculator::getMp() const
{
    return Mp;
}

Mat JohnsonCalculator::getIp() const
{
    return Ip;
}

void JohnsonCalculator::setLightvector(Point L){
    lightvector = L;
}

Point JohnsonCalculator::getLightvector(){
    return lightvector;
}

void JohnsonCalculator::setLightvectorsUsingPatches(vector<float> L){
    lightvectorsUsingPatches = L;
}

vector<float> JohnsonCalculator::getLightvectorsUsingPatches(){
    return lightvectorsUsingPatches;
}


//Creates Matrix M according to equation (6)
void JohnsonCalculator::createM(){
    M =  Mat::zeros(4,3 , CV_32F);
    for(int i = 0; i< 4; i++){
        M.at<float>(i,0) = normals.at(i).x ;
        M.at<float>(i,1) = normals.at(i).y ;
        M.at<float>(i,2) = 1.f ;
        //printf("\n Zeile %i von M: %f, %f, %f", i, M.at<float>(i,0), M.at<float>(i,1),  M.at<float>(i,2) );
    }
}

int JohnsonCalculator::getNrOfPatches(){
    int nr;
    normalsUsed = normals.size();
    while(normalsUsed%patchSize !=0){
        normalsUsed--;
    }
    nr= normalsUsed/patchSize;
    return nr;
}

//Creates Matrix M according to equation (8)
void JohnsonCalculator::createMUsingPatches(){
    int ncols= (getNrOfPatches())*2+1;
    int i=0;
    int r = 0;

    Mp =  Mat::zeros(normalsUsed, ncols , CV_32F);

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
    //    printf("\n");
    //    printf("\n Matrix M:");
    //    printf("\n");
    //    for(int r = 0; r< Mp.size().height; r++){
    //        for(int c = 0; c< Mp.size().width; c++){
    //            printf(" %f,  ", Mp.at<float>(r,c) );
    //        }
    //        printf(" \n \n" );
    //    }
}




void JohnsonCalculator::calculateIntensity(const int distance, vector<Point> gss, Mat img){
    I =  Mat::zeros(4,1 , CV_32F);
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
        if(counter==4)break;
    }
}



void JohnsonCalculator::calculateIntensityUsingPatches(const int distance, vector<Point> gss, Mat img){
    Ip =  Mat::zeros(normalsUsed,1 , CV_32F);
    int  counter=0;
    for(int i = 0; i < gss.size()-distance; i+=distance){

        Point p = gss.at(i);
        Vec3b intensity = img.at<Vec3b>(p.y, p.x);
        uchar blue = intensity.val[0];
        uchar green = intensity.val[1];
        uchar red = intensity.val[2];
        float val = 0.299*red + 0.587*green + 0.114*blue;
        Ip.at<float>(counter, 0) = val;
        counter ++;
        if(counter==normalsUsed)break;
    }

    //    for (int i = 0; i <Ip.size().height; ++i) {
    //        printf("\n %f",  Ip.at<float>(i, 0));
    //    }
}



void JohnsonCalculator::setNormalVecs(const int distance, vector<Point> gss, Mat maskImage )
{
    for(int i = 0; i < gss.size()-distance; i+=distance){
        Point startPos = gss.at(i);
        Point endPos = gss.at(i+distance);
        float dx = endPos.x - startPos.x;
        float dy = endPos.y - startPos.y;
        float vLenght = dx*dx + dy*dy;
        dx/=vLenght;
        dy/=vLenght;
        dx*=50.;
        dy*=50.;
        // Point2f normalOne = Point2f(dy,-dx);
        Point2f normalAsPoint = Point2f(-dy,dx);
        normals.push_back(normalAsPoint);

    }
    createM();
    createMUsingPatches();
    createC();
}

vector<Point2f> JohnsonCalculator::getNormals() const
{
    return normals;
}

void JohnsonCalculator::clearAll(){
    normals.clear();
    M = Mat::zeros(0,0,CV_32F);
    I= Mat::zeros(0,0,CV_32F);
    Mp= Mat::zeros(0,0,CV_32F);
    C= Mat::zeros(0,0,CV_32F);
    Ip= Mat::zeros(0,0,CV_32F);
    lightvector = Point();
    lightvectorsUsingPatches.clear();
    normalsUsed = 0;
}

void JohnsonCalculator::createC(){
    C =  Mat::zeros(2*getNrOfPatches()-2, 2*getNrOfPatches()+1,  CV_32F);

    int r=0;
    for(int c = 0; c< C.size().width-1; c++){
        C.at<float>(r,c) = -1.0f ;
        C.at<float>(r,c+2) = 1.0f ;
        r++;
    }

    //          printf("\n");
    //          printf("\n Matrix C:");
    //          printf("\n");
    //          for(int r = 0; r< C.size().height; r++){
    //              for(int c = 0; c< C.size().width; c++){
    //              printf(" %f,  ", C.at<float>(r,c) );
    //              }
    //              printf(" \n \n" );
    //          }
}



