// STBI for image loading
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Includes.h"

int WIDTH = 1300, HEIGHT = 300;

GLFWwindow* window;
glm::mat4 projection;

GLuint shaderProgram;

int tileMap[10][10];

// Callback function called upon click of mouse
void mouseClick(GLFWwindow* window, int button, int action, int mods) {

}

void loadTileMap() {
    string line, value;

    ifstream in("../Resources/TileMap.csv");

    if (!in) {
        cout << "Cannot open file.\n";
        return;
    }

    for (int r = 9; r >= 0; r = r - 1) {
        getline(in, line);

        if (!in.good())
            break;

        stringstream stream(line);

        for (int c = 0; c < 10; c = c + 1) {
            getline(stream, value, ',');

            stringstream streamValue(value);
            tileMap[r][c] = stoi(streamValue.str());
        }
    }

    in.close();
}

int main() {

    // Variables to hold texture properties
    int texWidth, texHeight, nrChannels;
    unsigned char* data;

    loadTileMap();

	glfwInit();
    
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL features
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    // Prints the version of OpenGL
    const char* glfwversion = glfwGetVersionString();
    printf("GLFW %s\n", glfwversion);

    // Creates window with starting dimensions set in WIDTH and HEIGHT
    window = glfwCreateWindow(WIDTH, HEIGHT, "IsoGame", nullptr, nullptr);
    
    // Get framebuffer size to render viewport in proper scale - useful for HiDPI screens
    int fbWidth;
    int fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    
    if (window == nullptr) {
        std::cout << "Failed to create GLFW Window" << std::endl;
        glfwTerminate();
        
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW." << std::endl;
        return EXIT_FAILURE;
    }
    
    // Sets up initial ortho and viewport
    projection =  glm::ortho(0.0f, (float) WIDTH, 0.0f, (float) HEIGHT, 0.1f, 100.0f);
    glViewport(0, 0, fbWidth, fbHeight);

    // Sets up texture processing
    glEnable(GL_TEXTURE_2D);

    // Enable blending and testing of alpha
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Sets up how images should be rendered and sized
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Vertices
    float tw = 100.0f;
    float th = 50.0f;

    GLfloat vertices[] = {
            tw/2, 0.0f, -0.1f, // top-left
            0.0f, th/2, -0.1f, // bottom-left
            tw/2, th,   -0.1f, // bottom-right
            tw,   th/2, -0.1f  // top-right
    };

    // Setup for X = 0.125f initially because it's a sprite
    GLfloat textureCoord[] = {
            +0.000f, +0.125f, // top-left
            +0.000f, +0.000f, // bottom-left
            +0.125f, +0.000f, // bottom-right
            +0.125f, +0.125f  // top-right
    };

    GLfloat identity[] = {
            +1.0f, +0.0f, +0.0f, +0.0f,
            +0.0f, +1.0f, +0.0f, +0.0f,
            +0.0f, +0.0f, +1.0f, +0.0f,
            +0.0f, +0.0f, +0.0f, +1.0f
    };

    unsigned int indices[6];

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 3;
    indices[3] = 1;
    indices[4] = 2;
    indices[5] = 3;

    GLuint vao, vbo, vboTexture, ebo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Texture buffer & pointer
    glGenBuffers(1, &vboTexture);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexture);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoord), textureCoord, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Setting up texture for tiles
    GLuint textures[100];

    data = stbi_load("../Resources/TileSet.png", &texWidth, &texHeight, &nrChannels, 0);

    glGenTextures(1, textures);

    for (int i = 0; i < 100; i = i + 1) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
    }

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture for tiles" << std::endl;
    }

    stbi_image_free(data);

    const GLchar* vertexShader =
    "#version 410 core\n"
    "layout(location=0) in vec3 vp;"
    "layout(location=1) in vec2 vt;"
    "uniform mat4 proj;"
    "uniform mat4 matrix;"
    "uniform float oXv;"
    "uniform float oYv;"
    "out vec2 texCoord;"
    "void main () {"
    "    texCoord = vt;"
    "    gl_Position = proj * vec4(vp.x + oXv, vp.y + oYv, vp.z, 1.0);"
    "}";
    
    const GLchar* fragmentShader =
    "#version 410 core\n"
    "in vec2 texCoord;"
    "uniform sampler2D theTexture;"
    "uniform float oXt;"
    "uniform float oYt;"
    "out vec4 color;"
    "void main () {"
    "   vec4 texel = texture(theTexture, vec2(texCoord.x + oXt, texCoord.y + oYt));"
    "   color = texel;"
    "}";

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertexShader, nullptr);
	glCompileShader(vs);
    
    GLint success;
    GLchar infoLog[512];
    
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        glGetShaderInfoLog(vs, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragmentShader, nullptr);
	glCompileShader(fs);
    
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        glGetShaderInfoLog(fs, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vs);
	glAttachShader(shaderProgram, fs);
	glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    std::cout << infoLog << std::endl;

    glDeleteShader(vs);
    glDeleteShader(fs);

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE, glm::value_ptr(projection));

    float oXv;
    float oYv;

    float oXt;
    float oYt;

    int tileID;

    while (!glfwWindowShouldClose(window)) {

        // Sets mouse click callback
        glfwSetMouseButtonCallback(window, mouseClick);

        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        oYv = 0.0f;

        glBindVertexArray(vao);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "matrix"), 1, GL_FALSE, identity);

        glActiveTexture(GL_TEXTURE0);

        for (int i = 0; i < 100; i = i + 1) {
            glBindTexture(GL_TEXTURE_2D, textures[i]);
        }

        glUniform1i(glGetUniformLocation(shaderProgram, "theTexture"), 0);

        for (int r = 0; r < 10; r = r + 1) {
            for (int c = 0; c < 10; c = c + 1) {
                glUniform1f(glGetUniformLocation(shaderProgram, "oXv"), oXv);
                glUniform1f(glGetUniformLocation(shaderProgram, "oYv"), oYv);

                tileID = tileMap[r][c];
                oXt = (tileID % 8) / 8.0f;
                oYt = (tileID / 8.0f) / 8.0f;

                if(oYt < 0.125f) {
                    oYt = 0.0f;
                }
                oXv = c * tw + r * (tw/2);
                oYv = r * (th/2);
                glUniform1f(glGetUniformLocation(shaderProgram, "oXt"), oXt);
                glUniform1f(glGetUniformLocation(shaderProgram, "oYt"), oYt);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }

        glUniform1f(glGetUniformLocation(shaderProgram, "oXt"), 0.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "oYt"), 0.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);

    glDeleteBuffers(1, &vbo);

    glDeleteBuffers(1, &ebo);

    glfwTerminate();

	return EXIT_SUCCESS;
}
