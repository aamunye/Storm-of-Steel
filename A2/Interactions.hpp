#pragma once

// TODO: remove later
#include <iostream>
using namespace std;

enum Mode { ROTATE_VIEW, TRANSLATE_VIEW, PERSPECTIVE, ROTATE_MODEL,
  TRANSLATE_MODEL, SCALE_MODEL, VIEWPORT };

class Interaction {
public:
	Interaction(){}
	~Interaction(){}
	virtual void left( float value );
  virtual void centre( float value );
  virtual void right( float value );
};

class RotateViewInteraction : public Interaction {
public:
	void left( float value );
  void centre( float value );
  void right( float value );
};

class TranslateViewInteraction : public Interaction {
public:
	void left( float value );
  void centre( float value );
  void right( float value );
};

class PerspectiveInteraction : public Interaction {
public:
	void left( float value );
  void centre( float value );
  void right( float value );
};

class RotateModelInteraction : public Interaction {
public:
	void left( float value );
  void centre( float value );
  void right( float value );
};

class TranslateModelInteraction : public Interaction {
public:
	void left( float value );
  void centre( float value );
  void right( float value );
};

class ScaleModelInteraction : public Interaction {
public:
	void left( float value );
  void centre( float value );
  void right( float value );
};

class ViewportInteraction : public Interaction {
public:
	void left( float value );
  void centre( float value );
  void right( float value );
};
