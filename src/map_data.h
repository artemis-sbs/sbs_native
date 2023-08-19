#pragma once

#include "point_scatter.h"
#include <FastNoise/FastNoise.h>
#include <FastNoise/Metadata.h>
#include <set>
#include <utility>

#include <random>


template <typename T>
class MapData {
public:
    T x;
    T y;
    T z;
//    T terrain_raw;
//    T sample_raw;
    int terrain;
    int sample_type;
    

    MapData(T x, T y, T z, int terrain, int sample_type) : 
        x(x),
        y(y), 
        z(z),
//        terrain_raw(terrain_raw),
//        sample_raw(sample_raw),
        terrain(terrain),
        sample_type(sample_type)
        {}

};

template <typename T>
class MapGenerator
{
    FastNoise::SmartNode<FastNoise::Generator> node;
    PointScatter3d<T> scat;
    unsigned long int seed;
    std::set<std::pair<int, int>> inactive;
public:
    T global_scale = 0.000008f; // this is base scale
    T width_scale = 1.0;
    T height_scale = 1.0;
    T depth_scale = 1.0;
    int layer_multiplier = 8;
    int sample_multiplier = 10;
public:
    MapGenerator(T cx, T cy, T cz, T width, T height, T depth, long unsigned int seed=0)
    {
        this->seed = seed;
        auto simplex =FastNoise::New<FastNoise::OpenSimplex2>(); 
        auto fractal =FastNoise::New<FastNoise::FractalFBm>(); 
        auto dom_scale =FastNoise::New<FastNoise::DomainScale>(); 
        dom_scale->SetScale(0.66f);
        dom_scale->SetSource(fractal);
        fractal->SetSource(simplex);
        fractal->SetGain(0.3f);
        fractal->SetLacunarity(4.5);
        //fractal->SetWeightedStrength(0.5);
        fractal->SetOctaveCount(3);
        // Use this as terrain
        node = fractal;

        scat = PointScatter3d<T>(cx, cy, cz, width, height, depth, seed);
    }
    void set_layer_multiplier(int count) {
        layer_multiplier = count;
        node->SetMultiplier(count);
    }
    void reposition(T cx, T cy, T cz, T width, T height, T depth, long unsigned int new_seed=0) {
        if (new_seed != 0) {
            seed = new_seed;
        }
        scat = PointScatter3d<T>(cx, cy, cz, width, height, depth, seed);
    }
    void get_value_at(float x, float y, float z, int& terrace_type, int& sample_type, T additional_scale = 1.0) {
        float scale = additional_scale*global_scale;
        float terrace = node->GenSingle2D(x*scale*width_scale, z*scale*depth_scale, seed);
        terrace = terrace*0.5f+0.5f;
        //terrace = tan(terrace);
        terrace *= layer_multiplier;
        terrace = std::clamp(terrace,0.0f,(float)layer_multiplier);
        //// return 
        terrace_type = (int)terrace;
        

        float sample_scale = scale*(12-2*(abs((terrace_type-4))));
        auto sample = node->GenSingle2D(x*sample_scale, z*sample_scale, seed);
        sample *= sample_multiplier/2;
        sample += sample_multiplier/2;
        //// return
        sample_type = (int)sample;
    }
    MapData<T> next(){
        auto v = scat.next();
        int terrace=0;
        int sample = 0;
        get_value_at(v.x, v.y, v.z, terrace, sample);
        
        return MapData<T> {v.x, v.y, v.z, terrace, sample};
    }
};

void create_image_from_map(MapGenerator<float>& map, int height, int width, float image_scale);