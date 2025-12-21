#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "IndexBuffer.h"
#include "VertexBuffer.h"

static std::string ParseShader(const std::string &filePath)
{
    std::ifstream stream(filePath);
    if (!stream.is_open())
    {
        std::cerr << "❌ Failed to open shader file: " << filePath << std::endl;
        return "";
    }

    std::stringstream ss;
    ss << stream.rdbuf();
    std::string source = ss.str();

    std::cout << "Loaded shader: " << filePath << "\n";
    std::cout << "Shader length: " << source.length() << std::endl;

    return source;
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char message[length];
        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex": "fragment") << " shader" << std::endl;
        std::cerr << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string &vertexShader,
                                 const std::string &fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    if (vs == 0 || fs == 0)
    {
        std::cerr << "Shader compilation failed!" << std::endl;
        return 0;
    }

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char message[length];
        glGetProgramInfoLog(program, length, &length, message);
        std::cerr << "Shader program link failed:\n"
                  << message << std::endl;

        glDeleteProgram(program);
        return 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void debug_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
)
{
    std::cout << "---------------------opengl-callback-start------------" << std::endl;
    std::cout << "message: " << message << std::endl;
    std::cout << "type: ";
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        std::cout << "ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cout << "DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cout << "UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cout << "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cout << "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_OTHER:
        std::cout << "OTHER";
        break;
    }
    std::cout << std::endl;

    std::cout << "id: " << id << std::endl;
    std::cout << "severity: ";
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_LOW:
        std::cout << "LOW";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cout << "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        std::cout << "HIGH";
        break;
    }
    std::cout << std::endl;
    std::cout << "---------------------opengl-callback-end--------------" << std::endl;
}

void processInput(GLFWwindow* window) 
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {

    /**
     * This is the basic setup 
     */
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // This shows that we are using version 3.3 <major>.<minor>
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Rectangle", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make an OpenGL context BEFORE initializing GLAD
    glfwMakeContextCurrent(window);

    // Sync framerate with vsync
    glfwSwapInterval(1);

    // Load GLAD
    /**
     * GLAD: a library that links to OpenGL functions that are specific to GPU drivers. (see glad.c file)
     * This way we avoid manual linking and support cross-platform.
     */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // Set error handling callback function
    if (glDebugMessageCallback) {
        std::cout << "Debug callback registered" << std::endl;
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debug_callback, nullptr);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
    }


    // Square vertex positions
    float vertices[] = {
        -0.5f, -0.5f,     // bottom left
         0.5f, -0.5f,     // bottom right
        -0.5f,  0.5f,     // top left
         0.5f,  0.5f,     // top right
    };

    // Has to be unsigned
    unsigned int indices[] = {
        0, 1, 3,  // first tri
        2, 3, 0   // second tri
    };


    // Vertex array object 
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    VertexBuffer vb(vertices, 4 * 2 * sizeof(float));

    // Enable the vertex attribute
    glEnableVertexAttribArray(0);
    // Set vertex attirbute for position
    glVertexAttribPointer(
        0,                  // Index: The index in the vertex array where this attribute is located. For position in this case the index is 0.
        2,                  // Size: number of components. 2 floats for vertex position.
        GL_FLOAT,           // TYPE
        GL_FALSE,           // Normalised: If true normalizes the values to floats between -1.0 and 1.0, positions are already normalized so this is set to false.
        2 * sizeof(float),  // Stride: The amount of bytes to skip to reach the next vertex 2 floats in this case
        (void*) 0           // Pointer: The inital offset where the stride starts from
    );

    // index buffer object
    IndexBuffer ib(indices, 6);
    ib.Bind();


    std::string vertexShader = ParseShader("../res/shaders/vertex.glsl");
    std::string fragmentShader = ParseShader("../res/shaders/fragment.glsl");
    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    // get uniform ID
    int location = glGetUniformLocation(shader, "u_Color");
    
    float r = 0.0f;
    float increment = 0.05f;
    
    
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Process input
        processInput(window);
        
        // Rendering
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);
        
        // if (r > 1.0f)
        //     increment = -0.05f;
        // else if (r < 0.0f)
        //     increment = 0.05f;
        // r += increment;

        // glUniform4f(location, r, 0.3f, 0.8f, 1.0f);
        // // Use glDrawElements when using index buffer
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Check call events and swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}