#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/animator.h>
#include <learnopengl/model_animation.h>

#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 640;
const unsigned int SCR_HEIGHT = 480;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// model (character) state
glm::vec3 modelPosition = glm::vec3(0.0f, -0.4f, 0.0f);
float modelYaw = 0.0f; // facing direction in radians
float walkSpeed = 1.6f; // units / second
float runSpeed = 4.0f; // units / second
bool prevLeftMouse = false;
bool prevRightMouse = false;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


enum AnimState {
	IDLE = 1,
	IDLE_PUNCH,
	PUNCH_IDLE,
	IDLE_KICK,
	KICK_IDLE,
	IDLE_WALK,
	WALK_IDLE,
	WALK,
	RUN,
	WALK_RUN,
	RUN_WALK
};

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
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader ourShader("anim_model.vs", "anim_model.fs");

	
	// load models
	// -----------
	// idle 3.3, walk 2.06, run 0.83, punch 1.03, kick 1.6
	Model ourModel(FileSystem::getPath("resources/objects/lewis/lewis.dae"));
	Animation idleAnimation(FileSystem::getPath("resources/objects/lewis/idle.dae"),&ourModel);
	Animation walkAnimation(FileSystem::getPath("resources/objects/lewis/walk.dae"), &ourModel);
	Animation runAnimation(FileSystem::getPath("resources/objects/lewis/run.dae"), &ourModel);
	Animation punchAnimation(FileSystem::getPath("resources/objects/lewis/punch.dae"), &ourModel);
	Animation kickAnimation(FileSystem::getPath("resources/objects/lewis/kick.dae"), &ourModel);
	Animator animator(&idleAnimation);
	enum AnimState charState = IDLE;
	float blendAmount = 0.0f;
	float blendRate = 0.0125f;

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input + character control
		// -------------------------
		processInput(window);

		// detect mouse clicks (edge triggered)
		bool leftPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
		bool rightPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

		// movement vector from WASD
		glm::vec3 moveDir(0.0f);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			moveDir.z -= 1.0f;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			moveDir.z += 1.0f;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			moveDir.x -= 1.0f;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			moveDir.x += 1.0f;

		bool isMoving = glm::length(moveDir) > 0.001f;
		bool isRunning = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

		// transform movement to world (no camera transform applied here)
		if (isMoving) {
			glm::vec3 dirNorm = glm::normalize(moveDir);
			float speed = isRunning ? runSpeed : walkSpeed;
			modelPosition += dirNorm * speed * deltaTime;
			// update facing yaw so model looks toward movement direction
			modelYaw = atan2(dirNorm.x, -dirNorm.z); // yaw so that forward is -Z
		}

		// Mouse click triggers (only on edge)
		if (leftPressed && !prevLeftMouse) {
			// punch
			blendAmount = 0.0f;
			animator.PlayAnimation(&idleAnimation, &punchAnimation, animator.m_CurrentTime, 0.0f, blendAmount);
			charState = IDLE_PUNCH;
		}
		if (rightPressed && !prevRightMouse) {
			// kick
			blendAmount = 0.0f;
			animator.PlayAnimation(&idleAnimation, &kickAnimation, animator.m_CurrentTime, 0.0f, blendAmount);
			charState = IDLE_KICK;
		}

		prevLeftMouse = leftPressed;
		prevRightMouse = rightPressed;

		// simple movement <-> idle <-> run state transitions with blending
		switch (charState) {
			case IDLE:
				if (isMoving) {
					// start blending idle -> walk or run
					blendAmount = 0.0f;
					animator.PlayAnimation(&idleAnimation, isRunning ? &runAnimation : &walkAnimation, animator.m_CurrentTime, 0.0f, blendAmount);
					charState = IDLE_WALK;
				}
				break;
			case IDLE_WALK:
				blendAmount += blendRate;
				blendAmount = fmod(blendAmount, 1.0f);
				animator.PlayAnimation(&idleAnimation, isRunning ? &runAnimation : &walkAnimation, animator.m_CurrentTime, animator.m_CurrentTime2, blendAmount);
				if (blendAmount > 0.9f) {
					blendAmount = 0.0f;
					float startTime = animator.m_CurrentTime2;
					animator.PlayAnimation(isRunning ? &runAnimation : &walkAnimation, NULL, startTime, 0.0f, blendAmount);
					charState = isRunning ? RUN : WALK;
				}
				break;
			case WALK:
				// remain in walk while moving, transition to idle or run as needed
				if (!isMoving) {
					// start walk -> idle blend
					blendAmount = 0.0f;
					animator.PlayAnimation(&walkAnimation, &idleAnimation, animator.m_CurrentTime, 0.0f, blendAmount);
					charState = WALK_IDLE;
				} else if (isRunning) {
					// transition walk -> run smoothly
					blendAmount = 0.0f;
					animator.PlayAnimation(&walkAnimation, &runAnimation, animator.m_CurrentTime, 0.0f, blendAmount);
					charState = WALK_RUN;
				}
				break;
			case RUN:
				// remain in run while moving and shift held, transition otherwise
				if (!isMoving) {
					// run -> idle (via walk for smoother transition)
					blendAmount = 0.0f;
					animator.PlayAnimation(&runAnimation, &idleAnimation, animator.m_CurrentTime, 0.0f, blendAmount);
					charState = WALK_IDLE;
				} else if (!isRunning) {
					// transition run -> walk smoothly
					blendAmount = 0.0f;
					animator.PlayAnimation(&runAnimation, &walkAnimation, animator.m_CurrentTime, 0.0f, blendAmount);
					charState = RUN_WALK;
				}
				break;
			case WALK_RUN:
				// blending from walk to run
				blendAmount += blendRate;
				blendAmount = fmod(blendAmount, 1.0f);
				animator.PlayAnimation(&walkAnimation, &runAnimation, animator.m_CurrentTime, animator.m_CurrentTime2, blendAmount);
				if (blendAmount > 0.9f) {
					blendAmount = 0.0f;
					float startTime = animator.m_CurrentTime2;
					animator.PlayAnimation(&runAnimation, NULL, startTime, 0.0f, blendAmount);
					charState = RUN;
				}
				break;
			case RUN_WALK:
				// blending from run to walk
				blendAmount += blendRate;
				blendAmount = fmod(blendAmount, 1.0f);
				animator.PlayAnimation(&runAnimation, &walkAnimation, animator.m_CurrentTime, animator.m_CurrentTime2, blendAmount);
				if (blendAmount > 0.9f) {
					blendAmount = 0.0f;
					float startTime = animator.m_CurrentTime2;
					animator.PlayAnimation(&walkAnimation, NULL, startTime, 0.0f, blendAmount);
					charState = WALK;
				}
				break;
			case WALK_IDLE:
				blendAmount += blendRate;
				blendAmount = fmod(blendAmount, 1.0f);
				animator.PlayAnimation(&walkAnimation, &idleAnimation, animator.m_CurrentTime, animator.m_CurrentTime2, blendAmount);
				if (blendAmount > 0.9f) {
					blendAmount = 0.0f;
					float startTime = animator.m_CurrentTime2;
					animator.PlayAnimation(&idleAnimation, NULL, startTime, 0.0f, blendAmount);
					charState = IDLE;
				}
				break;
			case IDLE_PUNCH:
				blendAmount += blendRate;
				blendAmount = fmod(blendAmount, 1.0f);
				animator.PlayAnimation(&idleAnimation, &punchAnimation, animator.m_CurrentTime, animator.m_CurrentTime2, blendAmount);
				if (blendAmount > 0.9f) {
					blendAmount = 0.0f;
					float startTime = animator.m_CurrentTime2;
					animator.PlayAnimation(&punchAnimation, NULL, startTime, 0.0f, blendAmount);
					charState = PUNCH_IDLE;
				}
				break;
			case PUNCH_IDLE:
				if (animator.m_CurrentTime > 0.7f) {
					blendAmount += blendRate;
					blendAmount = fmod(blendAmount, 1.0f);
					animator.PlayAnimation(&punchAnimation, &idleAnimation, animator.m_CurrentTime, animator.m_CurrentTime2, blendAmount);
					if (blendAmount > 0.9f) {
						blendAmount = 0.0f;
						float startTime = animator.m_CurrentTime2;
						animator.PlayAnimation(&idleAnimation, NULL, startTime, 0.0f, blendAmount);
						charState = IDLE;
					}
				}
				break;
			case IDLE_KICK:
				blendAmount += blendRate;
				blendAmount = fmod(blendAmount, 1.0f);
				animator.PlayAnimation(&idleAnimation, &kickAnimation, animator.m_CurrentTime, animator.m_CurrentTime2, blendAmount);
				if (blendAmount > 0.9f) {
					blendAmount = 0.0f;
					float startTime = animator.m_CurrentTime2;
					animator.PlayAnimation(&kickAnimation, NULL, startTime, 0.0f, blendAmount);
					charState = KICK_IDLE;
				}
				break;
			case KICK_IDLE:
				if (animator.m_CurrentTime > 1.0f) {
					blendAmount += blendRate;
					blendAmount = fmod(blendAmount, 1.0f);
					animator.PlayAnimation(&kickAnimation, &idleAnimation, animator.m_CurrentTime, animator.m_CurrentTime2, blendAmount);
					if (blendAmount > 0.9f) {
						blendAmount = 0.0f;
						float startTime = animator.m_CurrentTime2;
						animator.PlayAnimation(&idleAnimation, NULL, startTime, 0.0f, blendAmount);
						charState = IDLE;
					}
				}
				break;
			default:
				break;
		}




		animator.UpdateAnimation(deltaTime);
		
		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		ourShader.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		// third-person follow camera: compute camera position relative to model
		glm::vec3 camOffsetLocal = glm::vec3(0.0f, 1.5f, 3.0f);
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), modelYaw, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec3 camOffset = glm::vec3(rot * glm::vec4(camOffsetLocal, 1.0f));
		glm::vec3 camPos = modelPosition + camOffset;
		glm::mat4 view = glm::lookAt(camPos, modelPosition + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		// update camera struct so zoom & mouse still operate reasonably
		camera.Position = camPos;
		camera.Front = glm::normalize(modelPosition - camPos);
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

        auto transforms = animator.GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i)
			ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);


		// render the loaded model at the character position and rotation
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, modelPosition);
		model = glm::rotate(model, modelYaw, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(.5f, .5f, .5f));	// scale down
		ourShader.setMat4("model", model);
		ourModel.Draw(ourShader);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
