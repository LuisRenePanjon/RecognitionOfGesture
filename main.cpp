#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <opencv2/features2d/features2d.hpp> // Librería que contiene funciones para detección de descriptores como SIFT

#include <opencv2/xfeatures2d/nonfree.hpp> // Librería que contiene funciones para detección de descriptores (que no son libres) como SURF
#include <ctime>
#include <sstream>
#include <vector>
#include <time.h>
#include <dirent.h>
#include <list>
#include <map>
#include<string>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;
void eventoTrack(int v, void* pP) {

}
double distancia(Point p1, Point p2){
    double dis = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
    return dis;
}

int main() {
	VideoCapture videoCapture(0);

	if (!videoCapture.isOpened()) {
		cout << "Can't find camera!" << endl;
		return -1;
	}

	Mat frame, frameOut, hsv, bw, ycrcb;
    int findBiggestContour(vector<vector<Point> >);

    Mat canny_output;
    vector<vector<Point> > contours;
    vector<vector<Point> > contornosFinal;
    vector<Vec4i> hierarchy;
    vector<Vec4i> jerarquia;
    Mat dilatado;
    int hMin = 0;
    int sMin = 0;
    int vMin = 0;
    int hMax = 0;
    int sMax = 0;
    int vMax = 0;

    int yMin = 0;
    int crMin = 0;
    int cbMin = 0;

    int yMax = 0;
    int crMax = 0;
    int cbMax = 0;

    Mat mano = Mat(Size(290, 480), CV_8UC3, Scalar(0, 0, 0));
		vector<vector<Point> > puntosContorno;
		while (true) {
			videoCapture >> frame;
        //Create the bar, with the maximum and minimum values ​​of hsv and Ycrcb
	        namedWindow("frame", WINDOW_AUTOSIZE);
	        createTrackbar("H-Min", "frame", &hMin, 180, eventoTrack, NULL);
	        createTrackbar("S-Min", "frame", &sMin, 255, eventoTrack, NULL);
	        createTrackbar("V-Min", "frame", &vMin, 255, eventoTrack, NULL);
	        createTrackbar("H-Max", "frame", &hMax, 180, eventoTrack, NULL);
	        createTrackbar("S-Max", "frame", &sMax, 255, eventoTrack, NULL);
	        createTrackbar("V-Max", "frame", &vMax, 255, eventoTrack, NULL);
	        createTrackbar("Y-Min", "frame", &yMin, 255, eventoTrack, NULL);
	        createTrackbar("Cr-Min", "frame", &crMin, 255, eventoTrack, NULL);
	        createTrackbar("Cb-Min", "frame", &cbMin, 255, eventoTrack, NULL);
	        createTrackbar("Y-Max", "frame", &yMax, 255, eventoTrack, NULL);
	        createTrackbar("Cr-Max", "frame", &crMax, 255, eventoTrack, NULL);
	        createTrackbar("Cb-Max", "frame", &cbMax, 255, eventoTrack, NULL);

          //Here you define the area of ​​interest
          Point p1(350,0), p2(350,480);
	        Scalar colorLine(0,255,0);
	        int thicknessline = 2;
	        line(frame, p1, p2, colorLine, thicknessline);
	        imshow("frame", frame);
	        for (int i = 0; i < frame.rows ; i++){
	        	for (int j = 350 ; j < frame.cols; j++){
	          	mano.at<Vec3b>(i,j-350) = frame.at<Vec3b>(i,j);
	          }
	        }

          //Apply a filter, in this case GaussianBlur, the size of kernel is 19*19
	        GaussianBlur( frame, frame, Size(19,19),0.0,0);

          //Change of space color to HSV and Ycrcv
	        cvtColor(mano, hsv, COLOR_BGR2HSV);
	        cvtColor(mano, ycrcb, COLOR_BGR2YCrCb);
	        inRange(hsv, Scalar(hMin, sMin, vMin), Scalar(hMax, sMax, vMax), bw);
	        inRange(ycrcb,Scalar(yMin, crMin, cbMin),Scalar(yMax, crMax, cbMax), ycrcb);
	        imshow("ycrcb", ycrcb);

          //Find a contour in area of interest
	        findContours( bw, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

          //Get Biggest Contour
					int s = findBiggestContour(contours);

	        Mat drawing = Mat::zeros( mano.size(), CV_8UC1 );
          //draw the contours and apply filter, in this case dilate for get better results.
	        drawContours( drawing, contours, s, Scalar(255), -1, 8, hierarchy, 0, Point() );
          dilate(drawing, drawing, getStructuringElement(MORPH_CROSS, Size(3, 3)));

          //manobgr is a final frame, where remove the background.
          Mat manobgr = Mat(Size(drawing.cols, drawing.rows), CV_8UC3, Scalar(0, 0, 0));
	        for (int i = 0; i < drawing.rows; i++){
	            for(int j = 0; j < drawing.cols; j++){
	                if(drawing.at<uchar>(i,j) ==255){
	                    manobgr.at<Vec3b>(i,j) = mano.at<Vec3b>(i,j);
	                }
	            }
	        }

          int tecla = waitKey(1);
          //When touch key r
          if(tecla==114){
              double resultado = 255.0;
              string imgPath = "";
	            vector<string> imagenes;
	            Mat imagenI = manobgr;
	            cvtColor(imagenI, imagenI, COLOR_BGR2GRAY);
	            resize(imagenI, imagenI, Size(200, 200));
	            Mat imagenJ;
	            Mat resta;

	            DIR *dir;
	            struct dirent *ent;
	            string directorio="../DataBase";
	            dir = opendir(directorio.c_str());
	            while ((ent = readdir (dir)) != NULL){
	                if ( (strcmp(ent->d_name, ".")!=0) && (strcmp(ent->d_name, "..")!=0)
									&& (strcmp(ent->d_name, "imagenes.directory")!=0)
									&& (strcmp(ent->d_name, ".directory")!=0) ){
	                    imagenes.push_back(directorio+"/"+ent->d_name);
	                }
	            }
              //First change the color space to gray, then do the subtraction of the images.
              for (int j = 0; j < imagenes.size(); j++){
	                imagenJ = imread(imagenes[j]);
	                cvtColor(imagenJ, imagenJ, COLOR_BGR2GRAY);
	                resize(imagenJ, imagenJ, Size(200, 200));
                  resta = abs(imagenI-imagenJ);
	                int sumatoria = 0;
	                int cantidadElementos = 200 * 200;
	                for (int y = 0; y < 200; y++) {
                    for (int x = 0; x < 200; x++) {
                      int elementoActual = resta.at<uchar>(y,x);
	                      sumatoria = sumatoria + elementoActual;
	                  }
	                }
	                double promedio = (float) sumatoria / (float) cantidadElementos;
                  if ( promedio < resultado){
                    imgPath = imagenes[j];
                    resultado = promedio;
                  }
	           }
             if (resultado < 80){
             cout << "identificado" << resultado << " "<< imgPath << endl;
             putText(manobgr , "Gesto Encontrado", Point (10,50),FONT_HERSHEY_SCRIPT_COMPLEX, 1, CV_RGB(125,12,145), 2);
            }else{
              cout << "No identificado" << resultado <<endl;
             putText(manobgr , "No se encuentra en la DB :C", Point (10,50),FONT_HERSHEY_SCRIPT_COMPLEX, 1, CV_RGB(125,12,145), 2);
            }
          //Save Images
	        }else if(tecla == 49){
	            cout << "Guardado" << endl;
	            imwrite("../DataBase/mano1.png",mano);
	        }else if(tecla == 50){   
            cout << "Guardado" << endl;
            imwrite("../DataBase/mano2.png",mano);
          }else if(tecla == 51){
            cout << "Guardado" << endl;
            imwrite("../DataBase/mano3.png",mano);
          }else if(tecla == 27){
	            break;

	        }else if(tecla == 116){ //Through the percentage of coincidence define which photo is the most similar. letter T
              double porcentaje = 0.0;
              string imgPath = "";
              Mat imgBase;
              vector<string> imagenes;
              DIR *dir;
	            struct dirent *ent;
	            string directorio="../DataBase";
	            dir = opendir(directorio.c_str());
	            while ((ent = readdir (dir)) != NULL){
	                if ( (strcmp(ent->d_name, ".")!=0) && (strcmp(ent->d_name, "..")!=0)
									&& (strcmp(ent->d_name, "imagenes.directory")!=0)
									&& (strcmp(ent->d_name, ".directory")!=0) ){
	                    imagenes.push_back(directorio+"/"+ent->d_name);
	                }
	            }
              Mat scoreImg;
              for(int i = 0; i < imagenes.size(); i++){
                imgBase = imread(imagenes[i]);
                double maxScore;
                matchTemplate(manobgr, imgBase, scoreImg, TM_CCOEFF_NORMED);
                minMaxLoc(scoreImg, 0, &maxScore);
                //cout << "score imagen" << imagenes[i] << " "<< maxScore *100 << endl;
                if((maxScore * 100) > porcentaje){
                  imgPath = imagenes[i];
                  porcentaje = maxScore *100;

                }
              }
              if (porcentaje > 25){
                cout << "Identificado, similar a: " << imgPath << "Porcentaje: " << porcentaje << endl;
                putText(manobgr , "Detectado", Point (10,50),FONT_HERSHEY_SCRIPT_COMPLEX, 2, CV_RGB(125,12,145), 2);
              }else {
                cout << "No detectado" << endl;
                putText(manobgr , "No Detectado", Point (10,50),FONT_HERSHEY_SCRIPT_COMPLEX, 2, CV_RGB(125,12,145), 2);
              }
	        }
	        imshow("Mano",mano);
	        imshow("manobgr", manobgr);
	        imshow("drw", drawing);
		}
	return 0;
}

int findBiggestContour(vector<vector<Point> > contours){
    int indexOfBiggestContour = -1;
    int sizeOfBiggestContour = 0;
    for (int i = 0; i < contours.size(); i++){
        if(contours[i].size() > sizeOfBiggestContour){
            sizeOfBiggestContour = contours[i].size();
            indexOfBiggestContour = i;
        }
    }
    return indexOfBiggestContour;
}
