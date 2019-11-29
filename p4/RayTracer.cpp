//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2019 Orthrus Group.                         |
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
// OVERVIEW: RayTracer.cpp
// ========
// Source file for simple ray tracer.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 16/10/2019

#include "Camera.h"
#include "RayTracer.h"
#include <time.h>

using namespace std;

namespace cg
{ // begin namespace cg

  void
    printElapsedTime(const char* s, clock_t time)
  {
    printf("%sElapsed time: %.4f s\n", s, (float)time / CLOCKS_PER_SEC);
  }


  /////////////////////////////////////////////////////////////////////
  //
  // RayTracer implementation
  // =========
  RayTracer::RayTracer(Scene& scene, Camera* camera) :
    Renderer{ scene, camera },
    _maxRecursionLevel{ 6 },
    _minWeight{ MIN_WEIGHT }
  {
    // TODO: BVH
  }

  void
    RayTracer::render()
  {
    throw std::runtime_error("RayTracer::render() invoked");
  }

  inline float
    windowHeight(Camera* c)
  {
    if (c->projectionType() == Camera::Parallel)
      return c->height();
    return c->nearPlane() * tan(math::toRadians(c->viewAngle() * 0.5f)) * 2;

  }
  void
    RayTracer::renderImage(Image& image)
  {
    auto t = clock();
    const auto& m = _camera->cameraToWorldMatrix();

    // VRC axes
    _vrc.u = m[0];
    _vrc.v = m[1];
    _vrc.n = m[2];
    // init auxiliary mapping variables
    _W = image.width();
    _H = image.height();
    _Iw = math::inverse(float(_W));
    _Ih = math::inverse(float(_H));

    auto height = windowHeight(_camera);

    _W >= _H ? _Vw = (_Vh = height) * _W * _Ih : _Vh = (_Vw = height) * _H * _Iw;
    // init pixel ray
    _pixelRay.origin = _camera->transform()->position();
    _pixelRay.direction = -_vrc.n;
    _camera->clippingPlanes(_pixelRay.tMin, _pixelRay.tMax);
    _numberOfRays = _numberOfHits = 0;
    scan(image);
    printf("\nNumber of rays: %llu", _numberOfRays);
    printf("\nNumber of hits: %llu", _numberOfHits);
    printElapsedTime("\nDONE! ", clock() - t);
  }

  void
    RayTracer::setPixelRay(float x, float y)
    //[]---------------------------------------------------[]
    //|  Set pixel ray                                      |
    //|  @param x coordinate of the pixel                   |
    //|  @param y cordinates of the pixel                   |
    //[]---------------------------------------------------[]
  {
    auto p = imageToWindow(x, y);

    switch (_camera->projectionType())
    {
    case Camera::Perspective:
      _pixelRay.direction = (p - _camera->nearPlane() * _vrc.n).versor();
      break;

    case Camera::Parallel:
      _pixelRay.origin = _camera->transform()->position() + p;
      break;
    }
  }

  void
    RayTracer::scan(Image& image)
  {
    ImageBuffer scanLine{ _W, 1 };

    for (int j = 0; j < _H; j++)
    {
      auto y = (float)j + 0.5f;

      printf("Scanning line %d of %d\r", j + 1, _H);
      for (int i = 0; i < _W; i++)
        scanLine[i] = shoot((float)i + 0.5f, y);
      image.setData(0, j, scanLine);
    }
  }

  Color
    RayTracer::shoot(float x, float y)
    //[]---------------------------------------------------[]
    //|  Shoot a pixel ray                                  |
    //|  @param x coordinate of the pixel                   |
    //|  @param y cordinates of the pixel                   |
    //|  @return RGB color of the pixel                     |
    //[]---------------------------------------------------[]
  {
    // set pixel ray
    setPixelRay(x, y);

    // trace pixel ray
    Color color = trace(_pixelRay, 0, 1.0f);

    // adjust RGB color
    if (color.r > 1.0f)
      color.r = 1.0f;
    if (color.g > 1.0f)
      color.g = 1.0f;
    if (color.b > 1.0f)
      color.b = 1.0f;
    // return pixel color
    return color;
  }

  Color
    RayTracer::trace(const Ray& ray, uint32_t level, float weight)
    //[]---------------------------------------------------[]
    //|  Trace a ray                                        |
    //|  @param the ray                                     |
    //|  @param recursion level                             |
    //|  @param ray weight                                  |
    //|  @return color of the ray                           |
    //[]---------------------------------------------------[]
  {
    if (level > _maxRecursionLevel)
      return Color::black;
    _numberOfRays++;

    Intersection hit;

    return intersect(ray, hit) ? shade(ray, hit, level, weight) : background();
  }

  inline constexpr auto
    rt_eps()
  {
    return 1e-2f;
  }

  bool
    RayTracer::intersect(const Ray& ray, Intersection& hit)
    //[]---------------------------------------------------[]
    //|  Ray/object intersection                            |
    //|  @param the ray (input)                             |
    //|  @param information on intersection (output)        |
    //|  @return true if the ray intersects an object       |
    //[]---------------------------------------------------[]
  {
    hit.object = nullptr;
    hit.distance = ray.tMax;
    float distance = ray.tMax;
    auto scene = this->scene();
    auto it = scene->getScenePrimitiveIterator();
    auto end = scene->getScenePrimitiveEnd();
  
    for (; it != end; it++)
    {
      auto component = it->get();
      if (auto p = dynamic_cast<Primitive*>(component))
      {
        auto t = (p->sceneObject())->transform();
        auto origin = t->worldToLocalMatrix().transform(ray.origin);
        auto D = t->worldToLocalMatrix().transformVector(ray.direction);
        Ray localRay{ origin, D };
        origin = localRay.origin;
        D = localRay.direction;
        auto d = math::inverse(D.length());
        float tMin;
        float tMax;

        
        if (p->getbvh()->intersect(localRay, hit, d))
        {
          _numberOfHits++;
          if (distance > hit.distance)
          {
            distance = hit.distance;
            hit.object = p;
          }          
        }
      }
    }    
    return hit.object != nullptr;
  }

  Color
    RayTracer::shade(const Ray& ray, Intersection& hit, int level, float weight)
    //[]---------------------------------------------------[]
    //|  Shade a point P                                    |
    //|  @param the ray (input)                             |
    //|  @param information on intersection (input)         |
    //|  @param recursion level                             |
    //|  @param ray weight                                  |
    //|  @return color at point P                           |
    //[]---------------------------------------------------[]
  {
    // TODO: insert your code here

    auto triangles = hit.object->mesh()->data().triangles;
    auto normals = hit.object->mesh()->data().vertexNormals;

    auto n0 = normals[triangles[hit.triangleIndex].v[0]];
    auto n1 = normals[triangles[hit.triangleIndex].v[1]];
    auto n2 = normals[triangles[hit.triangleIndex].v[2]];

    auto N = hit.p[0] * n0 + hit.p[1] * n1 + hit.p[2] * n2;

    auto normalMatrix = mat3f(hit.object->sceneObject()->transform()->worldToLocalMatrix().transposed());
    N = normalMatrix.transform(N.versor());
    N = N.versor();
    auto p = ray.origin + hit.distance * ray.direction;
    p += rt_eps() * N;

    auto material = hit.object->sceneObject()->primitive()->material;
    auto Or = material.specular;
    auto w = weight * std::max({ Or.r, Or.g, Or.b });
    
    auto it = _scene->getSceneLightsIterator();
    auto end = _scene->getSceneLightsEnd();

    Color I = Color::black;
    for (; it != end; it++)
    {
      auto l = it->get();
      vec3f lPos = l->sceneObject()->transform()->position();
      auto lDirection = l->sceneObject()->transform()->rotation() * vec3f(0, -1, 0);
      auto ray = l->type() == Light::Type::Directional ? Ray{ p,-lDirection} : Ray{ p,lPos - p, 0, (lPos-p).length() };
      auto camPos = Camera::current()->transform()->position();
      auto V = (camPos - p).versor();

      Color Il;
      vec3f Ll;
      float dl;
      float gammaL;
      float phiL;
      
      if (!shadow(ray))
      {
        I += material.ambient * _scene->ambientLight;
        switch (l->type())
        {
        case (0):// directional
          Il = l->color;
          Ll = -lDirection.versor();
          break;
        case(1):// point
          Ll = (lDirection - p).versor();
          dl = (p - lPos).length();
          Il = l->color * (1.0f / (pow(dl, l->fl())));
          break;
        case(2): // spot
          gammaL = math::toRadians((float)l->gammaL());
          Ll = (lPos - p).versor();
          phiL = abs(acos(lDirection.dot(-Ll)));
          dl = (p - lPos).length();
          Il = gammaL < phiL ? Color::black : l->color * (1.0f / (pow(dl, l->fl()) * pow(cos(phiL), l->decayExponent())));
          break;
        }
        auto OdIl = material.diffuse * Il;
        auto OsIl = material.spot * Il;
        auto Rl = Ll - 2 * N.dot(Ll) * N;
        I += OdIl * N.dot(Ll) + OsIl * pow(max(Rl.dot(V), 0.0f), material.shine);

      }
    }
    auto Rf = ray.direction - 2 * N.dot(ray.direction) * N;
    if (Or != Color::black)
    {
      if (w > _minWeight)
      {
        auto tr = trace({ p,Rf }, level + 1, w);
        if (tr != background())
        {
          I += Or * tr;
        }
      }
    }

    return I;
  }

  Color
    RayTracer::background() const
    //[]---------------------------------------------------[]
    //|  Background                                         |
    //|  @return background color                           |
    //[]---------------------------------------------------[]
  {
    return _scene->backgroundColor;
  }

  bool
    RayTracer::shadow(const Ray& ray)
    //[]---------------------------------------------------[]
    //|  Verifiy if ray is a shadow ray                     |
    //|  @param the ray (input)                             |
    //|  @return true if the ray intersects an object       |
    //[]---------------------------------------------------[]
  {
    Intersection hit;
    return intersect(ray, hit);
  }

} // end namespace cg
