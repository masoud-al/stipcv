#include "pcheader.h"

#include "cvutil.h"


CVUtil::CVUtil(void)
{
}

CVUtil::~CVUtil(void)
{
}


void CVUtil::ShowRealImage(char* win, IplImage* im)
{
	//double m, M;
	//cvMinMaxLoc(im, &m, &M, NULL, NULL, NULL);
	cvNormalize(im,im,1,0,CV_MINMAX); 
	//cvMinMaxLoc(im, &m, &M, NULL, NULL, NULL);
    //cvScale(im, im, 1.0/(M-m), 1.0*(-m)/(M-m));
	cvShowImage(win, im );
}


void CVUtil::DrawCross(IplImage* im, CvPoint pt, int length)
{
	int linetype = 8;// CV_AA;
	int s=2; 
	cvLine( im, cvPoint(pt.x,pt.y+s), cvPoint(pt.x,pt.y+length), CV_COLOR_RED , 1,linetype, 0 );
	cvLine( im, cvPoint(pt.x+s,pt.y), cvPoint(pt.x+length,pt.y), CV_COLOR_RED , 1,linetype, 0 );
	cvLine( im, cvPoint(pt.x,pt.y-s), cvPoint(pt.x,pt.y-length), CV_COLOR_RED , 1,linetype, 0 );
	cvLine( im, cvPoint(pt.x-s,pt.y), cvPoint(pt.x-length,pt.y), CV_COLOR_RED , 1,linetype, 0 );
}

std::vector<double> CVUtil::GaussianMask1D(double variance, int masksize, int szfct)
{
	const double MINVAL=1E-5;
	int sz=masksize/2;
	if (!masksize)
		sz=(int)(sqrt(variance)*szfct);
	double *tmp=new double[2*sz+1];	
	double sum=0;
	int x;
	for(x=-sz ; x<=sz ; x++)
		sum+=tmp[x+sz]=exp(-x*x/(2*variance));
	for(x=-sz ; x<=sz && sum ; x++)
		tmp[x+sz]/=sum;
	int nnz=0;
	for(x=1; x<=sz ; x++)
		if(tmp[x]>MINVAL)
			nnz++;
	std::vector<double> mask;
	for(int i=0;i<2*nnz+1;i++)
		mask.push_back(0);
	for(x=-nnz; x<=nnz ; x++)
		mask[x+nnz]=tmp[x+nnz];	

	delete[] tmp;
	return mask;
}

int CVUtil::RGB2GRAY(IplImage* rgb, IplImage* gray)
{
	if(!rgb)
		return -1;
	if(!gray)
		return -1;
	if(rgb->nChannels<3)
		return -1;
	//todo: CV_BGR2GRAY or CV_RGB2GRAY
	cvCvtColor(rgb,gray,CV_BGR2GRAY);
	return 0;
}


void CVUtil::DrawCross(CvPoint* pt,int sz)
{
}

int CVUtil::GaussianSmooth(IplImage* src, IplImage* dst, double sigma2, SmoothingMethod method)
{
	//automatically set kernel size
	cvSmooth(src, dst, CV_GAUSSIAN, 0, 0, sqrt(sigma2));

	//3x3 7x7 kernels  are more efficient
	//cvSmooth(src, dst, CV_GAUSSIAN, 7, 7, sqrt(sigma2));


	return 0;
}

int CVUtil::GaussianSmooth(IplImage* src, IplImage* dst, CvArr* gker, SmoothingMethod method)
{
	cvSmooth(src, dst, CV_GAUSSIAN, 0, 0, 2.0); 
	//cvSmooth( src, dst, CV_BLUR, 15, 15, 0, 0 );

   /*//Computing 2D Convolution using DFT

   CvMat* A = cvCreateMat( M1, N1, CV_32F );
   CvMat* B = cvCreateMat( M2, N2, A->type );

   // it is also possible to have only abs(M2-M1)+1×abs(N2-N1)+1
   // part of the full convolution result
   CvMat* conv = cvCreateMat( A->rows + B->rows - 1, A->cols + B->cols - 1, A->type );

   // initialize A and B
   ...

   int dft_M = cvGetOptimalDFTSize( A->rows + B->rows - 1 );
   int dft_N = cvGetOptimalDFTSize( A->cols + B->cols - 1 );

   CvMat* dft_A = cvCreateMat( dft_M, dft_N, A->type );
   CvMat* dft_B = cvCreateMat( dft_M, dft_N, B->type );
   CvMat tmp;

   // copy A to dft_A and pad dft_A with zeros
   cvGetSubRect( dft_A, &tmp, cvRect(0,0,A->cols,A->rows));
   cvCopy( A, &tmp );
   cvGetSubRect( dft_A, &tmp, cvRect(A->cols,0,dft_A->cols - A->cols,A->rows));
   cvZero( &tmp );
   // no need to pad bottom part of dft_A with zeros because of
   // use nonzero_rows parameter in cvDFT() call below

   cvDFT( dft_A, dft_A, CV_DXT_FORWARD, A->rows );

   // repeat the same with the second array
   cvGetSubRect( dft_B, &tmp, cvRect(0,0,B->cols,B->rows));
   cvCopy( B, &tmp );
   cvGetSubRect( dft_B, &tmp, cvRect(B->cols,0,dft_B->cols - B->cols,B->rows));
   cvZero( &tmp );
   // no need to pad bottom part of dft_B with zeros because of
   // use nonzero_rows parameter in cvDFT() call below

   cvDFT( dft_B, dft_B, CV_DXT_FORWBRD, B->rows );

   cvMulSpectrums( dft_A, dft_B, dft_A, 0 );// or CV_DXT_MUL_CONJ to get correlation rather than convolution 

   cvDFT( dft_A, dft_A, CV_DXT_INV_SCALE, conv->rows ); // calculate only the top part
   cvGetSubRect( dft_A, &tmp, cvRect(0,0,conv->cols,conv->rows) );

   cvCopy( &tmp, conv );*/

   /*if(src==dst)
	   cvReleaseImage(&dst2);*/

	return 0;
}



int CVUtil::ImageGradient(IplImage* src, IplImage* dX, IplImage* dY)
{
#if 0

#endif
	cvSobel( src, dX, 1, 0, 3 );
    cvSobel( src, dY, 0, 1, 3 );
	return 0;
}


/*Function///////////////////////////////////////////////////////////////

Name:       cvShowManyImages

Purpose:    This is a function illustrating how to display more than one 
               image in a single window using Intel OpenCV

Parameters: char *title: Title of the window to be displayed
            int nArgs:   Number of images to be displayed
            ...:         IplImage*, which contains the images

Language:   C++

The method used is to set the ROIs of a Single Big image and then resizing 
and copying the input images on to the Single Big Image.

This function does not stretch the image... 
It resizes the image without modifying the width/height ratio..

This function can be called like this:

cvShowManyImages("Images", 2, img1, img2);
or
cvShowManyImages("Images", 5, img2, img2, img3, img4, img5);

This function can display upto 12 images in a single window.
It does not check whether the arguments are of type IplImage* or not.
The maximum window size is 700 by 660 pixels.
Does not display anything if the number of arguments is less than
    one or greater than 12.

If you pass a pointer that is not IplImage*, Error will occur.
Take care of the number of arguments you pass, and the type of arguments, 
which should be of type IplImage* ONLY.

Idea was from BettySanchi of OpenCV Yahoo! Groups.

If you have trouble compiling and/or executing
this code, I would like to hear about it.

You could try posting on the OpenCV Yahoo! Groups
[url]http://groups.yahoo.com/group/OpenCV/messages/ [/url]


Parameswaran, 
Chennai, India.

cegparamesh[at]gmail[dot]com            

...
///////////////////////////////////////////////////////////////////////*/

void CVUtil::cvShowManyImages(char* title, int nArgs, ...) 
{

	if(nArgs==1)
	{
		va_list args;
		va_start(args, nArgs);
		cvShowImage(title,va_arg(args, IplImage*));
		return;
	}
    // img - Used for getting the arguments 
    IplImage *img;

    // DispImage - the image in which input images are to be copied
    IplImage *DispImage;

    int size;
    int i;
    int m, n;
    int x, y;

    // w - Maximum number of images in a row 
    // h - Maximum number of images in a column 
    int w, h;

    // scale - How much we have to resize the image
    float scale;
    int max;

    // If the number of arguments is lesser than 0 or greater than 12
    // return without displaying 
    if(nArgs <= 0) {
        printf("Number of arguments too small....\n");
        return;
    }
    else if(nArgs > 12) {
        printf("Number of arguments too large....\n");
        return;
    }
    // Determine the size of the image, 
    // and the number of rows/cols 
    // from number of arguments 
    else if (nArgs == 1) {
        w = h = 1;
        size = 300;
    }
    else if (nArgs == 2) {
        w = 2; h = 1;
        size = 300;
    }
    else if (nArgs == 3 || nArgs == 4) {
        w = 2; h = 2;
        size = 300;
    }
    else if (nArgs == 5 || nArgs == 6) {
        w = 3; h = 2;
        size = 200;
    }
    else if (nArgs == 7 || nArgs == 8) {
        w = 4; h = 2;
        size = 200;
    }
    else {
        w = 4; h = 3;
        size = 150;
    }

    // Create a new 3 channel image
    DispImage = cvCreateImage( cvSize(100 + size*w, 60 + size*h), 8, 3 );

    // Used to get the arguments passed
    va_list args;
    va_start(args, nArgs);

    // Loop for nArgs number of arguments
    for (i = 0, m = 20, n = 20; i < nArgs; i++, m += (20 + size)) {

        // Get the Pointer to the IplImage
        img = va_arg(args, IplImage*);

        // Check whether it is NULL or not
        // If it is NULL, release the image, and return
        if(img == 0) {
            printf("Invalid arguments");
            cvReleaseImage(&DispImage);
            return;
        }

        // Find the width and height of the image
        x = img->width;
        y = img->height;

        // Find whether height or width is greater in order to resize the image
        max = (x > y)? x: y;

        // Find the scaling factor to resize the image
        scale = (float) ( (float) max / size );

        // Used to Align the images
        if( i % w == 0 && m!= 20) {
            m = 20;
            n+= 20 + size;
        }

        // Set the image ROI to display the current image
        cvSetImageROI(DispImage, cvRect(m, n, (int)( x/scale ), (int)( y/scale )));

        // Resize the input image and copy the it to the Single Big Image
        cvResize(img, DispImage);

        // Reset the ROI in order to display the next image
        cvResetImageROI(DispImage);
    }

    // Create a new window, and show the Single Big Image
    //cvNamedWindow( title, 1 );
    cvShowImage( title, DispImage);

    //cvWaitKey();
   //cvDestroyWindow(title);

    // End the number of arguments
    va_end(args);

    // Release the Image Memory
    cvReleaseImage(&DispImage);
}

