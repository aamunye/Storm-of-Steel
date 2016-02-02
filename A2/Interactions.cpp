#include "Interactions.hpp"

// TODO: remove later
#include <iostream>
using namespace std;

Interaction::Interaction( glm::vec4 modGnoArr[], glm::vec4 cubeArr[] ) : modelGnomonArray(modGnoArr),cubeArray(cubeArr){
  worldFrame[0]=vec4(1.0f,0.0f,0.0f,0.0f);
  worldFrame[1]=vec4(0.0f,1.0f,0.0f,0.0f);
  worldFrame[2]=vec4(0.0f,0.0f,1.0f,0.0f);
  worldFrame[3]=vec4(0.0f,0.0f,0.0f,1.0f);
  printMatrix(getCOBWorldToModel(),"The Thing");
}
void Interaction::printMatrix(glm::mat4 mat,string s) {
  cout<<"---------"<<s<<"---------"<<endl;
  for(int i=0;i<4;i++){
    for(int j=0;j<4;j++){
      cout<<mat[i][j]<<" ";
    }
    cout<<endl;
  }
  cout<<"------------------"<<endl<<endl;
}
glm::mat4 Interaction::getCOBWorldToModel(){
  glm::mat4 result = glm::mat4();
  for(int i=0;i<3;i++){
    for(int j=0;j<3;j++){
      //result[i][j] = (dot(worldFrame[j],modelGnomonArray[i])/dot(modelGnomonArray[i],modelGnomonArray[i]));
      result[i][j] = (dot(worldFrame[j],modelGnomonArray[i]));
    }
  }
  for(int i = 0;i<3;i++){
    result[i][3] = (dot(worldFrame[3]-modelGnomonArray[3],modelGnomonArray[i]));
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
  cout<<"ScaleModelInteraction left "<<value<<endl;
}
void ScaleModelInteraction::centre( float value ){
  cout<<"ScaleModelInteraction centre "<<value<<endl;
}
void ScaleModelInteraction::right( float value ){
  cout<<"ScaleModelInteraction right "<<value<<endl;
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
