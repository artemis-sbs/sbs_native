#pragma once


// Fill background with given color
template <typename GRID_DATA = unsigned char>
class GridData
{
    GRID_DATA *grid;
    int width;
    int height;

public:
    GridData(int w, int h) : width(w), height(h)
    {
        grid = new GRID_DATA[width * height];
    }
    ~GridData()
    {
        delete[] grid;
    }

    int get_min_x() { return 0; }
    int get_max_x() { return width - 1; }
    int get_min_y() { return 0; }
    int get_max_y() { return height - 1; }

    GRID_DATA get_grid(int x, int y)
    {
        int loc = x + y * width;
        return grid[loc];
    }
    void set_grid(int x, int y, GRID_DATA value)
    {
        int loc = x + y * width;
        grid[loc] = value;
    }

    int find_closest_grid(int sx, int sy, int skip)
    {
        int left = sx-1; int left_spot = skip;
        int right = sx+1; int right_spot = skip;
        int count = 0;
        while (left >= get_min_x() || right <= get_max_x()){
            
            if (left>=get_min_x()) {
                left_spot  = get_grid(left, sy);
                if (left_spot < 127) return left_spot;
            }
            if (right<=get_max_x()) {
                right_spot  = get_grid(right, sy);
                if (right_spot < 127) return right_spot;
            }
            int mid = (right-left)/2;
            for(int  steps=0; steps<=mid; steps++) {
                if (sy-steps >= get_min_y()){
                    left_spot  = get_grid(left+steps, sy-steps);
                    if (left_spot < 127) return left_spot;
                }
                if (sy+steps <=get_max_y()) {
                    right_spot  = get_grid(right-steps, sy+steps);
                    if (right_spot < 127) return right_spot;
                }
            }
            if (left>get_min_x()) left--;
            if (right<get_max_x()) right++;
            
            count++;
        }
        return BORDER;
    }

    void floodFill(int sx, int sy, int currColor, int newColor, int &count)
    {
        std::queue<std::pair<int, int>> locs;
        locs.push(std::pair<int, int>(sx, sy));
        while (!locs.empty())
        {
            std::pair<int, int> xy = locs.front();
            locs.pop();
            int x = xy.first;
            int y = xy.second;

            // Base cases
            if (x < 0 || x > get_max_x() || y < 0 || y > get_max_y())
                continue;

            int this_spot = get_grid(x, y);
            if (this_spot != currColor)
            {
                continue;
            }
            if (get_grid(x, y) == newColor)
                continue;

            // Replace the color at cell (x, y)
            set_grid(x, y, newColor);
            count++;

            // Recursively call for north, east, south and west
            locs.push(std::pair<int, int>(x + 1, y));
            locs.push(std::pair<int, int>(x - 1, y));
            locs.push(std::pair<int, int>(x, y - 1));
            locs.push(std::pair<int, int>(x, y + 1));
        }
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

    void generate_image(char *factionFileName)
    {
        slow_fill();
        recolor_anom();
        //remove_border();

        unsigned char *faction_image = new unsigned char[height * width * BYTES_PER_PIXEL];
        int image_width = width * BYTES_PER_PIXEL;

        // try {
        for (int y = get_min_y(); y <= get_max_y(); y++)
        {
            for (int x = get_min_x(); x <= get_max_x(); x++)
            {

                faction_image[2 + x * BYTES_PER_PIXEL + y * image_width] = (unsigned char)(rgb.R); /// red
                faction_image[1 + x * BYTES_PER_PIXEL + y * image_width] = (unsigned char)(rgb.G); /// green
                faction_image[x * BYTES_PER_PIXEL + y * image_width] = (unsigned char)(rgb.B);     /// blue
            }
        }

        //}
        // catch {

        // };
        generateBitmapImage(faction_image, width, height, factionFileName);
        delete[] faction_image;
    }
};