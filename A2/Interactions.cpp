#include "Interactions.hpp"

// TODO: remove later
#include <iostream>
using namespace std;

void Interaction::left( float value ){
  cout<<"Interaction left "<<value<<endl;
}
void Interaction::centre( float value ){
  cout<<"Interaction centre "<<value<<endl;
}
void Interaction::right( float value ){
  cout<<"Interaction right "<<value<<endl;
}


void RotateViewInteraction::left( float value ){
  cout<<"RotateViewInteraction left "<<value<<endl;
}
void RotateViewInteraction::centre( float value ){
  cout<<"RotateViewInteraction centre "<<value<<endl;
}
void RotateViewInteraction::right( float value ){
  cout<<"RotateViewInteraction right "<<value<<endl;
}

void TranslateViewInteraction::left( float value ){
  cout<<"TranslateViewInteraction left "<<value<<endl;
}
void TranslateViewInteraction::centre( float value ){
  cout<<"TranslateViewInteraction centre "<<value<<endl;
}
void TranslateViewInteraction::right( float value ){
  cout<<"TranslateViewInteraction right "<<value<<endl;
}

void PerspectiveInteraction::left( float value ){
  cout<<"PerspectiveInteraction left "<<value<<endl;
}
void PerspectiveInteraction::centre( float value ){
  cout<<"PerspectiveInteraction centre "<<value<<endl;
}
void PerspectiveInteraction::right( float value ){
  cout<<"PerspectiveInteraction right "<<value<<endl;
}

void RotateModelInteraction::left( float value ){
  cout<<"RotateModelInteraction left "<<value<<endl;
}
void RotateModelInteraction::centre( float value ){
  cout<<"RotateModelInteraction centre "<<value<<endl;
}
void RotateModelInteraction::right( float value ){
  cout<<"RotateModelInteraction right "<<value<<endl;
}

void TranslateModelInteraction::left( float value ){
  cout<<"TranslateModelInteraction left "<<value<<endl;
}
void TranslateModelInteraction::centre( float value ){
  cout<<"TranslateModelInteraction centre "<<value<<endl;
}
void TranslateModelInteraction::right( float value ){
  cout<<"TranslateModelInteraction right "<<value<<endl;
}

void ScaleModelInteraction::left( float value ){
  cout<<"ScaleModelInteraction left "<<value<<endl;
}
void ScaleModelInteraction::centre( float value ){
  cout<<"ScaleModelInteraction centre "<<value<<endl;
}
void ScaleModelInteraction::right( float value ){
  cout<<"ScaleModelInteraction right "<<value<<endl;
}

void ViewportInteraction::left( float value ){
  cout<<"ViewportInteraction left "<<value<<endl;
}
void ViewportInteraction::centre( float value ){
  cout<<"ViewportInteraction centre "<<value<<endl;
}
void ViewportInteraction::right( float value ){
  cout<<"ViewportInteraction right "<<value<<endl;
}
