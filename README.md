This program is the C++/OpenCV implementation of the Ivan Laptev's 
MATLAB programs for online detection Space-Time interest points. 
The speed up is 12-15 times.

This program was developed during an internship of the AMI project (www.amiproject.org)
under supervision of Dr. Barabara Caputo at IDIAP (www.idiap.ch).

Current implementation only supports Harris interest points and the 
computes JET features in a local neighborhood (5x5x5) around an interest
point in 3D dimensional array (2 dimensions for space and 1 for time).

This program has been tested both on WindowsXP and Linux. For building 
you need to have installed the OpenCV library version 1.

Building notes:

This program supports two ways for acquiring images from webcam
1- HighGUI library (default and already built in OpenCV) but supports 
a few webcam.
2-cvcam: if you have built the cvcam interface in the OpenCV you can 
use this library for supporting a wider range of webcams.
for using cvcam library just uncomment the following macro in pcheader.h #define USE_CVCAM
for testing the cvcam library, a small program is provided in cmdcam directory.


Windows: related project files are provided for Visual Studio 2003.
Linux: You should have premake  (premake.sourceforge.net) after 
editing "stfeatures.lua" for the related path (include and libs) 
issue the following command in the top directory:

premake --file --target gnu
make   (for debug build) or
make CONFIG=Release  (for release build)

 
 
 
Usage notes:

by issuing the command :
stfeatures or stfeatures -h , you can get a list of parameters and options





Output format:

Each row in the output file corresponds to an interest point
with the following elements:

x y t harrisfunction  "34 values for JET features" 


for a small demo in MATLAB run go to test directory and 
run "demo_stf"
