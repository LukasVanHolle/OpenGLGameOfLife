#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    
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
    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    // Buffer address is given to genbuffers which will be filled
    unsigned int buffer;
    // Every object in OpenGL needs an ID. In this case 1 is the ID for the vertex buffer
    //          ID  Buffer address
    glGenBuffers(1, &buffer);
    // Select buffer with ID 1
    glBindBuffer(GL_ARRAY_BUFFER, 1);
    // Creates the buffers data store    Positions is an array so it's already a pointer.
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
    // Set vertex attirbute for position
    glVertexAttribPointer(
        0,                  // Index: The index in the vertex array where this attribute is located. For position in this case the index is 0.
        2,                  // Size: number of components. 2 floats for vertex position.
        GL_FLOAT,           // TYPE
        GL_FALSE,           // Normalised: If true normalizes the values to floats between -1.0 and 1.0, positions are already normalized so this is set to false.
        2 * sizeof(float),  // Stride: The amount of bytes to skip to reach the next vertex 2 floats in this case
        (void*) 0           // Pointer: The inital offset where the stride starts from
    );
    // Enable the vertex attribute
    glEnableVertexAttribArray(0);
    

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

    glfwTerminate();
    return 0;
}

