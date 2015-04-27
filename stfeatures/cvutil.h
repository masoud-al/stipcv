
#ifndef CVUTIL_H
#define CVUTIL_H

#include "cv.h"

#define CV_COLOR_RED CV_RGB( 255,0, 0 )

enum SmoothingMethod { Separable, FFT};

class CVUtil
{
public:
	

	CVUtil(void);
	~CVUtil(void);
	static void ShowRealImage(char* win, IplImage* im);
	static void DrawCross(IplImage* im, CvPoint pt, int length=5);
	static int RGB2GRAY(IplImage* rgb, IplImage* gray);
	static void DrawCross(CvPoint*,int);
	static int GaussianSmooth(IplImage* src, IplImage* dst, CvArr* gker, SmoothingMethod method);
	static int GaussianSmooth(IplImage* src, IplImage* dst, double sigma2, SmoothingMethod method);
	static int ImageGradient(IplImage* src, IplImage* dX, IplImage* dY);


	static std::vector<double> GaussianMask1D(double variance, int masksize=0, int szfct=3);

	static void cvShowManyImages(char* title, int nArgs, ...) ;


};


#endif //CVUTIL_H
