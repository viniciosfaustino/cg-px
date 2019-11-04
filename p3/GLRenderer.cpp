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
// OVERVIEW: GLRenderer.cpp
// ========
// Source file for OpenGL renderer.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 09/09/2019

#include "GLRenderer.h"

namespace cg
{ // begin namespace cg


//////////////////////////////////////////////////////////
//
// GLRenderer implementation
// ==========
  void
    GLRenderer::update()
  {
    Renderer::update();
    // TODO
  }

  inline void
    GLRenderer::drawMesh(GLMesh* mesh, GLuint mode)
  {
    glPolygonMode(GL_FRONT_AND_BACK, mode);
    glDrawElements(GL_TRIANGLES, mesh->vertexCount(), GL_UNSIGNED_INT, 0);
  }


  inline void
    GLRenderer::drawPrimitive(Primitive& primitive)
  {
    auto program = getProgram();

    auto m = glMesh(primitive.mesh());

    if (nullptr == m)
      return;

    auto t = primitive.transform();
    auto normalMatrix = mat3f{ t->worldToLocalMatrix() }.transposed();

    program->setUniformMat4("transform", t->localToWorldMatrix());
    program->setUniformMat3("normalMatrix", normalMatrix);

    program->setUniformVec4("material.ambient", primitive.material.ambient);
    program->setUniformVec4("material.diffuse", primitive.material.diffuse);
    program->setUniformVec4("material.spot", primitive.material.spot);
    program->setUniform("material.shine", primitive.material.shine);
    //sapega vai dar ruim
    //deu mesmo
    //program->setUniformVec4("color", primitive.material.ambient);

    auto lit = _scene->getSceneLightsIterator();
    auto lend = _scene->getSceneLightsEnd();
    int cont = 0;
    int size = std::min(_scene->getSceneLightsCounter(), 10);
    std::string attr;
    for (; lit != lend && cont < size; lit++)
    {
      attr = "lights[" + std::to_string(cont) + "].";
      program->setUniform((attr + "type").c_str(), lit->get()->type());
      program->setUniform((attr + "fl").c_str(), lit->get()->fl());
      program->setUniform((attr + "gammaL").c_str(), lit->get()->gammaL());
      program->setUniform((attr + "decayExponent").c_str(), lit->get()->decayExponent());
      program->setUniformVec3((attr + "lightPosition").c_str(), lit->get()->sceneObject()->transform()->position());
      program->setUniformVec4((attr + "lightColor").c_str(), lit->get()->color);
      program->setUniformVec3((attr + "direction").c_str(), lit->get()->sceneObject()->transform()->rotation() * vec3f(0, -1, 0));

      cont++;
    }

    program->setUniform("numLights", (int)size);

    program->setUniform("flatMode", (int)0);
    m->bind();
    drawMesh(m, GL_FILL);
  }


  void GLRenderer::renderRecursive(Reference<SceneObject> parent)
  {
    auto it = parent->getIterator();
    auto end = parent->getIteratorEnd();
    while (it != end)
    {
      if (it->get()->childrenSize() > 0)
      {
        renderRecursive(it->get());
      }
      else {
        auto o = it->get();
        if (!o->visible)
          continue;

        auto it = o->getComponentsIterator();
        auto itEnd = o->getComponentsEnd();
        while (it != itEnd)
        {
          auto component = it->get();
          if (auto p = dynamic_cast<Primitive*>(component))
            drawPrimitive(*p);
          it++;
        }
      }
      it++;
    }
  }

  void
    GLRenderer::render()
  {
    const auto& bc = _scene->backgroundColor;
    auto program = getProgram();
    program->use();

    glClearColor(bc.r, bc.g, bc.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    auto ec = camera();
    const auto& p = ec->transform()->position();
    auto vp = vpMatrix(ec);    

    program->setUniformMat4("vpMatrix", vp);    

    program->setUniformVec4("ambientLight", _scene->ambientLight);
    program->setUniformVec3("camPos", ec->transform()->position());


    renderRecursive(_scene->root());
  }

} // end namespace cg
