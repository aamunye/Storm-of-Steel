#include "Interactions.hpp"

// TODO: remove later
#include <iostream>
using namespace std;

mat4 Interaction::M;
mat4 Interaction::cumulativeModelTR;
glm::mat4 abc = mat4(1.0f);
float Interaction::scaleX;
float Interaction::scaleY;
float Interaction::scaleZ;

Interaction::Interaction( glm::vec4 modGnoArr[], glm::vec4 cubeArr[], glm::mat4 &cumulMod ) : cumulativeModel(cumulMod) {

  modelGnomonArray = modGnoArr;
  cubeArray = cubeArr;

  cumulativeModelTR = mat4(1.0f);
  scaleX = 0.0f;
  scaleY = 0.0f;
  scaleZ = 0.0f;

  vec3 lookAt = vec3(0.0f,0.0f,0.0f); // also the origin
  vec3 lookFrom = vec3(0.0f,0.0f,5.0f);

  vec3 up = vec3(0.0f,1.0f,0.0f);

  vec3 v_z = (lookFrom - lookAt)/abs(length(lookAt - lookFrom));
  vec3 v_x = cross(vec3(up),vec3(v_z))/abs(length(cross(vec3(up),vec3(v_z))));
  vec3 v_y = cross(v_z, v_x);

  mat4 R = mat4(
    vec4(v_x,0),
    vec4(v_y,0),
    vec4(v_z,0),
    vec4(0,0,0,1)
  );

  mat4 T = translate(mat4(1.0f),-lookFrom);

  cumulativeView = R * T;

  /*
  for(int i=0;i<3;i++){
    cout<<v_y[i]<<endl;
  }
  cout<<endl;
  */
  printMatrix(cumulativeView,"cumulativeView");

}


void Interaction::applyResultMatrix(mat4 result){
  for(int i=0;i<8;i++) {
    cubeArray[i] = result * cubeArray[i];
  }
  for(int i=0;i<4;i++) {
    modelGnomonArray[i] = result * modelGnomonArray[i];
  }
}
void Interaction::applyMatrix(mat4 mat, vec4 arr[], int length){
  for(int i=0;i<length;i++){
    arr[i] = mat * arr[i];
  }
}
void Interaction::printVecArray(glm::vec4 mat[],int length,string s) {
  cout<<"---------"<<s<<"---------"<<endl;
  for(int i=0;i<length;i++){
    for(int j=0;j<4;j++){
      cout<<mat[i][j]<<" ";
    }
    cout<<endl;
  }
  cout<<"------------------"<<endl<<endl;
}
void Interaction::printMatrix(glm::mat4 mat,string s) {
  cout<<"---------"<<s<<"---------"<<endl;
  /*
  for(int i=0;i<4;i++){
    for(int j=0;j<4;j++){
      cout<<mat[i][j]<<" ";
    }
    cout<<endl;
  }
  */
  cout<<mat<<endl;
  cout<<"------------------"<<endl<<endl;
}
/*
glm::mat4 Interaction::getCOBModelToWorld(){
  glm::mat4 result = glm::mat4();
  for(int i=0;i<3;i++){
    for(int j=0;j<3;j++){
      result[i][j] = dot(modelGnomonArray[j],worldFrame[i])/dot(worldFrame[i],worldFrame[i]);
    }
  }
  for(int i = 0;i<3;i++){
    result[i][3] = dot(modelGnomonArray[3]-worldFrame[3],worldFrame[i])/dot(worldFrame[i],worldFrame[i]);
  }
  return result;
}
glm::mat4 Interaction::getCOBWorldToModel(){
  glm::mat4 result = glm::mat4();
  for(int i=0;i<3;i++){
    for(int j=0;j<3;j++){
      result[i][j] = dot(worldFrame[j],modelGnomonArray[i])/dot(modelGnomonArray[i],modelGnomonArray[i]);
    }
  }
  for(int i = 0;i<3;i++){
    result[i][3] = dot(worldFrame[3]-modelGnomonArray[3],modelGnomonArray[i])/dot(modelGnomonArray[i],modelGnomonArray[i]);
  }
  return result;
}
*/
glm::mat4 Interaction::getCOBModelToWorld(){
  glm::mat4 result = glm::mat4();
  for(int i=0;i<3;i++){
    for(int j=0;j<3;j++){
      //result[i][j] = dot((modelGnomonArray[j]-modelGnomonArray[3]),worldFrame[i])/dot(worldFrame[i],worldFrame[i]);
      result[i][j] = dot((modelGnomonArray[j]-modelGnomonArray[3]),worldFrame[i]);
    }
  }
  for(int i = 0;i<3;i++){
    //result[i][3] = dot(modelGnomonArray[3]-worldFrame[3],worldFrame[i])/dot(worldFrame[i],worldFrame[i]);
    result[i][3] = dot(modelGnomonArray[3]-worldFrame[3],worldFrame[i]);
  }
  return result;
}
glm::mat4 Interaction::getCOBWorldToModel(){
  glm::mat4 result = glm::mat4();
  for(int i=0;i<3;i++){
    for(int j=0;j<3;j++){
      result[i][j] = dot(worldFrame[j],(modelGnomonArray[i]-modelGnomonArray[3]))/dot((modelGnomonArray[i]-modelGnomonArray[3]),(modelGnomonArray[i]-modelGnomonArray[3]));
    }
  }
  for(int i = 0;i<3;i++){
    result[i][3] =
    dot(
      worldFrame[3]-modelGnomonArray[3],
      (modelGnomonArray[i]-modelGnomonArray[3]))/dot((modelGnomonArray[i]-modelGnomonArray[3]),(modelGnomonArray[i]-modelGnomonArray[3]));
  }
  return result;
}

void Interaction::left( float value ){
  cout<<"Interaction left "<<value<<endl;
}
void Interaction::centre( float value ){
  cout<<"Interaction centre "<<value<<endl;
}
void Interaction::right( float value ){
  cout<<"Interaction right "<<value<<endl;
}


RotateViewInteraction::RotateViewInteraction( glm::vec4 modGnoArr[], glm::vec4 cubeArr[], glm::mat4 &cumulMod ):Interaction(modGnoArr,cubeArr,cumulMod){}
void RotateViewInteraction::left( float value ){
  cout<<"RotateViewInteraction left "<<value<<endl;
}
void RotateViewInteraction::centre( float value ){
  cout<<"RotateViewInteraction centre "<<value<<endl;
}
void RotateViewInteraction::right( float value ){
  cout<<"RotateViewInteraction right "<<value<<endl;
}

TranslateViewInteraction::TranslateViewInteraction( glm::vec4 modGnoArr[], glm::vec4 cubeArr[], glm::mat4 &cumulMod ):Interaction(modGnoArr,cubeArr,cumulMod){}
void TranslateViewInteraction::left( float value ){
  cout<<"TranslateViewInteraction left "<<value<<endl;
}
void TranslateViewInteraction::centre( float value ){
  cout<<"TranslateViewInteraction centre "<<value<<endl;
}
void TranslateViewInteraction::right( float value ){
  cout<<"TranslateViewInteraction right "<<value<<endl;
}

PerspectiveInteraction::PerspectiveInteraction( glm::vec4 modGnoArr[], glm::vec4 cubeArr[], glm::mat4 &cumulMod ):Interaction(modGnoArr,cubeArr,cumulMod){}
void PerspectiveInteraction::left( float value ){
  cout<<"PerspectiveInteraction left "<<value<<endl;
}
void PerspectiveInteraction::centre( float value ){
  cout<<"PerspectiveInteraction centre "<<value<<endl;
}
void PerspectiveInteraction::right( float value ){
  cout<<"PerspectiveInteraction right "<<value<<endl;
}

RotateModelInteraction::RotateModelInteraction( glm::vec4 modGnoArr[], glm::vec4 cubeArr[], glm::mat4 &cumulMod ):Interaction(modGnoArr,cubeArr,cumulMod){}
void RotateModelInteraction::left( float value ){
  mat4 scaleMatrix = scale(mat4(1.0f),vec3(pow(10.0f,scaleX),pow(10.0f,scaleY),pow(10.0f,scaleZ)));
  mat4 rotateMatrix = glm::rotate(mat4(1.0f), value*0.001f,vec3(1.0f,0.0f,0.0f));
  cumulativeModelTR = cumulativeModelTR * rotateMatrix;
  cumulativeModel = cumulativeModelTR * scaleMatrix;
}
void RotateModelInteraction::centre( float value ){
  mat4 scaleMatrix = scale(mat4(1.0f),vec3(pow(10.0f,scaleX),pow(10.0f,scaleY),pow(10.0f,scaleZ)));
  mat4 rotateMatrix = glm::rotate(mat4(1.0f), value*0.001f,vec3(0.0f,1.0f,0.0f));
  cumulativeModelTR = cumulativeModelTR * rotateMatrix;
  cumulativeModel = cumulativeModelTR * scaleMatrix;
}
void RotateModelInteraction::right( float value ){
  mat4 scaleMatrix = scale(mat4(1.0f),vec3(pow(10.0f,scaleX),pow(10.0f,scaleY),pow(10.0f,scaleZ)));
  mat4 rotateMatrix = glm::rotate(mat4(1.0f), value*0.001f,vec3(0.0f,0.0f,1.0f));
  cumulativeModelTR = cumulativeModelTR * rotateMatrix;
  cumulativeModel = cumulativeModelTR * scaleMatrix;
}

TranslateModelInteraction::TranslateModelInteraction( glm::vec4 modGnoArr[], glm::vec4 cubeArr[], glm::mat4 &cumulMod ):Interaction(modGnoArr,cubeArr,cumulMod){}
void TranslateModelInteraction::left( float value ){
  mat4 scaleMatrix = scale(mat4(1.0f),vec3(pow(10.0f,scaleX),pow(10.0f,scaleY),pow(10.0f,scaleZ)));
  mat4 translateMatrix = translate(mat4(1.0f),vec3(value*0.001,0.0f,0.0f));
  cumulativeModelTR = cumulativeModelTR * translateMatrix;
  cumulativeModel = cumulativeModelTR * scaleMatrix;
}
void TranslateModelInteraction::centre( float value ){
  mat4 scaleMatrix = scale(mat4(1.0f),vec3(pow(10.0f,scaleX),pow(10.0f,scaleY),pow(10.0f,scaleZ)));
  mat4 translateMatrix = translate(mat4(1.0f),vec3(0.0f,value*0.001,0.0f));
  cumulativeModelTR = cumulativeModelTR * translateMatrix;
  cumulativeModel = cumulativeModelTR * scaleMatrix;
}
void TranslateModelInteraction::right( float value ){
  mat4 scaleMatrix = scale(mat4(1.0f),vec3(pow(10.0f,scaleX),pow(10.0f,scaleY),pow(10.0f,scaleZ)));
  mat4 translateMatrix = translate(mat4(1.0f),vec3(0.0f,0.0f,value*0.001));
  cumulativeModelTR = cumulativeModelTR * translateMatrix;
  cumulativeModel = cumulativeModelTR * scaleMatrix;
}



ScaleModelInteraction::ScaleModelInteraction( glm::vec4 modGnoArr[], glm::vec4 cubeArr[], glm::mat4 &cumulMod ):Interaction(modGnoArr,cubeArr,cumulMod){}
void ScaleModelInteraction::left( float value ){
  scaleX += value * 0.001;
  mat4 scaleMatrix = scale(mat4(1.0f),vec3(pow(10.0f,scaleX),pow(10.0f,scaleY),pow(10.0f,scaleZ)));
  cumulativeModel = cumulativeModelTR * scaleMatrix;
}
void ScaleModelInteraction::centre( float value ){
  scaleY += value * 0.001;
  mat4 scaleMatrix = scale(mat4(1.0f),vec3(pow(10.0f,scaleX),pow(10.0f,scaleY),pow(10.0f,scaleZ)));
  cumulativeModel = cumulativeModelTR * scaleMatrix;
}
void ScaleModelInteraction::right( float value ){
  scaleZ += value * 0.001;
  mat4 scaleMatrix = scale(mat4(1.0f),vec3(pow(10.0f,scaleX),pow(10.0f,scaleY),pow(10.0f,scaleZ)));
  cumulativeModel = cumulativeModelTR * scaleMatrix;
}

ViewportInteraction::ViewportInteraction( glm::vec4 modGnoArr[], glm::vec4 cubeArr[], glm::mat4 &cumulMod ):Interaction(modGnoArr,cubeArr,cumulMod){}
void ViewportInteraction::left( float value ){
  cout<<"ViewportInteraction left "<<value<<endl;
}
void ViewportInteraction::centre( float value ){
  cout<<"ViewportInteraction centre "<<value<<endl;
}
void ViewportInteraction::right( float value ){
  cout<<"ViewportInteraction right "<<value<<endl;
}
