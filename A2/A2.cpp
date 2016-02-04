#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.reserve(kMaxVertices);
	colours.reserve(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f))
{

}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);


	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();

	// Setup Cube Coordinates
	cubeArray[ 0  ] = vec4( -1.0f, 1.0f, 1.0f, 1.0f );
	cubeArray[ 1  ] = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	cubeArray[ 2  ] = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	cubeArray[ 3  ] = vec4( 1.0f, 1.0f, -1.0f, 1.0f );
	cubeArray[ 4  ] = vec4( 1.0f, 1.0f, -1.0f, 1.0f );
	cubeArray[ 5  ] = vec4( -1.0f, 1.0f, -1.0f, 1.0f );
	cubeArray[ 6  ] = vec4( -1.0f, 1.0f, -1.0f, 1.0f );
	cubeArray[ 7  ] = vec4( -1.0f, 1.0f, 1.0f, 1.0f );
	cubeArray[ 8  ] = vec4( -1.0f, -1.0f, 1.0f, 1.0f );
	cubeArray[ 9  ] = vec4( 1.0f, -1.0f, 1.0f, 1.0f );
	cubeArray[ 10 ] = vec4( 1.0f, -1.0f, 1.0f, 1.0f );
	cubeArray[ 11 ] = vec4( 1.0f, -1.0f, -1.0f, 1.0f );
	cubeArray[ 12 ] = vec4( 1.0f, -1.0f, -1.0f, 1.0f );
	cubeArray[ 13 ] = vec4( -1.0f, -1.0f, -1.0f, 1.0f );
	cubeArray[ 14 ] = vec4( -1.0f, -1.0f, -1.0f, 1.0f );
	cubeArray[ 15 ] = vec4( -1.0f, -1.0f, 1.0f, 1.0f );
	cubeArray[ 16 ] = vec4( -1.0f, 1.0f, 1.0f, 1.0f );
	cubeArray[ 17 ] = vec4( -1.0f, -1.0f, 1.0f, 1.0f );
	cubeArray[ 18 ] = vec4( -1.0f, 1.0f, -1.0f, 1.0f );
	cubeArray[ 19 ] = vec4( -1.0f, -1.0f, -1.0f, 1.0f );
	cubeArray[ 20 ] = vec4( 1.0f, 1.0f, -1.0f, 1.0f );
	cubeArray[ 21 ] = vec4( 1.0f, -1.0f, -1.0f, 1.0f );
	cubeArray[ 22 ] = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	cubeArray[ 23 ] = vec4( 1.0f, -1.0f, 1.0f, 1.0f );

	modelGnomonArray[ 0 ] = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	modelGnomonArray[ 1 ] = vec4( 0.25f , 0.0f, 0.0f, 1.0f );
	modelGnomonArray[ 2 ] = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	modelGnomonArray[ 3 ] = vec4( 0.0f, 0.25f, 0.0f , 1.0f );
	modelGnomonArray[ 4 ] = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	modelGnomonArray[ 5 ] = vec4( 0.0f, 0.0f, 0.25f , 1.0f );

	cumulativeModel = mat4(1.0);

	interactionModes[0] = new RotateModelInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[1] = new TranslateModelInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[2] = new ScaleModelInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[3] = new RotateViewInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[4] = new TranslateViewInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[5] = new PerspectiveInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[6] = new ViewportInteraction(modelGnomonArray, cubeArray, cumulativeModel);

	currentMode = ROTATE_MODEL;
	currentInteraction = interactionModes[currentMode];

	leftButtonPressed = false;
	centreButtonPressed = false;
	rightButtonPressed = false;

	previousMouseXPos = 0;

	viewportStart = vec2(-0.9f, 0.9f);
	viewportEnd = vec2( 0.9f, -0.9f);
	//updateViewportArray();
}

//----------------------------------------------------------------------------------------
void updateViewportArray() {

}

bool A2::nearAndFarClipping(vec4 &vec1,vec4 &vec2){
	float near = currentInteraction->pNear;
	float far = currentInteraction->pFar;
	if(vec1.z < near && vec2.z < near ){
		// disappear
		vec1 = vec4(0.0f,0.0f,0.0f,1.0f);
		vec2 = vec4(0.0f,0.0f,0.0f,1.0f);
		return false;
	}
	if(vec1.z > far && vec2.z > far ){
		// disappear
		vec1 = vec4(0.0f,0.0f,0.0f,1.0f);
		vec2 = vec4(0.0f,0.0f,0.0f,1.0f);
		return false;
	}
	if(vec1.z <= far && vec1.z >= near &&  vec2.z <= far && vec2.z >= near){
		return true;
	}

	vec4 &vecBiggerZ = (vec1.z > vec2.z) ? vec1 : vec2;
	vec4 &vecSmallerZ = (vec1.z > vec2.z) ? vec2 : vec1;

	float tBigger = 0.0f;
	float tSmaller = 0.0f;

	bool biggerSet = false;
	bool smallerSet = false;

	vec4 vecSmallerZcopy;
	vec4 vecBiggerZcopy;

	cout<<endl;
	if(vecBiggerZ.z > far){
		tBigger = (far - vecSmallerZ.z)/(vecBiggerZ.z - vecSmallerZ.z);
		vecBiggerZcopy =  vecBiggerZ + tBigger * (vecSmallerZ-vecBiggerZ);
		biggerSet = true;
	}
	if(vecSmallerZ.z < near){
		tSmaller = (near - vecBiggerZ.z)/(vecSmallerZ.z - vecBiggerZ.z);
		vecSmallerZcopy =  vecSmallerZ + tSmaller * (vecBiggerZ - vecSmallerZ);
		smallerSet = true;
	}

	cout<<"before"<<endl;

	cout<<vecBiggerZ<<endl;
	cout<<vecSmallerZ<<endl;

	if(biggerSet)vecBiggerZ = vecBiggerZcopy;
	if(smallerSet)vecSmallerZ = vecSmallerZcopy;

	cout<<"after"<<endl;

	cout<<vecBiggerZ<<endl;
	cout<<vecSmallerZ<<endl;

	return false;
}

void A2::transformToViewport(vec4 &vec){
	float x = vec.x;
	float y = vec.y;

	float smallX = (viewportStart.x < viewportEnd.x) ? viewportStart.x : viewportEnd.x;
	float bigX = (viewportStart.x > viewportEnd.x) ? viewportStart.x : viewportEnd.x;

	float smallY = (viewportStart.y < viewportEnd.y) ? viewportStart.y : viewportEnd.y;
	float bigY = (viewportStart.y > viewportEnd.y) ? viewportStart.y : viewportEnd.y;

	float xin = (x + 1)/2*abs(bigX - smallX) + smallX;
	float yin = (y + 1)/2*abs(smallY - bigY) + smallY;
	vec.x = xin;
	vec.y = yin;
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//----------------------------------------------------------------------------------------
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec2 & v0,   // Line Start (NDC coordinate)
		const glm::vec2 & v1    // Line End (NDC coordinate)
) {

	m_vertexData.positions[m_vertexData.index] = v0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = v1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

//---------------------------------------------------------------------------------------
// Overloaded with vec3
// Only uses the first 2 coordinates
//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec3 & v0,   // Line Start (NDC coordinate)
		const glm::vec3 & v1    // Line End (NDC coordinate)
) {
	drawLine(vec2(v0[0],v0[1]),vec2(v1[0],v1[1]));
}

//---------------------------------------------------------------------------------------
// Overloaded with vec4
// Only uses the first 2 coordinates
//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec4 & v0,   // Line Start (NDC coordinate)
		const glm::vec4 & v1    // Line End (NDC coordinate)
) {
	drawLine(vec2(v0[0],v0[1]),vec2(v1[0],v1[1]));
}

bool A2::clip(vec4 &A,vec4&B,vec4 P,vec4 n){
	float wecA = dot((A-P),n);
	float wecB = dot((B-P),n);
	if( wecA < 0 && wecB < 0 ){
		return false;
	}
	if (wecA >= 0 && wecB >= 0){
		return true;
	}
	float t = wecA/(wecA-wecB);
	if(wecA<0){
		A = A + t *(B-A);
	}else{
		B = A + t *(B-A);
	}
	return true;
}



//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Place per frame, application logic here ...

	// Call at the beginning of frame, before drawing lines:
	initLineData();

	// Draw cube:
	bool bDrawCube[12];
	for(int i=0;i<12;i++)bDrawCube[i]=true;
	setLineColour(vec3(1.0f, 1.0f, 1.0f));
	mat4 cumulView = currentInteraction->cumulativeView;
	mat4 cumulProj = currentInteraction->cumulativeProj;
	vec4 transformedCube[24];
	for(int i=0;i<24;i++){
		transformedCube[i] = cumulView * cumulativeModel * cubeArray[i];
	}
	float far = currentInteraction->pFar;
	float near = currentInteraction->pNear;
	for(int i=0;i<12;i++){
		//nearAndFarClipping(transformedCube[2*i],transformedCube[2*i+1]);
		/*

		vec4 P = vec4(0.0f,0.0f,near,1.0f);
		vec4 n = vec4(0,0,1,0);
		float wecA = dot((transformedCube[2*i]-P),n);
		float wecB = dot((transformedCube[2*i+1]-P),n);
		if( wecA < 0 && wecB < 0 ){
			bDrawCube[i]=false;
		}
		if (wecA >= 0 && wecB >= 0){
			continue;
		}
		float t = wecA/(wecA-wecB);
		if(wecA<0){
			transformedCube[2*i] = transformedCube[2*i] + t *(transformedCube[2*i+1]-transformedCube[2*i]);
		}else{
			transformedCube[2*i+1] = transformedCube[2*i] + t *(transformedCube[2*i+1]-transformedCube[2*i]);
		}
		*/

		// far clipping
		bDrawCube[i] &= clip(transformedCube[2*i],transformedCube[2*i+1],vec4(0.0f,0.0f,near,1.0f),vec4(0,0,1,0));
		bDrawCube[i] &= clip(transformedCube[2*i],transformedCube[2*i+1],vec4(0.0f,0.0f,far,1.0f),vec4(0,0,-1,0));

		transformedCube[2*i] = cumulProj * transformedCube[2*i];
		transformedCube[2*i+1] = cumulProj * transformedCube[2*i+1];
	}
	for(int i=0;i<24;i++){

		for(int j=0;j<4;j++){
			if(transformedCube[i][3]==0)cout<<"panic"<<endl;
			transformedCube[i][j]/=transformedCube[i][3];
		}
	}
	for(int i=0;i<12;i++){
		//right
		bDrawCube[i] &= clip(transformedCube[2*i],transformedCube[2*i+1],vec4(1.0f,1.0f,1.0f,1.0f),vec4(-1,0,0,0));
		//left
		bDrawCube[i] &= clip(transformedCube[2*i],transformedCube[2*i+1],vec4(-1.0f,1.0f,1.0f,1.0f),vec4(1,0,0,0));
		//top
		bDrawCube[i] &= clip(transformedCube[2*i],transformedCube[2*i+1],vec4(0.0f,1.0f,0.0f,1.0f),vec4(0,-1,0,0));
		//bottom
		bDrawCube[i] &= clip(transformedCube[2*i],transformedCube[2*i+1],vec4(0.0f,-1.0f,0.0f,1.0f),vec4(0,1,0,0));

		transformToViewport(transformedCube[2*i]);
		transformToViewport(transformedCube[2*i+1]);

		if(bDrawCube[i]){
			drawLine(transformedCube[2*i],transformedCube[2*i+1]);
		}
	}

	bool bDrawModelGnomon[3];
	vec4 transformedModelGnomon[6];
	mat4 cumulModelTR = currentInteraction->cumulativeModelTR;
	for(int i=0;i<3;i++){
		bDrawModelGnomon[i]=true;

		transformedModelGnomon[2*i] =  cumulView * cumulModelTR * modelGnomonArray[2*i];
		transformedModelGnomon[2*i+1] = cumulView * cumulModelTR * modelGnomonArray[2*i+1];

		//near and far
		bDrawModelGnomon[i] &= clip(transformedModelGnomon[2*i],transformedModelGnomon[2*i+1],vec4(0.0f,0.0f,near,1.0f),vec4(0,0,1,0));
		bDrawModelGnomon[i] &= clip(transformedModelGnomon[2*i],transformedModelGnomon[2*i+1],vec4(0.0f,0.0f,far,1.0f),vec4(0,0,-1,0));

		transformedModelGnomon[2*i] = cumulProj * transformedModelGnomon[2*i];
		transformedModelGnomon[2*i+1] = cumulProj * transformedModelGnomon[2*i+1];

		for(int j=0;j<4;j++){
			transformedModelGnomon[2*i][j]/=transformedModelGnomon[2*i][3];
			transformedModelGnomon[2*i+1][j]/=transformedModelGnomon[2*i+1][3];
		}

		//right
		bDrawModelGnomon[i] &= clip(transformedModelGnomon[2*i],transformedModelGnomon[2*i+1],vec4(1.0f,1.0f,1.0f,1.0f),vec4(-1,0,0,0));
		//left
		bDrawModelGnomon[i] &= clip(transformedModelGnomon[2*i],transformedModelGnomon[2*i+1],vec4(-1.0f,1.0f,1.0f,1.0f),vec4(1,0,0,0));
		//top
		bDrawModelGnomon[i] &= clip(transformedModelGnomon[2*i],transformedModelGnomon[2*i+1],vec4(0.0f,1.0f,0.0f,1.0f),vec4(0,-1,0,0));
		//bottom
		bDrawModelGnomon[i] &= clip(transformedModelGnomon[2*i],transformedModelGnomon[2*i+1],vec4(0.0f,-1.0f,0.0f,1.0f),vec4(0,1,0,0));

		transformToViewport(transformedModelGnomon[2*i]);
		transformToViewport(transformedModelGnomon[2*i+1]);
	}
	setLineColour(vec3(1.0f, 0.0f, 0.0f));
	if(bDrawModelGnomon[0]){
		drawLine(transformedModelGnomon[0],transformedModelGnomon[1]);
	}
	setLineColour(vec3(0.0f, 1.0f, 0.0f));
	if(bDrawModelGnomon[1]){
		drawLine(transformedModelGnomon[2],transformedModelGnomon[3]);
	}
	setLineColour(vec3(0.0f, 0.0f, 1.0f));
	if(bDrawModelGnomon[2]){
		drawLine(transformedModelGnomon[4],transformedModelGnomon[5]);
	}

	bool bDrawWorldGnomon[3];
	vec4 transformedWorldGnomon[6];
	for(int i=0;i<3;i++)bDrawWorldGnomon[i]=true;
	/*

	for(int i=0;i<3;i++){
		bDrawWorldGnomon[i]=true;

		transformedWorldGnomon[2*i] = cumulView * modelGnomonArray[2*i];
		transformedWorldGnomon[2*i+1] = cumulView * modelGnomonArray[2*i];

		bDrawWorldGnomon[i] &= clip(transformedWorldGnomon[2*i],transformedWorldGnomon[2*i+1],vec4(0.0f,0.0f,near,1.0f),vec4(0,0,1,0));
		bDrawWorldGnomon[i] &= clip(transformedWorldGnomon[2*i],transformedWorldGnomon[2*i+1],vec4(0.0f,0.0f,far,1.0f),vec4(0,0,-1,0));

		transformedWorldGnomon[2*i] = cumulProj * modelGnomonArray[2*i];
		transformedWorldGnomon[2*i+1] = cumulProj * modelGnomonArray[2*i];
		for(int j=0;j<4;j++){
			transformedWorldGnomon[2*i][j]/=transformedWorldGnomon[2*i][3];
			transformedWorldGnomon[2*i+1][j]/=transformedWorldGnomon[2*i+1][3];

		}
		//right
		bDrawWorldGnomon[i] &= clip(transformedWorldGnomon[2*i],transformedWorldGnomon[2*i+1],vec4(1.0f,1.0f,1.0f,1.0f),vec4(-1,0,0,0));
		//left
		bDrawWorldGnomon[i] &= clip(transformedWorldGnomon[2*i],transformedWorldGnomon[2*i+1],vec4(-1.0f,1.0f,1.0f,1.0f),vec4(1,0,0,0));
		//top
		bDrawWorldGnomon[i] &= clip(transformedWorldGnomon[2*i],transformedWorldGnomon[2*i+1],vec4(0.0f,1.0f,0.0f,1.0f),vec4(0,-1,0,0));
		//bottom
		bDrawWorldGnomon[i] &= clip(transformedWorldGnomon[2*i],transformedWorldGnomon[2*i+1],vec4(0.0f,-1.0f,0.0f,1.0f),vec4(0,1,0,0));

		//TODO cleanup
		transformToViewport(transformedWorldGnomon[2*i]);
		transformToViewport(transformedWorldGnomon[2*i+1]);

	}

	cout<<"bDrawWorldGnomon[0]"<<bDrawWorldGnomon[0]<<endl;

	setLineColour(vec3(1.0f, 1.0f, 0.0f));
	if(bDrawWorldGnomon[0]){
		drawLine(transformedWorldGnomon[0],transformedWorldGnomon[1]);
	}
	setLineColour(vec3(0.0f, 1.0f, 1.0f));
	if(bDrawWorldGnomon[1]){
		drawLine(transformedWorldGnomon[2],transformedWorldGnomon[3]);
	}
	setLineColour(vec3(1.0f, 0.0f, 1.0f));
	if(bDrawWorldGnomon[2]){
		drawLine(transformedWorldGnomon[4],transformedWorldGnomon[5]);
	}*/


	for(int i=0;i<3;i++){
		bDrawWorldGnomon[i]=true;

		transformedWorldGnomon[2*i] = cumulView * modelGnomonArray[2*i];
		transformedWorldGnomon[2*i+1] = cumulView * modelGnomonArray[2*i+1];

		bDrawWorldGnomon[i] &= clip(transformedWorldGnomon[2*i],transformedWorldGnomon[2*i+1],vec4(0.0f,0.0f,near,1.0f),vec4(0,0,1,0));
		bDrawWorldGnomon[i] &= clip(transformedWorldGnomon[2*i],transformedWorldGnomon[2*i+1],vec4(0.0f,0.0f,far,1.0f),vec4(0,0,-1,0));

		transformedWorldGnomon[2*i] = cumulProj * transformedWorldGnomon[2*i];
		transformedWorldGnomon[2*i+1] = cumulProj * transformedWorldGnomon[2*i+1];
		for(int j=0;j<4;j++){
			transformedWorldGnomon[2*i][j]/=transformedWorldGnomon[2*i][3];
			transformedWorldGnomon[2*i+1][j]/=transformedWorldGnomon[2*i+1][3];

		}
		//right
		bDrawWorldGnomon[i] &= clip(transformedWorldGnomon[2*i],transformedWorldGnomon[2*i+1],vec4(1.0f,1.0f,1.0f,1.0f),vec4(-1,0,0,0));
		//left
		bDrawWorldGnomon[i] &= clip(transformedWorldGnomon[2*i],transformedWorldGnomon[2*i+1],vec4(-1.0f,1.0f,1.0f,1.0f),vec4(1,0,0,0));
		//top
		bDrawWorldGnomon[i] &= clip(transformedWorldGnomon[2*i],transformedWorldGnomon[2*i+1],vec4(0.0f,1.0f,0.0f,1.0f),vec4(0,-1,0,0));
		//bottom
		bDrawWorldGnomon[i] &= clip(transformedWorldGnomon[2*i],transformedWorldGnomon[2*i+1],vec4(0.0f,-1.0f,0.0f,1.0f),vec4(0,1,0,0));
		//TODO cleanup
		transformToViewport(transformedWorldGnomon[2*i]);
		transformToViewport(transformedWorldGnomon[2*i+1]);
	}

	setLineColour(vec3(1.0f, 1.0f, 0.0f));
	if(bDrawWorldGnomon[0]){
		drawLine(transformedWorldGnomon[0],transformedWorldGnomon[1]);
	}
	setLineColour(vec3(0.0f, 1.0f, 1.0f));
	if(bDrawWorldGnomon[1]){
		drawLine(transformedWorldGnomon[2],transformedWorldGnomon[3]);
	}
	setLineColour(vec3(1.0f, 0.0f, 1.0f));
	if(bDrawWorldGnomon[2]){
		drawLine(transformedWorldGnomon[4],transformedWorldGnomon[5]);
	}


	setLineColour(vec3(0.0f, 0.0f, 0.0f));
	drawLine(vec2(viewportStart.x,viewportStart.y),vec2(viewportEnd.x,viewportStart.y));
	drawLine(vec2(viewportEnd.x,viewportStart.y),vec2(viewportEnd.x,viewportEnd.y));
	drawLine(vec2(viewportEnd.x,viewportEnd.y),vec2(viewportStart.x,viewportEnd.y));
	drawLine(vec2(viewportStart.x,viewportEnd.y),vec2(viewportStart.x,viewportStart.y));


}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if( ImGui::Button( "Reset Values" ) ) {
			resetValues();
		}


		if( ImGui::RadioButton( "Rotate Model", &currentMode, ROTATE_MODEL ) ) {
		}
		if( ImGui::RadioButton( "Translate Model", &currentMode, TRANSLATE_MODEL ) ) {
		}
		if( ImGui::RadioButton( "Scale Model", &currentMode, SCALE_MODEL ) ) {
		}
		if( ImGui::RadioButton( "Rotate View", &currentMode, ROTATE_VIEW ) ) {
		}
		if( ImGui::RadioButton( "Translate View", &currentMode, TRANSLATE_VIEW ) ) {
		}
		if( ImGui::RadioButton( "Perspective", &currentMode, PERSPECTIVE ) ) {
		}
		if( ImGui::RadioButton( "Viewport", &currentMode, VIEWPORT ) ) {
		}

		currentInteraction = interactionModes[ currentMode ];

		ImGui::Text( "FOV: %.1f° ", currentInteraction->pFOV * 180 / M_PI );
		ImGui::Text( "Near: %.1f ", currentInteraction->pNear );
		ImGui::Text( "Far: %.1f ", currentInteraction->pFar );
		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Resets working variables to their original state
 */
void A2::resetValues()
{
	// Setup Cube Coordinates
	cubeArray[ 0  ] = vec4( -1.0f, 1.0f, 1.0f, 1.0f );
	cubeArray[ 1  ] = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	cubeArray[ 2  ] = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	cubeArray[ 3  ] = vec4( 1.0f, 1.0f, -1.0f, 1.0f );
	cubeArray[ 4  ] = vec4( 1.0f, 1.0f, -1.0f, 1.0f );
	cubeArray[ 5  ] = vec4( -1.0f, 1.0f, -1.0f, 1.0f );
	cubeArray[ 6  ] = vec4( -1.0f, 1.0f, -1.0f, 1.0f );
	cubeArray[ 7  ] = vec4( -1.0f, 1.0f, 1.0f, 1.0f );
	cubeArray[ 8  ] = vec4( -1.0f, -1.0f, 1.0f, 1.0f );
	cubeArray[ 9  ] = vec4( 1.0f, -1.0f, 1.0f, 1.0f );
	cubeArray[ 10 ] = vec4( 1.0f, -1.0f, 1.0f, 1.0f );
	cubeArray[ 11 ] = vec4( 1.0f, -1.0f, -1.0f, 1.0f );
	cubeArray[ 12 ] = vec4( 1.0f, -1.0f, -1.0f, 1.0f );
	cubeArray[ 13 ] = vec4( -1.0f, -1.0f, -1.0f, 1.0f );
	cubeArray[ 14 ] = vec4( -1.0f, -1.0f, -1.0f, 1.0f );
	cubeArray[ 15 ] = vec4( -1.0f, -1.0f, 1.0f, 1.0f );
	cubeArray[ 16 ] = vec4( -1.0f, 1.0f, 1.0f, 1.0f );
	cubeArray[ 17 ] = vec4( -1.0f, -1.0f, 1.0f, 1.0f );
	cubeArray[ 18 ] = vec4( -1.0f, 1.0f, -1.0f, 1.0f );
	cubeArray[ 19 ] = vec4( -1.0f, -1.0f, -1.0f, 1.0f );
	cubeArray[ 20 ] = vec4( 1.0f, 1.0f, -1.0f, 1.0f );
	cubeArray[ 21 ] = vec4( 1.0f, -1.0f, -1.0f, 1.0f );
	cubeArray[ 22 ] = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	cubeArray[ 23 ] = vec4( 1.0f, -1.0f, 1.0f, 1.0f );





	modelGnomonArray[ 0 ] = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	modelGnomonArray[ 1 ] = vec4( 0.25f , 0.0f, 0.0f, 1.0f );
	modelGnomonArray[ 2 ] = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	modelGnomonArray[ 3 ] = vec4( 0.0f, 0.25f, 0.0f , 1.0f );
	modelGnomonArray[ 4 ] = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	modelGnomonArray[ 5 ] = vec4( 0.0f, 0.0f, 0.25f , 1.0f );

	cumulativeModel = mat4(1.0);

	interactionModes[0] = new RotateModelInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[1] = new TranslateModelInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[2] = new ScaleModelInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[3] = new RotateViewInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[4] = new TranslateViewInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[5] = new PerspectiveInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[6] = new ViewportInteraction(modelGnomonArray, cubeArray, cumulativeModel);

	currentMode = ROTATE_MODEL;
	currentInteraction = interactionModes[currentMode];

	leftButtonPressed = false;
	centreButtonPressed = false;
	rightButtonPressed = false;

	viewportStart = vec2(-0.9f, 0.9f);
	viewportEnd = vec2( 0.9f, -0.9f);
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);
	double xDiff = xPos - previousMouseXPos;

	if( leftButtonPressed ){
		if(currentMode == VIEWPORT){
			float x = previousMouseXPos/m_windowWidth*2-1;
			float y = previousMouseYPos/m_windowHeight*-2+1;
			if(x<-1)x=-1;
			if(x>1)x=1;
			if(y<-1)y=-1;
			if(y>1)y=1;
			viewportEnd = vec2(x,y);
		}
		currentInteraction->left(xDiff);
		eventHandled = true;
	}
	if( centreButtonPressed ){
		currentInteraction->centre(xDiff);
		eventHandled = true;
	}
	if( rightButtonPressed ){
		currentInteraction->right(xDiff);
		eventHandled = true;
	}


	previousMouseXPos = xPos;
	previousMouseYPos = yPos;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	if (actions == GLFW_PRESS) {
		if (!ImGui::IsMouseHoveringAnyWindow()) {
			if( button==GLFW_MOUSE_BUTTON_LEFT )
			{
				leftButtonPressed = true;
				if(currentMode == VIEWPORT){
					float x = previousMouseXPos/m_windowWidth*2-1;
					float y = previousMouseYPos/m_windowHeight*-2+1;
					viewportStart = vec2(x,y);
					viewportEnd = vec2(x,y);
				}
				eventHandled = true;
			}
			else if( button==GLFW_MOUSE_BUTTON_MIDDLE )
			{
				centreButtonPressed = true;
				eventHandled = true;
			}
			else if( button==GLFW_MOUSE_BUTTON_RIGHT )
			{
				rightButtonPressed = true;
				eventHandled = true;
			}
		}

	}
	if (actions == GLFW_RELEASE) {
		if( button==GLFW_MOUSE_BUTTON_LEFT )
		{
			leftButtonPressed = false;
			eventHandled = true;
		}
		else if( button==GLFW_MOUSE_BUTTON_MIDDLE )
		{
			centreButtonPressed = false;
			eventHandled = true;
		}
		else if( button==GLFW_MOUSE_BUTTON_RIGHT )
		{
			rightButtonPressed = false;
			eventHandled = true;
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	m_windowWidth = width;
	m_windowHeight = height;
	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	if( action == GLFW_PRESS ) {
		// Closing
		if ( key == GLFW_KEY_Q ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}

		// Resetting values
		if ( key == GLFW_KEY_A ) {
			resetValues();
			eventHandled = true;
		}

		// Changed interaction mode
		bool changedMode = false;

		// Keyboard shortcuts corresponding to interaction modes
		if ( key == GLFW_KEY_O ) {
			changedMode = true;
			currentMode = ROTATE_VIEW;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_N ) {
			changedMode = true;
			currentMode = TRANSLATE_VIEW;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_P ) {
			changedMode = true;
			currentMode = PERSPECTIVE;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_R ) {
			changedMode = true;
			currentMode = ROTATE_MODEL;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_T ) {
			changedMode = true;
			currentMode = TRANSLATE_MODEL;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_S ) {
			changedMode = true;
			currentMode = SCALE_MODEL;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_V ) {
			changedMode = true;
			currentMode = VIEWPORT;
			eventHandled = true;
		}

		if(changedMode){
			currentInteraction = interactionModes[currentMode];
		}
	}

	return eventHandled;
}
