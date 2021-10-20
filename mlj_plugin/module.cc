// Copyright (c) 2009-2021 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.

// Include the defined classes that are to be exported to python
#include "mLJPotential.h"
#include "hoomd/md/PotentialPair.h"
#include "hoomd/ForceCompute.h"
#include "hoomd/Compute.h"

#include <pybind11/pybind11.h>
// #include <hoomd/extern/pybind/include/pybind11/pybind11.h>
namespace py = pybind11;

// #define ENABLE_HIP // to test

#ifdef ENABLE_HIP
#include "hoomd/md/PotentialPairGPU.h"
#include "mLJDriverPotentialPairGPU.cuh"
#endif

// specify the python module. Note that the name must explicitly match the PROJECT() name provided
// in CMakeLists (with an underscore in front)
PYBIND11_MODULE(_mlj_plugin, m)
    {
    export_PotentialPair<PotentialPairmLJ>(m, "PotentialPairmLJ");
#ifdef ENABLE_HIP
    export_PotentialPairGPU<PotentialPairmLJGPU, PotentialPairmLJ>(m, "PotentialPairmLJGPU");
#endif
    export_Compute(m);
    export_ForceCompute(m);
    }
