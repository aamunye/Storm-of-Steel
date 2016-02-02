#pragma once

// TODO: remove later
#include <iostream>
#include <string>
using namespace std;


#include <glm/glm.hpp>
using namespace glm;

#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <vector>

enum Mode { ROTATE_VIEW, TRANSLATE_VIEW, PERSPECTIVE, ROTATE_MODEL,
  TRANSLATE_MODEL, SCALE_MODEL, VIEWPORT };


class Interaction {
public:
	Interaction(glm::vec4 modelGnomonArray[], glm::vec4 cubeArray[]);
	~Interaction(){}
	virtual void left( float value );
  virtual void centre( float value );
  virtual void right( float value );
protected:
  glm::vec4 worldFrame[4];
  glm::vec4 *modelGnomonArray;
  glm::vec4 *cubeArray;
  glm::mat4 getCOBWorldToModel();
  void printMatrix(glm::mat4,string);
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
