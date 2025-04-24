#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include "straight.h"

// Global variables for straight line demo
float straight_start_x = 100.0f, straight_start_y = 100.0f;
float straight_end_x = 400.0f, straight_end_y = 300.0f;
bool straight_animating = false;
int straight_currentStep = 0;
float straight_x_current, straight_y_current;
float straight_dx, straight_dy, straight_steps, straight_x_inc, straight_y_inc;
int straight_totalSteps;

// Function to draw a point
void straight_drawPoint(int x, int y, float size) {
    glPointSize(size);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

// DDA Line Drawing Algorithm function with step-by-step visualization
void straight_DDA(float x1, float y1, float x2, float y2, bool visualizeSteps) {
    // Calculate dx & dy
    straight_dx = x2 - x1;
    straight_dy = y2 - y1;

    // Calculate steps required
    straight_steps = (fabs(straight_dx) > fabs(straight_dy)) ? fabs(straight_dx) : fabs(straight_dy);
    straight_totalSteps = (int)straight_steps;

    // Calculate x & y increments
    straight_x_inc = straight_dx / straight_steps;
    straight_y_inc = straight_dy / straight_steps;

    if (!visualizeSteps) {
        // Standard DDA implementation for immediate drawing
        float x = x1;
        float y = y1;

        glBegin(GL_POINTS);

        // Plot all points
        for (int i = 0; i <= straight_totalSteps; i++) {
            glVertex2i((int)round(x), (int)round(y));
            x += straight_x_inc;
            y += straight_y_inc;
        }

        glEnd();
    } else {
        // Initialize for animation
        straight_x_current = x1;
        straight_y_current = y1;
        straight_currentStep = 0;
        straight_animating = true;
    }
}

// Function to handle keyboard input for straight line demo
void straight_keyboard(unsigned char key, int x, int y) {
    if (key == 's' || key == 'S') {
        // Start/restart the step-by-step visualization
        straight_DDA(straight_start_x, straight_start_y, straight_end_x, straight_end_y, true);
    } else if (key == 'n' || key == 'N') {
        // Advance to next step if animating
        if (straight_animating && straight_currentStep <= straight_totalSteps) {
            straight_currentStep++;
            if (straight_currentStep > straight_totalSteps) {
                printf("Line drawing complete!\n");
            }
            glutPostRedisplay();
        }
    } else if (key == 'r' || key == 'R') {
        // Draw the complete line immediately
        straight_animating = false;
        glutPostRedisplay();
    } else if (key == 'c' || key == 'C') {
        // Clear and reset
        straight_animating = false;
        straight_currentStep = 0;
        glutPostRedisplay();
    } else if (key == 'q' || key == 'Q' || key == 27) {  // 27 is ESC key
        exit(0);
    }
}

// Display function for OpenGL - straight line demo
void straight_display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw coordinate axes for reference
    glColor3f(0.5f, 0.5f, 0.5f);  // Gray color
    glBegin(GL_LINES);
    glVertex2i(0, 250);
    glVertex2i(500, 250);
    glVertex2i(250, 0);
    glVertex2i(250, 500);
    glEnd();

    // Display instructions
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(10, 480);
    const char* instructions = "S: Start/Reset | N: Next Step | R: Draw Complete Line | C: Clear | Q: Quit";
    for (int i = 0; instructions[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, instructions[i]);
    }

    if (straight_animating) {
        // Draw endpoints
        glColor3f(1.0f, 0.0f, 0.0f);  // Red for endpoints
        straight_drawPoint((int)straight_start_x, (int)straight_start_y, 8.0f);
        straight_drawPoint((int)straight_end_x, (int)straight_end_y, 8.0f);

        // Draw the steps that have been calculated so far
        glColor3f(0.0f, 1.0f, 0.0f);  // Green for intermediate points
        for (int i = 0; i <= straight_currentStep; i++) {
            float x = straight_start_x + i * straight_x_inc;
            float y = straight_start_y + i * straight_y_inc;
            straight_drawPoint((int)round(x), (int)round(y), 5.0f);

            // Display the coordinates of the current point
            if (i == straight_currentStep) {
                char coordText[30];
                sprintf(coordText, "(%d, %d)", (int)round(x), (int)round(y));
                glColor3f(1.0f, 1.0f, 0.0f);  // Yellow for text
                glRasterPos2i((int)round(x) + 10, (int)round(y) + 10);
                for (int j = 0; coordText[j] != '\0'; j++) {
                    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, coordText[j]);
                }
            }
        }

        // Display step information
        char stepInfo[50];
        sprintf(stepInfo, "Step: %d/%d", straight_currentStep, straight_totalSteps);
        glColor3f(1.0f, 1.0f, 1.0f);  // White for info text
        glRasterPos2i(10, 460);
        for (int i = 0; stepInfo[i] != '\0'; i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, stepInfo[i]);
        }

        // Display calculation details
        char details[100];
        sprintf(details, "dx = %.2f, dy = %.2f, steps = %.0f, x_inc = %.2f, y_inc = %.2f",
                straight_dx, straight_dy, straight_steps, straight_x_inc, straight_y_inc);
        glRasterPos2i(10, 440);
        for (int i = 0; details[i] != '\0'; i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, details[i]);
        }

    } else {
        // Draw the complete line using DDA
        glColor3f(1.0f, 1.0f, 1.0f);  // White for the complete line
        straight_DDA(straight_start_x, straight_start_y, straight_end_x, straight_end_y, false);

        // Draw endpoints
        glColor3f(1.0f, 0.0f, 0.0f);  // Red for endpoints
        straight_drawPoint((int)straight_start_x, (int)straight_start_y, 5.0f);
        straight_drawPoint((int)straight_end_x, (int)straight_end_y, 5.0f);
    }

    glutSwapBuffers();
}

// Initialize OpenGL settings for straight line demo
void straight_init() {
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);  // Dark blue background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 500, 0, 500);  // Set coordinate system
    glPointSize(3.0f);  // Default point size
}

// Run function for straight line demo
void run_straight_line_demo(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("DDA Line Drawing Algorithm - Interactive Demo");

    printf("=== DDA Line Drawing Algorithm - Interactive Demo ===\n");
    printf("Controls:\n");
    printf("  S: Start/Reset the step-by-step visualization\n");
    printf("  N: Advance to the next step\n");
    printf("  R: Draw the complete line immediately\n");
    printf("  C: Clear the display\n");
    printf("  Q/ESC: Quit the program\n\n");

    straight_init();
    glutDisplayFunc(straight_display);
    glutKeyboardFunc(straight_keyboard);
    glutMainLoop();
}
