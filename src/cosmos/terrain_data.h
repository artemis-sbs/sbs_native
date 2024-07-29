#pragma once

#include "grid_data.h"

// Fill background with given color
template <typename TERRAIN_VALUE = unsigned char>
class TerrainData : public GridData<TERRAIN_VALUE>
{
public:
    TerrainData(int w, int h) : GridData<TERRAIN_VALUE>(w,h)
    {
        
    }
    ~TerrainData()
    {
        ~GridData<TERRAIN_VALUE>();
    }

    TERRAIN_VALUE get_terrain(int x, int y)
    {
        return this->get_grid(x,y);
    }
    void set_faction(int x, int y, TERRAIN_VALUE value)
    {
        return this->set_grid(x,y, value);
    }

    RGB  get_color_value(int x, int y) {
        int f = get_grid(x, y);

        double v = 1.0 - 0.5f * (float)(f % 2);
        double s = 1.0;
        int h = f * 32;

        if (f == BORDER)
            v = 0;
        else if (f >= 240 && f<250)
        {
            h = (f - 240) * 32;
            v = 1.0;
            s = 0.5;
        }
        if (f == ANOMALOUS_SPACE)
        {
            v = 0.5;
            s = 0.0;
        }
        HSV hsv{h, s, v};
        RGB rgb = HSVToRGB(hsv);

    }
};