#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/shader_s.h>
#include <iostream>
#include <array>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, unsigned int shaderObjectId);
std::array<float, 6> midpoint(const std::array<float, 6>& a, const std::array<float, 6>& b);
void sierpinski(std::vector<float>& vertices, const std::array<float, 6>& a, const std::array<float, 6>& b, const std::array<float, 6>& c, int depth);
std::array<float, 6> rotate_vertex(const std::array<float, 6>& v, float angle_rad);
std::array<float, 6> rotate_vertex(const std::array<float, 6>& v, float angle_rad);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int depth = 1;
bool isDynamicColor = false;
bool isUpside = false;
bool isRotate = false;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader ourShader("shader.vs", "shader.fs");

    std::array<float, 6> v0 = {-0.8f, -0.7f, 0.0f, 1.0f, 0.0f, 0.0f,};
    std::array<float, 6> v1 = {0.8f, -0.7f, 0.0f, 0.0f, 1.0f, 0.0f};
    std::array<float, 6> v2 = {0.0f, 0.9f, 0.0f, 0.0f, 0.0f, 1.0f};
    std::vector<float> vertices;

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // position attribute layout (location = 0)
    //(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) *pointer is offset
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // color attribute layout (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUniform1i(glGetUniformLocation(ourShader.ID, "isDynamicColor"), GL_FALSE);
    glUniform1i(glGetUniformLocation(ourShader.ID, "isUpside"), GL_FALSE);


    while (!glfwWindowShouldClose(window))
    {
        float timeValue = glfwGetTime();
        ourShader.use();
        processInput(window, ourShader.ID);
        vertices.clear();
        // Prepare triangle vertices, apply rotation if needed
        std::array<float, 6> rv0 = v0, rv1 = v1, rv2 = v2;
        if (isRotate) {
            float angle = timeValue; // radians
            rv0 = rotate_vertex(v0, angle);
            rv1 = rotate_vertex(v1, angle);
            rv2 = rotate_vertex(v2, angle);
        }

        sierpinski(vertices, rv0, rv1, rv2, depth);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(ourShader.ID);
        float redValue = (cos(depth * timeValue) + 1.0f) / 2.0f;
        float greenValue = (sin(depth * timeValue) + 1.0f) / 2.0f;
        float blueValue = ((sin(depth * timeValue) * -1) + 1.0f) / 2.0f;
        glUniform4f(glGetUniformLocation(ourShader.ID, "ourUniformColor"), redValue, greenValue, blueValue, 1.0f);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 6);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

std::array<float, 6> rotate_vertex(const std::array<float, 6>& v, float angle_rad) {
  float c = cos(angle_rad);
  float s = sin(angle_rad);
  float x = v[0] * c - v[1] * s;
  float y = v[0] * s + v[1] * c;
  return {x, y, v[2], v[3], v[4], v[5]};
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, unsigned int shaderObjectId)
{
  static int prevQ = GLFW_RELEASE, prevW = GLFW_RELEASE, prevA = GLFW_RELEASE, prevD = GLFW_RELEASE, prevE = GLFW_RELEASE;
  int currQ = glfwGetKey(window, GLFW_KEY_Q);
  int currW = glfwGetKey(window, GLFW_KEY_W);
  int currA = glfwGetKey(window, GLFW_KEY_A);
  int currD = glfwGetKey(window, GLFW_KEY_D);
  int currE = glfwGetKey(window, GLFW_KEY_E);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (currQ == GLFW_PRESS && prevQ == GLFW_RELEASE) {
        isDynamicColor = !isDynamicColor;
        glUniform1i(glGetUniformLocation(shaderObjectId, "isDynamicColor"), isDynamicColor ? GL_TRUE : GL_FALSE);
    }
    if (currW == GLFW_PRESS && prevW == GLFW_RELEASE) {
        isUpside = !isUpside;
        glUniform1i(glGetUniformLocation(shaderObjectId, "isUpside"), isUpside ? GL_TRUE : GL_FALSE);
    }
    if (currE == GLFW_PRESS && prevE == GLFW_RELEASE) {
        isRotate = !isRotate;
    }

    if (currA == GLFW_PRESS && prevA == GLFW_RELEASE && depth > 1) depth--;
    if (currD == GLFW_PRESS && prevD == GLFW_RELEASE) depth++;
    
    prevQ = currQ;
    prevW = currW;
    prevE = currE;
    prevA = currA;
    prevD = currD;
}

std::array<float, 6> midpoint(const std::array<float, 6>& a, const std::array<float, 6>& b) {
  return {
    (a[0] + b[0]) / 2.0f, // x
    (a[1] + b[1]) / 2.0f, // y
    (a[2] + b[2]) / 2.0f, // z
    (a[3] + b[3]) / 2.0f, // r
    (a[4] + b[4]) / 2.0f, // g
    (a[5] + b[5]) / 2.0f  // b
  };
}

void sierpinski(std::vector<float>& vertices, const std::array<float, 6>& a, const std::array<float, 6>& b, const std::array<float, 6>& c, int depth) {
  if (depth == 1) {
    vertices.insert(vertices.end(), {
      a[0], a[1], a[2], a[3], a[4], a[5],
      b[0], b[1], b[2], b[3], b[4], b[5],
      c[0], c[1], c[2], c[3], c[4], c[5]
    });
    return;
  }
  std::array<float, 6> ab = midpoint(a, b);
  std::array<float, 6> bc = midpoint(b, c);
  std::array<float, 6> ca = midpoint(c, a);
  sierpinski(vertices, a, ab, ca, depth - 1);
  sierpinski(vertices, ab, b, bc, depth - 1);
  sierpinski(vertices, ca, bc, c, depth - 1);
}