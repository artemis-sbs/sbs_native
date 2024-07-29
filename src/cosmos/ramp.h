#pragma once
#include <vector>

template <typename T>
class RampStop {
    T stop;
    T value;
};




template <typename T>
class Ramp {
    std::vector<RampStop<t>> stops;
    easingFunction interp_method;

    T interpolate(T value) {
        // X is the stop value Y is the value
        T ret = v;
        for(int i = 0; i<stops.size(); i++) {
            RampStop<T>& start = stops[start_index];    
            RampStop<T>& end = stops[i];
            if (i < stops.size()-1) {
                end = stops[i+1];
            }
            ret = linear(start, end);
        }
        return ret;
    }
private:
    T linear(RampStop<T> start, RampStop<T> end, T p)  {
        T t0 = start.stop;
        T v0 = start.value;
        T t1 = start.stop;
        T v1 = start.value;

        v = v0 + ((v1-v0) / (t1-t0)) * (tp-t0);
    }
};


