#include <GL/glut.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define TEAM_SIZE 4
int team1_effort = 0, team2_effort = 0;
int team1_individual[TEAM_SIZE];
int team2_individual[TEAM_SIZE];
int game_time = 0;
float team1_shift = 0.0f, team2_shift = 0.0f;

void drawText(float x, float y, const char* text) {
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
    }
}

void drawCircle(float cx, float cy, float r, float red, float green, float blue) {
    glColor3f(red, green, blue);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 100; i++) {
        float angle = 2.0f * 3.1415926f * i / 100;
        float x = r * cosf(angle);
        float y = r * sinf(angle);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void drawLimb(float x1, float y1, float x2, float y2) {
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void drawCloud(float x, float y) {
    drawCircle(x, y, 0.05f, 1.0f, 1.0f, 1.0f);
    drawCircle(x + 0.04f, y + 0.01f, 0.04f, 1.0f, 1.0f, 1.0f);
    drawCircle(x - 0.04f, y + 0.01f, 0.04f, 1.0f, 1.0f, 1.0f);
    drawCircle(x, y + 0.02f, 0.03f, 1.0f, 1.0f, 1.0f);
}

void drawGrassPatch(float x, float y) {
    glColor3f(0.1f, 0.6f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y);
    glVertex2f(x + 0.01f, y + 0.05f);
    glVertex2f(x + 0.02f, y);
    glVertex2f(x + 0.015f, y);
    glVertex2f(x + 0.025f, y + 0.06f);
    glVertex2f(x + 0.035f, y);
    glEnd();
}

void drawBackground() {
    // Sky
    glColor3f(0.5f, 0.8f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -0.3f);
    glVertex2f(1.0f, -0.3f);
    glVertex2f(1.0f, 0.5f);
    glVertex2f(-1.0f, 0.5f);
    glEnd();

    // Ground (spring grass)
    glColor3f(0.3f, 0.8f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -0.3f);
    glVertex2f(1.0f, -0.3f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glEnd();

    // Grass patches
    for (float i = -0.95f; i < 1.0f; i += 0.08f) {
        drawGrassPatch(i, -0.3f);
    }

    // Clouds
    drawCloud(-0.7f, 0.35f);
    drawCloud(0.2f, 0.4f);
    drawCloud(0.7f, 0.3f);
}

void drawPlayer(float x, float y, float red, float green, float blue, float effortDirection, int effort) {
    float head_radius = 0.025f;
    float body_length = 0.045f;

    // Head
    drawCircle(x, y, head_radius, red, green, blue);

    // Body
    drawLimb(x, y - head_radius, x, y - head_radius - body_length);

    // Arms
    float arm_offset = 0.035f;
    float hand_y = y - head_radius - 0.015f;
    float hand_x = x + arm_offset * effortDirection;
    drawLimb(x, hand_y, hand_x, hand_y - 0.02f);
    drawLimb(x, hand_y, hand_x, hand_y + 0.02f);

    // Legs
    float foot_y = -0.3f;
    drawLimb(x, y - head_radius - body_length, x - 0.02f, foot_y);
    drawLimb(x, y - head_radius - body_length, x + 0.02f, foot_y);

    // Effort text
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d", effort);
    drawText(x - 0.015f, y + 0.03f, buffer);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawBackground();

    // Team 1 pulling right
    for (int i = 0; i < TEAM_SIZE; i++) {
        drawPlayer(-0.8f + i * 0.2f + team1_shift, -0.05f, 0.0f, 0.0f, 1.0f, 1.0f, team1_individual[i]);
    }

    // Team 2 pulling left
    for (int i = 0; i < TEAM_SIZE; i++) {
        drawPlayer(0.3f + i * 0.2f + team2_shift, -0.05f, 1.0f, 0.0f, 0.0f, -1.0f, team2_individual[i]);
    }

    // Rope aligned with players' hands
    float rope_y = -0.05f - 0.025f - 0.015f;
    glColor3f(0.7f, 0.4f, 0.1f);
    glBegin(GL_LINES);
    glVertex2f(-1.0f, rope_y);
    glVertex2f(1.0f, rope_y);
    glEnd();

    // Center indicator on rope
    float center = (team1_effort - team2_effort) / 1000.0f;
    if (center > 0.5f) center = 0.5f;
    if (center < -0.5f) center = -0.5f;

    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(center - 0.02f, rope_y - 0.02f);
    glVertex2f(center + 0.02f, rope_y - 0.02f);
    glVertex2f(center, rope_y + 0.04f);
    glEnd();

    // Referee and game time
    drawText(-0.98f, 0.42f, "Referee: Watching the game");
    char time_str[32];
    snprintf(time_str, sizeof(time_str), "Time: %d sec", game_time);
    drawText(0.7f, 0.42f, time_str);

    glutSwapBuffers();
}

void timer(int value) {
    team1_effort = 0;
    team2_effort = 0;
    for (int i = 0; i < TEAM_SIZE; i++) {
        team1_individual[i] = rand() % 100;
        team2_individual[i] = rand() % 100;
        team1_effort += team1_individual[i];
        team2_effort += team2_individual[i];
    }

    float shift = (team1_effort - team2_effort) / 1000.0f;
    if (shift > 0.2f) shift = 0.2f;
    if (shift < -0.2f) shift = -0.2f;

    team1_shift = shift;
    team2_shift = shift;

    game_time++;

    glutPostRedisplay();
    glutTimerFunc(1000, timer, 0);
}

void init() {
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -0.3, 0.5);
}

int main(int argc, char** argv) {
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 400);
    glutCreateWindow("Rope Pull Game Visualization");
    init();
    glutDisplayFunc(display);
    glutTimerFunc(1000, timer, 0);
    glutMainLoop();
    return 0;
}