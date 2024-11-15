#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include <iostream>

// Window dimensions
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

// Game state variables
float fireMagePos = -5.0f;
float iceMagePos = 5.0f;
float fireMageHealth = 100.0f;
float iceMageHealth = 100.0f;
float artifactSize = 1.0f;
float rayRotation = 0.0f;
bool isArtifactGlowing = false;
float cloudPosition = 0.0f;

// Animation variables
float spellAnimationTime = 0.0f;
bool isSpellAnimating = false;

// VBO and VAO identifiers
GLuint vboMage, vboArtifact, vaoMage, vaoArtifact;

// Define vertices for mage and artifact
GLfloat mageVertices[] = {
    // Body (triangle)
    -0.5f, 0.0f, 0.0f,
     0.5f, 0.0f, 0.0f,
     0.0f, 2.0f, 0.0f,

     // Staff (line)
      0.0f, 0.0f, 0.0f,
      0.5f, 2.0f, 0.0f, // For Fire Mage
};

// Define artifact vertices
GLfloat artifactVertices[] = {
    // Artifact core (square)
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.5f,  0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f,
};

void createVBOs() {
    // Mage VBO and VAO
    glGenVertexArrays(1, &vaoMage);
    glBindVertexArray(vaoMage);

    glGenBuffers(1, &vboMage);
    glBindBuffer(GL_ARRAY_BUFFER, vboMage);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mageVertices), mageVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // Artifact VBO and VAO
    glGenVertexArrays(1, &vaoArtifact);
    glBindVertexArray(vaoArtifact);

    glGenBuffers(1, &vboArtifact);
    glBindBuffer(GL_ARRAY_BUFFER, vboArtifact);
    glBufferData(GL_ARRAY_BUFFER, sizeof(artifactVertices), artifactVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
}

void drawHealthBar(float x, float health) {
    glPushMatrix();
    glTranslatef(x, 8.0f, 0.0f);
    glScalef(3.0f, 0.3f, 1.0f);

    // Background (red)
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, 0.0f);
    glVertex2f(1.0f, 0.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    // Health (green)
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, 0.0f);
    glVertex2f(-1.0f + (health / 50.0f), 0.0f);
    glVertex2f(-1.0f + (health / 50.0f), 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    glPopMatrix();
}

void drawMage(float x, bool isFireMage) {
    glPushMatrix();
    glTranslatef(x, 0.0f, 0.0f);

    // Set color based on mage type
    if (isFireMage)
        glColor3f(1.0f, 0.4f, 0.0f);
    else
        glColor3f(0.0f, 0.8f, 1.0f);

    // Draw body using VBO
    glBindVertexArray(vaoMage);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Draw staff using VBO
    glColor3f(0.6f, 0.3f, 0.1f);
    glBindVertexArray(vaoMage); // Staff data is included in the same VBO
    glDrawArrays(GL_LINES, 3, 2); // Drawing lines for the staff

    glPopMatrix();
}

void drawArtifact() {
    glPushMatrix();
    glTranslatef(0.0f, 3.0f, 0.0f);
    glScalef(artifactSize, artifactSize, 1.0f);

    // Artifact core
    glColor3f(1.0f, 1.0f, 0.0f);
    glBindVertexArray(vaoArtifact);
    glDrawArrays(GL_QUADS, 0, 4);

    glPopMatrix();
}

void drawEnvironment() {
    // Sky
    glColor3f(0.4f, 0.6f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-10.0f, -5.0f);
    glVertex2f(10.0f, -5.0f);
    glVertex2f(10.0f, 10.0f);
    glVertex2f(-10.0f, 10.0f);
    glEnd();

    // Mountains
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_TRIANGLES);
    // Left mountain
    glVertex2f(-8.0f, -5.0f);
    glVertex2f(-3.0f, -5.0f);
    glVertex2f(-5.5f, 3.0f);
    // Right mountain
    glVertex2f(3.0f, -5.0f);
    glVertex2f(8.0f, -5.0f);
    glVertex2f(5.5f, 3.0f);
    glEnd();

    // Sea
    glColor3f(0.0f, 0.3f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(-10.0f, -5.0f);
    glVertex2f(10.0f, -5.0f);
    glVertex2f(10.0f, -10.0f);
    glVertex2f(-10.0f, -10.0f);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawEnvironment();
    drawMage(fireMagePos, true);
    drawMage(iceMagePos, false);
    drawArtifact();
    drawHealthBar(-5.0f, fireMageHealth);
    drawHealthBar(5.0f, iceMageHealth);

    glutSwapBuffers();
}

void update(int value) {
    // Update cloud position
    cloudPosition += 0.01f;
    if (cloudPosition > 20.0f)
        cloudPosition = -20.0f;

    // Update ray rotation
    if (isArtifactGlowing)
        rayRotation += 2.0f;

    // Update spell animation
    if (isSpellAnimating) {
        spellAnimationTime += 0.1f;
        if (spellAnimationTime >= 1.0f) {
            isSpellAnimating = false;
            spellAnimationTime = 0.0f;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'a':  // Move fire mage left
        if (fireMagePos > -9.0f)
            fireMagePos -= 0.5f;
        break;
    case 'd':  // Move fire mage right
        if (fireMagePos < -1.0f)
            fireMagePos += 0.5f;
        break;
    case 'j':  // Move ice mage left
        if (iceMagePos > 1.0f)
            iceMagePos -= 0.5f;
        break;
    case 'l':  // Move ice mage right
        if (iceMagePos < 9.0f)
            iceMagePos += 0.5f;
        break;
    case 'f':  // Fire mage attack
        isSpellAnimating = true;
        if (abs(fireMagePos - iceMagePos) < 7.0f) {
            iceMageHealth -= 10.0f;
            iceMagePos += 1.0f;
        }
        break;
    case 'h':  // Ice mage attack
        isSpellAnimating = true;
        if (abs(fireMagePos - iceMagePos) < 7.0f) {
            fireMageHealth -= 10.0f;
            fireMagePos -= 1.0f;
        }
        break;
    case ' ':  // Toggle artifact glow
        isArtifactGlowing = !isArtifactGlowing;
        break;
    }
    glutPostRedisplay();
}

void mouseWheel(int button, int dir, int x, int y) {
    if (dir > 0) {
        artifactSize += 0.1f;
        if (artifactSize > 2.0f)
            artifactSize = 2.0f;
    }
    else {
        artifactSize -= 0.1f;
        if (artifactSize < 0.5f)
            artifactSize = 0.5f;
    }
    glutPostRedisplay();
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10.0, 10.0, -10.0, 10.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    // Initialize GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW Initialization Failed: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }

    createVBOs();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Mage Battle");
    glutFullScreen();

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}
