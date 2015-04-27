#include "pcheader.h"

#include "harrisbuffer.h"
#include "cvutil.h"

//std::ofstream logfile("log.txt");


const int LengthFeatures=34;//length of feature vector
const int SizeNeighb=125; //mask of 5x5x5 (vectorized)
//JET filter computed in MATLAB
double jet[LengthFeatures][SizeNeighb]={
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.5,0,0,0,0,0,0,0,0,0,0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.5,0,0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,-2,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.25,0,-0.25,0,0,0,0,0,0,0,-0.25,0,0.25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.25,0,0,0,0,0,0,0,0,0,-0.25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.25,0,0,0,0,0,0,0,0,0,0.25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.25,0,-0.25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.25,0,0.25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.5,0,0,0,0,1,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.5,0,0.5,0,0,1,0,-1,0,0,-0.5,0,0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.5,1,-0.5,0,0,0,0,0,0,0,0.5,-1,0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.5,1,0,-1,0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.5,0,0,0,0,1,0,0,0,0,-0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.5,0,0,0,0,-1,0,0,0,0,0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.125,0,0.125,0,0,0,0,0,0,0,0.125,0,-0.125,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.125,0,-0.125,0,0,0,0,0,0,0,-0.125,0,0.125,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.5,1,-0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.5,-1,0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.5,0,0,0,0,0,0,0,0,0,0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.5,0,0,0,0,0,0,0,0,0,0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.5,0,0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.5,0,0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,-0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.5,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,-4,0,0,0,0,6,0,0,0,0,-4,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.25,0,-0.25,0,0,-0.5,0,0.5,0,0,0,0,0,0,0,0.5,0,-0.5,0,0,-0.25,0,0.25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-2,1,0,0,-2,4,-2,0,0,1,-2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.25,-0.5,0,0.5,-0.25,0,0,0,0,0,-0.25,0.5,0,-0.5,0.25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-4,6,-4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.25,0,0,0,0,-0.5,0,0,0,0,0,0,0,0,0,0.5,0,0,0,0,-0.25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.25,0,0,0,0,0.5,0,0,0,0,0,0,0,0,0,-0.5,0,0,0,0,0.25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.25,0,-0.25,0,0,-0.5,0,0.5,0,0,0.25,0,-0.25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.25,0,0.25,0,0,0.5,0,-0.5,0,0,-0.25,0,0.25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0625,0,-0.125,0,0.0625,0,0,0,0,0,-0.0625,0,0.125,0,-0.0625,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.0625,0,0.125,0,-0.0625,0,0,0,0,0,0.0625,0,-0.125,0,0.0625,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.25,-0.5,0,0.5,-0.25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.25,0.5,0,-0.5,0.25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,-2,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-2,0,0,0,0,4,0,0,0,0,-2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,-2,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.25,0,-0.25,0,0,0,0,0,0,0,-0.25,0,0.25,0,0,0,0,0,0,0,0,0,0,0,0,-0.5,0,0.5,0,0,0,0,0,0,0,0.5,0,-0.5,0,0,0,0,0,0,0,0,0,0,0,0,0.25,0,-0.25,0,0,0,0,0,0,0,-0.25,0,0.25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-2,4,-2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0.25,0,0,0,0,0,0,0,0,0,-0.25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.5,0,0,0,0,0,0,0,0,0,0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.5,0,0,0,0,0,0,0,0,0,-0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.25,0,0,0,0,0,0,0,0,0,0.25,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0.25,0,-0.25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.5,0,0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.5,0,-0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-0.25,0,0.25,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
};


void LogMinMax(CvArr* mat,std::ostream& os)
{
	//cvNormalize(gray,frame,1,0,CV_MINMAX);
	double m, M;
	cvMinMaxLoc(mat, &m, &M, NULL, NULL, NULL);
	os<<m<<"\t"<<M<<std::endl;
}

HarrisBuffer::HarrisBuffer(void):kparam(5e-4),sig2(8.0),tau2(2.0),delay(0),SignificantPointThresh(1E-9),Border(5)
{
	iFrame=0;

	/*SpatialMaskSeparable=NULL;
	SpatialMask=NULL;
	TemporalMask1=NULL;
	TemporalMask2=NULL;*/
	
	normvec=NULL;

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

	
	if(normvec) cvReleaseMat(&normvec);
	
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

bool HarrisBuffer::Init(IplImage* firstfrm,std::string fname)
{
	SpatialMaskSeparable=CVUtil::GaussianMask1D(sig2);
	TemporalMask1=CVUtil::GaussianMask1D(tau2);
	TemporalMask2=CVUtil::GaussianMask1D(2*tau2);
	DerivMask.push_back(-0.5);DerivMask.push_back(0.0);DerivMask.push_back(0.5);


	int sz1=(int)TemporalMask1.size();
	int sz2=(int)TemporalMask2.size();

	if(SpatialMaskSeparable.size()<3)
	{
		std::cerr<<"Spacial smooting variance is too low"<<std::endl;
		return false;
	}

	if(sz1<3 || sz2<5)
	{
		std::cerr<<"Temporal smooting variance is too low"<<std::endl;
		return false;
	}

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

	if(!fname.empty())
		FeatureFile.open(fname.c_str());


	//JetFilter=cvCreateMat( LengthFeatures, SizeNeighb, CV_64F );
	cvInitMatHeader(&JetFilter,LengthFeatures,SizeNeighb,CV_64FC1,jet);
	//Initilizing normalization vector for JET features
	normvec= cvCreateMat( LengthFeatures, 1, CV_64F );
	double sx1=sqrt(sig2);
	double st1=sqrt(tau2);
	double sx2=sx1*sx1, sx3=sx1*sx2, sx4=sx1*sx3;
	double st2=st1*st1, st3=st1*st2, st4=st1*st3;
	double* data=(double*) normvec->data.ptr;
	
	data[0]= sx1;
	data[1]= sx1;
	data[2]= st1;
	data[3]= sx2;
	data[4]= sx2;
	data[5]= sx2;
	data[6]= sx1*st1;
	data[7]= sx1*st1;
	data[8]= st2;
	data[9]= sx3;
	data[10]= sx3;
	data[11]= sx3;
	data[12]= sx3;
	data[13]= sx2*st1;
	data[14]= sx2*st1;
	data[15]= sx2*st1;
	data[16]= sx1*st2;
	data[17]= sx1*st2;
	data[18]= st3;
	data[19]= sx4;
	data[20]= sx4;
	data[21]= sx4;
	data[22]= sx4;
	data[23]= sx4;
	data[24]= sx3*st1;
	data[25]= sx3*st1;
	data[26]= sx3*st1;
	data[27]= sx3*st1;
	data[28]= sx2*st2;
	data[29]= sx2*st2;
	data[30]= sx2*st2;
	data[31]= sx1*st3;
	data[32]= sx1*st3;
	data[33]= st4;

	return true;
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

	HarrisFunction(kparam, tmp);
	Hbuffer.Update(tmp,tstamp2);
	
	//LogMinMax(Hbuffer.Buffer,logfile);
	//databuffer.FrameIndices.print(std::cout);
	//databuffer.FrameIndices.print(logfile);
	//convbuffer.FrameIndices.print(logfile);
	//Hbuffer.FrameIndices.print(logfile);
	//std::cout<<iFrame<<std::endl;
	DetectInterestPoints(Border);

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
}

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

void HarrisBuffer::WriteFeatures(InterestPoint &ip)
{
	assert(ip.features);
	double *data=(double*)ip.features->data.ptr;
	FeatureFile<<ip.x<<"\t"<<ip.y<<"\t"<<ip.t<<"\t"<<ip.val<<"\t";
	for(int i=0;i<34;i++)
		FeatureFile<<data[i]<<"\t";
	FeatureFile<<std::endl;
}

void HarrisBuffer::DetectInterestPoints(int border)
{
	ipList.clear();
	Hbuffer.FindLocalMaxima(ipList,true);
	CvMat *reg=cvCreateMat( SizeNeighb, 1, CV_64F );
	
		
	//remove border
	if(border<2)
		border=2; // interest points in the boundary should be remove to have a valid local 5x5x5 mask
				  // an alternative could be extending by 2 pixel in space dimensions	

	//select significant points which are not in the boundary
	for(int i=0;i<(int)ipList.size();i++)
		if(ipList[i].x>=border && (ipList[i].x<frame->width-border) &&
			ipList[i].y>=border && (ipList[i].y<frame->height-border) && ipList[i].val>SignificantPointThresh)
			ipList[i].reject=false;

	//computing JET features around an interest point by 5x5x5 local mask
	for(int i=0;i<(int)ipList.size();i++)
		if(!ipList[i].reject)
		{
			ipList[i].features=cvCreateMat( LengthFeatures, 1, CV_64F );
			convbuffer.GetLocalRegion(ipList[i].x,ipList[i].y ,ipList[i].t,5,5,5, reg);
			cvMatMul(&JetFilter,reg,ipList[i].features);
			cvMul(ipList[i].features,normvec,ipList[i].features);//normalization
		}
	cvReleaseMat(&reg);

	//check tstamp for any possible error

	//writing selected interest points to file
	for(int i=0;i<(int)ipList.size();i++)
		if(!ipList[i].reject)
			WriteFeatures(ipList[i]);
}		

int HarrisBuffer::NumberOfDetectedIPs()
{
	//return ipList.size();
	int n=0;
	for(int i=0;i<(int)ipList.size();i++)
		if(!ipList[i].reject)
			n++;
	return n;
}

void HarrisBuffer::DrawInterestPoints(IplImage* im)
{	
	if(ipList.size()>0)
		if(ipList[0].t!=iFrame-convbuffer.BufferSize)
			return;
	for(int i=0;i<(int)ipList.size();i++)
		if(!ipList[i].reject)
			CVUtil::DrawCross(im,cvPoint(ipList[i].x,ipList[i].y));
}

