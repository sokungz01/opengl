#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_s.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <string>

// Function prototypes
glm::vec3 getBarColor(int i);
void renderBars(Shader &shader, unsigned int VAO, float barWidth);
void updateSortingStep();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void resetArray();
void stepInsertion();
void stepSelection();
void stepBubble();
void stepMerge();
void stepQuick();
void resetQuickSort();
void startArrayTransition();
void finishArrayTransition();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Sorting visualization settings
const int NUM_BARS = 50;
std::vector<int> arr(NUM_BARS);
std::vector<int> aux(NUM_BARS); // for merge sort

// Transition / animation state when switching sort types
bool transitioning = false;
float transitionStart = 0.0f;
float transitionDuration = 0.6f; // seconds
std::vector<int> sourceArr(NUM_BARS);
std::vector<int> targetArr(NUM_BARS);

enum SortType { INSERTION, SELECTION, BUBBLE, MERGE, QUICK };
SortType currentSort = INSERTION;

const char* sortName(SortType s) {
    switch (s) {
        case INSERTION: return "Insertion";
        case SELECTION: return "Selection";
        case BUBBLE:    return "Bubble";
        case MERGE:     return "Merge";
        case QUICK:     return "Quick";
        default:        return "Unknown";
    }
}

// State for all sorts
bool sorting = false; // don't auto-start until user clicks Start
bool started = false; // whether user started the initial sort
float stepDelay = 0.0f;
float lastStepTime = 0.0f;

// UI: Start button in normalized device coords
const float BTN_W = 0.25f; // width
const float BTN_H = 0.10f; // height
const float BTN_X = -BTN_W/2.0f; // centered
const float BTN_Y = -0.9f; // near bottom

bool isCursorInsideButton(GLFWwindow* window) {
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    int w, h;
    glfwGetWindowSize(window, &w, &h);
    // convert pixel coords to NDC (-1..1)
    float nx = (float)(mx / w) * 2.0f - 1.0f;
    float ny = 1.0f - (float)(my / h) * 2.0f;
    return nx >= BTN_X && nx <= (BTN_X + BTN_W) && ny >= BTN_Y && ny <= (BTN_Y + BTN_H);
}

int ins_current = 1;
int ins_j = 0;
int ins_key = 0;
bool ins_in_progress = false;

int sel_i = 0, sel_j = 1, sel_min = 0;

int bub_i = 0, bub_j = 0;
bool bub_swapped = false;

struct MergeFrame {
    int left, mid, right, i, j, k, phase;
    std::vector<int> temp;
};
std::vector<MergeFrame> mergeStack;
bool merge_inited = false;

struct QuickFrame {
    int left, right;      
    int pivotIndex;       
    int i, j;             
    int phase;            
    int pivotValue;       
};
std::vector<QuickFrame> quickStack;
bool quick_inited = false;


int main()
{
    // Initialize array with random values
    std::iota(arr.begin(), arr.end(), 1);
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(arr.begin(), arr.end(), std::default_random_engine(seed));

    // ensure QuickSort internal stack/state is initialized so the first run works
    resetQuickSort();

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // glfw window creation
    std::string title = std::string("Sort Visualization - ") + sortName(currentSort);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title.c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    Shader ourShader("transform.vs", "transform.fs");

    // set up vertex data for a single bar (rectangle)
    float barWidth = 2.0f / NUM_BARS * 0.9f; // 90% of slot width
    float vertices[] = {
        // positions         // no texture coords
        0.0f, 1.0f, 0.0f,   // top left
        1.0f, 1.0f, 0.0f,   // top right
        1.0f, 0.0f, 0.0f,   // bottom right
        0.0f, 0.0f, 0.0f    // bottom left
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    float startTime = (float)glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (int key = GLFW_KEY_1; key <= GLFW_KEY_5; ++key) {
            if (glfwGetKey(window, key) == GLFW_PRESS) {
                SortType newSort = (SortType)(key - GLFW_KEY_1);
                if (currentSort != newSort) {
                        // start a smooth transition between current array and a freshly shuffled target
                        currentSort = newSort;
                        startArrayTransition();
                        std::string newTitle = std::string("Sort Visualization - ") + sortName(currentSort);
                        glfwSetWindowTitle(window, newTitle.c_str());
                }
            }
        }
        updateSortingStep();
        renderBars(ourShader, VAO, barWidth);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (!started && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        started = true;
        sorting = true;
        lastStepTime = (float)glfwGetTime();
    }

    if (!started && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (isCursorInsideButton(window)) {
            started = true;
            sorting = true;
            lastStepTime = (float)glfwGetTime();
        }
    }
}

// mouse callback to handle clicking the on-screen Start button
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !started) {
        if (isCursorInsideButton(window)) {
            started = true;
            sorting = true;
            lastStepTime = (float)glfwGetTime();
        }
    }
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

glm::vec3 getBarColor(int i) {
    bool highlight = false;
    bool compareA = false, compareB = false;
    bool currentIdx = false;

    switch (currentSort) {
        case INSERTION:
            if (ins_in_progress) {
                currentIdx = (i == ins_j + 1);
                compareA = (i == ins_j);
            } else {
                currentIdx = sorting && (i == ins_current);
            }
            break;

        case SELECTION:
            currentIdx = sorting && (i == sel_i);
            compareA = sorting && (i == sel_min);
            compareB = sorting && (i == sel_j);
            break;

        case BUBBLE:
            currentIdx = sorting && (i == bub_i);
            compareA = sorting && (i == bub_j);
            compareB = sorting && (i == bub_j + 1);
            break;

        case MERGE:
            if (merge_inited && !mergeStack.empty()) {
                MergeFrame &f = mergeStack.front();
                if (f.i <= f.mid && f.j <= f.right) {
                    compareA = (i == f.i);
                    compareB = (i == f.j);
                }
                highlight = (i >= f.left && i <= f.right);
            }
            break;

        case QUICK:
            if (quick_inited && !quickStack.empty()) {
                QuickFrame &f = quickStack.back();
                highlight = (i >= f.left && i <= f.right);
                if (f.phase == 1 && i == f.j) compareA = true;
                if (f.phase == 2 && i == f.i) compareB = true;
            }
            break;
    }

    if (currentIdx)   return glm::vec3(1.0f, 1.0f, 0.0f); // yellow = current index
    if (compareA)     return glm::vec3(1.0f, 0.2f, 0.2f); // red = comparator A
    if (compareB)     return glm::vec3(0.2f, 1.0f, 0.2f); // green = comparator B
    if (highlight)    return glm::vec3(1.0f, 0.5f, 0.2f); // orange = active subarray
    return glm::vec3(0.2f, 0.7f, 1.0f);                   // blue = normal
}

void renderBars(Shader &shader, unsigned int VAO, float barWidth) {
    shader.use();
    glBindVertexArray(VAO);

    for (int i = 0; i < NUM_BARS; ++i) {
        glm::mat4 model = glm::mat4(1.0f);
        float xpos = -1.0f + i * (2.0f / NUM_BARS);

        float height;
        if (transitioning) {
            float now = (float)glfwGetTime();
            float t = (now - transitionStart) / transitionDuration;
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;
            // smoothstep ease
            t = t * t * (3.0f - 2.0f * t);
            float s = (float)sourceArr[i] / (float)NUM_BARS;
            float d = (float)targetArr[i] / (float)NUM_BARS;
            height = s * (1.0f - t) + d * t;
        } else {
            if (currentSort == INSERTION && ins_in_progress && i == ins_j + 1) {
                height = (float)ins_key / (float)NUM_BARS;
            } else {
                height = (float)arr[i] / (float)NUM_BARS;
            }
        }

        model = glm::translate(model, glm::vec3(xpos, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(barWidth, height * 2.0f, 1.0f));

        unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));

        glm::vec3 color = getBarColor(i);
        int colorLoc = glGetUniformLocation(shader.ID, "ourColor");
        glUniform3f(colorLoc, color.r, color.g, color.b);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    if (!started) {
        // button color: hover or normal
        glm::vec3 btnColor = glm::vec3(0.2f, 0.8f, 0.2f);
        GLFWwindow* win = glfwGetCurrentContext();
        if (isCursorInsideButton(win)) btnColor = glm::vec3(0.1f, 0.6f, 0.1f);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(BTN_X, BTN_Y, 0.0f));
        model = glm::scale(model, glm::vec3(BTN_W, BTN_H, 1.0f));
        unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));
        int colorLoc = glGetUniformLocation(shader.ID, "ourColor");
        glUniform3f(colorLoc, btnColor.r, btnColor.g, btnColor.b);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}


void updateSortingStep() {
    float now = (float)glfwGetTime();
    // don't step sorting while we are transitioning between sorts
    if (transitioning) {
        // finish transition after duration
        if (now - transitionStart >= transitionDuration) finishArrayTransition();
        return;
    }

    if (sorting && now - lastStepTime > stepDelay) {
        switch (currentSort) {
            case INSERTION: stepInsertion(); break;
            case SELECTION: stepSelection(); break;
            case BUBBLE:    stepBubble(); break;
            case MERGE:     stepMerge(); break;
            case QUICK:     stepQuick(); break;
        }
        lastStepTime = now;
    }
}

void resetArray() {
    // initialize arr to ordered values and mark sorting active
    std::iota(arr.begin(), arr.end(), 1);
    // keep sorting state as-is; caller decides when to start
    ins_current = 1;
    ins_j = 0;
    ins_key = 0;
    ins_in_progress = false;
    sel_i = 0; sel_j = 1; sel_min = 0;
    bub_i = 0; bub_j = 0; bub_swapped = false;
    mergeStack.clear(); merge_inited = false;
    quickStack.clear(); quick_inited = false;
}

void startArrayTransition() {
    // capture current array as source
    sourceArr = arr;
    // build a new shuffled target
    std::iota(targetArr.begin(), targetArr.end(), 1);
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(targetArr.begin(), targetArr.end(), std::default_random_engine(seed));

    // stop current sorting and mark transitioning
    sorting = false;
    transitioning = true;
    transitionStart = (float)glfwGetTime();
}

void finishArrayTransition() {
    arr = targetArr;        
    transitioning = false;

    ins_current = 1; ins_j = 0; ins_key = 0; ins_in_progress = false;
    sel_i = 0; sel_j = 1; sel_min = 0;
    bub_i = 0; bub_j = 0; bub_swapped = false;
    mergeStack.clear(); merge_inited = false;

    resetQuickSort();

    sorting = started;
    lastStepTime = (float)glfwGetTime();
}

void stepInsertion() {
    if (!ins_in_progress) {
        if (ins_current >= NUM_BARS) {
            sorting = false;
            return;
        }
        ins_key = arr[ins_current];
        ins_j = ins_current - 1;
        ins_in_progress = true;
        return;
    }
    if (ins_j >= 0 && arr[ins_j] > ins_key) {
        arr[ins_j + 1] = arr[ins_j];
        ins_j--;
    } else {
        arr[ins_j + 1] = ins_key;
        ins_current++;          
        ins_in_progress = false;
    }
}


void stepSelection() {
    if (sel_i < NUM_BARS - 1) {
        if (sel_j == sel_i + 1) sel_min = sel_i;
        if (sel_j < NUM_BARS) {
            if (arr[sel_j] < arr[sel_min]) sel_min = sel_j;
            sel_j++;
        } else {
            std::swap(arr[sel_i], arr[sel_min]);
            sel_i++;
            sel_j = sel_i + 1;
        }
    } else {
        sorting = false;
    }
}

void stepBubble() {
    if (bub_i < NUM_BARS - 1) {
        if (bub_j < NUM_BARS - bub_i - 1) {
            if (arr[bub_j] > arr[bub_j + 1]) {
                std::swap(arr[bub_j], arr[bub_j + 1]);
            }
            bub_j++;
        } else {
            bub_j = 0;
            bub_i++;
        }
    } else {
        sorting = false;
    }
}

void stepMerge() {
    if (!merge_inited) {
        mergeStack.clear();
        for (int size = 1; size < NUM_BARS; size *= 2) {
            for (int left = 0; left < NUM_BARS - size; left += 2 * size) {
                int mid = left + size - 1;
                int right = std::min(left + 2 * size - 1, NUM_BARS - 1);
                mergeStack.push_back({left, mid, right, left, mid + 1, left, 0, std::vector<int>(right - left + 1)});
            }
        }
        merge_inited = true;
    }
    if (!mergeStack.empty()) {
        MergeFrame &f = mergeStack.front();
        if (f.i <= f.mid && f.j <= f.right) {
            if (arr[f.i] <= arr[f.j]) {
                f.temp[f.k - f.left] = arr[f.i];
                f.i++;
            } else {
                f.temp[f.k - f.left] = arr[f.j];
                f.j++;
            }
            f.k++;
        } else if (f.i <= f.mid) {
            f.temp[f.k - f.left] = arr[f.i];
            f.i++; f.k++;
        } else if (f.j <= f.right) {
            f.temp[f.k - f.left] = arr[f.j];
            f.j++; f.k++;
        } else {
            for (int t = 0; t < f.temp.size(); ++t) arr[f.left + t] = f.temp[t];
            mergeStack.erase(mergeStack.begin());
        }
    } else {
        sorting = false;
    }
}

void resetQuickSort() {
    quickStack.clear();

    if (NUM_BARS == 0) return;

    QuickFrame frame;
    frame.left = 0;
    frame.right = NUM_BARS - 1;
    frame.phase = 0; // initialize
    quickStack.push_back(frame);
    
    quick_inited = true;
}

void stepQuick() {
    if (!quick_inited || quickStack.empty()) {
        sorting = false;
        return;
    }

    QuickFrame &f = quickStack.back();

    switch (f.phase) {
        case 0: { // Initialize and find pivot
            if (f.left >= f.right) {
                // This sub-array is already sorted (0 or 1 elements), so we're done with it.
                quickStack.pop_back();
                break;
            }
            // Using middle element as pivot is often more robust than f.left
            int mid = f.left + (f.right - f.left) / 2;
            std::swap(arr[mid], arr[f.right]); // Move pivot to the end
            f.pivotValue = arr[f.right];
            f.i = f.left - 1; // i is the index of the last element smaller than pivot
            f.j = f.left;
            f.phase = 1; // Move to partitioning phase
            break;
        }
        case 1: { // Partitioning loop
            if (f.j < f.right) { // Iterate up to the pivot
                if (arr[f.j] < f.pivotValue) {
                    f.i++;
                    std::swap(arr[f.i], arr[f.j]);
                }
                f.j++;
            } else {
                f.phase = 2; // Partitioning is done, move to next phase
            }
            break;
        }
        case 2: { // Finalize partition and create new sub-problems
            // Swap pivot into its final sorted position
            std::swap(arr[f.i + 1], arr[f.right]);
            int pi = f.i + 1; // The partition index

            // IMPORTANT: Capture the bounds from the current frame
            // because 'f' will become invalid after we pop.
            int left = f.left;
            int right = f.right;

            // Pop the completed frame. This is the critical fix.
            quickStack.pop_back();

            // Now, push the two new, smaller sub-problems onto the stack.
            // Pushing the right sub-array first is a common practice.
            if (pi + 1 < right) {
                QuickFrame rightFrame = {pi + 1, right, 0, 0, 0, 0, 0};
                quickStack.push_back(rightFrame);
            }

            if (left < pi - 1) {
                QuickFrame leftFrame = {left, pi - 1, 0, 0, 0, 0, 0};
                quickStack.push_back(leftFrame);
            }
            break; // Break from the switch
        }
        // Phase 3 is no longer needed
    }
}