// STBI for image loading
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Includes.h"
#include "Triangle.h"
#include "Tile.h"
#include <iomanip>

int WIDTH = 1000, HEIGHT = 500;

GLFWwindow* window;
glm::mat4 projection;

GLuint shaderProgram;

Tile* tileMap[10][10];
float tw = 100.0f;
float th = 50.0f;

bool checkCollision(int row, int column, double x, double y) {
    if (row < 0 || row > 9 || column < 0 || column > 9) {
        return false;
    } else {
        Triangle *tOps = new Triangle();

        double p[] = {x, y};

        Triangle *t[2] = {tileMap[row][column]->t1, tileMap[row][column]->t2};

        double a1 = tOps->triangleArea(t[0]->A, p, t[0]->B);
        double a2 = tOps->triangleArea(p, t[0]->B, t[0]->C);
        double a3 = tOps->triangleArea(t[0]->A, p, t[0]->C);

        double a4 = tOps->triangleArea(t[1]->A, p, t[1]->B);
        double a5 = tOps->triangleArea(p, t[1]->B, t[1]->C);
        double a6 = tOps->triangleArea(t[1]->A, p, t[1]->C);

        if (t[0]->area() >= (a1 + a2 + a3) || t[1]->area() >= (a4 + a5 + a6)) {
            return true;
        } else {
            return false;
        }
    }
}

// Callback function called upon click of mouse
void mouseClick(GLFWwindow* window, int button, int action, int mods) {

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

        double mx, my;
        glfwGetCursorPos(window, &mx, &my);

        double yC = (((2 * mx / tw) + (2 * my / th)) / 2) - 5.5;
        double xC = (-2 * my / th + yC) + 10.5;

        int column = yC;
        int row = xC;
//
//        double xT = abs(yC * (tw/2) + xC * tw);
//        double yT = abs((yC * (th / 2) - xC * (th / 2) + HEIGHT / 2 - th / 2));

        double xT = mx;
        double yT = my;

        cout << "\nMouse clicked, coordinates [" << fixed << mx << "," << my << "]\n" << endl;

        cout << "Mouse clicked, de-projected coordinates [" << fixed << xC << "," << yC << "]\n" << endl;

        if (checkCollision(row, column, xT, yT)) {
            cout << "Clicked on tile [" << row << "," << column << "]\n" << endl;
        } else if (checkCollision(row - 1, column, xT, yT)) {
            cout << "Clicked on tile [" << row - 1 << "," << column << "]\n" << endl;
        } else if (checkCollision(row + 1, column, xT, yT)) {
            cout << "Clicked on tile [" << row + 1 << "," << column << "]\n" << endl;
        } else if (checkCollision(row, column - 1, xT, yT)) {
            cout << "Clicked on tile [" << row << "," << column - 1 << "]\n" << endl;
        } else if (checkCollision(row, column + 1, xT, yT)) {
            cout << "Clicked on tile [" << row << "," << column + 1 << "]\n" << endl;
        } else if (checkCollision(row, column + 1, xT, yT)) {
            cout << "Clicked on tile [" << row << "," << column + 1 << "]\n" << endl;
        } else
            return;
    }
}

void loadTileMap() {
    string line, value;

    ifstream in("../Resources/IsoTileMap.csv");

    if (!in) {
        cout << "Cannot open file.\n";
        return;
    }

    for (int r = 9; r >= 0; r = r - 1) {
        getline(in, line);

//        if (!in.good())
//            break;

        stringstream stream(line);

        for (int c = 0; c < 10; c = c + 1) {
            getline(stream, value, ',');

            stringstream streamValue(value);
            tileMap[r][c] = new Tile(r, c, tw, th, stoi(streamValue.str()), true, false);

            cout << "\t" << tileMap[r][c]->texture << " ";
        }
        cout << endl;
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
    projection =  glm::ortho(0.0f, (float) WIDTH, (float) HEIGHT, 0.0f, 0.1f, 100.0f);
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
    GLfloat vertices[] = {
            tw/2, 0.0f, -0.1f, // top
            0.0f, th/2, -0.1f, // left
            tw/2, th,   -0.1f, // bottom
            tw,   th/2, -0.1f  // right
    };


    float xt = 1.0f/8;
    float yt = 1.0f/12;

    GLfloat textureCoord[] = {
            xt/2,  0.0f, // top-left
            0.0f,  yt/2, // bottom-left
            xt/2,  yt, // bottom-right
            xt,    yt/2  // top-right
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

    data = stbi_load("../Resources/IsoTileSet.png", &texWidth, &texHeight, &nrChannels, 0);

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

        glBindVertexArray(vao);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "matrix"), 1, GL_FALSE, identity);

        glActiveTexture(GL_TEXTURE0);

        for (int i = 0; i < 100; i = i + 1) {
            glBindTexture(GL_TEXTURE_2D, textures[i]);
        }

        glUniform1i(glGetUniformLocation(shaderProgram, "theTexture"), 0);

        for (int row = 0; row < 10; row = row + 1) {
            for (int column = 0; column < 10; column = column + 1) {
                tileID = tileMap[row][column]->texture;
                oXt = (tileID % 8) * xt;
                oYt = (tileID / 8) * yt;

                oXv = column * (tw/2) + row * (tw/2);
                oYv = column * (th/2) - row * (th/2);

                glUniform1f(glGetUniformLocation(shaderProgram, "oXv"), oXv);
                glUniform1f(glGetUniformLocation(shaderProgram, "oYv"), oYv + HEIGHT/2);

                glUniform1f(glGetUniformLocation(shaderProgram, "oXt"), oXt);
                glUniform1f(glGetUniformLocation(shaderProgram, "oYt"), oYt);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }

//        glUniform1f(glGetUniformLocation(shaderProgram, "oXt"), 0.0f);
//        glUniform1f(glGetUniformLocation(shaderProgram, "oYt"), 0.0f);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);

    glDeleteBuffers(1, &vbo);

    glDeleteBuffers(1, &ebo);

    glfwTerminate();

	return EXIT_SUCCESS;
}
