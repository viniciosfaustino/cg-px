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
// OVERVIEW: SceneObject.cpp
// ========
// Source file for scene object.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 25/08/2018

#include "SceneObject.h"
#include "Scene.h"
#include <iostream>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// SceneObject implementation
// ===========

  auto SceneObject::getComponentsIterator()
  {
    return this->_components.begin();
  }

  auto SceneObject::getComponentsEnd()
  {
    return this->_components.end();
  }

  void
    SceneObject::setParent(Reference<SceneObject> parent)
  {
    if (_parent != nullptr)
    {
      _parent->removeChild(this);
      if (parent != nullptr)
      {
        parent->_children.push_back(this);
      }
    }

    if (parent == nullptr)
    {      
      _parent = this->scene()->root();
      _parent->_children.push_back(this);
    }
    else
    {      
      _parent = parent;      
    }
    
  }

  void SceneObject::removeChild(Reference<SceneObject> child)
  {
    auto it = this->getIterator();
    auto it_end = this->getEnd();
    bool found = false;
    while (!found && it != it_end)
    {
      if (it->get() == child) 
      {
        found = true;        
        this->_children.erase(it);
      }
      else
      {
        it++;
      }      
    }    
  }

  void SceneObject::addComponent(Component* component)
  {
    _components.push_back(component);
  }

  void SceneObject::removeComponent(Component* component)
  {
    auto it = this->getComponentsIterator();
    _components.erase(it);

  }


  void SceneObject::addChild(Reference<SceneObject> child)
{
    child->_parent = this;
    _children.push_back(child);
}

} // end namespace cg
