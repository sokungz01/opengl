#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <array>

using namespace std;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

std::array<float, 3> midpoint(const std::array<float, 3>& a, const std::array<float, 3>& b);
void sierpinski(std::vector<float>& vertices, const std::array<float, 3>& a, const std::array<float, 3>& b, const std::array<float, 3>& c, int depth);
void processInput(GLFWwindow *window, int * depth);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

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
    "   FragColor = vec4(0.15f, 0.65f, 0.85f, 1.0f);\n"
    "}\n\0";

int main()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // glfw window creation
  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Sierpinski", NULL, NULL);

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


  std::array<float, 3> v0 = {-0.8f, -0.7f, 0.0f};
  std::array<float, 3> v1 = {0.8f, -0.7f, 0.0f};
  std::array<float, 3> v2 = {0.0f, 0.9f, 0.0f};
  std::vector<float> vertices;
  int depth = 0;

  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // render loop
  while (!glfwWindowShouldClose(window))
  {
    // input
    processInput(window, &depth);
    vertices.clear();
    
    sierpinski(vertices, v0, v1, v2, depth);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

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

// Helper function to interpolate between two points
std::array<float, 3> midpoint(const std::array<float, 3>& a, const std::array<float, 3>& b) {
  return { (a[0] + b[0]) / 2.0f, (a[1] + b[1]) / 2.0f, (a[2] + b[2]) / 2.0f };
}

// Recursively generate Sierpinski triangle vertices
void sierpinski(std::vector<float>& vertices, const std::array<float, 3>& a, const std::array<float, 3>& b, const std::array<float, 3>& c, int depth) {
  if (depth == 0) {
    // Add one triangle
    vertices.insert(vertices.end(), {a[0], a[1], a[2], b[0], b[1], b[2], c[0], c[1], c[2]});
    return;
  }
  std::array<float, 3> ab = midpoint(a, b);
  std::array<float, 3> bc = midpoint(b, c);
  std::array<float, 3> ca = midpoint(c, a);
  sierpinski(vertices, a, ab, ca, depth - 1);
  sierpinski(vertices, ab, b, bc, depth - 1);
  sierpinski(vertices, ca, bc, c, depth - 1);
}

void processInput(GLFWwindow *window, int *depth)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }

  static int prevQState = GLFW_RELEASE;
  static int prevEState = GLFW_RELEASE;
  int currQState = glfwGetKey(window, GLFW_KEY_Q);
  int currEState = glfwGetKey(window, GLFW_KEY_E);

  // Debounce Q: only increment on press transition
  if (currQState == GLFW_PRESS && prevQState == GLFW_RELEASE && (*depth) > 0) (*depth)--;
  prevQState = currQState;

  // Debounce E: only decrement on press transition
  if (currEState == GLFW_PRESS && prevEState == GLFW_RELEASE) (*depth)++;
  prevEState = currEState;
  
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}