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

	// The maximum height a tower can be
	static const size_t MAX_TOWER_HEIGHT = 10;
	// The total number of points needed to draw a tower
	static const size_t POINTS_PER_TOWER = 6 * 2 * 3;
	// The total number of coordinates(x,y,z) needed to draw a tower
	static const size_t COORDINATES_PER_TOWER = 6 * 2 * 3 * 3;
	// The total numerber of coordinates needed to draw all towers
	static const size_t TOTAL_COORDINATES = DIM * DIM * 6 * 2 * 3 * 3;

	// Limits of zooming in and out
	static const int MAX_ZOOM_COUNT = 30;
	static const int MIN_ZOOM_COUNT = -50;

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

	void resetValues();
	void increaseTowerHeight();
	void decreaseTowerHeight();
	float randomGenerator();
	void updateTowersVertices(int xCord, int zCord);
	void setCurrentColour();

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
	glm::mat4 flyMat;	// Matrix related to fly over using the WASD keys
	glm::mat4 rotateMat;  // Matrix related to rotation using the mouse
	glm::mat4 zoom;	// Matrix that specifies the zoom level
	int zoomCount;	// int that specifies the zoom level to put limits

	float colours[8][3];
	int current_col;			// The current colour being displayed

	// Fields related to the towers and theactive cell
	int towerHeight[DIM][DIM];
	int currentX, currentZ;

	// List of vertices correspoinding to the tower faces.  Loaded up into
	// the towers VBO.
	float *towersVertices;

	// The colour of each cell
	int cellColour[DIM][DIM];

	// The x coordinate of the mouse that was last stored
	float previousMouseXPos;

	// True if left mouse button is currently being clicked
	bool mouseButtonActive;
};
