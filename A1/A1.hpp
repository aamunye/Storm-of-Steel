#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "grid.hpp"

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

	static const size_t DIM = 16;
	//TODO set a reasonable number
	static const size_t MAX_TOWER_HEIGHT = 10;
	static const size_t tsz = DIM * DIM * 6 * 2 * 3 * 3;

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

private:
	void initGrid();

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object

	// Fields related to tower geometry.
	GLuint m_towers_vao; // Vertex Array Object
	GLuint m_towers_vbo; // Vertex Buffer Object

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	float colours[8][3];
	int current_col;

	// Fields related to the towers and theactive cell
	int towerHeight[DIM][DIM];
	int currentX, currentZ;

	float *towersVertices;


	// Field related to cell colours
	int cellColour[DIM][DIM];

	void increaseTowerHeight();
	void decreaseTowerHeight();

	float randomGenerator();
	void resetValues();

	void updateTowersVertices(int xCord, int zCord);
	void setCurrentColour();
};
