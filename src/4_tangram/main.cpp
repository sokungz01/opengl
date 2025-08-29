#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

// Vertex Shader source code
const char *vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
     "}\0";
// Fragment Shader source code
const char *fragmentShaderSource = 
  "#version 330 core\n"
  "out vec4 FragColor;\n"
  "void main()\n"
  "{\n"
  "   float t = gl_FragCoord.x / 800.0;\n"
  "   vec3 color = mix(vec3(0.7, 1.0, 0.7), vec3(0.7, 0.9, 1.0), t);\n"
  "   FragColor = vec4(color, 1.0);\n"
  "}\n\0";

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Tangram pieces: 2 large triangles, 1 medium triangle, 2 small triangles, 1 square, 1 parallelogram
// Each piece is a triangle or square, defined in normalized device coordinates
// Colors will be set in the fragment shader by gl_VertexID

const unsigned int TANGRAM_SIZE = 7;
typedef struct TANGRAM {
    GLfloat vertices[];
    float centroidPosX;
    float centroidPosY;
}  TANGRAM_T;

void onMouseMoveEvent(GLFWwindow *window){
  
}

void glGenerateBufferData(GLfloat vertices[]) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}


void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }
}

int main()
{
  // glfw: initialize and configure

    TANGRAM_T tangramVertices = malloc(TANGRAM_SIZE * sizeof(TANGRAM_T));
    

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // need to bind the vertex array first, then bind and set vertex buffer
    glBindVertexArray(VAO);
    glGenerateBufferData();

    // position attribute
    // (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer (offset) )
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
    // input
        processInput(window);

        // clear screen
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw tangram (7 pieces, 7 colors)
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        for(int i = 0 ; i < sizeof(tangramVertices)/sizeof(tangramVertices[0]); i ++) {
          glGenerateBufferData(tangramVertices.vertices);
        }
        glDrawArrays(GL_TRIANGLES, 0, 21);
        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}