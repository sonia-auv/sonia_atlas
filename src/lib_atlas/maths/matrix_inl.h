/**
 * \file	matrix_inl.h
 * \author	Thibaut Mattio <thibaut.mattio@gmail.com>
 * \date	20/02/2016
 *
 * \copyright Copyright (c) 2015 S.O.N.I.A. All rights reserved.
 *
 * \section LICENSE
 *
 * This file is part of S.O.N.I.A. software.
 *
 * S.O.N.I.A. software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * S.O.N.I.A. software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with S.O.N.I.A. software. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIB_ATLAS_MATHS_MATRIX_H_
#error This file may only be included matrix.h
#endif  // LIB_ATLAS_MATHS_MATRIX_H_

#include <stdexcept>

using std::cos;
using std::sin;
using std::atan2;
using std::asin;
using std::pow;
using std::sqrt;

namespace atlas {

//------------------------------------------------------------------------------
//
ATLAS_INLINE Eigen::Quaterniond RotToQuat(const Eigen::Matrix3d &m) {
  Eigen::Matrix3d r =
      m + (Eigen::Matrix3d::Identity(3, 3) - m * m.transpose()) * 0.5 * m;
  auto m1 = 1 + r(0, 0) + r(1, 1) + r(2, 2);
  auto m2 = 1 + r(0, 0) - r(1, 1) - r(2, 2);
  auto m3 = 1 - r(0, 0) + r(1, 1) - r(2, 2);
  auto m4 = 1 - r(0, 0) - r(1, 1) + r(2, 2);

  Eigen::Quaterniond b;

  if ((m1 > m2) && (m1 > m3) && (m1 > m4)) {
    if (m1 > 0) {
      b.w() = (0.5 * std::sqrt(m1));
      b.x() = (r(2, 1) - r(1, 2)) / (4 * b.w());
      b.y() = (r(0, 2) - r(2, 0)) / (4 * b.w());
      b.z() = (r(1, 0) - r(0, 1)) / (4 * b.w());
      b.normalize();
    } else {
      throw std::runtime_error("M1 won under 0.");
    }
  } else if ((m2 > m3) && (m2 > m4)) {
    if (m2 > 0) {
      b.x() = 0.5 * std::sqrt(m2);
      b.y() = (r(1, 0) + r(0, 1)) / (4 * b.x());
      b.z() = (r(2, 0) + r(0, 2)) / (4 * b.x());
      b.w() = (r(2, 1) - r(1, 2)) / (4 * b.x());
      b.normalize();
    } else {
      throw std::runtime_error("M2 won under 0.");
    }
  } else if (m3 > m4) {
    if (m3 > 0) {
      b.y() = 0.5 * std::sqrt(m3);
      b.x() = (r(0, 1) + r(1, 0)) / (4 * b.y());
      b.z() = (r(2, 1) + r(1, 2)) / (4 * b.y());
      b.w() = (r(0, 2) - r(2, 0)) / (4 * b.y());
      b.normalize();
    } else {
      throw std::runtime_error("M3 won under 0.");
    }
  } else {
    if (m4 > 0) {
      b.z() = 0.5 * std::sqrt(m4);
      b.x() = (r(0, 2) + r(2, 0)) / (4 * b.z());
      b.y() = (r(1, 2) + r(2, 1)) / (4 * b.z());
      b.w() = (r(1, 0) - r(0, 1)) / (4 * b.z());
      b.normalize();
    } else {
      throw std::runtime_error("M4 won under 0.");
    }
  }
  return b;
}

//------------------------------------------------------------------------------
//
ATLAS_INLINE Eigen::Matrix3d QuatToRot(const Eigen::Quaterniond &b) {
  Eigen::Matrix<double, 3, 1> bv = Eigen::Matrix<double, 3, 1>::Zero();

  if (b.norm() != 0) {
    decltype(b) b_norm = b.normalized();
    auto w = b_norm.w();
    bv(0) = b_norm.x();
    bv(1) = b_norm.y();
    bv(2) = b_norm.z();

    Eigen::Matrix<double, 3, 3> bc;
    bc(0, 0) = 0;
    bc(0, 1) = -bv(2);
    bc(0, 2) = bv(1);
    bc(1, 0) = bv(2);
    bc(1, 1) = 0;
    bc(1, 2) = -bv(0);
    bc(2, 0) = -bv(1);
    bc(2, 1) = bv(0);
    bc(2, 2) = 0;

    return (w * w - bv.transpose() * bv) * Eigen::Matrix3d::Identity(3, 3) +
           2 * (bv * bv.transpose()) + 2 * w * bc;
  } else {
    throw std::invalid_argument("Norm of the Quaternion is 0.");
  }
}

//------------------------------------------------------------------------------
//
ATLAS_INLINE Eigen::Matrix3d SkewMatrix(const Eigen::Vector3d &v)
    ATLAS_NOEXCEPT {
  Eigen::Matrix3d m = Eigen::Matrix3d::Zero();
  m(0, 1) = -v(2);
  m(1, 0) = v(2);
  m(0, 2) = v(1);
  m(2, 0) = -v(1);
  m(1, 2) = -v(0);
  m(2, 1) = v(0);
  return m;
}

//------------------------------------------------------------------------------
//
ATLAS_INLINE Eigen::Vector3d QuatToEuler(const Eigen::Quaterniond &b)
    ATLAS_NOEXCEPT {
  Eigen::Quaterniond q = NormalizeQuat(b);

  // Cap all inputs to asin to 1, since values >1 produce complex results
  auto asin_input = -2*(q.x()*q.z()-q.w()*q.y());
  asin_input = asin_input > 1 ? 1 : asin_input;


  auto yaw = atan2( 2*(q.x()*q.y()+q.w()*q.z()), q.w()*q.w() + q.x()*q.x() -
      q.y()*q.y() - q.z()*q.z());
  auto pitch = asin(asin_input);
  auto roll = atan2(2*(q.y()*q.z()+q.w()*q.x()), q.w()*q.w() - q.x()*q.x() -
      q.y()*q.y() + q.z()*q.z());
  return Eigen::Vector3d(roll, pitch, yaw);
}

//------------------------------------------------------------------------------
//
ATLAS_INLINE Eigen::Quaterniond NormalizeQuat(const Eigen::Quaterniond &b)
ATLAS_NOEXCEPT {
  double n = b.x()*b.x() + b.y()*b.y() + b.z()*b.z() + b.w()*b.w();

  if (n == 1)
    return b;

  n = 1.0f / sqrt(n);
  auto normalized_b = b;
  normalized_b.w() = b.w() * n;
  normalized_b.x() = b.x() * n;
  normalized_b.y() = b.y() * n;
  normalized_b.z() = b.z() * n;
  return normalized_b;

}

//------------------------------------------------------------------------------
//
ATLAS_INLINE Eigen::Matrix3d EulerToRot(double roll, double pitch,
                                        double yaw) ATLAS_NOEXCEPT {
  Eigen::Matrix3d r_b_n;
  r_b_n(0, 0) = cos(pitch) * cos(yaw);
  r_b_n(0, 1) = sin(roll) * sin(pitch) * cos(yaw) - cos(roll) * sin(yaw);
  r_b_n(0, 2) = cos(roll) * sin(pitch) * cos(yaw) + sin(roll) * sin(yaw);
  r_b_n(1, 0) = cos(pitch) * sin(yaw);
  r_b_n(1, 1) = sin(roll) * sin(pitch) * sin(yaw) + cos(roll) * cos(yaw);
  r_b_n(1, 2) = cos(roll) * sin(pitch) * sin(yaw) - sin(roll) * cos(yaw);
  r_b_n(2, 0) = -sin(pitch);
  r_b_n(2, 1) = sin(roll) * cos(pitch);
  r_b_n(2, 2) = cos(roll) * cos(pitch);
  return r_b_n.transpose();
}

//------------------------------------------------------------------------------
//
ATLAS_INLINE Eigen::Quaterniond ExactQuat(const Eigen::Vector3d &w_ib_b,
                                          double dt,
                                          const Eigen::Quaterniond &b_k) {
  // Euqation 10.24 - Farrell (w_in_b assumed to be 0)
  decltype(w_ib_b) w_bn_b = -w_ib_b;

  // Integration of the quaternion derivative (Equation D.36 - Farrell)
  decltype(w_bn_b) w = 0.5 * w_bn_b * dt;
  double n = w.norm();

  if (std::abs(n) > 1) {
    throw std::runtime_error("Integrated angle too large.");
  }

  auto skew_w = SkewMatrix(w);
  Eigen::Matrix4d w_m;
  w_m(0, 0) = 0;
  w_m(0, 1) = -w(0);
  w_m(0, 2) = -w(1);
  w_m(0, 3) = -w(2);
  w_m(1, 0) = w(0);
  w_m(1, 1) = skew_w(0, 0);
  w_m(1, 2) = skew_w(0, 1);
  w_m(1, 3) = skew_w(0, 2);
  w_m(2, 0) = w(1);
  w_m(2, 1) = skew_w(1, 0);
  w_m(2, 2) = skew_w(1, 1);
  w_m(2, 3) = skew_w(1, 2);
  w_m(3, 0) = w(2);
  w_m(3, 1) = skew_w(2, 0);
  w_m(3, 2) = skew_w(2, 1);
  w_m(3, 3) = skew_w(2, 2);

  double sinw;
  // Handle singularity
  if (n == 0) {
    sinw = 1;
  } else {
    sinw = std::sin(n) / n;
  }

  // Cannot multiply the matrix by a quaternion in Eigen, we are forced to do
  // a conversion before...
  Eigen::Matrix<double, 4, 1> b;
  b(0) = b_k.w();
  b(1) = b_k.x();
  b(2) = b_k.y();
  b(3) = b_k.z();

  // Equation D.36 - Farrell
  Eigen::Matrix<double, 4, 1> exact_b =
      (std::cos(n) * Eigen::Matrix<double, 4, 4>::Identity(4, 4) + sinw * w_m) *
      b;
  return Eigen::Quaterniond(exact_b(0), exact_b(1), exact_b(2), exact_b(3));
}

}  // namespace atlas
