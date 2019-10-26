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
// OVERVIEW: Light.h
// ========
// Class definition for light.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 14/10/2019

#ifndef __Light_h
#define __Light_h

#define MIN_ANGLE_SPOT_LIGHT 10;

#include "Component.h"
#include "graphics/Color.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Light: light class
// =====
class Light: public Component
{
public:
  
  enum Type
  {
    Directional,
    Point,
    Spot
  };

  Color color{Color::white};

  Light():
    Component{"Light"},
    _type{Directional}
  {
    // do nothing
  }

  auto type() const
  {
    return _type;
  }

  void setType(Type type)
  {
    _type = type;
  }

  int fl()
  {
	  return _fl;
  }

  void fl(int fL)
  {
	  _fl = fL;
  }

  float gammaL()
  {
	  return _gammaL;
  }

  void setGammaL(float gL)
  {
	  _gammaL = gL;
  }

  int decayExponent()
  {
	  return _decayExponent;
  }

  void decayExponent(int dE)
  {
	  _decayExponent = dE;
  }

private:
  Type _type;
  int _fl; //falloff
  float _gammaL; // angulo entre eixo do cone e qualquer de suas geratrizes
  int _decayExponent;// expoente de decaimento

}; // Light

} // end namespace cg

#endif // __Light_h
