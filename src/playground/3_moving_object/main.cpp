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


GLfloat vertices[] =
{
    0.0f, 0.3f, 0.0f,
    0.0f, -0.4f, 0.0f,
    0.3f, 0.2f, 0.0f,
    0.3f, 0.2f, 0.0f,
    0.1f, 0.4f, 0.0f,
    0.0f, 0.3f, 0.0f,

    0.0f, 0.3f, 0.0f,
    0.0f, -0.4f, 0.0f,
    -0.3f, 0.2f, 0.0f,
    -0.3f, 0.2f, 0.0f,
    -0.1f, 0.4f, 0.0f,
    0.0f, 0.3f, 0.0f,

};

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

  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
    for(int i = 0 ; i < sizeof(vertices) / sizeof(vertices[0]); i += 3){
        vertices[i+1] += 0.01f;
    }
  }
  if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
    for(int i = 0 ; i < sizeof(vertices) / sizeof(vertices[0]); i += 3){
        vertices[i+1] -= 0.01f;
    }
  }
  if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
    for(int i = 0 ; i < sizeof(vertices) / sizeof(vertices[0]); i += 3){
        vertices[i] -= 0.01f;
    }
  }
  if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
    for(int i = 0 ; i < sizeof(vertices) / sizeof(vertices[0]); i += 3){
        vertices[i] += 0.01f;
    }
  }
  // Rotate left (A) or right (D) around object centroid
  float angle = 0.0f;
  if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
    angle = 0.05f; // counterclockwise
  }
  if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
    angle = -0.05f; // clockwise
  }
  if(angle != 0.0f) {
    float cx = 0.0f, cy = 0.0f;
    int count = 0;
    for(int i = 0 ; i < sizeof(vertices) / sizeof(vertices[0]); i += 3){
      cx += vertices[i];
      cy += vertices[i+1];
      count++;
    }
    cx /= count;
    cy /= count;
    float cosA = cos(angle);
    float sinA = sin(angle);
    for(int i = 0 ; i < sizeof(vertices) / sizeof(vertices[0]); i += 3){
      float x = vertices[i] - cx;
      float y = vertices[i+1] - cy;
      float xr = x * cosA - y * sinA;
      float yr = x * sinA + y * cosA;
      vertices[i]   = xr + cx;
      vertices[i+1] = yr + cy;
    }
  }
  
}

int main()
{
  // glfw: initialize and configure
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
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

    //draw our first triangle
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 12);

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