#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <ctime>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;


//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 )
{
	rotateMat = mat4(1.0f);
	// Set the zoom level to 0
	zoom = glm::mat4(1.0f);
	zoomCount = 0;

	// Set the flyMat matrix to the identity matrix
	flyMat = mat4(1.0f);

	mouseButtonActive = false;
	previousMouseXPos = 0;
	towersVertices = new float[ TOTAL_COORDINATES ];

	// Makes the bottom left cell the active one
	currentX = 0;
	currentZ = 15;

	for(int i=0;i<TOTAL_COORDINATES;i++){
		towersVertices[i] = 0.0f;
	}
	memset(towerHeight, 0, sizeof(towerHeight));
	memset(cellColour, 0, sizeof(cellColour));

	// Select eight random colours for the colour palette
	for(int i=0; i<8; i++){
		for(int j=0; j<3; j++){
			colours[i][j]=randomGenerator();
		}
	}

}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{
	delete [] towersVertices;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );

	initGrid();

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt(
		glm::vec3( 0.0f, float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );
	proj = glm::perspective(
		glm::radians( 45.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A1::initGrid()
{
	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );

	// -------- Code for setting up grid: START ----------------
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );


	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;
	// -------- Code for setting up grid: END ----------------

	// -------- Code for setting up towers: START ----------------
	glGenVertexArrays( 1, &m_towers_vao );
	glBindVertexArray( m_towers_vao );

	glGenBuffers( 1, &m_towers_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_towers_vbo );
	glBufferData( GL_ARRAY_BUFFER, TOTAL_COORDINATES*sizeof(float),
		towersVertices, GL_STATIC_DRAW );

	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	updateTowersVertices(0, 15);
	// -------- Code for setting up towers: END ----------------

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...


}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		if( ImGui::Button( "Reset Values" ) ) {
			resetValues();
		}

		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.

		// Colour Palette
		ImGui::PushID( 0 );
		ImGui::ColorEdit3( "##Colour", colours[0] );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 0 ) ) {
			cellColour[currentX][currentZ] = current_col;
		}
		ImGui::PopID();

		ImGui::PushID( 1 );
		ImGui::ColorEdit3( "##Colour", colours[1] );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 1 ) ) {
			cellColour[currentX][currentZ] = current_col;
		}
		ImGui::PopID();

		ImGui::PushID( 2 );
		ImGui::ColorEdit3( "##Colour", colours[2] );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 2 ) ) {
			cellColour[currentX][currentZ] = current_col;
		}
		ImGui::PopID();

		ImGui::PushID( 3 );
		ImGui::ColorEdit3( "##Colour", colours[3] );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 3 ) ) {
			cellColour[currentX][currentZ] = current_col;
		}
		ImGui::PopID();

		ImGui::PushID( 4 );
		ImGui::ColorEdit3( "##Colour", colours[4] );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 4 ) ) {
			cellColour[currentX][currentZ] = current_col;
		}
		ImGui::PopID();

		ImGui::PushID( 5 );
		ImGui::ColorEdit3( "##Colour", colours[5] );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 5 ) ) {
			cellColour[currentX][currentZ] = current_col;
		}
		ImGui::PopID();

		ImGui::PushID( 6 );
		ImGui::ColorEdit3( "##Colour", colours[6] );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 6 ) ) {
			cellColour[currentX][currentZ] = current_col;
		}
		ImGui::PopID();

		ImGui::PushID( 7 );
		ImGui::ColorEdit3( "##Colour", colours[7] );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 7 ) ) {
			cellColour[currentX][currentZ] = current_col;
		}
		ImGui::PopID();

/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}
*/

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// Clear the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Create a global transformation for the model (centre it).
	mat4 W;
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );

	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( zoom * view * flyMat * rotateMat ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr(  W ) );


		//Draw the grid
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// Draw the cubes
		glBindVertexArray( m_towers_vao );

		for ( int x = 0; x < DIM; x++ ) {
			for ( int z = 0; z < DIM; z++ ) {
				bool activeCell = (x == currentX && z == currentZ);
				// Only draw tower if its height is greater than 0
				if ( towerHeight[x][z] > 0 || activeCell ) {
					// store colour of current x and z for easy access later
					float cols[3];

					for(int i=0; i<3; i++){
						cols[i] = colours[cellColour[x][z]][i];
					}
					if ( activeCell ) {
						// We draw the active cell's top face last to make it stand out

						// Draw the trianges
						glUniform3f( col_uni, cols[0], cols[1], cols[2] );
						glDrawArrays( GL_TRIANGLES, (x*DIM+z)*POINTS_PER_TOWER+6, 32-6);

						// Reuse the vertices used to create the triangles to draw lines
						// to make the edges easy to see.
						// Draw the active cell's lines in black
						glUniform3f( col_uni, 0, 0, 0 );
						glDrawArrays( GL_LINES, (x*DIM+z)*POINTS_PER_TOWER, 32);
					} else {
						// Draw the triangles
						glUniform3f( col_uni, cols[0], cols[1], cols[2] );
						glDrawArrays( GL_TRIANGLES, (x*DIM+z)*POINTS_PER_TOWER, 32);

						// Reuse the vertices used to create the triangles to draw lines
						// to make the edges easy to see.
						// Draw this non-active cell's lines in white
						glUniform3f( col_uni, 1, 1, 1 );
						glDrawArrays( GL_LINES, (x*DIM+z)*POINTS_PER_TOWER, 32);
					}
				}

				// The first six vertices correspond to the top face.  Disable depth
				// testing before drawing it to avoid z-buffer clashes.
				// Since this is the active cell, set a random colour every frame.
				//glDisable(GL_DEPTH_TEST);
				glUniform3f( col_uni, randomGenerator(), randomGenerator(), randomGenerator() );
				glDisable(GL_DEPTH_TEST);
				glDrawArrays( GL_TRIANGLES, (currentX*DIM+currentZ)*POINTS_PER_TOWER, 6);
				glEnable(GL_DEPTH_TEST);
			}
		}
		// Highlight the active square.
	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Resets the manipulated variables to their original and clear grid.
 */
void A1::resetValues()
{
		flyMat = mat4(1.0f);

		rotateMat = mat4(1.0f);

		// reset the zoom values
		zoom = glm::mat4(1.0f);
		zoomCount = 0;

		// Makes the bottom left cell the active one
		currentX = 0;
		currentZ = 15;

		for(int i=0;i<TOTAL_COORDINATES;i++){
			towersVertices[i] = 0.0f;
		}
		memset(towerHeight, 0, sizeof(towerHeight));
		memset(cellColour, 0, sizeof(cellColour));

		// Select eight new random colours for the colour palette
		for(int i=0; i<8; i++){
			for(int j=0; j<3; j++){
				colours[i][j]=randomGenerator();
			}
		}

		updateTowersVertices(0,15);
}

//----------------------------------------------------------------------------------------
/*
 * Resets the manipulated variables to their original  the grid.
 */
void A1::setCurrentColour()
{
	if( towerHeight[currentX][currentZ] > 0 )
	{
		// Set the current colour to the tower's saved colour if
		// the tower's height is bigger than 0
		current_col = cellColour[currentX][currentZ];
	} else {
		// If the tower's height is 0, set it's colour to the currently
		// selected colour.
		cellColour[currentX][currentZ] = current_col;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Sets the vertices of the given coordinates based on its tower's height.
 */
void A1::updateTowersVertices(int xCord, int zCord)
{
	int offset = (DIM * xCord + zCord) * COORDINATES_PER_TOWER;
	int height = towerHeight[xCord][zCord];

	size_t ct = 0;

	// Top
	towersVertices[ offset + ct 		] = xCord;
	towersVertices[ offset + ct + 1 ] = height;
	towersVertices[ offset + ct + 2 ] = zCord;

	towersVertices[ offset + ct +	3	] = xCord+1;
	towersVertices[ offset + ct + 4 ] = height;
	towersVertices[ offset + ct + 5 ] = zCord;

	towersVertices[ offset + ct + 6	] = xCord+1;
	towersVertices[ offset + ct + 7 ] = height;
	towersVertices[ offset + ct + 8 ] = zCord+1;

	towersVertices[ offset + ct + 9	] = xCord;
	towersVertices[ offset + ct + 10 ] = height;
	towersVertices[ offset + ct + 11 ] = zCord;

	towersVertices[ offset + ct + 12 ] = xCord;
	towersVertices[ offset + ct + 13 ] = height;
	towersVertices[ offset + ct + 14 ] = zCord+1;

	towersVertices[ offset + ct + 15 ] = xCord+1;
	towersVertices[ offset + ct + 16 ] = height;
	towersVertices[ offset + ct + 17 ] = zCord+1;

	ct += 18;

	// Bottom
	towersVertices[ offset + ct 		] = xCord;
	towersVertices[ offset + ct + 1 ] = 0;
	towersVertices[ offset + ct + 2 ] = zCord;

	towersVertices[ offset + ct +	3	] = xCord+1;
	towersVertices[ offset + ct + 4 ] = 0;
	towersVertices[ offset + ct + 5 ] = zCord;

	towersVertices[ offset + ct + 6 ] = xCord+1;
	towersVertices[ offset + ct + 7 ] = 0;
	towersVertices[ offset + ct + 8 ] = zCord+1;

	towersVertices[ offset + ct + 9 ] = xCord;
	towersVertices[ offset + ct + 10 ] = 0;
	towersVertices[ offset + ct + 11 ] = zCord;

	towersVertices[ offset + ct + 12 ] = xCord;
	towersVertices[ offset + ct + 13 ] = 0;
	towersVertices[ offset + ct + 14 ] = zCord+1;

	towersVertices[ offset + ct + 15 ] = xCord+1;
	towersVertices[ offset + ct + 16 ] = 0;
	towersVertices[ offset + ct + 17 ] = zCord+1;

	ct += 18;

	// Left
	towersVertices[ offset + ct 		] = xCord;
	towersVertices[ offset + ct + 1 ] = 0;
	towersVertices[ offset + ct + 2 ] = zCord;

	towersVertices[ offset + ct +	3	] = xCord;
	towersVertices[ offset + ct + 4 ] = height;
	towersVertices[ offset + ct + 5 ] = zCord;

	towersVertices[ offset + ct + 6 ] = xCord;
	towersVertices[ offset + ct + 7 ] = height;
	towersVertices[ offset + ct + 8 ] = zCord+1;

	towersVertices[ offset + ct + 9 ] = xCord;
	towersVertices[ offset + ct + 10 ] = 0;
	towersVertices[ offset + ct + 11 ] = zCord;

	towersVertices[ offset + ct + 12 ] = xCord;
	towersVertices[ offset + ct + 13 ] = 0;
	towersVertices[ offset + ct + 14 ] = zCord+1;

	towersVertices[ offset + ct + 15 ] = xCord;
	towersVertices[ offset + ct + 16 ] = height;
	towersVertices[ offset + ct + 17 ] = zCord+1;

	ct += 18;

	// Right
	towersVertices[ offset + ct 		] = xCord+1;
	towersVertices[ offset + ct + 1 ] = 0;
	towersVertices[ offset + ct + 2 ] = zCord+1;

	towersVertices[ offset + ct +	3	] = xCord+1;
	towersVertices[ offset + ct + 4 ] = height;
	towersVertices[ offset + ct + 5 ] = zCord+1;

	towersVertices[ offset + ct + 6 ] = xCord+1;
	towersVertices[ offset + ct + 7 ] = height;
	towersVertices[ offset + ct + 8 ] = zCord;

	towersVertices[ offset + ct + 9 ] = xCord+1;
	towersVertices[ offset + ct + 10 ] = 0;
	towersVertices[ offset + ct + 11 ] = zCord+1;

	towersVertices[ offset + ct + 12 ] = xCord+1;
	towersVertices[ offset + ct + 13 ] = 0;
	towersVertices[ offset + ct + 14 ] = zCord;

	towersVertices[ offset + ct + 15 ] = xCord+1;
	towersVertices[ offset + ct + 16 ] = height;
	towersVertices[ offset + ct + 17 ] = zCord;

	ct += 18;

	// Back
	towersVertices[ offset + ct 		] = xCord;
	towersVertices[ offset + ct + 1 ] = 0;
	towersVertices[ offset + ct + 2 ] = zCord;

	towersVertices[ offset + ct +	3	] = xCord;
	towersVertices[ offset + ct + 4 ] = height;
	towersVertices[ offset + ct + 5 ] = zCord;

	towersVertices[ offset + ct + 6 ] = xCord+1;
	towersVertices[ offset + ct + 7 ] = height;
	towersVertices[ offset + ct + 8 ] = zCord;

	towersVertices[ offset + ct + 9 ] = xCord;
	towersVertices[ offset + ct + 10 ] = 0;
	towersVertices[ offset + ct + 11 ] = zCord;

	towersVertices[ offset + ct + 12 ] = xCord+1;
	towersVertices[ offset + ct + 13 ] = 0;
	towersVertices[ offset + ct + 14 ] = zCord;

	towersVertices[ offset + ct + 15 ] = xCord+1;
	towersVertices[ offset + ct + 16 ] = height;
	towersVertices[ offset + ct + 17 ] = zCord;

	ct += 18;

	// Front
	towersVertices[ offset + ct 		] = xCord;
	towersVertices[ offset + ct + 1 ] = 0;
	towersVertices[ offset + ct + 2 ] = zCord+1;

	towersVertices[ offset + ct +	3	] = xCord;
	towersVertices[ offset + ct + 4 ] = height;
	towersVertices[ offset + ct + 5 ] = zCord+1;

	towersVertices[ offset + ct + 6 ] = xCord+1;
	towersVertices[ offset + ct + 7 ] = height;
	towersVertices[ offset + ct + 8 ] = zCord+1;

	towersVertices[ offset + ct + 9 ] = xCord;
	towersVertices[ offset + ct + 10 ] = 0;
	towersVertices[ offset + ct + 11 ] = zCord+1;

	towersVertices[ offset + ct + 12 ] = xCord+1;
	towersVertices[ offset + ct + 13 ] = 0;
	towersVertices[ offset + ct + 14 ] = zCord+1;

	towersVertices[ offset + ct + 15 ] = xCord+1;
	towersVertices[ offset + ct + 16 ] = height;
	towersVertices[ offset + ct + 17 ] = zCord+1;

	ct += 18;

	// Send new tower vertices to the tower's vbo
	glBindBuffer( GL_ARRAY_BUFFER, m_towers_vbo );
	glBufferData( GL_ARRAY_BUFFER, TOTAL_COORDINATES*sizeof(float), towersVertices, GL_STATIC_DRAW );
}

//----------------------------------------------------------------------------------------
/*
 * Generate a random number between 0.0f and 1.0f
 */
float A1::randomGenerator()
{
	return (float)rand()/(float)(RAND_MAX/1);
}

//----------------------------------------------------------------------------------------
/*
 * Increase the height of the tower.  Makes sure to not go over limit of MAX_TOWER_HEIGHT.
 */
void A1::increaseTowerHeight()
{
	int currentHeight = towerHeight[currentX][currentZ];
	if ( currentHeight < MAX_TOWER_HEIGHT ) {
		towerHeight[currentX][currentZ] = currentHeight + 1;
		updateTowersVertices(currentX, currentZ);
	}
}

//----------------------------------------------------------------------------------------
/*
 * Decreases the height of the tower.  Makes sure to not go below limit of 0.
 */
void A1::decreaseTowerHeight()
{
	int currentHeight = towerHeight[currentX][currentZ];
	if ( currentHeight > 0 ) {
			towerHeight[currentX][currentZ] = currentHeight-1;
			updateTowersVertices(currentX, currentZ);
	}
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
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
bool A1::mouseMoveEvent(double xPos, double yPos)
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so
		// that you can rotate relative to the *change* in X.

		// If the mouse buton is clicked, rotate the grid by the difference between the current value
		// and the previous values of the mouse position.
		if(mouseButtonActive){
			rotateMat = glm::rotate(rotateMat, (float)(xPos-previousMouseXPos)*0.003f,vec3(0.0f,1.0f,0.0f));
		}
		previousMouseXPos = xPos;

	}

	return eventHandled;
}



//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (actions == GLFW_PRESS) {
		if (!ImGui::IsMouseHoveringAnyWindow()) {
			mouseButtonActive = true;
		}
	}

	if (actions == GLFW_RELEASE) {
		mouseButtonActive = false;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);

	// Zoom in
	if ( yOffSet > 0 && zoomCount < MAX_ZOOM_COUNT ){
		zoomCount++;
		zoom = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f * zoomCount));
		eventHandled = true;

	}

	// Zoom out
	if ( yOffSet < 0 && zoomCount > MIN_ZOOM_COUNT ) {
		zoomCount--;
		zoom = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f * zoomCount));
		eventHandled = true;
	}


	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);


	if( action == GLFW_PRESS ) {

		// dx and dz are the amount x and z are changed by the arrow button clicked
		int dx=0, dz=0;
		if ( key == GLFW_KEY_RIGHT ) {
			dx = 1;
		} else if ( key == GLFW_KEY_LEFT ) {
			dx = -1;
		} else if ( key == GLFW_KEY_UP ) {
			dz = -1;
		} else if ( key == GLFW_KEY_DOWN ) {
			dz = 1;
		}

		int destX = currentX + dx;
		int destZ = currentZ + dz;

		// Make sure that an arrow button was pressed and the destination is in bounds
		if( ( dx|dz != 0) && destX >= 0 && destX < DIM && destZ >= 0 && destZ < DIM) {

			if ( mods == GLFW_MOD_SHIFT ) {
				// If there is a shift modifier, copy the height and colour of the current cell
				towerHeight[destX][destZ] = towerHeight[currentX][currentZ];
				cellColour[destX][destZ] = cellColour[currentX][currentZ];
			}

			currentX = destX;
			currentZ = destZ;

			updateTowersVertices(currentX, currentZ);
			setCurrentColour();
			eventHandled = true;
		}

		// Closing
		if ( key == GLFW_KEY_Q ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		// Resetting
		if ( key == GLFW_KEY_R ) {
			resetValues();
			eventHandled = true;
		}
		// Increasing height of active cell
		if ( key == GLFW_KEY_SPACE ) {
			increaseTowerHeight();
			eventHandled = true;
		}
		// Decreasing height of active cell
		if ( key == GLFW_KEY_BACKSPACE ) {
			decreaseTowerHeight();
			eventHandled = true;
		}
		// Clicking on number keys on keyboard changes the selected colour
		if ( key >= GLFW_KEY_1 && key <= GLFW_KEY_8 ) {
			current_col = key-GLFW_KEY_1;
			cellColour[currentX][currentZ] = current_col;
			eventHandled = true;
		}
		// Clicking on number keys on number pad changes the selected colour
		if ( key >= GLFW_KEY_KP_1 && key <= GLFW_KEY_KP_8 ) {
			current_col = key-GLFW_KEY_KP_1;
			cellColour[currentX][currentZ] = current_col;
			eventHandled = true;
		}

		// Fly forwards
		if ( key == GLFW_KEY_W ){
			flyMat *= glm::translate(glm::vec3(0.0f,0.0f,3.2f));
			eventHandled = true;
		}
		// Fly backwards
		if ( key == GLFW_KEY_S ){
			flyMat *= glm::translate(glm::vec3(0.0f,0.0f,-3.2f));
			eventHandled = true;
		}
		// Fly towards the left
		if ( key == GLFW_KEY_A ){
			flyMat *= glm::translate(glm::vec3(3.2f,0.0f,0.0f));
			eventHandled = true;
		}
		// Fly towards the right
		if ( key == GLFW_KEY_D ){
			flyMat *= glm::translate(glm::vec3(-3.2f,0.0f,0.0f));
			eventHandled = true;
		}
	}

	return eventHandled;
}
