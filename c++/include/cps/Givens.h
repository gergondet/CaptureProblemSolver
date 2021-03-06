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

#include <Eigen/Jacobi>

#include <cps/cps_api.h>
#include <cps/defs.h>

namespace cps
{
  /** A dummy structure for dinstinguishing overloads.*/
  struct condensed_t {};

  /** Represent a Givens matrix G.*/
  class CPS_DLLAPI Givens
  {
  public:
    using Index = Eigen::DenseIndex;

    /** Default value: identity matrix*/
    Givens();

    /** Build a Givens rotation G with submatrix G([i,j],[i,j]) = [c s; -s c]
      * (Matlab notations)
      */
    Givens(Index i, Index j, double c, double s);
    /** Version with j = i+1. */
    Givens(Index i, double c, double s);

    /** Build a Givens rotation G such that 
    *                 | M(i,k) |   | x |
    * G([i,j][i,j])^T | M(j,k) | = | 0 |
    */
    template<typename Derived>
    Givens(const Eigen::MatrixBase<Derived>& M, Index i, Index j, Index k);
    /** Version with j=i+1. */
    template<typename Derived>
    Givens(const Eigen::MatrixBase<Derived>& M, Index i, Index k);

    /** Performs M = G^T M
      *
      * Don't let the const ref on M fool you: it is a (recommended) trick to
      * accept temporaries such as blocks. Internally, the const is cast away.
      *
      * We don't use Eigen::Ref here because this function will be used with
      * many small fixed-size matrices, and we want the compiler to take 
      * advantage of that.
      */
    template<typename Derived>
    void applyTo(const Eigen::MatrixBase<Derived>& M) const;

    /** Same as above for M with row size 2, and we ignore i and j.*/
    template<typename Derived>
    void applyTo(const Eigen::MatrixBase<Derived>& M, condensed_t) const;

    /** Computes M = M * G*/
    template<typename Derived>
    void applyOnTheRightTo(const Eigen::MatrixBase<Derived>& M) const;

    /** Add incr to i and j.
      * This is in particular useful if G was computed for submatrix S starting
      * at row incr of a matrix M. In this case with G' the result of G.extend(incr)
      * G^T S and G'^T M have the same rows changed.
      */
    void extend(Index incr);

  private:
    Index i_;
    Index j_;
    /** We store directily the transpose matrix*/
    Eigen::JacobiRotation<double> Jt_;
  };


  template<typename Derived>
  inline Givens::Givens(const Eigen::MatrixBase<Derived>& M, Index i, Index j, Index k)
    : i_(i), j_(j)
  {
    Eigen::JacobiRotation<double> G;
    G.makeGivens(M(i, k), M(j, k));
    Jt_ = G.transpose();
  }

  template<typename Derived>
  inline Givens::Givens(const Eigen::MatrixBase<Derived>& M, Index i, Index k)
    : Givens(M, i, i + 1, k)
  {
  }

  template<typename Derived>
  inline void Givens::applyTo(const Eigen::MatrixBase<Derived>& M) const
  {
    //recall that Jt_ stores the transpose of the rotation, so we don't need to transpose again.
    const_cast<Eigen::MatrixBase<Derived>&>(M).applyOnTheLeft(i_, j_, Jt_);
  }

  template<typename Derived>
  inline void Givens::applyTo(const Eigen::MatrixBase<Derived>& M, condensed_t) const
  {
    static_assert(Eigen::internal::traits<Derived>::RowsAtCompileTime == 2, "This methods works only for matrices with row size 2");
    //FIXME: shall we specialized for fixed size matrix?
    const_cast<Eigen::MatrixBase<Derived>&>(M).applyOnTheLeft(0, 1, Jt_);
  }

  template<typename Derived>
  inline void Givens::applyOnTheRightTo(const Eigen::MatrixBase<Derived>& M) const
  {
    const_cast<Eigen::MatrixBase<Derived>&>(M).applyOnTheRight(i_, j_, Jt_.transpose());
  }
}
