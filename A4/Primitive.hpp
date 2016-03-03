#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "polyroots.hpp"

#include <iostream>

using namespace std;
using namespace glm;

class Primitive {
public:
  virtual ~Primitive();
  virtual bool hit(glm::vec3 origin, glm::vec3 direction, float& shortestT);
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
};

class Cube : public Primitive {
public:
  virtual ~Cube();
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();

  bool hit(glm::vec3 origin, glm::vec3 pworld, float& shortestT);

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }

  virtual ~NonhierBox();

private:
  glm::vec3 m_pos;
  double m_size;
};
