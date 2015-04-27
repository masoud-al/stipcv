#include "pcheader.h"

#include "stbuffer.h"

STBuffer::STBuffer(void):Buffer(NULL),BufferSize(0),Width(0),Height(0)
{			
}

STBuffer::STBuffer(int size):Buffer(NULL),Width(0),Height(0)
{
	Init(size);
}

STBuffer::~STBuffer(void)
{
	/*IplImage *tmp;
	for (  int i=0;i<(int)Data.size();i++)
		if(Data[i])
		{
			tmp=Data[i];
			cvReleaseImage(&tmp);
		}*/
	if(Buffer)
		cvReleaseMat(&Buffer);

}

void STBuffer::Init(const int size)
{
	BufferSize=size;
	//Data.reserve(BufferSize);
}

/*IplImage* STBuffer::GetFrame(int istamp)
{
	int i=FrameIndices.find(istamp);
	if(i>=0)
		return Data[i];
	else
		return NULL;
}*/


int STBuffer::GetSingleFrame(int i,IplImage* dst)
{
	if(i<0 || i>=BufferSize)
		return -1;
	assert(dst->widthStep * dst->height == Buffer->step);
	memcpy((void*)dst->imageData,Buffer->data.ptr + Buffer->step*i ,Buffer->step);
	/*CvMat *r,rh;
	r=cvGetRow( Buffer , &rh, i ) ;*/
	return 0;
		
}
int STBuffer::GetFrame(int istamp,IplImage* dst)
{
	int i=FrameIndices.find(istamp);
	return GetSingleFrame(i,dst);
}

void STBuffer::Update(IplImage* newframe)
{
	if(!Buffer) //if (!Data.size())
	{
		FrameIndices.Init(BufferSize);
		/*for (  int i=0;i<BufferSize;i++)
   			Data.push_back(cvCloneImage(newframe)); */
		Width=newframe->width;Height=newframe->height;
		Buffer=cvCreateMat(BufferSize, Width*Height,DATATYPE);
	}
	else
	{
		int k=FrameIndices.Add();
		/*IplImage* pIm=Data[k];
		cvCopy(newframe,pIm);*/
		assert(newframe->widthStep * newframe->height == Buffer->step);
		memcpy((void*)(Buffer->data.ptr + Buffer->step*k) ,
						newframe->imageData,Buffer->step);
	}

}

void STBuffer::Update(IplImage* newframe,   int istamp)
{
	if(!Buffer) //if (!Data.size())
	{
		FrameIndices.Init(BufferSize, istamp);
		/*for (  int i=0;i<BufferSize;i++)
   			Data.push_back(cvCloneImage(newframe)); */
		Width=newframe->width;Height=newframe->height;
		Buffer=cvCreateMat(BufferSize, Width*Height,DATATYPE);
	}
	else
	{
		int k=FrameIndices.Add(istamp);
		/*IplImage* pIm=Data[k];
		cvCopy(newframe,pIm);*/
		//M(i,j) = ((float*)(mat->data.ptr + mat->step*i))[j]
		//cvReduce cvReshape(0,0,0);
		assert(newframe->widthStep * newframe->height == Buffer->step);
		memcpy((void*)(Buffer->data.ptr + Buffer->step*k) ,
						newframe->imageData,Buffer->step);
	}
}


/*
mask should be a symmetric filter and its size should be an odd number
*/
int STBuffer::TemporalConvolve(IplImage* dst,std::vector<double> mask)
{
	int	tfsz=(int)mask.size();
	assert(tfsz<=BufferSize);

	int i;


	if(BufferSize)
	{
		/*IplImage* f1=(IplImage*)Data[0];
		assert(f1->width==width);
		assert(f1->height==height);*/
		assert(dst->widthStep * dst->height == Buffer->step);
		assert(BufferSize>3);
	}

	assert(tfsz%2); //the size of filter should be odd
	
	int tstampres=FrameIndices.Middle(tfsz);
	
	
	//int step = dst->widthStep/sizeof(float);  //prb:float assumption (IPL_DEPTH_32F)
	//int n=Width*Height;
	//todo: use OpenCV or IPL function for benefiting from SIMD instructions


	if((int)mask.size()<BufferSize)
		for(i=(int)mask.size();i<BufferSize;i++)
			mask.push_back(0);

	/*if(deriv)
	{
		int k0=Sorted[0];int k2=Sorted[2];
		float *D0 = (float*)((IplImage*)Data[k0])->imageData;
		float *D2 = (float*)((IplImage*)Data[k2])->imageData;
		float *Ds=(float*)dst->imageData;
		for(i=0;i<n;i++)
			//dst->imageData[i] = ( ((IplImage*)Data[k2])->imageData[i] - ((IplImage*)Data[k0])->imageData[i] ) / 2.0f; 
			Ds[i] = (D2[i]-D0[i])/2.0f;
		
	}*/



	std::vector<int> Sorted =FrameIndices.GetSortedIndices();

	//return tstampres;

	CvMat *fil=cvCreateMat(1,BufferSize,DATATYPE);
	assert(BufferSize==(int)mask.size()); //filter is too big (it could be cut)
	float* filter=new float[BufferSize];
	/*for(k=0;k<tfsz;k++)
		//((float*)(mask->data.ptr + mat->step*i))[j]
		//CV_MAT_ELEM(mask,float,0,k)
		filter[k]=((float*)(mask->data.ptr))[k];*/
	for(i=0;i<BufferSize;i++)
		//filter[i]=((float*)(mask->data.ptr))[i];
		//filter[Sorted[i]]=((float*)(mask->data.ptr))[i];
		filter[Sorted[i]]=(float)mask[i];

	for(i=0;i<BufferSize;i++)
		//CV_MAT_ELEM(fil,float,0,i)=filter[i];
		cvmSet(fil,0,i, filter[i]);

	/*float *Ds=(float*)dst->imageData;
	for(i=0;i<n;i++)
	{
		//I(x,y) ~ ((float*)(img->imageData + img->widthStep*y))[x]

		Ds[i]=0;
		for(k=0;k<tfsz;k++)
		{
			float *D = (float*)((IplImage*)Data[k])->imageData;
			Ds[i] += filter[k] *  D[i];
			//todo : correct casting
			//dst->imageData[4*i] +=filter[k] *  d[i];
		}
	}*/

	
	CvMat *rdst, dsthdr;
	rdst = cvReshape(dst,&dsthdr,0,1);
	cvMatMul(fil,Buffer,rdst);
	
	//memcpy((void*)dst->imageData, res->data.ptr, Buffer->step);//costly

	delete[] filter;
	

	return tstampres;
}

void STBuffer::FindLocalMaxima(InterestPointList& pts)
{
	int cols=Width;
	int rows=Height;
	int cc=rows*cols;
	float *D = (float*)Buffer->data.ptr; //prb: if datatype changes 
	//M(k,u) = D+k*cc+u
	//T(i,j) =  i*cols+j
#define MM(k,i,j)  *(D + (k)*(cc) + (i)*(cols)+(j) )

	int i,j,k,s,l,n;
	float v;

	
	//3x3x3 mask
	const int ns=27;
	int neighbs[ns][3];
	s=0;
	for(k=-1;k<=1;k++)
		for(i=-1;i<=1;i++)
			for(j=-1;j<=1;j++)
			{
				neighbs[s][0]=k;
				neighbs[s][1]=i;
				neighbs[s][2]=j;
				s++;
			}

	 //8x2 mask
	/*const int ns=11;
	int neighbs[ns][3];
	s=0;
	for(i=-1;i<=1;i++)
		for(j=-1;j<=1;j++)
		{
			neighbs[s][0]=0;
			neighbs[s][1]=i;
			neighbs[s][2]=j;
			s++;
		}
	neighbs[s][0]=1;neighbs[s][1]=0;neighbs[s++][2]=0;
	neighbs[s][0]=-1;neighbs[s][1]=0;neighbs[s++][2]=0;*/
	
	std::vector<int> Sorted =FrameIndices.GetSortedIndices();
	int *SI=new int[Sorted.size()];
	for(s=0;s<(int)Sorted.size();s++) SI[s]=Sorted[s];

	l=0;InterestPoint pt;
	for(k=1;k<BufferSize-1;k++)
		for(i=1;i<rows-1;i++)
			for(j=1;j<cols-1;j++)
			{
				s=0;
				v=MM(SI[k],i,j);
				for(n=0;n<ns;n++)
					if ( v > MM(SI[neighbs[n][0]+k], neighbs[n][1]+i, neighbs[n][2]+j) )
						s++;
				if(s==ns-1)
				{
					pt.x=j;pt.y=i;pt.t=FrameIndices.get(SI[k]);pt.val=v;
					pts.push_back(pt);
					l++;//add
				}
			}
	l++;
	//std::cout<<l<<std::endl;
	delete[] SI;
}


