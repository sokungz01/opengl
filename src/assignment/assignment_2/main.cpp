#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <iostream>
#include <vector>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("6.multiple_lights.vs", "6.multiple_lights.fs");
    Shader lightCubeShader("6.light_cube.vs", "6.light_cube.fs");
    // background shader (fullscreen textured quad)
    Shader bgShader("bg.vs", "bg.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    // tentacle + body parameters
    const int NUM_TENTACLES = 8;
    const int SEGMENTS_PER_TENTACLE = 8;
    const float SEGMENT_LENGTH = 0.45f;

    // body: center position and scale
    glm::vec3 bodyCenter = glm::vec3(0.0f, -0.4f, 0.0f);
    float bodyScaleY = 1.45f;    // height
    float bodyScaleXZ = 1.8f;   // width/depth (controls tentacle radius)
    float bodyRadius = bodyScaleXZ * 0.9f;
    float bodyBottomY = bodyCenter.y - bodyScaleY * 0.5f;

    // tentacle bases arranged around the bottom rim of the body
    glm::vec3 tentacleBases[NUM_TENTACLES];
    for (int i = 0; i < NUM_TENTACLES; ++i)
    {
        float a = (float)i / (float)NUM_TENTACLES * glm::two_pi<float>();
        float r = bodyRadius * (0.9f + 0.15f * sin(a * 3.0f)); // slight variation
        tentacleBases[i] = glm::vec3(cos(a) * r, bodyBottomY - 0.05f, sin(a) * r);
    }
    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // fullscreen quad for background (two triangles)
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/textures/octopus-tentacles.png").c_str());
    unsigned int specularMap = loadTexture(FileSystem::getPath("resources/textures/octopus-tentacles-specular.png").c_str());

    // Some face images may be upside-down relative to OpenGL UV origin.
    // Flip these on load so the face appears upright on the head.
    stbi_set_flip_vertically_on_load(true);
    unsigned int faceDiffuse = loadTexture(FileSystem::getPath("resources/textures/octopus-face.png").c_str());
    unsigned int faceSpecular = loadTexture(FileSystem::getPath("resources/textures/octopus-face-specular.png").c_str());

    // background texture (deep ocean)
    unsigned int backgroundTex = loadTexture(FileSystem::getPath("resources/textures/deep-ocean.png").c_str());
    bgShader.use();
    bgShader.setInt("backgroundTex", 4);
    // restore default (don't flip other textures unless needed)
    stbi_set_flip_vertically_on_load(false);

    // shader configuration
    // --------------------
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    // bind additional samplers for face textures
    lightingShader.setInt("faceDiffuse", 2);
    lightingShader.setInt("faceSpecular", 3);
    // threshold: when dot(normal, viewDir) >= faceThreshold use face texture
    lightingShader.setFloat("faceThreshold", 0.6f);
    // default to using face textures (we'll toggle it before drawing body/tentacles vs head)
    lightingShader.setFloat("useFace", 1.0f);
    // default: don't force body-only in case shaders expect the uniform
    lightingShader.setFloat("forceUseBody", 0.0f);

    // background/top-right directional light (soft bluish rim light)
    lightingShader.setVec3("bgLight.direction", glm::vec3(-0.6f, -0.8f, -0.2f)); // coming from top-right toward center
    lightingShader.setVec3("bgLight.ambient", 0.02f, 0.03f, 0.06f);
    lightingShader.setVec3("bgLight.diffuse", 0.18f, 0.22f, 0.35f);
    lightingShader.setVec3("bgLight.specular", 0.25f, 0.3f, 0.45f);

    // rim/glow parameters (subtle emissive rim)
    lightingShader.setVec3("glowColor", glm::vec3(0.5f, 0.6f, 0.9f));
    lightingShader.setFloat("glowStrength", 0.25f);
    lightingShader.setFloat("glowPower", 2.5f);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        // draw background first (disable depth writes so background doesn't occlude the 3D scene)
        bgShader.use();
        glDepthMask(GL_FALSE);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, backgroundTex);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDepthMask(GL_TRUE);

        // then 3D scene
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);

        // per-frame fantasy animations for the octopus sculpture
        // bobbing + slow twist + pulsing scale to give it life
        glm::mat4 octopusBase = glm::mat4(1.0f);
        float bob = sin(currentFrame * 0.8f) * 0.08f; // vertical bob
        octopusBase = glm::translate(octopusBase, glm::vec3(0.0f, bob, 0.0f));
        float baseTwist = sin(currentFrame * 0.22f) * glm::radians(6.0f);
        octopusBase = glm::rotate(octopusBase, baseTwist, glm::vec3(0.0f, 1.0f, 0.0f));
        float pulse = 1.0f + 0.08f * sin(currentFrame * 2.1f);

        // animated glow (pulse) and slightly shifting glow color for fantasy look
        float glowPulse = 0.25f * (1.0f + 0.6f * sin(currentFrame * 2.0f));
        lightingShader.setFloat("glowStrength", glowPulse);
        lightingShader.setVec3("glowColor", glm::vec3(0.4f + 0.15f * sin(currentFrame * 1.3f), 0.55f, 0.8f + 0.08f * cos(currentFrame * 1.7f)));

        /*
           Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index 
           the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
           by defining light types as classes and set their values in there, or by using a more efficient uniform approach
           by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
        */
        // directional light
        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09f);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.09f);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09f);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", 0.09f);
        lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));     

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        // bind face textures
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, faceDiffuse);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, faceSpecular);

        // --- draw octopus body ---
        glBindVertexArray(cubeVAO);
        glm::mat4 bodyModel = octopusBase;
        bodyModel = glm::translate(bodyModel, bodyCenter);
        bodyModel = glm::scale(bodyModel, glm::vec3(bodyScaleXZ * pulse, bodyScaleY * pulse, bodyScaleXZ * pulse));
        lightingShader.setFloat("useFace", 0.0f); // don't apply face texture to the body
        lightingShader.setFloat("forceUseBody", 1.0f); // ensure body uses body textures only
        lightingShader.setMat4("model", bodyModel);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        float bodyTopY = bodyCenter.y + bodyScaleY * 0.5f;
        float bodyBottomY_local = bodyCenter.y - bodyScaleY * 0.5f;

        // --- draw octopus face ---
        glm::vec3 headPos = glm::vec3(bodyCenter.x, bodyTopY + 0.25f, bodyCenter.z);
        glm::vec2 headToCam = glm::normalize(glm::vec2(camera.Position.x - headPos.x, camera.Position.z - headPos.z));
        float headYaw = atan2(headToCam.y, headToCam.x);
        glm::mat4 headModel = glm::mat4(1.0f);
        headModel = octopusBase;
        headModel = glm::translate(headModel, headPos);
        headModel = glm::rotate(headModel, -headYaw + glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        headModel = glm::scale(headModel, glm::vec3(0.9f * pulse, 0.7f * pulse, 0.9f * pulse));
        lightingShader.setFloat("forceUseBody", 0.0f); // allow face texture for head
        lightingShader.setFloat("useFace", 1.0f); // enable face texture for head
        lightingShader.setMat4("model", headModel);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        lightingShader.setFloat("useFace", 0.0f);
        lightingShader.setFloat("forceUseBody", 1.0f);
        glBindVertexArray(cubeVAO);
            for (int t = 0; t < NUM_TENTACLES; ++t)
            {
                glm::mat4 baseModel = octopusBase;
                baseModel = glm::translate(baseModel, tentacleBases[t]);

                // animate using time; each tentacle has a phase offset
                float time = currentFrame;
                float tentaclePhase = t * 0.6f;

                // hierarchical build: at each segment we rotate, draw a scaled cube, then translate to the end
                glm::mat4 jointTransform = baseModel;
                for (int s = 0; s < SEGMENTS_PER_TENTACLE; ++s)
                {
                    // angle decays along the tentacle so the tip is less influential
                    float decay = 1.0f - (float)s / (float)SEGMENTS_PER_TENTACLE;
                    float yaw = sin(time * 2.0f + tentaclePhase + s * 0.25f) * glm::radians(25.0f) * decay;
                    float pitch = cos(time * 1.5f + tentaclePhase + s * 0.33f) * glm::radians(18.0f) * decay;

                    // apply rotations around local axes
                    jointTransform = glm::rotate(jointTransform, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
                    jointTransform = glm::rotate(jointTransform, pitch, glm::vec3(0.0f, 0.0f, 1.0f));

                    // taper: base segments larger, tip smaller
                    float tscale = 0.6f + 0.9f * decay; // ranges ~1.5 at base -> ~0.6 at tip
                    float radius = 0.25f * tscale; // radial scale
                    float length = SEGMENT_LENGTH * (0.9f + 0.8f * decay); // slightly longer base segments

                    // create model for this segment: translate half-length so cube centers correctly, then scale
                    glm::mat4 segmentModel = jointTransform;
                    segmentModel = glm::translate(segmentModel, glm::vec3(0.0f, length * 0.5f, 0.0f));
                    // scale Y by length and X/Z by radius to taper
                    segmentModel = glm::scale(segmentModel, glm::vec3(radius, length, radius));

                    lightingShader.setMat4("model", segmentModel);
                    glDrawArrays(GL_TRIANGLES, 0, 36);

                    // move jointTransform to the end of this segment to place the next rotation pivot
                    jointTransform = glm::translate(jointTransform, glm::vec3(0.0f, length, 0.0f));
                }
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
    }
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
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
