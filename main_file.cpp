
#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "constants.h"
#include "shader.h"

#include "objects_vertices.h"

#include "Model.h"
#include "skybox.h"
#include "collisions.h"

unsigned int VBO, VAO;


float roll_speed=0;
float pitch_speed=0;
float yaw_speed = 0;
float movement_speed = 0;
float acceleration = 0;

float bird_speed = 4;
float birdMove = 0;

float aspectRatio=2; // = width/height

Shader *object_shader;
Shader *light_shader;
Shader *skybox_shader;

Model *plane;
Model *bird;

Skybox *skybox;

bool spacebarReset = false;

GLuint texPlane;//plane textures
GLuint texPlaneC;

GLuint texBird;//goose textures

GLuint texGrass; //grass textures 
GLuint texGrassC;

GLuint texRunway; //runway textures
GLuint texRunwayC;

GLuint texStone; //stone textures
GLuint texStoneC;

GLuint texBuilding1; //buidling1 textures
GLuint texBuilding1C;

GLuint texBuilding2; //building2 textures
GLuint texBuilding2C;

GLuint texBuilding3; //building3 textures
GLuint texBuilding3C;

GLuint texRoof1; //roof1 textures
GLuint texRoof1C;

GLuint texRoof2; //roof2 textures
GLuint texRoof2C;

GLuint texRoof3; //roof3 textures
GLuint texRoof3C;



void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}



GLuint readTexture(char const* path)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}



void key_callback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mod
) {
	if (action == GLFW_PRESS) 
	{
		if (key == GLFW_KEY_LEFT) roll_speed = -2;
		if (key == GLFW_KEY_RIGHT) roll_speed = 2;
		if (key == GLFW_KEY_UP) pitch_speed = 2;
		if (key == GLFW_KEY_DOWN) pitch_speed = -2;
		if (key == GLFW_KEY_A) yaw_speed = 2;
		if (key == GLFW_KEY_D) yaw_speed = -2;
		if (key == GLFW_KEY_W) acceleration = 0.25;
		if (key == GLFW_KEY_S) acceleration = -0.125;

	}
	if (action == GLFW_RELEASE) 
	{
		if (key == GLFW_KEY_LEFT) roll_speed = 0;
		if (key == GLFW_KEY_RIGHT) roll_speed = 0;
		if (key == GLFW_KEY_UP) pitch_speed = 0;
		if (key == GLFW_KEY_DOWN) pitch_speed = 0;
		if (key == GLFW_KEY_A) yaw_speed = 0;
		if (key == GLFW_KEY_D) yaw_speed = 0;
		if (key == GLFW_KEY_SPACE) spacebarReset = true;
		if (key == GLFW_KEY_W) acceleration = 0;
		if (key == GLFW_KEY_S) acceleration = 0;
	}
}



void windowResizeCallback(GLFWwindow* window,int width,int height)
{
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}



void setupLight() {
	object_shader->use();

	object_shader->setVec3("dirLight.position", glm::vec3(400.0f, 200.0f, 200.0f));
	object_shader->setVec3("dirLight.ambient", glm::vec3(0.3f,0.3f,0.3f)); 
	object_shader->setVec3("dirLight.diffuse", 1.0f, 1.0f, 1.0f); 
	object_shader->setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f); 

	for (int i = 0; i < 10; i++)
	{
		object_shader->setVec3("pointLights[" + to_string(i) + "].position", runwayPlates[i] + glm::vec3(5.2f, 0.24f, 0.0f));
		object_shader->setVec3("pointLights[" + to_string(i) + "].ambient", 0.8f, 0.1f, 0.1f);
		object_shader->setVec3("pointLights[" + to_string(i) + "].diffuse", 1.0f, 0.0f, 0.0f);
		object_shader->setVec3("pointLights[" + to_string(i) + "].specular", 1.0f, 0.0f, 0.0f);
		object_shader->setFloat("pointLights[" + to_string(i) + "].constant", 1.0f); 
		object_shader->setFloat("pointLights[" + to_string(i) + "].linear", 0.22);
		object_shader->setFloat("pointLights[" + to_string(i) + "].quadratic", 0.20);
	}
	for (int i = 0; i < 10; i++)
	{
		object_shader->setVec3("pointLights[" + to_string(i + 10) + "].position", runwayPlates[i] + glm::vec3(-5.2f, 0.24f, 0.0f));
		object_shader->setVec3("pointLights[" + to_string(i + 10) + "].ambient", 0.8f, 0.1f, 0.1f);
		object_shader->setVec3("pointLights[" + to_string(i + 10) + "].diffuse", 1.0f, 0.0f, 0.0f);
		object_shader->setVec3("pointLights[" + to_string(i + 10) + "].specular", 1.0f, 0.0f, 0.0f);
		object_shader->setFloat("pointLights[" + to_string(i + 10) + "].constant", 1.0f);
		object_shader->setFloat("pointLights[" + to_string(i + 10) + "].linear", 0.22);
		object_shader->setFloat("pointLights[" + to_string(i + 10) + "].quadratic", 0.20);
	}
	object_shader->setFloat("material.shininess", 32.0f); //set to the all materials - effect of sunlight
}



void initOpenGLProgram(GLFWwindow* window)
{
	glClearColor(0,0.75,0.75,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,key_callback);

	//init shaders
	object_shader = new Shader("shaders/v_object.glsl", "shaders/f_object.glsl");
	setupLight();
	light_shader = new Shader("shaders/v_light.glsl", "shaders/f_light.glsl");
	skybox_shader = new Shader("shaders/v_skybox.glsl", "shaders/f_skybox.glsl");

	//init skybox
	skybox = new Skybox;

	//init models
	plane = new Model(std::string("models/plane/piper_pa18.obj"));
	bird = new Model(std::string("models/bird/bird.obj"));

	//read plane textures
	texPlane = readTexture("models/plane/textures/piper_diffuse.jpg");
	texPlaneC = readTexture("models/plane/textures/piper_refl.jpg");

	//read bird textures
	texBird = readTexture("models/bird/textures/SparrowAO.png");

	//read grass textures
	texGrass = readTexture("textures/grass2.png");
	texGrassC = readTexture("textures/grass2_c.png");

	//read runway textures
	texRunway = readTexture("textures/runway.png");
	texRunwayC = readTexture("textures/runway_c.png");

	//read stone textures
	texStone = readTexture("textures/stone.png");
	texStoneC = readTexture("textures/stone_c.png");

	//read buildings textures
	texBuilding1 = readTexture("textures/building1.png");
	texBuilding1C = readTexture("textures/building1_c.png");
	texBuilding2 = readTexture("textures/building2.png");
	texBuilding2C = readTexture("textures/building2_c.png");
	texBuilding3 = readTexture("textures/building3.png");
	texBuilding3C = readTexture("textures/building3_c.png");

	//read roofs textures
	texRoof1 = readTexture("textures/roof1.png");
	texRoof1C = readTexture("textures/roof1_c.png");
	texRoof2 = readTexture("textures/roof2.png");
	texRoof2C = readTexture("textures/roof2_c.png");
	texRoof3 = readTexture("textures/roof3.png");
	texRoof3C = readTexture("textures/roof3_c.png");
}



void freeOpenGLProgram(GLFWwindow* window)
{
	//Shader
	delete object_shader;
	delete light_shader;
	delete skybox_shader;
	//Model
	delete plane;
	delete bird;
	//Skybox
	delete skybox;
	//Texture
	glDeleteTextures(1, &texPlane);
	glDeleteTextures(1, &texPlaneC);
	glDeleteTextures(1, &texBird);
	glDeleteTextures(1, &texGrass);
	glDeleteTextures(1, &texGrassC);
	glDeleteTextures(1, &texRunway);
	glDeleteTextures(1, &texRunwayC);
	glDeleteTextures(1, &texStone);
	glDeleteTextures(1, &texStoneC);
	glDeleteTextures(1, &texBuilding1);
	glDeleteTextures(1, &texBuilding1C);
	glDeleteTextures(1, &texBuilding2);
	glDeleteTextures(1, &texBuilding2C);
	glDeleteTextures(1, &texBuilding3);
	glDeleteTextures(1, &texBuilding3C);
	glDeleteTextures(1, &texRoof1);
	glDeleteTextures(1, &texRoof1C);
	glDeleteTextures(1, &texRoof2);
	glDeleteTextures(1, &texRoof2C);
	glDeleteTextures(1, &texRoof3);
	glDeleteTextures(1, &texRoof3C);
}


//setup to draw cube
void setupBuffersCube()
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}





//==================================================GET_MATRIX===============================================
glm::mat4 getPlaneM(glm::mat4 model, float roll_angle, float yaw_angle, float pitch_angle, float movement)
{
	glm::mat4 M = model;

	glm::mat4 testM = glm::rotate(M, pitch_angle, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::bvec3 collision = collisionDetect(testM);
	if (!(collision.x || collision.y || collision.z)) M = testM;

	testM = glm::rotate(M, yaw_angle, glm::vec3(0.0f, 1.0f, 0.0f));
	collision = collisionDetect(testM);
	if (!(collision.x || collision.y || collision.z)) M = testM;

	testM = glm::rotate(M, roll_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	collision = collisionDetect(testM);
	if (!(collision.x || collision.y || collision.z)) M = testM;


	glm::mat3 rotation = glm::mat3(M);
	
	glm::vec3 shift = rotation * glm::vec3(0, 0, movement);

	testM = glm::translate(M, glm::vec3(0, 0, movement));
	collision = collisionDetect(testM);

	if (rotation[1][1] < 0.85 && collision.y) spacebarReset = true;

	if (collision.x == false) M[3][0] += shift.x;
	if (collision.y == false) M[3][1] += shift.y;
	if (collision.z == false) M[3][2] += shift.z;

	return M;
}



glm::vec3 correctCameraPosition(glm::vec3 cameraPosition) 
{
	glm::vec3 correctedCameraPosition = cameraPosition;

	if (cameraPosition.y <= 0.2f) correctedCameraPosition.y = 0.3f;

	return correctedCameraPosition;
}



glm::mat4 getV(glm::mat4 M)
{
	glm::vec3 planePosition = glm::vec3(M[3][0], M[3][1], M[3][2]);
	glm::vec3 cameraDistance = glm::vec3(0, 6, -20);
	glm::mat3 planeRotation = glm::mat3(M);

	cameraDistance = planeRotation * cameraDistance;
	glm::vec3 cameraPosition = cameraDistance + planePosition;
	glm::vec3 cameraUp = planeRotation * glm::vec3(0, 1, 0);
	
	cameraPosition = correctCameraPosition(cameraPosition);

	return glm::lookAt(cameraPosition, planePosition, cameraUp);
}


//============================================DRAWING====================================================
void drawGroundW(glm::mat4 V, glm::mat4 P) 
{
	object_shader->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texGrass);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texGrassC);

	glm::mat4 M = glm::mat4(1.0f);
	M = glm::scale(M, glm::vec3(100.0f, 0.2f, 100.0f));

	object_shader->setMat4("P", P);
	object_shader->setMat4("V", V);
	object_shader->setMat4("M", M);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
	glBindVertexArray(0);
}

void drawRunway(glm::mat4 V, glm::mat4 P) 
{
	for (int i = 0; i < 10; i++) 
	{
		object_shader->use();
		glm::mat4 Runway = glm::mat4(1.0f);
		Runway = glm::translate(Runway, runwayPlates[i]);
		Runway = glm::scale(Runway, glm::vec3(5.0f, 0.23f, 5.0f));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texRunway);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texRunwayC);

		object_shader->setMat4("P", P);
		object_shader->setMat4("V", V);
		object_shader->setMat4("M", Runway);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
		glBindVertexArray(0);

		light_shader->use();

		glm::mat4 Light = glm::mat4(1.0f);
		Light = glm::translate(Light, runwayPlates[i] + glm::vec3(5.2f, 0.0f, 0.0f));
		Light = glm::scale(Light, glm::vec3(0.5f, 0.5f, 0.75f));

		light_shader->setMat4("P", P);
		light_shader->setMat4("V", V);
		light_shader->setMat4("M", Light);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
		glBindVertexArray(0);

		glm::mat4 Light2 = glm::mat4(1.0f);
		Light2 = glm::translate(Light2, runwayPlates[i] + glm::vec3(-5.2f, 0.0f, 0.0f));
		Light2 = glm::scale(Light2, glm::vec3(0.5f, 0.5f, 0.75f));

		light_shader->setMat4("M", Light2);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
		glBindVertexArray(0);
	}
}

void drawStone(glm::mat4 V, glm::mat4 P)
{
	object_shader->use();

	object_shader->setMat4("P", P);
	object_shader->setMat4("V", V);

	for (int i = 0; i < 4; i++) 
	{
		glm::mat4 Stone = glm::mat4(1.0f);
		Stone = glm::translate(Stone, stonePlates[i]);
		Stone = glm::scale(Stone, glm::vec3(22.5f, 0.24f, 22.5f));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texStone);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texStoneC);

		object_shader->setMat4("M", Stone);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
		glBindVertexArray(0);
	}
}

void drawBuildings(glm::mat4 V, glm::mat4 P) 
{
	object_shader->use();

	object_shader->setMat4("P", P);
	object_shader->setMat4("V", V);

	GLuint buildingTextures[4] = {
		texBuilding1,texBuilding3,texBuilding2,texBuilding1
	};

	GLuint buildingTexturesC[4] = {
		texBuilding1C,texBuilding3C,texBuilding2C,texBuilding1C
	};

	GLuint roofTextures[3] = {
		texRoof1,texRoof2,texRoof3
	};

	GLuint roofTexturesC[3] = {
		texRoof1C,texRoof2C,texRoof3C
	};

	float x_shift;
	float z_shift;

	for (int i = 0; i < 4; i++) 
	{
		for (int j = 0; j < 4; j++)
		{
			if (j % 2 == 1) x_shift = -10.0f;
			else x_shift = 10.0f;

			if (j > 1) z_shift = -10.0f;
			else z_shift = 10.0f;

			//Buildings drawing

			glm::mat4 Building = glm::mat4(1.0f);
			glm::vec3 shift = stonePlates[i] + glm::vec3(x_shift, 0.48f, z_shift);
			Building = glm::translate(Building, shift);
			Building = glm::translate(Building, glm::vec3(0.0f, (3.0f * (j + 1)) / 2, 0.0f));
			Building = glm::scale(Building, glm::vec3(7.5f, 3.0f * (j + 1), 7.5f));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, buildingTextures[j]);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, buildingTexturesC[j]);

			object_shader->setMat4("M", Building);

			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
			glBindVertexArray(0);

			//Roofs drawing

			glm::mat4 Roof = glm::mat4(1.0f);
			glm::vec3 roofShift = stonePlates[i] + glm::vec3(x_shift, 0.48f, z_shift);
			Roof = glm::translate(Roof, roofShift);
			Roof = glm::translate(Roof, glm::vec3(0.0f, 3.0f * (j + 1) * 1.5f, 0.0f));
			Roof = glm::scale(Roof, glm::vec3(7.8f, 0.25f, 7.8f));


			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, roofTextures[(i + j) % 3]);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, roofTexturesC[(i + j) % 3]);

			object_shader->setMat4("M", Roof);

			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
			glBindVertexArray(0);
		}
	}
}

void drawPlane(glm::mat4 P, glm::mat4 V, glm::mat4 M) 
{
	object_shader->use();

	object_shader->setMat4("P", P);
	object_shader->setMat4("V", V);
	object_shader->setMat4("M", M);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texPlane);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texPlaneC);

	plane->Draw(*object_shader);
}

void drawBird(glm::mat4 P, glm::mat4 V) 
{
	object_shader->use();

	birdMove += bird_speed * glfwGetTime();
	glm::mat4 M = glm::mat4(1.0f);

	for (int i = 0; i < 4; i++) {
		M = glm::mat4(1.0f);
		M = glm::translate(M, glm::vec3(-50, 30, -50));
		M = glm::rotate(M, (PI / 2)*i + birdMove, glm::vec3(0, 1, 0));
		M = glm::rotate(M, -PI / 3, glm::vec3(0, 0, 1));
		M = glm::translate(M, glm::vec3(0, 0, 35));

		object_shader->setMat4("P", P);
		object_shader->setMat4("V", V);
		object_shader->setMat4("M", M);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texBird);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texBird);

		bird->Draw(*object_shader);
	}
}


glm::mat4 drawScene(GLFWwindow* window, glm::mat4 planeModel, float roll_angle, float yaw_angle, float pitch_angle, float movement)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.1f, 200.0f);

	glm::mat4 planeM = getPlaneM(planeModel, roll_angle, yaw_angle, pitch_angle, movement);

	glm::mat4 V = getV(planeM);

	drawPlane(P, V, planeM);
	drawBird(P, V);
	drawGroundW(V, P);
	drawRunway(V, P);
	drawStone(V, P);
	drawBuildings(V, P);
	
	skybox->draw(*skybox_shader, V, P);
    glfwSwapBuffers(window); 

	return planeM;
}

float calculateMovementSpeed(float movementSpeed, glm::mat4 planeM) 
{
	float newMovementSpeed = movementSpeed + acceleration;
	float maxSpeed = 20;
	float minSpeed = 10;
	if (acceleration >= 0) 
	{
		if (newMovementSpeed > maxSpeed) newMovementSpeed = maxSpeed;
	}
	else 
	{
		if (planeM[3][1] < 1) {
			if (newMovementSpeed < 0) newMovementSpeed = 0;
		}
		else 
		{
			if (newMovementSpeed < minSpeed) newMovementSpeed = minSpeed;
		}
	}
	
	return newMovementSpeed;
}



//========================================================MAIN======================================
int main(void)
{
	GLFWwindow* window; 

	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) { 
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1600, 800, "Lot Samolotem", NULL, NULL);

	if (!window) 
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	initOpenGLProgram(window); 

	float angle_x; 
	float angle_y;
	float angle_z; 
	float movement = 0;
	glm::mat4 planeM = glm::mat4(1.0f);

	//plane staring positioning
	planeM[3][0] = 25;
	planeM[3][1] = 0.3;
	planeM[3][2] = 80;
	planeM = rotate(planeM, PI, glm::vec3(0.0f, 1.0f, 0.0f));


	bool reset;
	glfwSetTime(0);
	setupBuffersCube();	//we already draw only cube so placed this function here
	while (!glfwWindowShouldClose(window))
	{
		reset = CollisionWithBuilding(planeM);
		if (reset || spacebarReset) {
			planeM = glm::mat4(1.0f);

			//plane staring positioning
			planeM[3][0] = 25;
			planeM[3][1] = 0.3;
			planeM[3][2] = 80;
			planeM = rotate(planeM, PI, glm::vec3(0.0f, 1.0f, 0.0f));

			movement_speed = 0;
			acceleration = 0;
		}
		reset = false;
		spacebarReset = false;
		angle_x = roll_speed * glfwGetTime();
		angle_y = yaw_speed * glfwGetTime();
		angle_z = pitch_speed * glfwGetTime(); 
		movement_speed = calculateMovementSpeed(movement_speed, planeM);
		movement = movement_speed * glfwGetTime();
        glfwSetTime(0);
		planeM = drawScene(window, planeM, angle_x, angle_y, angle_z, movement);
		glfwPollEvents();
	}

	freeOpenGLProgram(window);
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
