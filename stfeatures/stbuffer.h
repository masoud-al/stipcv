#ifndef STBUFFER_H
#define STBUFFER_H

#include <vector>

#include "cv.h"


//#define IMGTYPE		IPL_DEPTH_8U 
//#define IMGTYPE		IPL_DEPTH_8S 
//#define IMGTYPE		IPL_DEPTH_16U 
//#define IMGTYPE		IPL_DEPTH_16S 
//#define IMGTYPE		IPL_DEPTH_32S 
#define IMGTYPE			IPL_DEPTH_32F
#define IMG_ELEM_TYPE		float
//#define IMGTYPE		IPL_DEPTH_64F


#define DATATYPE		CV_32FC1
#define mod(x, y)   (x) -  (int)floor((x)/(double)(y)) * (y)


class InterestPoint
{
public:
	bool reject;
	int x,y,t;
	double val;
	CvMat *features;
	InterestPoint():reject(true),x(0),y(0),t(0),val(0),features(NULL){;}
	~InterestPoint(){if(features) cvReleaseMat(&features);}
};

typedef std::vector<InterestPoint> InterestPointList;


class CircularIndex
{
public:
	std::vector<int> Indices;
	std::vector<int> SortedIndices;
	int LastIndex;	
	int Total;

	CircularIndex()
	{
	}
	CircularIndex(int sz)
	{
		Init(sz);
	}
	void Init(int sz,int k=0)
	{
		Indices.reserve(sz);
		SortedIndices.reserve(sz);
		for (int i=0;i<sz;i++)
		{
			Indices.push_back(k);
			SortedIndices.push_back(i);
		}
		LastIndex=0;
		Total=0;
	}

	int Add()
	{
		return Add(Indices[LastIndex]+1);
	}

	int Add(int k)
	{
		Total++;
		LastIndex = (LastIndex+1) % (int)Indices.size();
		Indices[LastIndex]=k;
		return LastIndex;
	}


	int Middle(int sz)
	{
		int mid = (int) ceil(sz/2.0)-1;
		int t = mod( LastIndex - mid , (int)Indices.size());
		return Indices[t];
	}

	int get(int i)
	{
		assert(i<(int)Indices.size());
		return Indices[i];
	}

	int find(int q)
	{
		int i;
		for (i=0;i<(int)Indices.size();i++)
			if( Indices[i]==q)
				return i;
		return -1;
	}

	std::vector<int>& GetSortedIndices()
	{
		//increasing
		for (int i=0;i<(int)Indices.size();i++)
			SortedIndices[(i+Total+1)%Indices.size()]=i;
		//decreasing
		for (int i=0;i<(int)Indices.size();i++)
			SortedIndices[i]=abs((int)Indices.size()-SortedIndices[i]-1);
		return SortedIndices;
	}

	void print(std::ostream& of)
	{
		of<<"D: ";
		for (int i=0;i<(int)Indices.size();i++)
			of<<Indices[i]<<"  ";
		of<<std::endl;
		of<<"S: ";
		GetSortedIndices();
		for (int i=0;i<(int)Indices.size();i++)
			of<<SortedIndices[i]<<"  ";
		of<<std::endl;
	}

	/*string print()
	{
		string r="";
		for (int i=0;i<Indices.size();i++)
		{
			r+=Indices[i]+"  ";
			//SortedIndices.push_back(i);
		}
	}*/
};

class STBuffer
{
public:
	CvMat* Buffer;
	int BufferSize;
	int Width,Height;
	CircularIndex FrameIndices;
	//std::vector<IplImage*> Data;
	
	// to be sure that compiler generated copy const is not used
	// These are not supported
    // and are not implemented!
    STBuffer(const STBuffer& x);
    STBuffer& operator=(const STBuffer& x);

private:
	/* Neighbourhood masks are arrays of size n by 3 where 
	 n is the number of neighbours in the mask and 
	 second index indicates  t,x,y respectively */
	 int Neighbs3x3x3[26][3]; //3^3 - 1
	 int Neighbs3x3p2[10][3]; // 8 (3^2-1) neighbours in the space  and just 2 neighbours in the time
	 int Neighbs5x5x5[124][3]; //5^3 - 1
	 int Neighbs5x5p4[28][3];// 24 (5^2-1) neighbours in the space  and just 4 neighbours in the time
	 void CreateLocalMasks();

public:
	STBuffer(void);
	STBuffer(  int);

	~STBuffer(void);

	void Init(int size);
	IplImage* GetFrame(int istamp);
	int GetFrame(int istamp,IplImage* dst);
	int GetSingleFrame(int istamp,IplImage* dst);
	void Update(IplImage*); 
	void Update(IplImage*,  int); 
	int TemporalConvolve(IplImage* dst,std::vector<double> mask);
	void FindLocalMaxima(InterestPointList& pts,bool full=false);
	void GetLocalRegion(int x,int y,int t, int nx,int ny,int nt,	CvMat* rm);
};






#endif //STBUFFER_H
