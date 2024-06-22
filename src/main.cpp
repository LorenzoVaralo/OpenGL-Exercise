#include "./glad.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <ostream>
#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"
#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const char* vShaderCode = 
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "\n"
        "out vec2 TexCoord;\n"
        "uniform vec3 deltaPos;\n"
        "\n"
        "void main()\n"
        "{\n"
        "	gl_Position = vec4(aPos.x + deltaPos.x, aPos.y + deltaPos.y, aPos.z + deltaPos.z, 1.0);\n"
        "	TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
        "}\n";

    const char * fShaderCode = 
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec2 TexCoord;\n"
        "uniform sampler2D img_texture;\n"
        "void main()\n"
        "{\n"
        "	FragColor = texture(img_texture, TexCoord);\n"
        "}\n";

    unsigned int ID, vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    float vertices[] = {
        // positions          // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f, // top left 

         0.0f, -0.5f, 0.0f,   1.0f, 1.0f,
         0.0f, -0.8f, 0.0f,   1.0f, 0.0f,
        -0.3f, -0.8f, 0.0f,   0.0f, 0.0f,
        -0.3f, -0.5f, 0.0f,   0.0f, 1.0f,

         0.0f, -0.3f, 0.0f,   1.0f, 1.0f,
         0.0f, -0.8f, 0.0f,   1.0f, 0.0f,
        -0.2f, -0.8f, 0.0f,   0.0f, 0.0f,
        -0.2f, -0.3f, 0.0f,   0.0f, 1.0f,

        -0.1f, -0.1f, 0.0f,   1.0f, 1.0f,
        -0.1f, -0.4f, 0.0f,   1.0f, 0.0f,
        -0.3f, -0.4f, 0.0f,   0.0f, 0.0f,
        -0.3f, -0.1f, 0.0f,   0.0f, 1.0f 
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3,

        4, 5, 7,
        5, 6, 7,

        8,  9, 11,
        9, 10, 11,

        12, 13, 15,
        13, 14, 15
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    std::string imgpatharray[] = { 
        "../field.png", 
        "../Orc.png",
        "../Orc.png",
        "../Bee.png"
    };

    int num_of_images = sizeof(imgpatharray)/sizeof(imgpatharray[0]);

    GLfloat velocity[][3] = {
        {0.0f, 0.0f, 0.0f},
        {0.001f, 0.001f, 0.0f},
        {0.002f, 0.002f, 0.0f},
        {0.0005f, 0.0001f, 0.0f},
    };

    GLfloat deltaPos[num_of_images][3];

    unsigned int textures[num_of_images];


    for(int i = 0; i < num_of_images; i++){

        deltaPos[i][0] = 0.0f;
        deltaPos[i][1] = 0.0f;
        deltaPos[i][2] = 0.0f;

        glGenTextures(1, &textures[i]);
        glBindTexture(GL_TEXTURE_2D, textures[i]); 
         // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);

        unsigned char *data = stbi_load(imgpatharray[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            if(nrChannels == 4){
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            }
            if(nrChannels == 3){
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            }
            
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }
    glUseProgram(ID);

    glUniform1i(glGetUniformLocation(ID, "img_texture"), 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for(int i = 0; i < num_of_images; i++){
            // bind textures on corresponding texture units
            glUseProgram(ID); 
            glUniform3fv(glGetUniformLocation(ID, "deltaPos"), 1, &deltaPos[i][0]);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[i]);

            // render container
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i * 6 * sizeof(unsigned int)));
            deltaPos[i][0] += velocity[i][0];
            deltaPos[i][1] += velocity[i][1];
            deltaPos[i][2] += velocity[i][2];
        }

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
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
