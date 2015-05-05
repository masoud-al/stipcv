
#include "../cvcam/cvcam.h"
#include <cv.h>
#include <highgui.h>
#include <cstdlib>
#include <cstdio>

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
