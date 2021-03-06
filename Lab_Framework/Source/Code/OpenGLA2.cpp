//
// COMP 371 Labs Framework
// ASSIGNMENT 1 - GEORGE MAVROEIDIS (40065356)
//
// Created by Nicolas Bergeron on 20/06/2019.
//

#include <iostream>
#include <list>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GLFW/glfw3.h> // cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ctime> // for random number


using namespace glm;
using namespace std;


const char* getVertexShaderSource();

const char* getLightVertexShaderSource();

const char* getFragmentShaderSource();

const char* getLightFragmentShaderSource();

int compileAndLinkShaders(const char* vertexShaderSource, const char* fragmentShaderSource);

int createVertexArrayObject();

int createVertexArrayObject2();

int createVertexArrayObject3();

int createVertexArrayObject4();

int createVertexArrayObject5();

GLuint loadTexture(const char* filename);

bool initContext();

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

GLFWwindow * window = NULL;

const float windowWidth = 768;
const float windowLength = 1024;
bool firstMouse = true;
float fov = 70.0f;

class SphereModel {
public:
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 color;
	};

	unsigned int mVAO;
	unsigned int mVBO;
	unsigned int numOfVertices;
};

unsigned int numOfVertices;

int main(int argc, char*argv[])
{
	if (!initContext()) return -1;


	// Background Color
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// Compile and link shaders here ...

	int shaderProgram = compileAndLinkShaders(getVertexShaderSource(), getFragmentShaderSource());
	int lightProgram = compileAndLinkShaders(getLightVertexShaderSource(), getLightFragmentShaderSource());

	// SHADER PROGRAM LOCATIONS
	GLuint worldMatrixLocation1 = glGetUniformLocation(shaderProgram, "worldMatrix");
	GLuint viewMatrixLocation1 = glGetUniformLocation(shaderProgram, "viewMatrix");
	GLuint projectionMatrixLocation1 = glGetUniformLocation(shaderProgram, "projectionMatrix");
	GLuint colorLocation = glGetUniformLocation(shaderProgram, "objectColor");

	// SHADER PROGRAM LOCATIONS
	GLuint worldMatrixLocation2 = glGetUniformLocation(lightProgram, "worldMatrix");
	GLuint viewMatrixLocation2 = glGetUniformLocation(lightProgram, "viewMatrix");
	GLuint projectionMatrixLocation2 = glGetUniformLocation(lightProgram, "projectionMatrix");
	


	// INITIAL Camera parameters for view transform
	vec3 cameraPosition(0.0f, 5.0f, 20.0f);
	vec3 cameraLookAt(0.0f, 0.0f, 0.0f);
	vec3 cameraUp(0.0f, 1.0f, 0.0f);
	// Other camera parameters
	float cameraSpeed = 1.0f;
	float cameraFastSpeed = 7 * cameraSpeed;
	float cameraHorizontalAngle = 90.0f;
	float cameraVerticalAngle = 0.0f;


	// SET PERSPECTIVE VIEW
	mat4 projectionMatrix = glm::perspective(glm::radians(fov),            // field of view in degrees
		800.0f / 600.0f,  // aspect ratio
		0.01f, 100.0f);   // near and far (near > 0)

	// Set initial view matrix
	mat4 viewMatrix = lookAt(cameraPosition,  // eye
		cameraLookAt,  // center
		cameraUp); // up

	setViewMatrix(shaderProgram, viewMatrix);
	setViewMatrix(lightProgram, viewMatrix);

	setProjectionMatrix(shaderProgram, projectionMatrix);
	setProjectionMatrix(lightProgram, projectionMatrix);


	// Define and upload geometry to the GPU here
	int vao = createVertexArrayObject();
	int vao2 = createVertexArrayObject2();
	int vao3 = createVertexArrayObject3();
	int vao4 = createVertexArrayObject4();
	int vao5 = createVertexArrayObject5();

	// For frame time
	float lastFrameTime = glfwGetTime();
	int lastMouseLeftState = GLFW_RELEASE;
	int lastMouseRightState = GLFW_RELEASE;
	int lastMouseMiddleState = GLFW_RELEASE;
	double lastMousePosX, lastMousePosY;
	glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

	// Other OpenGL states to set once before the Game Loop
	// Enable Backface culling
	//glEnable(GL_CULL_FACE);

	srand(time(NULL));
	GLfloat random1 = 0.0f;
	GLfloat random2 = 0.0f;

	mat4 olafWorldMatrix; // olaf matrix
	mat4 gizmoWorldMatrix; // gizmo matrix
	mat4 gridWorldMatrix; // grid matrix
	mat4 lightWorldMatrix; // light source matrix
	mat4 model = mat4(1.0f); // identity matrix
	vec3 olafPosition(0.0f, 0.0f, 0.0f); // initial olag position

	mat4 orientationMatrix = mat4(1.0f); // initialize orientation matrix
	vec2 currentOrientation(0.0f, 0.0f); // current orientation of matrix

	mat4 groupMatrix = mat4(1.0f); // matrix for group 
	mat4 bodyMatrix = mat4(1.0f); // rotation matrix for Olaf
	mat4 scaleMatrix = mat4(1.0f); // scale matrix for Olaf objects
	vec3 currentRotation(0.0f, 0.0f, 0.0f); // current rotation for rotation matrix
	vec3 currentScale(1.0f, 1.0f, 1.0f); // currentScale applied to scale Matrix
	
	// Entering Game Loop
	while (!glfwWindowShouldClose(window))
	{
		
		// Add the GL_DEPTH_BUFFER_BIT to glClear – TODO 1
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Frame time calculation
		float dt = glfwGetTime() - lastFrameTime;
		lastFrameTime += dt;

		// generate random coordinates for olaf
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			random1 = (rand() % 50) - 25;
			random2 = (rand() % 50) - 25;
		}

		

		// Draw geometry for cube

		// set orientation matrix
		orientationMatrix = rotate(rotate(mat4(1.0f), currentOrientation.x, vec3(1.0f, 0.0f, 0.0f)), currentOrientation.y, vec3(0.0f, 1.0f, 0.0f));
		setOrientationMatrix(shaderProgram, orientationMatrix);
		setOrientationMatrix(lightProgram, orientationMatrix);

		bodyMatrix = rotate(model, currentRotation.y, vec3(0.0f, 1.0f, 0.0f));
		groupMatrix = translate(model, vec3(random1, 0.0f, random2)) *  translate(model, olafPosition) * scale(model, currentScale);

		// Light Source
		glUseProgram(lightProgram);
		glBindVertexArray(vao5);
		GLuint lightWorldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
		gridWorldMatrix = translate(model, vec3(5.0f, 10.0f, 10.0f));
		setWorldMatrix(lightProgram, gridWorldMatrix);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		// GIZMO
		// X-axis

		gizmoWorldMatrix = translate(model, vec3(2.5f, 0.0f, 0.0f)) * scale(model, vec3(5.0f, 0.1f, 0.1f));
		setWorldMatrix(shaderProgram, gizmoWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Y-axis
		gizmoWorldMatrix = translate(model, vec3(0.0f, 2.5f, 0.0f)) * scale(model, vec3(0.1f, 5.0f, 0.1f));
		setWorldMatrix(shaderProgram, gizmoWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 1.0, 0.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Z-axis
		gizmoWorldMatrix = translate(model, vec3(0.0f, 0.0f, 2.5f)) * scale(model, vec3(0.1f, 0.1f, 5.0f));
		setWorldMatrix(shaderProgram, gizmoWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 1.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Draw arm-left
		olafWorldMatrix = groupMatrix * bodyMatrix * translate(model, vec3(0.0f - 1.2f, 2.2f, 0.0f)) * rotate(model, radians(45.0f), vec3(0.0f, 0.0f, 1.0f)) * scale(model, vec3(2.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &olafWorldMatrix[0][0]);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.90, 0.60, 0.40)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Draw arm-right
		olafWorldMatrix = groupMatrix * bodyMatrix * translate(model, vec3(0.0f + 1.2f, 2.2f, 0.0f)) * rotate(model, radians(-45.0f), vec3(0.0f, 0.0f, 1.0f)) * scale(model, vec3(2.0f, 0.2f, 0.2f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.90, 0.60, 0.40)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Hat1
		olafWorldMatrix = groupMatrix * translate(model, vec3(0.0f, 4.5f, 0.0f)) * bodyMatrix * scale(model, vec3(2.0f, 0.5f, 2.0f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Hat2
		olafWorldMatrix = groupMatrix * translate(model, vec3(0.0f, 5.0f, 0.0f)) * bodyMatrix * scale(model, vec3(0.8f, 0.5f, 0.8f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Mouth1
		olafWorldMatrix = groupMatrix * bodyMatrix * translate(model, vec3(0.0f, 3.4f, 0.75f)) * scale(model, vec3(0.2f, 0.1f, 0.2f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Mouth2
		olafWorldMatrix = groupMatrix * bodyMatrix * translate(model, vec3(0.15f, 3.45f, 0.75f)) * rotate(model, radians(45.0f), vec3(0.0f, 0.0f, 1.0f)) * scale(model, vec3(0.2f, 0.1f, 0.2f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Mouth3
		olafWorldMatrix = groupMatrix * bodyMatrix * translate(model, vec3(-0.15f, 3.45f, 0.75f)) * rotate(model, radians(-45.0f), vec3(0.0f, 0.0f, 1.0f)) * scale(model, vec3(0.2f, 0.1f, 0.2f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		

		// Broom Base
		olafWorldMatrix = groupMatrix * bodyMatrix * translate(model, vec3(2.0f, 1.8f, 0.1f)) * rotate(model, radians(-30.0f), vec3(0.0f, 0.0f, 1.0f)) * scale(model, vec3(0.2f, 4.0f, 0.2f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.7, 0.2, 0.2)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Broom Hold
		olafWorldMatrix = groupMatrix * bodyMatrix * translate(model, vec3(2.9f, 3.3f, 0.1f)) * rotate(model, radians(-30.0f), vec3(0.0f, 0.0f, 1.0f)) * scale(model, vec3(1.3f, 0.3f, 0.3f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.7, 0.2, 0.2)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Broom Hair
		olafWorldMatrix = groupMatrix * bodyMatrix * translate(model, vec3(2.95f, 3.4f, 0.1f)) * rotate(model, radians(-30.0f), vec3(0.0f, 0.0f, 1.0f)) * scale(model, vec3(1.2f, 0.5f, 0.25f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);


		// Switch to Pyramid
		glBindVertexArray(vao2);

		// NOSE
		olafWorldMatrix = groupMatrix * bodyMatrix * translate(model, vec3(0.0f, 3.75f, 0.5f)) * rotate(model, radians(90.0f), vec3(1.0f, 0.0f, 0.0f)) * scale(model, vec3(0.4f, 1.0f, 0.4f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0f, 0.65, 0.0f)));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// GIZMO ARROWS (R,G,B)
		gizmoWorldMatrix = translate(model, vec3(5.0f, 0.0f, 0.0f)) * rotate(model, radians(-90.0f), vec3(0.0f, 0.0f, 1.0f)) * scale(model, vec3(0.2f, 0.5f, 0.2f));
		setWorldMatrix(shaderProgram, gizmoWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		gizmoWorldMatrix = translate(model, vec3(0.0f, 5.0f, 0.0f)) * scale(model, vec3(0.2f, 0.5f, 0.2f));
		setWorldMatrix(shaderProgram, gizmoWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 1.0, 0.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		gizmoWorldMatrix = translate(model, vec3(0.0f, 0.0f, 5.0f)) * rotate(model, radians(90.0f), vec3(1.0f, 0.0f, 0.0f)) * scale(model, vec3(0.2f, 0.5f, 0.2f));
		setWorldMatrix(shaderProgram, gizmoWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 1.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(vao3);
		// Draw bottom
		olafWorldMatrix = groupMatrix * translate(model, vec3(0.0f, 1.0f, 0.0f)) * bodyMatrix * scale(model, vec3(1.2f, 1.2f, 1.2f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, numOfVertices);
		// Draw middle
		olafWorldMatrix = groupMatrix * translate(model, vec3(0.0f, 2.5f, 0.0f)) * bodyMatrix * scale(model, vec3(0.8f, 0.8f, 0.8f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, numOfVertices);
		// Draw top
		olafWorldMatrix = groupMatrix * translate(model, vec3(0.0f, 3.8f, 0.0f)) * bodyMatrix * scale(model, vec3(0.9f, 0.9f, 0.9f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, numOfVertices);

		// Button1
		olafWorldMatrix = groupMatrix * bodyMatrix * translate(model, vec3(0.0f, 2.8f, 0.7f)) * scale(model, vec3(0.1f, 0.1f, 0.1f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, numOfVertices);
		// Button2
		olafWorldMatrix = groupMatrix * bodyMatrix * translate(model, vec3(0.0f, 2.5f, 0.8f)) * scale(model, vec3(0.1f, 0.1f, 0.1f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, numOfVertices);
		// Button3
		olafWorldMatrix = groupMatrix * bodyMatrix * translate(model, vec3(0.0f, 2.2f, 0.7)) * scale(model, vec3(0.1f, 0.1f, 0.1f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, numOfVertices);
		// Eye-right
		olafWorldMatrix = groupMatrix * bodyMatrix * translate(model, vec3(0.35f, 4.0f, 0.8f)) * scale(model, vec3(0.1f, 0.1f, 0.1f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, numOfVertices);
		// Eye-left
		olafWorldMatrix = groupMatrix * bodyMatrix * translate(model, vec3(-0.35f, 4.0f, 0.8f)) * scale(model, vec3(0.1f, 0.1f, 0.1f));
		setWorldMatrix(shaderProgram, olafWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, numOfVertices);

		// Plane Textured
		glBindVertexArray(vao4);
		gridWorldMatrix = rotate(model, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)) * scale(model, vec3(100.0f, 100.0f, 100.0f));
		setWorldMatrix(shaderProgram, gridWorldMatrix);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		


		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		double mousePosX, mousePosY;
		glfwGetCursorPos(window, &mousePosX, &mousePosY);
		

		// CAMERA TILT AND PAN
		double dx = 0;
		double dy = 0;
		if (lastMouseRightState == GLFW_RELEASE && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			dx = mousePosX - lastMousePosX;
		}
		if (lastMouseMiddleState == GLFW_RELEASE && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			dy = mousePosY - lastMousePosY;
		}
		

		lastMousePosX = mousePosX;
		lastMousePosY = mousePosY;

		// Convert to spherical coordinates
		const float cameraAngularSpeed = 30.0f;
		cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
		cameraVerticalAngle -= dy * cameraAngularSpeed * dt;

		// Clamp vertical angle to [-85, 85] degrees
		cameraVerticalAngle = max(-85.0f, min(85.0f, cameraVerticalAngle));
		if (cameraHorizontalAngle > 360)
		{
			cameraHorizontalAngle -= 360;
		}
		else if (cameraHorizontalAngle < -360)
		{
			cameraHorizontalAngle += 360;
		}

		float theta = radians(cameraHorizontalAngle);
		float phi = radians(cameraVerticalAngle);

		cameraLookAt = vec3(cosf(phi)*cosf(theta), sinf(phi), -cosf(phi)*sinf(theta));
		vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));

		glm::normalize(cameraSideVector);

		// TRANSLATION INPUT
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
		{
			olafPosition.x += 5 * dt;
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
		{
			olafPosition.x -= 5 * dt;
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera up
		{
			olafPosition.z -= 5 * dt;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera down
		{
			olafPosition.z += 5 * dt;
		}

		// ROTATION INPUT
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // move camera to the left
		{
			currentRotation.y += radians(2.0f);
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // move camera to the left
		{
			currentRotation.y -= radians(2.0f);
		}

		// SCALE INPUT
		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) // move camera to the left
		{
			currentScale.x += 0.1f;
			currentScale.y += 0.1f;
			currentScale.z += 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) // move camera to the left
		{
			currentScale.x -= 0.1f;
			currentScale.y -= 0.1f;
			currentScale.z -= 0.1f;
		}

		// ORIENTATION INPUT (CHANGE COORDINATES)
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			currentOrientation.x += radians(5.0f);
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			currentOrientation.x -= radians(5.0f);
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			currentOrientation.y += radians(5.0f);
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			currentOrientation.y -= radians(5.0f);
		}
		// reset orientation
		if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) {
			currentOrientation.y = 0;
			currentOrientation.x = 0;
		}



		// RENDER TYPES
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) 
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);
		}
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// PROJECTION MATRIX
		projectionMatrix = glm::perspective(glm::radians(fov),            // field of view in degrees
			800.0f / 600.0f,  // aspect ratio
			0.01f, 100.0f);   // near and far (near > 0)
		setProjectionMatrix(shaderProgram, projectionMatrix);
		setProjectionMatrix(lightProgram, projectionMatrix);

		// VIEW MATRIX
		viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
		setViewMatrix(shaderProgram, viewMatrix);
		setViewMatrix(lightProgram, viewMatrix);

		// End Frame
		glfwSwapBuffers(window);
		glfwPollEvents();

	}


	// Shutdown GLFW
	glfwTerminate();

	return 0;
}

const char* getVertexShaderSource()
{
	// For now, you use a string for your shader code, in the assignment, shaders will be stored in .glsl files
	return
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;"
		"layout (location = 1) in vec3 aColor;"
		""
		"uniform mat4 worldMatrix;"
		"uniform mat4 orientationMatrix = mat4(1.0);"
		"uniform mat4 viewMatrix = mat4(1.0);"  // default value for view matrix (identity)
		"uniform mat4 projectionMatrix = mat4(1.0);"
		""
		"out vec3 vertexColor;"
		"void main()"
		"{"
		"   vertexColor = aColor;"
		"   mat4 modelViewProjection = projectionMatrix * viewMatrix * orientationMatrix * worldMatrix;"
		"   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
		"}";
}

const char* getFragmentShaderSource()
{
	return
		"#version 330 core\n"
		"in vec3 vertexColor;"
		"uniform vec3 objectColor;"
		""
		"out vec4 FragColor;"
		""
		"void main()"
		"{"
		"	vec4 baseColor= vec4(objectColor.r, objectColor.g, objectColor.b, 1.0f);"
		"   FragColor = baseColor;"
		"}";
}

const char* getLightVertexShaderSource() {
	// For now, you use a string for your shader code, in the assignment, shaders will be stored in .glsl files
	return
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;"
		""
		"uniform mat4 worldMatrix;"
		"uniform mat4 orientationMatrix = mat4(1.0);"
		"uniform mat4 viewMatrix = mat4(1.0);"  // default value for view matrix (identity)
		"uniform mat4 projectionMatrix = mat4(1.0);"
		""
		"void main()"
		"{"
		"   mat4 modelViewProjection = projectionMatrix * viewMatrix * orientationMatrix * worldMatrix;"
		"   gl_Position = modelViewProjection * vec4(aPos, 1.0);"
		"}";
}

const char* getLightFragmentShaderSource() {
	return
		"#version 330 core"
		"out vec4 FragColor;"
		""
		"void main()"
		"{"
		"	FragColor = vec4(1.0); // set all 4 vector values to 1.0"
		"}";
}

int compileAndLinkShaders(const char* vertexShaderSource, const char* fragmentShaderSource)
{
	// compile and link shader program
	// return shader program id
	// ------------------------------------

	// vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// link shaders
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

int createVertexArrayObject()
{
	// Cube model
	vec3 vertexArray[] = {  // position,                            color
		vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), //left - red
		vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
		vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),

		vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),
		vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
		vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),

		vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), // far - blue
		vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
		vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),

		vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
		vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
		vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),

		vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), // bottom - turquoise
		vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
		vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),

		vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
		vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
		vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),

		vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), // near - green
		vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
		vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),

		vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
		vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
		vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),

		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), // right - purple
		vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
		vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),

		vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),
		vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),

		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), // top - yellow
		vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),
		vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),

		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f),
		vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),
		vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f)
	};


	// Create a vertex array
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);


	// Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
		3,                   // size
		GL_FLOAT,            // type
		GL_FALSE,            // normalized?
		2 * sizeof(vec3), // stride - each vertex contain 2 vec3 (position, color)
		(void*)0             // array buffer offset
	);
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		2 * sizeof(vec3),
		(void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
	);
	glEnableVertexAttribArray(1);


	return vertexBufferObject;
}

int createVertexArrayObject2()
{
	// Cube model

	vec3 vertexArray[] = {  // position,                            color
		vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
		vec3(-1.0f, -1.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f),
		vec3(1.0f, -1.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f),
		
		vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
		vec3(1.0f, -1.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f),
		vec3(1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, 0.0f),

		vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
		vec3(1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, 0.0f),
		vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, 0.0f),
		
		vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
		vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, 0.0f),
		vec3(-1.0f, -1.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f),
		
};
	// grid model
	GLuint indexArray[] = {
		0, 1, 2,
		0, 2, 3
	};





	// Create a vertex array
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);


	// Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);
	/*
	GLuint elsementBufferObject;
	glGenBuffers(1, &elementBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);
	*/

	glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
		3,                   // size
		GL_FLOAT,            // type
		GL_FALSE,            // normalized?
		2 * sizeof(vec3), // stride - each vertex contain 2 vec3 (position, color)
		(void*)0             // array buffer offset
	);
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		2 * sizeof(vec3),
		(void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
	);
	glEnableVertexAttribArray(1);

	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)(6 * sizeof(vec3)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBindVertexArray(0);

	return vertexArrayObject;
}

int createVertexArrayObject3() {
	SphereModel::Vertex vertexArray[] = {  // position,                            color
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.173648, 0.000000, -0.984808), vec3(0.173648, 0.000000, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, 0.030154, -0.984808), vec3(0.171010, 0.030154, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.342020, 0.000000, -0.939693), vec3(0.342020, 0.000000, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.336824, 0.059391, -0.939693), vec3(0.336824, 0.059391, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.500000, 0.000000, -0.866025), vec3(0.500000, 0.000000, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, 0.086824, -0.866025), vec3(0.492404, 0.086824, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.642788, 0.000000, -0.766044), vec3(0.642788, 0.000000, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.633022, 0.111619, -0.766044), vec3(0.633022, 0.111619, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.766044, 0.000000, -0.642788), vec3(0.766044, 0.000000, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.754407, 0.133022, -0.642788), vec3(0.754407, 0.133022, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.866025, 0.000000, -0.500000), vec3(0.866025, 0.000000, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.852869, 0.150384, -0.500000), vec3(0.852869, 0.150384, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.939693, 0.000000, -0.342020), vec3(0.939693, 0.000000, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.925417, 0.163176, -0.342020), vec3(0.925417, 0.163176, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.984808, 0.000000, -0.173648), vec3(0.984808, 0.000000, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.969846, 0.171010, -0.173648), vec3(0.969846, 0.171010, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(1.000000, 0.000000, 0.000000), vec3(1.000000, 0.000000, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.984808, 0.173648, 0.000000), vec3(0.984808, 0.173648, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.984808, 0.000000, 0.173648), vec3(0.984808, 0.000000, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.969846, 0.171010, 0.173648), vec3(0.969846, 0.171010, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.939693, 0.000000, 0.342020), vec3(0.939693, 0.000000, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.925417, 0.163176, 0.342020), vec3(0.925417, 0.163176, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.866025, 0.000000, 0.500000), vec3(0.866025, 0.000000, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.852869, 0.150384, 0.500000), vec3(0.852869, 0.150384, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.766044, 0.000000, 0.642788), vec3(0.766044, 0.000000, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.754407, 0.133022, 0.642788), vec3(0.754407, 0.133022, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.642788, 0.000000, 0.766044), vec3(0.642788, 0.000000, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.633022, 0.111619, 0.766044), vec3(0.633022, 0.111619, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.500000, 0.000000, 0.866025), vec3(0.500000, 0.000000, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, 0.086824, 0.866025), vec3(0.492404, 0.086824, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.342020, 0.000000, 0.939693), vec3(0.342020, 0.000000, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.336824, 0.059391, 0.939693), vec3(0.336824, 0.059391, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.173648, 0.000000, 0.984808), vec3(0.173648, 0.000000, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, 0.030154, 0.984808), vec3(0.171010, 0.030154, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, 0.030154, 0.984808), vec3(0.171010, 0.030154, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.163176, 0.059391, 0.984808), vec3(0.163176, 0.059391, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.336824, 0.059391, 0.939693), vec3(0.336824, 0.059391, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, 0.116978, 0.939693), vec3(0.321394, 0.116978, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, 0.086824, 0.866025), vec3(0.492404, 0.086824, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.469846, 0.171010, 0.866025), vec3(0.469846, 0.171010, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.633022, 0.111619, 0.766044), vec3(0.633022, 0.111619, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.604023, 0.219846, 0.766044), vec3(0.604023, 0.219846, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.754407, 0.133022, 0.642788), vec3(0.754407, 0.133022, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.719846, 0.262003, 0.642788), vec3(0.719846, 0.262003, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.852869, 0.150384, 0.500000), vec3(0.852869, 0.150384, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.813798, 0.296198, 0.500000), vec3(0.813798, 0.296198, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.925417, 0.163176, 0.342020), vec3(0.925417, 0.163176, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.883022, 0.321394, 0.342020), vec3(0.883022, 0.321394, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.969846, 0.171010, 0.173648), vec3(0.969846, 0.171010, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.925417, 0.336824, 0.173648), vec3(0.925417, 0.336824, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.984808, 0.173648, 0.000000), vec3(0.984808, 0.173648, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.939693, 0.342020, 0.000000), vec3(0.939693, 0.342020, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.969846, 0.171010, -0.173648), vec3(0.969846, 0.171010, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.925417, 0.336824, -0.173648), vec3(0.925417, 0.336824, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.925417, 0.163176, -0.342020), vec3(0.925417, 0.163176, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.883022, 0.321394, -0.342020), vec3(0.883022, 0.321394, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.852869, 0.150384, -0.500000), vec3(0.852869, 0.150384, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.813798, 0.296198, -0.500000), vec3(0.813798, 0.296198, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.754407, 0.133022, -0.642788), vec3(0.754407, 0.133022, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.719846, 0.262003, -0.642788), vec3(0.719846, 0.262003, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.633022, 0.111619, -0.766044), vec3(0.633022, 0.111619, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.604023, 0.219846, -0.766044), vec3(0.604023, 0.219846, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, 0.086824, -0.866025), vec3(0.492404, 0.086824, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.469846, 0.171010, -0.866025), vec3(0.469846, 0.171010, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.336824, 0.059391, -0.939693), vec3(0.336824, 0.059391, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, 0.116978, -0.939693), vec3(0.321394, 0.116978, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, 0.030154, -0.984808), vec3(0.171010, 0.030154, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.163176, 0.059391, -0.984808), vec3(0.163176, 0.059391, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.163176, 0.059391, -0.984808), vec3(0.163176, 0.059391, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.150384, 0.086824, -0.984808), vec3(0.150384, 0.086824, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, 0.116978, -0.939693), vec3(0.321394, 0.116978, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.296198, 0.171010, -0.939693), vec3(0.296198, 0.171010, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.469846, 0.171010, -0.866025), vec3(0.469846, 0.171010, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.433013, 0.250000, -0.866025), vec3(0.433013, 0.250000, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.604023, 0.219846, -0.766044), vec3(0.604023, 0.219846, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.556670, 0.321394, -0.766044), vec3(0.556670, 0.321394, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.719846, 0.262003, -0.642788), vec3(0.719846, 0.262003, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.663414, 0.383022, -0.642788), vec3(0.663414, 0.383022, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.813798, 0.296198, -0.500000), vec3(0.813798, 0.296198, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.750000, 0.433013, -0.500000), vec3(0.750000, 0.433013, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.883022, 0.321394, -0.342020), vec3(0.883022, 0.321394, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.813798, 0.469846, -0.342020), vec3(0.813798, 0.469846, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.925417, 0.336824, -0.173648), vec3(0.925417, 0.336824, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.852869, 0.492404, -0.173648), vec3(0.852869, 0.492404, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.939693, 0.342020, 0.000000), vec3(0.939693, 0.342020, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.866025, 0.500000, 0.000000), vec3(0.866025, 0.500000, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.925417, 0.336824, 0.173648), vec3(0.925417, 0.336824, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.852869, 0.492404, 0.173648), vec3(0.852869, 0.492404, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.883022, 0.321394, 0.342020), vec3(0.883022, 0.321394, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.813798, 0.469846, 0.342020), vec3(0.813798, 0.469846, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.813798, 0.296198, 0.500000), vec3(0.813798, 0.296198, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.750000, 0.433013, 0.500000), vec3(0.750000, 0.433013, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.719846, 0.262003, 0.642788), vec3(0.719846, 0.262003, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.663414, 0.383022, 0.642788), vec3(0.663414, 0.383022, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.604023, 0.219846, 0.766044), vec3(0.604023, 0.219846, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.556670, 0.321394, 0.766044), vec3(0.556670, 0.321394, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.469846, 0.171010, 0.866025), vec3(0.469846, 0.171010, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.433013, 0.250000, 0.866025), vec3(0.433013, 0.250000, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, 0.116978, 0.939693), vec3(0.321394, 0.116978, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.296198, 0.171010, 0.939693), vec3(0.296198, 0.171010, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.163176, 0.059391, 0.984808), vec3(0.163176, 0.059391, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.150384, 0.086824, 0.984808), vec3(0.150384, 0.086824, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.150384, 0.086824, 0.984808), vec3(0.150384, 0.086824, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.133022, 0.111619, 0.984808), vec3(0.133022, 0.111619, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.296198, 0.171010, 0.939693), vec3(0.296198, 0.171010, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.262003, 0.219846, 0.939693), vec3(0.262003, 0.219846, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.433013, 0.250000, 0.866025), vec3(0.433013, 0.250000, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.383022, 0.321394, 0.866025), vec3(0.383022, 0.321394, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.556670, 0.321394, 0.766044), vec3(0.556670, 0.321394, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, 0.413176, 0.766044), vec3(0.492404, 0.413176, 0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.663414, 0.383022, 0.642788), vec3(0.663414, 0.383022, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.586824, 0.492404, 0.642788), vec3(0.586824, 0.492404, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.750000, 0.433013, 0.500000), vec3(0.750000, 0.433013, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.663414, 0.556670, 0.500000), vec3(0.663414, 0.556670, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.813798, 0.469846, 0.342020), vec3(0.813798, 0.469846, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.719846, 0.604023, 0.342020), vec3(0.719846, 0.604023, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.852869, 0.492404, 0.173648), vec3(0.852869, 0.492404, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.754407, 0.633022, 0.173648), vec3(0.754407, 0.633022, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.866025, 0.500000, 0.000000), vec3(0.866025, 0.500000, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.766044, 0.642788, 0.000000), vec3(0.766044, 0.642788, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.852869, 0.492404, -0.173648), vec3(0.852869, 0.492404, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.754407, 0.633022, -0.173648), vec3(0.754407, 0.633022, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.813798, 0.469846, -0.342020), vec3(0.813798, 0.469846, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.719846, 0.604023, -0.342020), vec3(0.719846, 0.604023, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.750000, 0.433013, -0.500000), vec3(0.750000, 0.433013, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.663414, 0.556670, -0.500000), vec3(0.663414, 0.556670, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.663414, 0.383022, -0.642788), vec3(0.663414, 0.383022, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.586824, 0.492404, -0.642788), vec3(0.586824, 0.492404, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.556670, 0.321394, -0.766044), vec3(0.556670, 0.321394, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, 0.413176, -0.766044), vec3(0.492404, 0.413176, -0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.433013, 0.250000, -0.866025), vec3(0.433013, 0.250000, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.383022, 0.321394, -0.866025), vec3(0.383022, 0.321394, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.296198, 0.171010, -0.939693), vec3(0.296198, 0.171010, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.262003, 0.219846, -0.939693), vec3(0.262003, 0.219846, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.150384, 0.086824, -0.984808), vec3(0.150384, 0.086824, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.133022, 0.111619, -0.984808), vec3(0.133022, 0.111619, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.133022, 0.111619, -0.984808), vec3(0.133022, 0.111619, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.111619, 0.133022, -0.984808), vec3(0.111619, 0.133022, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.262003, 0.219846, -0.939693), vec3(0.262003, 0.219846, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.219846, 0.262003, -0.939693), vec3(0.219846, 0.262003, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.383022, 0.321394, -0.866025), vec3(0.383022, 0.321394, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, 0.383022, -0.866025), vec3(0.321394, 0.383022, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, 0.413176, -0.766044), vec3(0.492404, 0.413176, -0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.413176, 0.492404, -0.766044), vec3(0.413176, 0.492404, -0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.586824, 0.492404, -0.642788), vec3(0.586824, 0.492404, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, 0.586824, -0.642788), vec3(0.492404, 0.586824, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.663414, 0.556670, -0.500000), vec3(0.663414, 0.556670, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.556670, 0.663414, -0.500000), vec3(0.556670, 0.663414, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.719846, 0.604023, -0.342020), vec3(0.719846, 0.604023, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.604023, 0.719846, -0.342020), vec3(0.604023, 0.719846, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.754407, 0.633022, -0.173648), vec3(0.754407, 0.633022, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.633022, 0.754407, -0.173648), vec3(0.633022, 0.754407, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.766044, 0.642788, 0.000000), vec3(0.766044, 0.642788, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.642788, 0.766044, 0.000000), vec3(0.642788, 0.766044, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.754407, 0.633022, 0.173648), vec3(0.754407, 0.633022, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.633022, 0.754407, 0.173648), vec3(0.633022, 0.754407, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.719846, 0.604023, 0.342020), vec3(0.719846, 0.604023, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.604023, 0.719846, 0.342020), vec3(0.604023, 0.719846, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.663414, 0.556670, 0.500000), vec3(0.663414, 0.556670, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.556670, 0.663414, 0.500000), vec3(0.556670, 0.663414, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.586824, 0.492404, 0.642788), vec3(0.586824, 0.492404, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, 0.586824, 0.642788), vec3(0.492404, 0.586824, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, 0.413176, 0.766044), vec3(0.492404, 0.413176, 0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.413176, 0.492404, 0.766044), vec3(0.413176, 0.492404, 0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.383022, 0.321394, 0.866025), vec3(0.383022, 0.321394, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, 0.383022, 0.866025), vec3(0.321394, 0.383022, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.262003, 0.219846, 0.939693), vec3(0.262003, 0.219846, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.219846, 0.262003, 0.939693), vec3(0.219846, 0.262003, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.133022, 0.111619, 0.984808), vec3(0.133022, 0.111619, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.111619, 0.133022, 0.984808), vec3(0.111619, 0.133022, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.111619, 0.133022, 0.984808), vec3(0.111619, 0.133022, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.086824, 0.150384, 0.984808), vec3(0.086824, 0.150384, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.219846, 0.262003, 0.939693), vec3(0.219846, 0.262003, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, 0.296198, 0.939693), vec3(0.171010, 0.296198, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, 0.383022, 0.866025), vec3(0.321394, 0.383022, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.250000, 0.433013, 0.866025), vec3(0.250000, 0.433013, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.413176, 0.492404, 0.766044), vec3(0.413176, 0.492404, 0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, 0.556670, 0.766044), vec3(0.321394, 0.556670, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, 0.586824, 0.642788), vec3(0.492404, 0.586824, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.383022, 0.663414, 0.642788), vec3(0.383022, 0.663414, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.556670, 0.663414, 0.500000), vec3(0.556670, 0.663414, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.433013, 0.750000, 0.500000), vec3(0.433013, 0.750000, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.604023, 0.719846, 0.342020), vec3(0.604023, 0.719846, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.469846, 0.813798, 0.342020), vec3(0.469846, 0.813798, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.633022, 0.754407, 0.173648), vec3(0.633022, 0.754407, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, 0.852869, 0.173648), vec3(0.492404, 0.852869, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.642788, 0.766044, 0.000000), vec3(0.642788, 0.766044, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.500000, 0.866025, 0.000000), vec3(0.500000, 0.866025, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.633022, 0.754407, -0.173648), vec3(0.633022, 0.754407, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, 0.852869, -0.173648), vec3(0.492404, 0.852869, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.604023, 0.719846, -0.342020), vec3(0.604023, 0.719846, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.469846, 0.813798, -0.342020), vec3(0.469846, 0.813798, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.556670, 0.663414, -0.500000), vec3(0.556670, 0.663414, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.433013, 0.750000, -0.500000), vec3(0.433013, 0.750000, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, 0.586824, -0.642788), vec3(0.492404, 0.586824, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.383022, 0.663414, -0.642788), vec3(0.383022, 0.663414, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.413176, 0.492404, -0.766044), vec3(0.413176, 0.492404, -0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, 0.556670, -0.766044), vec3(0.321394, 0.556670, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, 0.383022, -0.866025), vec3(0.321394, 0.383022, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.250000, 0.433013, -0.866025), vec3(0.250000, 0.433013, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.219846, 0.262003, -0.939693), vec3(0.219846, 0.262003, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, 0.296198, -0.939693), vec3(0.171010, 0.296198, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.111619, 0.133022, -0.984808), vec3(0.111619, 0.133022, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.086824, 0.150384, -0.984808), vec3(0.086824, 0.150384, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.086824, 0.150384, -0.984808), vec3(0.086824, 0.150384, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.059391, 0.163176, -0.984808), vec3(0.059391, 0.163176, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, 0.296198, -0.939693), vec3(0.171010, 0.296198, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.116978, 0.321394, -0.939693), vec3(0.116978, 0.321394, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.250000, 0.433013, -0.866025), vec3(0.250000, 0.433013, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, 0.469846, -0.866025), vec3(0.171010, 0.469846, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, 0.556670, -0.766044), vec3(0.321394, 0.556670, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.219846, 0.604023, -0.766044), vec3(0.219846, 0.604023, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.383022, 0.663414, -0.642788), vec3(0.383022, 0.663414, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.262003, 0.719846, -0.642788), vec3(0.262003, 0.719846, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.433013, 0.750000, -0.500000), vec3(0.433013, 0.750000, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.296198, 0.813798, -0.500000), vec3(0.296198, 0.813798, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.469846, 0.813798, -0.342020), vec3(0.469846, 0.813798, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, 0.883022, -0.342020), vec3(0.321394, 0.883022, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, 0.852869, -0.173648), vec3(0.492404, 0.852869, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.336824, 0.925417, -0.173648), vec3(0.336824, 0.925417, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.500000, 0.866025, 0.000000), vec3(0.500000, 0.866025, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.342020, 0.939693, 0.000000), vec3(0.342020, 0.939693, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, 0.852869, 0.173648), vec3(0.492404, 0.852869, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.336824, 0.925417, 0.173648), vec3(0.336824, 0.925417, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.469846, 0.813798, 0.342020), vec3(0.469846, 0.813798, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, 0.883022, 0.342020), vec3(0.321394, 0.883022, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.433013, 0.750000, 0.500000), vec3(0.433013, 0.750000, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.296198, 0.813798, 0.500000), vec3(0.296198, 0.813798, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.383022, 0.663414, 0.642788), vec3(0.383022, 0.663414, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.262003, 0.719846, 0.642788), vec3(0.262003, 0.719846, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, 0.556670, 0.766044), vec3(0.321394, 0.556670, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.219846, 0.604023, 0.766044), vec3(0.219846, 0.604023, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.250000, 0.433013, 0.866025), vec3(0.250000, 0.433013, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, 0.469846, 0.866025), vec3(0.171010, 0.469846, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, 0.296198, 0.939693), vec3(0.171010, 0.296198, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.116978, 0.321394, 0.939693), vec3(0.116978, 0.321394, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.086824, 0.150384, 0.984808), vec3(0.086824, 0.150384, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.059391, 0.163176, 0.984808), vec3(0.059391, 0.163176, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.059391, 0.163176, 0.984808), vec3(0.059391, 0.163176, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.030154, 0.171010, 0.984808), vec3(0.030154, 0.171010, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.116978, 0.321394, 0.939693), vec3(0.116978, 0.321394, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.059391, 0.336824, 0.939693), vec3(0.059391, 0.336824, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, 0.469846, 0.866025), vec3(0.171010, 0.469846, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.086824, 0.492404, 0.866025), vec3(0.086824, 0.492404, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.219846, 0.604023, 0.766044), vec3(0.219846, 0.604023, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.111619, 0.633022, 0.766044), vec3(0.111619, 0.633022, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.262003, 0.719846, 0.642788), vec3(0.262003, 0.719846, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.133022, 0.754407, 0.642788), vec3(0.133022, 0.754407, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.296198, 0.813798, 0.500000), vec3(0.296198, 0.813798, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.150384, 0.852869, 0.500000), vec3(0.150384, 0.852869, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, 0.883022, 0.342020), vec3(0.321394, 0.883022, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.163176, 0.925417, 0.342020), vec3(0.163176, 0.925417, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.336824, 0.925417, 0.173648), vec3(0.336824, 0.925417, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, 0.969846, 0.173648), vec3(0.171010, 0.969846, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.342020, 0.939693, 0.000000), vec3(0.342020, 0.939693, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.173648, 0.984808, 0.000000), vec3(0.173648, 0.984808, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.336824, 0.925417, -0.173648), vec3(0.336824, 0.925417, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, 0.969846, -0.173648), vec3(0.171010, 0.969846, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, 0.883022, -0.342020), vec3(0.321394, 0.883022, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.163176, 0.925417, -0.342020), vec3(0.163176, 0.925417, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.296198, 0.813798, -0.500000), vec3(0.296198, 0.813798, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.150384, 0.852869, -0.500000), vec3(0.150384, 0.852869, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.262003, 0.719846, -0.642788), vec3(0.262003, 0.719846, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.133022, 0.754407, -0.642788), vec3(0.133022, 0.754407, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.219846, 0.604023, -0.766044), vec3(0.219846, 0.604023, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.111619, 0.633022, -0.766044), vec3(0.111619, 0.633022, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, 0.469846, -0.866025), vec3(0.171010, 0.469846, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.086824, 0.492404, -0.866025), vec3(0.086824, 0.492404, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.116978, 0.321394, -0.939693), vec3(0.116978, 0.321394, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.059391, 0.336824, -0.939693), vec3(0.059391, 0.336824, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.059391, 0.163176, -0.984808), vec3(0.059391, 0.163176, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.030154, 0.171010, -0.984808), vec3(0.030154, 0.171010, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.030154, 0.171010, -0.984808), vec3(0.030154, 0.171010, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.173648, -0.984808), vec3(0.000000, 0.173648, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.059391, 0.336824, -0.939693), vec3(0.059391, 0.336824, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.342020, -0.939693), vec3(0.000000, 0.342020, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.086824, 0.492404, -0.866025), vec3(0.086824, 0.492404, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.500000, -0.866025), vec3(0.000000, 0.500000, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.111619, 0.633022, -0.766044), vec3(0.111619, 0.633022, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.642788, -0.766044), vec3(0.000000, 0.642788, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.133022, 0.754407, -0.642788), vec3(0.133022, 0.754407, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.766044, -0.642788), vec3(0.000000, 0.766044, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.150384, 0.852869, -0.500000), vec3(0.150384, 0.852869, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.866025, -0.500000), vec3(0.000000, 0.866025, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.163176, 0.925417, -0.342020), vec3(0.163176, 0.925417, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.939693, -0.342020), vec3(0.000000, 0.939693, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, 0.969846, -0.173648), vec3(0.171010, 0.969846, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.984808, -0.173648), vec3(0.000000, 0.984808, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.173648, 0.984808, 0.000000), vec3(0.173648, 0.984808, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 1.000000, 0.000000), vec3(0.000000, 1.000000, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, 0.969846, 0.173648), vec3(0.171010, 0.969846, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.984808, 0.173648), vec3(0.000000, 0.984808, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.163176, 0.925417, 0.342020), vec3(0.163176, 0.925417, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.939693, 0.342020), vec3(0.000000, 0.939693, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.150384, 0.852869, 0.500000), vec3(0.150384, 0.852869, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.866025, 0.500000), vec3(0.000000, 0.866025, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.133022, 0.754407, 0.642788), vec3(0.133022, 0.754407, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.766044, 0.642788), vec3(0.000000, 0.766044, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.111619, 0.633022, 0.766044), vec3(0.111619, 0.633022, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.642788, 0.766044), vec3(0.000000, 0.642788, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.086824, 0.492404, 0.866025), vec3(0.086824, 0.492404, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.500000, 0.866025), vec3(0.000000, 0.500000, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.059391, 0.336824, 0.939693), vec3(0.059391, 0.336824, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.342020, 0.939693), vec3(0.000000, 0.342020, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.030154, 0.171010, 0.984808), vec3(0.030154, 0.171010, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.173648, 0.984808), vec3(0.000000, 0.173648, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.173648, 0.984808), vec3(0.000000, 0.173648, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.030154, 0.171010, 0.984808), vec3(-0.030154, 0.171010, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.342020, 0.939693), vec3(0.000000, 0.342020, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.059391, 0.336824, 0.939693), vec3(-0.059391, 0.336824, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.500000, 0.866025), vec3(0.000000, 0.500000, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.086824, 0.492404, 0.866025), vec3(-0.086824, 0.492404, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.642788, 0.766044), vec3(0.000000, 0.642788, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.111619, 0.633022, 0.766044), vec3(-0.111619, 0.633022, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.766044, 0.642788), vec3(0.000000, 0.766044, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.133022, 0.754407, 0.642788), vec3(-0.133022, 0.754407, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.866025, 0.500000), vec3(0.000000, 0.866025, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.150384, 0.852869, 0.500000), vec3(-0.150384, 0.852869, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.939693, 0.342020), vec3(0.000000, 0.939693, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.163176, 0.925417, 0.342020), vec3(-0.163176, 0.925417, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.984808, 0.173648), vec3(0.000000, 0.984808, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, 0.969846, 0.173648), vec3(-0.171010, 0.969846, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 1.000000, 0.000000), vec3(0.000000, 1.000000, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.173648, 0.984808, 0.000000), vec3(-0.173648, 0.984808, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.984808, -0.173648), vec3(0.000000, 0.984808, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, 0.969846, -0.173648), vec3(-0.171010, 0.969846, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.939693, -0.342020), vec3(0.000000, 0.939693, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.163176, 0.925417, -0.342020), vec3(-0.163176, 0.925417, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.866025, -0.500000), vec3(0.000000, 0.866025, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.150384, 0.852869, -0.500000), vec3(-0.150384, 0.852869, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.766044, -0.642788), vec3(0.000000, 0.766044, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.133022, 0.754407, -0.642788), vec3(-0.133022, 0.754407, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.642788, -0.766044), vec3(0.000000, 0.642788, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.111619, 0.633022, -0.766044), vec3(-0.111619, 0.633022, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.500000, -0.866025), vec3(0.000000, 0.500000, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.086824, 0.492404, -0.866025), vec3(-0.086824, 0.492404, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.342020, -0.939693), vec3(0.000000, 0.342020, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.059391, 0.336824, -0.939693), vec3(-0.059391, 0.336824, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.173648, -0.984808), vec3(0.000000, 0.173648, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.030154, 0.171010, -0.984808), vec3(-0.030154, 0.171010, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.030154, 0.171010, -0.984808), vec3(-0.030154, 0.171010, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.059391, 0.163176, -0.984808), vec3(-0.059391, 0.163176, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.059391, 0.336824, -0.939693), vec3(-0.059391, 0.336824, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.116978, 0.321394, -0.939693), vec3(-0.116978, 0.321394, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.086824, 0.492404, -0.866025), vec3(-0.086824, 0.492404, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, 0.469846, -0.866025), vec3(-0.171010, 0.469846, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.111619, 0.633022, -0.766044), vec3(-0.111619, 0.633022, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.219846, 0.604023, -0.766044), vec3(-0.219846, 0.604023, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.133022, 0.754407, -0.642788), vec3(-0.133022, 0.754407, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.262003, 0.719846, -0.642788), vec3(-0.262003, 0.719846, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.150384, 0.852869, -0.500000), vec3(-0.150384, 0.852869, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.296198, 0.813798, -0.500000), vec3(-0.296198, 0.813798, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.163176, 0.925417, -0.342020), vec3(-0.163176, 0.925417, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, 0.883022, -0.342020), vec3(-0.321394, 0.883022, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, 0.969846, -0.173648), vec3(-0.171010, 0.969846, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.336824, 0.925417, -0.173648), vec3(-0.336824, 0.925417, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.173648, 0.984808, 0.000000), vec3(-0.173648, 0.984808, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.342020, 0.939693, 0.000000), vec3(-0.342020, 0.939693, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, 0.969846, 0.173648), vec3(-0.171010, 0.969846, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.336824, 0.925417, 0.173648), vec3(-0.336824, 0.925417, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.163176, 0.925417, 0.342020), vec3(-0.163176, 0.925417, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, 0.883022, 0.342020), vec3(-0.321394, 0.883022, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.150384, 0.852869, 0.500000), vec3(-0.150384, 0.852869, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.296198, 0.813798, 0.500000), vec3(-0.296198, 0.813798, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.133022, 0.754407, 0.642788), vec3(-0.133022, 0.754407, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.262003, 0.719846, 0.642788), vec3(-0.262003, 0.719846, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.111619, 0.633022, 0.766044), vec3(-0.111619, 0.633022, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.219846, 0.604023, 0.766044), vec3(-0.219846, 0.604023, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.086824, 0.492404, 0.866025), vec3(-0.086824, 0.492404, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, 0.469846, 0.866025), vec3(-0.171010, 0.469846, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.059391, 0.336824, 0.939693), vec3(-0.059391, 0.336824, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.116978, 0.321394, 0.939693), vec3(-0.116978, 0.321394, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.030154, 0.171010, 0.984808), vec3(-0.030154, 0.171010, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.059391, 0.163176, 0.984808), vec3(-0.059391, 0.163176, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.059391, 0.163176, 0.984808), vec3(-0.059391, 0.163176, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.086824, 0.150384, 0.984808), vec3(-0.086824, 0.150384, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.116978, 0.321394, 0.939693), vec3(-0.116978, 0.321394, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, 0.296198, 0.939693), vec3(-0.171010, 0.296198, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, 0.469846, 0.866025), vec3(-0.171010, 0.469846, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.250000, 0.433013, 0.866025), vec3(-0.250000, 0.433013, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.219846, 0.604023, 0.766044), vec3(-0.219846, 0.604023, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, 0.556670, 0.766044), vec3(-0.321394, 0.556670, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.262003, 0.719846, 0.642788), vec3(-0.262003, 0.719846, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.383022, 0.663414, 0.642788), vec3(-0.383022, 0.663414, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.296198, 0.813798, 0.500000), vec3(-0.296198, 0.813798, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.433013, 0.750000, 0.500000), vec3(-0.433013, 0.750000, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, 0.883022, 0.342020), vec3(-0.321394, 0.883022, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.469846, 0.813798, 0.342020), vec3(-0.469846, 0.813798, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.336824, 0.925417, 0.173648), vec3(-0.336824, 0.925417, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, 0.852869, 0.173648), vec3(-0.492404, 0.852869, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.342020, 0.939693, 0.000000), vec3(-0.342020, 0.939693, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.500000, 0.866025, 0.000000), vec3(-0.500000, 0.866025, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.336824, 0.925417, -0.173648), vec3(-0.336824, 0.925417, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, 0.852869, -0.173648), vec3(-0.492404, 0.852869, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, 0.883022, -0.342020), vec3(-0.321394, 0.883022, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.469846, 0.813798, -0.342020), vec3(-0.469846, 0.813798, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.296198, 0.813798, -0.500000), vec3(-0.296198, 0.813798, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.433013, 0.750000, -0.500000), vec3(-0.433013, 0.750000, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.262003, 0.719846, -0.642788), vec3(-0.262003, 0.719846, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.383022, 0.663414, -0.642788), vec3(-0.383022, 0.663414, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.219846, 0.604023, -0.766044), vec3(-0.219846, 0.604023, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, 0.556670, -0.766044), vec3(-0.321394, 0.556670, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, 0.469846, -0.866025), vec3(-0.171010, 0.469846, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.250000, 0.433013, -0.866025), vec3(-0.250000, 0.433013, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.116978, 0.321394, -0.939693), vec3(-0.116978, 0.321394, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, 0.296198, -0.939693), vec3(-0.171010, 0.296198, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.059391, 0.163176, -0.984808), vec3(-0.059391, 0.163176, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.086824, 0.150384, -0.984808), vec3(-0.086824, 0.150384, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.086824, 0.150384, -0.984808), vec3(-0.086824, 0.150384, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.111619, 0.133022, -0.984808), vec3(-0.111619, 0.133022, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, 0.296198, -0.939693), vec3(-0.171010, 0.296198, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.219846, 0.262003, -0.939693), vec3(-0.219846, 0.262003, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.250000, 0.433013, -0.866025), vec3(-0.250000, 0.433013, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, 0.383022, -0.866025), vec3(-0.321394, 0.383022, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, 0.556670, -0.766044), vec3(-0.321394, 0.556670, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.413176, 0.492404, -0.766044), vec3(-0.413176, 0.492404, -0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.383022, 0.663414, -0.642788), vec3(-0.383022, 0.663414, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, 0.586824, -0.642788), vec3(-0.492404, 0.586824, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.433013, 0.750000, -0.500000), vec3(-0.433013, 0.750000, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.556670, 0.663414, -0.500000), vec3(-0.556670, 0.663414, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.469846, 0.813798, -0.342020), vec3(-0.469846, 0.813798, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.604023, 0.719846, -0.342020), vec3(-0.604023, 0.719846, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, 0.852869, -0.173648), vec3(-0.492404, 0.852869, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.633022, 0.754407, -0.173648), vec3(-0.633022, 0.754407, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.500000, 0.866025, 0.000000), vec3(-0.500000, 0.866025, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.642788, 0.766044, 0.000000), vec3(-0.642788, 0.766044, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, 0.852869, 0.173648), vec3(-0.492404, 0.852869, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.633022, 0.754407, 0.173648), vec3(-0.633022, 0.754407, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.469846, 0.813798, 0.342020), vec3(-0.469846, 0.813798, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.604023, 0.719846, 0.342020), vec3(-0.604023, 0.719846, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.433013, 0.750000, 0.500000), vec3(-0.433013, 0.750000, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.556670, 0.663414, 0.500000), vec3(-0.556670, 0.663414, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.383022, 0.663414, 0.642788), vec3(-0.383022, 0.663414, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, 0.586824, 0.642788), vec3(-0.492404, 0.586824, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, 0.556670, 0.766044), vec3(-0.321394, 0.556670, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.413176, 0.492404, 0.766044), vec3(-0.413176, 0.492404, 0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.250000, 0.433013, 0.866025), vec3(-0.250000, 0.433013, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, 0.383022, 0.866025), vec3(-0.321394, 0.383022, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, 0.296198, 0.939693), vec3(-0.171010, 0.296198, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.219846, 0.262003, 0.939693), vec3(-0.219846, 0.262003, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.086824, 0.150384, 0.984808), vec3(-0.086824, 0.150384, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.111619, 0.133022, 0.984808), vec3(-0.111619, 0.133022, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.111619, 0.133022, 0.984808), vec3(-0.111619, 0.133022, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.133022, 0.111619, 0.984808), vec3(-0.133022, 0.111619, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.219846, 0.262003, 0.939693), vec3(-0.219846, 0.262003, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.262003, 0.219846, 0.939693), vec3(-0.262003, 0.219846, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, 0.383022, 0.866025), vec3(-0.321394, 0.383022, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.383022, 0.321394, 0.866025), vec3(-0.383022, 0.321394, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.413176, 0.492404, 0.766044), vec3(-0.413176, 0.492404, 0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, 0.413176, 0.766044), vec3(-0.492404, 0.413176, 0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, 0.586824, 0.642788), vec3(-0.492404, 0.586824, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.586824, 0.492404, 0.642788), vec3(-0.586824, 0.492404, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.556670, 0.663414, 0.500000), vec3(-0.556670, 0.663414, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.663414, 0.556670, 0.500000), vec3(-0.663414, 0.556670, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.604023, 0.719846, 0.342020), vec3(-0.604023, 0.719846, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.719846, 0.604023, 0.342020), vec3(-0.719846, 0.604023, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.633022, 0.754407, 0.173648), vec3(-0.633022, 0.754407, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.754407, 0.633022, 0.173648), vec3(-0.754407, 0.633022, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.642788, 0.766044, 0.000000), vec3(-0.642788, 0.766044, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.766044, 0.642788, 0.000000), vec3(-0.766044, 0.642788, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.633022, 0.754407, -0.173648), vec3(-0.633022, 0.754407, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.754407, 0.633022, -0.173648), vec3(-0.754407, 0.633022, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.604023, 0.719846, -0.342020), vec3(-0.604023, 0.719846, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.719846, 0.604023, -0.342020), vec3(-0.719846, 0.604023, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.556670, 0.663414, -0.500000), vec3(-0.556670, 0.663414, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.663414, 0.556670, -0.500000), vec3(-0.663414, 0.556670, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, 0.586824, -0.642788), vec3(-0.492404, 0.586824, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.586824, 0.492404, -0.642788), vec3(-0.586824, 0.492404, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.413176, 0.492404, -0.766044), vec3(-0.413176, 0.492404, -0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, 0.413176, -0.766044), vec3(-0.492404, 0.413176, -0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, 0.383022, -0.866025), vec3(-0.321394, 0.383022, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.383022, 0.321394, -0.866025), vec3(-0.383022, 0.321394, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.219846, 0.262003, -0.939693), vec3(-0.219846, 0.262003, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.262003, 0.219846, -0.939693), vec3(-0.262003, 0.219846, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.111619, 0.133022, -0.984808), vec3(-0.111619, 0.133022, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.133022, 0.111619, -0.984808), vec3(-0.133022, 0.111619, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.133022, 0.111619, -0.984808), vec3(-0.133022, 0.111619, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.150384, 0.086824, -0.984808), vec3(-0.150384, 0.086824, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.262003, 0.219846, -0.939693), vec3(-0.262003, 0.219846, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.296198, 0.171010, -0.939693), vec3(-0.296198, 0.171010, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.383022, 0.321394, -0.866025), vec3(-0.383022, 0.321394, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.433013, 0.250000, -0.866025), vec3(-0.433013, 0.250000, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, 0.413176, -0.766044), vec3(-0.492404, 0.413176, -0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.556670, 0.321394, -0.766044), vec3(-0.556670, 0.321394, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.586824, 0.492404, -0.642788), vec3(-0.586824, 0.492404, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.663414, 0.383022, -0.642788), vec3(-0.663414, 0.383022, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.663414, 0.556670, -0.500000), vec3(-0.663414, 0.556670, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.750000, 0.433013, -0.500000), vec3(-0.750000, 0.433013, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.719846, 0.604023, -0.342020), vec3(-0.719846, 0.604023, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.813798, 0.469846, -0.342020), vec3(-0.813798, 0.469846, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.754407, 0.633022, -0.173648), vec3(-0.754407, 0.633022, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.852869, 0.492404, -0.173648), vec3(-0.852869, 0.492404, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.766044, 0.642788, 0.000000), vec3(-0.766044, 0.642788, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.866025, 0.500000, 0.000000), vec3(-0.866025, 0.500000, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.754407, 0.633022, 0.173648), vec3(-0.754407, 0.633022, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.852869, 0.492404, 0.173648), vec3(-0.852869, 0.492404, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.719846, 0.604023, 0.342020), vec3(-0.719846, 0.604023, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.813798, 0.469846, 0.342020), vec3(-0.813798, 0.469846, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.663414, 0.556670, 0.500000), vec3(-0.663414, 0.556670, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.750000, 0.433013, 0.500000), vec3(-0.750000, 0.433013, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.586824, 0.492404, 0.642788), vec3(-0.586824, 0.492404, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.663414, 0.383022, 0.642788), vec3(-0.663414, 0.383022, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, 0.413176, 0.766044), vec3(-0.492404, 0.413176, 0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.556670, 0.321394, 0.766044), vec3(-0.556670, 0.321394, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.383022, 0.321394, 0.866025), vec3(-0.383022, 0.321394, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.433013, 0.250000, 0.866025), vec3(-0.433013, 0.250000, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.262003, 0.219846, 0.939693), vec3(-0.262003, 0.219846, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.296198, 0.171010, 0.939693), vec3(-0.296198, 0.171010, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.133022, 0.111619, 0.984808), vec3(-0.133022, 0.111619, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.150384, 0.086824, 0.984808), vec3(-0.150384, 0.086824, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.150384, 0.086824, 0.984808), vec3(-0.150384, 0.086824, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.163176, 0.059391, 0.984808), vec3(-0.163176, 0.059391, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.296198, 0.171010, 0.939693), vec3(-0.296198, 0.171010, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, 0.116978, 0.939693), vec3(-0.321394, 0.116978, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.433013, 0.250000, 0.866025), vec3(-0.433013, 0.250000, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.469846, 0.171010, 0.866025), vec3(-0.469846, 0.171010, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.556670, 0.321394, 0.766044), vec3(-0.556670, 0.321394, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.604023, 0.219846, 0.766044), vec3(-0.604023, 0.219846, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.663414, 0.383022, 0.642788), vec3(-0.663414, 0.383022, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.719846, 0.262003, 0.642788), vec3(-0.719846, 0.262003, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.750000, 0.433013, 0.500000), vec3(-0.750000, 0.433013, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.813798, 0.296198, 0.500000), vec3(-0.813798, 0.296198, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.813798, 0.469846, 0.342020), vec3(-0.813798, 0.469846, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.883022, 0.321394, 0.342020), vec3(-0.883022, 0.321394, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.852869, 0.492404, 0.173648), vec3(-0.852869, 0.492404, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.925417, 0.336824, 0.173648), vec3(-0.925417, 0.336824, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.866025, 0.500000, 0.000000), vec3(-0.866025, 0.500000, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.939693, 0.342020, 0.000000), vec3(-0.939693, 0.342020, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.852869, 0.492404, -0.173648), vec3(-0.852869, 0.492404, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.925417, 0.336824, -0.173648), vec3(-0.925417, 0.336824, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.813798, 0.469846, -0.342020), vec3(-0.813798, 0.469846, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.883022, 0.321394, -0.342020), vec3(-0.883022, 0.321394, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.750000, 0.433013, -0.500000), vec3(-0.750000, 0.433013, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.813798, 0.296198, -0.500000), vec3(-0.813798, 0.296198, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.663414, 0.383022, -0.642788), vec3(-0.663414, 0.383022, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.719846, 0.262003, -0.642788), vec3(-0.719846, 0.262003, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.556670, 0.321394, -0.766044), vec3(-0.556670, 0.321394, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.604023, 0.219846, -0.766044), vec3(-0.604023, 0.219846, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.433013, 0.250000, -0.866025), vec3(-0.433013, 0.250000, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.469846, 0.171010, -0.866025), vec3(-0.469846, 0.171010, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.296198, 0.171010, -0.939693), vec3(-0.296198, 0.171010, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, 0.116978, -0.939693), vec3(-0.321394, 0.116978, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.150384, 0.086824, -0.984808), vec3(-0.150384, 0.086824, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.163176, 0.059391, -0.984808), vec3(-0.163176, 0.059391, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.163176, 0.059391, -0.984808), vec3(-0.163176, 0.059391, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, 0.030154, -0.984808), vec3(-0.171010, 0.030154, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, 0.116978, -0.939693), vec3(-0.321394, 0.116978, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.336824, 0.059391, -0.939693), vec3(-0.336824, 0.059391, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.469846, 0.171010, -0.866025), vec3(-0.469846, 0.171010, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, 0.086824, -0.866025), vec3(-0.492404, 0.086824, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.604023, 0.219846, -0.766044), vec3(-0.604023, 0.219846, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.633022, 0.111619, -0.766044), vec3(-0.633022, 0.111619, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.719846, 0.262003, -0.642788), vec3(-0.719846, 0.262003, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.754407, 0.133022, -0.642788), vec3(-0.754407, 0.133022, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.813798, 0.296198, -0.500000), vec3(-0.813798, 0.296198, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.852869, 0.150384, -0.500000), vec3(-0.852869, 0.150384, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.883022, 0.321394, -0.342020), vec3(-0.883022, 0.321394, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.925417, 0.163176, -0.342020), vec3(-0.925417, 0.163176, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.925417, 0.336824, -0.173648), vec3(-0.925417, 0.336824, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.969846, 0.171010, -0.173648), vec3(-0.969846, 0.171010, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.939693, 0.342020, 0.000000), vec3(-0.939693, 0.342020, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.984808, 0.173648, 0.000000), vec3(-0.984808, 0.173648, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.925417, 0.336824, 0.173648), vec3(-0.925417, 0.336824, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.969846, 0.171010, 0.173648), vec3(-0.969846, 0.171010, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.883022, 0.321394, 0.342020), vec3(-0.883022, 0.321394, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.925417, 0.163176, 0.342020), vec3(-0.925417, 0.163176, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.813798, 0.296198, 0.500000), vec3(-0.813798, 0.296198, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.852869, 0.150384, 0.500000), vec3(-0.852869, 0.150384, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.719846, 0.262003, 0.642788), vec3(-0.719846, 0.262003, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.754407, 0.133022, 0.642788), vec3(-0.754407, 0.133022, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.604023, 0.219846, 0.766044), vec3(-0.604023, 0.219846, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.633022, 0.111619, 0.766044), vec3(-0.633022, 0.111619, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.469846, 0.171010, 0.866025), vec3(-0.469846, 0.171010, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, 0.086824, 0.866025), vec3(-0.492404, 0.086824, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, 0.116978, 0.939693), vec3(-0.321394, 0.116978, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.336824, 0.059391, 0.939693), vec3(-0.336824, 0.059391, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.163176, 0.059391, 0.984808), vec3(-0.163176, 0.059391, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, 0.030154, 0.984808), vec3(-0.171010, 0.030154, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, 0.030154, 0.984808), vec3(-0.171010, 0.030154, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.173648, 0.000000, 0.984808), vec3(-0.173648, 0.000000, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.336824, 0.059391, 0.939693), vec3(-0.336824, 0.059391, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.342020, 0.000000, 0.939693), vec3(-0.342020, 0.000000, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, 0.086824, 0.866025), vec3(-0.492404, 0.086824, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.500000, 0.000000, 0.866025), vec3(-0.500000, 0.000000, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.633022, 0.111619, 0.766044), vec3(-0.633022, 0.111619, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.642788, 0.000000, 0.766044), vec3(-0.642788, 0.000000, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.754407, 0.133022, 0.642788), vec3(-0.754407, 0.133022, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.766044, 0.000000, 0.642788), vec3(-0.766044, 0.000000, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.852869, 0.150384, 0.500000), vec3(-0.852869, 0.150384, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.866025, 0.000000, 0.500000), vec3(-0.866025, 0.000000, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.925417, 0.163176, 0.342020), vec3(-0.925417, 0.163176, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.939693, 0.000000, 0.342020), vec3(-0.939693, 0.000000, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.969846, 0.171010, 0.173648), vec3(-0.969846, 0.171010, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.984808, 0.000000, 0.173648), vec3(-0.984808, 0.000000, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.984808, 0.173648, 0.000000), vec3(-0.984808, 0.173648, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-1.000000, 0.000000, 0.000000), vec3(-1.000000, 0.000000, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.969846, 0.171010, -0.173648), vec3(-0.969846, 0.171010, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.984808, 0.000000, -0.173648), vec3(-0.984808, 0.000000, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.925417, 0.163176, -0.342020), vec3(-0.925417, 0.163176, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.939693, 0.000000, -0.342020), vec3(-0.939693, 0.000000, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.852869, 0.150384, -0.500000), vec3(-0.852869, 0.150384, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.866025, 0.000000, -0.500000), vec3(-0.866025, 0.000000, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.754407, 0.133022, -0.642788), vec3(-0.754407, 0.133022, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.766044, 0.000000, -0.642788), vec3(-0.766044, 0.000000, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.633022, 0.111619, -0.766044), vec3(-0.633022, 0.111619, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.642788, 0.000000, -0.766044), vec3(-0.642788, 0.000000, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, 0.086824, -0.866025), vec3(-0.492404, 0.086824, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.500000, 0.000000, -0.866025), vec3(-0.500000, 0.000000, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.336824, 0.059391, -0.939693), vec3(-0.336824, 0.059391, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.342020, 0.000000, -0.939693), vec3(-0.342020, 0.000000, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, 0.030154, -0.984808), vec3(-0.171010, 0.030154, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.173648, 0.000000, -0.984808), vec3(-0.173648, 0.000000, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.173648, 0.000000, -0.984808), vec3(-0.173648, 0.000000, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, -0.030154, -0.984808), vec3(-0.171010, -0.030154, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.342020, 0.000000, -0.939693), vec3(-0.342020, 0.000000, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.336824, -0.059391, -0.939693), vec3(-0.336824, -0.059391, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.500000, 0.000000, -0.866025), vec3(-0.500000, 0.000000, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, -0.086824, -0.866025), vec3(-0.492404, -0.086824, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.642788, 0.000000, -0.766044), vec3(-0.642788, 0.000000, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.633022, -0.111619, -0.766044), vec3(-0.633022, -0.111619, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.766044, 0.000000, -0.642788), vec3(-0.766044, 0.000000, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.754407, -0.133022, -0.642788), vec3(-0.754407, -0.133022, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.866025, 0.000000, -0.500000), vec3(-0.866025, 0.000000, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.852869, -0.150384, -0.500000), vec3(-0.852869, -0.150384, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.939693, 0.000000, -0.342020), vec3(-0.939693, 0.000000, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.925417, -0.163176, -0.342020), vec3(-0.925417, -0.163176, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.984808, 0.000000, -0.173648), vec3(-0.984808, 0.000000, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.969846, -0.171010, -0.173648), vec3(-0.969846, -0.171010, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-1.000000, 0.000000, 0.000000), vec3(-1.000000, 0.000000, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.984808, -0.173648, 0.000000), vec3(-0.984808, -0.173648, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.984808, 0.000000, 0.173648), vec3(-0.984808, 0.000000, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.969846, -0.171010, 0.173648), vec3(-0.969846, -0.171010, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.939693, 0.000000, 0.342020), vec3(-0.939693, 0.000000, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.925417, -0.163176, 0.342020), vec3(-0.925417, -0.163176, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.866025, 0.000000, 0.500000), vec3(-0.866025, 0.000000, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.852869, -0.150384, 0.500000), vec3(-0.852869, -0.150384, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.766044, 0.000000, 0.642788), vec3(-0.766044, 0.000000, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.754407, -0.133022, 0.642788), vec3(-0.754407, -0.133022, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.642788, 0.000000, 0.766044), vec3(-0.642788, 0.000000, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.633022, -0.111619, 0.766044), vec3(-0.633022, -0.111619, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.500000, 0.000000, 0.866025), vec3(-0.500000, 0.000000, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, -0.086824, 0.866025), vec3(-0.492404, -0.086824, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.342020, 0.000000, 0.939693), vec3(-0.342020, 0.000000, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.336824, -0.059391, 0.939693), vec3(-0.336824, -0.059391, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.173648, 0.000000, 0.984808), vec3(-0.173648, 0.000000, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, -0.030154, 0.984808), vec3(-0.171010, -0.030154, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, -0.030154, 0.984808), vec3(-0.171010, -0.030154, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.163176, -0.059391, 0.984808), vec3(-0.163176, -0.059391, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.336824, -0.059391, 0.939693), vec3(-0.336824, -0.059391, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, -0.116978, 0.939693), vec3(-0.321394, -0.116978, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, -0.086824, 0.866025), vec3(-0.492404, -0.086824, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.469846, -0.171010, 0.866025), vec3(-0.469846, -0.171010, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.633022, -0.111619, 0.766044), vec3(-0.633022, -0.111619, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.604023, -0.219846, 0.766044), vec3(-0.604023, -0.219846, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.754407, -0.133022, 0.642788), vec3(-0.754407, -0.133022, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.719846, -0.262003, 0.642788), vec3(-0.719846, -0.262003, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.852869, -0.150384, 0.500000), vec3(-0.852869, -0.150384, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.813798, -0.296198, 0.500000), vec3(-0.813798, -0.296198, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.925417, -0.163176, 0.342020), vec3(-0.925417, -0.163176, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.883022, -0.321394, 0.342020), vec3(-0.883022, -0.321394, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.969846, -0.171010, 0.173648), vec3(-0.969846, -0.171010, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.925417, -0.336824, 0.173648), vec3(-0.925417, -0.336824, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.984808, -0.173648, 0.000000), vec3(-0.984808, -0.173648, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.939693, -0.342020, 0.000000), vec3(-0.939693, -0.342020, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.969846, -0.171010, -0.173648), vec3(-0.969846, -0.171010, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.925417, -0.336824, -0.173648), vec3(-0.925417, -0.336824, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.925417, -0.163176, -0.342020), vec3(-0.925417, -0.163176, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.883022, -0.321394, -0.342020), vec3(-0.883022, -0.321394, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.852869, -0.150384, -0.500000), vec3(-0.852869, -0.150384, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.813798, -0.296198, -0.500000), vec3(-0.813798, -0.296198, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.754407, -0.133022, -0.642788), vec3(-0.754407, -0.133022, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.719846, -0.262003, -0.642788), vec3(-0.719846, -0.262003, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.633022, -0.111619, -0.766044), vec3(-0.633022, -0.111619, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.604023, -0.219846, -0.766044), vec3(-0.604023, -0.219846, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, -0.086824, -0.866025), vec3(-0.492404, -0.086824, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.469846, -0.171010, -0.866025), vec3(-0.469846, -0.171010, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.336824, -0.059391, -0.939693), vec3(-0.336824, -0.059391, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, -0.116978, -0.939693), vec3(-0.321394, -0.116978, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, -0.030154, -0.984808), vec3(-0.171010, -0.030154, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.163176, -0.059391, -0.984808), vec3(-0.163176, -0.059391, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.163176, -0.059391, -0.984808), vec3(-0.163176, -0.059391, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.150384, -0.086824, -0.984808), vec3(-0.150384, -0.086824, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, -0.116978, -0.939693), vec3(-0.321394, -0.116978, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.296198, -0.171010, -0.939693), vec3(-0.296198, -0.171010, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.469846, -0.171010, -0.866025), vec3(-0.469846, -0.171010, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.433013, -0.250000, -0.866025), vec3(-0.433013, -0.250000, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.604023, -0.219846, -0.766044), vec3(-0.604023, -0.219846, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.556670, -0.321394, -0.766044), vec3(-0.556670, -0.321394, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.719846, -0.262003, -0.642788), vec3(-0.719846, -0.262003, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.663414, -0.383022, -0.642788), vec3(-0.663414, -0.383022, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.813798, -0.296198, -0.500000), vec3(-0.813798, -0.296198, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.750000, -0.433013, -0.500000), vec3(-0.750000, -0.433013, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.883022, -0.321394, -0.342020), vec3(-0.883022, -0.321394, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.813798, -0.469846, -0.342020), vec3(-0.813798, -0.469846, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.925417, -0.336824, -0.173648), vec3(-0.925417, -0.336824, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.852869, -0.492404, -0.173648), vec3(-0.852869, -0.492404, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.939693, -0.342020, 0.000000), vec3(-0.939693, -0.342020, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.866025, -0.500000, 0.000000), vec3(-0.866025, -0.500000, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.925417, -0.336824, 0.173648), vec3(-0.925417, -0.336824, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.852869, -0.492404, 0.173648), vec3(-0.852869, -0.492404, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.883022, -0.321394, 0.342020), vec3(-0.883022, -0.321394, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.813798, -0.469846, 0.342020), vec3(-0.813798, -0.469846, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.813798, -0.296198, 0.500000), vec3(-0.813798, -0.296198, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.750000, -0.433013, 0.500000), vec3(-0.750000, -0.433013, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.719846, -0.262003, 0.642788), vec3(-0.719846, -0.262003, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.663414, -0.383022, 0.642788), vec3(-0.663414, -0.383022, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.604023, -0.219846, 0.766044), vec3(-0.604023, -0.219846, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.556670, -0.321394, 0.766044), vec3(-0.556670, -0.321394, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.469846, -0.171010, 0.866025), vec3(-0.469846, -0.171010, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.433013, -0.250000, 0.866025), vec3(-0.433013, -0.250000, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, -0.116978, 0.939693), vec3(-0.321394, -0.116978, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.296198, -0.171010, 0.939693), vec3(-0.296198, -0.171010, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.163176, -0.059391, 0.984808), vec3(-0.163176, -0.059391, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.150384, -0.086824, 0.984808), vec3(-0.150384, -0.086824, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.150384, -0.086824, 0.984808), vec3(-0.150384, -0.086824, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.133022, -0.111619, 0.984808), vec3(-0.133022, -0.111619, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.296198, -0.171010, 0.939693), vec3(-0.296198, -0.171010, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.262003, -0.219846, 0.939693), vec3(-0.262003, -0.219846, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.433013, -0.250000, 0.866025), vec3(-0.433013, -0.250000, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.383022, -0.321394, 0.866025), vec3(-0.383022, -0.321394, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.556670, -0.321394, 0.766044), vec3(-0.556670, -0.321394, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, -0.413176, 0.766044), vec3(-0.492404, -0.413176, 0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.663414, -0.383022, 0.642788), vec3(-0.663414, -0.383022, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.586824, -0.492404, 0.642788), vec3(-0.586824, -0.492404, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.750000, -0.433013, 0.500000), vec3(-0.750000, -0.433013, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.663414, -0.556670, 0.500000), vec3(-0.663414, -0.556670, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.813798, -0.469846, 0.342020), vec3(-0.813798, -0.469846, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.719846, -0.604023, 0.342020), vec3(-0.719846, -0.604023, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.852869, -0.492404, 0.173648), vec3(-0.852869, -0.492404, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.754407, -0.633022, 0.173648), vec3(-0.754407, -0.633022, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.866025, -0.500000, 0.000000), vec3(-0.866025, -0.500000, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.766044, -0.642788, 0.000000), vec3(-0.766044, -0.642788, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.852869, -0.492404, -0.173648), vec3(-0.852869, -0.492404, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.754407, -0.633022, -0.173648), vec3(-0.754407, -0.633022, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.813798, -0.469846, -0.342020), vec3(-0.813798, -0.469846, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.719846, -0.604023, -0.342020), vec3(-0.719846, -0.604023, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.750000, -0.433013, -0.500000), vec3(-0.750000, -0.433013, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.663414, -0.556670, -0.500000), vec3(-0.663414, -0.556670, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.663414, -0.383022, -0.642788), vec3(-0.663414, -0.383022, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.586824, -0.492404, -0.642788), vec3(-0.586824, -0.492404, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.556670, -0.321394, -0.766044), vec3(-0.556670, -0.321394, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, -0.413176, -0.766044), vec3(-0.492404, -0.413176, -0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.433013, -0.250000, -0.866025), vec3(-0.433013, -0.250000, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.383022, -0.321394, -0.866025), vec3(-0.383022, -0.321394, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.296198, -0.171010, -0.939693), vec3(-0.296198, -0.171010, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.262003, -0.219846, -0.939693), vec3(-0.262003, -0.219846, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.150384, -0.086824, -0.984808), vec3(-0.150384, -0.086824, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.133022, -0.111619, -0.984808), vec3(-0.133022, -0.111619, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.133022, -0.111619, -0.984808), vec3(-0.133022, -0.111619, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.111619, -0.133022, -0.984808), vec3(-0.111619, -0.133022, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.262003, -0.219846, -0.939693), vec3(-0.262003, -0.219846, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.219846, -0.262003, -0.939693), vec3(-0.219846, -0.262003, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.383022, -0.321394, -0.866025), vec3(-0.383022, -0.321394, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, -0.383022, -0.866025), vec3(-0.321394, -0.383022, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, -0.413176, -0.766044), vec3(-0.492404, -0.413176, -0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.413176, -0.492404, -0.766044), vec3(-0.413176, -0.492404, -0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.586824, -0.492404, -0.642788), vec3(-0.586824, -0.492404, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, -0.586824, -0.642788), vec3(-0.492404, -0.586824, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.663414, -0.556670, -0.500000), vec3(-0.663414, -0.556670, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.556670, -0.663414, -0.500000), vec3(-0.556670, -0.663414, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.719846, -0.604023, -0.342020), vec3(-0.719846, -0.604023, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.604023, -0.719846, -0.342020), vec3(-0.604023, -0.719846, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.754407, -0.633022, -0.173648), vec3(-0.754407, -0.633022, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.633022, -0.754407, -0.173648), vec3(-0.633022, -0.754407, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.766044, -0.642788, 0.000000), vec3(-0.766044, -0.642788, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.642788, -0.766044, 0.000000), vec3(-0.642788, -0.766044, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.754407, -0.633022, 0.173648), vec3(-0.754407, -0.633022, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.633022, -0.754407, 0.173648), vec3(-0.633022, -0.754407, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.719846, -0.604023, 0.342020), vec3(-0.719846, -0.604023, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.604023, -0.719846, 0.342020), vec3(-0.604023, -0.719846, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.663414, -0.556670, 0.500000), vec3(-0.663414, -0.556670, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.556670, -0.663414, 0.500000), vec3(-0.556670, -0.663414, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.586824, -0.492404, 0.642788), vec3(-0.586824, -0.492404, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, -0.586824, 0.642788), vec3(-0.492404, -0.586824, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, -0.413176, 0.766044), vec3(-0.492404, -0.413176, 0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.413176, -0.492404, 0.766044), vec3(-0.413176, -0.492404, 0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.383022, -0.321394, 0.866025), vec3(-0.383022, -0.321394, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, -0.383022, 0.866025), vec3(-0.321394, -0.383022, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.262003, -0.219846, 0.939693), vec3(-0.262003, -0.219846, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.219846, -0.262003, 0.939693), vec3(-0.219846, -0.262003, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.133022, -0.111619, 0.984808), vec3(-0.133022, -0.111619, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.111619, -0.133022, 0.984808), vec3(-0.111619, -0.133022, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.111619, -0.133022, 0.984808), vec3(-0.111619, -0.133022, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.086824, -0.150384, 0.984808), vec3(-0.086824, -0.150384, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.219846, -0.262003, 0.939693), vec3(-0.219846, -0.262003, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, -0.296198, 0.939693), vec3(-0.171010, -0.296198, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, -0.383022, 0.866025), vec3(-0.321394, -0.383022, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.250000, -0.433013, 0.866025), vec3(-0.250000, -0.433013, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.413176, -0.492404, 0.766044), vec3(-0.413176, -0.492404, 0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, -0.556670, 0.766044), vec3(-0.321394, -0.556670, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, -0.586824, 0.642788), vec3(-0.492404, -0.586824, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.383022, -0.663414, 0.642788), vec3(-0.383022, -0.663414, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.556670, -0.663414, 0.500000), vec3(-0.556670, -0.663414, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.433013, -0.750000, 0.500000), vec3(-0.433013, -0.750000, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.604023, -0.719846, 0.342020), vec3(-0.604023, -0.719846, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.469846, -0.813798, 0.342020), vec3(-0.469846, -0.813798, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.633022, -0.754407, 0.173648), vec3(-0.633022, -0.754407, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, -0.852869, 0.173648), vec3(-0.492404, -0.852869, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.642788, -0.766044, 0.000000), vec3(-0.642788, -0.766044, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.500000, -0.866025, 0.000000), vec3(-0.500000, -0.866025, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.633022, -0.754407, -0.173648), vec3(-0.633022, -0.754407, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, -0.852869, -0.173648), vec3(-0.492404, -0.852869, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.604023, -0.719846, -0.342020), vec3(-0.604023, -0.719846, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.469846, -0.813798, -0.342020), vec3(-0.469846, -0.813798, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.556670, -0.663414, -0.500000), vec3(-0.556670, -0.663414, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.433013, -0.750000, -0.500000), vec3(-0.433013, -0.750000, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, -0.586824, -0.642788), vec3(-0.492404, -0.586824, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.383022, -0.663414, -0.642788), vec3(-0.383022, -0.663414, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.413176, -0.492404, -0.766044), vec3(-0.413176, -0.492404, -0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, -0.556670, -0.766044), vec3(-0.321394, -0.556670, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, -0.383022, -0.866025), vec3(-0.321394, -0.383022, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.250000, -0.433013, -0.866025), vec3(-0.250000, -0.433013, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.219846, -0.262003, -0.939693), vec3(-0.219846, -0.262003, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, -0.296198, -0.939693), vec3(-0.171010, -0.296198, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.111619, -0.133022, -0.984808), vec3(-0.111619, -0.133022, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.086824, -0.150384, -0.984808), vec3(-0.086824, -0.150384, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.086824, -0.150384, -0.984808), vec3(-0.086824, -0.150384, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.059391, -0.163176, -0.984808), vec3(-0.059391, -0.163176, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, -0.296198, -0.939693), vec3(-0.171010, -0.296198, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.116978, -0.321394, -0.939693), vec3(-0.116978, -0.321394, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.250000, -0.433013, -0.866025), vec3(-0.250000, -0.433013, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, -0.469846, -0.866025), vec3(-0.171010, -0.469846, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, -0.556670, -0.766044), vec3(-0.321394, -0.556670, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.219846, -0.604023, -0.766044), vec3(-0.219846, -0.604023, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.383022, -0.663414, -0.642788), vec3(-0.383022, -0.663414, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.262003, -0.719846, -0.642788), vec3(-0.262003, -0.719846, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.433013, -0.750000, -0.500000), vec3(-0.433013, -0.750000, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.296198, -0.813798, -0.500000), vec3(-0.296198, -0.813798, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.469846, -0.813798, -0.342020), vec3(-0.469846, -0.813798, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, -0.883022, -0.342020), vec3(-0.321394, -0.883022, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, -0.852869, -0.173648), vec3(-0.492404, -0.852869, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.336824, -0.925417, -0.173648), vec3(-0.336824, -0.925417, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.500000, -0.866025, 0.000000), vec3(-0.500000, -0.866025, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.342020, -0.939693, 0.000000), vec3(-0.342020, -0.939693, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.492404, -0.852869, 0.173648), vec3(-0.492404, -0.852869, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.336824, -0.925417, 0.173648), vec3(-0.336824, -0.925417, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.469846, -0.813798, 0.342020), vec3(-0.469846, -0.813798, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, -0.883022, 0.342020), vec3(-0.321394, -0.883022, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.433013, -0.750000, 0.500000), vec3(-0.433013, -0.750000, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.296198, -0.813798, 0.500000), vec3(-0.296198, -0.813798, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.383022, -0.663414, 0.642788), vec3(-0.383022, -0.663414, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.262003, -0.719846, 0.642788), vec3(-0.262003, -0.719846, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, -0.556670, 0.766044), vec3(-0.321394, -0.556670, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.219846, -0.604023, 0.766044), vec3(-0.219846, -0.604023, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.250000, -0.433013, 0.866025), vec3(-0.250000, -0.433013, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, -0.469846, 0.866025), vec3(-0.171010, -0.469846, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, -0.296198, 0.939693), vec3(-0.171010, -0.296198, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.116978, -0.321394, 0.939693), vec3(-0.116978, -0.321394, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.086824, -0.150384, 0.984808), vec3(-0.086824, -0.150384, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.059391, -0.163176, 0.984808), vec3(-0.059391, -0.163176, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.059391, -0.163176, 0.984808), vec3(-0.059391, -0.163176, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.030154, -0.171010, 0.984808), vec3(-0.030154, -0.171010, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.116978, -0.321394, 0.939693), vec3(-0.116978, -0.321394, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.059391, -0.336824, 0.939693), vec3(-0.059391, -0.336824, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, -0.469846, 0.866025), vec3(-0.171010, -0.469846, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.086824, -0.492404, 0.866025), vec3(-0.086824, -0.492404, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.219846, -0.604023, 0.766044), vec3(-0.219846, -0.604023, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.111619, -0.633022, 0.766044), vec3(-0.111619, -0.633022, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.262003, -0.719846, 0.642788), vec3(-0.262003, -0.719846, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.133022, -0.754407, 0.642788), vec3(-0.133022, -0.754407, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.296198, -0.813798, 0.500000), vec3(-0.296198, -0.813798, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.150384, -0.852869, 0.500000), vec3(-0.150384, -0.852869, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, -0.883022, 0.342020), vec3(-0.321394, -0.883022, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.163176, -0.925417, 0.342020), vec3(-0.163176, -0.925417, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.336824, -0.925417, 0.173648), vec3(-0.336824, -0.925417, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, -0.969846, 0.173648), vec3(-0.171010, -0.969846, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.342020, -0.939693, 0.000000), vec3(-0.342020, -0.939693, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.173648, -0.984808, 0.000000), vec3(-0.173648, -0.984808, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.336824, -0.925417, -0.173648), vec3(-0.336824, -0.925417, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, -0.969846, -0.173648), vec3(-0.171010, -0.969846, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.321394, -0.883022, -0.342020), vec3(-0.321394, -0.883022, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.163176, -0.925417, -0.342020), vec3(-0.163176, -0.925417, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.296198, -0.813798, -0.500000), vec3(-0.296198, -0.813798, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.150384, -0.852869, -0.500000), vec3(-0.150384, -0.852869, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.262003, -0.719846, -0.642788), vec3(-0.262003, -0.719846, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.133022, -0.754407, -0.642788), vec3(-0.133022, -0.754407, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.219846, -0.604023, -0.766044), vec3(-0.219846, -0.604023, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.111619, -0.633022, -0.766044), vec3(-0.111619, -0.633022, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, -0.469846, -0.866025), vec3(-0.171010, -0.469846, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.086824, -0.492404, -0.866025), vec3(-0.086824, -0.492404, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.116978, -0.321394, -0.939693), vec3(-0.116978, -0.321394, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.059391, -0.336824, -0.939693), vec3(-0.059391, -0.336824, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.059391, -0.163176, -0.984808), vec3(-0.059391, -0.163176, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.030154, -0.171010, -0.984808), vec3(-0.030154, -0.171010, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.030154, -0.171010, -0.984808), vec3(-0.030154, -0.171010, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.173648, -0.984808), vec3(-0.000000, -0.173648, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.059391, -0.336824, -0.939693), vec3(-0.059391, -0.336824, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.342020, -0.939693), vec3(-0.000000, -0.342020, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.086824, -0.492404, -0.866025), vec3(-0.086824, -0.492404, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.500000, -0.866025), vec3(-0.000000, -0.500000, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.111619, -0.633022, -0.766044), vec3(-0.111619, -0.633022, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.642788, -0.766044), vec3(-0.000000, -0.642788, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.133022, -0.754407, -0.642788), vec3(-0.133022, -0.754407, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.766044, -0.642788), vec3(-0.000000, -0.766044, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.150384, -0.852869, -0.500000), vec3(-0.150384, -0.852869, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.866025, -0.500000), vec3(-0.000000, -0.866025, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.163176, -0.925417, -0.342020), vec3(-0.163176, -0.925417, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.939693, -0.342020), vec3(-0.000000, -0.939693, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, -0.969846, -0.173648), vec3(-0.171010, -0.969846, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.984808, -0.173648), vec3(-0.000000, -0.984808, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.173648, -0.984808, 0.000000), vec3(-0.173648, -0.984808, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -1.000000, 0.000000), vec3(-0.000000, -1.000000, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.171010, -0.969846, 0.173648), vec3(-0.171010, -0.969846, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.984808, 0.173648), vec3(-0.000000, -0.984808, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.163176, -0.925417, 0.342020), vec3(-0.163176, -0.925417, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.939693, 0.342020), vec3(-0.000000, -0.939693, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.150384, -0.852869, 0.500000), vec3(-0.150384, -0.852869, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.866025, 0.500000), vec3(-0.000000, -0.866025, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.133022, -0.754407, 0.642788), vec3(-0.133022, -0.754407, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.766044, 0.642788), vec3(-0.000000, -0.766044, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.111619, -0.633022, 0.766044), vec3(-0.111619, -0.633022, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.642788, 0.766044), vec3(-0.000000, -0.642788, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.086824, -0.492404, 0.866025), vec3(-0.086824, -0.492404, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.500000, 0.866025), vec3(-0.000000, -0.500000, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.059391, -0.336824, 0.939693), vec3(-0.059391, -0.336824, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.342020, 0.939693), vec3(-0.000000, -0.342020, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.030154, -0.171010, 0.984808), vec3(-0.030154, -0.171010, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.173648, 0.984808), vec3(-0.000000, -0.173648, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.173648, 0.984808), vec3(-0.000000, -0.173648, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.030154, -0.171010, 0.984808), vec3(0.030154, -0.171010, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.342020, 0.939693), vec3(-0.000000, -0.342020, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.059391, -0.336824, 0.939693), vec3(0.059391, -0.336824, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.500000, 0.866025), vec3(-0.000000, -0.500000, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.086824, -0.492404, 0.866025), vec3(0.086824, -0.492404, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.642788, 0.766044), vec3(-0.000000, -0.642788, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.111619, -0.633022, 0.766044), vec3(0.111619, -0.633022, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.766044, 0.642788), vec3(-0.000000, -0.766044, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.133022, -0.754407, 0.642788), vec3(0.133022, -0.754407, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.866025, 0.500000), vec3(-0.000000, -0.866025, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.150384, -0.852869, 0.500000), vec3(0.150384, -0.852869, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.939693, 0.342020), vec3(-0.000000, -0.939693, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.163176, -0.925417, 0.342020), vec3(0.163176, -0.925417, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.984808, 0.173648), vec3(-0.000000, -0.984808, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, -0.969846, 0.173648), vec3(0.171010, -0.969846, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -1.000000, 0.000000), vec3(-0.000000, -1.000000, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.173648, -0.984808, 0.000000), vec3(0.173648, -0.984808, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.984808, -0.173648), vec3(-0.000000, -0.984808, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, -0.969846, -0.173648), vec3(0.171010, -0.969846, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.939693, -0.342020), vec3(-0.000000, -0.939693, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.163176, -0.925417, -0.342020), vec3(0.163176, -0.925417, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.866025, -0.500000), vec3(-0.000000, -0.866025, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.150384, -0.852869, -0.500000), vec3(0.150384, -0.852869, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.766044, -0.642788), vec3(-0.000000, -0.766044, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.133022, -0.754407, -0.642788), vec3(0.133022, -0.754407, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.642788, -0.766044), vec3(-0.000000, -0.642788, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.111619, -0.633022, -0.766044), vec3(0.111619, -0.633022, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.500000, -0.866025), vec3(-0.000000, -0.500000, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.086824, -0.492404, -0.866025), vec3(0.086824, -0.492404, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.342020, -0.939693), vec3(-0.000000, -0.342020, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.059391, -0.336824, -0.939693), vec3(0.059391, -0.336824, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(-0.000000, -0.173648, -0.984808), vec3(-0.000000, -0.173648, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.030154, -0.171010, -0.984808), vec3(0.030154, -0.171010, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.030154, -0.171010, -0.984808), vec3(0.030154, -0.171010, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.059391, -0.163176, -0.984808), vec3(0.059391, -0.163176, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.059391, -0.336824, -0.939693), vec3(0.059391, -0.336824, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.116978, -0.321394, -0.939693), vec3(0.116978, -0.321394, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.086824, -0.492404, -0.866025), vec3(0.086824, -0.492404, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, -0.469846, -0.866025), vec3(0.171010, -0.469846, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.111619, -0.633022, -0.766044), vec3(0.111619, -0.633022, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.219846, -0.604023, -0.766044), vec3(0.219846, -0.604023, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.133022, -0.754407, -0.642788), vec3(0.133022, -0.754407, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.262003, -0.719846, -0.642788), vec3(0.262003, -0.719846, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.150384, -0.852869, -0.500000), vec3(0.150384, -0.852869, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.296198, -0.813798, -0.500000), vec3(0.296198, -0.813798, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.163176, -0.925417, -0.342020), vec3(0.163176, -0.925417, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, -0.883022, -0.342020), vec3(0.321394, -0.883022, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, -0.969846, -0.173648), vec3(0.171010, -0.969846, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.336824, -0.925417, -0.173648), vec3(0.336824, -0.925417, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.173648, -0.984808, 0.000000), vec3(0.173648, -0.984808, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.342020, -0.939693, 0.000000), vec3(0.342020, -0.939693, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, -0.969846, 0.173648), vec3(0.171010, -0.969846, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.336824, -0.925417, 0.173648), vec3(0.336824, -0.925417, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.163176, -0.925417, 0.342020), vec3(0.163176, -0.925417, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, -0.883022, 0.342020), vec3(0.321394, -0.883022, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.150384, -0.852869, 0.500000), vec3(0.150384, -0.852869, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.296198, -0.813798, 0.500000), vec3(0.296198, -0.813798, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.133022, -0.754407, 0.642788), vec3(0.133022, -0.754407, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.262003, -0.719846, 0.642788), vec3(0.262003, -0.719846, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.111619, -0.633022, 0.766044), vec3(0.111619, -0.633022, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.219846, -0.604023, 0.766044), vec3(0.219846, -0.604023, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.086824, -0.492404, 0.866025), vec3(0.086824, -0.492404, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, -0.469846, 0.866025), vec3(0.171010, -0.469846, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.059391, -0.336824, 0.939693), vec3(0.059391, -0.336824, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.116978, -0.321394, 0.939693), vec3(0.116978, -0.321394, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.030154, -0.171010, 0.984808), vec3(0.030154, -0.171010, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.059391, -0.163176, 0.984808), vec3(0.059391, -0.163176, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.059391, -0.163176, 0.984808), vec3(0.059391, -0.163176, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.086824, -0.150384, 0.984808), vec3(0.086824, -0.150384, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.116978, -0.321394, 0.939693), vec3(0.116978, -0.321394, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, -0.296198, 0.939693), vec3(0.171010, -0.296198, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, -0.469846, 0.866025), vec3(0.171010, -0.469846, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.250000, -0.433013, 0.866025), vec3(0.250000, -0.433013, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.219846, -0.604023, 0.766044), vec3(0.219846, -0.604023, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, -0.556670, 0.766044), vec3(0.321394, -0.556670, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.262003, -0.719846, 0.642788), vec3(0.262003, -0.719846, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.383022, -0.663414, 0.642788), vec3(0.383022, -0.663414, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.296198, -0.813798, 0.500000), vec3(0.296198, -0.813798, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.433013, -0.750000, 0.500000), vec3(0.433013, -0.750000, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, -0.883022, 0.342020), vec3(0.321394, -0.883022, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.469846, -0.813798, 0.342020), vec3(0.469846, -0.813798, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.336824, -0.925417, 0.173648), vec3(0.336824, -0.925417, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, -0.852869, 0.173648), vec3(0.492404, -0.852869, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.342020, -0.939693, 0.000000), vec3(0.342020, -0.939693, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.500000, -0.866025, 0.000000), vec3(0.500000, -0.866025, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.336824, -0.925417, -0.173648), vec3(0.336824, -0.925417, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, -0.852869, -0.173648), vec3(0.492404, -0.852869, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, -0.883022, -0.342020), vec3(0.321394, -0.883022, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.469846, -0.813798, -0.342020), vec3(0.469846, -0.813798, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.296198, -0.813798, -0.500000), vec3(0.296198, -0.813798, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.433013, -0.750000, -0.500000), vec3(0.433013, -0.750000, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.262003, -0.719846, -0.642788), vec3(0.262003, -0.719846, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.383022, -0.663414, -0.642788), vec3(0.383022, -0.663414, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.219846, -0.604023, -0.766044), vec3(0.219846, -0.604023, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, -0.556670, -0.766044), vec3(0.321394, -0.556670, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, -0.469846, -0.866025), vec3(0.171010, -0.469846, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.250000, -0.433013, -0.866025), vec3(0.250000, -0.433013, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.116978, -0.321394, -0.939693), vec3(0.116978, -0.321394, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, -0.296198, -0.939693), vec3(0.171010, -0.296198, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.059391, -0.163176, -0.984808), vec3(0.059391, -0.163176, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.086824, -0.150384, -0.984808), vec3(0.086824, -0.150384, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.086824, -0.150384, -0.984808), vec3(0.086824, -0.150384, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.111619, -0.133022, -0.984808), vec3(0.111619, -0.133022, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, -0.296198, -0.939693), vec3(0.171010, -0.296198, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.219846, -0.262003, -0.939693), vec3(0.219846, -0.262003, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.250000, -0.433013, -0.866025), vec3(0.250000, -0.433013, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, -0.383022, -0.866025), vec3(0.321394, -0.383022, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, -0.556670, -0.766044), vec3(0.321394, -0.556670, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.413176, -0.492404, -0.766044), vec3(0.413176, -0.492404, -0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.383022, -0.663414, -0.642788), vec3(0.383022, -0.663414, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, -0.586824, -0.642788), vec3(0.492404, -0.586824, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.433013, -0.750000, -0.500000), vec3(0.433013, -0.750000, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.556670, -0.663414, -0.500000), vec3(0.556670, -0.663414, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.469846, -0.813798, -0.342020), vec3(0.469846, -0.813798, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.604023, -0.719846, -0.342020), vec3(0.604023, -0.719846, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, -0.852869, -0.173648), vec3(0.492404, -0.852869, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.633022, -0.754407, -0.173648), vec3(0.633022, -0.754407, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.500000, -0.866025, 0.000000), vec3(0.500000, -0.866025, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.642788, -0.766044, 0.000000), vec3(0.642788, -0.766044, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, -0.852869, 0.173648), vec3(0.492404, -0.852869, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.633022, -0.754407, 0.173648), vec3(0.633022, -0.754407, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.469846, -0.813798, 0.342020), vec3(0.469846, -0.813798, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.604023, -0.719846, 0.342020), vec3(0.604023, -0.719846, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.433013, -0.750000, 0.500000), vec3(0.433013, -0.750000, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.556670, -0.663414, 0.500000), vec3(0.556670, -0.663414, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.383022, -0.663414, 0.642788), vec3(0.383022, -0.663414, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, -0.586824, 0.642788), vec3(0.492404, -0.586824, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, -0.556670, 0.766044), vec3(0.321394, -0.556670, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.413176, -0.492404, 0.766044), vec3(0.413176, -0.492404, 0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.250000, -0.433013, 0.866025), vec3(0.250000, -0.433013, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, -0.383022, 0.866025), vec3(0.321394, -0.383022, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, -0.296198, 0.939693), vec3(0.171010, -0.296198, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.219846, -0.262003, 0.939693), vec3(0.219846, -0.262003, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.086824, -0.150384, 0.984808), vec3(0.086824, -0.150384, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.111619, -0.133022, 0.984808), vec3(0.111619, -0.133022, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.111619, -0.133022, 0.984808), vec3(0.111619, -0.133022, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.133022, -0.111619, 0.984808), vec3(0.133022, -0.111619, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.219846, -0.262003, 0.939693), vec3(0.219846, -0.262003, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.262003, -0.219846, 0.939693), vec3(0.262003, -0.219846, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, -0.383022, 0.866025), vec3(0.321394, -0.383022, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.383022, -0.321394, 0.866025), vec3(0.383022, -0.321394, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.413176, -0.492404, 0.766044), vec3(0.413176, -0.492404, 0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, -0.413176, 0.766044), vec3(0.492404, -0.413176, 0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, -0.586824, 0.642788), vec3(0.492404, -0.586824, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.586824, -0.492404, 0.642788), vec3(0.586824, -0.492404, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.556670, -0.663414, 0.500000), vec3(0.556670, -0.663414, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.663414, -0.556670, 0.500000), vec3(0.663414, -0.556670, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.604023, -0.719846, 0.342020), vec3(0.604023, -0.719846, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.719846, -0.604023, 0.342020), vec3(0.719846, -0.604023, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.633022, -0.754407, 0.173648), vec3(0.633022, -0.754407, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.754407, -0.633022, 0.173648), vec3(0.754407, -0.633022, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.642788, -0.766044, 0.000000), vec3(0.642788, -0.766044, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.766044, -0.642788, 0.000000), vec3(0.766044, -0.642788, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.633022, -0.754407, -0.173648), vec3(0.633022, -0.754407, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.754407, -0.633022, -0.173648), vec3(0.754407, -0.633022, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.604023, -0.719846, -0.342020), vec3(0.604023, -0.719846, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.719846, -0.604023, -0.342020), vec3(0.719846, -0.604023, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.556670, -0.663414, -0.500000), vec3(0.556670, -0.663414, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.663414, -0.556670, -0.500000), vec3(0.663414, -0.556670, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, -0.586824, -0.642788), vec3(0.492404, -0.586824, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.586824, -0.492404, -0.642788), vec3(0.586824, -0.492404, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.413176, -0.492404, -0.766044), vec3(0.413176, -0.492404, -0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, -0.413176, -0.766044), vec3(0.492404, -0.413176, -0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, -0.383022, -0.866025), vec3(0.321394, -0.383022, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.383022, -0.321394, -0.866025), vec3(0.383022, -0.321394, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.219846, -0.262003, -0.939693), vec3(0.219846, -0.262003, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.262003, -0.219846, -0.939693), vec3(0.262003, -0.219846, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.111619, -0.133022, -0.984808), vec3(0.111619, -0.133022, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.133022, -0.111619, -0.984808), vec3(0.133022, -0.111619, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.133022, -0.111619, -0.984808), vec3(0.133022, -0.111619, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.150384, -0.086824, -0.984808), vec3(0.150384, -0.086824, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.262003, -0.219846, -0.939693), vec3(0.262003, -0.219846, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.296198, -0.171010, -0.939693), vec3(0.296198, -0.171010, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.383022, -0.321394, -0.866025), vec3(0.383022, -0.321394, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.433013, -0.250000, -0.866025), vec3(0.433013, -0.250000, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, -0.413176, -0.766044), vec3(0.492404, -0.413176, -0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.556670, -0.321394, -0.766044), vec3(0.556670, -0.321394, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.586824, -0.492404, -0.642788), vec3(0.586824, -0.492404, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.663414, -0.383022, -0.642788), vec3(0.663414, -0.383022, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.663414, -0.556670, -0.500000), vec3(0.663414, -0.556670, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.750000, -0.433013, -0.500000), vec3(0.750000, -0.433013, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.719846, -0.604023, -0.342020), vec3(0.719846, -0.604023, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.813798, -0.469846, -0.342020), vec3(0.813798, -0.469846, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.754407, -0.633022, -0.173648), vec3(0.754407, -0.633022, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.852869, -0.492404, -0.173648), vec3(0.852869, -0.492404, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.766044, -0.642788, 0.000000), vec3(0.766044, -0.642788, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.866025, -0.500000, 0.000000), vec3(0.866025, -0.500000, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.754407, -0.633022, 0.173648), vec3(0.754407, -0.633022, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.852869, -0.492404, 0.173648), vec3(0.852869, -0.492404, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.719846, -0.604023, 0.342020), vec3(0.719846, -0.604023, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.813798, -0.469846, 0.342020), vec3(0.813798, -0.469846, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.663414, -0.556670, 0.500000), vec3(0.663414, -0.556670, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.750000, -0.433013, 0.500000), vec3(0.750000, -0.433013, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.586824, -0.492404, 0.642788), vec3(0.586824, -0.492404, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.663414, -0.383022, 0.642788), vec3(0.663414, -0.383022, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, -0.413176, 0.766044), vec3(0.492404, -0.413176, 0.766045), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.556670, -0.321394, 0.766044), vec3(0.556670, -0.321394, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.383022, -0.321394, 0.866025), vec3(0.383022, -0.321394, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.433013, -0.250000, 0.866025), vec3(0.433013, -0.250000, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.262003, -0.219846, 0.939693), vec3(0.262003, -0.219846, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.296198, -0.171010, 0.939693), vec3(0.296198, -0.171010, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.133022, -0.111619, 0.984808), vec3(0.133022, -0.111619, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.150384, -0.086824, 0.984808), vec3(0.150384, -0.086824, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.150384, -0.086824, 0.984808), vec3(0.150384, -0.086824, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.163176, -0.059391, 0.984808), vec3(0.163176, -0.059391, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.296198, -0.171010, 0.939693), vec3(0.296198, -0.171010, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, -0.116978, 0.939693), vec3(0.321394, -0.116978, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.433013, -0.250000, 0.866025), vec3(0.433013, -0.250000, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.469846, -0.171010, 0.866025), vec3(0.469846, -0.171010, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.556670, -0.321394, 0.766044), vec3(0.556670, -0.321394, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.604023, -0.219846, 0.766044), vec3(0.604023, -0.219846, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.663414, -0.383022, 0.642788), vec3(0.663414, -0.383022, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.719846, -0.262003, 0.642788), vec3(0.719846, -0.262003, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.750000, -0.433013, 0.500000), vec3(0.750000, -0.433013, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.813798, -0.296198, 0.500000), vec3(0.813798, -0.296198, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.813798, -0.469846, 0.342020), vec3(0.813798, -0.469846, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.883022, -0.321394, 0.342020), vec3(0.883022, -0.321394, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.852869, -0.492404, 0.173648), vec3(0.852869, -0.492404, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.925417, -0.336824, 0.173648), vec3(0.925417, -0.336824, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.866025, -0.500000, 0.000000), vec3(0.866025, -0.500000, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.939693, -0.342020, 0.000000), vec3(0.939693, -0.342020, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.852869, -0.492404, -0.173648), vec3(0.852869, -0.492404, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.925417, -0.336824, -0.173648), vec3(0.925417, -0.336824, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.813798, -0.469846, -0.342020), vec3(0.813798, -0.469846, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.883022, -0.321394, -0.342020), vec3(0.883022, -0.321394, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.750000, -0.433013, -0.500000), vec3(0.750000, -0.433013, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.813798, -0.296198, -0.500000), vec3(0.813798, -0.296198, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.663414, -0.383022, -0.642788), vec3(0.663414, -0.383022, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.719846, -0.262003, -0.642788), vec3(0.719846, -0.262003, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.556670, -0.321394, -0.766044), vec3(0.556670, -0.321394, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.604023, -0.219846, -0.766044), vec3(0.604023, -0.219846, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.433013, -0.250000, -0.866025), vec3(0.433013, -0.250000, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.469846, -0.171010, -0.866025), vec3(0.469846, -0.171010, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.296198, -0.171010, -0.939693), vec3(0.296198, -0.171010, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, -0.116978, -0.939693), vec3(0.321394, -0.116978, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.150384, -0.086824, -0.984808), vec3(0.150384, -0.086824, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.163176, -0.059391, -0.984808), vec3(0.163176, -0.059391, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.163176, -0.059391, -0.984808), vec3(0.163176, -0.059391, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, -0.030154, -0.984808), vec3(0.171010, -0.030154, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, -0.116978, -0.939693), vec3(0.321394, -0.116978, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.336824, -0.059391, -0.939693), vec3(0.336824, -0.059391, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.469846, -0.171010, -0.866025), vec3(0.469846, -0.171010, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, -0.086824, -0.866025), vec3(0.492404, -0.086824, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.604023, -0.219846, -0.766044), vec3(0.604023, -0.219846, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.633022, -0.111619, -0.766044), vec3(0.633022, -0.111619, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.719846, -0.262003, -0.642788), vec3(0.719846, -0.262003, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.754407, -0.133022, -0.642788), vec3(0.754407, -0.133022, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.813798, -0.296198, -0.500000), vec3(0.813798, -0.296198, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.852869, -0.150384, -0.500000), vec3(0.852869, -0.150384, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.883022, -0.321394, -0.342020), vec3(0.883022, -0.321394, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.925417, -0.163176, -0.342020), vec3(0.925417, -0.163176, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.925417, -0.336824, -0.173648), vec3(0.925417, -0.336824, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.969846, -0.171010, -0.173648), vec3(0.969846, -0.171010, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.939693, -0.342020, 0.000000), vec3(0.939693, -0.342020, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.984808, -0.173648, 0.000000), vec3(0.984808, -0.173648, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.925417, -0.336824, 0.173648), vec3(0.925417, -0.336824, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.969846, -0.171010, 0.173648), vec3(0.969846, -0.171010, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.883022, -0.321394, 0.342020), vec3(0.883022, -0.321394, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.925417, -0.163176, 0.342020), vec3(0.925417, -0.163176, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.813798, -0.296198, 0.500000), vec3(0.813798, -0.296198, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.852869, -0.150384, 0.500000), vec3(0.852869, -0.150384, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.719846, -0.262003, 0.642788), vec3(0.719846, -0.262003, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.754407, -0.133022, 0.642788), vec3(0.754407, -0.133022, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.604023, -0.219846, 0.766044), vec3(0.604023, -0.219846, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.633022, -0.111619, 0.766044), vec3(0.633022, -0.111619, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.469846, -0.171010, 0.866025), vec3(0.469846, -0.171010, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, -0.086824, 0.866025), vec3(0.492404, -0.086824, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.321394, -0.116978, 0.939693), vec3(0.321394, -0.116978, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.336824, -0.059391, 0.939693), vec3(0.336824, -0.059391, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.163176, -0.059391, 0.984808), vec3(0.163176, -0.059391, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, -0.030154, 0.984808), vec3(0.171010, -0.030154, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, 1.000000), vec3(0.000000, 0.000000, 1.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, -0.030154, 0.984808), vec3(0.171010, -0.030154, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.173648, -0.000000, 0.984808), vec3(0.173648, -0.000000, 0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.336824, -0.059391, 0.939693), vec3(0.336824, -0.059391, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.342020, -0.000000, 0.939693), vec3(0.342020, -0.000000, 0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, -0.086824, 0.866025), vec3(0.492404, -0.086824, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.500000, -0.000000, 0.866025), vec3(0.500000, -0.000000, 0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.633022, -0.111619, 0.766044), vec3(0.633022, -0.111619, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.642788, -0.000000, 0.766044), vec3(0.642788, -0.000000, 0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.754407, -0.133022, 0.642788), vec3(0.754407, -0.133022, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.766044, -0.000000, 0.642788), vec3(0.766044, -0.000000, 0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.852869, -0.150384, 0.500000), vec3(0.852869, -0.150384, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.866025, -0.000000, 0.500000), vec3(0.866025, -0.000000, 0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.925417, -0.163176, 0.342020), vec3(0.925417, -0.163176, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.939693, -0.000000, 0.342020), vec3(0.939693, -0.000000, 0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.969846, -0.171010, 0.173648), vec3(0.969846, -0.171010, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.984808, -0.000000, 0.173648), vec3(0.984808, -0.000000, 0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.984808, -0.173648, 0.000000), vec3(0.984808, -0.173648, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(1.000000, -0.000000, 0.000000), vec3(1.000000, -0.000000, 0.000000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.969846, -0.171010, -0.173648), vec3(0.969846, -0.171010, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.984808, -0.000000, -0.173648), vec3(0.984808, -0.000000, -0.173648), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.925417, -0.163176, -0.342020), vec3(0.925417, -0.163176, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.939693, -0.000000, -0.342020), vec3(0.939693, -0.000000, -0.342020), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.852869, -0.150384, -0.500000), vec3(0.852869, -0.150384, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.866025, -0.000000, -0.500000), vec3(0.866025, -0.000000, -0.500000), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.754407, -0.133022, -0.642788), vec3(0.754407, -0.133022, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.766044, -0.000000, -0.642788), vec3(0.766044, -0.000000, -0.642788), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.633022, -0.111619, -0.766044), vec3(0.633022, -0.111619, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.642788, -0.000000, -0.766044), vec3(0.642788, -0.000000, -0.766044), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.492404, -0.086824, -0.866025), vec3(0.492404, -0.086824, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.500000, -0.000000, -0.866025), vec3(0.500000, -0.000000, -0.866025), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.336824, -0.059391, -0.939693), vec3(0.336824, -0.059391, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.342020, -0.000000, -0.939693), vec3(0.342020, -0.000000, -0.939693), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.171010, -0.030154, -0.984808), vec3(0.171010, -0.030154, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.173648, -0.000000, -0.984808), vec3(0.173648, -0.000000, -0.984808), vec3(1.0f, 0.05f, 0.05f) },
		{ vec3(0.000000, 0.000000, -1.000000), vec3(0.000000, 0.000000, -1.000000), vec3(1.0f, 0.05f, 0.05f) },
	};
	// grid model
	GLuint indexArray[] = {
		0, 1, 2,
		0, 2, 3
	};

	numOfVertices = sizeof(vertexArray) / sizeof(SphereModel::Vertex);

	// Create a vertex array
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);


	// Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
		3,                   // size
		GL_FLOAT,            // type
		GL_FALSE,            // normalized?
		sizeof(SphereModel::Vertex), // stride - each vertex contain 2 vec3 (position, color)
		(void*)0             // array buffer offset
	);
	glEnableVertexAttribArray(0);

	// 2nd attribute buffer : vertex normal
	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(SphereModel::Vertex),
		(void*)sizeof(vec3)    // Normal is Offseted by vec3 (see class Vertex)
	);
	glEnableVertexAttribArray(1);


	glVertexAttribPointer(2,                            // attribute 1 matches aColor in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(SphereModel::Vertex),
		(void*)(2 * sizeof(vec3))   // color is offseted a vec3 (comes after position)
	);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBindVertexArray(0);

	return vertexArrayObject;
}

int createVertexArrayObject4() {
	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

int createVertexArrayObject5()
{
	// Cube model
	vec3 vertexArray[] = {  // position,                            color
		vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), //left - red
		vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
		vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),

		vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),
		vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
		vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),

		vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), // far - blue
		vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
		vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),

		vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
		vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
		vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),

		vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), // bottom - turquoise
		vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
		vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),

		vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
		vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
		vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),

		vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), // near - green
		vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
		vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),

		vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
		vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
		vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),

		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), // right - purple
		vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
		vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),

		vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),
		vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),

		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), // top - yellow
		vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),
		vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),

		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f),
		vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),
		vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f)
	};


	// Create a vertex array
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);


	// Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
		3,                   // size
		GL_FLOAT,            // type
		GL_FALSE,            // normalized?
		2 * sizeof(vec3), // stride - each vertex contain 2 vec3 (position, color)
		(void*)0             // array buffer offset
	);
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		2 * sizeof(vec3),
		(void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
	);
	glEnableVertexAttribArray(1);


	return vertexArrayObject;
}

bool initContext() {     // Initialize GLFW and OpenGL version
	glfwInit();

#if defined(PLATFORM_OSX)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
	// On windows, we set OpenGL version to 2.1, to support more hardware
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

	// Create Window and rendering context using GLFW, resolution is 1024, 768
	window = glfwCreateWindow(windowLength, windowWidth, "Comp371 - Assignment 1", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	// Scroll Callback for field of view
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to create GLEW" << std::endl;
		glfwTerminate();
		return false;
	}
	return true;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 100.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 100.0f)
		fov = 100.0f;
}
