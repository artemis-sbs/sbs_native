#include "map_data.h"
#include "image.h"
#include "colors.h"

void create_image_from_map(MapGenerator<float>& map, int height, int width, float image_scale) {
    unsigned long int seed = 1234;
    unsigned char* image = new unsigned char [height*width*BYTES_PER_PIXEL];

    float multiplier = 4.0;
    char* imageFileName = (char*) "map.bmp";
int i, j;
    float scale = image_scale;
    int values[11] {0, 0, 0, 0, 0,0,0,0,0,0,0};

    //float map_left = map.
    
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            vector3d<float> v{(float)i,0,(float)j};
            // terrace is -1 to
            
            //https://www.reddit.com/r/GraphicsProgramming/comments/kcx4yl/is_perlin_noise_supposed_to_be_this_gray_and/
            int terrain_type;
            int sample_type;
            map.get_value_at(v.x, 0, v.z, terrain_type, sample_type, scale);
            //node->GenUniformGrid2D(&image, 0,0, width, height, scale, 1234);
            
                
            int h = terrain_type*32;
            int val = 100; // (int)sample_cell * 3 + 25;
            int s = (int)sample_type*10;
            s /= 2; s += 50;
            // if (sample_height < 1 && t == 4) {
            //      val = 0;
            // }
            //if (terrace_pre < 0.5f)  val = 50;

            
            HSV hsl {(double)h, s/100.0, val/100.0};
            //HSV hsl {t*32, 1.0, 1.0};
            RGB rgb = HSVToRGB(hsl);

            // Image is flipped for Artemis??
            int y =  i; // height-1 - i;//+1024.0f);
            int x = j; // width -1 -j;//+1024.0f);
            int image_width = width*BYTES_PER_PIXEL;
            image[2+x*BYTES_PER_PIXEL+y*image_width] = (unsigned char) ( rgb.R );             ///red
            image[1+x*BYTES_PER_PIXEL+y*image_width] = (unsigned char) ( rgb.G );              ///green
            image[x*BYTES_PER_PIXEL+y*image_width] = (unsigned char) ( rgb.B ); ///blue
        }
    }

    generateBitmapImage((unsigned char*) image, height, width, imageFileName);
    printf("Map Image generated!!");
 
    delete [] image;
}

