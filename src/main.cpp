#include "./glad.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <ostream>
#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"
#include <chrono>
#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, unsigned int ID, float movement_speed);
unsigned int textures[4];

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
std::string imgpatharray[] = { 
    "../field.png", 
    "../imgs/1/D_Walk.png",
    "../imgs/2/D_Walk.png",
    "../imgs/4/D_Walk.png"
};

std::string movement_images[] = { 
    "../imgs/4/U_Walk.png",
    "../imgs/4/D_Walk.png",
    "../imgs/4/Left_Walk.png",
    "../imgs/4/Right_Walk.png"
};

GLfloat deltaPos[4][3];
char last_movement_direction = 'w';

void checkCompileErrors(unsigned int shader, std::string type){
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

void load_images(unsigned int ID){
    int num_of_images = 4;
    for(int i = 0; i < num_of_images; i++){


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

}
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
        "uniform int numSprites;\n"
        "uniform int spriteStep;\n"
        "\n"
        "void main()\n"
        "{\n"
        "	float fSpriteStep = float(spriteStep);\n"
        "	gl_Position = vec4(aPos.x + deltaPos.x, aPos.y + deltaPos.y, aPos.z + deltaPos.z, 1.0);\n"
        "	TexCoord = vec2((aTexCoord.x + fSpriteStep)/numSprites, aTexCoord.y);\n"
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
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

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


    
    int numOfSprites[] = {
        1,
        6,
        6,
        6
    };

    int num_of_images = sizeof(imgpatharray)/sizeof(imgpatharray[0]);

    GLfloat velocity[][3] = {
        {0.0f, 0.0f, 0.0f},
        {0.001f, 0.001f, 0.0f},
        {0.002f, 0.002f, 0.0f},
        {0.0f, 0.0f, 0.0f},
    };




    load_images(ID);

    // render loop
    // -----------
    int spriteStep = 0;
    auto time = std::chrono::system_clock::now();

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window, ID, 0.005f);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for(int i = 0; i < num_of_images; i++){
            // bind textures on corresponding texture units
            glUseProgram(ID); 
            glUniform3fv(glGetUniformLocation(ID, "deltaPos"), 1, &deltaPos[i][0]);
            glUniform1i(glGetUniformLocation(ID, "numSprites"), numOfSprites[i]);
            glUniform1i(glGetUniformLocation(ID, "spriteStep"), spriteStep);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[i]);

            // render container
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i * 6 * sizeof(unsigned int)));
            deltaPos[i][0] += velocity[i][0];
            deltaPos[i][1] += velocity[i][1];
            deltaPos[i][2] += velocity[i][2];
        }
        if(
            std::chrono::duration_cast<std::chrono::duration<float>>(
                std::chrono::system_clock::now() - time
            ).count() >= 0.1
        ){
            spriteStep += 1;
            time = std::chrono::system_clock::now();
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

void processInput(GLFWwindow *window, unsigned int ID, float movement_speed)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        if (last_movement_direction != 'w'){
            last_movement_direction = 'w';
            imgpatharray[3] = movement_images[0];
            load_images(ID);
        }
        deltaPos[3][1] += movement_speed;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        if (last_movement_direction != 'a'){
            last_movement_direction = 'a';
            imgpatharray[3] = movement_images[2];
            load_images(ID);
        }
        deltaPos[3][0] -= movement_speed;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        if (last_movement_direction != 's'){
            last_movement_direction = 's';
            imgpatharray[3] = movement_images[1];
            load_images(ID);
        }
        deltaPos[3][1] -= movement_speed;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        if (last_movement_direction != 'd'){
            last_movement_direction = 'd';
            imgpatharray[3] = movement_images[3];
            load_images(ID);
        }
        deltaPos[3][0] += movement_speed;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
