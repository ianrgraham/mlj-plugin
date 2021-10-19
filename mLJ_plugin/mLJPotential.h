
#ifndef __MLJ_PAIR_POTENTIAL_H__
#define __MLJ_PAIR_POTENTIAL_H__

#include "hoomd/md/PotentialPair.h"
#include "EvaluatorPairmLJ.h"

// #define ENABLE_HIP // to test

#ifdef ENABLE_HIP
#include "hoomd/md/AllDriverPotentialPairGPU.cuh"
#include "hoomd/md/PotentialPairGPU.h"
#endif

#ifdef __HIPCC__
#error This header cannot be compiled by nvcc
#endif

//! Pair potential force compute for lj forces

typedef PotentialPair<EvaluatorPairmLJ> PotentialPairmLJ;

#ifdef ENABLE_HIP
//! Pair potential force compute for lj forces on the GPU
typedef PotentialPairGPU<EvaluatorPairmLJ, gpu_compute_ljtemp_forces> PotentialPairmLJGPU;
#endif

#endif // __PAIR_POTENTIALS_H__