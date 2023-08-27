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
#include "src/region_data.h"


void old_test();


int main()
{

    //MapGenerator<float> map(0,0,0, 100000, 0, 100000, 1234);
    //create_image_from_map(map, 1000,1000, 100000/1000);
    //MapGenerator<float> map(0,0,0, 50000, 0, 50000, 1234);
    //create_image_from_map(map, 1000,1000, 50000/1000);

    old_test();
    //try_border();

}

void swirl (float x, float y, float cx, float cy, float& sx, float& sy, float swirlRadius, float swirlTwists) {
    // compute the distance and angle from the swirl center:
    float pixelX = (float)x - cx;
    float pixelY = (float)y - cy;
    float pixelDistance = sqrt((pixelX * pixelX) + (pixelY * pixelY));
    float pixelAngle = atan2(pixelY, pixelX);

    // work out how much of a swirl to apply (1.0 in the center fading out to 0.0 at the radius):
    float swirlAmount = 1.0f - (pixelDistance / swirlRadius);
    if(swirlAmount > 0.0f)
    {
        float twistAngle = swirlTwists * swirlAmount * PI * 2.0;

        // adjust the pixel angle and compute the adjusted pixel co-ordinates:
        pixelAngle += twistAngle;
        sx = cos(pixelAngle) * pixelDistance;
        sy = sin(pixelAngle) * pixelDistance;
    }
}

void old_test()
{
    unsigned long int seed = 1234;
    auto simplex =FastNoise::New<FastNoise::Value>(); 
    auto fractal =FastNoise::New<FastNoise::FractalFBm>(); 
    auto region_distance = FastNoise::New<FastNoise::CellularDistance>();
    auto region_value = FastNoise::New<FastNoise::CellularValue>();
    float region_jitter = 2.0;
    region_distance->SetJitterModifier(region_jitter);
    region_value->SetJitterModifier(region_jitter);
    region_distance->SetDistanceFunction(FastNoise::DistanceFunction::EuclideanSquared);
    region_value->SetDistanceFunction(FastNoise::DistanceFunction::EuclideanSquared);

    // auto dom_scale =FastNoise::New<FastNoise::DomainScale>(); 
    // dom_scale->SetScale(0.66f);
    // dom_scale->SetSource(fractal);
    fractal->SetSource(simplex);
    fractal->SetGain(0.3f);
    fractal->SetLacunarity(5);
    //fractal->SetWeightedStrength(1.5);
    fractal->SetOctaveCount(3);
    // Use this as terrain
    auto node = simplex;

    char* imageFileName = (char*) "bitmapImage.bmp";
    char* regionFileName = (char*) "new_region.bmp";
    RegionData region_data(1000,1000);

    float scale = 0.00003f; // this is normal scale
    scale *= 10; // zoom in == 10,000
    scale *= 10; // zoom in == 100,000
    
    float multiplier = 9.0;
    float sample_multiplier=10.0;
    float region_count = 9.0;
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
            vector3d v{X,0,Y};
            //10  zoom in == 1,000,000
            //100 zoom in == 10,000,000
            float t_scale = 5.2;
            
            // terrace is -1 to 1
            float source_x = v.x*scale*t_scale;
            float source_z = v.z*scale*t_scale;
            float center_x = width/2*scale*t_scale;
            float center_z = height/2*scale*t_scale;
            float fade = 50;

            swirl(source_x, source_z, center_x,center_z, source_x, source_z, t_scale*scale*width, 0.3);
            
            //https://www.reddit.com/r/GraphicsProgramming/comments/kcx4yl/is_perlin_noise_supposed_to_be_this_gray_and/
            auto terrace = node->GenSingle2D(source_x, source_z, seed) ;
            // add 'octave'
            terrace += node->GenSingle2D(source_x*4, source_z*4, seed) * 0.25;

            terrace = std::clamp(terrace,-1.0f,1.0f);
            

            if (v.x < fade) terrace = terrace * (1.0-((fade-v.x)/fade));
            if (v.z < fade) terrace = terrace * (1.0-((fade-v.z)/fade));
            if (v.x > width- fade) terrace = terrace * (width-v.x)/fade;
            if (v.z > (height-fade)) terrace = terrace * ((height-v.z)/fade);

            // auto t_holes = abs(node->GenSingle2D(v.x*scale*t_scale, v.z*scale*t_scale, seed+2000));
            // if (t_holes < 0.25) t_holes = 0;
            // else (t_holes = 1.0) = 1.0;
            //t_holes -= node->GenSingle2D(v.x*scale*t_scale, v.z*scale*t_scale, seed-2000);
            //auto t_holes = 1.0;

            float region_scale = t_scale * 1.0;
            center_x = width/2*scale*region_scale;
            center_z = height/2*scale*region_scale;
            source_x = v.x*scale*region_scale;
            source_z = v.z*scale*region_scale;
            swirl(source_x, source_z, center_x,center_z, source_x, source_z, region_scale*scale*width, 0.3);
            // Use simple for regions for crisp borders?
            
            auto region_temp = node->GenSingle2D(source_x, source_z, seed+5578412) * 20;
            auto region_moist = node->GenSingle2D(source_x, source_z, seed-251890) * 20;
            float region =floor((round(region_moist+20) + round(region_temp+20)) /10);
            if (v.x < fade) region = region * (1.0-((fade-v.x)/fade));
            if (v.z < fade) region = region * (1.0-((fade-v.z)/fade));
            if (v.x > width- fade) region = region * (width-v.x)/fade;
            if (v.z > (height-fade)) region = region * ((height-v.z)/fade);


            // regions are marked from the peaks down
            float peaks_threshold = 0.35;
            bool is_neg = region <-1+peaks_threshold;
            bool is_pos = region >1-peaks_threshold;
            bool is_zero = !is_neg && !is_pos;
            //bool true_zero = region <= 0.01 && region >= 0.01;
            int f= 255;
            //if (true_zero) f = 129;
            //else 
            if (is_neg) f = 254;
            else if (is_pos) f = 253;

            region_data.set_region(X-1, Y-1,region);
            /// Terrain is from outer edge in
            float terrain_border_threshold = 0.1;
            is_neg = terrace <-terrain_border_threshold;
            is_pos = terrace > terrain_border_threshold;
            is_zero = !is_neg && !is_pos;
            
            auto terrace_pre = terrace;
            terrace = terrace*0.5f+0.5f;
            terrace *= multiplier;
            terrace = std::clamp(terrace,0.0f,multiplier);
            
            
            //int t = (int)terrace;
            int t = (int)region;
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
            s = 50; val = 100;
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
    region_data.generate_image(regionFileName);
    delete [] image;

    for (int i=0; i<11;i++ ) {
        std::cout << i-1 << " had " << values[i] << '\n';
    }
    printf("Image generated!!");
}

