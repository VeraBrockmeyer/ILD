#include "johnsoncalculator.h"


JohnsonCalculator::JohnsonCalculator()
{

}

JohnsonCalculator::~JohnsonCalculator(){

}

void JohnsonCalculator::calculateLightVector(){

   CvLevMarq solver;
   solver.init(3, normals.size(),  cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, DBL_EPSILON), false);

   const CvMat* param = 0;
         CvMat* jac = 0;
         CvMat* err = 0;
   bool proceed = solver.update(param, jac, err);

   //printf(" \n Solver Error Norm %f" , solver.errNorm);
   //printf("\n Groesse Param cols: %i und rows: %i", param->cols, param->rows);

   //hier für Lx und Ly das Ergebnis der Minimierung übergeben, dann wird LV in weiß ins Bild gemalt.
   int Lx = 2;
   int Ly = 3;
   Point L = Point(Lx,Ly);
   setLightVector(L);

}


    //  ( N0x, N0y, 1 )
    //M=( N1x, N1y, 1 )
    //  ( ........... )
    void JohnsonCalculator::createM(){
        M =  Mat::zeros(normals.size(),3 , CV_8U);
        for(int i = 0; i< normals.size(); i++){
        M.at<int>(i,0) = normals.at(i).x;
        M.at<int>(i,1) = normals.at(i).y;
        M.at<int>(i,2) = 1 ;
       // printf("\n Zeile %i von M: %i, %i, %i", i, M.at<int>(i,0), M.at<int>(i,1),  M.at<int>(i,2) );
        }

       //imshow("Matrix M", M);
    }

    //R ist Einheitszahl= 1
    //3 Unbekannte: L.x, L.y und A (int oder float?)
    //Ist die Frage ob wir die Intensity überhaupt berechnen müssen, weil wir ja eigentlich nur L rausbekommen wollen.
    void JohnsonCalculator::calculateIntensity(int R, vector<Point> N, vector<Point> L, int A){
        int NL;

        for (int i=0; i<L.size(); i++){
        NL = N.at(i).x * L.at(i).x;
        NL+= N.at(i).y * L.at(i).y;
        int I = R * NL +A;
        //printf("\n Die Intensitaet an Normale %i liegt bei: %i ",i, I);
    }
    }


    void JohnsonCalculator::clearNormals(){
     normals.clear();
    }


    void JohnsonCalculator::setNormalVecs(const int distance, vector<Point> gss )
    {
        for(int i = 0; i < gss.size()-distance; i+=distance){
            Point startPos = gss.at(i);
            Point endPos = gss.at(i+distance);
            int dx = endPos.x - startPos.x;
            int dy = endPos.y - startPos.y;

            //Point normalOne = Point(dy,-dx);
            Point normalAsPoint = Point(-dy,dx);
            normals.push_back(normalAsPoint);
       }
        //printf("\n Anzahl Normalen: %i" , normals.size());
        createM();

    }

    vector<Point> JohnsonCalculator::getNormals() const
    {
        return normals;
    }

     Point JohnsonCalculator::getLightVector() const{
         return lightvector;
     }

      void JohnsonCalculator::setLightVector(const Point lv){
          lightvector = lv;
      }


     void JohnsonCalculator::calculateIntensity(const int distance, vector<Point> gss, Mat img){
          I =  Mat::zeros(normals.size(),1 , CV_8U);
          printf("Size of I: %i ; Size of normals: %i", I.size().height, normals.size());
          for(int i = 0; i < gss.size()-distance; i+=distance){
              Point p = gss.at(i);
              Vec3b intensity = img.at<Vec3b>(p.y, p.x);
              uchar blue = intensity.val[0];
              uchar green = intensity.val[1];
              uchar red = intensity.val[2];
              uchar val = 0.299*red + 0.587*green + 0.114*blue;
              I.at<uchar>(i, 0) = val;
              //printf("Int Val %i", val);

           }
          //imshow("I",I);
      }
