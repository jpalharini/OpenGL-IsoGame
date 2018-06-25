// STBI for image loading
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Includes.h"
#include "Triangle.h"
#include "Tile.h"
#include <iomanip>
#include "GameObject.h"

const bool DEBUG = true;
// if (DEBUG)

// Sets size of map
const int ROWS = 14;
const int COLS = 14;

// Sets dimensions of tiles
const float TW = 100.0f;
const float TH = 50.0f;

// Sets the speed of the Jeep
const float JEEP_SPEED = 60;

// Sets size of the screen based on map dimensions
const int WIDTH = ROWS*TW, HEIGHT = (COLS*TH);

GLFWwindow* window;
glm::mat4 projection;

GLuint shaderProgram;

// Creates a matrix holding tiles
Tile* tileMap[ROWS][COLS];

// The Jeep
GameObject* jeep;

// Clicked row and column
int cRow = 0;
int cCol = 0;

// Jeep drawing coordinates
float jeepX, jeepY;

// Flags
bool moving = false;
bool explosion = false;

int explosionCoords[2];

// Textures that are walkable
int walkableTextures[] = {10,11,13,14,15};

// Reads the tilemap from CSV file
void loadTileMap() {
    string line, value;
    int texture;
    bool walkable, bomb;

    ifstream in("../Resources/IsoTileMap2.csv");

    if (!in) {
        cout << "Cannot open file.\n";
        return;
    }

    // Inverting row order because of ortho
    for (int r = ROWS-1; r >= 0; r = r - 1) {
        getline(in, line);

        stringstream stream(line);

        for (int c = 0; c < COLS; c = c + 1) {
            getline(stream, value, ',');

            stringstream streamValue(value);
            texture = stoi(streamValue.str());

            walkable = false;
            bomb = false;

            // Identifies walkable textures
            for (int i = 0; i < (sizeof(walkableTextures)/4); i = i + 1) {
                if (texture == walkableTextures[i]) {
                    // For each walkable texture aside from the starting tile, there is a 10% chance of bombs being placed
                    if (!(r == 1 && c == 13)){
                        bomb = (rand() % 100) < 10;
                    }
                    walkable = true;
                }
            }

            // Creates new Tile object inside map matrix
            tileMap[r][c] = new Tile(r, c, TW, TH, HEIGHT, stoi(streamValue.str()), walkable, bomb);

            if (DEBUG) cout << "\t" << tileMap[r][c]->texture << "\t";
        }
        cout << endl;
    }

    in.close();
}

/* Puts the mouse click in the correct tile
 *      row, column: position of candidate tile
 *      x, y: coordinates for mouse click
 */
bool checkCollision(int row, int column, double x, double y) {
    if (row < 0 || row > ROWS-1 || column < 0 || column > COLS-1) {
        return false;
    } else {

        // Coordinates of mouse click
        double p[] = {x, y};

        // Loads the triangles that form the candidate tile
        Triangle *t[2] = {tileMap[row][column]->t1, tileMap[row][column]->t2};

        // Calculates the areas of triangles formed by the coordinates of click and vertices of candidate tile
        double a1 = Triangle::triangleArea(t[0]->A, p, t[0]->B);
        double a2 = Triangle::triangleArea(p, t[0]->B, t[0]->C);
        double a3 = Triangle::triangleArea(t[0]->A, p, t[0]->C);

        // Same as above, for second triangle
        double a4 = Triangle::triangleArea(t[1]->A, p, t[1]->B);
        double a5 = Triangle::triangleArea(p, t[1]->B, t[1]->C);
        double a6 = Triangle::triangleArea(t[1]->A, p, t[1]->C);

        /* Checks if the sum of areas calculated above match the total area
         *      true: correct tile
         *      false: incorrect tile
         */
        if (t[0]->area() == (a1 + a2 + a3) || t[1]->area() == (a4 + a5 + a6)) {
            return true;
        } else {
            return false;
        }
    }
}

// Callback function called upon click of mouse
void mouseClick(GLFWwindow* window, int button, int action, int mods) {

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !moving) {

        double mx, my;
        glfwGetCursorPos(window, &mx, &my);

        // Project click over tilemap
        int column = (((2 * mx / TW) + (2 * my / TH)) / 2) - ((COLS/2) + 0.5);
        int row = (-2 * my / TH + column) + (ROWS + 0.5);

//        cout << "\nMouse clicked, coordinates [" << fixed << mx << "," << my << "]\n" << endl;
//
//        cout << "Mouse clicked, de-projected coordinates [" << fixed << xC << "," << yC << "]\n" << endl;

        /* Checks collision with the four candidate tiles:
         *      - r,   c
         *      - r,   c-1
         *      - r+1, c
         *      - r-1, c
         */
        if (checkCollision(row, column, mx, my)) {
            //cout << "Clicked on tile [" << row << "," << column << "]\n" << endl;
            cRow = row;
            cCol = column;
            moving = true;
            if (DEBUG) cout << "Moving Jeep to [" << cRow << "," << cCol << "]" << " - Texture: " << tileMap[cRow][cCol]->texture << endl;

        } else if (checkCollision(row, column - 1, mx, my)) {
            //cout << "Clicked on tile [" << row << "," << column - 1 << "]\n" << endl;
            cRow = row;
            cCol = column - 1;
            moving = true;
            if (DEBUG) cout << "Moving Jeep to [" << cRow << "," << cCol << "]" << " - Texture: " << tileMap[cRow][cCol]->texture << endl;

        } else if (checkCollision(row + 1, column, mx, my)) {
            //cout << "Clicked on tile [" << row + 1 << "," << column << "]\n" << endl;
            cRow = row + 1;
            cCol = column;
            moving = true;
            if (DEBUG) cout << "Moving Jeep to [" << cRow << "," << cCol << "]" << " - Texture: " << tileMap[cRow][cCol]->texture << endl;

        } else if (checkCollision(row - 1, column, mx, my)) {
            //cout << "Clicked on tile [" << row - 1 << "," << column << "]\n" << endl;
            cRow = row - 1;
            cCol = column;
            moving = true;
            if (DEBUG) cout << "Moving Jeep to [" << cRow << "," << cCol << "]" << " - Texture: " << tileMap[cRow][cCol]->texture << endl;
        }
    }
}

double getTime() {
    static double previousSecondsTrans = glfwGetTime();
    double currentSecondsTrans = glfwGetTime();
    double elapsedSecondsTrans = currentSecondsTrans - previousSecondsTrans;
    previousSecondsTrans = currentSecondsTrans;

    return elapsedSecondsTrans;
}

void setExplosion(int row, int column) {
    explosion = true;
    explosionCoords[0] = row;
    explosionCoords[1] = column;
}

// Moves the Jeep in the given direction (dir)
void moveJeep(char dir) {
    float stepX, stepY;

    double elapsedSecondsTrans;

    switch (dir) {
        // To move north, decrease X and Y
        case 'n':
            if(tileMap[cRow][cCol]->walkable) {
                // Gets final coordinates
                stepX = tileMap[jeep->row][cCol]->offsetX;
                stepY = tileMap[jeep->row][cCol]->offsetY;

                if (jeepX >= stepX || jeepY >= stepY) {
                    elapsedSecondsTrans = getTime();
                    if (jeepX >= stepX) {
                        jeepX = jeepX - elapsedSecondsTrans * JEEP_SPEED;
                    }
                    if (jeepY >= stepY) {
                        jeepY = jeepY - elapsedSecondsTrans * JEEP_SPEED/2;
                    }
                } else {
                    jeep->column = cCol;
                    if (DEBUG) cout << "Moved Jeep to [" << jeep->row << "," << jeep->column << "]" << endl;

                    // Ensures coordinates are set to an integer position to avoid drifting
                    jeepX = stepX;
                    jeepY = stepY;

                    if(tileMap[jeep->row][jeep->column]->bomb) {
                        cout << "The bomb exploded!" << endl;
                        moving = false;
                        setExplosion(jeep->row, jeep->column);
                        return;
                    }

                    moving = false;
                    return;
                }
            } else {
                cout << "Reached non-walkable tile" << endl;
                moving = false;
                return;
            }

            if(tileMap[jeep->row][jeep->column]->bomb) {
                cout << "The bomb exploded!" << endl;
                moving = false;
                setExplosion(jeep->row, jeep->column);
                return;
            }
            break;
        // To move south, increase X and Y
        case 's':
            if(tileMap[cRow][cCol]->walkable) {

                stepX = tileMap[jeep->row][cCol]->offsetX;
                stepY = tileMap[jeep->row][cCol]->offsetY;

                if (jeepX <= stepX || jeepY <= stepY) {
                    elapsedSecondsTrans = getTime();
                    if (jeepX <= stepX) {
                        jeepX = jeepX + elapsedSecondsTrans * JEEP_SPEED;
                    }
                    if (jeepY <= stepY) {
                        jeepY = jeepY + elapsedSecondsTrans * JEEP_SPEED/2;
                    }
                } else {
                    jeep->column = cCol;
                    if (DEBUG) cout << "Moved Jeep to [" << jeep->row << "," << jeep->column << "]" << endl;

                    // Ensures coordinates are set to an integer position to avoid drifting
                    jeepX = stepX;
                    jeepY = stepY;

                    if(tileMap[jeep->row][jeep->column]->bomb) {
                        cout << "The bomb exploded!" << endl;
                        moving = false;
                        setExplosion(jeep->row, jeep->column);
                        return;
                    }

                    moving = false;
                    return;
                }
            } else {
                cout << "Reached non-walkable tile" << endl;
                moving = false;
                return;
            }
            break;
        // To move east, decrease X and increase Y
        case 'e':
            if(tileMap[cRow][cCol]->walkable) {

                stepX = tileMap[cRow][jeep->column]->offsetX;
                stepY = tileMap[cRow][jeep->column]->offsetY;

                if (jeepX <= stepX || jeepY >= stepY) {
                    elapsedSecondsTrans = getTime();
                    if (jeepX <= stepX) {
                        jeepX = jeepX + elapsedSecondsTrans * JEEP_SPEED;
                    }
                    if (jeepY >= stepY) {
                        jeepY = jeepY - elapsedSecondsTrans * JEEP_SPEED/2;
                    }
                } else {
                    jeep->row = cRow;
                    if (DEBUG) cout << "Moved Jeep to [" << jeep->row << "," << jeep->column << "]" << endl;

                    // Ensures coordinates are set to an integer position to avoid drifting
                    jeepX = stepX;
                    jeepY = stepY;

                    if(tileMap[jeep->row][jeep->column]->bomb) {
                        cout << "The bomb exploded!" << endl;
                        moving = false;
                        setExplosion(jeep->row, jeep->column);
                        return;
                    }

                    moving = false;
                    return;
                }
            } else {
                cout << "Reached non-walkable tile" << endl;
                moving = false;
                return;
            }

            break;
        // To move west, increase X and decrease Y
        case 'w':
            if(tileMap[jeep->row][jeep->column]->walkable) {

                stepX = tileMap[cRow][jeep->column]->offsetX;
                stepY = tileMap[cRow][jeep->column]->offsetY;

                if (jeepX >= stepX || jeepY <= stepY) {
                    elapsedSecondsTrans = getTime();
                    if (jeepX >= stepX) {
                        jeepX = jeepX - elapsedSecondsTrans * JEEP_SPEED;
                    }
                    if (jeepY <= stepY) {
                        jeepY = jeepY + elapsedSecondsTrans * JEEP_SPEED/2;
                    }
                } else {
                    jeep->row = cRow;
                    if (DEBUG) cout << "Moved Jeep to [" << jeep->row << "," << jeep->column << "]" << endl;

                    // Ensures coordinates are set to an integer position to avoid drifting
                    jeepX = stepX;
                    jeepY = stepY;

                    if(tileMap[jeep->row][jeep->column]->bomb) {
                        cout << "The bomb exploded!" << endl;
                        moving = false;
                        setExplosion(jeep->row, jeep->column);
                        return;
                    }

                    moving = false;
                    return;
                }
            } else {
                cout << "Reached non-walkable tile" << endl;
                moving = false;
                return;
            }

            break;
    }
}

int main() {

    // Generate random seed
    srand((unsigned) time (0));

    // Variables to hold texture properties
    int texWidth, texHeight, nrChannels;
    unsigned char* data;

    // Loads the tilemap
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

    // Vertices for tiles
    GLfloat verticesTiles[] = {
            TW/2, 0.0f, -0.1f, // top
            0.0f, TH/2, -0.1f, // left
            TW/2, TH,   -0.1f, // bottom
            TW,   TH/2, -0.1f  // right
    };

    // Texture coordinates of tiles
    float xt = 1.0f/8;
    float yt = 1.0f/12;

    GLfloat textureCoordTiles[] = {
            xt/2,  0.0f,    // top
            0.0f,  yt/2,    // left
            xt/2,  yt,      // bottom
            xt,    yt/2     // right
    };

    auto tile = new GameObject(verticesTiles, textureCoordTiles, "../Resources/IsoTileSet.png");

    // Dimensions of Jeep
    float jwN = 69.0f; // Width of Jeep pointed to North
    float jwS = 61.0f; // Width of Jeep pointed to South
    float jwE = 68.0f; // Width of Jeep pointed to East
    float jwW = 58.5f; // Width of Jeep pointed to West

    float jh = 50.0f;

    // The Jeep for each direction has a different width, so we create 4 different GameObject instances with different vertices
    GLfloat verticesJeepN[] = {
            0.0f, 0.0f, -0.1f, // top-left
            0.0f, jh,   -0.1f, // bottom-left
            jwN,  jh,   -0.1f, // bottom-right
            jwN,  0.0f, -0.1f  // top-right
    };

    GLfloat verticesJeepS[] = {
            0.0f, 0.0f, -0.1f, // top-left
            0.0f, jh,   -0.1f, // bottom-left
            jwS,  jh,   -0.1f, // bottom-right
            jwS,  0.0f, -0.1f  // top-right
    };

    GLfloat verticesJeepE[] = {
            0.0f, 0.0f, -0.1f, // top-left
            0.0f, jh,   -0.1f, // bottom-left
            jwE,  jh,   -0.1f, // bottom-right
            jwE,  0.0f, -0.1f  // top-right
    };


    GLfloat verticesJeepW[] = {
            0.0f, 0.0f, -0.1f, // top-left
            0.0f, jh,   -0.1f, // bottom-left
            jwW,  jh,   -0.1f, // bottom-right
            jwW,  0.0f, -0.1f  // top-right
    };

    // Texture coordinates for bomb, note the inverse order because of ortho
    GLfloat textureCoordJeep[] = {
            +0.0f, +0.0f, // top-left
            +0.0f, +1.0f, // bottom-left
            +1.0f, +1.0f, // bottom-right
            +1.0f, +0.0f  // top-right
    };

    int jeepR = 1, jeepC = 13;

    auto jeepN = new GameObject(verticesJeepN, textureCoordJeep, "../Resources/jeepN.png", jeepR, jeepC);
    auto jeepS = new GameObject(verticesJeepS, textureCoordJeep, "../Resources/jeepS.png", jeepR, jeepC);
    auto jeepE = new GameObject(verticesJeepE, textureCoordJeep, "../Resources/jeepE.png", jeepR, jeepC);
    auto jeepW = new GameObject(verticesJeepW, textureCoordJeep, "../Resources/jeepW.png", jeepR, jeepC);

    // Bomb dimensions
    float bw = 100.0f;
    float bh = 50.0f;

    // Bomb sprite frame specs
    // Number of rows and columns
    int bSr = 4;
    int bSc = 3;

    // Width and height of frame
    float bTw = 1.0f/bSc;
    float bTh = 1.0f/bSr;

    // Vertices of the bomb
    GLfloat verticesBomb[] = {
            0.0f, 0.0f, -0.1f, // top-left
            0.0f, bh,   -0.1f, // bottom-left
            bw,   bh,   -0.1f, // bottom-right
            bw,   0.0f, -0.1f  // top-right
    };

    // Texture coordinates for bomb, note the inverse order because of ortho
    GLfloat textureCoordBomb[] = {
            0.0f,  0.0f, // top-left
            0.0f,  bTh, // bottom-left
            bTw,   bTh, // bottom-right
            bTw,   0.0f  // top-right
    };

    auto bomb = new GameObject(verticesBomb, textureCoordBomb, "../Resources/explosion.png");

    // Setting up texture for tiles
    GLuint texturesTiles[ROWS*COLS];

    data = stbi_load(tile->textureFile, &texWidth, &texHeight, &nrChannels, 0);

    glGenTextures(1, texturesTiles);

    for (int i = 0; i < ROWS*COLS; i = i + 1) {
        glBindTexture(GL_TEXTURE_2D, texturesTiles[i]);
    }

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture for tiles" << std::endl;
    }

    stbi_image_free(data);

    // Setting up texture for jeepN
    GLuint textureJeepN;

    data = stbi_load(jeepN->textureFile, &texWidth, &texHeight, &nrChannels, 0);

    glGenTextures(1, &textureJeepN);

    glBindTexture(GL_TEXTURE_2D, textureJeepN);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture for Jeep N" << std::endl;
    }

    stbi_image_free(data);

    // Setting up texture for jeepS
    GLuint textureJeepS;

    data = stbi_load(jeepS->textureFile, &texWidth, &texHeight, &nrChannels, 0);

    glGenTextures(1, &textureJeepS);

    glBindTexture(GL_TEXTURE_2D, textureJeepS);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture for Jeep S" << std::endl;
    }

    stbi_image_free(data);

    // Setting up texture for jeepE
    GLuint textureJeepE;

    data = stbi_load(jeepE->textureFile, &texWidth, &texHeight, &nrChannels, 0);

    glGenTextures(1, &textureJeepE);

    glBindTexture(GL_TEXTURE_2D, textureJeepE);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture for Jeep E" << std::endl;
    }

    stbi_image_free(data);

    // Setting up texture for jeepW
    GLuint textureJeepW;

    data = stbi_load(jeepW->textureFile, &texWidth, &texHeight, &nrChannels, 0);

    glGenTextures(1, &textureJeepW);

    glBindTexture(GL_TEXTURE_2D, textureJeepW);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture for Jeep W" << std::endl;
    }

    stbi_image_free(data);

    // Setting up textures for bomb
    GLuint texturesBomb[bSr*bSc];

    data = stbi_load(bomb->textureFile, &texWidth, &texHeight, &nrChannels, 0);

    glGenTextures(1, texturesBomb);

    for (int i = 0; i < bSr*bSc; i = i + 1) {
        glBindTexture(GL_TEXTURE_2D, texturesBomb[i]);
    }

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture for bombs" << std::endl;
    }

    // Layers
    float zMap = -0.53f;
    float zJeep = -0.52f;
    float zBomb = -0.51f;

    // Loading objects into their own buffers
    tile->load();

    jeepE->load();
    jeepN->load();
    jeepS->load();
    jeepW->load();

    bomb->load();

    const GLchar* vertexShader =
    "#version 410 core\n"
    "layout(location=0) in vec3 vp;"
    "layout(location=1) in vec2 vt;"
    "uniform mat4 proj;"
    "uniform mat4 matrix;"
    "uniform float oXv;"
    "uniform float oYv;"
    "uniform float layer;"
    "out vec2 texCoord;"
    "void main () {"
    "    texCoord = vt;"
    "    gl_Position = proj * vec4(vp.x + oXv, vp.y + oYv, layer, 1.0);"
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

    float oXt;
    float oYt;

    float oXtBomb = 0.0f;
    float oYtBomb = 0.0f;

    int tileID;

    jeepX = tileMap[jeepR][jeepC]->offsetX;
    jeepY = tileMap[jeepR][jeepC]->offsetY;

    char direction = 'n';

    auto textureJeep = textureJeepN;

    // Start the game showing bombs
    bool showBombs = true;

    // Measures the time of start of the game
    static double previousSeconds = glfwGetTime();
    double currentSeconds;

    while (!glfwWindowShouldClose(window)) {

        // If it's been 10 seconds, hide bombs
        if (currentSeconds - previousSeconds >= 10) {
            showBombs = false;
        }

        // Sets mouse click callback
        glfwSetMouseButtonCallback(window, mouseClick);

        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(tile->vao);

        glActiveTexture(GL_TEXTURE0);

        for (int i = 0; i < (ROWS*COLS); i = i + 1) {
            glBindTexture(GL_TEXTURE_2D, texturesTiles[i]);
        }

        glUniform1f(glGetUniformLocation(shaderProgram, "layer"), zMap);

        // Draws the tilemap
        for (int row = 0; row < ROWS; row = row + 1) {
            for (int column = 0; column < COLS; column = column + 1) {
                tileID = tileMap[row][column]->texture;

                oXt = (tileID % 8) * xt;
                oYt = (tileID / 8) * yt;

                glUniform1f(glGetUniformLocation(shaderProgram, "oXv"), tileMap[row][column]->offsetX);
                glUniform1f(glGetUniformLocation(shaderProgram, "oYv"), tileMap[row][column]->offsetY);

                glUniform1f(glGetUniformLocation(shaderProgram, "oXt"), oXt);
                glUniform1f(glGetUniformLocation(shaderProgram, "oYt"), oYt);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }

        // On the first 10 seconds, show bombs on the map
        if (showBombs) {
            for (int row = 0; row < ROWS; row = row + 1) {
                for (int column = 0; column < COLS; column = column + 1) {

                    if (tileMap[row][column]->bomb) {
                        glBindVertexArray(bomb->vao);

                        glActiveTexture(GL_TEXTURE0);

                        for (int i = 0; i < (bSr * bSc); i = i + 1) {
                            glBindTexture(GL_TEXTURE_2D, texturesBomb[i]);
                        }

                        glUniform1f(glGetUniformLocation(shaderProgram, "oXv"), tileMap[row][column]->offsetX);
                        glUniform1f(glGetUniformLocation(shaderProgram, "oYv"), tileMap[row][column]->offsetY);
                        glUniform1f(glGetUniformLocation(shaderProgram, "oXt"), 0);
                        glUniform1f(glGetUniformLocation(shaderProgram, "oYt"), 0);
                        glUniform1f(glGetUniformLocation(shaderProgram, "layer"), zBomb);

                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                    }
                }
            }
        }

        // Loads the Jeep on the correct direction
        switch (direction) {
            case 'n':
                if (jeep != nullptr) {
                    jeepN = jeep;
                }
                jeep = jeepN;
                textureJeep = textureJeepN;
                break;
            case 's':
                if (jeep != nullptr) {
                    jeepS = jeep;
                }
                jeep = jeepS;
                textureJeep = textureJeepS;
                break;
            case 'e':
                if (jeep != nullptr) {
                    jeepE = jeep;
                }
                jeep = jeepE;
                textureJeep = textureJeepE;
                break;
            case 'w':
                if (jeep != nullptr) {
                    jeepW = jeep;
                }
                jeep = jeepW;
                textureJeep = textureJeepW;
                break;
        }

        // Draws Jeep
        glBindVertexArray(jeep->vao);

        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, textureJeep);

        glUniform1f(glGetUniformLocation(shaderProgram, "oXv"), jeepX);
        glUniform1f(glGetUniformLocation(shaderProgram, "oYv"), jeepY);
        glUniform1f(glGetUniformLocation(shaderProgram, "oXt"), 0);
        glUniform1f(glGetUniformLocation(shaderProgram, "oYt"), 0);
        glUniform1f(glGetUniformLocation(shaderProgram, "layer"), zJeep);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Moves jeep
        if (moving) {

            // Only allow movement one step at a time
            if (cCol == jeep->column - 1 || cRow == jeep->row - 1 || cCol == jeep->column + 1 || cRow == jeep->row + 1) {

                // Checks direction of movement, we only allow N, S, E and W because of the way the map was designed
                if (cCol < jeep->column && cRow == jeep->row) {
                    direction = 'n';
                    moveJeep(direction);
                } else if (cCol > jeep->column && cRow == jeep->row) {
                    direction = 's';
                    moveJeep(direction);
                } else if (cCol == jeep->column && cRow > jeep->row) {
                    direction = 'e';
                    moveJeep(direction);
                } else if (cCol == jeep->column && cRow < jeep->row) {
                    direction = 'w';
                    moveJeep(direction);
                } else {
                    cout << "You can only move to north, south, east or west!" << endl;
                    moving = false;
                }
            } else {
                cout << "Jeep can only be moved one step at a time!" << endl;
                moving = false;
            }
        }

        // When explosion occurs, draw bomb in its location
        if (explosion) {
            glBindVertexArray(bomb->vao);

            glActiveTexture(GL_TEXTURE0);

            for (int i = 0; i < (bSr * bSc); i = i + 1) {
                glBindTexture(GL_TEXTURE_2D, texturesBomb[i]);
            }

            glUniform1f(glGetUniformLocation(shaderProgram, "oXv"), tileMap[explosionCoords[0]][explosionCoords[1]]->offsetX);
            glUniform1f(glGetUniformLocation(shaderProgram, "oYv"), tileMap[explosionCoords[0]][explosionCoords[1]]->offsetY);

            glUniform1f(glGetUniformLocation(shaderProgram, "oXt"), oXtBomb);
            glUniform1f(glGetUniformLocation(shaderProgram, "oYt"), oYtBomb);
            glUniform1f(glGetUniformLocation(shaderProgram, "layer"), zBomb);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // Animate the bomb
            oXtBomb = oXtBomb + bTw;
            if (oXtBomb == 1.0f) {
                oXtBomb = 0.0f;
                oYtBomb = oYtBomb + bTh;
                if (oYtBomb == 1.0f) {
                    // At the end of the animation, wait 10 seconds and exit the game
                    usleep(1*10e+6);
                    return EXIT_SUCCESS;
                }
            }
        }

        // Measures time of frame to set the time that the bomb will be displayed - I might use for something else :)
        currentSeconds = glfwGetTime();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &tile->vao);

    glDeleteBuffers(1, &tile->vbo);

    glDeleteBuffers(1, &tile->ebo);

    glfwTerminate();

	return EXIT_SUCCESS;
}
