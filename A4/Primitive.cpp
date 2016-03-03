#include "Primitive.hpp"

Primitive::~Primitive()
{
}

bool Primitive::hit(glm::vec3 origin, glm::vec3 direction, float& shortestT)
{
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

bool NonhierSphere::hit(glm::vec3 origin, glm::vec3 pworld, float& shortestT)
{
  vec3 a = origin;
  vec3 b = pworld;
  vec3 c = m_pos;
  //cout<<"NonhierSphere::hit"<<endl;
  double A = dot(b-a, b-a);
  double B = 2 * dot(b-a, a-c);
  double C = dot(a-c, a-c)-m_radius*m_radius;

  double roots[2];

  int nRoots = quadraticRoots(A,B,C,roots);
  //int nRoots = quadraticRoots(1.0f,2.0f,1.0f,roots);

  vec3 N;

  if( nRoots > 0 ) {
    if(roots[0]>=1 && roots[0]<shortestT){
      shortestT = roots[0];

      N = c-(a+shortestT*(b-a));
    }

    if(roots[1]>=1 && roots[1]<shortestT){
      shortestT = roots[1];

      N = c-(a+shortestT*(b-a));
    }
  }

  //cout<<nRoots<<endl;
  //cout<<roots[0]<<" "<<roots[1]<<endl;

  return true;
}

NonhierBox::~NonhierBox()
{
}
