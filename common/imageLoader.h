#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

// Source courtesy of J. Manson
// http://josiahmanson.com/prose/optimize_ppm/


namespace ppmLoader{
using namespace std;
void eat_comment(ifstream &f);

struct RGB
{
    unsigned char r, g, b;
};

struct ImageRGB
{
    int w, h;
    vector<RGB> data;
};


void load_ppm(ImageRGB &img, const string &name);


enum loadedFormat {
    rgb,
    rbg
};


void load_ppm( unsigned char * & pixels , unsigned int & w , unsigned int & h , const string &name , loadedFormat format = rgb);
}

#endif