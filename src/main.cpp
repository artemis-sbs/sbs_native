#include <pybind11/pybind11.h>
#include <FastNoise/FastNoise.h>
#include <FastNoise/Metadata.h>

#include <random>
#include <string>
#include "point_scatter.h"


#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

template <typename T>
class MapData {
public:
    T x;
    T y;
    T z;
    int terrain;

    MapData(T x, T y, T z, int terrain) : 
        x(x),
        y(y), 
        z(z),
        terrain(terrain)
        {}

};

template <typename T>
class MapGenerator
{
    FastNoise::SmartNode<FastNoise::Terrace> node;
    PointScatter3d<T> scat;
    unsigned long int seed;
public:
    T width_scale = 1.0;
    T height_scale = 1.0;
    T depth_scale = 1.0;
    int layer_count = 8;
public:
    MapGenerator(T cx, T cy, T cz, T width, T height, T depth, long unsigned int seed=0)
    {
        this->seed = seed;
        //node = FastNoise::NewFromEncodedNodeTree( "IgAAAIBAAAAAAAgA" );
        // default is 8 layers
        node = FastNoise::New<FastNoise::Terrace>();
        node->SetSource( FastNoise::New<FastNoise::Simplex>() );
        node->SetMultiplier(4.0);
        node->SetSmoothness(0.0);

        //node->SetMultiplier(4.0);
        //node->SetSmoothness(0.0);
        scat = PointScatter3d<T>(cx, cy, cz, width, height, depth, seed);
    }
    void set_layer_count(int count) {
        layer_count = count;
        node->SetMultiplier((float)count/ 2.0);
    }
    void reposition(T cx, T cy, T cz, T width, T height, T depth, long unsigned int new_seed=0) {
        if (new_seed != 0) {
            seed = new_seed;
        }
        scat = PointScatter3d<T>(cx, cy, cz, width, height, depth, seed);
    }
    MapData<T> next(){
        auto v = scat.next();
        //std::cout << v;
        auto terrace = node->GenSingle3D(v.x * width_scale, v.y * height_scale, depth_scale* v.z, seed);
        terrace *= layer_count/2.0;
        terrace += layer_count/2.0;
        int layer = floor(terrace) ;
        return MapData<T> {v.x, v.y, v.z, layer};
    }
};


namespace py = pybind11;

PYBIND11_MODULE(sbs_native, m)
{
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: sbs_native

        .. autosummary::
           :toctree: _generate

           add
           subtract
    )pbdoc";

    

      py::class_<MapData<double>>(m, "MapData")
        .def_readonly("x", &MapData<double>::x)
        .def_readonly("y", &MapData<double>::y)
        .def_readonly("z", &MapData<double>::z)
        .def_readonly("terrain", &MapData<double>::terrain)
        ;
    py::class_<MapGenerator<double>>(m, "MapGenerator")
       .def(py::init<double, double, double, double, double, double, unsigned long int>())
       .def_readwrite("width_scale", &MapGenerator<double>::width_scale)
       .def_readwrite("height_scale", &MapGenerator<double>::height_scale)
       .def_readwrite("depth_scale", &MapGenerator<double>::depth_scale)
       .def("next", &MapGenerator<double>::next);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}


// // std:: cout << terrace;
// //std::cout << ' ';
// if (terrace <= -3.0 / 4.0)
// {
//     //  std::cout << "Celestial Body";
// }
// else if (terrace <= -2.0 / 4.0)
// {
//     //std::cout << "Far Nebula";
// }
// else if (terrace <= -1.0 / 4.0)
// {
//     //std::cout << "Enemy Hideout";
// }
// else if (terrace <= 0)
// {
//     //std::cout << "Far Asteroid";
// }
// else if (terrace <= 1.0 / 2.0)
// {
//     //std::cout << "Near Nebula";
// }
// else if (terrace <= 2.0 / 4.0)
// {
//     // std::cout << "Near asteroid";
// }
// else if (terrace <= 3.0 / 4.0)
// {
//     // std::cout << "Friendly Support";
// }
// else if (terrace <= 4.0)
// {
//     // std::cout << "Station";
// }
// // if (i % 3 == 2)
// //     std::cout << '\n';
// // else
// //     std::cout << ' ';
// }
