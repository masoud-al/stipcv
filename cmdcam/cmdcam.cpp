// cmdCam.cpp : Definiert den Einsprungpunkt für die Konsolenanwendung.
//

#include <cvcam.h>
#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <stdio.h>

int x=0;
void grabframe(IplImage *frame)
{
	printf("%d\n",x++);
}



int main(int argc, char* argv[])
{
	printf("CamTest started ..\n");
	int cameras = cvcamGetCamerasCount();
	printf("Cameras detected: %d \n",cameras);

	int * out;
	int nselected = cvcamSelectCamera(&out);
	int cameraSelected = -1;

	if(nselected>0)	cameraSelected = out[0];

	

	if (cameraSelected > -1)
	{
		printf("The selected camera is camera number %d \n", cameraSelected);
		printf("Starting Camera %d \n",cameraSelected );
	// starting camera 1

	int h = 240;
	int w = 320;
	int t=0;
	cvcamSetProperty(cameraSelected,CVCAM_RNDWIDTH , &w);
	cvcamSetProperty(cameraSelected,CVCAM_RNDHEIGHT , &h);
	cvcamSetProperty(cameraSelected,CVCAM_PROP_ENABLE, &t);
	cvcamSetProperty(cameraSelected,CVCAM_PROP_RENDER, &t);
	//cvcamSetProperty(0,CVCAM_PROP_WINDOW, NULL);
	
	cvNamedWindow("cvcam", CV_WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle("cvcam");
	cvcamSetProperty(cameraSelected, CVCAM_PROP_WINDOW, &hWnd);
	cvMoveWindow("cvcam",112,0);
	cvResizeWindow("cvcam",320,240);
	cvcamSetProperty(cameraSelected, CVCAM_PROP_CALLBACK, grabframe);
	cvcamInit();
	cvcamStart();
	printf("It's working !!! \n");
	//Sleep(10000);
	//cvcamStop();
	cvWaitKey(0);
	cvcamExit();
	printf("Camera stopped. \n");

	}
	else printf("No Camera selected - terminating! \n");

  return 0;
}

/*

#define SIZEX 640
#define SIZEY 480

#define CASX 7
#define CASY 9

#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

int AI,AD,BI,BD;

int x,y;

float _AI = 0;		// Esquina Sup. Izda
float _AD = 0;		// Esquina Sup. Dcha
float _BI = 0;		// Esquina Inf. Izda
float _BD = 0;		// Esquina Inf. Dcha

float temp, ErrMin;

CvPoint3D32f* Posicion;

CvVect32f distortion32f;
CvMatr32f cameraMatrix32f;
CvVect32f transVects32f;
CvMatr32f rotMatrs32f;


CvPoint2D32f estimado[CASX-1][CASY-1];
CvPoint2D32f TT;

double BaseX, BaseX_Y, PasoX, PasoX_Y;
double BaseY, BaseY_X, PasoY, PasoY_X;

int numpoints = 48*4;            // number of inner points on the pattern
int numcorners;                // number of inner points found
int maxnumcorners = 100;       // max number of inner points to look for
int numimages = 4;            // number of images to process
int t = 0;
int i = 0;
IplImage* img = 0;             // initialize some image arrays
IplImage* img0 = 0;
IplImage* img1 = 0;
IplImage* greyimg = 0;
CvSize size= {SIZEX,SIZEY};        // set image size
CvMemStorage* storage = 0;     // allocate some storage for ??
CvPoint2D32f* corners;         // declare an array for the corner points
CvPoint* cornersplot;          // setup a variable to hold ??
CvPoint onecorner;            // setup a variable to hold the coordinates of one corner

FILE *cps;                     //file to write corner points

CvFont dfont;
float hscale = 0.3;
float vscale = 0.3;
float italicscale = 0.0f;
int thickness =1;


// initialize the names of the calibration images
char* names[] = { "C:\\WINDOWS\\Escritorio\\PFC\\Calibracion\\Calib\\Debug\\calib1.tif", "C:\\WINDOWS\\Escritorio\\PFC\\Calibracion\\Calib\\Debug\\calib2.tif", "C:\\WINDOWS\\Escritorio\\PFC\\Calibracion\\Calib\\Debug\\calib3.tif", "C:\\WINDOWS\\Escritorio\\PFC\\Calibracion\\Calib\\Debug\\calib4.tif"};

int main()
{

cps = fopen("pts.txt", "w");

	
// allocate memory for the array of inner points	
corners = (CvPoint2D32f *)malloc((numpoints + 1) * sizeof(CvPoint2D32f));	
Posicion = (CvPoint3D32f *)malloc((numpoints + 1) * sizeof(CvPoint3D32f));	

distortion32f = (CvVect32f)malloc(4 * sizeof(float));
cameraMatrix32f = (CvMatr32f)malloc(3 * 3 * sizeof(float));
transVects32f = (CvVect32f)malloc((numimages+1) * 3 * 3 * sizeof(float));
rotMatrs32f = (CvMatr32f)malloc((numimages+1) * 3 * 3 * sizeof(float));


cvvNamedWindow( "image", 1 );  // create window with the name "image"

// initalize font for drawing numbers on images
cvInitFont (&dfont, CV_FONT_VECTOR0, hscale, vscale, italicscale, thickness);

// loop through the images
for ( i = 0; i < numimages; i++ )
{
	numcorners = maxnumcorners;   // reset numcorners to the max number of inner points to look for
				      // you must do this since cvFindChessBoardCornerGuesses set numcorners
				      // to the actual number found during each pass
				      // 
	img = cvvLoadImage(names[i]); // load next image
	img0 = cvCloneImage(img);     // make copy of the image
	img1 = cvCloneImage(img);     // make another copy of the image
	greyimg = cvCreateImageHeader(size,IPL_DEPTH_8U,1); 
	cvCreateImageData(greyimg);

	cvCvtColor(img, greyimg, CV_RGB2GRAY); // convery color image to grey
	img0 = cvCloneImage(greyimg); 

	
	// Find Corners
	cvFindChessBoardCornerGuesses(greyimg, img0, storage, cvSize(9,7),&corners[i*48], &numcorners);
	
	// Find sub-corners
	cvFindCornerSubPix(greyimg, &corners[i*48], numcorners, cvSize(9,7), cvSize(-1, -1), cvTermCriteria
			(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 0.1));

	for( t = 0; t < numcorners; t++ ) // print corner coordinates
        {
        	printf("Point: %i =   ", t);
        	printf("x: %f  ", corners[i*48+t].x);
        	printf("y: %f\n", corners[i*48+t].y);
		}
		
	printf("I FOUND:  %d\n", numcorners);


	// *** Order corners *** //
	
	// 1: Hallar esquinas principales (Falla en caso de tabla en 45?)

	for (t = 0; t < numcorners; t++ )
	{
		if (_BD < (temp = corners[i*48+t].y + corners[i*48+t].x))
		{
			BD = t;
			_BD = temp;
		}
		if (_BI < (temp = corners[i*48+t].y + (SIZEY-corners[i*48+t].x)))
		{
			BI = t;
			_BI = temp;
		}
		if (_AD < (temp = (SIZEX-corners[i*48+t].y) + corners[i*48+t].x))
		{
			AD = t;
			_AD = temp;
		}
		if (_AI < (temp = (SIZEX-corners[i*48+t].y) + (SIZEY-corners[i*48+t].x)))
		{
			AI = t;
			_AI = temp;
		}
	}

	// 2: Estimar el patr?n entre las esquinas:

	BaseX = corners[i*48+AI].x;
	BaseY = corners[i*48+AI].y;

	BaseX_Y = (corners[i*48+BI].x - corners[i*48+AI].x) / (CASY-2);
	BaseY_X = (corners[i*48+AD].y - corners[i*48+AI].y) / (CASX-2);

	PasoX = (corners[i*48+AD].x - corners[i*48+AI].x) / (CASX-2);
	PasoY = (corners[i*48+BI].y - corners[i*48+AI].y) / (CASY-2);

	PasoX_Y = (((corners[i*48+BD].x - corners[i*48+BI].x) / (CASX-2)) - PasoX)/(CASY-2);
	PasoY_X = (((corners[i*48+BD].y - corners[i*48+AD].y) / (CASY-2)) - PasoY)/(CASX-2);

	for (x = 0; x < CASX-1; x++)
		for (y = 0; y < CASY-1; y++)
		{
			estimado[x][y].x = BaseX + BaseX_Y*y + PasoX*x + PasoX_Y*x*y;
			estimado[x][y].y = BaseY + BaseY_X*x + PasoY*y + PasoY_X*x*y;
		}
			

	// 3: Asignar el punto equivalente a toda esquina.
	//    (Estimaci?n con m?nimo error cuadr?tico)

	for (t = 0; t < numcorners; t++ )
	{	
		Posicion[i*48+t].x = 0;
		Posicion[i*48+t].y = 0;
		Posicion[i*48+t].z = 0;
		ErrMin = 100000;
		for (x = 0; x < CASX-1; x++)
			for (y = 0; y < CASY-1; y++)
			{	
				temp = fabs(estimado[x][y].x - corners[i*48+t].x) + fabs(estimado[x][y].y - corners[i*48+t].y);

				if (temp < ErrMin)
				{
					ErrMin = temp;
					Posicion[i*48+t].x = x;
					Posicion[i*48+t].y = y;
				}
			}
	}


	/// *** Draw corners found and estimated corners *** ///

	for( t = 0; t < numcorners; t++ ) // draw a circle and put the corner number at each subcorner found
    {
        	onecorner.x = corners[i*48+t].x;
			onecorner.y = corners[i*48+t].y;
        	cvCircle(img1, onecorner, 3, CV_RGB(255,0,0),1);		
            //cvPutText(img1,numbers[PosY[i*48+t]], cvPoint(onecorner.x, onecorner.y + 20), &dfont, CV_RGB(255,0,0)); 
	}	
	
	for (x = 0; x < CASX-1; x++)
		for (y = 0; y < CASY-1; y++) // draw a circle and put the corner number at each subcorner found
		{
        	onecorner.x = estimado[x][y].x;
			onecorner.y = estimado[x][y].y;
        	cvCircle(img1, onecorner, 3, CV_RGB(0,0,255),1);		
		}

	cvvShowImage("image",img1);

	cvvWaitKey(0);  // wait for key press before displaying next image

	// release images
	cvReleaseImage( &img );
	cvReleaseImage( &img0 );
	cvReleaseImage( &img1 );
	cvReleaseImage( &greyimg );
	
	// Reinicializar variables
	_AI = 0;
	_AD = 0;
	_BI = 0;
	_BD = 0;
}


/// *** Once located and ordered all the corners, ///
///		calculate the calibration parameters ***  ///

cvCalibrateCamera( numimages, 48, size, corners, Posicion, 
				  distortion32f, cameraMatrix32f, transVects32f, rotMatrs32f,0);
				  
// clear memory storage - reset free space position
free (corners);
free (Posicion);
free (distortion32f);
free (cameraMatrix32f);
free (transVects32f);
free (rotMatrs32f);

fclose(cps);

return 0;
}

*/
