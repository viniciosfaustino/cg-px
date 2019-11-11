//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018 Orthrus Group.                               |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: Primitive.cpp
// ========
// Source file for primitive.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 30/10/2018

#include "Primitive.h"
#include "Transform.h"

namespace cg
{ // begin namespace cg

bool
Primitive::intersect(const Ray& ray, float& distance) const
{
  if (_mesh == nullptr)
    return false;

  auto t = const_cast<Primitive*>(this)->transform();
  Ray localRay{ray, t->worldToLocalMatrix()};
  auto d = math::inverse(localRay.direction.length());
  float tMin;
  float tMax;

  localRay.direction *= d;
  if (_mesh->bounds().intersect(localRay, tMin, tMax))
  {
    // TODO: mesh intersection
    if (tMin >= ray.tMin && tMin <= ray.tMax)
    {
      distance = tMin * d;
      return true;
    }
    if (tMax >= ray.tMin && tMax <= ray.tMax)
    {
      distance = tMax * d;
      return true;
    }
  }
  return false;
}

} // end namespace cg
