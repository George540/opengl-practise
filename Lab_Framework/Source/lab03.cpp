//
// COMP 371 Labs Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
//

#include <iostream>
#include <list>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

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

const char* getFragmentShaderSource();

int compileAndLinkShaders();

int createVertexArrayObject();

bool initContext();

GLFWwindow * window = NULL;

int main(int argc, char*argv[])
{
	if (!initContext()) return -1;

	// Black background
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	// Compile and link shaders here ...
	int shaderProgram = compileAndLinkShaders();

	// We can set the shader once, since we have only one
	glUseProgram(shaderProgram);


	// Camera parameters for view transform
	vec3 cameraPosition(0.6f, 1.0f, 20.0f);
	vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
	vec3 cameraUp(0.0f, 1.0f, 0.0f);

	// Other camera parameters
	float cameraSpeed = 1.0f;
	float cameraFastSpeed = 7 * cameraSpeed;
	float cameraHorizontalAngle = 90.0f;
	float cameraVerticalAngle = 0.0f;
	bool  cameraFirstPerson = true; // press 1 or 2 to toggle this variable

									// Set projection matrix for shader, this won't change
	mat4 projectionMatrix = glm::perspective(70.0f,            // field of view in degrees
		800.0f / 600.0f,  // aspect ratio
		0.01f, 100.0f);   // near and far (near > 0)

	GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

	GLuint colorLocation = glGetUniformLocation(shaderProgram, "objectColor");

	// Set initial view matrix
	mat4 viewMatrix = lookAt(cameraPosition,  // eye
		cameraPosition + cameraLookAt,  // center
		cameraUp); // up

	GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);



	// Define and upload geometry to the GPU here ...
	int vao = createVertexArrayObject();

	// For frame time
	float lastFrameTime = glfwGetTime();
	int lastMouseLeftState = GLFW_RELEASE;
	double lastMousePosX, lastMousePosY;
	glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

	// Other OpenGL states to set once before the Game Loop
	// Enable Backface culling
	glEnable(GL_CULL_FACE);

	// @TODO 1 - Enable Depth Test
	glEnable(GL_DEPTH_TEST);

	srand(time(NULL));
	GLfloat random1 = 0.0f;
	GLfloat random2 = 0.0f;

	GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
	GLuint orientationMatrixLocation = glGetUniformLocation(shaderProgram, "orientationMatrix");
	mat4 olafWorldMatrix;
	mat4 gizmoWorldMatrix;
	mat4 gridWorldMatrix;
	mat4 model = mat4(1.0f);
	vec3 olafPosition(0.0f, 0.0f, 0.0f);

	mat4 orientationMatrix = mat4(1.0f);
	vec2 currentOrientation(0.0f, 0.0f);

	float spinningAngle = 0.0f;
	// Entering Game Loop
	while (!glfwWindowShouldClose(window))
	{
		// Add the GL_DEPTH_BUFFER_BIT to glClear – TODO 1
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Each frame, reset color of each pixel to glClearColor
		//glClear(GL_COLOR_BUFFER_BIT);

		// Frame time calculation
		float dt = glfwGetTime() - lastFrameTime;
		lastFrameTime += dt;

		// Each frame, reset color of each pixel to glClearColor

		// @TODO 1 - Clear Depth Buffer Bit as well
		// ...
		glClear(GL_COLOR_BUFFER_BIT);


		// Draw geometry
		glBindVertexArray(vao);
		//glBindBuffer(GL_ARRAY_BUFFER, vbo);

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			random1 = (rand() % 50) - 25;
			random2 = (rand() % 50) - 25;
		}


		orientationMatrix = rotate(rotate(mat4(1.0f), currentOrientation.x, vec3(1.0f, 0.0f, 0.0f)), currentOrientation.y, vec3(0.0f, 1.0f, 0.0f)) * scale;
		glUniformMatrix4fv(orientationMatrixLocation, 1, GL_FALSE, &orientationMatrix[0][0]);

		// Gizmo
		// X-axis
		gizmoWorldMatrix = translate(model, vec3(2.5f, 0.0f, 0.0f)) * scale(model, vec3(5.0f, 0.1f, 0.1f));
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &gizmoWorldMatrix[0][0]);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Y-axis
		gizmoWorldMatrix = translate(model, vec3(0.0f, 2.5f, 0.0f)) * scale(model, vec3(0.1f, 5.0f, 0.1f));
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &gizmoWorldMatrix[0][0]);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 1.0, 0.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Z-axis
		gizmoWorldMatrix = translate(model, vec3(0.0f, 0.0f, 2.5f)) * scale(model, vec3(0.1f, 0.1f, 5.0f));
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &gizmoWorldMatrix[0][0]);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 1.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		spinningAngle += 180.0f *dt;
		// Draw cube-bottom
		olafWorldMatrix = translate(model, olafPosition) * translate(model, vec3(random1, 1.0f, random2 - 5.0f)) * scale(model, vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &olafWorldMatrix[0][0]);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Draw cube-Middle
		olafWorldMatrix = translate(model, olafPosition) * translate(model, vec3(random1, 2.5f, random2 - 5.0f)) * scale(model, vec3(1.2f, 1.2f, 1.2f));
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &olafWorldMatrix[0][0]);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Draw cube-Top
		olafWorldMatrix = translate(model, olafPosition) * translate(model, vec3(random1, 3.8f, random2 - 5.0f)) * scale(model, vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &olafWorldMatrix[0][0]);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Draw arm-left
		olafWorldMatrix = translate(model, olafPosition) * translate(model, vec3(random1 - 1.2f, 2.2f, random2 - 5.0f)) * rotate(model, radians(45.0f), vec3(0.0f, 0.0f, 1.0f)) * scale(model, vec3(2.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &olafWorldMatrix[0][0]);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Draw arm-right
		olafWorldMatrix = translate(model, olafPosition) * translate(model, vec3(random1 + 1.2f, 2.2f, random2 - 5.0f)) * rotate(model, radians(-45.0f), vec3(0.0f, 0.0f, 1.0f)) * scale(model, vec3(2.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &olafWorldMatrix[0][0]);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Hat1
		olafWorldMatrix = translate(model, olafPosition) * translate(model, vec3(random1, 4.5f, random2 - 5.0f)) * scale(model, vec3(2.0f, 0.5f, 2.0f));
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &olafWorldMatrix[0][0]);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Hat2
		olafWorldMatrix = translate(model, olafPosition) * translate(model, vec3(random1, 5.0f, random2 - 5.0f)) * scale(model, vec3(0.8f, 0.5f, 0.8f));
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &olafWorldMatrix[0][0]);
		glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glDrawArrays(GL_TRIANGLES, 0, 36);




		//if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		//olafWorldMatrix *= rotate(model, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));

		//Draw grid
		for (int i = 0; i <= 100; ++i)
		{
			gridWorldMatrix = translate(model, vec3(-50.0f + i, 0.0f, 0.0f)) * scale(model, vec3(0.01f, 0.01f, 100.0f));
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &gridWorldMatrix[0][0]);
			glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 0.0)));
			glDrawArrays(GL_TRIANGLES, 0, 36);

			gridWorldMatrix = translate(model, vec3(0.0f, 0.0f, -50.0f + i)) * rotate(model, radians(90.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(model, vec3(0.01f, 0.01f, 100.0f));
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &gridWorldMatrix[0][0]);
			glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 0.0)));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		// @TODO 3 - Update and draw projectiles
		// ...



		// End Frame
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Handle inputs
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);


		// Fast Camera Movement
		bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
		float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;


		// @TODO 4 - Calculate mouse motion dx and dy
		//         - Update camera horizontal and vertical angle


		double mousePosX, mousePosY;
		glfwGetCursorPos(window, &mousePosX, &mousePosY);

		double dx = mousePosX - lastMousePosX;
		double dy = mousePosY - lastMousePosY;

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
		//*/

		// @TODO 5 = use camera lookat and side vectors to update positions with ASDW
		// adjust code below
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
		{
			cameraPosition -= cameraSideVector * currentCameraSpeed * dt;
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
		{
			cameraPosition += cameraSideVector * currentCameraSpeed * dt;
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera up
		{
			cameraPosition -= cameraLookAt * currentCameraSpeed * dt;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera down
		{
			cameraPosition += cameraLookAt * currentCameraSpeed * dt;
		}


		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			//olafPosition.x += 5 * dt;
			currentOrientation.x += radians(5.0f);
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			//olafPosition.x -= 5 * dt;
			currentOrientation.x -= radians(5.0f);
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			olafPosition.z += 5 * dt;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			olafPosition.z -= 5 * dt;
		}





		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) // SPACE INPUT
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) // SPACE INPUT
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) // SPACE INPUT
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);
		}
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) // SPACE INPUT
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}





		// TODO 6
		// Set the view matrix for first and third person cameras
		// - In first person, camera lookat is set like below
		// - In third person, camera position is on a sphere looking towards center
		mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

		GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);


		// @TODO 2 - Shoot Projectiles
		//
		// shoot projectiles on mouse left click
		// To detect onPress events, we need to check the last state and the current state to detect the state change
		// Otherwise, you would shoot many projectiles on each mouse press
		// ...


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
		"out vec4 FragColor;"
		"void main()"
		"{"
		"   FragColor = vec4(objectColor.r, objectColor.g, objectColor.b, 1.0f);"
		"}";
}

int compileAndLinkShaders()
{
	// compile and link shader program
	// return shader program id
	// ------------------------------------

	// vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexShaderSource = getVertexShaderSource();
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
	const char* fragmentShaderSource = getFragmentShaderSource();
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
		vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), //left
		vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),

		vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

		vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), // far
		vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

		vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

		vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), // bottom
		vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

		vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

		vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), // near
		vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),

		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),

		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), // right
		vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

		vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),

		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), // top
		vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
		vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
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

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBindVertexArray(0);

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
	window = glfwCreateWindow(1024, 768, "Comp371 - Assignment 1", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	// @TODO 3 - Disable mouse cursor
	// ...
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
