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
// OVERVIEW: SceneObject.h
// ========
// Class definition for scene object.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 25/08/2018

#ifndef __SceneObject_h
#define __SceneObject_h

#include "SceneNode.h"
#include "Transform.h"
#include "Camera.h"
#include "Primitive.h"
#include "Light.h"
#include <vector>
#include <iterator>

namespace cg
{ // begin namespace cg

// Forward definition
class Scene;


/////////////////////////////////////////////////////////////////////
//
// SceneObject: scene object class
// ===========
class SceneObject: public SceneNode
{
public:
  bool visible{true};


  /// Constructs an empty scene object.
  SceneObject(const char* name, Scene* scene) :
    SceneNode{ name },
    _scene{ scene },
    _parent{}
  {    
    //auto _trans = new Transform{};
    
    addComponent(&_transform);


    makeUse(&_transform);
    _camera = nullptr;
    _primitive = nullptr;
	  _light = nullptr;
  }

  /// Returns the scene which this scene object belong to.
  auto scene() const
  {
    return _scene;
  }
  ~SceneObject();

  void removeComponentFromScene();


  /// Returns the parent of this scene object.
  auto parent() const
  {
    return _parent;
  }

  size_t childrenSize()
  {
	  return this->_children.size();
  }

  size_t componentsSize()
  {
    return this->_components.size();
  }

  /// Sets the parent of this scene object.
  void setParent(Reference<SceneObject> parent);

  /// Returns the transform of this scene object.
  auto transform()
  {
    return &_transform;
  }

  void removeChild(Reference<SceneObject> child);
  void removeChildRecursive(Reference<SceneObject> current, Reference<SceneObject> child);
  void removeComponent(Component* component);
  bool isRelated(SceneObject*);

  void addChild(Reference<SceneObject> child);
  void addComponent(Component* component);

  auto getIterator()
  {
	  return this->_children.begin();
  }

  auto getIteratorEnd()
  {
	  return this->_children.end();
  }

  auto getComponentsIterator()
  {
    return this->_components.begin();
  }

  auto getComponentsEnd()
  {
    return this->_components.end();
  }

  auto primitive()
  {
    return _primitive;
  }

  auto camera()
  {
    return _camera;
  }

  auto light()
  {

	  return _light;
  }

  void setLight(Reference<Light> light)
  {

	  _light = light;
  }

  void setPrimitive(Reference<Primitive> primitive)
  {
    _primitive = primitive;
  }  

  void setCamera(Reference<Camera> camera)
  {
    _camera = camera;
  }

private:
  Scene* _scene;
  SceneObject* _parent;
  std::vector  <Reference<SceneObject>>  _children;
  std::vector  <Reference<Component>> _components;  
  Reference<Primitive> _primitive;
  Reference<Camera> _camera;
  Reference<Light> _light;
  Transform _transform;

  friend class Scene;

}; // SceneObject

/// Returns the transform of a component.
inline Transform*
Component::transform() // declared in Component.h
{
  return sceneObject()->transform();
}

/// Returns the parent of a transform.
inline Transform*
Transform::parent() const // declared in Transform.h
{
  if (auto p = sceneObject()->parent())
    return p->transform();
   return nullptr;
}

} // end namespace cg

#endif // __SceneObject_h
