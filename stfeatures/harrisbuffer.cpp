#include "pcheader.h"

#include "harrisbuffer.h"
#include "cvutil.h"

std::ofstream merd("merd.txt");
std::ofstream ipts("ipts.txt");


void LogMinMax(CvArr* mat,std::ostream& os)
{
	//cvNormalize(gray,frame,1,0,CV_MINMAX);
	double m, M;
	cvMinMaxLoc(mat, &m, &M, NULL, NULL, NULL);
	os<<m<<"\t"<<M<<std::endl;
}

HarrisBuffer::HarrisBuffer(void):kparam(5e-4),sig2(8.0),tau2(2.0),delay(0),SignificantPointThresh(1E-9)
{
	iFrame=0;

	/*SpatialMaskSeparable=NULL;
	SpatialMask=NULL;
	TemporalMask1=NULL;
	TemporalMask2=NULL;*/

	tmp=NULL;
	tmp1=NULL;
	tmp2=NULL;
	tmp3=NULL;
	tmp4=NULL;

	frame=NULL;

	cxx=NULL;
	cxy=NULL;
	cxt=NULL;
	cyy=NULL;
	cyt=NULL;
	ctt=NULL;
	L=Lt=Lx=Ly=NULL;
	vis=NULL;
}

HarrisBuffer::~HarrisBuffer(void)
{ 
/*	if(SpatialMaskSeparable) cvReleaseMat(&SpatialMaskSeparable);
	if(SpatialMask) cvReleaseMat(&SpatialMask);
	if(TemporalMask1) cvReleaseMat(&TemporalMask1);
	if(TemporalMask2) cvReleaseMat(&TemporalMask2);*/

	if(tmp)		cvReleaseImage(&tmp);
	if(tmp1)	cvReleaseImage(&tmp1);
	if(tmp2)	cvReleaseImage(&tmp2);
	if(tmp3)	cvReleaseImage(&tmp3);
	if(tmp4)	cvReleaseImage(&tmp4);

	if(frame)	cvReleaseImage(&frame);

	if(cxx)	cvReleaseImage(&cxx);
	if(cxy)	cvReleaseImage(&cxy);
	if(cxt)	cvReleaseImage(&cxt);
	if(cyy)	cvReleaseImage(&cyy);
	if(cyt)	cvReleaseImage(&cyt);
	if(ctt)	cvReleaseImage(&ctt);

	if(L)	cvReleaseImage(&L);
	if(Lt)	cvReleaseImage(&Lt);
	if(Lx)	cvReleaseImage(&Lx);
	if(Ly)	cvReleaseImage(&Ly);
	if(vis)	cvReleaseImage(&Ly);
}

void HarrisBuffer::Init(IplImage* firstfrm)
{
	SpatialMaskSeparable=CVUtil::GaussianMask1D(sig2);
	TemporalMask1=CVUtil::GaussianMask1D(tau2);
	TemporalMask2=CVUtil::GaussianMask1D(2*tau2);
	DerivMask.push_back(-0.5);DerivMask.push_back(0.0);DerivMask.push_back(0.5);

	/*int sz1=TemporalMask1->width;
	int sz2=TemporalMask2->width;*/
	int sz1=(int)TemporalMask1.size();
	int sz2=(int)TemporalMask2.size();
	// estimate delay in point detection (in frames)
	if(!delay)
		delay= (int)((sz1+sz2)/2.0) +2;

	databuffer.Init(sz1);
	convbuffer.Init(sz2);
	cxxbuffer.Init(sz2);
	cxybuffer.Init(sz2);
	cxtbuffer.Init(sz2);
	cyybuffer.Init(sz2);
	cytbuffer.Init(sz2);
	cttbuffer.Init(sz2);	
	original.Init(sz2);//prb:? ??? sz1? sz2? delay?
	Hbuffer.Init(3);

	//header= cvCreateImageHeader(cvGetSize(aframe),IMGTYPE ,1);
	//tmp=cvCreateData(header);
	//todo:difference between cvMat and IplImage
	//		CvMat *work=cvCreateMat(frame->width,frame->height,CV_64FC1);
	tmp= cvCreateImage(cvGetSize(firstfrm),IMGTYPE ,1);
	tmp1= cvCreateImage(cvGetSize(firstfrm),IMGTYPE ,1);
	tmp2= cvCreateImage(cvGetSize(firstfrm),IMGTYPE ,1);
	tmp3= cvCreateImage(cvGetSize(firstfrm),IMGTYPE ,1);
	tmp4= cvCreateImage(cvGetSize(firstfrm),IMGTYPE ,1);

	frame = cvCreateImage(cvGetSize(firstfrm),IMGTYPE ,1);

	cxx= cvCreateImage(cvGetSize(firstfrm),IMGTYPE ,1);
	cxy= cvCreateImage(cvGetSize(firstfrm),IMGTYPE ,1);
	cxt= cvCreateImage(cvGetSize(firstfrm),IMGTYPE ,1);
	cyy= cvCreateImage(cvGetSize(firstfrm),IMGTYPE ,1);
	cyt= cvCreateImage(cvGetSize(firstfrm),IMGTYPE ,1);
	ctt= cvCreateImage(cvGetSize(firstfrm),IMGTYPE ,1);

	L  = cvCreateImage(cvGetSize(firstfrm),IMGTYPE ,1);
	Lt = cvCreateImage(cvGetSize(firstfrm),IMGTYPE ,1);
	Lx = cvCreateImage(cvGetSize(firstfrm),IMGTYPE ,1);
	Ly = cvCreateImage(cvGetSize(firstfrm),IMGTYPE ,1);

	vis = cvCreateImage(cvGetSize(firstfrm),IMGTYPE ,1);
}



void HarrisBuffer::ProcessFrame(IplImage* frm)
{
	gray=frm;
	//todo:scale depending on input type and IMGTYPE 
	//cvNormalize(gray,frame,1,0,CV_MINMAX);
	/*double m, M;
	cvMinMaxLoc(gray, &m, &M, NULL, NULL, NULL);*/
	cvScale(gray, frame, 1.0/255.0, 0.0);// 
	/*double m, M;
	cvMinMaxLoc(frame, &m, &M, NULL, NULL, NULL); 
	std::cout<<m<<"\t"<<M<<endl;*/

	
	original.Update(frame);

	//spatial filtering
	CVUtil::GaussianSmooth(frame,tmp,sig2,FFT);
	databuffer.Update(tmp);

	//temporal filtering
	int tstamp1=databuffer.TemporalConvolve(tmp1, TemporalMask1);
	convbuffer.Update(tmp1,tstamp1);

	int tstamp1d=convbuffer.TemporalConvolve(Lt,DerivMask);
	cvScale(Lt,Lt, sqrt(tau2) , 0);

	convbuffer.GetFrame(tstamp1d,L);
	CVUtil::ImageGradient(L,Lx,Ly);//prb: a possible scale
	cvScale(Lx,Lx, sqrt(sig2)*0.5 , 0);
	cvScale(Ly,Ly, sqrt(sig2)*0.5 , 0);

	//update second-moment matrix
	GaussianSmoothingMul(Lx,Lx, tmp1,2*sig2);
	cxxbuffer.Update(tmp1,tstamp1d);
	GaussianSmoothingMul(Lx,Ly, tmp1,2*sig2);
	cxybuffer.Update(tmp1,tstamp1d);
	GaussianSmoothingMul(Lx,Lt, tmp1,2*sig2);
	cxtbuffer.Update(tmp1,tstamp1d);
	GaussianSmoothingMul(Ly,Ly, tmp1,2*sig2);
	cyybuffer.Update(tmp1,tstamp1d);
	GaussianSmoothingMul(Ly,Lt, tmp1,2*sig2);
	cytbuffer.Update(tmp1,tstamp1d);
	GaussianSmoothingMul(Lt,Lt, tmp1,2*sig2);
	cttbuffer.Update(tmp1,tstamp1d);

	//update Harris buffer

	int tstamp2=0;
	tstamp2=cxxbuffer.TemporalConvolve(cxx, TemporalMask2);
	tstamp2=cxybuffer.TemporalConvolve(cxy, TemporalMask2);
	tstamp2=cxtbuffer.TemporalConvolve(cxt, TemporalMask2);
	tstamp2=cyybuffer.TemporalConvolve(cyy, TemporalMask2);
	tstamp2=cytbuffer.TemporalConvolve(cyt, TemporalMask2);
	tstamp2=cttbuffer.TemporalConvolve(ctt, TemporalMask2);

	HarrisFunction(/*cxx, cxy, cxt, cyy, cyt, ctt, */kparam, tmp);//CVUtil::ShowRealImage("win1",tmp);
	Hbuffer.Update(tmp,tstamp2);
	
	LogMinMax(Hbuffer.Buffer,merd);
	//databuffer.FrameIndices.print(std::cout);
	//databuffer.FrameIndices.print(merd);
	//convbuffer.FrameIndices.print(merd);
	//Hbuffer.FrameIndices.print(merd);
	//std::cout<<iFrame<<std::endl;
	DetectInterestPoints();

	iFrame++;
	return;
}

void HarrisBuffer::GaussianSmoothingMul(IplImage* im1, IplImage* im2, IplImage* dst, double var)
{
	cvMul(im1,im2,tmp4);
	CVUtil::GaussianSmooth(tmp4,dst,var,FFT);
}

void HarrisBuffer::HarrisFunction(double k, IplImage* dst)
{
	// Harris function in 3D
	// original space-time Harris
	/*detC=  
		cxx.*cyy.*ctt +		xx yy tt
		cxy.*cyt.*cxt +		2 * xy yt xt
		cxt.*cxy.*cyt -		.
		cxx.*cyt.*cyt -		xx yt^2
		cxy.*cxy.*ctt -		tt xy^2	
		cxt.*cyy.*cxt ;		yy xt^2
*/
        cvMul(cxx, cyy, tmp1);
        cvMul(ctt, tmp1, tmp1);

        cvMul(cxy, cxt, tmp2);
        cvMul(cyt, tmp2, tmp2,2);

        cvAdd(tmp1,tmp2,tmp1);

        cvMul(cyt,cyt,tmp2);
        cvMul(cxx,tmp2,tmp2);

        cvSub(tmp1,tmp2,tmp1);

        cvMul(cxy,cxy,tmp2);
        cvMul(ctt,tmp2,tmp2);

        cvSub(tmp1,tmp2,tmp1);

        cvMul(cxt,cxt,tmp2);
        cvMul(cyy,tmp2,tmp2);

        cvSub(tmp1,tmp2,tmp1);

        //trace3C=(cxx+cyy+ctt).^3;
        cvAdd(cxx,cyy,tmp2);
        cvAdd(ctt,tmp2,tmp2);
        cvPow(tmp2,tmp2,3);

        //H=detC-stharrisbuffer.kparam*trace3C;
        cvScale(tmp2,tmp2,k,0);
        cvSub(tmp1,tmp2,dst);

	/*cvMul(cxx, cyy, tmp1);
	cvMul(ctt, tmp1, tmp2);

	cvMul(cxy, cxt, tmp1);
	cvMul(cyt, tmp1, tmp3,2);

	cvAdd(tmp2,tmp3,tmp4);

	cvMul(cyt,cyt,tmp1);
	cvMul(cxx,tmp1,tmp2);

	cvSub(tmp4,tmp2,tmp3);

	cvMul(cxy,cxy,tmp1);
	cvMul(ctt,tmp1,tmp2);

	cvSub(tmp3,tmp2,tmp4);

	cvMul(cxt,cxt,tmp1);
	cvMul(cyy,tmp1,tmp2);

	cvSub(tmp4,tmp2,tmp3);

	//(cxx+cyy+ctt).^3
	cvAdd(cxx,cyy,tmp1);
	cvAdd(ctt,tmp1,tmp2);
	cvPow(tmp2,tmp1,(int)3);
	
	//H=detC-stharrisbuffer.kparam*trace3C;
	cvScale(tmp1,tmp2,k,0);
	cvSub(tmp2,tmp3,dst);*/
}
int xxx=0;

IplImage* HarrisBuffer::getHBufferImage(int type)
{
	int r;
	//cvAbs(tmp3,tmp3);cvLog(tmp3,tmp3);
	if(type==0)
		r=original.GetFrame(iFrame-convbuffer.BufferSize,vis);
	else
		r=Hbuffer.GetFrame(iFrame-convbuffer.BufferSize,vis);

	if(r==-1)
		cvZero(vis);

	return vis;
}


void HarrisBuffer::DetectInterestPoints(int border)
{
	InterestPointList tmp;
	Hbuffer.FindLocalMaxima(tmp);
	ipList=tmp;

	ipts<<iFrame<<":\t----------------------------------------------"<<std::endl;
	for(int i=0;i<(int)ipList.size();i++)
	{
		if(ipList[i].val > SignificantPointThresh )
		{
			xxx++;
		}
		ipts <<"\t"<<ipList[i].x <<'\t'<<ipList[i].y<<'\t'<<ipList[i].t<<'\t'<<ipList[i].val<<std::endl;
	}
		merd<<"nf= "<<xxx<<std::endl;


	/*CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* seq = cvCreateSeq( CV_32FC3, 
							sizeof(CvSeq), 
							sizeof(int), 
							storage );
	int i;
	for( i = 0; i < 100; i++ )
	{
		int* added = (int*)cvSeqPush( seq, &i );
		printf( "%d is added\n", *added );
	}

	...
	//release memory storage in the end 
	cvReleaseMemStorage( &storage );*/

	//remove border
	if(border>0)
	{

	}

	//check tstamp for any possible error
}

void HarrisBuffer::DrawInterestPoints(IplImage* im)
{	
	if(ipList.size()>0)
		if(ipList[0].t!=iFrame-convbuffer.BufferSize)
			return;
	for(int i=0;i<(int)ipList.size();i++)
	{
	
		if(ipList[i].val > SignificantPointThresh )
			CVUtil::DrawCross(im,cvPoint(ipList[i].x,ipList[i].y));
	}
}

