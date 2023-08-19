#include <pybind11/pybind11.h>
#include <string>
#include "map_data.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

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

    

      py::class_<MapData<float>>(m, "MapData")
        .def_readonly("x", &MapData<float>::x)
        .def_readonly("y", &MapData<float>::y)
        .def_readonly("z", &MapData<float>::z)
        //.def_readonly("terrain_raw", &MapData<float>::terrain_raw)
        //.def_readonly("sample_raw", &MapData<float>::sample_raw)
        .def_readonly("terrain", &MapData<float>::terrain)
        .def_readonly("sample_type", &MapData<float>::sample_type)
        ;
    py::class_<MapGenerator<float>>(m, "MapGenerator")
       .def(py::init<double, double, double, double, double, double, unsigned long int>())
       .def_readwrite("global_scale", &MapGenerator<float>::global_scale)
       .def_readwrite("width_scale", &MapGenerator<float>::width_scale)
       .def_readwrite("height_scale", &MapGenerator<float>::height_scale)
       .def_readwrite("depth_scale", &MapGenerator<float>::depth_scale)
       .def("__iter__", [](MapGenerator<float> &self)
             { return &self; })
       .def("__next__", &MapGenerator<float>::next);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}

