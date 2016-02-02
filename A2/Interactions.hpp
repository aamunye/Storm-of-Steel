#pragma once

// TODO: remove later
#include <iostream>
#include <string>
using namespace std;


#include <glm/glm.hpp>
using namespace glm;

#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include <glm/gtx/transform.hpp>

#include <vector>
#include <math.h>

enum Mode { ROTATE_VIEW, TRANSLATE_VIEW, PERSPECTIVE, ROTATE_MODEL,
  TRANSLATE_MODEL, SCALE_MODEL, VIEWPORT };


class Interaction {
public:
	Interaction(glm::vec4 modelGnomonArray[], glm::vec4 cubeArray[]);
	~Interaction(){}
	virtual void left( float value );
  virtual void centre( float value );
  virtual void right( float value );
  void printMatrix(glm::mat4,string);
  void printVecArray(glm::vec4 mat[],int length,string s);
protected:
  glm::vec4 worldFrame[4]={
    vec4(1.0f,0.0f,0.0f,0.0f),
    vec4(0.0f,1.0f,0.0f,0.0f),
    vec4(0.0f,0.0f,1.0f,0.0f),
    vec4(0.0f,0.0f,0.0f,1.0f)
  };
  glm::vec4 originalCubeArray[8]={
    vec4( -1.0f, 1.0f, 1.0f, 0.0f ),	// Front, top, left
  	vec4( 1.0f, 1.0f, 1.0f, 0.0f ),	// Front, top, right
  	vec4( 1.0f, 1.0f, -1.0f, 0.0f ),	// Back, top, right
  	vec4( -1.0f, 1.0f, -1.0f, 0.0f ),	// Back, top, left
  	vec4( -1.0f, -1.0f, 1.0f, 0.0f ),	// Front, bottom, left
  	vec4( 1.0f, -1.0f, 1.0f, 0.0f ),	// Front, bottom, right
  	vec4( 1.0f, -1.0f, -1.0f, 0.0f ),	// Back, bottom, right
  	vec4( -1.0f, -1.0f, -1.0f, 0.0f )	// Back, bottom, left
  };
  glm::vec4 *modelGnomonArray;
  glm::vec4 *cubeArray;
  glm::mat4 getCOBWorldToModel();
  glm::mat4 getCOBModelToWorld();

  void applyResultMatrix(mat4 result);
  void applyMatrix(mat4 mat, vec4 arr[], int length);

  static float scaleX;
  static float scaleY;
  static float scaleZ;

};

class RotateViewInteraction : public Interaction {

public:
  RotateViewInteraction(glm::vec4 modelGnomonArray[], glm::vec4 cubeArray[]);
	void left( float value );
  void centre( float value );
  void right( float value );
};


class TranslateViewInteraction : public Interaction {
public:
  TranslateViewInteraction(glm::vec4 modelGnomonArray[], glm::vec4 cubeArray[]);
	void left( float value );
  void centre( float value );
  void right( float value );
};

class PerspectiveInteraction : public Interaction {
public:
  PerspectiveInteraction(glm::vec4 modelGnomonArray[], glm::vec4 cubeArray[]);
	void left( float value );
  void centre( float value );
  void right( float value );
};

class RotateModelInteraction : public Interaction {
public:
  RotateModelInteraction(glm::vec4 modelGnomonArray[], glm::vec4 cubeArray[]);
	void left( float value );
  void centre( float value );
  void right( float value );
private:
  float currentScaleX = 1.0f;
  float currentScaleY = 1.0f;
  float currentScaleZ = 1.0f;
};

class TranslateModelInteraction : public Interaction {
public:
  TranslateModelInteraction(glm::vec4 modelGnomonArray[], glm::vec4 cubeArray[]);
	void left( float value );
  void centre( float value );
  void right( float value );
};

class ScaleModelInteraction : public Interaction {
public:
  ScaleModelInteraction(glm::vec4 modelGnomonArray[], glm::vec4 cubeArray[]);
	void left( float value );
  void centre( float value );
  void right( float value );
};

class ViewportInteraction : public Interaction {
public:
  ViewportInteraction(glm::vec4 modelGnomonArray[], glm::vec4 cubeArray[]);
	void left( float value );
  void centre( float value );
  void right( float value );
};
