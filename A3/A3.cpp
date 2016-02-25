#include "A3.hpp"
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
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.35, 0.35, 0.35, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

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

	//cout<<"Number is "<<m_rootNode->totalSceneNodes()<<endl;

	selected = vector<bool>(m_rootNode->totalSceneNodes(),false);
	connectedToJoint = vector<bool>(m_rootNode->totalSceneNodes(),false);

	rotationStack = vector<vector<mat4>>(m_rootNode->totalSceneNodes(),vector<mat4>());
	rotationStackIndex = vector<int>(m_rootNode->totalSceneNodes(),-1);

	numOfNodes = m_rootNode->totalSceneNodes();

	parentJoint = vector<int>(numOfNodes,-1);
	//cout<<"size "<<rotationStack[0].size()<<endl;
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
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
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
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

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
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
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
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

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perspective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
	//m_perspective = glm::perspective(degreesToRadians(110.0f), aspect, 0.1f, 1000.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));

}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(-2.0f, 5.0f, 0.5f);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
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
void A3::appLogic()
{
	// Place per frame, application logic here ...
	hue+=0.1f;

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
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

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
void A3::updateShaderUniforms(
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
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * translationMatrix * root.trans * rotationMatrix * inverse(root.trans) * matrixStack;
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
		//cout<<"I am doing picking "<<do_picking<<endl;
		if(do_picking) {
			//cout<<"bla "<<node<<endl;
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
void A3::draw() {

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



}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.
	/*
	for (const SceneNode * node : root.children) {

		if (node->m_nodeType != NodeType::GeometryNode)
			continue;

		const GeometryNode * geometryNode = static_cast<const GeometryNode *>(node);

		updateShaderUniforms(m_shader, *geometryNode, m_view * root.trans);


		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

		//-- Now render the mesh:
		m_shader.enable();
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		m_shader.disable();
	}
	*/

	//pushMatrix();
	//multMatrix(m_view);

	//multMatrix(translationMatrix);
	traverseNode(root,root);

	//translationMatrix = mat4(1.0f);
	//rotationMatrix = mat4(1.0f);
	//popMatrix();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

void A3::traverseNode(SceneNode &node, SceneNode &root) {
	pushMatrix();
	mat4 m(1.0f);
	if (node.m_nodeType == NodeType::JointNode) {
		JointNode &jointNode = static_cast<JointNode&>(node);

		//multMatrix()
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
			//cout<<child->m_nodeId<<endl;
			//cout<<"length"<<connectedToJoint.size()<<endl;
			//cout<<"setting "<<*child<<" as connected"<<endl;
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
		}

		traverseNode(*child,root);

	}
	popMatrix();

}

void A3::pushMatrix() {
	if(!matrixStack.empty()) {
		matrixStack.push(matrixStack.top());
	}
}

void A3::popMatrix() {
	matrixStack.pop();
}

void A3::multMatrix(mat4 mat) {
	matrixStack.top() *= mat;
}

void A3::resetMatrix() {
	while(!matrixStack.empty()) {
		matrixStack.pop();
	}
	matrixStack.push(mat4(1.0f));
}

void A3::resetPosition() {
	translationMatrix=mat4(1.0f);
}

void A3::resetOrientation() {
	rotationMatrix = mat4(1.0f);
}

void A3::resetJoints() {
	rotationStack = vector<vector<mat4>>(m_rootNode->totalSceneNodes(),vector<mat4>());
	rotationStackIndex = vector<int>(m_rootNode->totalSceneNodes(),-1);
}

void A3::resetAll() {
	translationMatrix=mat4(1.0f);
	rotationMatrix = mat4(1.0f);
	rotationStack = vector<vector<mat4>>(m_rootNode->totalSceneNodes(),vector<mat4>());
	rotationStackIndex = vector<int>(m_rootNode->totalSceneNodes(),-1);
}

void A3::undoChange() {
	for(int i=0;i<rotationStack.size();i++){
		if(rotationStackIndex[i] > -1) {
			rotationStackIndex[i] -= 1;
		}
	}
}

void A3::redoChange() {
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

void A3::drawCircle() {
	circleAppear = !circleAppear;
}

void A3::zBuffer() {
	zBufferEnabled = !zBufferEnabled;
}

void A3::backCull() {
	cullBack = !cullBack;
}

void A3::frontCull() {
	cullFront = !cullFront;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
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
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
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
bool A3::mouseMoveEvent (
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
 bool A3::mouseButtonInputEvent (
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
bool A3::mouseScrollEvent (
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
bool A3::windowResizeEvent (
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
bool A3::keyInputEvent (
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
	}
	// Fill in with event handling code...

	return eventHandled;
}
