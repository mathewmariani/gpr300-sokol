#pragma once

// batteries
#include "mesh.h"
#include "transform.h"

namespace batteries
{
  struct Shape
  {
    Transform transform;
    Mesh mesh;
  };

  Shape CreateCube(float size);
  Shape CreatePlane(float width, float height, int subdivisions);
  Shape CreateSphere(float radius, int subdivisions);
}