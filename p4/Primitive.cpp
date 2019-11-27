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
#include "Intersection.h"

namespace cg
{ // begin namespace cg

  bool
    Primitive::intersect(const Ray& ray, Intersection& hit) const
  {
    if (_mesh == nullptr)
      return false;

    auto t = const_cast<Primitive*>(this)->transform();
    auto origin = t->worldToLocalMatrix().transform(ray.origin);
    auto D = t->worldToLocalMatrix().transformVector(ray.direction);
    Ray localRay{ origin, D };
    auto d = math::inverse(D.length());
    float tMin;
    float tMax;

    float localMin = math::Limits<float>::inf();

    //localRay.direction *= d;
    if (_mesh->bounds().intersect(localRay, tMin, tMax))
    {
      // TODO: mesh intersection

      //iterar dentro dos triangulugulu      
      auto triangles = _mesh->data().triangles;
      auto numTriangles = _mesh->data().numberOfTriangles;
      auto vertexArray = _mesh->data().vertices;
      for (int i = 0; i < numTriangles; i++)
      {

        auto p0 = vertexArray[triangles[i].v[0]];
        auto p1 = vertexArray[triangles[i].v[1]];
        auto p2 = vertexArray[triangles[i].v[2]];
        auto e1 = p1 - p0;// #1
        auto e2 = p2 - p0;// #2
        auto s1 = D.cross(e2);// #3
        auto s1e1 = s1.dot(e1);
        auto invd = 1 / s1e1;
        if (math::isZero(abs(invd)))// #4
        {
          continue; //5
        }

        auto s = origin - p0; // #6
        auto s2 = s.cross(e1);// #7
        auto t = s2.dot(e2) * invd;// #8
        if (t < 0)
        {
          continue;
        }

        auto b1 = s1.dot(s) * invd;// #9
        if (b1 < 0)
        {
          continue;
        }

        auto b2 = s2.dot(D) * invd;// #10
        if (b2 < 0)
        {
          continue;
        }

        if (b1 + b2 > 1)
        {
          continue;
        }
        auto td = t * d;
        if (td > localMin) continue;
        hit.triangleIndex = i;
        hit.distance = localMin = td;
        hit.p = vec3f{ 1 - b1 - b2, b1, b2 };

      }

      /*if (tMin >= ray.tMin && tMin <= ray.tMax)
      {
        distance = tMin * d;
        return true;
      }
      if (tMax >= ray.tMin && tMax <= ray.tMax)
      {
        distance = tMax * d;
        return true;
      }*/
    }
    return localMin != math::Limits<float>::inf();
  }

} // end namespace cg
