#include "./glad.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cmath>
#include <ostream>
#include <fstream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"
#include <chrono>
#include <iostream>
#define SIN_OF_ANGLE 0.7071f
#define sSIN_OF_ANGLE "0.7071"
#define COS_OF_ANGLE 0.7071f
#define sCOS_OF_ANGLE "0.7071"
#define NUM_DIFFERENT_TILES 5

std::vector<std::string> imgpatharray;
std::vector<int> walkableMatrix;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, unsigned int ID, float movement_speed, int rows, int cols);



// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
std::string projectRoot = PROJECT_ROOT;



GLfloat deltaPos[3];
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

std::vector<float> generateVertices(std::vector<int> &tilemap, int rows, int cols){
    float float_max_rows = float(rows);
    float float_max_cols = float(cols);
    float tile_width = 1.0 / float(NUM_DIFFERENT_TILES);
    std::vector<float> result;

    for(int r = 0; r < rows; r++){
        for(int c = 0; c < cols; c++){
            float x1 = ( (2.0 * c) / float_max_cols) - 1.0;
            float x2 = ( (2.0 * (c + 1.0)) / float_max_cols) - 1.0;
            float y1 = ((-2.0 * r) / float_max_rows) + 1.0;
            float y2 = ((-2.0 * (r + 1.0)) / float_max_rows) + 1.0;
            float tileID = float(tilemap[cols*r + c]);
            float tile_min_x = tile_width * tileID;
            float tile_max_x = tile_min_x + tile_width;
            // top right
            // bottom rig
            // bottom lef
            // top left 
            std::vector<float> tileVertices = {
                x2, y1, 0.0,  tile_max_x, 1.0,
                x2, y2, 0.0,  tile_max_x, 0.0,
                x1, y2, 0.0,  tile_min_x, 0.0,
                x1, y1, 0.0,  tile_min_x, 1.0
            };

            // std::cout<<x2<<", "<<y1<<",   "<<tile_max_x<<", "<<1.0<<std::endl;
            // std::cout<<x2<<", "<<y2<<",   "<<tile_max_x<<", "<<0.0<<std::endl;
            // std::cout<<x1<<", "<<y2<<",   "<<tile_min_x<<", "<<0.0<<std::endl;
            // std::cout<<x1<<", "<<y1<<",   "<<tile_min_x<<", "<<1.0<<std::endl;
            // std::cout<<std::endl;

            result.insert(result.end(), tileVertices.begin(), tileVertices.end());
        }
    }
    return result;
    // for(int tr = 0; tr<8; tr++){
    //     for(int tc = 0; tc<5; tc++){
    //         std::cout<<result[5*tr + tc]<<", ";
    //     }
    //     std::cout<<std::endl;
    // }
}
int main()
{
    std::ifstream tilemapFile(projectRoot + "/tilemap.txt");
    std::ifstream walkableFile(projectRoot + "/walkable.txt");

    if (!tilemapFile) {
        std::cerr << "Unable to open tilemap file";
        return 1; // Return an error code
    }
    if (!walkableFile) {
        std::cerr << "Unable to open walkable file";
        return 1; // Return an error code
    }

    int rows, cols, rows2, cols2;
    tilemapFile  >> rows >> cols;
    walkableFile >> rows2 >> cols2;

    std::vector<int> tilemapMatrix;
    tilemapMatrix.reserve(rows * cols);
    walkableMatrix.reserve(rows * cols);

    if (rows != rows2 || cols != cols2) {
        std::cerr << "Tilemap sizes are different in tilemap.txt and walkable.txt";
        return 1; // Return an error code
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int value;
            tilemapFile >> value;
            tilemapMatrix.push_back(value);

            int walkableValue;
            walkableFile >> walkableValue;
            walkableMatrix.push_back(walkableValue);
        }
    }

    tilemapFile.close();
    walkableFile.close();

    imgpatharray = { 
        projectRoot + "/imgs/beeSprites.png",
    };

    int num_elements = (rows * cols) + imgpatharray.size();

    unsigned int textures[num_elements];
    std::vector<float> mainCharacterVertices = {
        -0.6f,  0.2f, -1.0f,   1.0f, 1.0f,
        -0.6f, -0.1f, -1.0f,   1.0f, 0.0f,
        -0.8f, -0.1f, -1.0f,   0.0f, 0.0f,
        -0.8f,  0.2f, -1.0f,   0.0f, 1.0f 
    };

    std::vector<int> numOfSprites = {6};
    std::vector<int> numOfActions = {4};
    for (int i=0; i < (rows * cols); i++){
        imgpatharray.insert(imgpatharray.begin(), projectRoot + "/imgs/tilemap2.png");
        numOfSprites.insert(numOfSprites.begin(), 1);
        numOfActions.insert(numOfActions.begin(), 1);
    }
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
        "uniform int numActions;\n"
        "uniform int actionStep;\n"
        "\n"
        "void main()\n"
        "{\n"
        "	float angle = radians(45.0);\n"
        "	mat4 rotationMatrix = mat4(\n"
        "		0.7 * " sCOS_OF_ANGLE ",  0.4 *-" sSIN_OF_ANGLE ", 0.0, 0.0,\n"
        "		0.7 * " sSIN_OF_ANGLE ",  0.4 * " sCOS_OF_ANGLE ", 0.0, 0.0,\n"
        "		0.0,         0.0,        1.0, 0.0,\n"
        "		0.0,         0.0,        0.0, 1.0\n"
        "	);\n"
        "	float fSpriteStep = float(spriteStep);\n"
        "	float fActionStep = float(actionStep);\n"
        "	if (aPos.z == -1.0){\n"
        "		gl_Position = vec4(aPos.x + deltaPos.x, aPos.y + deltaPos.y, 1.0, 1.0);\n"
        "	} else {\n"
        "		gl_Position = rotationMatrix * vec4(aPos.x, aPos.y, 1.0, 1.0);\n"
        "	}\n"
        "	TexCoord = vec2((aTexCoord.x + fSpriteStep)/numSprites, (aTexCoord.y + fActionStep)/numActions);\n"
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


    std::vector<float> vertices = generateVertices(tilemapMatrix, rows, cols);

    vertices.insert(vertices.end(), mainCharacterVertices.begin(), mainCharacterVertices.end());

    unsigned int indices[num_elements * 6];
    int ind_pos = 0;
    for (int rect = 0; rect < (num_elements); rect++) {
        int base_vertex = rect*4;
        // triangulo 1:
        indices[ind_pos  ] = base_vertex;
        indices[ind_pos+1] = base_vertex+1;
        indices[ind_pos+2] = base_vertex+3;
        // triangulo 2:
        indices[ind_pos+3] = base_vertex+1;
        indices[ind_pos+4] = base_vertex+2;
        indices[ind_pos+5] = base_vertex+3;

        ind_pos += 6;
    }

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),&vertices.front(),GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    for(int i = 0; i < num_elements; i++){
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

    glUniform1i(glGetUniformLocation(ID, "actionStep"), 0);

    // render loop
    // -----------
    int spriteStep = 0;
    auto time = std::chrono::system_clock::now();

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window, ID, 0.005f, rows, cols);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for(int i = 0; i < num_elements; i++){
            // bind textures on corresponding texture units
            if(i>0 && imgpatharray[i] == imgpatharray[i-1]){
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i * 6 * sizeof(unsigned int)));
                continue;
            } else {
                glUseProgram(ID); 
                glUniform3fv(glGetUniformLocation(ID, "deltaPos"), 1, &deltaPos[0]);
                glUniform1i(glGetUniformLocation(ID, "numSprites"), numOfSprites[i]);
                glUniform1i(glGetUniformLocation(ID, "spriteStep"), spriteStep);
                glUniform1i(glGetUniformLocation(ID, "numActions"), numOfActions[i]);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textures[i]);

                // render container
                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i * 6 * sizeof(unsigned int)));
            }
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

void processInput(GLFWwindow *window, unsigned int ID, float movement_speed, int rows, int cols)
{
    float x = (-0.7f+deltaPos[0])/0.7f; 
    float y = ( 0.0f+deltaPos[1])/0.4f*-1.0f; 

    int current_x_tile = int(((x* COS_OF_ANGLE) + (y* SIN_OF_ANGLE) +1.0)*cols/2);
    int current_y_tile = int(((x*-SIN_OF_ANGLE) + (y* COS_OF_ANGLE) +1.0)*rows/2);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        if (last_movement_direction != 'w'){
            last_movement_direction = 'w';
            glUniform1i(glGetUniformLocation(ID, "actionStep"), 3);
        }

        int walkableID = walkableMatrix[cols * current_y_tile + current_x_tile];
        if(walkableID == 1){
            glfwSetWindowShouldClose(window, true);
        } else if(walkableID == 2){
            std::cout<<"VOCÊ GANHOU"<<std::endl;
            glfwSetWindowShouldClose(window, true);
        } else {
            deltaPos[1] += movement_speed;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        if (last_movement_direction != 'a'){
            last_movement_direction = 'a';
            glUniform1i(glGetUniformLocation(ID, "actionStep"), 1);
        }

        int walkableID = walkableMatrix[cols * current_y_tile + current_x_tile];
        if(walkableID == 1){
            glfwSetWindowShouldClose(window, true);
        } else if(walkableID == 2){
            std::cout<<"VOCÊ GANHOU"<<std::endl;
            glfwSetWindowShouldClose(window, true);
        }else{
            deltaPos[0] -= movement_speed;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        if (last_movement_direction != 's'){
            last_movement_direction = 's';
            glUniform1i(glGetUniformLocation(ID, "actionStep"), 2);
        }
        int walkableID = walkableMatrix[cols * current_y_tile + current_x_tile];
        if(walkableID == 1){
            glfwSetWindowShouldClose(window, true);
        } else if(walkableID == 2){
            std::cout<<"VOCÊ GANHOU"<<std::endl;
            glfwSetWindowShouldClose(window, true);
        } else {
            deltaPos[1] -= movement_speed;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        if (last_movement_direction != 'd'){
            last_movement_direction = 'd';
            glUniform1i(glGetUniformLocation(ID, "actionStep"), 0);
        }
        int walkableID = walkableMatrix[cols * current_y_tile + current_x_tile];
        if(walkableID == 1){
            glfwSetWindowShouldClose(window, true);
        } else if(walkableID == 2){
            std::cout<<"VOCÊ GANHOU"<<std::endl;
            glfwSetWindowShouldClose(window, true);
        }else{
            deltaPos[0] += movement_speed;
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
