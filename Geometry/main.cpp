#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "stb_image.h"
#include "shaders.h"

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out VS_OUT\n"
"{\n"
"    vec3 color;\n"
"} vs_out;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
"   vs_out.color = aColor;\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 fcolor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(fcolor, 1.0);\n"
"}\0";

const char* gShaderCode = "#version 330 core\n"
"layout (points) in;\n"
"layout (triangle_strip, max_vertices = 5) out;\n"
"in VS_OUT\n"
"{\n"
"    vec3 color;\n"
"} gs_in[];\n"
"out vec3 fcolor;\n"
"void build_house(vec4 position) {\n"
"    fcolor = gs_in[0].color;\n"
"    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);\n"
"EmitVertex();\n"
"    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);\n"
"EmitVertex();\n"
"    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);\n"
"EmitVertex();\n"
"    gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);\n"
"EmitVertex();\n"
"    gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);\n"
"    fcolor = vec3(1.0, 1.0, 1.0);\n"
"EmitVertex();\n"
"EndPrimitive();\n"
"}\n"
"void main() {\n"
"	build_house(gl_in[0].gl_Position);\n"
"}\0";


unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int geometryShader;

unsigned int VBO;
unsigned int VAO;



float points[] = {
    -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
    -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, // bottom-left
	 0.0f,  0.5f, 0.7f, 0.3f, 0.5f,	// middle top
	 0.0f, -0.5f, 0.0f, 0.0f, 0.0f  // middle bottom
};  


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Load the icon image
	int width, height, channels;
	unsigned char* image = stbi_load("icon.png", &width, &height, &channels, 4); // Load as RGBA

	GLFWimage icon;
	icon.width = width;
	icon.height = height;
	icon.pixels = image;

	glfwSetWindowIcon(window, 1, &icon);

	stbi_image_free(image);


	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(1);


	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometryShader, 1, &gShaderCode, NULL);
	glCompileShader(geometryShader);

	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glAttachShader(shaderProgram, geometryShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(geometryShader);

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;


	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);

		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();

		processInput(window);
	}
	// we would like to properly clean / delete all of GLFW's resources that were allocated
	glfwTerminate();
	return 0;
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}