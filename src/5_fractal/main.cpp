#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <array>

using namespace std;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void koch_snowflake(std::vector<float>& vertices, float x1, float y1, float x2, float y2, int depth);
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


  std::vector<float> vertices;
  int depth = 0;

  float size = 0.8f;
  float x1 = -size, y1 = -size / sqrt(3.0f);
  float x2 = size, y2 = -size / sqrt(3.0f);
  float x3 = 0.0f, y3 = 2 * size / sqrt(3.0f);

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

    koch_snowflake(vertices, x1, y1, x2, y2, depth);
    koch_snowflake(vertices, x2, y2, x3, y3, depth);
    koch_snowflake(vertices, x3, y3, x1, y1, depth);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, vertices.size() / 3);

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

void koch_snowflake(std::vector<float>& vertices, float x1, float y1, float x2, float y2, int depth) {
    if (depth == 0) {
        // Add line segment as two points (for GL_LINE_STRIP or GL_LINES)
        vertices.push_back(x1);
        vertices.push_back(y1);
        vertices.push_back(0.0f);
        vertices.push_back(x2);
        vertices.push_back(y2);
        vertices.push_back(0.0f);
        return;
    }
    float dx = x2 - x1;
    float dy = y2 - y1;
    float xA = x1 + dx / 3.0f;
    float yA = y1 + dy / 3.0f;
    float xB = x1 + 2.0f * dx / 3.0f;
    float yB = y1 + 2.0f * dy / 3.0f;
    float angle = M_PI / 3.0f; // 60 degrees
    float xC = xA + cos(angle) * (xB - xA) - sin(angle) * (yB - yA);
    float yC = yA + sin(angle) * (xB - xA) + cos(angle) * (yB - yA);

    koch_snowflake(vertices, x1, y1, xA, yA, depth - 1);
    koch_snowflake(vertices, xA, yA, xC, yC, depth - 1);
    koch_snowflake(vertices, xC, yC, xB, yB, depth - 1);
    koch_snowflake(vertices, xB, yB, x2, y2, depth - 1);
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