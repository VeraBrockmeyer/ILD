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
    printf("\n \n Lightvector \n");
    for(int i = 0; i<v.size().height-2; i+=2){
        printf("%f " , v.at<float>(i,0));
        printf("%f \n " , v.at<float>(i+1,0));
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

    //    printf("\n");
    //    printf("\n Matrix TRANSPONIERT:");
    //    printf("\n");
    //    for(int r = 0; r< MpT.size().height; r++){
    //        for(int c = 0; c< MpT.size().width; c++){
    //        printf(" %f,  ", MpT.at<float>(r,c) );
    //        }
    //        printf(" \n" );
    //    }

    //    printf("\n");
    //    printf("\n Matrix Bracket:");
    //    printf("\n");
    //    for(int r = 0; r< bracket.size().height; r++){
    //        for(int c = 0; c< bracket.size().width; c++){
    //        printf(" %f,  ", bracket.at<float>(r,c) );
    //        }
    //        printf(" \n" );
    //    }

    //    printf("\n");
    //    printf("\n Matrix Bracket INVERSE:");
    //    printf("\n");
    //    for(int r = 0; r< bracketInvert.size().height; r++){
    //        for(int c = 0; c< bracketInvert.size().width; c++){
    //        printf(" %f,  ", bracketInvert.at<float>(r,c) );
    //        }
    //        printf(" \n" );
    //    }


    //    printf("\n");
    //    printf("\n B:");
    //    printf("\n");
    //    for(int r = 0; r< bvec.size().height; r++){
    //        //for(int c = 0; c< bvec.size().width; c++){
    //        printf(" %f,  ", bvec.at<float>(r,0));
    //       // }
    //        printf(" \n" );
    //    }




    vector<float> L;
    printf("\n \n Lightvectors \n");
    for(int i = 0; i<v.size().height-2; i+=2){
        L.push_back(v.at<float>(i,0));
        L.push_back(v.at<float>(i+1,0));
        printf("%f " , v.at<float>(i,0));
        printf("%f \n " , v.at<float>(i+1,0));
    }
    setLightvectorsUsingPatches(L);
}

//void JohnsonCalculator::calculateLightVectorUsingPatches(){
//    Mat v;
//    cv::solve(Mp,Ip,v, DECOMP_SVD );
//    vector<float> L;
//     printf("\n \n Lightvectors \n");
//     for(int i = 0; i<v.size().height-2; i+=2){
//         L.push_back(v.at<float>(i,0));
//         L.push_back(v.at<float>(i+1,0));
//         printf("%f " , v.at<float>(i,0));
//         printf("%f \n " , v.at<float>(i+1,0));
//     }
//     setLightvectorsUsingPatches(L);
//}

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


//  ( N0x, N0y, 1 )
//M=( N1x, N1y, 1 )
//  ( ........... )
void JohnsonCalculator::createM(){
    M =  Mat::zeros(4,3 , CV_32F);
    for(int i = 0; i< 4; i++){
        M.at<float>(i,0) = normals.at(i).x ;
        M.at<float>(i,1) = normals.at(i).y ;
        M.at<float>(i,2) = 1.f ;
        //printf("\n Zeile %i von M: %f, %f, %f", i, M.at<float>(i,0), M.at<float>(i,1),  M.at<float>(i,2) );
    }

    //imshow("Matrix M", M);
}

int JohnsonCalculator::getNrOfPatches(){
    int nr;
    normalsUsed = normals.size();
    while(normalsUsed%patchSize !=0){
        normalsUsed--;
    }
    nr= normalsUsed/patchSize;
    //printf("\n Laenge Normalen: %i genutzte Normalen:%i PatchNR:%i", normals.size(), normalsUsed, nr);
    return nr;
}


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
    printf("\n");
    printf("\n Matrix M:");
    printf("\n");
    for(int r = 0; r< Mp.size().height; r++){
        for(int c = 0; c< Mp.size().width; c++){
            printf(" %f,  ", Mp.at<float>(r,c) );
        }
        printf(" \n \n" );
    }
    printf("finished Mp" );
    //imshow("Matrix Mp", Mp);
}




void JohnsonCalculator::calculateIntensity(const int distance, vector<Point> gss, Mat img){
    I =  Mat::zeros(4,1 , CV_32F);
    //printf("Size of I: %i ; Size of normals: %i", I.size().height, normals.size());
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
        // printf("/n Intensity: %f",  I.at<float>(i, 0));
        if(counter==4)break;
    }
    //imshow("I",I);
}



void JohnsonCalculator::calculateIntensityUsingPatches(const int distance, vector<Point> gss, Mat img){
    Ip =  Mat::zeros(normalsUsed,1 , CV_32F);
    int  counter=0;
    printf("\n \n Intensitaet:");
    for(int i = 0; i < gss.size()-distance; i+=distance){

        Point p = gss.at(i);
        Vec3b intensity = img.at<Vec3b>(p.y, p.x);
        uchar blue = intensity.val[0];
        uchar green = intensity.val[1];
        uchar red = intensity.val[2];
        float val = 0.299*red + 0.587*green + 0.114*blue;
        Ip.at<float>(counter, 0) = val;
        printf("\n %f",  Ip.at<float>(counter, 0));
        counter ++;
        if(counter==normalsUsed)break;  // if(counter==normalsUsed-1)break;
    }
    printf("nach berechnung \n");
    for (int i = 0; i <Ip.size().height; ++i) {
        printf("\n %f",  Ip.at<float>(i, 0));
    }
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
        Point2f normalOne = Point2f(dy,-dx);
        Point2f normalAsPoint = Point2f(-dy,dx);
//        float endXOne = gss.at(i+distance/2).x - normalOne.x;
//        float endYOne = gss.at(i+distance/2).y - normalOne.y;
//        Vec3b intensity = maskImage.at<Vec3b>(endYOne,endXOne);
//        uchar blue = intensity.val[0];
//        uchar green = intensity.val[1];
//        uchar red = intensity.val[2];
//        float val = 0.299*red + 0.587*green + 0.114*blue;
//        printf("Intensity: %f \n",val);
//        if(val>3){
        normals.push_back(normalOne);
    //}
//        else { normals.push_back(normalOne);}
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



