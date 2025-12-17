#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

static std::string ParseShader(const std::string& filePath) 
{
    std::ifstream stream(filePath);
    std::stringstream ss;
    ss << stream.rdbuf();
    return ss.str();
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

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // attach shaders to program
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    // Delete shaders after they have been linked
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
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

    GLFWwindow* window = glfwCreateWindow(800, 600, "Triangle", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make an OpenGL context BEFORE initializing GLAD
    glfwMakeContextCurrent(window);

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


    // Triangle vertex positions kinda like a buffer
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
         0.0f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f
    };

    // Vertex array object 
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    // Buffer address is given to genbuffers which will be filled
    unsigned int vbo;
    // Every object in OpenGL needs an ID. In this case 1 is the ID for the vertex buffer
    //          ID  Buffer address
    glGenBuffers(1, &vbo);
    // Select buffer with ID 1
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Creates the buffers data store    Positions is an array so it's already a pointer.
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), vertices, GL_STATIC_DRAW);
    // Set vertex attirbute for position
    glVertexAttribPointer(
        0,                  // Index: The index in the vertex array where this attribute is located. For position in this case the index is 0.
        3,                  // Size: number of components. 2 floats for vertex position.
        GL_FLOAT,           // TYPE
        GL_FALSE,           // Normalised: If true normalizes the values to floats between -1.0 and 1.0, positions are already normalized so this is set to false.
        6 * sizeof(float),  // Stride: The amount of bytes to skip to reach the next vertex 2 floats in this case
        (void*) 0           // Pointer: The inital offset where the stride starts from
    );

    // Set vertex attribute for color
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*) (3 * sizeof(float))
    );
    // Enable the vertex attribute
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);


    std::string vertexShader = ParseShader("../res/shaders/vertex.vert");
    std::string fragmentShader = ParseShader("../res/shaders/fragment.frag");
    
    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);
    

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Process input
        processInput(window);

        // Rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Check call events and swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}

