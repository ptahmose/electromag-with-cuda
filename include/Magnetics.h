/*
 * Copyright (C) 2010 - Alexandru Gagniuc - <mr.nuke.me@gmail.com>
 * This file is part of ElectroMag.
 *
 * ElectroMag is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ElectroMag is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with ElectroMag.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _MAGNETICS_H
#define _MAGNETICS_H

#include "Electrodynamics.h"

namespace magnetic
{
#define magneto_k 1E-7;     // miu_0 / (4 * pi)

/**
 * \brief Returns the partial magnetic field generated by a moving point charge
 */
template<class T>
inline Vector3<T> PartField(
    electro::dynamicPointCharge<T> charge,
    Vector3<T> point)
{
    Vector3<T> r = vec3(point, charge.staticProp.position); // 3 FLOP
    T lenSq = vec3LenSq(r);                                 // 5 FLOP
    return charge.staticProp.magnitude * magneto_k
           * vec3Cross(charge.velocity, r)  // 12 FLOP (3 vecMul, 9 cross)
           / (lenSq * sqrt(lenSq));          // 3 FLOP (1 mul, 1 div, 1 sqrt)
    // TOTAL: 23 FLOPs
}

#define magneticPartFieldFLOP 23

/**
 * \brief Operates on the inverse square vector to give the magnetic field
 */
template<class T>
inline Vector3<T> PartFieldOp(
    /// [in] The velocity vector of the particle generating the magnetic field
    Vector3<T> srcVelocity,
    /// [in] The charge on the particle generating the field
    T srcCharge,
    /// [in] The inverse square vector to the point where the field is of
    ///     interest
    Vector3<T> rInvSq
)
{
    return srcCharge * magneto_k                    // 1 FLOP (mul)
           * vec3Cross(srcVelocity, rInvSq);    // 12 FLOP (3 vecMul, 9 cross)
    // TOTAL: 13 FLOPs
}

#define magneticPartFieldOpFLOP 13

template<class T>
inline Vector3<T> PartFieldOp(
    /// [in] The velocity vector of the particle generating the magnetic field
    Vector3<T> srcVelocity,
    /// [in] The charge on the particle generating the field
    T srcCharge,
    /// [in] The inverse square vector to the point where the field is of
    ///     interest
    Vector3<T> rInvSq,
    /// [in,opt] The magnetic constant; useful when T is a vector type
    T magnetoK
)
{
    return srcCharge * magnetoK                 // 1 FLOP (mul)
           * vec3Cross(srcVelocity, rInvSq);    // 12 FLOP (3 vecMul, 9 cross)
    // TOTAL: 13 FLOPs
}


/**
 * \brief Returns the magnetic force on a point charge moving in a magnetic
 * \brief field
 */
template<class T>
inline Vector3<T> Force(electro::dynamicPointCharge<T> charge, Vector3<T> B)
{
    return charge.staticProp.magnitude * vec3Cross(charge.velocity, B);
    // 10 FLOP (1 mul, 9 cross)

    // TOTAL: 10 FLOPs
}
#define magneticForceFLOP 10

}//namespace magnetic

#endif  /* _MAGNETICS_H */

