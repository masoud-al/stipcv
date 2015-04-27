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
//#define IMGTYPE		IPL_DEPTH_64F 

#define DATATYPE		CV_32FC1


struct InterestPoint
{
	int x,y,t;
	double val;
};

typedef std::vector<InterestPoint> InterestPointList;


//#include <hash_map>

//using namespace std;
//using namespace stdext;


/* this class provides a hashed vector i.e. the data of vector are hash keys 
so it can be used for constant operation using data instead of search */

/*
template <class TD>

class HashVector
{
	typedef pair <TD,   int> DataIntPair;
private:
	vector<TD> data;
	hash_map <TD,   int> keys;
	
	//not supported
	HashVector(const HashVector& hv);
	HashVector& operator=(const HashVector& x);

   
public:
	HashVector() {;}
	~HashVector() {;}
	HashVector(const   int sz) 
	{
		data.reserve(sz);
	}
	
	inline void push_back(TD x)
	{
		data.push_back(x);
		keys.insert(DataIntPair(x,(  int)data.size()-1));
	}

	TD& operator[](const   int index)   
	{
		return data[index];
	}

    TD& operator[](  int index) const;


	inline   int getIndex(const TD& k)
	{
		hash_map <TD,   int> :: const_iterator hditer;
		hditer = keys.find( k );
		return (hditer == keys.end())?-1:hditer -> second;
	}
	

};*/

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

	int mod(int x, int y)
	{
		return  x -  (int)floor(x/(double)y) * y;
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
	void FindLocalMaxima(InterestPointList& pts);

};

#endif //STBUFFER_H
