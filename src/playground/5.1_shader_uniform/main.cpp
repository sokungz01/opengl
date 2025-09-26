#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/shader_s.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, unsigned int shaderObjectId);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(ourShader.ID);
        float redValue = (cos(timeValue) + 1.0f) / 2.0f;
        float greenValue = (sin(timeValue) + 1.0f) / 2.0f;
        float blueValue = ((sin(timeValue) * -1) + 1.0f) / 2.0f;
        glUniform4f(glGetUniformLocation(ourShader.ID, "ourUniformColor"), redValue, greenValue, blueValue, 1.0f);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window, unsigned int shaderObjectId)
{
    static int prevQ = GLFW_RELEASE, prevE = GLFW_RELEASE, prevW = GLFW_RELEASE, prevS = GLFW_RELEASE;
    int currQ = glfwGetKey(window, GLFW_KEY_Q);
    int currE = glfwGetKey(window, GLFW_KEY_E);
    int currW = glfwGetKey(window, GLFW_KEY_W);
    int currS = glfwGetKey(window, GLFW_KEY_S);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (currQ == GLFW_PRESS && prevQ == GLFW_RELEASE) glUniform1i(glGetUniformLocation(shaderObjectId, "isDynamicColor"), GL_TRUE);
    prevQ = currQ;
    if (currE == GLFW_PRESS && prevE == GLFW_RELEASE) glUniform1i(glGetUniformLocation(shaderObjectId, "isDynamicColor"), GL_FALSE);
    prevE = currE;
    if (currW == GLFW_PRESS && prevW == GLFW_RELEASE) glUniform1i(glGetUniformLocation(shaderObjectId, "isUpside"), GL_TRUE);
    prevW = currW;
    if (currS == GLFW_PRESS && prevS == GLFW_RELEASE) glUniform1i(glGetUniformLocation(shaderObjectId, "isUpside"), GL_FALSE);
    prevS = currS;

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}