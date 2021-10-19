// Copyright (c) 2009-2021 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.

// Include the defined classes that are to be exported to python
#include "mLJPotential.h"
#include "hoomd/md/PotentialPair.h"
#include "hoomd/md/PotentialPairGPU.h"

#include <pybind11/pybind11.h>

// #define ENABLE_HIP // to test

// specify the python module. Note that the name must explicitly match the PROJECT() name provided
// in CMakeLists (with an underscore in front)
PYBIND11_MODULE(mLJ_plugin, m)
    {
    export_PotentialPair<PotentialPairmLJ>(m, "PotentialPairmLJ");

#ifdef ENABLE_HIP
    export_PotentialPairGPU<PotentialPairmLJGPU, PotentialPairmLJ>(m, "PotentialPairmLJGPU");
#endif
    }
