#ifndef __P2_h
#define __P2_h

#include "Assets.h"
#include "GLRenderer.h"
#include "Primitive.h"
#include "SceneEditor.h"
#include "core/Flags.h"
#include "graphics/Application.h"
#include <vector>

using namespace cg;

class P2 : public GLWindow
{
public:
  P2(int width, int height) :
    GLWindow{ "cg2019 - P2", width, height },
    _program{ "P2" }
  {
    // do nothing
  }

  /// Initialize the app.
  void initialize() override;

  /// Update the GUI.
  void gui() override;

  void recursiveRender();

  void recursiveRender(Reference<SceneObject> obj);

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

  GLSL::Program _program;
  Reference<Scene> _scene;
  Reference<SceneEditor> _editor;
  Reference<GLRenderer> _renderer;
  // **Begin temporary attributes
  // Those are just to show some geometry
  // They should be replaced by your scene hierarchy
  std::vector<Reference<SceneObject>> _objects;
  // **End temporary attributes
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


  static MeshMap _defaultMeshes;
  // bool isFatherNChild(SceneObject*, SceneObject*);
  void buildScene();
  void renderScene();
  void cameraFocus();
  void preview();
  void dragNDrop(SceneObject*);

  void mainMenu();
  void fileMenu();
  void showOptions();

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

          if (ImGui::IsItemClicked())//serve para colocar a sele��o sobre o objeto clicado
            _current = (*it); //todos que est�o em current recebem a sele��o

          dragNDrop(*it);

          recursionTree(open, (*it));
        }
        else /*N�o tem filhos*/
        {
          auto flag = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

          open = ImGui::TreeNodeEx((*it),
            _current == (*it) ? flag | ImGuiTreeNodeFlags_Selected : flag,
            (*it)->name());

          if (ImGui::IsItemClicked())//serve para colocar a sele��o sobre o objeto clicado
            _current = (*it); //todos que est�o em current recebem a sele��o

          dragNDrop(*it);

        }

        if (size != object->childrenSize())
          break;
      }

      ImGui::TreePop(); //serve pra mostrar efetivamente o role
    }
  }

  void createNewObject(bool empty, std::string shape);

  void removeObjectRecursive(Reference<SceneObject> object);

  void removeObject(Reference<SceneObject> object);

  void hierarchyWindow();
  void inspectorWindow();
  void assetsWindow();
  void editorView();
  void sceneGui();
  void showTransform(Reference<Transform> component);
  void showPrimitive(Reference<Primitive> component);
  void removeComponentButton(SceneObject& object);
  void sceneObjectGui();
  void objectGui();
  void editorViewGui();
  void inspectPrimitive(Primitive&);
  void inspectCamera(Camera&);
  void addComponentButton(SceneObject&);

  void drawPrimitive(Primitive&);
  void drawCamera(Camera&);

  bool windowResizeEvent(int, int) override;
  bool keyInputEvent(int, int, int) override;
  bool scrollEvent(double, double) override;
  bool mouseButtonInputEvent(int, int, int) override;
  bool mouseMoveEvent(double, double) override;

  static void buildDefaultMeshes();

}; // P2

#endif // __P2_h
