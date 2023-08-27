#pragma once

#include "point_scatter.h"
#include <FastNoise/FastNoise.h>
#include <FastNoise/Metadata.h>
#include <set>
#include <utility>

#include <random>

template <typename T>
class MapData
{
public:
    T x;
    T y;
    T z;
    //    T terrain_raw;
    //    T sample_raw;
    int terrain;
    int sample_type;

    MapData(T x, T y, T z, int terrain, int sample_type) : x(x),
                                                           y(y),
                                                           z(z),
                                                           //        terrain_raw(terrain_raw),
                                                           //        sample_raw(sample_raw),
                                                           terrain(terrain),
                                                           sample_type(sample_type)
    {
    }
};

template <typename T>
class MapGenerator
{
    FastNoise::SmartNode<FastNoise::Generator> node;
    PointScatter3d<T> scat;
    unsigned long int seed;
    std::set<std::pair<int, int>> inactive;

public:
    // MapGenerateStrategy<T>* strategy;
    //  Thee aren't used right now
    //T global_scale = 0.000008f; // this is base scale
    T global_scale = 0.00008f; // this is base scale
    //00003f
    T width_scale = 1.0;
    T height_scale = 1.0;
    T depth_scale = 1.0;
    int layer_multiplier = 8;
    int sample_multiplier = 10;
    T width;
    T height;
    T depth;
    T radius;
    T swirl_amount = 0.3;
    T cx;
    T cy;
    T cz;

public:
    MapGenerator(T cx, T cy, T cz, T width, T height, T depth, long unsigned int seed = 0)
    {
        this->width = width;
        this->height = height;
        this->radius = 1000;
        this->cx = cx;
        this->cy = cy;
        this->cz = cz;


        node = FastNoise::New<FastNoise::Value>();
        scat = PointScatter3d<T>(cx, cy, cz, width, height, depth, seed);
    }
    ~MapGenerator()
    {
        // delete strategy;
    }
    void set_layer_multiplier(int count)
    {
        layer_multiplier = count;
        node->SetMultiplier(count);
    }
    void reposition(T cx, T cy, T cz, T width, T height, T depth, long unsigned int new_seed = 0)
    {
        if (new_seed != 0)
        {
            seed = new_seed;
        }
        scat = PointScatter3d<T>(cx, cy, cz, width, height, depth, seed);
    }
    void swirl(float x, float y, float cx, float cy, float &sx, float &sy, float swirlRadius, float swirlTwists)
    {
        // compute the distance and angle from the swirl center:
        float pixelX = (float)x - cx;
        float pixelY = (float)y - cy;
        float pixelDistance = sqrt((pixelX * pixelX) + (pixelY * pixelY));
        float pixelAngle = atan2(pixelY, pixelX);

        // work out how much of a swirl to apply (1.0 in the center fading out to 0.0 at the radius):
        float swirlAmount = 1.0f - (pixelDistance / swirlRadius);
        if (swirlAmount > 0.0f)
        {
            float twistAngle = swirlTwists * swirlAmount * PI * 2.0;

            // adjust the pixel angle and compute the adjusted pixel co-ordinates:
            pixelAngle += twistAngle;
            sx = cos(pixelAngle) * pixelDistance;
            sy = sin(pixelAngle) * pixelDistance;
        }
    }

    T _get_value_at(float x, float y, float z, T additional_scale = 1.0)
    {
        // terrace is -1 to 1
        float source_x = x * additional_scale * width_scale;
        float source_z = z * additional_scale * depth_scale;
        float center_x =this->cx * additional_scale * width_scale;
        float center_z = this->cz * additional_scale * depth_scale;
        float scaled_radius =  additional_scale * radius * (width_scale / depth_scale);

        swirl(source_x, source_z, center_x, center_z, source_x, source_z, scaled_radius, swirl_amount);

        auto terrace = node->GenSingle2D(source_x, source_z, seed);
        // add 'octave'
        terrace += node->GenSingle2D(source_x * 4, source_z * 4, seed) * 0.25;
        terrace = std::clamp(terrace, -1.0f, 1.0f);
        return terrace;
    }

    virtual void get_value_at(float x, float y, float z, int &terrace_type, int &sample_type, int &faction_type, T additional_scale = 1.0)
    {
        float scale = additional_scale * global_scale;

        float terrace = _get_value_at(x, 0, z, scale);
        terrace = terrace * 0.5f + 0.5f;
        // terrace = tan(terrace);
        terrace *= layer_multiplier;
        terrace = std::clamp(terrace, 0.0f, (float)layer_multiplier);
        //// return
        terrace_type = (int)terrace;

        float sample_height =  terrace-(float)terrace_type;
        sample_height *= sample_multiplier;
        //// return
        sample_type = (int)sample_height;

        float faction = _get_value_at(x, 0, z, scale * 5.2);
        //// return
        terrace_type = (int)terrace;
        // Factions are marked from the peaks down
        float peaks_threshold = 0.35;
        bool is_neg = faction < -1 + peaks_threshold;
        bool is_pos = faction > 1 - peaks_threshold;
        bool is_zero = !is_neg && !is_pos;
        int f = 255;
        if (is_neg)
            f = 254;
        else if (is_pos)
            f = 253;
    }

    MapData<T> next()
    {
        auto v = scat.next();
        int terrace = 0;
        int sample = 0;
        int faction = 0;
        get_value_at(v.x, v.y, v.z, terrace, sample, faction);

        return MapData<T>{v.x, v.y, v.z, terrace, sample};
    }
};

void create_image_from_map(MapGenerator<float> &map, int height, int width, float image_scale);