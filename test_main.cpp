#include <FastNoise/FastNoise.h>
#include <FastNoise/Metadata.h>

#include <iostream>

#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include "src/point_scatter.h"
#include "src/colors.h"
#include <algorithm>


#include "src/image.h"
#include "src/map_data.h"
#include "src/faction_data.h"




void old_test();


int main()
{
    MapGenerator<float> map(0,0,0, 100000, 0, 100000, 1234);
    create_image_from_map(map, 1000,1000, 100000/1000);
    //MapGenerator<float> map(0,0,0, 50000, 0, 50000, 1234);
    //create_image_from_map(map, 1000,1000, 50000/1000);

    old_test();
    //try_border();

}

void old_test()
{
    unsigned long int seed = 1234;
    auto simplex =FastNoise::New<FastNoise::Simplex>(); 
    auto fractal =FastNoise::New<FastNoise::FractalFBm>(); 
    // auto dom_scale =FastNoise::New<FastNoise::DomainScale>(); 
    // dom_scale->SetScale(0.66f);
    // dom_scale->SetSource(fractal);
    fractal->SetSource(simplex);
    fractal->SetGain(0.3f);
    fractal->SetLacunarity(5);
    //fractal->SetWeightedStrength(1.5);
    fractal->SetOctaveCount(3);
    // Use this as terrain
    auto node = fractal;

    char* imageFileName = (char*) "bitmapImage.bmp";
    char* factionFileName = (char*) "new_faction.bmp";
    FactionData faction_data(1000,1000);

    float scale = 0.00001f; // this is normal scale
    scale *= 10; // zoom in == 10,000
    scale *= 10; // zoom in == 100,000
    //scale *= 10; // zoom in == 1,000,000
    //scale *= 10; // zoom in == 10,000,000
    float multiplier = 9.0;
    float sample_multiplier=10.0;
    float faction_count = 9.0;
    int Y, X;
    float last_sample=0;
    int values[11] {0, 0, 0, 0, 0,0,0,0,0,0,0};
    const int height = 1000;
    const int width = 1000;

    unsigned char* image = new unsigned char [height*width*BYTES_PER_PIXEL];
    // How bog should the cache be?
    // Borders is 100x100 (less than 4K) OK, or is (256k)
    //node->GenUniformGrid2D(&image, 0,0, width, height, scale, 1234);
    for (Y = 1; Y <= height; Y++) {
        for (X = 1; X <= width; X++) {
            vector3d v{Y,0,X};
            float t_scale = 10;
            // terrace is -1 to 1
            //https://www.reddit.com/r/GraphicsProgramming/comments/kcx4yl/is_perlin_noise_supposed_to_be_this_gray_and/
            auto terrace = node->GenSingle2D(v.x*scale*t_scale, v.z*scale*t_scale, seed);

            auto t_holes = abs(node->GenSingle2D(v.x*scale*t_scale, v.z*scale*t_scale, seed+2000));
            if (t_holes < 0.25) t_holes = 0;
            else (t_holes = 1.0) = 1.0;
            //t_holes -= node->GenSingle2D(v.x*scale*t_scale, v.z*scale*t_scale, seed-2000);
            //auto t_holes = 1.0;

            // Use simple for factions for crisp borders?
            auto faction = node->GenSingle2D(v.x*scale*10, v.z*scale*10, seed);
            auto holes = abs(node->GenSingle2D(v.x*scale*10, v.z*scale*10, seed+2000));
            if (holes < 0.25) holes = 0;
            else (holes = 1.0) = 1.0;
            //holes -= node->GenSingle2D(v.x*scale*10, v.z*scale*10, seed-2000);
            //auto holes=1.0;

            // auto holes2 = node->GenSingle2D(v.x*scale*10, v.z*scale*10, seed-2000);
            //terrace *= t_holes;
            faction*= holes;

            bool is_neg = faction <-0.1;
            bool is_pos = faction >0.1;
            
            bool is_zero = !is_neg && !is_pos;
            //bool true_zero = faction <= 0.01 && faction >= 0.01;
            int f= 255;
            //if (true_zero) f = 129;
            //else 
            if (is_neg) f = 254;
            else if (is_pos) f = 253;

            faction_data.set_faction(X-1, Y-1,f);

            is_neg = terrace <-0.1;
            is_pos = terrace >0.1;
            is_zero = !is_neg && !is_pos;
            
            auto terrace_pre = terrace;
            terrace = terrace*0.5f+0.5f;
            terrace *= multiplier;
            terrace = std::clamp(terrace,0.0f,multiplier);
            
            
            int t = (int)terrace;
            float sample_height =  terrace-(float)t;
            sample_height *= sample_multiplier;

            int h = t*32;
         
            int val = 50; // (int)sample_cell * 3 + 25;
            int s = (int)sample_height*10;
            s /= 2; s += 50;
             if (is_neg) val = 50;
            else if (is_pos) val = 100;
            // if (sample_height < 1 && t == 4) {
            //      val = 0;
            // }
            //if (terrace_pre < 0.5f)  val = 50;

            //last_sample = sample_cell;
                        
            values[t+1] += 1;
           
            HSV hsv {h, s/100.0, val/100.0};
            //HSV hsl {t*32, 1.0, 1.0};
            RGB rgb = HSVToRGB(hsv);

            int x = X-1;//+1024.0f);
            int y = Y-1;//+1024.0f);
            int image_width = width*BYTES_PER_PIXEL;
            image[2+x*BYTES_PER_PIXEL+y*image_width] = (unsigned char) ( rgb.R );             ///red
            image[1+x*BYTES_PER_PIXEL+y*image_width] = (unsigned char) ( rgb.G );              ///green
            image[x*BYTES_PER_PIXEL+y*image_width] = (unsigned char) ( rgb.B ); ///blue

        }
    }

    generateBitmapImage((unsigned char*) image, width, height, imageFileName);
    faction_data.generate_image(factionFileName);
    delete [] image;

    for (int i=0; i<11;i++ ) {
        std::cout << i-1 << " had " << values[i] << '\n';
    }
    printf("Image generated!!");
}

