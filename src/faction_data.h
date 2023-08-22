#pragma once
#include <queue>

const int BORDER = 255;
const int SPACE = 254;
const int NEG_SPACE = 253;
const int ANOMALOUS_SPACE = 252;
const int DEEP_SPACE = 251;

// Fill background with given color
template <typename FACTION_VALUE = unsigned char>
class FactionData
{
    FACTION_VALUE *grid;
    int width;
    int height;

public:
    FactionData(int w, int h) : width(w), height(h)
    {
        grid = new FACTION_VALUE[width * height];
    }
    ~FactionData()
    {
        delete[] grid;
    }

    int get_min_x() { return 0; }
    int get_max_x() { return width - 1; }
    int get_min_y() { return 0; }
    int get_max_y() { return height - 1; }

    FACTION_VALUE get_faction(int x, int y)
    {
        int loc = x + y * width;
        return grid[loc];
    }
    void set_faction(int x, int y, FACTION_VALUE value)
    {
        int loc = x + y * width;
        grid[loc] = value;
    }

    int find_closest_factions(int sx, int sy, int skip)
    {
        int left = sx-1; int left_spot = skip;
        int right = sx+1; int right_spot = skip;
        int count = 0;
        while (left >= get_min_x() || right <= get_max_x()){
            
            if (left>=get_min_x()) {
                left_spot  = get_faction(left, sy);
                if (left_spot < 127) return left_spot;
            }
            if (right<=get_max_x()) {
                right_spot  = get_faction(right, sy);
                if (right_spot < 127) return right_spot;
            }
            int mid = (right-left)/2;
            for(int  steps=0; steps<=mid; steps++) {
                if (sy-steps >= get_min_y()){
                    left_spot  = get_faction(left+steps, sy-steps);
                    if (left_spot < 127) return left_spot;
                }
                if (sy+steps <=get_max_y()) {
                    right_spot  = get_faction(right-steps, sy+steps);
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

            int this_spot = get_faction(x, y);
            if (this_spot != currColor)
            {
                continue;
            }
            if (get_faction(x, y) == newColor)
                continue;

            // Replace the color at cell (x, y)
            set_faction(x, y, newColor);
            count++;

            // Recursively call for north, east, south and west
            locs.push(std::pair<int, int>(x + 1, y));
            locs.push(std::pair<int, int>(x - 1, y));
            locs.push(std::pair<int, int>(x, y - 1));
            locs.push(std::pair<int, int>(x, y + 1));
        }
    }

    void remove_border()
    {
        for (int y = get_min_y(); y <= get_max_y(); y++)
        {
            for (int pass = 0; pass < 2; pass++)
            {
                int remove_color = BORDER;
                int faction_offset = 0;
                if (pass == 1)
                {
                    remove_color = ANOMALOUS_SPACE;
                    faction_offset = 240;
                }

                int start_color = remove_color;
                for (int start_x = 0; start_x <= get_max_x(); start_x++)
                {
                    int current = get_faction(start_x, y);
                    if (current != remove_color)
                    {
                        start_color = current;
                        continue;
                    }
                    // Got here mean we found start
                    // Find the end
                    int end_x = start_x + 1;
                    current = get_faction(end_x, y);
                    while (current == remove_color)
                    {
                        if (end_x == get_max_x())
                        {
                            break;
                        }
                        current = get_faction(end_x, y);
                        if (current != remove_color)
                            break;
                        end_x++;
                    }
                    int end_color = get_faction(end_x, y);
                    if (end_color == remove_color)
                        end_color = start_color;
                    if (start_color == remove_color)
                        start_color = end_color;
                    // As long as start_color or end_color is not border
                    // Fill the border over
                    int mid = start_x + (end_x - start_x) / 2;
                    for (int nx = start_x; nx <= end_x; nx++)
                    {
                        if (nx < mid)
                        {
                            set_faction(nx, y, start_color + faction_offset);
                        }
                        else
                        {
                            set_faction(nx, y, end_color + faction_offset);
                        }
                    }
                }
            }
        }
    }

    int recolor_anom()
    {
        int remove_color = ANOMALOUS_SPACE;
        int count=0;

        for (int y = get_min_y(); y <= get_max_y(); y++)
        {
            for (int start_x = get_min_x(); start_x <= get_max_x(); start_x++)
            {
                int current = get_faction(start_x, y);
                if (current == BORDER)
                    continue; // skip border
                if (current != remove_color)
                {
                    continue;
                }
                int start_color = find_closest_factions(start_x, y, ANOMALOUS_SPACE);
                
                floodFill(start_x, y, ANOMALOUS_SPACE, 240+start_color,count);
                count += 1;
            }
        }
        return count;
    }

    void slow_fill()
    {
        int faction = 0;
        int faction2 = 0;
        int count1 = 0;
        int count2 = 0;
        int way_too_small = 15;
        int too_small = 100;
        int limit = 25000;
        int count[10]{};

        for (int y = get_min_y(); y <= get_max_y(); y++)
        {
            for (int x = get_min_x(); x <= get_max_x(); x++)
            {
                // Build from the middle out
                int midx = (x + (width) / 2) % width;
                int midy = (y + (height) / 2) % height;
                FACTION_VALUE seed = get_faction(midx, midy);
                // Skip Edge
                if (seed == BORDER)
                    continue;
                int this_count = 0;
                if (seed == NEG_SPACE)
                {
                    floodFill(midx, midy, seed, faction, this_count);
                    if (this_count < way_too_small)
                    {
                        floodFill(midx, midy, faction, BORDER, this_count);
                    }
                    /// Anomalous space, refill with faction anomaly
                    else if (this_count < too_small)
                    {
                        floodFill(midx, midy, faction, ANOMALOUS_SPACE, this_count);
                    }
                    count[faction] += this_count;
                    if (count[faction] > limit)
                    {
                        faction++;
                        faction %= 4;
                        int retries = 0;
                        while (count[faction] > limit && retries < 10)
                        {
                            faction++;
                            faction %= 4;
                            retries++;
                        }
                    }
                }
                else if (seed == SPACE)
                {
                    int f2 = faction2 + 5;
                    floodFill(midx, midy, seed, f2, this_count);
                    if (this_count < way_too_small)
                    {
                        floodFill(midx, midy, f2, BORDER, this_count);
                    }
                    /// Anomalous space, refill with faction anomaly
                    else if (this_count < too_small)
                    {
                        floodFill(midx, midy, f2, ANOMALOUS_SPACE, this_count);
                        // printf("Anomalous %d\n", 250-(faction2+5));
                    }
                    count[f2] += this_count;
                    if (count[f2] > limit)
                    {
                        faction2++;
                        faction2 %= 4;
                        int retries = 0;
                        while (count[f2] > limit && retries < 10)
                        {
                            faction2++;
                            faction2 %= 4;
                            retries++;
                        }
                    }
                }
            }
        }
        printf("\n");
        for(int i=0;i<10;i++) {
            printf("\nfaction %d - count %d\n", i, count[i]);
        }
        
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
                int f = get_faction(x, y);

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