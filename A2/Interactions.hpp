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

#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include "Interactions.hpp"


#include <vector>


#include <vector>
#include <math.h>

enum Mode { ROTATE_VIEW, TRANSLATE_VIEW, PERSPECTIVE, ROTATE_MODEL,
  TRANSLATE_MODEL, SCALE_MODEL, VIEWPORT };


class Interaction {
public:
	Interaction(glm::vec4 modelGnomonArray[], glm::vec4 cubeArray[], glm::mat4 &cumulMod);
	~Interaction(){}
	virtual void left( float value );
  virtual void centre( float value );
  virtual void right( float value );
  void printMatrix(glm::mat4,string);
  void printVecArray(glm::vec4 mat[],int length,string s);

  static glm::mat4 cumulativeModelTR;
  glm::mat4 &cumulativeModel;
  static glm::mat4 cumulativeView;

  static glm::mat4 cumulativeProj;

  // In degrees
  const float P_FOV_MIN = 5 * M_PI / 180;
  const float P_FOV_MAX = 160 * M_PI / 180;

  static glm::vec2 viewPortStart;
  static glm::vec2 viewPortEnd;

  static glm::vec2 viewPortArray[4];

  static float pNear;
  static float pFar;
  static float pFOV;

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

  static glm::mat4 M;
  static glm::mat4 originalViewMatrix;
  static glm::mat4 rotateViewMat;
  static glm::mat4 translateViewMat;

  void updateCumulativeView();

  void updateCumulativeProj();






};

class RotateViewInteraction : public Interaction {

public:
  RotateViewInteraction(glm::vec4 modelGnomonArray[], glm::vec4 cubeArray[], glm::mat4 &cumulMod);
	void left( float value );
  void centre( float value );
  void right( float value );
};


class TranslateViewInteraction : public Interaction {
public:
  TranslateViewInteraction(glm::vec4 modelGnomonArray[], glm::vec4 cubeArray[], glm::mat4 &cumulMod);
	void left( float value );
  void centre( float value );
  void right( float value );
};

class PerspectiveInteraction : public Interaction {
public:
  PerspectiveInteraction(glm::vec4 modelGnomonArray[], glm::vec4 cubeArray[], glm::mat4 &cumulMod);
	void left( float value );
  void centre( float value );
  void right( float value );
};

class RotateModelInteraction : public Interaction {
public:
  RotateModelInteraction(glm::vec4 modelGnomonArray[], glm::vec4 cubeArray[], glm::mat4 &cumulMod);
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
  TranslateModelInteraction(glm::vec4 modelGnomonArray[], glm::vec4 cubeArray[], glm::mat4 &cumulMod);
	void left( float value );
  void centre( float value );
  void right( float value );
};

class ScaleModelInteraction : public Interaction {
public:
  ScaleModelInteraction(glm::vec4 modelGnomonArray[], glm::vec4 cubeArray[], glm::mat4 &cumulMod);
	void left( float value );
  void centre( float value );
  void right( float value );
};

class ViewportInteraction : public Interaction {
public:
  ViewportInteraction(glm::vec4 modelGnomonArray[], glm::vec4 cubeArray[], glm::mat4 &cumulMod);
	void left( float value );
  void centre( float value );
  void right( float value );
};
