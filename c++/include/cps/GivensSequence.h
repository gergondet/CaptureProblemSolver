#pragma once
/* Copyright 2018 CNRS-AIST JRL, CNRS-UM LIRMM
 *
 * This file is part of CPS.
 *
 * CPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CPS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with CPS.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <vector>

#include <cps/cps_api.h>
#include <cps/Givens.h>

namespace cps
{
  /** A sequence of Givens rotations G_0*G_1*...
    *
    * Note: it is ok to derive publicly from std::vector here as we do not have
    * any additional data.
    */
  class CPS_DLLAPI GivensSequence final : public std::vector<Givens>
  {
  public:
    using std::vector<Givens>::vector;

    /** M = G_{k-1}^T G_{k-2}^T .... G_0^T M*/
    template<typename Derived>
    void applyTo(const Eigen::MatrixBase<Derived>& M) const;

    /** Computes M = M * G_0 G_1 ... G_{k-1}*/
    template<typename Derived>
    void applyOnTheRightTo(const Eigen::MatrixBase<Derived>& M) const;

    /** Perform G.extend(incr) on all Givens rotations in the sequence*/
    void extend(int incr);

    /** Return the corresponding nxn orthogonal matrix
      *
      * Use only for debugging purposes.
      */
    Eigen::MatrixXd matrix(int n);
  };


  template<typename Derived>
  inline void GivensSequence::applyTo(const Eigen::MatrixBase<Derived>& M) const
  {
    for (const auto& G : *this)
      G.applyTo(M);
  }

  template<typename Derived>
  inline void GivensSequence::applyOnTheRightTo(const Eigen::MatrixBase<Derived>& M) const
  {
    for (const auto& G : *this)
      G.applyOnTheRightTo(M);
  }
}
