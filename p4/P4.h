#ifndef __P4_h
#define __P4_h

#include "Assets.h"
#include "BVH.h"

#include "GLRenderer.h"
#include "Light.h"
#include "Primitive.h"
#include "SceneEditor.h"
#include "RayTracer.h"
#include "core/Flags.h"
#include "graphics/Application.h"
#include "graphics/GLImage.h"
#include <vector>

using namespace cg;

class P4 : public GLWindow
{
public:
  P4(int width, int height) :
    GLWindow{ "cg2019 - P4", width, height },
    _programG{ "P4G" },
    _programP{ "P4P" }
  {
    // do nothing
  }

  /// Initialize the app.
  void initialize() override;

  Reference<Light> createLight(cg::Light::Type type);

  void recursionTree(bool open, Reference<SceneObject> object)
  {
    if (open)
    {
      auto it = object->getIterator();
      auto vecEnd = object->getIteratorEnd();
      auto size = object->childrenSize();
      for (; it != vecEnd; it++)
      {
        if ((*it)->childrenSize() != 0)
        {
          auto flag = ImGuiTreeNodeFlags_OpenOnArrow; //flag pra saber que tem filhos
          open = ImGui::TreeNodeEx((*it),
            _current == (*it) ? flag | ImGuiTreeNodeFlags_Selected : flag,
            (*it)->name());

          if (ImGui::IsItemClicked())//serve para colocar a seleção sobre o objeto clicado
            _current = (*it); //todos que estão em current recebem a seleção

          dragNDrop(*it);

          recursionTree(open, (*it));
        }
        else /*Não tem filhos*/
        {
          auto flag = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

          open = ImGui::TreeNodeEx((*it),
            _current == (*it) ? flag | ImGuiTreeNodeFlags_Selected : flag,
            (*it)->name());

          if (ImGui::IsItemClicked())//serve para colocar a seleção sobre o objeto clicado
            _current = (*it); //todos que estão em current recebem a seleção

          dragNDrop(*it);

        }

        if (size != object->childrenSize())
          break;
      }

      ImGui::TreePop(); //serve pra mostrar efetivamente o role
    }
  }

  void removeObject(Reference<SceneObject> object);

  void dragNDrop(SceneObject* obj);

  /// Update the GUI.
  void gui() override;

  void loadLights();

  /// Render the scene.
  void render() override;

private:
  enum ViewMode
  {
    Editor = 0,
    Renderer = 1
  };

  enum class MoveBits
  {
    Left = 1,
    Right = 2,
    Forward = 4,
    Back = 8,
    Up = 16,
    Down = 32
  };

  enum class DragBits
  {
    Rotate = 1,
    Pan = 2
  };

  enum class SceneObjectType
  {
    empty,
    shape,
    camera,
    spot,
    point,
    directional
  };

  using BVHRef = Reference<BVH>;
  using BVHMap = std::map<TriangleMesh*, BVHRef>;


  GLSL::Program _programG, _programP;
  Reference<Scene> _scene;
  Reference<SceneEditor> _editor;
  Reference<GLRenderer> _renderer;  
  std::vector<Reference<SceneObject>> _objects;
 
  SceneNode* _current{};
  Color _selectedWireframeColor{ 255, 102, 0 };
  Flags<MoveBits> _moveFlags{};
  Flags<DragBits> _dragFlags{};
  int _pivotX;
  int _pivotY;
  int _mouseX;
  int _mouseY;
  bool _showAssets{ true };
  bool _showEditorView{ true };
  ViewMode _viewMode{ ViewMode::Editor };
  Reference<RayTracer> _rayTracer;
  Reference<GLImage> _image;
  BVHMap bvhMap;


  static MeshMap _defaultMeshes;

  void buildScene();
  void buildScene1();
  void buildScene2();
  void buildScene3();
  void buildScene5();
  void renderScene();

  void mainMenu();
  void fileMenu();
  void showOptions();

  void hierarchyWindow();
  void inspectorWindow();
  void assetsWindow();
  void editorView();
  void sceneGui();
  void sceneObjectGui();
  void objectGui();
  void editorViewGui();
  void inspectPrimitive(Primitive&);
  void inspectShape(Primitive&);
  void inspectMaterial(Material&);
  void inspectLight(Light&);
  void inspectCamera(Camera&);
  void addComponentButton(SceneObject&);

  void drawPrimitive(Primitive&);
  void preview();
  void drawLight(Light&);
  void drawCamera(Camera&);

  bool windowResizeEvent(int, int) override;
  bool keyInputEvent(int, int, int) override;
  bool scrollEvent(double, double) override;
  bool mouseButtonInputEvent(int, int, int) override;
  bool mouseMoveEvent(double, double) override;

  Ray makeRay(int, int) const;

  static void buildDefaultMeshes();
  void createNewObject(SceneObjectType type, std::string shape);
  void setObj(Reference<Scene> o, vec3f localPos, vec3f localScale, vec3f rotate, Reference<Scene>_scene);

}; // P4

#endif // __P4_h

