#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"



#include "SceneNode.hpp"

#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <stack>
#include <vector>

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};


class A3 : public CS488Window {
public:
	A3(const std::string & luaSceneFile);
	virtual ~A3();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderSceneGraph(SceneNode &node);
	void renderArcCircle();
	void traverseNode(SceneNode &node, SceneNode &root);

	glm::mat4 m_perspective;
	glm::mat4 m_view;

	LightSource m_light;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	ShaderProgram m_shader;

	//-- GL resources for trackball circle geometry:
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;

	std::stack<glm::mat4> matrixStack;
	void pushMatrix();
	void popMatrix();
	void multMatrix(glm::mat4 mat);
	void resetMatrix();

	int currentMode;

	void resetPosition();
	void resetOrientation();
	void resetJoints();
	void resetAll();

	void undoChange();
	void redoChange();

	void drawCircle();
	void zBuffer();
	void backCull();
	void frontCull();

	bool leftButtonPressed;
	bool centreButtonPressed;
	bool rightButtonPressed;

	double previousMouseXPos;
	double previousMouseYPos;

	glm::mat4 translationMatrix;
	glm::mat4 rotationMatrix;

	bool circleAppear;
	bool zBufferEnabled;
	bool cullFront;
	bool cullBack;

	bool do_picking;
	void updateShaderUniforms(
			 const ShaderProgram & shader,
			 const GeometryNode & node,
			 const glm::mat4 & viewMatrix,
			 const glm::mat4 & matrixStack,
			 const SceneNode & root,
			 const glm::mat4 & rotationMatrix,
			 const glm::mat4 & translationMatrix
	);

	std::vector<bool> connectedToJoint;

	float hue;
	bool hueForward;

	std::vector<bool> selected;

	std::vector<std::vector<glm::mat4>> rotationStack;
	std::vector<int> rotationStackIndex;

	std::vector<int> parentJoint;


	double startMouseXPosM;
	double startMouseYPosM;

	double startMouseXPosR;
	double startMouseYPosR;

	int numOfNodes;

	glm::mat4 jointRotationMatrix;

	glm::mat4 headRotationMatrix;

	int headId;
	int jointHeadId;

};
