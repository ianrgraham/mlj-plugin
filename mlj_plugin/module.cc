// Copyright (c) 2009-2021 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.

// Include the defined classes that are to be exported to python
#include "mLJPotential.h"
#include "hoomd/md/PotentialPair.h"
// #include "hoomd/ForceCompute.h"
// #include "hoomd/Compute.h"

#include <pybind11/pybind11.h>
namespace py = pybind11;

// #define ENABLE_HIP // to test

#ifdef ENABLE_HIP
#include "hoomd/md/PotentialPairGPU.h"
#include "mLJDriverPotentialPairGPU.cuh"
#endif

// template<class T> void export_PotentialPair(pybind11::module& m, const std::string& name)
//     {
//     pybind11::class_<T, ForceCompute, std::shared_ptr<T>> potentialpair(m, name.c_str(), py::multiple_inheritance());
//     potentialpair
//         .def(pybind11::init<std::shared_ptr<SystemDefinition>, std::shared_ptr<NeighborList>>())
//         .def("setParams", &T::setParamsPython)
//         .def("getParams", &T::getParams)
//         .def("setRCut", &T::setRCutPython)
//         .def("getRCut", &T::getRCut)
//         .def("setROn", &T::setROnPython)
//         .def("getROn", &T::getROn)
//         .def_property("mode", &T::getShiftMode, &T::setShiftModePython)
//         .def("computeEnergyBetweenSets", &T::computeEnergyBetweenSetsPythonList)
//         .def("slotWriteGSDShapeSpec", &T::slotWriteGSDShapeSpec)
//         .def("connectGSDShapeSpec", &T::connectGSDShapeSpec);
//     }

// template<class T, class Base>
// void export_PotentialPairGPU(pybind11::module& m, const std::string& name)
//     {
//     pybind11::class_<T, Base, std::shared_ptr<T>>(m, name.c_str(), py::multiple_inheritance())
//         .def(pybind11::init<std::shared_ptr<SystemDefinition>, std::shared_ptr<NeighborList>>())
//         .def("setTuningParam", &T::setTuningParam);
//     }

// specify the python module. Note that the name must explicitly match the PROJECT() name provided
// in CMakeLists (with an underscore in front)
PYBIND11_MODULE(_mlj_plugin, m)
    {
    // auto hoomd = py::module_::import("hoomd._hoomd");
    // m.add_object("ForceCompute", hoomd.attr("ForceCompute"));
    // py::module_::import("hoomd.md._md");
    export_PotentialPair<PotentialPairmLJ>(m, "PotentialPairmLJ");
#ifdef ENABLE_HIP
    export_PotentialPairGPU<PotentialPairmLJGPU, PotentialPairmLJ>(m, "PotentialPairmLJGPU");
#endif
    }
