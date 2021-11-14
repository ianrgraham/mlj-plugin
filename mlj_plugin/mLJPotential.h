
#ifndef __MLJ_PAIR_POTENTIAL_H__
#define __MLJ_PAIR_POTENTIAL_H__

#include "EvaluatorPairmLJ.h"
#include "hoomd/md/PotentialPair.h"

// #define ENABLE_HIP // to test

#ifdef ENABLE_HIP
#include "hoomd/md/PotentialPairGPU.h"
#include "mLJDriverPotentialPairGPU.cuh"
#endif

#ifdef __HIPCC__
#error This header cannot be compiled by nvcc
#endif

//! Pair potential force compute for mlj forces

typedef PotentialPair<EvaluatorPairmLJ> PotentialPairmLJ;

#ifdef ENABLE_HIP
//! Pair potential force compute for lj forces on the GPU
typedef PotentialPairGPU<EvaluatorPairmLJ, gpu_compute_mljtemp_forces>
    PotentialPairmLJGPU;
#endif

#endif // _MLJ_PAIR_POTENTIAL_H__