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

const int height = 1000;
const int width = 1000;

unsigned char image[height][width][BYTES_PER_PIXEL];


void old_test();
void try_border();

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
    auto simplex =FastNoise::New<FastNoise::OpenSimplex2>(); 
    auto fractal =FastNoise::New<FastNoise::FractalFBm>(); 
    // auto dom_scale =FastNoise::New<FastNoise::DomainScale>(); 
    // dom_scale->SetScale(0.66f);
    // dom_scale->SetSource(fractal);
    fractal->SetSource(simplex);
    fractal->SetGain(0.3f);
    fractal->SetLacunarity(4.5);
    //fractal->SetWeightedStrength(0.5);
    fractal->SetOctaveCount(3);
    // Use this as terrain
    auto node = fractal;


      
    char* imageFileName = (char*) "bitmapImage.bmp";

    float scale = 0.000008f; // this is normal scale
    scale *= 10; // zoom in == 10,000
    scale *= 10; // zoom in == 100,000
    //scale *= 10; // zoom in == 1,000,000
    //scale *= 10; // zoom in == 10,000,000
    float multiplier = 8.0;
    float sample_multiplier=10;
    int i, j;
    float last_sample=0;
    int values[11] {0, 0, 0, 0, 0,0,0,0,0,0,0};
    for (i = 1; i <= height; i++) {
        for (j = 1; j <= width; j++) {
            vector3d v{i,0,j};
            // terrace is -1 to
            //https://www.reddit.com/r/GraphicsProgramming/comments/kcx4yl/is_perlin_noise_supposed_to_be_this_gray_and/
            auto terrace = node->GenSingle2D(v.x*scale, v.z*scale, seed);
            terrace = terrace*0.5f+0.5f;
            auto terrace_pre = terrace;
            terrace *= multiplier;
            terrace = std::clamp(terrace,0.0f,multiplier);
            
            int t = (int)terrace;
            
            float sample_height =  terrace-(float)t;
            sample_height *= sample_multiplier;

            //float sample_scale = scale*(12-2*(abs((t-4))));
            //auto sample_height = node->GenSingle2D(v.x*sample_scale, v.z*sample_scale, seed);

            //sample_height += sample_multiplier/2;

            // Cell 
            // float cell_scale = scale*1.1; //*(12-2*(abs((t-4))));
            // auto sample_cell = cell_node->GenSingle2D(v.x*cell_scale, v.z*cell_scale, seed);
      

            // sample_cell = sample_cell*0.5f+.5;
            // sample_cell *= sample_multiplier;
            
            int h = t*32;
            int val = 100; // (int)sample_cell * 3 + 25;
            int s = (int)sample_height*10;
            s /= 2; s += 50;
            // if (sample_height < 1 && t == 4) {
            //      val = 0;
            // }
            if (terrace_pre < 0.5f)  val = 50;

            

            
            
            
            //last_sample = sample_cell;
                        
            values[t+1] += 1;
           
            HSV hsv {h, s/100.0, val/100.0};
            //HSV hsl {t*32, 1.0, 1.0};
            RGB rgb = HSVToRGB(hsv);

            int x = i-1;//+1024.0f);
            int y = j-1;//+1024.0f);
            image[x][y][2] = (unsigned char) ( rgb.R );             ///red
            image[x][y][1] = (unsigned char) ( rgb.G );              ///green
            image[x][y][0] = (unsigned char) ( rgb.B ); ///blue
        }
    }

    generateBitmapImage((unsigned char*) image, height, width, imageFileName);
    for (int i=0; i<11;i++ ) {
        std::cout << i-1 << " had " << values[i] << '\n';
    }
    printf("Image generated!!");
}

void try_border()
{
    float multiplier = 8.0;
    float sample_multiplier=10;

    unsigned long int seed = 1234;
    auto node =FastNoise::New<FastNoise::CellularDistance>();
    if (seed == 0) seed = static_cast<long unsigned int>(time(0));
    auto eng = std::default_random_engine {seed};
    auto faction_gen = std::uniform_real_distribution<float> (0, multiplier);

    char* imageFileName = (char*) "border.bmp";

    float scale = 0.00003f; // this is normal scale
    scale *= 10; // zoom in == 10,000
    scale *= 10; // zoom in == 100,000
    scale *= 10; // zoom in == 1,000,000
    int i, j;
    int values[11] {0, 0, 0, 0, 0,0,0,0,0,0,0};
    for (i = 1; i <= height; i++) {
        for (j = 1; j <= width; j++) {
            vector3d v{i,0,j};
            // terrace is -1 to
            //https://www.reddit.com/r/GraphicsProgramming/comments/kcx4yl/is_perlin_noise_supposed_to_be_this_gray_and/
            auto sample_size = node->GenSingle2D(v.x*scale, v.z*scale, seed);
            //node->GenUniformGrid2D(&image, 0,0, width, height, scale, 1234);
            
            float  faction = faction_gen(eng);
            int t = (int)faction;

            int s = 100;
            if (sample_size>-0.1 && sample_size<0.1)
                s=0;

            
            values[t+1] += 1;
            
            HSV hsl {t*32, 1.0, s/100.0};
            //HSV hsl {t*32, 1.0, 1.0};
            RGB rgb = HSVToRGB(hsl);

            int x = i-1;//+1024.0f);
            int y = j-1;//+1024.0f);
            image[x][y][2] = (unsigned char) ( rgb.R );             ///red
            image[x][y][1] = (unsigned char) ( rgb.G );              ///green
            image[x][y][0] = (unsigned char) ( rgb.B ); ///blue
        }
    }

    generateBitmapImage((unsigned char*) image, height, width, imageFileName);
    for (int i=0; i<11;i++ ) {
        std::cout << i-1 << " had " << values[i] << '\n';
    }
    printf("Image generated!!");
}