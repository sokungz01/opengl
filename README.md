# CPE494 SPECIAL TOPIC IV: 3D GAME DEVELOPMENT USING C AND OPENGL

This project is part of the course **CPE494 SPECIAL TOPIC IV: 3D GAME DEVELOPMENT USING C AND OPENGL** at KMUTT. It demonstrates basic OpenGL rendering using C/C++ and covers foundational graphics programming concepts, including window creation, shader usage, and drawing primitives (triangle, etc.).

## How to Run on macOS

### Prerequisites

- CMake 3.3.0
- Homebrew (for installing dependencies)
- OpenGL dependencies: assimp, glm, glfw, freetype

### Step-by-step Instructions

1. **Download CMake 3.3.0**

   - [CMake 3.3.0 for macOS](https://cmake.org/files/v3.3/cmake-3.3.0-Darwin-x86_64.dmg)
   - Install required dependencies:
     ```sh
     brew install assimp glm glfw freetype
     ```

2. **Add CMake to your PATH**

   - Add the following line to your `~/.zshrc`, `~/.bashrc`, or equivalent shell config:
     ```sh
     export PATH=$PATH:/Applications/CMake.app/Contents/bin/
     ```
   - Restart your terminal or run `source ~/.zshrc` (or the appropriate file).

3. **Clone the LearnOpenGL Reference Repository**

   - This project is inspired by and references code from [LearnOpenGL](https://github.com/JoeyDeVries/LearnOpenGL/tree/master):
     ```sh
     git clone https://github.com/JoeyDeVries/LearnOpenGL.git
     ```

4. **Edit `CMakeLists.txt` for macOS Compatibility**

   - Disable the default C++ standard settings and use the following instead:
     ```cmake
     # set(CMAKE_CXX_STANDARD 17) # this does nothing for MSVC, use target_compile_options below
     # set(CMAKE_CXX_STANDARD_REQUIRED ON)
     if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
         CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
         set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
     elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
         add_definitions(/std:c++17)
     endif()
     ```

5. **[Optional] Set OS Architecture**

   - If you encounter build issues on Apple Silicon (M1/M2), add this before `project(...)` in `CMakeLists.txt`:
     ```cmake
     set(CMAKE_OSX_ARCHITECTURES "arm64")
     ```

6. **Build the Project**

   ```sh
   mkdir build && cd build
   cmake ..
   make
   ```

7. **Find the Built Files**

   - Go back to the root directory and enter the `bin` folder:
     ```sh
     cd ../bin
     ```

8. **Run the Executable**
   - Run any of the built example files:
     ```sh
     ./filename
     ```

---

## References

- [LearnOpenGL GitHub Repository](https://github.com/JoeyDeVries/LearnOpenGL/tree/master)
- [LearnOpenGL.com](https://learnopengl.com/)

---
