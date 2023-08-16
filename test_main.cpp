#include <FastNoise/FastNoise.h>
#include <FastNoise/Metadata.h>

#include <iostream>

#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include "src/point_scatter.h"

#include <stdio.h>

const int BYTES_PER_PIXEL = 3; /// red, green, & blue
const int FILE_HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;

void generateBitmapImage(unsigned char* image, int height, int width, char* imageFileName);
unsigned char* createBitmapFileHeader(int height, int stride);
unsigned char* createBitmapInfoHeader(int height, int width);

const int height = 2048;
const int width = 2048;

unsigned char image[height][width][BYTES_PER_PIXEL];

int main()
{
    unsigned long int seed = 1234;

    //FastNoise::SmartNode<> node = FastNoise::NewFromEncodedNodeTree( "IgAAAIBAAAAAAAgA" );
    auto node = FastNoise::New<FastNoise::Terrace>();
    node->SetSource( FastNoise::New<FastNoise::Simplex>() );
    node->SetMultiplier(4.0);
    node->SetSmoothness(0.0);


    auto scat = PointScatter3d<float>(0.0f, 0.0f, 0.0f, 2048.0f, 2048.0f, 2048.0f, seed);
    for (int i=0; i<40;i++) {
        auto v = scat.next();
        std::cout << v;
        auto terrace = node->GenSingle2D(v.x, v.z, seed);
        //std:: cout << terrace;
        std::cout << ' ';
        if (terrace <= -3.0/4.0) {
            std::cout << "Celestial Body";
        } else if (terrace <= -2.0/4.0) {
            std::cout << "Far Nebula";
        } else if (terrace <= -1.0/4.0) {
            std::cout << "Enemy Hideout";
        }else if (terrace <= 0) {
            std::cout << "Far Asteroid";
        }else if (terrace <= 1.0/2.0) {
            std::cout << "Near Nebula";
        }else if (terrace <= 2.0/4.0) {
            std::cout << "Near asteroid";
        }else if (terrace <= 3.0/4.0) {
            std::cout << "Friendly Support";
        }else if (terrace <= 4.0) {
            std::cout << "Station";
        }
        if (i%3==2) std::cout << '\n';
        else std::cout << ' ';
    }



   
    char* imageFileName = (char*) "bitmapImage.bmp";

    float scale = 0.001f;
    int i, j;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            vector3d v{i,0,j};
            auto terrace = node->GenSingle2D(v.x*scale, v.z*scale, seed);

            terrace *=  128;
            terrace += 128;
            int x = floor(v.x);//+1024.0f);
            int y = floor(v.z);//+1024.0f);
            image[x][y][2] = (unsigned char) ( terrace );             ///red
            image[x][y][1] = (unsigned char) ( terrace );              ///green
            image[x][y][0] = (unsigned char) ( terrace ); ///blue
        }
    }

    generateBitmapImage((unsigned char*) image, height, width, imageFileName);
    printf("Image generated!!");
    return 0;
}


void generateBitmapImage (unsigned char* image, int height, int width, char* imageFileName)
{
    int widthInBytes = width * BYTES_PER_PIXEL;

    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4 - (widthInBytes) % 4) % 4;

    int stride = (widthInBytes) + paddingSize;

    FILE* imageFile = fopen(imageFileName, "wb");

    unsigned char* fileHeader = createBitmapFileHeader(height, stride);
    fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);

    unsigned char* infoHeader = createBitmapInfoHeader(height, width);
    fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

    int i;
    for (i = 0; i < height; i++) {
        fwrite(image + (i*widthInBytes), BYTES_PER_PIXEL, width, imageFile);
        fwrite(padding, 1, paddingSize, imageFile);
    }

    fclose(imageFile);
}

unsigned char* createBitmapFileHeader (int height, int stride)
{
    int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

    static unsigned char fileHeader[] = {
        0,0,     /// signature
        0,0,0,0, /// image file size in bytes
        0,0,0,0, /// reserved
        0,0,0,0, /// start of pixel array
    };

    fileHeader[ 0] = (unsigned char)('B');
    fileHeader[ 1] = (unsigned char)('M');
    fileHeader[ 2] = (unsigned char)(fileSize      );
    fileHeader[ 3] = (unsigned char)(fileSize >>  8);
    fileHeader[ 4] = (unsigned char)(fileSize >> 16);
    fileHeader[ 5] = (unsigned char)(fileSize >> 24);
    fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

    return fileHeader;
}

unsigned char* createBitmapInfoHeader (int height, int width)
{
    static unsigned char infoHeader[] = {
        0,0,0,0, /// header size
        0,0,0,0, /// image width
        0,0,0,0, /// image height
        0,0,     /// number of color planes
        0,0,     /// bits per pixel
        0,0,0,0, /// compression
        0,0,0,0, /// image size
        0,0,0,0, /// horizontal resolution
        0,0,0,0, /// vertical resolution
        0,0,0,0, /// colors in color table
        0,0,0,0, /// important color count
    };

    infoHeader[ 0] = (unsigned char)(INFO_HEADER_SIZE);
    infoHeader[ 4] = (unsigned char)(width      );
    infoHeader[ 5] = (unsigned char)(width >>  8);
    infoHeader[ 6] = (unsigned char)(width >> 16);
    infoHeader[ 7] = (unsigned char)(width >> 24);
    infoHeader[ 8] = (unsigned char)(height      );
    infoHeader[ 9] = (unsigned char)(height >>  8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL*8);

    return infoHeader;
}