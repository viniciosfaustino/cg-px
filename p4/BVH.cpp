//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019 Orthrus Group.                               |
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
// OVERVIEW: BVH.cpp
// ========
// Source file for BVH.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 18/11/2019

#include "BVH.h"
#include <stack>
#include "Primitive.h"
#include "Transform.h"
#include "SceneObject.h"

namespace cg
{ // begin namespace cg

struct BVH::TriangleInfo
{
  int index;
  Bounds3f bounds;
  vec3f centroid;

  TriangleInfo() = default;

  TriangleInfo(int index, const Bounds3f& bounds):
    index{index},
    bounds{bounds},
    centroid{bounds.center()}
  {
    // do nothing
  }

}; // BVH::TriangleInfo

struct BVH::Node
{
  Bounds3f bounds;
  Node* children[2];
  int first;
  int count;

  Node(const Bounds3f& b, int first, int count):
    bounds{b},
    first{first},
    count{count}
  {
    children[0] = children[1] = nullptr;
  }

  Node(Node* c0, Node* c1):
    count{0}
  {
    bounds.inflate(c0->bounds);
    bounds.inflate(c1->bounds);
    children[0] = c0;
    children[1] = c1;
  }

  ~Node()
  {
    delete children[0];
    delete children[1];
  }

  bool isLeaf() const
  {
    return children[0] == nullptr;
  }

  static void iterate(const Node* node, BVHNodeFunction f);

}; // BVH::Node

void
BVH::Node::iterate(const Node* node, BVHNodeFunction f)
{
  if (node == nullptr)
    return;

  auto isLeaf = node->isLeaf();

  f({node->bounds, isLeaf, node->first, node->count});
  if (!isLeaf)
  {
    iterate(node->children[0], f);
    iterate(node->children[1], f);
  }
}

inline BVH::Node*
BVH::makeLeaf(TriangleInfoArray& triangleInfo,
  int start,
  int end,
  TriangleIndexArray& orderedTris)
{
  Bounds3f bounds;
  auto first = int(orderedTris.size());

  for (int i = start; i < end; ++i)
  {
    bounds.inflate(triangleInfo[i].bounds);
    orderedTris.push_back(_triangles[triangleInfo[i].index]);
  }
  return new Node{bounds, first, end - start};
}

inline auto
maxDim(const Bounds3f& b)
{
  auto s = b.size();
  return s.x > s.y && s.x > s.z ? 0 : (s.y > s.z ? 1 : 2);
}

BVH::Node*
BVH::makeNode(TriangleInfoArray& triangleInfo,
  int start,
  int end,
  TriangleIndexArray& orderedTris)
{
  ++_nodeCount;
  if (end - start <= _maxTrisPerNode)
    return makeLeaf(triangleInfo, start, end, orderedTris);

  Bounds3f centroidBounds;

  for (int i = start; i < end; ++i)
    centroidBounds.inflate(triangleInfo[i].centroid);

  auto dim = maxDim(centroidBounds);

  if (centroidBounds.max()[dim] == centroidBounds.min()[dim])
    return makeLeaf(triangleInfo, start, end, orderedTris);

  // Partition tris into two sets and build children
  auto mid = (start + end) / 2;

  std::nth_element(&triangleInfo[start],
    &triangleInfo[mid],
    &triangleInfo[end - 1] + 1,
    [dim] (const TriangleInfo& a, const TriangleInfo& b)
    {
      return a.centroid[dim] < b.centroid[dim];
    });
  return new Node{makeNode(triangleInfo, start, mid, orderedTris),
    makeNode(triangleInfo, mid, end, orderedTris)};
}

BVH::BVH(TriangleMesh& mesh, int maxTrisPerNode):
  _mesh{&mesh},
  _maxTrisPerNode{maxTrisPerNode}
{
  const auto& data = mesh.data();
  int nt{data.numberOfTriangles};

  if (nt == 0)
    return;
  _triangles.resize(nt);

  TriangleInfoArray triangleInfo(nt);

  for (int i = 0; i < nt; ++i)
  {
    _triangles[i] = i;

    auto t = data.triangles + i;
    Bounds3f b;

    b.inflate(data.vertices[t->v[0]]);
    b.inflate(data.vertices[t->v[1]]);
    b.inflate(data.vertices[t->v[2]]);
    triangleInfo[i] = {i, b};
  }

  TriangleIndexArray orderedTris;
  
  orderedTris.reserve(nt);
  _root = makeNode(triangleInfo, 0, nt, orderedTris);
  _triangles.swap(orderedTris);
#ifdef _DEBUG
  if (true)
  {
    mesh.bounds().print("Mesh bounds:");
    printf("Mesh triangles: %d\n", nt);
    bounds().print("BVH bounds:");
    printf("BVH nodes: %d\n", _nodeCount);
    iterate([this] (const BVHNodeInfo& node)
    {
      if (!node.isLeaf)
        return;
      node.bounds.print("Leaf bounds:");
      printf("Leaf triangles: %d\n", node.numberOfTriangles);
      //for (int i = 0; i < node.numberOfTriangles; ++i)
        //printf("%d ", _triangles[node.firstTriangleIndex + i]);
      //putchar('\n');
    });
    putchar('\n');
  }
#endif // _DEBUG
}

BVH::~BVH()
{
  delete _root;
}

Bounds3f
BVH::bounds() const
{
  return _root == nullptr ? Bounds3f{} : _root->bounds;
}

void
BVH::iterate(BVHNodeFunction f) const
{
  Node::iterate(_root, f);
}


bool
BVH::intersectTriangles(const Ray& ray, Node* no, Intersection& hit)
{
	if (_mesh == nullptr)
		return false;

	auto t = (hit.object->sceneObject())->transform();
	auto origin = t->worldToLocalMatrix().transform(ray.origin);
	auto D = t->worldToLocalMatrix().transformVector(ray.direction);
	Ray localRay{ origin, D };
	auto d = math::inverse(D.length());
	float tMin;
	float tMax;

	float localMin = math::Limits<float>::inf();

	
	auto triangles = _mesh->data().triangles;
	auto numTriangles = _mesh->data().numberOfTriangles;
	auto vertexArray = _mesh->data().vertices;
	for (int i = no->first; i < no->first + no->count; i++)
	{
		auto p0 = vertexArray[triangles[_triangles[i]].v[0]];
		auto p1 = vertexArray[triangles[_triangles[i]].v[1]];
		auto p2 = vertexArray[triangles[_triangles[i]].v[2]];

		auto e1 = p1 - p0;// #1
		auto e2 = p2 - p0;// #2
		auto s1 = D.cross(e2);// #3
		auto s1e1 = s1.dot(e1);
		auto invd = 1 / s1e1;
		if (math::isZero(abs(invd)))// #4
		{
			continue; //5
		}
		auto s = origin - p0; // #6
		auto s2 = s.cross(e1);// #7
		auto t = s2.dot(e2) * invd;// #8
		if (t < 0)
		{
			continue;
		}

		auto b1 = s1.dot(s) * invd;// #9
		if (b1 < 0)
		{
			continue;
		}

		auto b2 = s2.dot(D) * invd;// #10
		if (b2 < 0)
		{
			continue;
		}

		if (b1 + b2 > 1)
		{
			continue;
		}
		auto td = t * d;
		if (td > localMin) continue;
		hit.triangleIndex = i;
		hit.distance = localMin = td;
		hit.p = vec3f{ 1 - b1 - b2, b1, b2 };
	}
	return localMin != math::Limits<float>::inf();
}

bool
BVH::intersect(const Ray& ray, Intersection& hit) const
{
	std::stack<Node*> pilha;

	float min, max;

	pilha.push(_root);

	while(!pilha.empty())
	{
		Node* no = pilha.top();
		pilha.pop();

		if (no->bounds.intersect(ray, min, max))
		{
			if (!no->isLeaf()) {
				pilha.push(no->children[0]);
				pilha.push(no->children[1]);
			}
			else
			{
				if (_mesh == nullptr)
					return false;

				auto t = (hit.object->sceneObject())->transform();
				auto origin = t->worldToLocalMatrix().transform(ray.origin);
				auto D = t->worldToLocalMatrix().transformVector(ray.direction);
				Ray localRay{ origin, D };
				auto d = math::inverse(D.length());
				float tMin;
				float tMax;

				float localMin = math::Limits<float>::inf();


				auto triangles = _mesh->data().triangles;
				auto numTriangles = _mesh->data().numberOfTriangles;
				auto vertexArray = _mesh->data().vertices;
				for (int i = no->first; i < no->first + no->count; i++)
				{
					auto p0 = vertexArray[triangles[_triangles[i]].v[0]];
					auto p1 = vertexArray[triangles[_triangles[i]].v[1]];
					auto p2 = vertexArray[triangles[_triangles[i]].v[2]];

					auto e1 = p1 - p0;// #1
					auto e2 = p2 - p0;// #2
					auto s1 = D.cross(e2);// #3
					auto s1e1 = s1.dot(e1);
					auto invd = 1 / s1e1;
					if (math::isZero(abs(invd)))// #4
					{
						continue; //5
					}
					auto s = origin - p0; // #6
					auto s2 = s.cross(e1);// #7
					auto t = s2.dot(e2) * invd;// #8
					if (t < 0)
					{
						continue;
					}

					auto b1 = s1.dot(s) * invd;// #9
					if (b1 < 0)
					{
						continue;
					}

					auto b2 = s2.dot(D) * invd;// #10
					if (b2 < 0)
					{
						continue;
					}

					if (b1 + b2 > 1)
					{
						continue;
					}
					auto td = t * d;
					if (td > localMin) continue;
					hit.triangleIndex = _triangles[i];
					hit.distance = localMin = td;
					hit.p = vec3f{ 1 - b1 - b2, b1, b2 };
				}
				return localMin != math::Limits<float>::inf();

			}
		}
	
	}

  return false;
}

} // end namespace cg
