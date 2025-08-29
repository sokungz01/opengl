#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void onMouseMoveEvent(GLFWwindow *window);
void glGenerateBufferData(GLfloat vertices[]);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

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

struct TangramPiece {
  GLfloat vertices[9]; // 3 vertices * 3 coords
  float centroidPosX;
  float centroidPosY;
  bool selected = false;
};

int main()
{

  // Example: Define 2 triangles (for demo, you can add all 7 pieces later)
  std::vector<TangramPiece> tangramPieces = {
    { { -0.8f, -0.8f, 0.0f,   0.0f, 0.0f, 0.0f,   -0.8f, 0.0f, 0.0f }, -0.53f, -0.53f },
    { { 0.8f, -0.8f, 0.0f,   0.0f, 0.0f, 0.0f,   0.8f, 0.0f, 0.0f }, 0.53f, -0.53f },
  };

  GLuint VAO, VBO;

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


  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);


  // Mouse state
  double lastMouseX = 0, lastMouseY = 0;
  bool dragging = false;
  int selectedPiece = -1;

  // Mouse callback
  auto mouse_button_callback = [](GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);
      // Store in window user pointer for access in main loop
      double* mousePos = new double[2]{xpos, ypos};
      glfwSetWindowUserPointer(window, mousePos);
    }
  };
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  while (!glfwWindowShouldClose(window))
  {
    processInput(window);

    // Mouse drag logic
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    // Convert to OpenGL NDC
    float xNDC = (xpos / SCR_WIDTH) * 2.0f - 1.0f;
    float yNDC = 1.0f - (ypos / SCR_HEIGHT) * 2.0f;

    // On mouse press, select a piece
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !dragging) {
      for (size_t i = 0; i < tangramPieces.size(); ++i) {
        float dx = xNDC - tangramPieces[i].centroidPosX;
        float dy = yNDC - tangramPieces[i].centroidPosY;
        if (std::sqrt(dx*dx + dy*dy) < 0.3f) { // crude hit test
          selectedPiece = i;
          dragging = true;
          break;
        }
      }
    }
    // On mouse release, stop dragging
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
      dragging = false;
      selectedPiece = -1;
    }
    // If dragging, update piece position
    if (dragging && selectedPiece != -1) {
      tangramPieces[selectedPiece].centroidPosX = xNDC;
      tangramPieces[selectedPiece].centroidPosY = yNDC;
      // Update vertices (move by delta)
      // For demo, just move centroid, not actual vertices
    }

    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    for (const auto& piece : tangramPieces) {
      // For demo, just use original vertices
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(piece.vertices), piece.vertices, GL_STATIC_DRAW);
      glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    glBindVertexArray(0);

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

void onMouseMoveEvent(GLFWwindow *window){
  
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