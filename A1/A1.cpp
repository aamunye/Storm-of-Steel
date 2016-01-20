#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <ctime>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;


//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 )
{

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 3; j++) {
			colours[i][j] = randomGenerator();
		}
	}

	// Set the initial active cell of the program
	currentX = 0;
	currentZ = 0;

	// Set the heights of all the blocks to 0
	memset(towerHeight, 0, sizeof(towerHeight));
	memset(cellColour, 0, sizeof(cellColour));
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
	// Start of Towers

	/* This number corresponds to all the vertices needed to build
	 * the towers
	 */

	towersVertices = new float[ tsz ];
	// TODO replace with calls to updateTowersVertices()
	//memset(towersVertices, 0, sizeof(towersVertices));

	for(int i=0;i<tsz;i++){
		towersVertices[i]=0.0f;
	}

	glGenVertexArrays( 1, &m_towers_vao );
	glBindVertexArray( m_towers_vao );

	glGenBuffers( 1, &m_towers_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_towers_vbo );
	glBufferData( GL_ARRAY_BUFFER, tsz*sizeof(float),
		towersVertices, GL_STATIC_DRAW );

	GLint posAttrib2 = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib2 );
	glVertexAttribPointer( posAttrib2, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

		// End of Towers


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

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
	if( towerHeight[currentX][currentZ] > 0 )
	{
		current_col = cellColour[currentX][currentZ];
	} else {
		//TODO explain why this is needed
		cellColour[currentX][currentZ] = current_col;
	}
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

		if( ImGui::Button( "Print Colour Array" ) ) {
			//TODO remove later
			for(int i=0;i<16;i++){
				for(int j=0;j<16;j++){
					cout<<cellColour[j][i]<<" ";
				}
				cout<<endl;
			}
			cout<<endl;

		}

		if( ImGui::Button( "Print Heights Array" ) ) {
			//TODO remove later
			for(int i=0;i<16;i++){
				for(int j=0;j<16;j++){
					cout<<towerHeight[j][i]<<" ";
				}
				cout<<endl;
			}
			cout<<endl;

		}

		if( ImGui::Button( "Current x, z" ) ) {
			//TODO remove later
			cout<<"Current X: "<<currentX<<endl;
			cout<<"Current Z: "<<currentZ<<endl;

		}

		if( ImGui::Button( "Print x=0,z=1" ) ) {
			//TODO remove later
			cout<<endl;
			for(int i=0;i<6*2*3*3;i++){
				cout<<(1)*6*2*3*3+i<<" "<<towersVertices[(1)*6*2*3*3+i]<<endl;
			}
			cout<<endl;

		}



		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.

		ImGui::PushID( 0 );
		ImGui::ColorEdit3( "##Colour", colours[0] );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 0 ) ) {
			updateCurrentColour();
		}
		ImGui::PopID();

		ImGui::PushID( 1 );
		ImGui::ColorEdit3( "##Colour", colours[1] );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 1 ) ) {
			updateCurrentColour();
		}
		ImGui::PopID();

		ImGui::PushID( 2 );
		ImGui::ColorEdit3( "##Colour", colours[2] );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 2 ) ) {
			updateCurrentColour();
		}
		ImGui::PopID();

		ImGui::PushID( 3 );
		ImGui::ColorEdit3( "##Colour", colours[3] );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 3 ) ) {
			updateCurrentColour();
		}
		ImGui::PopID();

		ImGui::PushID( 4 );
		ImGui::ColorEdit3( "##Colour", colours[4] );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 4 ) ) {
			updateCurrentColour();
		}
		ImGui::PopID();

		ImGui::PushID( 5 );
		ImGui::ColorEdit3( "##Colour", colours[5] );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 5 ) ) {
			updateCurrentColour();
		}
		ImGui::PopID();

		ImGui::PushID( 6 );
		ImGui::ColorEdit3( "##Colour", colours[6] );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 6 ) ) {
			updateCurrentColour();
		}
		ImGui::PopID();

		ImGui::PushID( 7 );
		ImGui::ColorEdit3( "##Colour", colours[7] );
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 7 ) ) {
			updateCurrentColour();
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
	// Create a global transformation for the model (centre it).
	mat4 W;
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );

	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// Draw the cubes
		glBindVertexArray( m_towers_vao );
		//glUniform3f( col_uni, colours[cellColour[0][0]][0],colours[cellColour[0][0]][1],colours[cellColour[0][0]][2]);
		//glDrawArrays( GL_TRIANGLES, 0, 12);

		for(int x=0;x<DIM;x++){
			for(int z=0;z<DIM;z++){
				if(towerHeight[x][z] > 0){
					float cols[3];
					//cols = colours[cellColour[x][z]];
					for(int i=0;i<3;i++){
						cols[i] = colours[cellColour[x][z]][i];
					}
					if ( x == currentX && z == currentZ ) {
						glUniform3f( col_uni, randomGenerator(), randomGenerator(), randomGenerator() );
						glDrawArrays( GL_TRIANGLES, (x*DIM+z)*6*2*3, 6);
						glUniform3f( col_uni, cols[0], cols[1], cols[2] );
						glDrawArrays( GL_TRIANGLES, (x*DIM+z)*6*2*3+6, 32-6);
						glUniform3f( col_uni, 0, 0, 0 );
						glDrawArrays( GL_LINES, (x*DIM+z)*6*2*3, 32);
					} else {
						glUniform3f( col_uni, cols[0], cols[1], cols[2] );
						glDrawArrays( GL_TRIANGLES, (x*DIM+z)*6*2*3, 32);
						glUniform3f( col_uni, 1, 1, 1 );
						glDrawArrays( GL_LINES, (x*DIM+z)*6*2*3, 32);
					}
				}


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

void A1::updateTowersVertices(int xCord, int zCord)
{
	cout<<"xCord is "<<xCord<<" zCord is "<<zCord<<endl;
	int offset = (DIM * xCord + zCord) * 6 * 2 * 3 * 3;
	int height = towerHeight[xCord][zCord];
	cout<<"offset is "<<offset<<endl;

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


	glBindBuffer( GL_ARRAY_BUFFER, m_towers_vbo );
	glBufferData( GL_ARRAY_BUFFER, tsz*sizeof(float), towersVertices, GL_STATIC_DRAW );

	for(int i=0;i<6*2*3*3;i++){
		cout<<offset+i<<" "<<towersVertices[offset+i]<<endl;
	}

}

float A1::randomGenerator()
{
	return (float)rand()/(float)(RAND_MAX/1);
}

void A1::updateCurrentColour()
{
	cellColour[currentX][currentZ] = current_col;
}

void A1::increaseTowerHeight()
{
	int currentHeight = towerHeight[currentX][currentZ];
	if ( currentHeight < MAX_TOWER_HEIGHT ) {
		towerHeight[currentX][currentZ] = currentHeight + 1;
		updateTowersVertices(currentX, currentZ);
	}
}

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
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);

	// Zoom in or out.

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

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		//TODO add eventHandled=true; to these things
		// http://www.glfw.org/docs/latest/group__mods.html

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

		// dx dz is case where both are 0
		if( ( dx|dz != 0) && destX >= 0 && destX < DIM && destZ >= 0 && destZ < DIM) {

			if ( mods == GLFW_MOD_SHIFT ) {
				towerHeight[destX][destZ] = towerHeight[currentX][currentZ];
				cellColour[destX][destZ] = cellColour[currentX][currentZ];
			}
			currentX = destX;
			currentZ = destZ;
			updateTowersVertices(currentX, currentZ);
			eventHandled = true;
		}

		// Closing
		if ( key == GLFW_KEY_Q ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		if ( key == GLFW_KEY_R ) {
			//TODO resetValues();

		}
		if ( key == GLFW_KEY_SPACE ) {
			increaseTowerHeight();
			eventHandled = true;
		}
		if ( key == GLFW_KEY_BACKSPACE ) {
			decreaseTowerHeight();
			eventHandled = true;
			//TODO decreasing height
		}
	}

	return eventHandled;
}
