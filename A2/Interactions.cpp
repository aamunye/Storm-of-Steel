#include "Interactions.hpp"

// TODO: remove later
#include <iostream>
using namespace std;

Interaction::Interaction( glm::vec4 modGnoArr[], glm::vec4 cubeArr[] ) {
  modelGnomonArray = modGnoArr;
  cubeArray = cubeArr;
}
float Interaction::scaleX = 0.0f;
float Interaction::scaleY = 0.0f;
float Interaction::scaleZ = 0.0f;

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
void Interaction::left( float value ){
  cout<<"Interaction left "<<value<<endl;
}
void Interaction::centre( float value ){
  cout<<"Interaction centre "<<value<<endl;
}
void Interaction::right( float value ){
  cout<<"Interaction right "<<value<<endl;
}


RotateViewInteraction::RotateViewInteraction( glm::vec4 modGnoArr[], glm::vec4 cubeArr[] ):Interaction(modGnoArr,cubeArr){}
void RotateViewInteraction::left( float value ){
  cout<<"RotateViewInteraction left "<<value<<endl;
}
void RotateViewInteraction::centre( float value ){
  cout<<"RotateViewInteraction centre "<<value<<endl;
}
void RotateViewInteraction::right( float value ){
  cout<<"RotateViewInteraction right "<<value<<endl;
}

TranslateViewInteraction::TranslateViewInteraction( glm::vec4 modGnoArr[], glm::vec4 cubeArr[] ):Interaction(modGnoArr,cubeArr){}
void TranslateViewInteraction::left( float value ){
  cout<<"TranslateViewInteraction left "<<value<<endl;
}
void TranslateViewInteraction::centre( float value ){
  cout<<"TranslateViewInteraction centre "<<value<<endl;
}
void TranslateViewInteraction::right( float value ){
  cout<<"TranslateViewInteraction right "<<value<<endl;
}

PerspectiveInteraction::PerspectiveInteraction( glm::vec4 modGnoArr[], glm::vec4 cubeArr[] ):Interaction(modGnoArr,cubeArr){}
void PerspectiveInteraction::left( float value ){
  cout<<"PerspectiveInteraction left "<<value<<endl;
}
void PerspectiveInteraction::centre( float value ){
  cout<<"PerspectiveInteraction centre "<<value<<endl;
}
void PerspectiveInteraction::right( float value ){
  cout<<"PerspectiveInteraction right "<<value<<endl;
}

RotateModelInteraction::RotateModelInteraction( glm::vec4 modGnoArr[], glm::vec4 cubeArr[] ):Interaction(modGnoArr,cubeArr){}
void RotateModelInteraction::left( float value ){
  cout<<"RotateModelInteraction left "<<value<<endl;

  //printMatrix(scaleMatrix,"scaleMatrix");
  //printMatrix(cobWorldToModel,"cobWorldToModel");
  //printMatrix(cobModelToWorld,"cobModelToWorld");
  //printMatrix(cobModelToWorld*cobModelToWorld,"Together");

}
void RotateModelInteraction::centre( float value ){
  cout<<"RotateModelInteraction centre "<<value<<endl;
}
void RotateModelInteraction::right( float value ){
  cout<<"RotateModelInteraction right "<<value<<endl;
}

TranslateModelInteraction::TranslateModelInteraction( glm::vec4 modGnoArr[], glm::vec4 cubeArr[] ):Interaction(modGnoArr,cubeArr){}
void TranslateModelInteraction::left( float value ){
  cout<<"TranslateModelInteraction left "<<value<<endl;
}
void TranslateModelInteraction::centre( float value ){
  cout<<"TranslateModelInteraction centre "<<value<<endl;
}
void TranslateModelInteraction::right( float value ){
  cout<<"TranslateModelInteraction right "<<value<<endl;
}



ScaleModelInteraction::ScaleModelInteraction( glm::vec4 modGnoArr[], glm::vec4 cubeArr[] ):Interaction(modGnoArr,cubeArr){}
void ScaleModelInteraction::left( float value ){
  mat4 cobModelToWorld = getCOBModelToWorld();

  // Update scale
  scaleX = scaleX + value*0.001;

  mat4 scaleMatrix = scale(mat4(1.0f),vec3(pow(10.0f,scaleX),pow(10.0f,scaleY),pow(10.0f,scaleZ)));

  mat4 result = cobModelToWorld * scaleMatrix;

  for(int i=0;i<8;i++){
    cubeArray[i] = originalCubeArray[i];
  }

  applyMatrix(result,cubeArray,8);
}
void ScaleModelInteraction::centre( float value ){
  mat4 cobModelToWorld = getCOBModelToWorld();

  // Update scale
  scaleY = scaleY + value*0.001;

  mat4 scaleMatrix = scale(mat4(1.0f),vec3(pow(10.0f,scaleX),pow(10.0f,scaleY),pow(10.0f,scaleZ)));

  mat4 result = cobModelToWorld * scaleMatrix;

  for(int i=0;i<8;i++){
    cubeArray[i] = originalCubeArray[i];
  }

  applyMatrix(result,cubeArray,8);
}
void ScaleModelInteraction::right( float value ){
  mat4 cobModelToWorld = getCOBModelToWorld();

  // Update scale
  scaleZ = scaleZ + value*0.001;

  mat4 scaleMatrix = scale(mat4(1.0f),vec3(pow(10.0f,scaleX),pow(10.0f,scaleY),pow(10.0f,scaleZ)));

  mat4 result = cobModelToWorld * scaleMatrix;

  for(int i=0;i<8;i++){
    cubeArray[i] = originalCubeArray[i];
  }

  applyMatrix(result,cubeArray,8);
}

ViewportInteraction::ViewportInteraction( glm::vec4 modGnoArr[], glm::vec4 cubeArr[] ):Interaction(modGnoArr,cubeArr){}
void ViewportInteraction::left( float value ){
  cout<<"ViewportInteraction left "<<value<<endl;
}
void ViewportInteraction::centre( float value ){
  cout<<"ViewportInteraction centre "<<value<<endl;
}
void ViewportInteraction::right( float value ){
  cout<<"ViewportInteraction right "<<value<<endl;
}
