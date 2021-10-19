// Copyright (c) 2009-2021 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.

// Maintainer: joaander

// Edited Oct 19 2021 Ian Graham
// Taken originally from the HOOMD v3 md.pair module. Here we implement a slighly modified version
// of the LJ potential with a tunable well width, controlled by a parameter \Delta

#ifndef __PAIR_EVALUATOR_mLJ_H__
#define __PAIR_EVALUATOR_mLJ_H__

#ifndef __HIPCC__
#include <string>
#endif

#include "hoomd/HOOMDMath.h"

/*! \file EvaluatorPairmLJ.h
    \brief Defines the pair evaluator class for mLJ potentials
    \details Modified version of the LJ potential, taken directly from the HOOMD v3 source code
*/

// need to declare these class methods with __device__ qualifiers when building in nvcc
// DEVICE is __host__ __device__ when included in nvcc and blank when included into the host
// compiler
#ifdef __HIPCC__
#define DEVICE __device__
#define HOSTDEVICE __host__ __device__
#else
#define DEVICE
#define HOSTDEVICE
#endif

//! Original to the LJ example that this plugin was constructed from
//! Class for evaluating the LJ pair potential
/*! <b>General Overview</b>

    EvaluatorPairLJ is a low level computation class that computes the LJ pair potential V(r). As
   the standard MD potential, it also serves as a well documented example of how to write additional
   pair potentials. "Standard" pair potentials in hoomd are all handled via the template class
   PotentialPair. PotentialPair takes a potential evaluator as a template argument. In this way, all
   the complicated data management and other details of computing the pair force and potential on
   every single particle is only written once in the template class and the difference V(r)
   potentials that can be calculated are simply handled with various evaluator classes. Template
   instantiation is equivalent to inlining code, so there is no performance loss.

    In hoomd, a "standard" pair potential is defined as V(rsq, rcutsq, params, di, dj, qi, qj),
   where rsq is the squared distance between the two particles, rcutsq is the cutoff radius at which
   the potential goes to 0, params is any number of per type-pair parameters, di, dj are the
   diameters of particles i and j, and qi, qj are the charges of particles i and j respectively.

    Diameter and charge are not always needed by a given pair evaluator, so it must provide the
   functions needsDiameter() and needsCharge() which return boolean values signifying if they need
   those quantities or not. A false return value notifies PotentialPair that it need not even load
   those values from memory, boosting performance.

    If needsDiameter() returns true, a setDiameter(Scalar di, Scalar dj) method will be called to
   set the two diameters. Similarly, if needsCharge() returns true, a setCharge(Scalar qi, Scalar
   qj) method will be called to set the two charges.

    All other arguments are common among all pair potentials and passed into the constructor.
   Coefficients are handled in a special way: the pair evaluator class (and PotentialPair) manage
   only a single parameter variable for each type pair. Pair potentials that need more than 1
   parameter can specify that their param_type be a compound structure and reference that. For
   coalesced read performance on G200 GPUs, it is highly recommended that param_type is one of the
   following types: Scalar, Scalar2, Scalar4.

    The program flow will proceed like this: When a potential between a pair of particles is to be
   evaluated, a PairEvaluator is instantiated, passing the common parameters to the constructor and
   calling setDiameter() and/or setCharge() if need be. Then, the evalForceAndEnergy() method is
   called to evaluate the force and energy (more on that later). Thus, the evaluator must save all
   of the values it needs to compute the force and energy in member variables.

    evalForceAndEnergy() makes the necessary computations and sets the out parameters with the
   computed values. Specifically after the method completes, \a force_divr must be set to the value
    \f$ -\frac{1}{r}\frac{\partial V}{\partial r}\f$ and \a pair_eng must be set to the value \f$
   V(r) \f$ if \a energy_shift is false or \f$ V(r) - V(r_{\mathrm{cut}}) \f$ if \a energy_shift is
   true.

    A pair potential evaluator class is also used on the GPU. So all of its members must be declared
   with the DEVICE keyword before them to mark them __device__ when compiling in nvcc and blank
   otherwise. If any other code needs to diverge between the host and device (i.e., to use a special
   math function like __powf on the device), it can similarly be put inside an ifdef __HIPCC__
   block.

    <b>LJ specifics</b>

    EvaluatorPairLJ evaluates the function:
    \f[ V_{\mathrm{LJ}}(r) = 4 \varepsilon \left[ \left( \frac{\sigma}{r} \right)^{12} -
                                            \left( \frac{\sigma}{r} \right)^{6} \right] \f]
    broken up as follows for efficiency
    \f[ V_{\mathrm{LJ}}(r) = r^{-6} \cdot \left( 4 \varepsilon \sigma^{12} \cdot r^{-6} -
                                            4 \varepsilon \sigma^{6} \right) \f]
    . Similarly,
    \f[ -\frac{1}{r} \frac{\partial V_{\mathrm{LJ}}}{\partial r} = r^{-2} \cdot r^{-6} \cdot
            \left( 12 \cdot 4 \varepsilon \sigma^{12} \cdot r^{-6} - 6 \cdot 4 \varepsilon
   \sigma^{6} \right) \f]

    The LJ potential does not need diameter or charge. Two parameters are specified and stored in a
   Scalar2. \a lj1 is placed in \a params.x and \a lj2 is in \a params.y.

    These are related to the standard lj parameters sigma and epsilon by:
    - \a lj1 = 4.0 * epsilon * pow(sigma,12.0)
    - \a lj2 = 4.0 * epsilon * pow(sigma,6.0);

*/
class EvaluatorPairmLJ
    {
    public:
    //! Define the parameter type used by this pair potential evaluator
    struct param_type
        {
        Scalar lj1;
        Scalar lj2;
        Scalar dlt;

        DEVICE void load_shared(char*& ptr, unsigned int& available_bytes) { }

        HOSTDEVICE void allocate_shared(char*& ptr, unsigned int& available_bytes) const { }

#ifdef ENABLE_HIP
        //! Set CUDA memory hints
        void set_memory_hint() const
            {
            // default implementation does nothing
            }
#endif

#ifndef __HIPCC__
        param_type() : lj1(0), lj2(0), dlt(0){ }

        param_type(pybind11::dict v, bool managed = false)
            {
            auto sigma(v["sigma"].cast<Scalar>());
            auto epsilon(v["epsilon"].cast<Scalar>());
            auto delta(v["delta"].cast<Scalar>());
            auto dsigma = sigma - delta/pow(2.0, 1. / 6.);
            lj1 = 4.0 * epsilon * pow(dsigma, 12.0);
            lj2 = 4.0 * epsilon * pow(dsigma, 6.0);
            dlt = delta;
            }

        // this constructor facilitates unit testing
        param_type(Scalar sigma, Scalar epsilon, Scalar delta, bool managed = false)
            {
            auto dsigma = sigma - delta/pow(2.0, 1. / 6.);
            lj1 = 4.0 * epsilon * pow(dsigma, 12.0);
            lj2 = 4.0 * epsilon * pow(dsigma, 6.0);
            dlt = delta;
            }

        pybind11::dict asDict()
            {
            pybind11::dict v;
            auto sigma6 = lj1 / lj2;
            v["sigma"] = pow(sigma6, 1. / 6.) + dlt/pow(2.0, 1. / 6.);
            v["epsilon"] = lj2 / (sigma6 * 4);
            v["delta"] = dlt;
            return v;
            }
#endif
        }
#ifdef SINGLE_PRECISION
    __attribute__((aligned(8)));
#else
    __attribute__((aligned(16)));
#endif

    //! Constructs the pair potential evaluator
    /*! \param _rsq Squared distance between the particles
        \param _rcutsq Squared distance at which the potential goes to 0
        \param _params Per type pair parameters of this potential
    */
    DEVICE EvaluatorPairmLJ(Scalar _rsq, Scalar _rcutsq, const param_type& _params)
        : rsq(_rsq), rcutsq(_rcutsq), lj1(_params.lj1), lj2(_params.lj2), dlt(_params.dlt)
        {
        }

    //! LJ doesn't use diameter
    DEVICE static bool needsDiameter()
        {
        return false;
        }
    //! Accept the optional diameter values
    /*! \param di Diameter of particle i
        \param dj Diameter of particle j
    */
    DEVICE void setDiameter(Scalar di, Scalar dj) { }

    //! LJ doesn't use charge
    DEVICE static bool needsCharge()
        {
        return false;
        }
    //! Accept the optional diameter values
    /*! \param qi Charge of particle i
        \param qj Charge of particle j
    */
    DEVICE void setCharge(Scalar qi, Scalar qj) { }

    //! Evaluate the force and energy
    /*! \param force_divr Output parameter to write the computed force divided by r.
        \param pair_eng Output parameter to write the computed pair energy
        \param energy_shift If true, the potential must be shifted so that
        V(r) is continuous at the cutoff
        \note There is no need to check if rsq < rcutsq in this method.
        Cutoff tests are performed in PotentialPair.

        \return True if they are evaluated or false if they are not because
        we are beyond the cutoff
    */
    DEVICE bool evalForceAndEnergy(Scalar& force_divr, Scalar& pair_eng, bool energy_shift)
        {
        // compute the force divided by r in force_divr
        if (rsq < rcutsq && lj1 != 0)
            {
            // Next two lines are the only deliniation from the LJ potential
            // original: Scalar r2inv = Scalar(1.0) / rsq;
            Scalar rinv = Scalar(1.0) / (fast::sqrt(rsq) - dlt); 
            Scalar r2inv = rinv * rinv;
            
            Scalar r6inv = r2inv * r2inv * r2inv;
            force_divr = r2inv * r6inv * (Scalar(12.0) * lj1 * r6inv - Scalar(6.0) * lj2);

            pair_eng = r6inv * (lj1 * r6inv - lj2);

            if (energy_shift)
                {
                Scalar rcut2inv = Scalar(1.0) / rcutsq;
                Scalar rcut6inv = rcut2inv * rcut2inv * rcut2inv;
                pair_eng -= rcut6inv * (lj1 * rcut6inv - lj2);
                }
            return true;
            }
        else
            return false;
        }

#ifndef __HIPCC__
    //! Get the name of this potential
    /*! \returns The potential name.
     */
    static std::string getName()
        {
        return std::string("lj");
        }

    std::string getShapeSpec() const
        {
        throw std::runtime_error("Shape definition not supported for this pair potential.");
        }
#endif

    protected:
    Scalar rsq;    //!< Stored rsq from the constructor
    Scalar rcutsq; //!< Stored rcutsq from the constructor
    Scalar lj1;    //!< lj1 parameter extracted from the params passed to the constructor
    Scalar lj2;    //!< lj2 parameter extracted from the params passed to the constructor
    Scalar dlt;    //!< dlt parameter extracted from the params passed to the constructor
    };

#endif // __PAIR_EVALUATOR_mLJ_H__
