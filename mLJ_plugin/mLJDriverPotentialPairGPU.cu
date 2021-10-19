// Copyright (c) 2009-2021 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.

/*! \file mLJDriverPotentialPairGPU.cu
    \brief Defines the driver functions for computing all types of pair forces on the GPU
*/

#include "hoomd/md/AllDriverPotentialPairGPU.cuh"
#include "EvaluatorPairmLJ.h"
hipError_t gpu_compute_mljtemp_forces(const pair_args_t& pair_args,
                                     const EvaluatorPairmLJ::param_type* d_params)
    {
    return gpu_compute_pair_forces<EvaluatorPairmLJ>(pair_args, d_params);
    }