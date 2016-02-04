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
	modelGnomonArray[ 1 ] = vec4( 1.0f , 0.0f, 0.0f, 1.0f );
	modelGnomonArray[ 2 ] = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	modelGnomonArray[ 3 ] = vec4( 0.0f, 1.0f, 0.0f , 1.0f );
	modelGnomonArray[ 4 ] = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	modelGnomonArray[ 5 ] = vec4( 0.0f, 0.0f, 1.0f , 1.0f );

	cumulativeModel = mat4(1.0);

	interactionModes[0] = new RotateViewInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[1] = new TranslateViewInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[2] = new PerspectiveInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[3] = new RotateModelInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[4] = new TranslateModelInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[5] = new ScaleModelInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[6] = new ViewportInteraction(modelGnomonArray, cubeArray, cumulativeModel);

	currentMode = ROTATE_VIEW;
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
		return true;
	}
	if(vec1.z > far && vec2.z > far ){
		// disappear
		vec1 = vec4(0.0f,0.0f,0.0f,1.0f);
		vec2 = vec4(0.0f,0.0f,0.0f,1.0f);
		return true;
	}
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
	setLineColour(vec3(1.0f, 1.0f, 1.0f));
	mat4 cumulView = currentInteraction->cumulativeView;
	mat4 cumulProj = currentInteraction->cumulativeProj;

	vec4 transformedCube[24];
	for(int i=0;i<24;i++){
		transformedCube[i] = cumulProj * cumulView * cumulativeModel * cubeArray[i];
	}
	for(int i=0;i<12;i++){
		nearAndFarClipping(transformedCube[2*i],transformedCube[2*i+1]);
	}
	for(int i=0;i<24;i++){
		for(int j=0;j<4;j++){
			if(transformedCube[i][3]==0)cout<<"panic"<<endl;
			transformedCube[i][j]/=transformedCube[i][3];
		}
	}
	for(int i=0;i<12;i++){
		transformToViewport(transformedCube[2*i]);
		transformToViewport(transformedCube[2*i+1]);
		drawLine(transformedCube[2*i],transformedCube[2*i+1]);
	}


	vec4 transformedModelGnomon[6];
	mat4 cumulModelTR = currentInteraction->cumulativeModelTR;
	for(int i=0;i<6;i++){
		transformedModelGnomon[i] = cumulProj * cumulView * cumulModelTR * modelGnomonArray[i];
		for(int j=0;j<4;j++){
			transformedModelGnomon[i][j]/=transformedModelGnomon[i][3];
		}
		transformToViewport(transformedModelGnomon[i]);
	}

	vec4 transformedWorldGnomon[6];
	for(int i=0;i<6;i++){
		transformedWorldGnomon[i] = cumulProj * cumulView * modelGnomonArray[i];
		for(int j=0;j<4;j++){
			transformedWorldGnomon[i][j]/=transformedWorldGnomon[i][3];

		}
		//TODO cleanup
		transformToViewport(transformedWorldGnomon[i]);
		/*
		float x = transformedWorldGnomon[i].x;
		float y = transformedWorldGnomon[i].y;
		float xin = (x + 1)/2*(viewportEnd.x - viewportStart.x) + viewportStart.x;
		float yin = (y + 1)/2*(viewportStart.y - viewportEnd.y) + viewportEnd.y;
		transformedWorldGnomon[i].x = xin;
		transformedWorldGnomon[i].y = yin;
		*/

	}



	setLineColour(vec3(1.0f, 0.0f, 0.0f));
	drawLine(
		transformedModelGnomon[0],
		0.25f*(transformedModelGnomon[1]-transformedModelGnomon[0])+transformedModelGnomon[0]);
	setLineColour(vec3(0.0f, 1.0f, 0.0f));
	drawLine(
		transformedModelGnomon[2],
		0.25f*(transformedModelGnomon[3]-transformedModelGnomon[2])+transformedModelGnomon[2]);
	setLineColour(vec3(0.0f, 0.0f, 1.0f));
	drawLine(
		transformedModelGnomon[4],
		0.25f*(transformedModelGnomon[5]-transformedModelGnomon[4])+transformedModelGnomon[4]);


	setLineColour(vec3(1.0f, 1.0f, 0.0f));
	drawLine(
		transformedWorldGnomon[0],
		0.25f*(transformedWorldGnomon[1]-transformedWorldGnomon[0])+transformedWorldGnomon[0]);
	setLineColour(vec3(0.0f, 1.0f, 1.0f));
	drawLine(
		transformedWorldGnomon[2],
		0.25f*(transformedWorldGnomon[3]-transformedWorldGnomon[2])+transformedWorldGnomon[2]);
	setLineColour(vec3(1.0f, 0.0f, 1.0f));
	drawLine(
		transformedWorldGnomon[4],
		0.25f*(transformedWorldGnomon[5]-transformedWorldGnomon[4])+transformedWorldGnomon[4]);

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


		if( ImGui::RadioButton( "Rotate View", &currentMode, ROTATE_VIEW ) ) {
		}
		if( ImGui::RadioButton( "Translate View", &currentMode, TRANSLATE_VIEW ) ) {
		}
		if( ImGui::RadioButton( "Perspective", &currentMode, PERSPECTIVE ) ) {
		}
		if( ImGui::RadioButton( "Rotate Model", &currentMode, ROTATE_MODEL ) ) {
		}
		if( ImGui::RadioButton( "Translate Model", &currentMode, TRANSLATE_MODEL ) ) {
		}
		if( ImGui::RadioButton( "Scale Model", &currentMode, SCALE_MODEL ) ) {
		}
		if( ImGui::RadioButton( "Viewport", &currentMode, VIEWPORT ) ) {
		}

		currentInteraction = interactionModes[ currentMode ];



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
	modelGnomonArray[ 1 ] = vec4( 1.0f , 0.0f, 0.0f, 1.0f );
	modelGnomonArray[ 2 ] = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	modelGnomonArray[ 3 ] = vec4( 0.0f, 1.0f, 0.0f , 1.0f );
	modelGnomonArray[ 4 ] = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	modelGnomonArray[ 5 ] = vec4( 0.0f, 0.0f, 1.0f , 1.0f );

	cumulativeModel = mat4(1.0);

	interactionModes[0] = new RotateViewInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[1] = new TranslateViewInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[2] = new PerspectiveInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[3] = new RotateModelInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[4] = new TranslateModelInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[5] = new ScaleModelInteraction(modelGnomonArray, cubeArray, cumulativeModel);
	interactionModes[6] = new ViewportInteraction(modelGnomonArray, cubeArray, cumulativeModel);

	currentMode = ROTATE_VIEW;
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
