#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image/stb_image.h"
#include "imageLoader.h"

// Source courtesy of J. Manson
// http://josiahmanson.com/prose/optimize_ppm/


namespace imageLoader {
    using namespace std;

    void eat_comment(ifstream &f) {
        char linebuf[1024];
        char ppp;
        while (ppp = f.peek(), ppp == '\n' || ppp == '\r')
            f.get();
        if (ppp == '#')
            f.getline(linebuf, 1023);
    }


    void load_ppm(ImageRGB &img, const string &name) {
        ifstream f(name.c_str(), ios::binary);
        if (f.fail()) {
            cout << "Could not open file: " << name << endl;
            return;
        }

        // get type of file
        eat_comment(f);
        int mode = 0;
        string s;
        f >> s;
        if (s == "P3")
            mode = 3;
        else if (s == "P6")
            mode = 6;

        // get w
        eat_comment(f);
        f >> img.w;

        // get h
        eat_comment(f);
        f >> img.h;

        // get bits
        eat_comment(f);
        int bits = 0;
        f >> bits;

        // error checking
        if (mode != 3 && mode != 6) {
            cout << "Unsupported magic number" << endl;
            f.close();
            return;
        }
        if (img.w < 1) {
            cout << "Unsupported width: " << img.w << endl;
            f.close();
            return;
        }
        if (img.h < 1) {
            cout << "Unsupported height: " << img.h << endl;
            f.close();
            return;
        }
        if (bits < 1 || bits > 255) {
            cout << "Unsupported number of bits: " << bits << endl;
            f.close();
            return;
        }

        // load image data
        img.data.resize(img.w * img.h);

        if (mode == 6) {
            f.get();
            f.read((char *) &img.data[0], img.data.size() * 3);
        } else if (mode == 3) {
            for (unsigned int i = 0; i < img.data.size(); i++) {
                int v;
                f >> v;
                img.data[i].r = v;
                f >> v;
                img.data[i].g = v;
                f >> v;
                img.data[i].b = v;
            }
        }

        // close file
        f.close();
    }

    //
    void load_png(ImageRGB &img, const string &name) {
        int channels;
        unsigned char *data = stbi_load(name.c_str(), &img.w, &img.h, &channels, 3); // Force 3 canaux (RGB)

        if (!data) {
            cout << "Could not open or decode file: " << name << endl;
            return;
        }
        cout << "lenght of the data" << data << endl;

        img.data.resize(img.w * img.h);
        for (int i = 0; i < img.w * img.h; i++) {
            img.data[i].r = data[i * 3];
            img.data[i].g = data[i * 3 + 1];
            img.data[i].b = data[i * 3 + 2];
        }
        cout<<"data lenght" << img.data.size() << endl;

    }


    void load_ppm(unsigned char *&pixels, unsigned int &w, unsigned int &h, const string &name, loadedFormat format) {
        ifstream f(name.c_str(), ios::binary);
        if (f.fail()) {
            cout << "Could not open file: " << name << endl;
            return;
        }

        // get type of file
        eat_comment(f);
        int mode = 0;
        string s;
        f >> s;
        if (s == "P3")
            mode = 3;
        else if (s == "P6")
            mode = 6;

        // get w
        eat_comment(f);
        f >> w;

        // get h
        eat_comment(f);
        f >> h;

        // get bits
        eat_comment(f);
        int bits = 0;
        f >> bits;

        // error checking
        if (mode != 3 && mode != 6) {
            cout << "Unsupported magic number" << endl;
            f.close();
            return;
        }
        if (w < 1) {
            cout << "Unsupported width: " << w << endl;
            f.close();
            return;
        }
        if (h < 1) {
            cout << "Unsupported height: " << h << endl;
            f.close();
            return;
        }
        if (bits < 1 || bits > 255) {
            cout << "Unsupported number of bits: " << bits << endl;
            f.close();
            return;
        }

        // load image data
        pixels = new unsigned char[3 * w * h];

        if (mode == 6) {
            f.get();
            f.read((char *) &(pixels[0]), 3 * w * h);
        } else if (mode == 3) {
            for (unsigned int i = 0; i < w * h; i++) {
                int v;
                f >> v;
                pixels[3 * i + 0] = v;
                f >> v;
                pixels[3 * i + 1] = v;
                f >> v;
                pixels[3 * i + 2] = v;
            }
        }

        // close file
        f.close();

        if (format == rgb) {
            return;
        }

        if (format == rbg) {
            for (unsigned int i = 0; i < w * h; i++) {
                unsigned char r = pixels[3 * i + 0], g = pixels[3 * i + 1], b = pixels[3 * i + 2];
                pixels[3 * i + 0] = r;
                pixels[3 * i + 1] = b;
                pixels[3 * i + 2] = g;
            }
            return;
        }
    }
//TODO


    void load_png(unsigned char *&pixels, unsigned int &w, unsigned int &h, const string &name, loadedFormat format) {
        // TODO
    }
}

