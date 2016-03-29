#include "A5.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

//----------------------------------------------------------------------------------------
// Constructor
A5::A5(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0),
		m_vao_field(0),
		m_vbo_field(0)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A5::~A5()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A5::init()
{
	// Set the background colour.
	glClearColor(0.35, 0.35, 0.35, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	glGenVertexArrays(1, &m_vao_field);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj"),
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initZoomMatrix();
	initFlyingMatrix();

	initLightSources();


	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.


	matrixStack.push(mat4(1.0f));

	currentMode = 0;

	leftButtonPressed = false;
	centreButtonPressed = false;
	rightButtonPressed = false;

	translationMatrix = mat4(1.0f);
	rotationMatrix = mat4(1.0f);
	jointRotationMatrix = mat4(1.0f);
	headRotationMatrix = mat4(1.0f);

	circleAppear = true;
	zBufferEnabled = true;

	cullFront = false;
	cullBack = true;
	do_picking = false;

	hue = 0.0f;
	hueForward = true;


	selected = vector<bool>(m_rootNode->totalSceneNodes(),false);
	connectedToJoint = vector<bool>(m_rootNode->totalSceneNodes(),false);

	rotationStack = vector<vector<mat4>>(m_rootNode->totalSceneNodes(),vector<mat4>());
	rotationStackIndex = vector<int>(m_rootNode->totalSceneNodes(),-1);

	numOfNodes = m_rootNode->totalSceneNodes();

	parentJoint = vector<int>(numOfNodes,-1);
}

//----------------------------------------------------------------------------------------
void A5::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could not open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A5::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();

	// Build the shader
	m_shader_field.generateProgramObject();
	m_shader_field.attachVertexShader(
		getAssetFilePath( "field_VertexShader.vs" ).c_str() );
	m_shader_field.attachFragmentShader(
		getAssetFilePath( "field_FragmentShader.fs" ).c_str() );
	m_shader_field.link();
}

//----------------------------------------------------------------------------------------
void A5::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}

	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	//-- Enable input slots for m_vao_field:
	{
		glBindVertexArray(m_vao_field);

		// Set up the uniforms
		P_uni = m_shader_field.getUniformLocation( "P" );
		V_uni = m_shader_field.getUniformLocation( "V" );
		M_uni = m_shader_field.getUniformLocation( "M" );
		col_uni = m_shader_field.getUniformLocation( "colour" );

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A5::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the field.
	{
		glGenBuffers( 1, &m_vbo_field );

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

		vector<vec3> abc;
		abc.push_back(vec3(0.0f,0.0f,0.0f));
		abc.push_back(vec3(0.0f,5.0f,0.0f));
		abc.push_back(vec3(0.0f,0.0f,5.0f));


		vec3* def = &abc[0];

		resetFieldArray();
		updateFieldTriangesArray();



		// Create the vertex array to record buffer assignments.
		glGenVertexArrays( 1, &m_vao_field );
		glBindVertexArray( m_vao_field );

		// Create the cube vertex buffer
		glGenBuffers( 1, &m_vbo_field );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_field );
		//glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float), verts, GL_STATIC_DRAW );
		//glBufferData( GL_ARRAY_BUFFER, abc.size()*sizeof(vec3), def, GL_STATIC_DRAW );
		glBufferData( GL_ARRAY_BUFFER, FIELD_SEGMENTS_X*FIELD_SEGMENTS_Z*2*3*sizeof(vec3), fieldTriangesArray, GL_STATIC_DRAW );


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
	}
}

void A5::resetFieldArray() {
	for(int x=0;x<FIELD_SEGMENTS_X;x++){
		float xSegLength = (float)FIELD_LENGTH_X/(float)(FIELD_SEGMENTS_X - 1);
		for(int z=0;z<FIELD_SEGMENTS_Z;z++){
			float zSegLength = (float)FIELD_LENGTH_Z/(float)(FIELD_SEGMENTS_Z - 1);
			//fieldArray[x][z]=vec3(xSegLength*x,0.0f,zSegLength*z);
			fieldArray[x][z]=vec3(xSegLength*x,(randomGenerator()*2.0f-1.0f)/6.0f,zSegLength*z);
			//fieldArray[x][z]=vec3(xSegLength*x,-sin(x*xSegLength*2*M_PI/FIELD_LENGTH_X)*2.0f,zSegLength*z);
		}
	}
}

void A5::updateFieldTriangesArray() {
	int i = 0;
	for(int x=0; x < FIELD_SEGMENTS_X-1; x++){
		for(int z=0; z < FIELD_SEGMENTS_Z-1; z++){

			fieldTriangesArray[i] = fieldArray[x][z];
			fieldTriangesArray[i+1] = fieldArray[x][z+1];
			fieldTriangesArray[i+2] = fieldArray[x+1][z];

			fieldTriangesArray[i+3] = fieldArray[x+1][z+1];
			fieldTriangesArray[i+4] = fieldArray[x+1][z];
			fieldTriangesArray[i+5] = fieldArray[x][z+1];

			i+=6;
		}
	}
}

float A5::randomGenerator()
{
	return (float)rand()/(float)(RAND_MAX/1);
}

//----------------------------------------------------------------------------------------
void A5::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

//////////////////////////////////////////////////////////////////////////////////////////////
	// Bind VAO in order to record the data mapping for the field.
	glBindVertexArray(m_vao_field);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_field);
	glVertexAttribPointer(m_arc_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A5::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perspective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
	//m_perspective = glm::perspective(degreesToRadians(110.0f), aspect, 0.1f, 1000.0f);
}


//----------------------------------------------------------------------------------------
void A5::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 4.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 1.0f, 0.0f));

}

void A5::initFlyingMatrix() {
	m_flyover = mat4(1.0f);
}

void A5::initZoomMatrix() {
	zoomCount = 0;
}

void A5::updateZoomMatrix() {
	m_zoom = glm::scale(mat4(),vec3(pow(1.1f,zoomCount),pow(1.1f,zoomCount),pow(1.1f,zoomCount)));
	//m_zoom = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f * zoomCount));
}

//----------------------------------------------------------------------------------------
void A5::initLightSources() {
	// World-space position
	m_light.position = vec3(-2.0f, 5.0f, 0.5f);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

//----------------------------------------------------------------------------------------
void A5::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perspective));
		CHECK_GL_ERRORS;

		location = m_shader.getUniformLocation("picking");
		glUniform1i( location, do_picking );


		if ( !do_picking ) {
			//-- Set LightSource uniform for the scene:
			{
				location = m_shader.getUniformLocation("light.position");
				glUniform3fv(location, 1, value_ptr(m_light.position));
				location = m_shader.getUniformLocation("light.rgbIntensity");
				glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
				CHECK_GL_ERRORS;
			}

			//-- Set background light ambient intensity
			{
				location = m_shader.getUniformLocation("ambientIntensity");
				vec3 ambientIntensity(0.05f);
				glUniform3fv(location, 1, value_ptr(ambientIntensity));
				CHECK_GL_ERRORS;
			}
		}

	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A5::appLogic()
{
	// Place per frame, application logic here ...
	hue+=0.1f;

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A5::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_MenuBar);
	float opacity(0.5f);


	ImGui::Begin("Menu", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("Application")) {
				if(ImGui::MenuItem("Reset Position", "I")) {
					resetPosition();
				}
				if(ImGui::MenuItem("Reset Orientation", "O")) {
					resetOrientation();
				}
				if(ImGui::MenuItem("Reset Joints", "N")) {
					resetJoints();
				}
				if(ImGui::MenuItem("Reset All", "A")) {
					resetAll();
				}
				if(ImGui::MenuItem("Quit", "Q")) {
					glfwSetWindowShouldClose(m_window, GL_TRUE);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit")) {
				if(ImGui::MenuItem("Undo", "U")) {
					undoChange();
				}
				if(ImGui::MenuItem("Redo", "R")) {
					redoChange();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Options")) {
				if(ImGui::MenuItem("Circle", "C", &circleAppear, true)) {
					//drawCircle();
				}
				if(ImGui::MenuItem("Z-buffer", "Z", &zBufferEnabled, true)) {
					//zBuffer();
				}
				if(ImGui::MenuItem("Backface culling", "B", &cullBack, true)) {
					//backCull();
				}
				if(ImGui::MenuItem("Frontface culling", "F", &cullFront, true)) {
					//frontCull();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}



		if( ImGui::RadioButton( "Position/Orientation\t(P)", &currentMode, 0 ) ) {
		}
		if( ImGui::RadioButton( "Joints\t(J)", &currentMode, 1 ) ) {
		}

		//ImGui::Text( "Framerate: %.1f FPS",updateShader
	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
void A5::updateShaderUniforms(
		const  ShaderProgram & shader,
		const  GeometryNode & node,
		const  glm::mat4 & viewMatrix,
		const  glm::mat4 & matrixStack,
		const  SceneNode &root,
		const  glm::mat4 & rotationMatrix,
		const glm::mat4 & translationMatrix
) {

	shader.enable();
	{
		mat4 W;

		W = glm::translate( W, vec3( -float(FIELD_LENGTH_X)/2.0f, 0, -float(FIELD_LENGTH_Z)/2.0f ) );

		a++;
		if(a*0.001f>FIELD_LENGTH_X){
			a=0;
		}
		//a%=FIELD_SEGMENTS_X;

		mat4 m_movement;
		m_movement = glm::translate( m_movement, vec3( a*0.001f, 0.0f, 0.0f ) );

		if(node.m_name=="rightFoot") {
			rightFoot = m_movement * matrixStack * vec4(0.0f,0.0f,0.0f,1.0f);
			/*
			cout<<"x "<<rightFoot.x<<endl;
			cout<<"y "<<rightFoot.y<<endl;
			cout<<"z "<<rightFoot.z<<endl;


			float xSegLength = (float)FIELD_LENGTH_X/(float)(FIELD_SEGMENTS_X - 1);
			cout<<(int)(rightFoot.x/xSegLength)<<endl;
			cout<<endl;
			*/

		}
		float xSegLength = (float)FIELD_LENGTH_X/(float)(FIELD_SEGMENTS_X - 1);
		float zSegLength = (float)FIELD_LENGTH_Z/(float)(FIELD_SEGMENTS_Z - 1);
		W = glm::translate( W, vec3(0.0f, fieldArray[(int)(rightFoot.x/xSegLength)][(int)(rightFoot.z/zSegLength)].y+1.0f, 0.0f));
		//cout<<fieldArray[(int)(rightFoot.x/xSegLength)][(int)(rightFoot.z/zSegLength)].y<<endl;


		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		//mat4 modelView = viewMatrix * translationMatrix * root.trans * rotationMatrix * inverse(root.trans) * matrixStack;

		mat4 modelView = m_view * m_flyover * m_zoom * m_movement * W * root.trans * inverse(root.trans) * matrixStack;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;



		location = shader.getUniformLocation("picking");
		glUniform1i( location, do_picking || selected[node.m_nodeId] );

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;


		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		vec3 kd = node.material.kd;
		if(do_picking) {
			vec3 col = vec3((double)node.m_nodeId/(double)node.totalSceneNodes(),0.0,0.0);
			glUniform3fv(location, 1, value_ptr(col));
		} else if (selected[node.m_nodeId]) {
			vec3 col = vec3((double)node.m_nodeId/(double)node.totalSceneNodes(),(sin((hue+node.m_nodeId))+1.0f),(sin((hue+node.m_nodeId))+1.0f));
			glUniform3fv(location, 1, value_ptr(col));
		}
		else {
			glUniform3fv(location, 1, value_ptr(kd));
		}


		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.ks");
		vec3 ks = node.material.ks;
		glUniform3fv(location, 1, value_ptr(ks));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.shininess");
		glUniform1f(location, node.material.shininess);
		CHECK_GL_ERRORS;

	}
	shader.disable();

}

// Update mesh specific shader uniforms:
void A5::updateShaderUniforms2(
		const  ShaderProgram & shader,
		const  GeometryNode & node,
		const  glm::mat4 & viewMatrix,
		const  glm::mat4 & matrixStack,
		const  SceneNode &root,
		const  glm::mat4 & rotationMatrix,
		const glm::mat4 & translationMatrix
) {

	shader.enable();
	{
		mat4 W;
		W = glm::translate( W, vec3( -float(FIELD_LENGTH_X)/2.0f, 0, -float(FIELD_LENGTH_Z)/2.0f - 1.0f ) );

		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		//mat4 modelView = viewMatrix * translationMatrix * root.trans * rotationMatrix * inverse(root.trans) * matrixStack;
		mat4 modelView = m_view * m_flyover * m_zoom * W * root.trans * inverse(root.trans) * matrixStack;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		location = shader.getUniformLocation("picking");
		glUniform1i( location, do_picking || selected[node.m_nodeId] );

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;


		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		vec3 kd = node.material.kd;
		if(do_picking) {
			vec3 col = vec3((double)node.m_nodeId/(double)node.totalSceneNodes(),0.0,0.0);
			glUniform3fv(location, 1, value_ptr(col));
		} else if (selected[node.m_nodeId]) {
			vec3 col = vec3((double)node.m_nodeId/(double)node.totalSceneNodes(),(sin((hue+node.m_nodeId))+1.0f),(sin((hue+node.m_nodeId))+1.0f));
			glUniform3fv(location, 1, value_ptr(col));
		}
		else {
			glUniform3fv(location, 1, value_ptr(kd));
		}


		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.ks");
		vec3 ks = node.material.ks;
		glUniform3fv(location, 1, value_ptr(ks));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.shininess");
		glUniform1f(location, node.material.shininess);
		CHECK_GL_ERRORS;

	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A5::draw() {

	glEnable(GL_CULL_FACE);
	if(cullFront && cullBack){
		glCullFace(GL_FRONT_AND_BACK);
	} else if (cullFront) {
		glCullFace(GL_FRONT);
	} else if (cullBack) {
		glCullFace(GL_BACK);
	} else {
		glDisable(GL_CULL_FACE);
	}


	if(zBufferEnabled){
		glEnable( GL_DEPTH_TEST );
	}
	renderSceneGraph(*m_rootNode);


	if(zBufferEnabled){
		glDisable( GL_DEPTH_TEST );
	}




	if( circleAppear ){
		renderArcCircle();
	}

	renderField();



}

void A5::renderField() {
	m_shader_field.enable();
		glEnable( GL_DEPTH_TEST );

		// Create a global transformation for the model (centre it).
		mat4 W;
		W = glm::translate( W, vec3( -float(FIELD_LENGTH_X)/2.0f, 0, -float(FIELD_LENGTH_Z)/2.0f ) );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( m_perspective ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr(   m_view * m_flyover ) );
		//glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr(  W ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr(  m_zoom * W * mat4() ) );


		//Draw the grid
		glBindVertexArray( m_vao_field );
		glUniform3f( col_uni, 1, 0, 0 );
		//glDrawArrays( GL_LINES, 0, (3+DIM)*4 );
		//glDrawArrays( GL_LINES, 0, 4 );
		for(int i=0;i<FIELD_SEGMENTS_X*FIELD_SEGMENTS_Z*2;i++){
			glUniform3f( col_uni, (float)(FIELD_SEGMENTS_X*FIELD_SEGMENTS_Z*2-i+0.1)/(float)(FIELD_SEGMENTS_X*FIELD_SEGMENTS_Z)/2.0, 0.0f, (float)(FIELD_SEGMENTS_X*FIELD_SEGMENTS_Z*2-i+0.1)/(float)(FIELD_SEGMENTS_X*FIELD_SEGMENTS_Z*2)/2.0 );
			glDrawArrays( GL_TRIANGLES, i*3, 3 );
		}
		//glDrawArrays( GL_TRIANGLES, 0, FIELD_SEGMENTS_X*FIELD_SEGMENTS_Z*2*3 );
	m_shader_field.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A5::renderSceneGraph(SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	traverseNode(root,root);

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

void A5::traverseNode(SceneNode &node, SceneNode &root) {
	pushMatrix();
	mat4 m(1.0f);

	if(node.m_name=="shellRoot") {
		cout<<"got here "<<a<<endl;
		mat4 mm;
		mm = rotate(mm,a*0.003f, vec3(0.0f,0.0f,1.0f));
		multMatrix(mm);
	}


	if (node.m_nodeType == NodeType::JointNode) {
		JointNode &jointNode = static_cast<JointNode&>(node);

		vector<bool> selectedJoints = vector<bool>(numOfNodes, false);
		for(int i = 0; i < numOfNodes; i++){
			if(selected[i] && parentJoint[i]!=-1)selectedJoints[parentJoint[i]] = true;
		}




		//cout<<"m's length is "<<rotationStackIndex[jointNode.m_nodeId]<<endl;
		for(int i=0;i<=rotationStackIndex[jointNode.m_nodeId];i++){
			//vec2 xy = rotationStack[jointNode.m_nodeId][i];
			//m = glm::rotate(m, xy[0]/m_windowWidth, vec3(1.0f,0.0f,0.0f));
			//m = glm::rotate(m, xy[1]/m_windowHeight, vec3(0.0f,1.0f,0.0f));
			m*=rotationStack[jointNode.m_nodeId][i];
		}




		multMatrix(m);
		if(selectedJoints[jointNode.m_nodeId]){
			if( jointNode.m_nodeId == jointHeadId ) {
				multMatrix(headRotationMatrix);
			} else {
				multMatrix(jointRotationMatrix);
			}

		}

		//cout<<jointNode.m_joint_x.min<<" "<<jointNode.m_joint_x.init<<" "<<jointNode.m_joint_x.max<<endl;
	}


	multMatrix(node.trans);

	//node.set_parent_joint(node.m_nodeId);

	for (SceneNode * child : node.children) {
		/*
		if(node.m_nodeType == NodeType::GeometryNode) {
			// parent is geometry; get parent's joint parent
			child->set_parent_joint(node.parentJoint);
			//cout<<node<<" "<<node.parentJoint<<endl;
		} else if (node.m_nodeType == NodeType::JointNode) {
			// parent is joint; set parent as joint parent
			child->set_parent_joint(node.m_nodeId);

		} else {
			// parent is root node
		}
		*/
		if(child->m_name=="head"){
			headId = child->m_nodeId;
			jointHeadId = parentJoint[headId];
			//cout<<headId<<" "<<jointHeadId<<endl;
			//multMatrix(inverse(m));
			//popMatrix();
			//pushMatrix();
			//multMatrix(headRotationMatrix);
			//multMatrix(node.trans);
		}


		if (node.m_nodeType == NodeType::JointNode) {
			// parent is joint; set parent as joint parent
			child->set_parent_joint(node.m_nodeId);
			parentJoint[child->m_nodeId] = node.m_nodeId;
			connectedToJoint[child->m_nodeId] = true;
		} else {
			// parent is root node
		}

		if (child->m_nodeType == NodeType::GeometryNode){
			const GeometryNode * geometryNode = static_cast<const GeometryNode *>(child);
			pushMatrix();
			multMatrix(child->trans);
			updateShaderUniforms(m_shader, *geometryNode, m_view, matrixStack.top(), root, rotationMatrix, translationMatrix);
			popMatrix();

			// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
			BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

			//-- Now render the mesh:
			m_shader.enable();
			glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
			m_shader.disable();


			{
				pushMatrix();
				multMatrix(child->trans);
				updateShaderUniforms2(m_shader, *geometryNode, m_view, matrixStack.top(), root, rotationMatrix, translationMatrix);
				popMatrix();

				// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
				BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

				//-- Now render the mesh:
				m_shader.enable();
				glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
				m_shader.disable();
			}
		}

		traverseNode(*child,root);

	}
	popMatrix();

}

void A5::pushMatrix() {
	if(!matrixStack.empty()) {
		matrixStack.push(matrixStack.top());
	}
}

void A5::popMatrix() {
	matrixStack.pop();
}

void A5::multMatrix(mat4 mat) {
	matrixStack.top() *= mat;
}

void A5::resetMatrix() {
	while(!matrixStack.empty()) {
		matrixStack.pop();
	}
	matrixStack.push(mat4(1.0f));
}

void A5::resetPosition() {
	translationMatrix=mat4(1.0f);
}

void A5::resetOrientation() {
	rotationMatrix = mat4(1.0f);
}

void A5::resetJoints() {
	rotationStack = vector<vector<mat4>>(m_rootNode->totalSceneNodes(),vector<mat4>());
	rotationStackIndex = vector<int>(m_rootNode->totalSceneNodes(),-1);
}

void A5::resetAll() {
	translationMatrix=mat4(1.0f);
	rotationMatrix = mat4(1.0f);
	rotationStack = vector<vector<mat4>>(m_rootNode->totalSceneNodes(),vector<mat4>());
	rotationStackIndex = vector<int>(m_rootNode->totalSceneNodes(),-1);
}

void A5::undoChange() {
	for(int i=0;i<rotationStack.size();i++){
		if(rotationStackIndex[i] > -1) {
			rotationStackIndex[i] -= 1;
		}
	}
}

void A5::redoChange() {
	for(int i=0;i<rotationStack.size();i++){
		//cout<<"rotationStackIndex[i] "<<rotationStackIndex[i]<<" rotationStack[i].size()-1 "<<rotationStack[i].size()-1<<endl;
		int a = rotationStackIndex[i];
		int b = (rotationStack[i].size()-1);
		//cout<<"a is "<<a<<" and b is "<<b<<" a<b is "<<(a<b)<<endl;
		if(a<b){
		//if(rotationStackIndex[i] < (rotationStack[i].size()-1)) {
			rotationStackIndex[i] += 1;
			//cout<<"added 1"<<endl;
		}
	}

}

void A5::drawCircle() {
	circleAppear = !circleAppear;
}

void A5::zBuffer() {
	zBufferEnabled = !zBufferEnabled;
}

void A5::backCull() {
	cullBack = !cullBack;
}

void A5::frontCull() {
	cullFront = !cullFront;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A5::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A5::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A5::cursorEnterWindowEvent (
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
bool A5::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	double xDiff = xPos - previousMouseXPos;
	double yDiff = yPos - previousMouseYPos;

	//cout<<"xDiff "<<xDiff<<" yDiff "<<yDiff<<endl;
	//cout<<"xPos "<<xPos<<" yPos "<<yPos<<endl;

	if ( currentMode == 0 ) {
		//Position/Orientation
		if( leftButtonPressed ){
			translationMatrix *= glm::translate(glm::mat4(1.0f),glm::vec3(xDiff/2000,-yDiff/2000,0));
			eventHandled = true;
		}
		if( centreButtonPressed ){
			translationMatrix *= glm::translate(glm::mat4(1.0f),glm::vec3(0,0,-yDiff/2000));
			eventHandled = true;
		}
		if( rightButtonPressed ){
			// in / out
			double radius = m_framebufferWidth < m_framebufferHeight ? m_framebufferWidth : m_framebufferHeight;
			radius/= 4;
			double x = xPos - m_framebufferWidth/2;
			double y = yPos - m_framebufferHeight/2;
			y*= -1.0f;
			double z = sqrt(radius*radius - x*x - y*y);
			vec3 V2 = vec3(x,y,z);
			V2 /= sqrt(x*x+y*y+z*z);

			double xp = previousMouseXPos - m_framebufferWidth/2;
			double yp = previousMouseYPos - m_framebufferHeight/2;
			yp*=-1.0f;
			double zp = sqrt(radius*radius - xp*xp - yp*yp);
			vec3 V1 = vec3(xp,yp,zp);
			V1 /= sqrt(xp*xp+yp*yp+zp*zp);

			vec3 N = cross(V1,V2);
			float theta = acos(dot(V1,V2));



			//cout<<"theta "<<theta<<endl;

			double rsquared = x*x + y*y;
			double rsquaredp = xp*xp + yp*yp;
			bool in = false;
			bool inp = false;
			if( rsquared < radius*radius ){
				in = true;
			}
			if( rsquaredp < radius*radius ){
				inp = true;
			}

			if(inp && in) {
				// in
				rotationMatrix = rotate(rotationMatrix,theta,N);
			} else if (!inp && !in){
				y = yPos - m_framebufferHeight/2;
				yp = previousMouseYPos - m_framebufferHeight/2;
				zp = 0;
				z = 0;

				V1 = vec3(xp,yp,zp);
				V2 = vec3(x,y,z);

				// out
				float theta =  acos(dot(V1,V2)/(length(V1)*length(V2)));
				vec3 N = cross(V1,V2);

				rotationMatrix = rotate(rotationMatrix,-theta,N);


			}
			eventHandled = true;
		}
	} else if ( currentMode == 1 ) {
		//Joints
		if( leftButtonPressed ){
			eventHandled = true;
		}
		if( centreButtonPressed ){
			float dX = xPos-previousMouseXPos;
			float dY = previousMouseYPos-yPos;
			//cout<<"dX is "<<dX<<" dY is "<<dY<<endl;
			jointRotationMatrix = glm::rotate(jointRotationMatrix, (float)dX/m_windowWidth, vec3(0.0f,1.0f,0.0f));
			jointRotationMatrix = glm::rotate(jointRotationMatrix, (float)-dY/m_windowHeight, vec3(1.0f,0.0f,0.0f));
			eventHandled = true;
		}
		if( rightButtonPressed ){
			float dX = xPos-previousMouseXPos;
			headRotationMatrix = glm::rotate(headRotationMatrix, (float)dX/m_windowHeight, vec3(0.0f,1.0f,0.0f));
			eventHandled = true;
		}
	}

	previousMouseXPos = xPos;
	previousMouseYPos = yPos;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
 bool A5::mouseButtonInputEvent (
 		int button,
 		int actions,
 		int mods
 ) {
 	bool eventHandled(false);

 	if (actions == GLFW_PRESS) {
		if ( currentMode == 0 ) {
			if (!ImGui::IsMouseHoveringAnyWindow()) {
	 			if( button==GLFW_MOUSE_BUTTON_LEFT )
	 			{
	 				leftButtonPressed = true;
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
		} else if ( currentMode == 1 ) {
			if (!ImGui::IsMouseHoveringAnyWindow()) {
	 			if( button==GLFW_MOUSE_BUTTON_LEFT )
	 			{
	 				leftButtonPressed = true;
					do_picking = true;

					uploadCommonSceneUniforms();
					glClearColor(1.0, 1.0, 1.0, 1.0 );
					glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
					glClearColor(0.35, 0.35, 0.35, 1.0);

					draw();

					// I don't know if these are really necessary anymore.
					// glFlush();
					// glFinish();

					CHECK_GL_ERRORS;

					double xpos, ypos;
					glfwGetCursorPos( m_window, &xpos, &ypos );

					/*
					uploadCommonSceneUniforms();
					glClearColor(1.0, 1.0, 1.0, 1.0 );
					glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
					glClearColor(0.35, 0.35, 0.35, 1.0);

					draw();
					*/

					xpos *= double(m_framebufferWidth) / double(m_windowWidth);
					// WTF, don't know why I have to measure y relative to the bottom of
					// the window in this case.
					ypos = m_windowHeight - ypos;
					ypos *= double(m_framebufferHeight) / double(m_windowHeight);

					GLubyte buffer[ 4 ] = { 0, 0, 0, 0 };
					// A bit ugly -- don't want to swap the just-drawn false colours
					// to the screen, so read from the back buffer.
					glReadBuffer( GL_BACK );
					// Actually read the pixel at the mouse location.
					glReadPixels( int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer );

					//unsigned int what = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);

					unsigned int which = buffer[0]*m_rootNode->totalSceneNodes()/256;
					//cout<<which<<endl;
					for(int i=0;i<numOfNodes;i++){
						//cout<<"i "<<i<<" "<<connectedToJoint[i]<<endl;
					}

					//cout<<which<<connectedToJoint[which+1]<<endl;
					//cout<<which
					which = nearbyint((float)buffer[0]*(float)m_rootNode->totalSceneNodes()/256.0f);


					if(connectedToJoint[which]) {
						selected[which] = !selected[which];
					}


					do_picking = false;





					//cout<<(double)buffer[0]<<endl;
					//cout<<what<<endl;

					// I don't know if these are really necessary anymore.
					// glFlush();
					// glFinish();

					CHECK_GL_ERRORS;

					//cout<<xpos<<" "<<ypos<<endl;


	 				eventHandled = true;
	 			}
	 			else if( button==GLFW_MOUSE_BUTTON_MIDDLE )
	 			{
	 				centreButtonPressed = true;
					glfwGetCursorPos( m_window, &startMouseXPosM, &startMouseYPosM );
	 				eventHandled = true;
	 			}
	 			else if( button==GLFW_MOUSE_BUTTON_RIGHT )
	 			{
	 				rightButtonPressed = true;
					glfwGetCursorPos( m_window, &startMouseXPosR, &startMouseYPosR );

	 				eventHandled = true;
	 			}
	 		}
		}


 	}
 	if (actions == GLFW_RELEASE) {
		//translationMatrix = mat4(1.0f);
		double currXPos, currYPos;
		glfwGetCursorPos( m_window, &currXPos, &currYPos );
		if( button==GLFW_MOUSE_BUTTON_LEFT )
		{
			leftButtonPressed = false;
			eventHandled = true;
		}
		if( currentMode == 0 ) {

			if( button==GLFW_MOUSE_BUTTON_MIDDLE )
	 		{
	 			centreButtonPressed = false;
	 			eventHandled = true;
	 		}
	 		else if( button==GLFW_MOUSE_BUTTON_RIGHT )
	 		{
	 			rightButtonPressed = false;
	 			eventHandled = true;
	 		}
		} else {
			if( button==GLFW_MOUSE_BUTTON_MIDDLE )
	 		{
	 			centreButtonPressed = false;

				//cout<<"M X:"<<currXPos-startMouseXPosM<<" Y:"<<startMouseYPosM-currYPos<<endl;
				double dX = currXPos-startMouseXPosM;
				double dY = startMouseYPosM-currYPos;

				vector<bool> selectedJoints = vector<bool>(numOfNodes, false);
				for(int i = 0; i < numOfNodes; i++){
					if(selected[i] && parentJoint[i]!=-1)selectedJoints[parentJoint[i]] = true;
				}

				for(int i=0;i<numOfNodes;i++) {
					if(rotationStackIndex[i]+1 < rotationStack[i].size()) {
						//cout<<"In here for "<<i<<endl;
						rotationStack[i].erase(rotationStack[i].begin()+rotationStackIndex[i]+1, rotationStack[i].end());
						//cout<<"new size "<<rotationStack[i].size()<<endl;
					}
					if(selectedJoints[i]){
						//cout<<i<<" selected"<<endl;

						//rotationStack[i].push_back(vec2(dX,dY));
						if(i!=jointHeadId) {
							rotationStack[i].push_back(jointRotationMatrix);
							rotationStackIndex[i]+=1;
						}
						else {
							rotationStack[i].push_back(mat4(1.0f));
							rotationStackIndex[i]+=1;
						}


					} else {
						if(i!=jointHeadId) {
							rotationStack[i].push_back(mat4(1.0f));
							rotationStackIndex[i]+=1;
						}
						else {
							rotationStack[i].push_back(mat4(1.0f));
							rotationStackIndex[i]+=1;
						}
					}
				}
				//cout<<endl;

				jointRotationMatrix = mat4(1.0f);

	 			eventHandled = true;
	 		}
	 		else if( button==GLFW_MOUSE_BUTTON_RIGHT )
	 		{
	 			rightButtonPressed = false;

				vector<bool> selectedJoints = vector<bool>(numOfNodes, false);
				for(int i = 0; i < numOfNodes; i++){
					if(selected[i] && parentJoint[i]!=-1)selectedJoints[parentJoint[i]] = true;
				}

				//cout<<"R X:"<<currXPos-startMouseXPosR<<" Y:"<<startMouseYPosR-currYPos<<endl;
				//cout<<"selected[jointHeadId]"<<selected[jointHeadId]<<endl;
				for(int i=0;i<numOfNodes; i++) {

					if(rotationStackIndex[i]+1 < rotationStack[i].size()) {
						//cout<<"In here for "<<i<<endl;
						rotationStack[i].erase(rotationStack[i].begin()+rotationStackIndex[i]+1, rotationStack[i].end());
						//cout<<"new size "<<rotationStack[i].size()<<endl;
					}

					if(i!=jointHeadId) {
						rotationStack[i].push_back(mat4(1.0f));
						rotationStackIndex[i]+=1;
					}
				}
				if( selectedJoints[jointHeadId] ){
					rotationStack[jointHeadId].push_back(headRotationMatrix);
					rotationStackIndex[jointHeadId]+=1;
				} else {
					rotationStack[jointHeadId].push_back(mat4(1.0f));
					rotationStackIndex[jointHeadId]+=1;
				}


				headRotationMatrix = mat4(1.0f);

	 			eventHandled = true;
	 		}
		}




 	}

 	return eventHandled;
 }

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A5::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {

	bool eventHandled(false);

	// Zoom in
	if ( yOffSet > 0 && zoomCount < MAX_ZOOM_COUNT ){
		zoomCount++;
		updateZoomMatrix();
		eventHandled = true;

	}

	bool b = yOffSet < 0 && zoomCount > MIN_ZOOM_COUNT;

	// Zoom out
	if ( yOffSet < 0 && zoomCount > MIN_ZOOM_COUNT ) {
		zoomCount--;
		updateZoomMatrix();
		eventHandled = true;
	}


	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A5::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A5::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_Q ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		if ( key == GLFW_KEY_P ) {
			currentMode = 0;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_J ) {
			currentMode = 1;
			eventHandled = true;
		}

		if ( key == GLFW_KEY_I ) {
			resetPosition();
			eventHandled = true;
		}

		if ( key == GLFW_KEY_O ) {
			resetOrientation();
			eventHandled = true;
		}

		if ( key == GLFW_KEY_N ) {
			resetJoints();
			eventHandled = true;
		}

		if ( key == GLFW_KEY_A ) {
			resetAll();
			eventHandled = true;
		}

		if ( key == GLFW_KEY_U ) {
			undoChange();
			eventHandled = true;
		}

		if ( key == GLFW_KEY_R ) {
			redoChange();
			eventHandled = true;
		}

		if ( key == GLFW_KEY_C ) {
			drawCircle();
			eventHandled = true;
		}

		if ( key == GLFW_KEY_Z ) {
			zBuffer();
			eventHandled = true;
		}

		if ( key == GLFW_KEY_B ) {
			backCull();
			eventHandled = true;
		}

		if ( key == GLFW_KEY_F ) {
			frontCull();
			eventHandled = true;
		}


		// Fly forwards
		if ( key == GLFW_KEY_W ){
			m_flyover = glm::translate(m_flyover, glm::vec3(0.0f,0.0f,0.2f));
			eventHandled = true;
		}
		// Fly backwards
		if ( key == GLFW_KEY_S ){
			m_flyover = glm::translate(m_flyover, glm::vec3(0.0f,0.0f,-0.2f));
			eventHandled = true;
		}
		// Fly towards the left
		if ( key == GLFW_KEY_A ){
			m_flyover = glm::translate(m_flyover, glm::vec3(0.2f,0.0f,0.0f));
			eventHandled = true;
		}
		// Fly towards the right
		if ( key == GLFW_KEY_D ){
			m_flyover = glm::translate(m_flyover, glm::vec3(-0.2f,0.0f,0.0f));
			eventHandled = true;
		}
	}
	// Fill in with event handling code...

	return eventHandled;
}
