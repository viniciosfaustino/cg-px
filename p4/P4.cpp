#include "geometry/MeshSweeper.h"
#include "P4.h"

MeshMap P4::_defaultMeshes;

inline auto
normalize(const vec4f& p)
{
  return vec3f{ p } *math::inverse(p.w);
}

inline auto
viewportToNDC(int x, int y)
{
  GLint v[4];

  glGetIntegerv(GL_VIEWPORT, v);

  const auto xn = float(x - v[0]) * 2.0f / float(v[2]) - 1.0f;
  const auto yn = float(y - v[1]) * 2.0f / float(v[3]) - 1.0f;

  return vec4f{ xn, yn, -1, 1 };
}

inline Ray
P4::makeRay(int x, int y) const
{
  auto c = _editor->camera();
  mat4f m{ vpMatrix(c) };

  m.invert();

  auto p = normalize(m * viewportToNDC(x, height() - y));
  auto t = c->transform();
  Ray r{ t->position(), -t->forward() };

  if (c->projectionType() == Camera::Perspective)
    r.direction = (p - r.origin).versor();
  else
    r.origin = p - r.direction * c->nearPlane();
  return r;
}

inline void
P4::buildDefaultMeshes()
{
  _defaultMeshes["None"] = nullptr;
  _defaultMeshes["Box"] = GLGraphics3::box();
  _defaultMeshes["Sphere"] = GLGraphics3::sphere();
}

inline Primitive*
makePrimitive(MeshMapIterator mit)
{
  return new Primitive(mit->second, mit->first);
}

inline void
P4::buildScene()
{
  //buildScene2();
  //return;
  int boxCounter = 0;
  _current = _scene = new Scene{ "Scene 1" };

  _editor = new SceneEditor{ *_scene };
  _editor->setDefaultView((float)width() / (float)height());

  auto o = new SceneObject{ "Main Camera", _scene };

  auto camera = new Camera;
  camera->setViewAngle(60);
  camera->setClippingPlanes(0.01, 1000);
  o->setParent(_scene->root());
  o->setCamera(camera);
  o->transform()->setLocalPosition(vec3f(0, 4.8, 3.7));
  o->transform()->rotate(vec3f(-50, 0, 0));

  o->addComponent(camera);

  _objects.push_back(o);
  _scene->root()->addChild(o);
  Camera::setCurrent(camera);

  o = new SceneObject{ "box1", _scene };

  auto p1 = makePrimitive(_defaultMeshes.find("Box"));
  o->setParent(_scene->root());
  o->addComponent(p1);
  o->transform()->setLocalScale(vec3f(0.4, 0.3, 0.4));
  o->transform()->setLocalPosition(vec3f(0, 2.3, 0));
  _scene->root()->addChild(o);


  o = new SceneObject{ "box2", _scene };
  p1 = makePrimitive(_defaultMeshes.find("Box"));
  o->setParent(_scene->root());
  o->addComponent(p1);  
  _scene->root()->addChild(o);

  auto l = createLight(cg::Light::Type::Spot);
  o = new SceneObject{ "light0", _scene };
  l->color = Color::red;
  l->setGammaL(10);
  o->setParent(_scene->root());
  o->addComponent(l);
  o->transform()->setLocalPosition(vec3f(1, 10, 0));
  _scene->root()->addChild(o);

  l = createLight(cg::Light::Type::Spot);
  o = new SceneObject{ "light1", _scene };
  l->color = Color::green;
  l->setGammaL(10);
  o->setParent(_scene->root());
  o->addComponent(l);
  o->transform()->setLocalPosition(vec3f(-1, 10, 0));
  _scene->root()->addChild(o);

  l = createLight(cg::Light::Type::Directional);
  o = new SceneObject{ "light2", _scene };
  l->color = Color::blue;  
  o->setParent(_scene->root());
  o->addComponent(l);  
  o->transform()->rotate(vec3f(90, 0, 0));
  _scene->root()->addChild(o);
}

inline void
setObj(Reference<SceneObject> o, vec3f localPos, vec3f localScale, vec3f rotate, Reference<Scene>_scene)
{
  o->setParent(_scene->root());  
  o->transform()->setLocalPosition(localPos);
  o->transform()->rotate(rotate);
}
inline void
setCam(Reference<Camera>cam, Reference<SceneObject>o, float f, float b)
{

}

inline void
P4::buildScene2()
{  
  _current = _scene = new Scene{ "Scene 1" };
  _editor = new SceneEditor{ *_scene };
  _editor->setDefaultView((float)width() / (float)height());

  auto o = new SceneObject{ "Main Camera", _scene };

  auto camera = new Camera;
  camera->setViewAngle(60);
  camera->setClippingPlanes(0.01, 1000);
  o->setParent(_scene->root());
  o->setCamera(camera);
  o->transform()->setLocalPosition(vec3f(0, 14, 0));
  o->transform()->rotate(vec3f(-90, 0, 0));

  o->addComponent(camera);

  _objects.push_back(o);
  _scene->root()->addChild(o);
  Camera::setCurrent(camera);

  o = new SceneObject{ "ground", _scene };

  auto p1 = makePrimitive(_defaultMeshes.find("Box"));
  o->setParent(_scene->root());
  o->addComponent(p1);
  o->transform()->setLocalScale(vec3f(7, 0.01, 7));
  _scene->root()->addChild(o);

  o = new SceneObject{ "base1", _scene };
  o->setParent(_scene->root());
  p1 = makePrimitive(_defaultMeshes.find("Box"));
  o->addComponent(p1);
  o->transform()->setLocalScale(vec3f(4, 0.25, 4));
  o->transform()->setLocalPosition(vec3f(0, 0.25, 0));
  _scene->root()->addChild(o);

  o = new SceneObject{ "base2", _scene };
  p1 = makePrimitive(_defaultMeshes.find("Box"));
  o->setParent(_scene->root());
  o->addComponent(p1);
  o->transform()->setLocalScale(vec3f(3.75, 0.5, 3.75));
  o->transform()->setLocalPosition(vec3f(0, 0.5, 0));
  _scene->root()->addChild(o);

  o = new SceneObject{ "base3", _scene };
  p1 = makePrimitive(_defaultMeshes.find("Box"));
  o->setParent(_scene->root());
  o->addComponent(p1);
  o->transform()->setLocalScale(vec3f(3.5, 0.75, 3.5));
  o->transform()->setLocalPosition(vec3f(0, 0.75, 0));
  _scene->root()->addChild(o);

  o = new SceneObject{ "base4", _scene };
  p1 = makePrimitive(_defaultMeshes.find("Box"));
  o->setParent(_scene->root());
  o->addComponent(p1);
  o->transform()->setLocalScale(vec3f(3.25, 1, 3.25));
  o->transform()->setLocalPosition(vec3f(0, 1, 0));
  _scene->root()->addChild(o);

  o = new SceneObject{ "base5", _scene };
  p1 = makePrimitive(_defaultMeshes.find("Box"));
  o->setParent(_scene->root());
  o->addComponent(p1);
  o->transform()->setLocalScale(vec3f(3.0, 1.25, 3.0));
  o->transform()->setLocalPosition(vec3f(0, 1.25, 0));
  _scene->root()->addChild(o);

  o = new SceneObject{ "base6", _scene };
  p1 = makePrimitive(_defaultMeshes.find("Box"));
  o->setParent(_scene->root());
  o->addComponent(p1);
  o->transform()->setLocalScale(vec3f(2.75, 1.5, 2.75));
  o->transform()->setLocalPosition(vec3f(0, 1.5, 0));
  _scene->root()->addChild(o);

  o = new SceneObject{ "base7", _scene };
  p1 = makePrimitive(_defaultMeshes.find("Box"));
  o->setParent(_scene->root());
  o->addComponent(p1);
  o->transform()->setLocalScale(vec3f(2.5, 1.75, 2.5));
  o->transform()->setLocalPosition(vec3f(0, 1.75, 0));
  _scene->root()->addChild(o);

  o = new SceneObject{ "base8", _scene };
  p1 = makePrimitive(_defaultMeshes.find("Box"));
  o->setParent(_scene->root());
  o->addComponent(p1);
  o->transform()->setLocalScale(vec3f(2.0, 2.0, 2.0));
  o->transform()->setLocalPosition(vec3f(0, 2.0, 0));
  _scene->root()->addChild(o);

  o = new SceneObject{ "stair1", _scene };
  p1 = makePrimitive(_defaultMeshes.find("Box"));
  o->setParent(_scene->root());
  o->addComponent(p1);
  o->transform()->setLocalScale(vec3f(0.1, 1.7, 0.5));
  o->transform()->setLocalPosition(vec3f(3.4, 1.5, 0));
  o->transform()->rotate(vec3f(0, 0, 27));
  _scene->root()->addChild(o);

  o = new SceneObject{ "stair2", _scene };
  p1 = makePrimitive(_defaultMeshes.find("Box"));
  o->setParent(_scene->root());
  o->addComponent(p1);
  o->transform()->setLocalScale(vec3f(0.1, 1.7, 0.5));
  o->transform()->setLocalPosition(vec3f(-3.4, 1.5, 0));
  o->transform()->rotate(vec3f(0, 0, -27));
  _scene->root()->addChild(o);

  o = new SceneObject{ "stair3", _scene };
  p1 = makePrimitive(_defaultMeshes.find("Box"));
  o->setParent(_scene->root());
  o->addComponent(p1);
  o->transform()->setLocalScale(vec3f(0.5, 1.7, 0.1));
  o->transform()->setLocalPosition(vec3f(0, 1.5, -3.4));
  o->transform()->rotate(vec3f(27, 0, 0));
  _scene->root()->addChild(o);

  o = new SceneObject{ "stair4", _scene };
  p1 = makePrimitive(_defaultMeshes.find("Box"));
  o->setParent(_scene->root());
  o->addComponent(p1);
  o->transform()->setLocalScale(vec3f(0.5, 1.7, 0.1));
  o->transform()->setLocalPosition(vec3f(0, 1.5, 3.4));
  o->transform()->rotate(vec3f(-27, 0, 0));
  _scene->root()->addChild(o);

  o = new SceneObject{ "cupula", _scene };
  p1 = makePrimitive(_defaultMeshes.find("Sphere"));
  o->setParent(_scene->root());
  o->addComponent(p1);
  o->transform()->setLocalScale(vec3f(1.5, 1.5, 1.5));
  o->transform()->setLocalPosition(vec3f(0, 3.5, 0));
  _scene->root()->addChild(o);

  //lights  
  auto l = createLight(cg::Light::Type::Spot);
  o = new SceneObject{ "light0", _scene };
  l->color = vec4f(0.4823, 0.4823, 0.4823, 1);
  o->setParent(_scene->root());
  o->addComponent(l);
  o->transform()->setLocalPosition(vec3f(0, 5.5, 0));
  _scene->root()->addChild(o);


  l = createLight(cg::Light::Type::Spot);
  o = new SceneObject{ "light1", _scene };
  l->color = vec4f(1, 0, 0, 1);
  l->setGammaL(15.f);
  o->setParent(_scene->root());
  o->addComponent(l);
  o->transform()->setLocalPosition(vec3f(0, 5.9, -0.5));
  o->transform()->rotate(vec3f(-38, 0, 0));
  _scene->root()->addChild(o);

  l = createLight(cg::Light::Type::Spot);
  o = new SceneObject{ "light2", _scene };
  l->color = vec4f(0, 0, 1, 1);
  l->setGammaL(15.f);
  o->setParent(_scene->root());
  o->addComponent(l);
  o->transform()->setLocalPosition(vec3f(0, 5.9, 0.5));
  o->transform()->rotate(vec3f(38, 0, 0));
  _scene->root()->addChild(o);

  l = createLight(cg::Light::Type::Spot);
  o = new SceneObject{ "light3", _scene };
  l->color = vec4f(0, 1, 0, 1);
  l->setGammaL(15.f);
  o->setParent(_scene->root());
  o->addComponent(l);
  o->transform()->setLocalPosition(vec3f(0.5, 5.9, 0));
  o->transform()->rotate(vec3f(0, 0, -38));
  _scene->root()->addChild(o);


  l = createLight(cg::Light::Type::Directional);
  o = new SceneObject{ "light4", _scene };
  l->color = vec4f(1, 0, 0, 1);
  o->setParent(_scene->root());
  o->addComponent(l);
  o->transform()->rotate(vec3f(90, 0, 0));
  _scene->root()->addChild(o);

  l = createLight(cg::Light::Type::Directional);
  o = new SceneObject{ "light5", _scene };
  l->color = vec4f(0, 0, 1, 1);
  o->setParent(_scene->root());
  o->addComponent(l);
  o->transform()->rotate(vec3f(0, 0, 90));
  _scene->root()->addChild(o);

  l = createLight(cg::Light::Type::Point);
  o = new SceneObject{ "light6", _scene };
  l->color = vec4f(1, 1, 1, 1);
  o->setParent(_scene->root());
  o->addComponent(l);
  o->transform()->setLocalPosition(vec3f(0, 3.3, 0));
  _scene->root()->addChild(o);

  camera = new Camera;
  camera->setViewAngle(60);
  camera->setClippingPlanes(0.01, 1000);

  o = new SceneObject{ "camera0", _scene };
  o->setCamera(camera);
  o->setParent(_scene->root());
  o->transform()->setLocalPosition(vec3f(-6, 6.8, 6));
  o->transform()->rotate(vec3f(-37, -45, 0));
  camera->setProjectionType(Camera::ProjectionType::Parallel);

  o->addComponent(camera);

  _objects.push_back(o);
  _scene->root()->addChild(o);
  Camera::setCurrent(camera);
}


void
P4::initialize()
{
  Application::loadShaders(_programG, "shaders/gouraud.vert", "shaders/gouraud.frag");
  Application::loadShaders(_programP, "shaders/phong.vert", "shaders/phong.frag");
  Assets::initialize();
  buildDefaultMeshes();
  buildScene2();
  _renderer = new GLRenderer{ *_scene };
  _rayTracer = new RayTracer{ *_scene };
  _renderer->setProgram(&_programP);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  glEnable(GL_LINE_SMOOTH);
  _programG.use();
}

Reference<Light>
P4::createLight(cg::Light::Type type)
{
  auto l = new Light();
  l->setType(type);
  l->fl(0);
  l->setGammaL(45);
  l->decayExponent(0);
  return l;
}

inline void
P4::createNewObject(SceneObjectType type, std::string shape)
{
  static int shapeCounter = 0;
  static int objectCounter = 0;
  static int cameraCounter = 0;
  static int lightCounter = 0;
  Reference<SceneObject> parent;
  std::string name;

  if (_current == _scene)
  {
    parent = dynamic_cast<SceneObject*>((SceneObject*)_scene->root());
  }
  else
  {
    parent = dynamic_cast<SceneObject*>(_current);
  }
  std::string typeName;
  Reference<SceneObject> child = nullptr;
  Reference<Camera> c = nullptr;
  Reference<Primitive> p1 = nullptr;
  Reference<Light> l = nullptr;
  switch (type)
  {
  case P4::SceneObjectType::empty:
    name = "Object " + std::to_string(shapeCounter);
    child = new SceneObject{ name.c_str(), _scene };
    objectCounter++;
    break;

  case P4::SceneObjectType::shape:
    name = shape + " " + std::to_string(shapeCounter);
    child = new SceneObject{ name.c_str(), _scene };
    shapeCounter++;

    p1 = makePrimitive(_defaultMeshes.find(shape));

    child->addComponent(p1);
    child->setPrimitive((Reference<Primitive>)p1);
    break;

  case P4::SceneObjectType::camera:
    c = new Camera();
    name = "Camera " + std::to_string(cameraCounter);
    child = new SceneObject{ name.c_str(), _scene };
    child->addComponent(c);
    child->setCamera(c);
    cameraCounter++;
    break;

  case P4::SceneObjectType::spot:
    name = "Light " + std::to_string(lightCounter);
    child = new SceneObject{ name.c_str(), _scene };
    l = createLight(cg::Light::Type::Spot);
    child->addComponent(l);
    lightCounter++;
    break;

  case P4::SceneObjectType::point:
    name = "Light " + std::to_string(lightCounter);
    child = new SceneObject{ name.c_str(), _scene };
    l = createLight(cg::Light::Type::Point);
    child->addComponent(l);
    lightCounter++;
    break;

  case P4::SceneObjectType::directional:
    name = "Light " + std::to_string(lightCounter);
    child = new SceneObject{ name.c_str(), _scene };
    l = createLight(cg::Light::Type::Directional);
    child->addComponent(l);
    lightCounter++;
    break;
  }

  parent->addChild(child);
  _objects.push_back(child);
}

void
P4::removeObject(Reference<SceneObject> object)
{
  auto it = _objects.begin();
  auto end = _objects.end();
  bool found = false;
  while (!found && it != end)
  {
    if (*it == object)
    {
      found = true;
      _objects.erase(it);
    }
    else
    {
      it++;
    }

  }
}

inline void
P4::dragNDrop(SceneObject* obj)
{
  if (ImGui::BeginDragDropSource())
  {
    ImGui::SetDragDropPayload("obj", &obj, sizeof(obj));
    //ImGui::Text(obj->name);
    ImGui::EndDragDropSource();
  }

  if (ImGui::BeginDragDropTarget())
  {
    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("obj", 3))
    {
      SceneObject* t_obj = *(SceneObject**)payload->Data;

      if (!t_obj->isRelated(obj))
        t_obj->setParent(obj);

    }
    ImGui::EndDragDropTarget();
  }

}


inline void
P4::hierarchyWindow()
{
  static int boxCounter = 8;
  ImGui::Begin("Hierarchy");
  if (ImGui::Button("Create###object"))
    ImGui::OpenPopup("CreateObjectPopup");
  if (ImGui::BeginPopup("CreateObjectPopup"))
  {
    if (ImGui::MenuItem("Empty Object"))
    {
      createNewObject(P4::SceneObjectType::empty, "");
    }
    if (ImGui::BeginMenu("3D Object"))
    {
      if (ImGui::MenuItem("Box"))
      {
        createNewObject(P4::SceneObjectType::shape, "Box");
      }
      if (ImGui::MenuItem("Sphere"))
      {
        createNewObject(P4::SceneObjectType::shape, "Sphere");
      }
      ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Camera"))
    {
      createNewObject(P4::SceneObjectType::camera, "");
    }
    if (ImGui::BeginMenu("Light"))
    {
      if (ImGui::MenuItem("Directional"))
      {
        createNewObject(P4::SceneObjectType::directional, "");
      }
      if (ImGui::MenuItem("Point"))
      {
        createNewObject(P4::SceneObjectType::point, "");
      }
      if (ImGui::MenuItem("Spot"))
      {
        createNewObject(P4::SceneObjectType::spot, "");
      }
      ImGui::EndMenu();
    }

    ImGui::EndPopup();
  }
  ImGui::SameLine();
  if (ImGui::Button("Delete"))
  {
    if (_current != _scene)
    {
      auto aux = dynamic_cast<SceneObject*>(_current);
      if (aux != _scene->root())
      {
        auto parent = aux->parent();
        _current = parent;
        aux->removeComponentFromScene();
        removeObject(aux);
        parent->removeChild(aux);
      }


    }
  }
  ImGui::Separator();


  ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_OpenOnArrow };
  auto open = ImGui::TreeNodeEx(_scene,
    _current == _scene ? flag | ImGuiTreeNodeFlags_Selected : flag,
    _scene->name());

  if (ImGui::IsItemClicked())
    _current = _scene;

  if (ImGui::BeginDragDropTarget())
  {
    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("obj"))
    {
      SceneObject* t_obj = *(SceneObject**)payload->Data;

      t_obj->setParent(nullptr);

    }
    ImGui::EndDragDropTarget();
  }



  recursionTree(open, _scene->root());

  ImGui::End();
  //chamar função recursiva que popa os object scenes na tela
}

namespace ImGui
{ // begin namespace ImGui

  void
    ObjectNameInput(NameableObject* object)
  {
    const int bufferSize{ 128 };
    static NameableObject* current;
    static char buffer[bufferSize];

    if (object != current)
    {
      strcpy_s(buffer, bufferSize, object->name());
      current = object;
    }
    if (ImGui::InputText("Name", buffer, bufferSize))
      object->setName(buffer);
  }

  inline bool
    ColorEdit3(const char* label, Color& color)
  {
    return ImGui::ColorEdit3(label, (float*)&color);
  }

  inline bool
    DragVec3(const char* label, vec3f& v)
  {
    return DragFloat3(label, (float*)&v, 0.1f, 0.0f, 0.0f, "%.2g");
  }

  void
    TransformEdit(Transform* transform)
  {
    vec3f temp;

    temp = transform->localPosition();
    if (ImGui::DragVec3("Position", temp))
      transform->setLocalPosition(temp);
    temp = transform->localEulerAngles();
    if (ImGui::DragVec3("Rotation", temp))
      transform->setLocalEulerAngles(temp);
    temp = transform->localScale();
    if (ImGui::DragVec3("Scale", temp))
      transform->setLocalScale(temp);
  }

} // end namespace ImGui

inline void
P4::sceneGui()
{
  auto scene = (Scene*)_current;

  ImGui::ObjectNameInput(_current);
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Colors"))
  {
    ImGui::ColorEdit3("Background", scene->backgroundColor);
    ImGui::ColorEdit3("Ambient Light", scene->ambientLight);
  }
}

inline void
P4::inspectShape(Primitive& primitive)
{
  char buffer[16];

  snprintf(buffer, 16, "%s", primitive.meshName());
  ImGui::InputText("Mesh", buffer, 16, ImGuiInputTextFlags_ReadOnly);
  if (ImGui::BeginDragDropTarget())
  {
    if (auto* payload = ImGui::AcceptDragDropPayload("PrimitiveMesh"))
    {
      auto mit = *(MeshMapIterator*)payload->Data;
      primitive.setMesh(mit->second, mit->first);
    }
    ImGui::EndDragDropTarget();
  }
  ImGui::SameLine();
  if (ImGui::Button("...###PrimitiveMesh"))
    ImGui::OpenPopup("PrimitiveMeshPopup");
  if (ImGui::BeginPopup("PrimitiveMeshPopup"))
  {
    auto& meshes = Assets::meshes();

    if (!meshes.empty())
    {
      for (auto mit = meshes.begin(); mit != meshes.end(); ++mit)
        if (ImGui::Selectable(mit->first.c_str()))
          primitive.setMesh(Assets::loadMesh(mit), mit->first);
      ImGui::Separator();
    }
    for (auto mit = _defaultMeshes.begin(); mit != _defaultMeshes.end(); ++mit)
      if (ImGui::Selectable(mit->first.c_str()))
        primitive.setMesh(mit->second, mit->first);
    ImGui::EndPopup();
  }
}

inline void
P4::inspectMaterial(Material& material)
{
  ImGui::ColorEdit3("Ambient", material.ambient);
  ImGui::ColorEdit3("Diffuse", material.diffuse);
  ImGui::ColorEdit3("Spot", material.spot);
  ImGui::DragFloat("Shine", &material.shine, 1, 0, 1000.0f);
  ImGui::ColorEdit3("Specular", material.specular);
}

inline void
P4::inspectPrimitive(Primitive& primitive)
{
  //const auto flag = ImGuiTreeNodeFlags_NoTreePushOnOpen;

  //if (ImGui::TreeNodeEx("Shape", flag))
  inspectShape(primitive);
  //if (ImGui::TreeNodeEx("Material", flag))
  inspectMaterial(primitive.material);
}

inline void
P4::inspectLight(Light& light)
{
  static const char* lightTypes[]{ "Directional", "Point", "Spot" };
  auto lt = light.type();

  if (ImGui::BeginCombo("Type", lightTypes[lt]))
  {
    for (auto i = 0; i < IM_ARRAYSIZE(lightTypes); ++i)
    {
      bool selected = lt == i;

      if (ImGui::Selectable(lightTypes[i], selected))
        lt = (Light::Type)i;
      if (selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  light.setType(lt);
  ImGui::ColorEdit3("Color", light.color);
  if (light.type() == Light::Type::Point)
  {
    auto falloff = light.fl();
    if (ImGui::SliderInt("Falloff",
      &falloff,
      0,
      2
    ))

      light.fl(falloff < 0 ? 0 : falloff);
    light.fl(falloff > 2 ? 2 : falloff);
  }
  else if (light.type() == Light::Type::Spot)
  {
    auto decayExponent = light.decayExponent();
    if (ImGui::SliderInt("Decay Exponent",
      &decayExponent,
      0,
      2
    ))

      light.decayExponent(decayExponent < 0 ? 0 : decayExponent);
    light.decayExponent(decayExponent > 2 ? 2 : decayExponent);

    auto gammaL = light.gammaL();

    if (ImGui::DragFloat("Cone Angle", &gammaL, 0.3f, 0.0f, 90.0f))
    {
      light.setGammaL(gammaL);
    }
  }
}

void
P4::inspectCamera(Camera& camera)
{
  static const char* projectionNames[]{ "Perspective", "Orthographic" };
  auto cp = camera.projectionType();

  if (ImGui::BeginCombo("Projection", projectionNames[cp]))
  {
    for (auto i = 0; i < IM_ARRAYSIZE(projectionNames); ++i)
    {
      auto selected = cp == i;

      if (ImGui::Selectable(projectionNames[i], selected))
        cp = (Camera::ProjectionType)i;
      if (selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  camera.setProjectionType(cp);
  if (cp == View3::Perspective)
  {
    auto fov = camera.viewAngle();

    if (ImGui::SliderFloat("View Angle",
      &fov,
      MIN_ANGLE,
      MAX_ANGLE,
      "%.0f deg",
      1.0f))
      camera.setViewAngle(fov <= MIN_ANGLE ? MIN_ANGLE : fov);
  }
  else
  {
    auto h = camera.height();

    if (ImGui::DragFloat("Height",
      &h,
      MIN_HEIGHT * 10.0f,
      MIN_HEIGHT,
      math::Limits<float>::inf()))
      camera.setHeight(h <= 0 ? MIN_HEIGHT : h);
  }

  float n;
  float f;

  camera.clippingPlanes(n, f);

  if (ImGui::DragFloatRange2("Clipping Planes",
    &n,
    &f,
    MIN_DEPTH,
    MIN_DEPTH,
    math::Limits<float>::inf(),
    "Near: %.2f",
    "Far: %.2f"))
  {
    if (n <= 0)
      n = MIN_DEPTH;
    if (f - n < MIN_DEPTH)
      f = n + MIN_DEPTH;
    camera.setClippingPlanes(n, f);
  }
  if (ImGui::Button("Delete"))
  {
    auto sceneObject = camera.sceneObject();
    sceneObject->removeComponent(dynamic_cast<Component*>(&camera));
    sceneObject->setCamera(nullptr);
  }
}

inline void
P4::addComponentButton(SceneObject& object)
{
  if (ImGui::Button("Add Component"))
    ImGui::OpenPopup("AddComponentPopup");
  if (ImGui::BeginPopup("AddComponentPopup"))
  {
    if (ImGui::MenuItem("Camera"))
    {
      if (!object.camera())
      {
        Reference<Camera> c = new Camera;
        object.addComponent(c);
        object.setCamera(c);
      }
    }
    if (ImGui::BeginMenu("Primitive"))
    {
      if (!object.primitive())
      {
        if (ImGui::MenuItem("Box"))
        {
          auto p = makePrimitive(_defaultMeshes.find("Box"));
          object.addComponent(p);
          object.setPrimitive((Reference<Primitive>)p);
        }

        if (ImGui::MenuItem("Sphere"))
        {
          auto p = makePrimitive(_defaultMeshes.find("Sphere"));
          object.addComponent(p);
          object.setPrimitive((Reference<Primitive>)p);
        }
      }

      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Light"))
    {
      if (!object.light())
      {
        Reference<Light> l = new Light();

        if (ImGui::MenuItem("Point"))
        {

          l->setType(cg::Light::Type::Point);
          l->fl(0);
          l->setGammaL(45);
          l->decayExponent(0);
          object.addComponent(l);

        }
        if (ImGui::MenuItem("Directional"))
        {
          l->setType(cg::Light::Type::Directional);
          l->fl(0);
          l->setGammaL(45);
          l->decayExponent(0);
          object.addComponent(l);
        }
        if (ImGui::MenuItem("Spot"))
        {
          l->setType(cg::Light::Type::Spot);
          l->fl(0);
          l->setGammaL(45);
          l->decayExponent(0);
          object.addComponent(l);
        }

      }
      ImGui::EndMenu();
    }
    ImGui::EndPopup();
  }
  ImGui::Separator();
}

inline void
P4::sceneObjectGui()
{
  auto object = (SceneObject*)_current;

  addComponentButton(*object);
  ImGui::Separator();
  ImGui::ObjectNameInput(object);
  ImGui::SameLine();
  ImGui::Checkbox("visible", &object->visible);
  ImGui::Separator();
  if (ImGui::CollapsingHeader(object->transform()->typeName()))
    ImGui::TransformEdit(object->transform());

  auto it = object->getComponentsIterator();
  auto itEnd = object->getComponentsEnd();
  auto size = object->componentsSize();
  for (; it != itEnd; it++)
  {
    auto component = it->get();
    if (auto p = dynamic_cast<Primitive*>(component))
    {
      auto notDelete{ true };
      auto open = ImGui::CollapsingHeader(p->typeName(), &notDelete);

      if (!notDelete)
      {
        auto sceneObject = p->sceneObject();
        sceneObject->removeComponent(dynamic_cast<Component*>(p));
        sceneObject->setPrimitive(nullptr);
      }
      else if (open)
        inspectPrimitive(*p);
    }
    else if (auto c = dynamic_cast<Camera*>(component))
    {
      auto notDelete{ true };
      auto open = ImGui::CollapsingHeader(c->typeName(), &notDelete);

      if (!notDelete)
      {
        auto sceneObject = c->sceneObject();
        sceneObject->removeComponent(dynamic_cast<Component*>(c));
        sceneObject->setCamera(nullptr);
      }
      else if (open)
      {
        auto isCurrent = c == Camera::current();

        ImGui::Checkbox("Current", &isCurrent);
        Camera::setCurrent(isCurrent ? c : nullptr);
        inspectCamera(*c);
      }
    }
    else if (auto l = dynamic_cast<Light*>(component))
    {
      auto notDelete{ true };
      auto open = ImGui::CollapsingHeader(l->typeName(), &notDelete);

      if (!notDelete)
      {
        auto sceneObject = l->sceneObject();
        sceneObject->removeComponent(dynamic_cast<Component*>(l));
        sceneObject->setLight(nullptr);
      }
      else if (open)
      {
        inspectLight(*l);
      }

    }
    if (size != object->componentsSize())
    {
      break;
    }
  }
  // **End inspection of temporary components
}

inline void
P4::objectGui()
{
  if (_current == nullptr)
    return;
  if (dynamic_cast<SceneObject*>(_current))
  {
    sceneObjectGui();
    return;
  }
  if (dynamic_cast<Scene*>(_current))
    sceneGui();
}

inline void
P4::inspectorWindow()
{
  ImGui::Begin("Inspector");
  objectGui();
  ImGui::End();
}

inline void
P4::editorViewGui()
{
  if (ImGui::Button("Set Default View"))
    _editor->setDefaultView(float(width()) / float(height()));
  ImGui::Separator();

  auto t = _editor->camera()->transform();
  vec3f temp;

  temp = t->localPosition();
  if (ImGui::DragVec3("Position", temp))
    t->setLocalPosition(temp);
  temp = t->localEulerAngles();
  if (ImGui::DragVec3("Rotation", temp))
    t->setLocalEulerAngles(temp);
  inspectCamera(*_editor->camera());
  ImGui::Separator();
  {
    static int sm;

    ImGui::Combo("Shading Mode", &sm, "None\0Flat\0Gouraud\0\0");
    // TODO

    static Color edgeColor;
    static bool showEdges;

    ImGui::ColorEdit3("Edges", edgeColor);
    ImGui::SameLine();
    ImGui::Checkbox("###showEdges", &showEdges);
  }
  ImGui::Separator();
  ImGui::Checkbox("Show Ground", &_editor->showGround);
}

inline void
P4::assetsWindow()
{
  ImGui::Begin("Assets");
  if (ImGui::CollapsingHeader("Meshes"))
  {
    auto& meshes = Assets::meshes();

    for (auto mit = meshes.begin(); mit != meshes.end(); ++mit)
    {
      auto meshName = mit->first.c_str();
      auto selected = false;

      ImGui::Selectable(meshName, &selected);
      if (ImGui::BeginDragDropSource())
      {
        Assets::loadMesh(mit);
        ImGui::Text(meshName);
        ImGui::SetDragDropPayload("PrimitiveMesh", &mit, sizeof(mit));
        ImGui::EndDragDropSource();
      }
    }
  }
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Textures"))
  {
    // next semester
  }
  ImGui::End();
}

inline void
P4::editorView()
{
  if (!_showEditorView)
    return;
  ImGui::Begin("Editor View Settings");
  editorViewGui();
  ImGui::End();
}

inline void
P4::fileMenu()
{
  if (ImGui::MenuItem("New"))
  {
    // TODO
  }
  if (ImGui::MenuItem("Open...", "Ctrl+O"))
  {
    // TODO
  }
  ImGui::Separator();
  if (ImGui::MenuItem("Save", "Ctrl+S"))
  {
    // TODO
  }
  if (ImGui::MenuItem("Save As..."))
  {
    // TODO
  }
  ImGui::Separator();
  if (ImGui::MenuItem("Exit", "Alt+F4"))
  {
    shutdown();
  }
}

inline bool
showStyleSelector(const char* label)
{
  static int style = 1;

  if (!ImGui::Combo(label, &style, "Classic\0Dark\0Light\0"))
    return false;
  switch (style)
  {
  case 0: ImGui::StyleColorsClassic();
    break;
  case 1: ImGui::StyleColorsDark();
    break;
  case 2: ImGui::StyleColorsLight();
    break;
  }
  return true;
}

inline void
P4::showOptions()
{
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
  showStyleSelector("Color Theme##Selector");
  ImGui::ColorEdit3("Selected Wireframe", _selectedWireframeColor);
  ImGui::PopItemWidth();
}

inline void
P4::mainMenu()
{
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      fileMenu();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View"))
    {
      if (Camera::current() == 0)
        ImGui::MenuItem("Edit View", nullptr, true, false);
      else
      {
        static const char* viewLabels[]{ "Editor", "Renderer" };

        if (ImGui::BeginCombo("View", viewLabels[_viewMode]))
        {
          for (auto i = 0; i < IM_ARRAYSIZE(viewLabels); ++i)
          {
            if (ImGui::Selectable(viewLabels[i], _viewMode == i))
              _viewMode = (ViewMode)i;
          }
          ImGui::EndCombo();
          // TODO: change mode only if scene has changed
          if (_viewMode == ViewMode::Editor)
            _image = nullptr;
        }
      }
      ImGui::Separator();
      ImGui::MenuItem("Assets Window", nullptr, &_showAssets);
      ImGui::MenuItem("Editor View Settings", nullptr, &_showEditorView);
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Tools"))
    {
      if (ImGui::BeginMenu("Options"))
      {
        showOptions();
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Examples"))
    {
      if (ImGui::MenuItem("Scene 1"))
      {
        _scene->clearScene();
        buildScene();
      }
      if (ImGui::MenuItem("Scene 2"))
      {
        _scene->clearScene();
        buildScene2();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void
P4::gui()
{
  mainMenu();
  if (_viewMode == ViewMode::Renderer)
    return;
  hierarchyWindow();
  inspectorWindow();
  assetsWindow();
  editorView();
}

inline void
drawMesh(GLMesh* mesh, GLuint mode)
{
  glPolygonMode(GL_FRONT_AND_BACK, mode);
  glDrawElements(GL_TRIANGLES, mesh->vertexCount(), GL_UNSIGNED_INT, 0);
}

inline void
P4::drawPrimitive(Primitive& primitive)
{
  auto mesh = primitive.mesh();

  auto m = glMesh(primitive.mesh());

  if (nullptr == m)
    return;

  auto t = primitive.transform();
  auto normalMatrix = mat3f{ t->worldToLocalMatrix() }.transposed();

  _programG.setUniformVec4("material.ambient", primitive.material.ambient);
  _programG.setUniformVec4("material.diffuse", primitive.material.diffuse);
  _programG.setUniformVec4("material.spot", primitive.material.spot);
  _programG.setUniform("material.shine", primitive.material.shine);

  _programG.setUniformMat4("transform", t->localToWorldMatrix());
  _programG.setUniformMat3("normalMatrix", normalMatrix);
  _programG.setUniformVec4("ambientLight", _scene->ambientLight);
  _programG.setUniform("flatMode", (int)0);
  /*_programG.setUniformVec3("camPos", Camera::current);*/


  m->bind();
  drawMesh(m, GL_FILL);

  //// **Begin BVH test
  //auto bvh = bvhMap[mesh];

  //if (bvh == nullptr)
  //  bvhMap[mesh] = bvh = new BVH{ *mesh, 16 };
  //// **End BVH test


  if (primitive.sceneObject() != _current)
    return;
  _programG.setUniformVec4("material.diffuse", _selectedWireframeColor);
  _programG.setUniform("flatMode", (int)1);
  drawMesh(m, GL_LINE);

  /*bvh->iterate([this, t](const BVHNodeInfo& node)
    {
      _editor->setLineColor(node.isLeaf ? Color::yellow : Color::magenta);
      _editor->drawBounds(node.bounds, t->localToWorldMatrix());
    });*/


}

inline void
P4::preview() {
  GLint previousViewPort[4];
  glGetIntegerv(GL_VIEWPORT, previousViewPort);
  glViewport(9, 9, 322, 182);
  glEnable(GL_SCISSOR_TEST);
  glScissor(9, 9, 322, 182);



  glClearColor(0, 0, 0, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  glViewport(10, 10, 320, 180);
  glScissor(10, 10, 320, 180);
  /*if (auto obj = dynamic_cast<SceneObject*>(_current))
  {*/
  /*if (obj->camera())*/
    /*_renderer->setCamera(obj->camera());
  _renderer->setImageSize(width(), height());*/
  _renderer->setProgram(&_programP);
  _renderer->render();
  _programG.use();

  //}
  glDisable(GL_SCISSOR_TEST);
  glViewport(previousViewPort[0], previousViewPort[1], previousViewPort[2], previousViewPort[3]);
}


inline void
P4::drawLight(Light& light)
{
  SceneObject* obj = light.sceneObject();
  vec3f pos = obj->transform()->position();
  vec3f locPos = obj->transform()->localPosition();
  mat4f ltw = obj->transform()->localToWorldMatrix();
  mat4f wtl = obj->transform()->worldToLocalMatrix();
  _editor->setVectorColor(light.color);

  vec3f normalx = { 1,0,0 };
  vec3f normaly = { 0,1,0 };
  vec3f normalz = { 0,0,1 };

  normalx = ltw.transformVector(normalx);
  normaly = ltw.transformVector(normaly);
  normalz = ltw.transformVector(normalz);

  if (light.type() == Light::Type::Point)
  {
    _editor->drawVector(pos, normalx * -1, 1.2);
    _editor->drawVector(pos, normalx, 1.2);
    _editor->drawVector(pos, normaly * -1, 1.2);
    _editor->drawVector(pos, normaly, 1.2);
    _editor->drawVector(pos, normalz * -1, 1.2);
    _editor->drawVector(pos, normalz, 1.2);

    _editor->drawVector(pos, (normalx + normaly) * 0.5, 0.6);
    _editor->drawVector(pos, (normalx + normalz) * 0.5, 0.6);
    _editor->drawVector(pos, (normaly + normalz) * 0.5, 0.6);
    _editor->drawVector(pos, (normalx + normaly) * -0.5, 0.6);
    _editor->drawVector(pos, (normalx + normalz) * -0.5, 0.6);
    _editor->drawVector(pos, (normaly + normalz) * -0.5, 0.6);

    _editor->drawVector(pos, (normalx + normaly * -1) * 0.5, 0.6);
    _editor->drawVector(pos, (normalx + normalz * -1) * 0.5, 0.6);
    _editor->drawVector(pos, (normaly + normalz * -1) * 0.5, 0.6);

    _editor->drawVector(pos, (normalx * -1 + normaly) * 0.5, 0.6);
    _editor->drawVector(pos, (normalx * -1 + normalz) * 0.5, 0.6);
    _editor->drawVector(pos, (normaly * -1 + normalz) * 0.5, 0.6);
  }
  if (light.type() == Light::Type::Directional)
  {
    vec3f dirPoints[5];
    dirPoints[0] = { 0,0,0 };
    dirPoints[1] = { -0.5,0,-0.5 };
    dirPoints[2] = { -0.5,0,0.5 };
    dirPoints[3] = { 0.5,0,0.5 };
    dirPoints[4] = { 0.5,0,-0.5 };

    for (int i = 0; i < 5; i++)
    {
      _editor->drawLine(ltw.transform(dirPoints[i]), ltw.transform(dirPoints[i]) + normaly * -5);
      auto cone = GLGraphics3::cone();
      _editor->drawMesh(*cone, ltw.transform(dirPoints[i]) + normaly * -5, mat3f{ ltw }, vec3f{ -0.1f, -0.4f, -0.1f });

    }
  }
  if (light.type() == Light::Type::Spot)
  {
    float coneHeight = 5.0f;

    float hip = coneHeight / std::cos(math::toRadians(light.gammaL()));

    float co = std::sin(math::toRadians(light.gammaL())) * hip;
    vec3f extremes[4];
    extremes[0] = pos + (normaly * -coneHeight) + (normalx * co);
    extremes[1] = pos + (normaly * -coneHeight) + (normalz * co);
    extremes[2] = pos + (normaly * -coneHeight) + (normalx * co * -1);
    extremes[3] = pos + (normaly * -coneHeight) + (normalz * co * -1);
    for (int i = 0; i < 4; i++)
    {
      _editor->drawLine(pos, extremes[i]);
    }
    _editor->drawCircle(-obj->transform()->up() * coneHeight + pos, co, -normaly * coneHeight);
  }
}


inline void
P4::drawCamera(Camera& camera)
{

  if (auto obj = dynamic_cast<SceneObject*>(_current))
  {
    if (obj->camera())
    {
      auto mane = obj->camera();
      float frontPlaneHeight;
      float frontPlaneWidth;
      float backPlaneHeight;
      float backPlaneWidth;

      backPlaneHeight = 2 * camera.B() * tanf(camera.viewAngle() * M_PI / 360);
      backPlaneWidth = camera.aspectRatio() * backPlaneHeight;
      frontPlaneHeight = 2 * camera.F() * tanf(camera.viewAngle() * M_PI / 360);
      frontPlaneWidth = camera.aspectRatio() * frontPlaneHeight;
      mat4f locToWor = camera.cameraToWorldMatrix();


      if (camera.projectionType() == Camera::ProjectionType::Parallel)
      {
        vec3f p1 = locToWor.transform(vec3f{ frontPlaneWidth / 2, frontPlaneHeight / 2, -camera.B() });
        vec3f p2 = locToWor.transform(vec3f{ frontPlaneWidth / 2, -frontPlaneHeight / 2, -camera.B() });
        vec3f P4 = locToWor.transform(vec3f{ -frontPlaneWidth / 2, -frontPlaneHeight / 2, -camera.B() });
        vec3f p4 = locToWor.transform(vec3f{ -frontPlaneWidth / 2, frontPlaneHeight / 2, -camera.B() });

        _editor->drawLine(p1, p2);

        _editor->drawLine(p2, P4);

        _editor->drawLine(P4, p4);

        _editor->drawLine(p4, p1);

        vec3f p5 = locToWor.transform(vec3f{ frontPlaneWidth / 2, frontPlaneHeight / 2, -camera.F() });
        vec3f p6 = locToWor.transform(vec3f{ frontPlaneWidth / 2, -frontPlaneHeight / 2, -camera.F() });
        vec3f p7 = locToWor.transform(vec3f{ -frontPlaneWidth / 2, -frontPlaneHeight / 2, -camera.F() });
        vec3f p8 = locToWor.transform(vec3f{ -frontPlaneWidth / 2, frontPlaneHeight / 2, -camera.F() });

        _editor->drawLine(p5, p6);

        _editor->drawLine(p6, p7);

        _editor->drawLine(p7, p8);

        _editor->drawLine(p8, p5);


        //frustum Connecting Lines 
        _editor->drawLine(p1, p5);

        _editor->drawLine(p2, p6);

        _editor->drawLine(P4, p7);

        _editor->drawLine(p4, p8);

      }
      else
      {
        //Back Plane

        vec3f p1 = locToWor.transform(vec3f{ backPlaneWidth / 2, backPlaneHeight / 2, -camera.B() });
        vec3f p2 = locToWor.transform(vec3f{ backPlaneWidth / 2, -backPlaneHeight / 2, -camera.B() });
        vec3f P4 = locToWor.transform(vec3f{ -backPlaneWidth / 2, -backPlaneHeight / 2, -camera.B() });
        vec3f p4 = locToWor.transform(vec3f{ -backPlaneWidth / 2, backPlaneHeight / 2, -camera.B() });


        _editor->drawLine(p1, p2);

        _editor->drawLine(p2, P4);

        _editor->drawLine(P4, p4);

        _editor->drawLine(p4, p1);


        //Front Plane
        vec3f p5 = locToWor.transform(vec3f{ frontPlaneWidth / 2, frontPlaneHeight / 2, -camera.F() });
        vec3f p6 = locToWor.transform(vec3f{ frontPlaneWidth / 2, -frontPlaneHeight / 2, -camera.F() });
        vec3f p7 = locToWor.transform(vec3f{ -frontPlaneWidth / 2, -frontPlaneHeight / 2, -camera.F() });
        vec3f p8 = locToWor.transform(vec3f{ -frontPlaneWidth / 2, frontPlaneHeight / 2, -camera.F() });

        _editor->drawLine(p5, p6);

        _editor->drawLine(p6, p7);

        _editor->drawLine(p7, p8);

        _editor->drawLine(p8, p5);

        //frustum Connecting Lines 
        _editor->drawLine(p1, p5);

        _editor->drawLine(p2, p6);

        _editor->drawLine(P4, p7);

        _editor->drawLine(p4, p8);
      }
    }
  }
}


inline void
P4::renderScene()
{
  if (auto camera = Camera::current())
  {
    if (_image == nullptr)
    {
      const auto w = width(), h = height();

      _image = new GLImage{ w, h };
      _rayTracer->setImageSize(w, h);
      _rayTracer->setCamera(camera);
      _rayTracer->renderImage(*_image);
    }
    _image->draw(0, 0);
  }
}

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

void
P4::loadLights()
{
  auto lit = _scene->getSceneLightsIterator();
  auto lend = _scene->getSceneLightsEnd();
  int cont = 0;
  int size = std::min(_scene->getSceneLightsCounter(), 10);
  std::string attr;
  for (; lit != lend && cont < size; lit++)
  {
    attr = "lights[" + std::to_string(cont) + "].";
    _programG.setUniform((attr + "type").c_str(), lit->get()->type());
    _programG.setUniform((attr + "fl").c_str(), lit->get()->fl());
    _programG.setUniform((attr + "gammaL").c_str(), lit->get()->gammaL());
    _programG.setUniform((attr + "decayExponent").c_str(), lit->get()->decayExponent());
    _programG.setUniformVec3((attr + "lightPosition").c_str(), lit->get()->sceneObject()->transform()->position());
    _programG.setUniformVec4((attr + "lightColor").c_str(), lit->get()->color);
    _programG.setUniformVec3((attr + "direction").c_str(), lit->get()->sceneObject()->transform()->rotation() * vec3f(0, -1, 0));

    cont++;
  }

  _programG.setUniform("numLights", (int)size);
}

void
P4::render()
{
  _programG.use();
  if (_viewMode == ViewMode::Renderer)
  {
    renderScene();
    return;
  }
  if (_moveFlags)
  {
    const auto delta = _editor->orbitDistance() * CAMERA_RES;
    auto d = vec3f::null();

    if (_moveFlags.isSet(MoveBits::Forward))
      d.z -= delta;
    if (_moveFlags.isSet(MoveBits::Back))
      d.z += delta;
    if (_moveFlags.isSet(MoveBits::Left))
      d.x -= delta;
    if (_moveFlags.isSet(MoveBits::Right))
      d.x += delta;
    if (_moveFlags.isSet(MoveBits::Up))
      d.y += delta;
    if (_moveFlags.isSet(MoveBits::Down))
      d.y -= delta;
    _editor->pan(d);
  }
  _editor->newFrame();

  // **Begin rendering of temporary scene objects
  // It should be replaced by your rendering code (and moved to scene editor?)
  auto ec = _editor->camera();
  const auto& p = ec->transform()->position();
  auto vp = vpMatrix(ec);

  _programG.setUniformMat4("vpMatrix", vp);
  _programG.setUniformVec4("ambientLight", _scene->ambientLight);
  _programG.setUniformVec3("camPos", ec->transform()->position());
  //_programG.setUniformVec3("lightPosition", p);
  
  loadLights();


  auto it = _scene->getScenePrimitiveIterator();
  auto end = _scene->getScenePrimitiveEnd();
  Camera* cam = nullptr;
  for (; it != end; it++)
  {
    auto o = it->get()->sceneObject();
    if (auto p = dynamic_cast<Primitive*>(it->get()))
    {
      drawPrimitive(*p);
    }
    else if (auto c = dynamic_cast<Camera*>(it->get()))
    {
      if (_current == o)
      {
        drawCamera(*c);
        cam = c;
        _renderer->setCamera(c);
        _renderer->setImageSize(width(), height());
      }
      _programG.use();
    }
    if (auto l = dynamic_cast<Light*>(it->get()))
    {
      if (_current == o)
      {
        drawLight(*l);
      }
    }
    if (o == _current)
    {
      auto t = o->transform();
      _editor->drawAxes(t->position(), mat3f{ t->rotation() });
    }
  }
  if (cam)
  {
    preview();
  }

}

bool
P4::windowResizeEvent(int width, int height)
{
  _editor->camera()->setAspectRatio(float(width) / float(height));
  _viewMode = ViewMode::Editor;
  _image = nullptr;
  return true;
}

bool
P4::keyInputEvent(int key, int action, int mods)
{
  auto active = action != GLFW_RELEASE && mods == GLFW_MOD_ALT;

  switch (key)
  {
  case GLFW_KEY_W:
    _moveFlags.enable(MoveBits::Forward, active);
    break;
  case GLFW_KEY_S:
    _moveFlags.enable(MoveBits::Back, active);
    break;
  case GLFW_KEY_A:
    _moveFlags.enable(MoveBits::Left, active);
    break;
  case GLFW_KEY_D:
    _moveFlags.enable(MoveBits::Right, active);
    break;
  case GLFW_KEY_Q:
    _moveFlags.enable(MoveBits::Up, active);
    break;
  case GLFW_KEY_Z:
    _moveFlags.enable(MoveBits::Down, active);
    break;
  }
  return false;
}

bool
P4::scrollEvent(double, double yOffset)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  _editor->zoom(yOffset < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
  return true;
}

bool
P4::mouseButtonInputEvent(int button, int actions, int mods)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  (void)mods;

  auto active = actions == GLFW_PRESS;

  if (button == GLFW_MOUSE_BUTTON_LEFT)
  {
    if (active)
    {
      cursorPosition(_pivotX, _pivotY);

      const auto ray = makeRay(_pivotX, _pivotY);
      auto minDistance = math::Limits<float>::inf();

      // **Begin picking of temporary scene objects
      // It should be replaced by your picking code
      auto it = _scene->getScenePrimitiveIterator();
      auto end = _scene->getScenePrimitiveEnd();
      Intersection hit;
      for (; it != end; it++)
      {
        auto component = it->get();

        float distance;

        if (auto p = dynamic_cast<Primitive*>(component))
          if (p->intersect(ray, hit) && hit.distance < minDistance)
          {
            minDistance = hit.distance;
            _current = component->sceneObject();
          }
      }
      // **End picking of temporary scene objects
    }
    return true;
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT)
    _dragFlags.enable(DragBits::Rotate, active);
  else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    _dragFlags.enable(DragBits::Pan, active);
  if (_dragFlags)
    cursorPosition(_pivotX, _pivotY);
  return true;
}

bool
P4::mouseMoveEvent(double xPos, double yPos)
{
  if (!_dragFlags)
    return false;
  _mouseX = (int)xPos;
  _mouseY = (int)yPos;

  const auto dx = (_pivotX - _mouseX);
  const auto dy = (_pivotY - _mouseY);

  _pivotX = _mouseX;
  _pivotY = _mouseY;
  if (dx != 0 || dy != 0)
  {
    if (_dragFlags.isSet(DragBits::Rotate))
    {
      const auto da = -_editor->camera()->viewAngle() * CAMERA_RES;
      isKeyPressed(GLFW_KEY_LEFT_ALT) ?
        _editor->orbit(dy * da, dx * da) :
        _editor->rotateView(dy * da, dx * da);
    }
    if (_dragFlags.isSet(DragBits::Pan))
    {
      const auto dt = -_editor->orbitDistance() * CAMERA_RES;
      _editor->pan(-dt * math::sign(dx), dt * math::sign(dy), 0);
    }
  }
  return true;
}
