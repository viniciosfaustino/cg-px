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

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// SceneObject implementation
// ===========

  bool SceneObject::isRelated(SceneObject* obj)
  {
    if (this != nullptr && obj->parent() != nullptr)
    {
      if (obj->parent() == this)
      {
        return true;
      }
      else
      {
        if (obj->parent() == this->scene()->root()) {
          return false;
        }
        isRelated(obj->parent());

      }
    }
    return false;
  }




  SceneObject::~SceneObject()
  {    
    _components.clear();
    _children.clear();
  }

  void SceneObject::removeComponentFromScene()
  {
    auto it = getComponentsIterator();
    auto end = getComponentsEnd();    
    for (; it != end; it++)
    {
      if (auto p =dynamic_cast<Primitive*>((Component*)* it))
      {
        _scene->removeScenePrimitive(p);
      }
      
    }
    auto childIt = getIterator();
    auto childItEnd = getIteratorEnd();
    for (; childIt != childItEnd; childIt++)
    {
      childIt->get()->removeComponentFromScene();
    }
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
    transform()->parentChanged();
  }

  void SceneObject::removeChild(Reference<SceneObject> child)
  {
    auto it = this->getIterator();
    auto it_end = this->getIteratorEnd();
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
    if (auto c = dynamic_cast<Camera*>(component))
    {
      setCamera(c);
      _scene->addScenePrimitive(c);
      
    }
    if (auto l = dynamic_cast<Light*>(component))
    {
      setLight(l);
      _scene->addSceneLight(l);
      _scene->addScenePrimitive(l);
    }
    if (auto p = dynamic_cast<Primitive*>(component))
    {
      _scene->addScenePrimitive(p);
      setPrimitive(p);
    }
    component->_sceneObject = this;
    _components.push_back(component);
  }

  void SceneObject::removeComponent(Component* component)
  {
    auto it = this->getComponentsIterator();
    auto end = this->getComponentsEnd();
    bool found = false;
    while (!found && it != end)
    {
      if (it->get() == component)
      {
        if (! dynamic_cast<Transform*>(component))
        {
          _scene->removeScenePrimitive(component);
        }
        _components.erase(it);
        found = true;
      }
      else
      {
        it++;
      }
    }
  }

  void SceneObject::addChild(Reference<SceneObject> child)
{
    child->_parent = this;
    _children.push_back(child);
    child->transform()->parentChanged();    
}

} // end namespace cg
