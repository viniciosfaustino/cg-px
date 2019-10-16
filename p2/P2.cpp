#include "geometry/MeshSweeper.h"
#include "P2.h"
#include <iostream>

MeshMap P2::_defaultMeshes;

inline void
P2::buildDefaultMeshes()
{
  _defaultMeshes["None"] = nullptr;
  _defaultMeshes["Box"] = GLGraphics3::box();
  _defaultMeshes["Sphere"] = GLGraphics3::sphere();
}

inline Primitive*
makePrimitive(MeshMapIterator mit)
{
  return new Primitive{mit->second, mit->first};
}



inline void
P2::buildScene()
{
  int boxCounter = 0;
  _current = _scene = new Scene{"Scene 1"};
  _editor = new SceneEditor{*_scene};
  _editor->setDefaultView((float)width() / (float)height());

  auto o = new SceneObject{ "Main Camera", _scene };
  
  auto camera = new Camera;
  o->setCamera(camera);

  o->addComponent(camera);
  o->setParent(_scene->root());
  _objects.push_back(o);
  _scene->root()->addChild(o);
  Camera::setCurrent(camera);

  Reference<SceneObject> _box = new SceneObject{ "Box1", _scene };
  int rootLevel = 3;
  int subLevel = 2;
  for (int i = 0; i < rootLevel; i++)
  {
    createNewObject(false, "Box");
  }

  // **Begin initialization of temporary attributes
  // It should be replaced by your scene initialization  
  
}

void
P2::initialize()
{
  Application::loadShaders(_program, "shaders/p2.vs", "shaders/p2.fs");
  Assets::initialize();
  buildDefaultMeshes();
  buildScene();
  _renderer = new GLRenderer{*_scene};
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  glEnable(GL_LINE_SMOOTH);
  _program.use();
}

namespace ImGui
{
  void ShowDemoWindow(bool*);
}

inline void
P2::createNewObject(bool empty, std::string shape)
{
  static int boxCounter = 0;
  static int objectCounter = 0;
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

  name = (empty) ? "Object " + std::to_string(objectCounter) : shape + " " + std::to_string(boxCounter);

  auto child = new SceneObject{ name.c_str(), _scene };
  if (!empty)
  {
    auto p1 = makePrimitive(_defaultMeshes.find(shape));
    child->addComponent(p1);
    _scene->addScenePrimitive(p1);
    child->setPrimitive((Reference<Primitive>)p1);
    boxCounter++;
  }
  else
  {
    objectCounter++;
  }

  parent->addChild(child);  
  _objects.push_back(child);
}

inline void
P2::removeObjectRecursive(Reference<SceneObject> object)
  {
    auto it = _objects.begin();
    auto it_end = _objects.end();
    bool found = false;
    while (!found && it != it_end)
    {
      if (it->get() == object)
      {
        found = true;
        auto cIt = object->getIterator();
        auto cEnd = object->getIteratorEnd();
        while ((object->childrenSize() > 0) && (cIt != cEnd))
        {
          auto aux = cIt;
          aux++;
          removeObjectRecursive(cIt->get());
          cIt = aux;
        }
        auto p = it->get()->primitive();
        if (p)
        {          
          _scene->removeScenePrimitive(p);
        }        
        _objects.erase(it);
      }
      else
      {
        it++;
      }
    }
  }


inline void
P2::hierarchyWindow()
{
  static int boxCounter = 8;
  ImGui::Begin("Hierarchy");
  if (ImGui::Button("Create###object"))
    ImGui::OpenPopup("CreateObjectPopup");
  if (ImGui::BeginPopup("CreateObjectPopup"))
  {
    if (ImGui::MenuItem("Empty Object"))
    {
      createNewObject(true, "");
    }
    if (ImGui::BeginMenu("3D Object"))
    {
      if (ImGui::MenuItem("Box"))
      {
        createNewObject(false, "Box");
      }
      if (ImGui::MenuItem("Sphere"))
      {
        createNewObject(false, "Sphere");
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
      auto parent = aux->parent();
      _current = parent;
      removeObjectRecursive(aux);
      parent->removeChildRecursive(parent, aux);
    }
  }
  ImGui::Separator();


  ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_OpenOnArrow };
  auto open = ImGui::TreeNodeEx(_scene,
    _current == _scene ? flag | ImGuiTreeNodeFlags_Selected : flag,
    _scene->name());

  if (ImGui::IsItemClicked())
    _current = _scene;

  recursionTree(open, _scene->root());

  ImGui::End();
  //chamar fun��o recursiva que popa os object scenes na tela
}

namespace ImGui
{ // begin namespace ImGui

void
ObjectNameInput(NameableObject* object)
{
  const int bufferSize{128};
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
P2::sceneGui()
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
P2::showTransform(Reference<Transform> component)
{
  if (ImGui::CollapsingHeader(component->typeName()))
  {
    //auto t = object->transform();

    ImGui::TransformEdit(component);
    //_transform = component->localToWorldMatrix();
  }
}
inline void
P2::showPrimitive(Reference<Primitive> component)
{
  if (ImGui::CollapsingHeader(component->typeName()))
  {
    //auto t = object->transform();

    //ImGui::Text((std::string{ "Quantidade de Vertices: " }+std::to_string(component->mesh()->vertexCount())).c_str());

  }

}

inline void
P2::inspectPrimitive(Primitive& primitive)
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
  ImGui::ColorEdit3("Mesh Color", (float*)&primitive.color);

  if (ImGui::Button("Delete"))
  {
    auto sceneObject = primitive.sceneObject();
    sceneObject->removeComponent(dynamic_cast<Component*>(&primitive));    
    sceneObject->setPrimitive(nullptr);
    _scene->removeScenePrimitive(&primitive);
  }

}

void
P2::inspectCamera(Camera& camera)
{
  static const char* projectionNames[]{"Perspective", "Orthographic"};
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
P2::addComponentButton(SceneObject& object)
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
          _scene->addScenePrimitive(p);
          object.setPrimitive((Reference<Primitive>)p);
        }

        if (ImGui::MenuItem("Sphere"))
        {
          auto p = makePrimitive(_defaultMeshes.find("Sphere"));
          object.addComponent(p);
          _scene->addScenePrimitive(p);          
          object.setPrimitive((Reference<Primitive>)p);
        }
      }

      ImGui::EndMenu();
    }
    ImGui::EndPopup();
  }
  ImGui::Separator();
}

inline void
P2::removeComponentButton(SceneObject& object)
{

}
inline void
P2::sceneObjectGui()
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

  // **Begin inspection of temporary components
  // It should be replaced by your component inspection
  auto it = object->getComponentsIterator();
  auto itEnd = object->getComponentsEnd();
  while (it != itEnd)
  {
    auto aux = it;
    aux++;
    auto component = it->get();
    if (auto p = dynamic_cast<Primitive*>(component))
    {
      auto notDelete{ true };
      auto open = ImGui::CollapsingHeader(p->typeName(), &notDelete);

      if (!notDelete)
      {
        // TODO: delete primitive
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
        // TODO: delete camera
      }
      else if (open)
      {
        auto isCurrent = c == Camera::current();

        ImGui::Checkbox("Current", &isCurrent);
        Camera::setCurrent(isCurrent ? c : nullptr);
        inspectCamera(*c);
      }
    }
    it = aux;
  }

  
  // **End inspection of temporary components
}

inline void
P2::objectGui()
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
P2::inspectorWindow()
{
  ImGui::Begin("Inspector");
  objectGui();
  ImGui::End();
}

inline void
P2::editorViewGui()
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
  ImGui::Checkbox("Show Ground", &_editor->showGround);
}

inline void
P2::assetsWindow()
{
  if (!_showAssets)
    return;
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
    // p3
  }
  ImGui::End();
}

inline void
P2::editorView()
{
  if (!_showEditorView)
    return;
  ImGui::Begin("Editor View Settings");
  editorViewGui();
  ImGui::End();
}

inline void
P2::fileMenu()
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
P2::showOptions()
{
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
  showStyleSelector("Color Theme##Selector");
  ImGui::ColorEdit3("Selected Wireframe", _selectedWireframeColor);
  ImGui::PopItemWidth();
}

inline void
P2::mainMenu()
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
        static const char* viewLabels[]{"Editor", "Renderer"};

        if (ImGui::BeginCombo("View", viewLabels[_viewMode]))
        {
          for (auto i = 0; i < IM_ARRAYSIZE(viewLabels); ++i)
          {
            if (ImGui::Selectable(viewLabels[i], _viewMode == i))
              _viewMode = (ViewMode)i;
          }
          ImGui::EndCombo();
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
    ImGui::EndMainMenuBar();
  }
}

void
P2::gui()
{
  mainMenu();
  hierarchyWindow();
  inspectorWindow();
  assetsWindow();
  editorView();

  /*
  static bool demo = true;
  ImGui::ShowDemoWindow(&demo);
  */
}

inline void
drawMesh(GLMesh* mesh, GLuint mode)
{
  glPolygonMode(GL_FRONT_AND_BACK, mode);
  glDrawElements(GL_TRIANGLES, mesh->vertexCount(), GL_UNSIGNED_INT, 0);
}

inline void
P2::drawPrimitive(Primitive& primitive)
{
  auto m = glMesh(primitive.mesh());

  if (nullptr == m)
    return;

  auto t = primitive.transform();
  auto normalMatrix = mat3f{t->worldToLocalMatrix()}.transposed();

  _program.setUniformMat4("transform", t->localToWorldMatrix());
  _program.setUniformMat3("normalMatrix", normalMatrix);
  _program.setUniformVec4("color", primitive.color);
  _program.setUniform("flatMode", (int)0);
  m->bind();
  drawMesh(m, GL_FILL);
  if (primitive.sceneObject() != _current)
    return;
  _program.setUniformVec4("color", _selectedWireframeColor);
  _program.setUniform("flatMode", (int)1);
  drawMesh(m, GL_LINE);
}

inline void
P2::preview() {
	GLint previousViewPort[4];
	glGetIntegerv(GL_VIEWPORT, previousViewPort);
	glViewport(10, 10, 320, 180);
	glEnable(GL_SCISSOR_TEST);
	glScissor(10, 10, 320, 180);
	
	
	
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//glViewport(10, 10, 320, 180);
	//glScissor(10, 10, 320, 180); 
  if (auto obj = dynamic_cast<SceneObject*>(_current))
  {
    if (obj->camera())
      _renderer->setCamera(obj->camera());
      _renderer->setImageSize(width(), height());
      _renderer->setProgram(&_program);
      _renderer->render();
      _program.use();

  }
  
  renderScene();
	glDisable(GL_SCISSOR_TEST);
	glViewport(previousViewPort[0], previousViewPort[1], previousViewPort[2], previousViewPort[3]);
}

inline void
P2::drawCamera(Camera& camera)
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
        vec3f p3 = locToWor.transform(vec3f{ -frontPlaneWidth / 2, -frontPlaneHeight / 2, -camera.B() });
        vec3f p4 = locToWor.transform(vec3f{ -frontPlaneWidth / 2, frontPlaneHeight / 2, -camera.B() });

        _editor->drawLine(p1, p2);

        _editor->drawLine(p2, p3);

        _editor->drawLine(p3, p4);

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

        _editor->drawLine(p3, p7);

        _editor->drawLine(p4, p8);

      }
      else
      {
        //Back Plane

        vec3f p1 = locToWor.transform(vec3f{ backPlaneWidth / 2, backPlaneHeight / 2, -camera.B() });
        vec3f p2 = locToWor.transform(vec3f{ backPlaneWidth / 2, -backPlaneHeight / 2, -camera.B() });
        vec3f p3 = locToWor.transform(vec3f{ -backPlaneWidth / 2, -backPlaneHeight / 2, -camera.B() });
        vec3f p4 = locToWor.transform(vec3f{ -backPlaneWidth / 2, backPlaneHeight / 2, -camera.B() });


        _editor->drawLine(p1, p2);

        _editor->drawLine(p2, p3);

        _editor->drawLine(p3, p4);

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

        _editor->drawLine(p3, p7);

        _editor->drawLine(p4, p8);


      }
    }    
  }	
}

inline void
P2::renderScene()
{
  if (auto camera = Camera::current())
  {
    _renderer->setCamera(camera);
    _renderer->setImageSize(width(), height());
    _renderer->setProgram(&_program);
    _renderer->render();
    _program.use();
    
  }
}

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

void
P2::render()
{  
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

  _program.setUniformMat4("vpMatrix", vp);
  _program.setUniformVec4("ambientLight", _scene->ambientLight);
  _program.setUniformVec3("lightPosition", p);
  bool auxCam = false;
  for (const auto& o : _objects)
  {
    if (!o->visible)
      continue;
    
    auto it = o->getComponentsIterator();
    auto itEnd = o->getComponentsEnd();
    while(it != itEnd)
    {
      auto component = it->get();
      if (auto p = dynamic_cast<Primitive*>(component))
        drawPrimitive(*p);
	    else if (auto c = dynamic_cast<Camera*>(component))
	    {
        auxCam = true;
        if (o == _current)
        {
          if (o->camera())
          {
            drawCamera(*c);            
          }          
        }		    
	    }
      if (o == _current)
      {
        auto t = o->transform();
        _editor->drawAxes(t->position(), mat3f{ t->rotation() });
      }
      it++;
    }
    
  }
  if(auxCam)
	  preview();
  // **End rendering of temporary scene objects
}

bool
P2::windowResizeEvent(int width, int height)
{
  _editor->camera()->setAspectRatio(float(width) / float(height));
  return true;
}
void
P2::cameraFocus()
{
	auto camera = _editor->camera();
	SceneObject* obj = dynamic_cast<SceneObject*>(_current);
	camera->transform()->setPosition(obj->transform()->position());
	camera->transform()->translate(vec3f{ 0,0,5});

}

bool
P2::keyInputEvent(int key, int action, int mods)
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
    case GLFW_KEY_E:
      _moveFlags.enable(MoveBits::Down, active);
	  break;
	case GLFW_KEY_F:
	  cameraFocus();
      break;
  }
  return false;
}

bool
P2::scrollEvent(double, double yOffset)
{
  if (ImGui::GetIO().WantCaptureMouse || _viewMode == ViewMode::Renderer)
    return false;
  _editor->zoom(yOffset < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
  return true;
}

bool
P2::mouseButtonInputEvent(int button, int actions, int mods)
{
  if (ImGui::GetIO().WantCaptureMouse || _viewMode == ViewMode::Renderer)
    return false;
  (void)mods;

  auto active = actions == GLFW_PRESS;

  if (button == GLFW_MOUSE_BUTTON_RIGHT)
    _dragFlags.enable(DragBits::Rotate, active);
  else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    _dragFlags.enable(DragBits::Pan, active);
  if (_dragFlags)
    cursorPosition(_pivotX, _pivotY);
  return true;
}

bool
P2::mouseMoveEvent(double xPos, double yPos)
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
