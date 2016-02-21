/**
 * \file	matrix.h
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
#define LIB_ATLAS_MATHS_MATRIX_H_

#include <math.h>
#include <eigen3/Eigen/Eigen>
#include <lib_atlas/macros.h>

namespace atlas {

/**
 * Convert a rotation matrix to a quaternion
 */
Eigen::Matrix3d RotToQuat(const Eigen::Matrix3d &m) ATLAS_NOEXCEPT;

}  // namespace atlas

#include <lib_atlas/maths/matrix_inl.h>

#endif  // LIB_ATLAS_MATHS_MATRIX_H_
