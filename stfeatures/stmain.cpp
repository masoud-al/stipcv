
#include "pcheader.h"

//#include <cctype.h>
#include <cstdio>
#include <cassert>

#include "highgui.h"
#include "cvutil.h"
#include "harrisbuffer.h"
#include "cmdline.h"

#define USE_CVCAM

#ifdef USE_CVCAM
#include "cvcam.h"
#endif 

CCmdLine cmdLine;
int xx=0;
bool show=true;
bool Processing=false;
IplImage* frame = 0;
HarrisBuffer hb;
CvCapture* capture = 0;  
char* win1="ST-Demo";
char* wincvcam="cvcam";
IplImage* vis  = NULL;
IplImage* vis2 = NULL;	
IplImage* vis3 = NULL;	
IplImage* camimg = NULL;	
IplImage* gray = NULL;	
double avg=0;
int ifr=0;

const int camresx[]={80,160,320,400,640};
const int camresy[]={60,120,240,300,480};	
int resid=1;


CvScalar random_color(CvRNG* rng)
{
    int icolor = cvRandInt(rng);
    return CV_RGB(icolor&255, (icolor>>8)&255, (icolor>>16)&255);
}


void ConvertRealImage(IplImage* im,IplImage* gray8u,IplImage* rgb8u)
{
	cvNormalize(im,im,1,0,CV_MINMAX); 
	cvScale(im,gray8u,255,0);
	cvCvtColor(gray8u,rgb8u,CV_GRAY2BGR);
}

void CapProperties( CvCapture* capture)
{
	//char* fourcc  = (char*) cvGetCaptureProperty(capture, CV_CAP_PROP_FOURCC);
	int frameH    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	int frameW    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	double fps    =  cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	int numFrames = (int) cvGetCaptureProperty(capture,  CV_CAP_PROP_FRAME_COUNT);
	double curPos = cvGetCaptureProperty(capture,  CV_CAP_PROP_POS_MSEC);
	printf("%.1f  ",curPos);
	//printf("fourcc=%s",fourcc);
	printf("%d frames ",numFrames);
	printf("[%dx%d]", frameW, frameH); 
	printf(" fps=%.1f  ",fps);
	printf("\n");
 //CV_CAP_PROP_POS_AVI_RATIO 
// CV_CAP_PROP_POS_FRAMES
}


/*void drawst(IplImage* im)
{

		return;
		CvRNG   rng = cvRNG((unsigned)-1);
		int line_type = CV_AA; // change it to 8 to see non-antialiased graphics

		int width=im->width;
		int height=im->height;
		//int width3 = width*3, height3 = height*3;

		for (int i = 0; i< 2; i++)
		{
			CvPoint pt1;
			pt1.x=i*20+10;//cvRandInt(&rng) % width3 - width;
			pt1.y=i*20+10;//cvRandInt(&rng) % height3 - height;

			cvCircle( im, pt1, cvRandInt(&rng)%300, random_color(&rng),
					cvRandInt(&rng)%10-1, line_type, 0 );
	
		}
}*/


bool first=true;;

void dostuff(IplImage *frm)
{
	frame=frm;
	if(first)
	{	
		first=false;
		hb.Init(frm) ;
		gray = cvCreateImage(cvGetSize(frm), IPL_DEPTH_8U, 1);
	}

	CVUtil::RGB2GRAY(frm,gray);
	double t,ft;	
	t = (double)cvGetTickCount();Processing=true;
	hb.ProcessFrame(gray);
    t = (double)cvGetTickCount() - t;Processing=false;
	ifr++;
	ft=t/(cvGetTickFrequency()*1000.);
	avg=((ifr-1)* avg + ft)/ifr;
	printf( "%d: Avg Time:%.1f - Avg FPS:%.1f  \n ",ifr, avg, 1000/avg);
}

void dovisstuff()
{
	if(!vis)
	{
		vis = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
		vis2 = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
		vis3 = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
		//IplImage* realimg = cvCreateImage(cvGetSize(frame), IPL_DEPTH_64F, 1);
	}
			//cvScale(gray,realimg,1.0);
			cvCvtColor(gray,vis,CV_GRAY2BGR);
			//hb.DrawInterestPoints(vis);
			//cvShowImage(win1,vis);
			//ShowRealImage(win2, hb.getHBufferImage() );
			ConvertRealImage(hb.getHBufferImage(1),gray,vis2);
			hb.DrawInterestPoints(vis2);

			ConvertRealImage(hb.getHBufferImage(0),gray,vis3);
			hb.DrawInterestPoints(vis3);

			CVUtil::cvShowManyImages(win1,3,vis,vis2,vis3);
			//cvShowImage(win2, vis2 );
}
#ifdef USE_CVCAM
void grabframe(IplImage *frm)
{
	
	if(!Processing)
	{
		cvResize(frm,camimg);
		cvFlip( camimg, camimg,0);

		dostuff(camimg);
		if(show)
			dovisstuff();
	}
	else
	{
		printf("skiping frame %d\n",xx++);
	}

}

bool InitCVCAM(int c)
{
	printf("CamTest started ..\n");
	int cameras = cvcamGetCamerasCount();
	printf("Cameras detected: %d \n",cameras);

	if(c>=cameras)
		return false;
	int cameraSelected = -1;
	
	/*if(cameras>0)
		cameraSelected=0;*/
	if(c==-1)
	{
		int * out;
		int nselected = cvcamSelectCamera(&out);
		if(nselected>0)	cameraSelected = out[0];
	}
	else
		cameraSelected=c;

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
	printf("It's working !!! \n");
	//Sleep(10000);
	//cvcamStop();
	//cvcamExit();
	//printf("Camera stopped. \n");

	}
	else 
	{
		printf("No Camera selected - terminating! \n");
		return false;
	}

	camimg=cvCreateImage(cvSize(camresx[resid],camresy[resid]), IPL_DEPTH_8U, 3);
	

	cvNamedWindow("cvcam", CV_WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle(wincvcam);
	cvcamSetProperty(cameraSelected, CVCAM_PROP_WINDOW, &hWnd);
	cvMoveWindow(wincvcam,112,0);
	cvResizeWindow(wincvcam,320,240);
	cvcamSetProperty(cameraSelected, CVCAM_PROP_CALLBACK, grabframe);
	cvcamInit();
	cvcamStart();
	return true;
}
#else

bool InitCVCAM(int c)
{
	std::cout<<"the macro USE_CVCAM was not enabled in compile time,"<<std::endl;
	std::cout<<"can not use cvcam lib for accessing the camera,"<<std::endl;
	return false;
}
#endif //USE_CVCAM

bool InitCapture(const char* param)
{
    /*if( param!=NULL || (param==NULL && strlen(param) == 1 && isdigit(param[0])))
        capture = cvCaptureFromCAM( !param ? param[0] - '0' : CV_CAP_ANY );
	 
    else //if( argc == 2 )*/
        capture = cvCaptureFromFile( param ); 

    if( !capture )
    {
        fprintf(stderr,"Could not initialize capturing from %s...\n",param);
        return false;
    }
	return true;
}

void ShowHelp()
{
	printf("\n");
	printf("This program detects space-time interest point and extracts features.\n");
	printf("currently only Harris corner and JET features supported.\n");
	printf("\n");
	printf("Usage:\n");
	printf("\n");
	printf("Input/Output options:\n");
	printf("   -f   : input video file\n");
	printf("   -o   : file name for saving detected features\n");
	printf("   -cam : the camera number (starts from 0) \n");
	printf("   (if you don't specify any number, it shows a dialog for choosing a camera)\n");
	printf("   -res : camera resolution for processing \n");
	printf("     the following resolutions are available \n");
	printf("	  0 : 80  x 60 \n");			
	printf("	  1 : 160 x 120 (default) \n");			
	printf("	  2 : 320 x 240 \n");
	printf("	  3 : 400 x 300 \n");			
	printf("	  4 : 640 x 480 \n");			
	printf("\n");
	printf("Algorithm options:\n");
	printf("   -sigma : variance for space Gaussian smoothing (default=%.1f)\n",hb.sig2);
	printf("   -tau   : variance for space Gaussian smoothing (default=%.1f)\n",hb.tau2);
	printf("   -kparam: K parameter in Harris function (default=%.5f)\n",hb.kparam);
	printf("   -thresh: threshold for ommiting weak points (default=%.3e)\n",hb.SignificantPointThresh);
	//printf("\t-lmax\t: method for local maximum (default=%.4f)\n",hb.kparam);
	printf("\n");
	printf("Other options:\n");
	printf("   -h    : shows this message\n");
	printf("   -vis  : [yes|no] visulization stuffs (default=%s)\n",show?"yes":"no");
	

}
int main( int argc, char** argv )
{
/*using namespace std;
   using namespace stdext;

   hash_map <int, int> hm1;
   hash_map <int, int> :: const_iterator hm1_AcIter, hm1_RcIter;
   typedef pair <int, int> Int_Pair;

   hm1.insert ( Int_Pair ( 13, 0 ) );
   hm1.insert ( Int_Pair ( -12, 1 ) );
   hm1.insert ( Int_Pair ( 75, 2 ) );

   hm1_RcIter = hm1.find( -12 );
   cout << "The element of hash_map hm1 with a key of 2 is: "
        << hm1_RcIter -> second << "." << endl;*/

	/*HashVector<int> hv;
	hv.push_back(7);
	hv.push_back(-22);
	hv.push_back(70);
	hv.push_back(33);
	int& x=hv[1];
	hv[1]=5;
	cout<<hv[1]<<std::endl;
	cout<<hv[hv.getIndex(33)]<<std::endl;
	cout<<hv[hv.getIndex(5)]<<std::endl;*/

	//void cvConvertScale( const CvArr* src, CvArr* dst, double scale=1, double shift=0 );



	//return 0;

	//todo:call in X
	//cvInitSystem

	//wchar_t *CC=L"Salam";
	//wprintf(CC);
	
	bool cvcam=true;
	
	if (cmdLine.SplitLine(argc, argv) < 1)
	{
      // no switches were given on the command line, abort
      ShowHelp();
      exit(-1);
	}

	std::string infile,outfile,tmp;
	int cam=-1;
	try
	{
		if( cmdLine.HasSwitch("-h") || 
			cmdLine.HasSwitch("-help") ||
			cmdLine.HasSwitch("--help"))
		{
			ShowHelp();
			exit(0);
		}

		if( cmdLine.HasSwitch("-f") && cmdLine.HasSwitch("-cam") )
		{
			std::cout<<"You can't specify both file and camera as input."<<std::endl;
			ShowHelp();
			exit(-1);
		}

	

		if( !(cmdLine.HasSwitch("-f") || cmdLine.HasSwitch("-cam") ))
		{
			std::cout<<"no input..."<<std::endl;
			ShowHelp();
			exit(-1);
		}

		if(cmdLine.GetArgumentCount("-f")>0) 
			infile = cmdLine.GetArgument("-f", 0);
		if(cmdLine.GetArgumentCount("-o")>0) 
			outfile = cmdLine.GetArgument("-o", 0);
	
 
		if(cmdLine.HasSwitch("-cam"))
			if(cmdLine.GetArgumentCount("-cam")>0)
				cam =  atoi(cmdLine.GetArgument("-cam", 0).c_str());
			else
				cam = -1;

		if(cmdLine.GetArgumentCount("-res")>0) resid =  atoi(cmdLine.GetArgument( "-res", 0 ).c_str());
		if(resid<0 || resid>4) resid=1;

		if(cmdLine.GetArgumentCount("-vis")>0) show = cmdLine.GetArgument("-vis", 0)=="yes"?true:false;

		if(cmdLine.GetArgumentCount("-sigma")>0) hb.sig2 =  atof(cmdLine.GetArgument( "-sigma", 0 ).c_str());
		if(cmdLine.GetArgumentCount("-tau")>0) hb.tau2 =  atof(cmdLine.GetArgument( "-tau", 0 ).c_str());
		if(cmdLine.GetArgumentCount("-kparam")>0) hb.kparam =  atof(cmdLine.GetArgument( "-kparam", 0 ).c_str());
		if(cmdLine.GetArgumentCount("-thresh")>0) hb.SignificantPointThresh =  atof(cmdLine.GetArgument( "-thres", 0 ).c_str());
		
		
	}
	catch (...)
	{
		ShowHelp();
		exit(-1);
	}


	if(infile=="")  //prb:both can handle cam and file
	{
		if(!InitCVCAM(cam))
			return -2;

	}
	else 
	{
		cvcam=false;
		if(!InitCapture(infile.c_str()))
			return -2;
		
	}

		
        
	if(show)
	{
		cvNamedWindow( win1,  CV_WINDOW_AUTOSIZE  );
		//cvNamedWindow( win2, 0 );
	}



	if(capture)
	{
    for(;;)
    {
        frame = cvQueryFrame( capture );
        if( !frame )
            break;
		//CapProperties(capture);
		dostuff(frame);
		if(show)
		{
			dovisstuff();
			if(cvWaitKey(10) >= 0 )
				break;
		}  
    }
	}

#ifdef USE_CVCAM
	if(cvcam)
	{
		cvWaitKey(0);
		cvcamExit();
	}
#endif
	
	if(capture)
		cvReleaseCapture( &capture );
	

	if(show)
	{
		cvDestroyWindow(win1);
	}

	if(gray) cvReleaseImage(&gray);
	if(vis)  cvReleaseImage(&vis);
	if(vis2)	cvReleaseImage(&vis2);
	if(vis3)	cvReleaseImage(&vis3);
	if(camimg) cvReleaseImage(&camimg);
    return 0;
}
