#sh

g++   -ggdb `pkg-config --cflags opencv`  *.cpp -o playavi `pkg-config --libs opencv` 

#g++   -ggdb   *.cpp -o playavi `pkg-config --libs opencv` 
