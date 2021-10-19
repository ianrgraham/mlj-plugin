# Copyright (c) 2009-2021 The Regents of the University of Michigan
# This file is part of the HOOMD-blue project, released under the BSD 3-Clause
# License.

"""Pair potentials."""

import copy
import warnings

import hoomd
from hoomd.md import _md
from hoomd.md import force
from hoomd.md.nlist import NList
from hoomd.data.parameterdicts import ParameterDict, TypeParameterDict
from hoomd.data.typeparam import TypeParameter
import numpy as np
from hoomd.data.typeconverter import (OnlyFrom, OnlyTypes, nonnegative_real)
import hoomd.md.pair as _pair

validate_nlist = OnlyTypes(NList)

class mLJ(_pair.Pair):
    r"""Modified Lennard-Jones pair potential.

    Args:
        nlist (`hoomd.md.nlist.NList`): Neighbor list.
        default_r_cut (float): Default cutoff radius :math:`[\mathrm{length}]`.
        default_r_on (float): Default turn-on radius :math:`[\mathrm{length}]`.
        mode (str): Energy shifting/smoothing mode.

    `mLJ` specifies that a modified Lennard-Jones pair potential should be
    applied between every non-excluded particle pair in the simulation.

    .. math::
        :nowrap:

        \begin{eqnarray*}
        V_{\mathrm{LJ}}(r)  = & 4 \varepsilon \left[ \left(
        \frac{\sigma}{r} \right)^{12} - \left( \frac{\sigma}{r}
        \right)^{6} \right]; & r < r_{\mathrm{cut}} \\
        = & 0; & r \ge r_{\mathrm{cut}} \\
        \end{eqnarray*}

    See `Pair` for details on how forces are calculated and the available
    energy shifting and smoothing modes.

    .. py:attribute:: params

        The mLJ potential parameters. The dictionary has the following keys:

        * ``epsilon`` (`float`, **required**) -
          energy parameter :math:`\varepsilon` :math:`[\mathrm{energy}]`
        * ``sigma`` (`float`, **required**) -
          particle size :math:`\sigma` :math:`[\mathrm{length}]`
        * ``delta`` (`float`, **required**) -
          particle size :math:`\sigma` :math:`[\mathrm{length}]`

        Type: `TypeParameter` [`tuple` [``particle_type``, ``particle_type``],
        `dict`]

    Example::

        nl = nlist.Cell()
        lj = pair.mLJ(nl, default_r_cut=2.5)
        lj.params[('A', 'A')] = {'sigma': 1.0, 'epsilon': 1.0, 'delta': 0.25}
        lj.r_cut[('A', 'B')] = 2.5
    """
    _cpp_class_name = "PotentialPairmLJ"

    def __init__(self, nlist, default_r_cut=None, default_r_on=0., mode='none'):
        super().__init__(nlist, default_r_cut, default_r_on, mode)
        params = TypeParameter(
            'params', 'particle_types',
            TypeParameterDict(epsilon=float, sigma=float, delta=float, len_keys=2))
        self._add_typeparam(params)