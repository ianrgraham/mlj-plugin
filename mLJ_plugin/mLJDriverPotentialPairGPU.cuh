
#include "hoomd/md/PotentialPairGPU.cuh"
#include "EvaluatorPairmLJ.h"

hipError_t __attribute__((visibility("default")))
gpu_compute_mljtemp_forces(const pair_args_t& pair_args,
                          const EvaluatorPairmLJ::param_type* d_params);