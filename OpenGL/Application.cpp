#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>
#include <tuple>

#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

int frames{ 0 };
int frameiter{ 0 };
float times = 0.0f;
float x{ 0.0f };
float y{ 0.0f };
float zoom{ 1.0 };
int color = { 0 };
int max_iterations = { 100 };

static void GLClearError() 
{
    while (glGetError() != GL_NO_ERROR);
}

void countFPS()
{
    double current_time = glfwGetTime();
    frames++;
    if (current_time - times >= 1.0)
    {
        std::cout << frames << " fps\n";
        frames = 0;
        times = times + 1.0;
    }
}

static bool GLLogCall(const char* function, const char* file, int line) 
{
    if (GLenum error = glGetError())
    {
        std::cout << "[OpenGl_Error] (" << error << ")" << function << " " << file << ":" << line <<std::endl;
        return false;
    }
    return true;
}

void process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)color = 0;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)color = 1;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)color = 2;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)color = 3;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)max_iterations = 3;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)max_iterations = 10;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)max_iterations = 20;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)max_iterations = 100;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)max_iterations = 1000;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)++max_iterations;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    {
        --max_iterations;
        if (max_iterations <= 0)max_iterations = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        ++frameiter;
        if (frameiter>25)
        {
            ++max_iterations;
            frameiter -= 25;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    {
        ++frameiter;
        if (frameiter > 25)
        {
            --max_iterations;
            if (max_iterations <= 0)max_iterations = 0;
            frameiter -= 25;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_UP)|| glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        y = y + 0.008f * zoom;
        if (y > 0.6f) y = 0.6f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        y = y - 0.008f * zoom;
        if (y < -0.6f) y = -0.6f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        x = x - 0.008f * zoom;
        if (x < -1.0f) x = -1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        x = x + 0.008f * zoom;
        if (x > 0.5f) x = 0.5f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        zoom = zoom * 1.02f;
        if (zoom > 1.0f) zoom = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)zoom = zoom * 0.98f;
}

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepathvertex, const std::string& filepathfragment)
{
    std::ifstream streamvertex(filepathvertex);
    std::ifstream streamfragment(filepathfragment);

    enum class ShaderType 
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(streamvertex, line)) 
    {
        type = ShaderType::VERTEX;
        ss[(int)type] << line << '\n';
    }
    while (getline(streamfragment, line))
    {
        type = ShaderType::FRAGMENT;
        ss[(int)type] << line << '\n';
    }
    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) // error checking
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*) alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
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

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs)); // dit kan omdat het al in program is gelinkt
    GLCall(glDeleteShader(fs));

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1400, 1400, "Mandelbrot", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); //limits framerate to monitor refreshrate

    if (glewInit() != GLEW_OK) {
        std::cout << "error" << std::endl;
    }
    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[] = {
     -1.0f,  -1.0f,
      1.0f,   1.0f,
     -1.0f,   1.0f,
      1.0f,  -1.0f,
    };

    unsigned int indices[] =
    {
        //  2---,1
        //  | .' |
        //  0'---3
        0, 1, 2,
        0, 3, 1
    };
    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

    ShaderProgramSource source = ParseShader("res/shaders/BasicVertex.shader", "res/shaders/BasicMandelbrotfrag.shader");
    //std::cout << source.VertexSource << std::endl;
    //std::cout << source.FragmentSource << std::endl;
    /*was om te testen of hij de bestanden goed las*/

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(shader));

    GLCall(int zoomlocation = glGetUniformLocation(shader, "zoom"));
    GLCall(int xlocation = glGetUniformLocation(shader, "x"));
    GLCall(int ylocation = glGetUniformLocation(shader, "y"));
    GLCall(int colorlocation = glGetUniformLocation(shader, "color"));
    GLCall(int maxiterlocation = glGetUniformLocation(shader, "maximum_iterations"));
    //ASSERT(location != -1);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        countFPS();

        process_input(window);

        GLCall(glUniform1i(colorlocation, color));
        GLCall(glUniform1i(maxiterlocation, max_iterations));
        GLCall(glUniform1f(zoomlocation, zoom));
        GLCall(glUniform1f(xlocation, x));
        GLCall(glUniform1f(ylocation, y));

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        /* Swap front and back buffers */
        GLCall(glfwSwapBuffers(window));

        /* Poll for and process events */
        GLCall(glfwPollEvents());
    }
    GLCall(glDeleteProgram(shader));

    glfwTerminate();
    return 0;
}