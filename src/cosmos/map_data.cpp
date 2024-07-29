#include "map_data.h"
#include "image.h"
#include "colors.h"
#include "star_system.h"
#include <algorithm>
#include <vector>
#include <glm/vec2.hpp> // glm::vec3



void fill_image_from_map(MapGenerator<float>& map, unsigned char* image, 
		int height, int width, float image_scale) {
	const int BYTES_PER_PIXEL = 3; /// red, green, & blue
    float multiplier = 4.0;

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
            int faction_type;
            map.get_value_at(v.x, 0, v.z, terrain_type, sample_type, faction_type, scale);
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

}

double distance(double x1, double y1, double x2, double y2) {
    return std::sqrt(std::pow((x2 - x1), 2) + std::pow((y2 - y1), 2));
}

// Build a lookup table that store than nearest feature point for late lookup
void fill_cellular_lookup(std::vector<glm::vec2>& feature_points, unsigned int* image, int height, int width) {

	const int BYTES_PER_PIXEL = 1; /// red, green, & blue
    
    uint32_t seed = 0;
    int grid_step = 100;
    int num_features = height/grid_step  * width/grid_step;
    
    int row_length = height/grid_step;
    int col_length = width/grid_step;

    int row_height = grid_step;
    int col_width = grid_step;
    int row_offset_max = grid_step/2;
    int col_offset_max = grid_step/2;
    int gutter =5;// col_offset_max/10;

    /// Calculate feature points
    int row, col;
    //auto feature_points = std::make_unique<glm::vec2[]>(num_features);//new glm::vec2[num_features];
    for (row = 0; row < height; row+= grid_step) {
        for (col = 0; col < width; col+=grid_step) {
            RandomContextSquirrel rnd(seed + ((col&0xFFFF) <<16) | (row&0xFFFF));
            int index = row/grid_step*row_length +col/grid_step;
            feature_points[index].x = col + col_offset_max + rnd.an_int(gutter-col_offset_max ,col_offset_max -gutter);
            feature_points[index].y = row + row_offset_max + rnd.an_int(gutter-row_offset_max,row_offset_max-gutter);
        }
    }

    //int i, j;
    // This uses a grid, finding the nearest neighbors 
    // could be done and could be done at O(n log n)
    // Then store the neighbors with the feature_point
    int local_feature_points[9];
    int last_index_y = -1;
    int last_index_x = -1;
    
    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            glm::vec2 this_point = {row,col};
            // Index of the center
            int index_y = row/grid_step;
            int index_x = col/grid_step;
            if (last_index_x != index_x || last_index_y != index_y ) {
                last_index_y = index_y;
                last_index_x = index_x;
                
                // Build the local ones
                int local_index = 0;
                for (int index_r=-1; index_r<2;index_r++) {
                    int row = index_r;
                    if (index_y+index_r < 0) row = 0;
                    if (index_y+index_r >= grid_step/10) row = 0;

                    for (int index_c=-1; index_c<2;index_c++) {
                        int col = index_c;
                        if (index_x+index_c < 0) col = 0;
                        if (index_x+index_c >= grid_step/10) col = 0;

                        int lf_i = (index_r+1)*3+index_c+1;
                        int f_i = (index_y+row)*row_length + (index_x+col);
                        //local_feature_points[lf_i] = feature_points[f_i];
                        local_feature_points[local_index++] = f_i;
                    }
                }
            }

            double d =  20000;
            
            int used_index = 0;
            for(int index=0;index<9;index++) {
                //int index = i/100*10+j/100;
                int feature_point_index = local_feature_points[index]; 
                auto feature_point = feature_points[feature_point_index];            
                double ld = std::min(d, distance(col,row, feature_point.x,feature_point.y));
                if (d > ld) {
                    used_index = feature_point_index;
                    d = ld;
                }
                
            }
            if (row != 0) {
                used_index  = used_index;

            }
            image[col+row*width] = used_index;
        }
    }

}


void fill_image_from_cellular(std::vector<glm::vec2>& feature_points, unsigned int* lookup, unsigned char* image, int height, int width) {

	const int BYTES_PER_PIXEL = 3; /// red, green, & blue
    
    uint32_t seed = 0;
    int grid_step = 100;
    int num_features = height/grid_step  * width/grid_step;
    
    int row_length = height/grid_step;
    int col_length = width/grid_step;

    int row_height = grid_step;
    int col_width = grid_step;
    int row_offset_max = grid_step/2;
    int col_offset_max = grid_step/2;
    int gutter =5;// col_offset_max/10;

    


    /// Calculate feature points
    int i, j;
    //auto feature_points = std::make_unique<glm::vec2[]>(num_features);//new glm::vec2[num_features];
    
    auto feature_points_colors = std::vector<float>(num_features);//new glm::vec2[num_features];
    //int i, j;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            glm::vec2 this_point = {i,j};
            // Index of the center
            int used_index = lookup[j + i*width ];
            glm::vec2 feature_point= feature_points[used_index];            
            double d = distance(j,i, feature_point.x,feature_point.y);
            
            unsigned char c =  (d / (float)grid_step) * 255;
            //HSV hsl {t*32, 1.0, 1.0};
            
            float s= 1.0;
            float g = 360;
            if (d < 5.0) {
                c = 255;
            }
            else if (d < 10.0) {
                c = 255;
                s = 0.5;
            }
            else if (d < 11.0) {
                c = 0;
            }
            else if (d < 20.0) {
                c = 255;
                s = 0.5;
            }
            else if (d < 24.0) {
                c = 0;
                s = 0.5;
            }
            else if (d < 30.0) {
                c = 255;
                s = 0.5;
            }
            else if (d < 33.0) {
                c = 0;
            }
            else {
                c = 255 - c;
                g = 280;
                if (feature_point.x > width/2) g-=90;
                if (feature_point.y > height/2) g-=180;
            }
            
            HSV hsl {g, s, c/255.0};
            // RGB rgb;
            // rgb.R = c;
            // rgb.G = c;
            // rgb.B = c;
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

}


void create_image_from_map(MapGenerator<float>& map, int height, int width, float image_scale) {
    char* imageFileName = (char*) "map.bmp";
    unsigned char* image = new unsigned char [height*width*BYTES_PER_PIXEL];
    fill_image_from_map(map, image, width, height, image_scale);

    generateBitmapImage((unsigned char*) image, height, width, imageFileName);
    printf("Map Image generated!!");
    delete [] image;
}



