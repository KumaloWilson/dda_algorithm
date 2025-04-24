#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include "curved.h"

// Global variables for demonstration
float curved_start_x = 100.0f, curved_start_y = 250.0f;
float curved_end_x = 400.0f, curved_end_y = 250.0f;
float curved_control_x = 250.0f, curved_control_y = 50.0f; // Control point for quadratic curve
bool curved_animating = false;
int curved_currentStep = 0;
float curved_x_current, curved_y_current;
float curved_dx, curved_dy, curved_steps, curved_x_inc, curved_y_inc;
int curved_totalSteps;
int curved_curveSegments = 20; // Number of line segments to approximate the curve

// Function to draw a point
void curved_drawPoint(int x, int y, float size) {
    glPointSize(size);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

// Calculate a point on quadratic Bezier curve: P = (1-t)²P₀ + 2(1-t)tP₁ + t²P₂
void curved_calculateQuadraticBezierPoint(float t, float p0x, float p0y, float p1x, float p1y, float p2x, float p2y, float* px, float* py) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;

    // Calculate the point on the curve
    *px = uu * p0x + 2 * u * t * p1x + tt * p2x;
    *py = uu * p0y + 2 * u * t * p1y + tt * p2y;
}

// DDA Line Drawing Algorithm function with step-by-step visualization
void curved_DDA(float x1, float y1, float x2, float y2, bool visualizeSteps) {
    // Calculate dx & dy
    curved_dx = x2 - x1;
    curved_dy = y2 - y1;

    // Calculate steps required
    curved_steps = (fabs(curved_dx) > fabs(curved_dy)) ? fabs(curved_dx) : fabs(curved_dy);
    curved_totalSteps = (int)curved_steps;

    // Calculate x & y increments
    curved_x_inc = curved_dx / curved_steps;
    curved_y_inc = curved_dy / curved_steps;

    if (!visualizeSteps) {
        // Standard DDA implementation for immediate drawing
        float x = x1;
        float y = y1;

        glBegin(GL_POINTS);

        // Plot all points
        for (int i = 0; i <= curved_totalSteps; i++) {
            glVertex2i((int)round(x), (int)round(y));
            x += curved_x_inc;
            y += curved_y_inc;
        }

        glEnd();
    } else {
        // Initialize for animation
        curved_x_current = x1;
        curved_y_current = y1;
        curved_currentStep = 0;
        curved_animating = true;
    }
}

// Draw a quadratic Bezier curve using DDA for line segments
void curved_drawCurve(float p0x, float p0y, float p1x, float p1y, float p2x, float p2y, bool visualizeSteps) {
    float prevX, prevY, nextX, nextY;
    float t;

    // Calculate first point on the curve
    curved_calculateQuadraticBezierPoint(0, p0x, p0y, p1x, p1y, p2x, p2y, &prevX, &prevY);

    if (!visualizeSteps) {
        // Draw the complete curve at once
        glBegin(GL_POINTS);

        for (int i = 0; i <= curved_curveSegments; i++) {
            t = (float)i / curved_curveSegments;
            curved_calculateQuadraticBezierPoint(t, p0x, p0y, p1x, p1y, p2x, p2y, &nextX, &nextY);

            // Draw a line segment using DDA
            float dx = nextX - prevX;
            float dy = nextY - prevY;

            float steps = (fabs(dx) > fabs(dy)) ? fabs(dx) : fabs(dy);
            if (steps < 1) steps = 1;

            float x_inc = dx / steps;
            float y_inc = dy / steps;

            float x = prevX;
            float y = prevY;

            for (int j = 0; j <= (int)steps; j++) {
                glVertex2i((int)round(x), (int)round(y));
                x += x_inc;
                y += y_inc;
            }

            prevX = nextX;
            prevY = nextY;
        }

        glEnd();
    } else {
        // Initialize for animation
        // We'll use the currentStep to track which segment we're on
        curved_currentStep = 0;
        curved_animating = true;
    }
}

// Function to handle keyboard input
void curved_keyboard(unsigned char key, int x, int y) {
    if (key == 's' || key == 'S') {
        // Start/restart the step-by-step visualization
        curved_animating = true;
        curved_currentStep = 0;
        glutPostRedisplay();
    } else if (key == 'n' || key == 'N') {
        // Advance to next step if animating
        if (curved_animating && curved_currentStep <= curved_curveSegments) {
            curved_currentStep++;
            if (curved_currentStep > curved_curveSegments) {
                printf("Curve drawing complete!\n");
            }
            glutPostRedisplay();
        }
    } else if (key == 'r' || key == 'R') {
        // Draw the complete curve immediately
        curved_animating = false;
        glutPostRedisplay();
    } else if (key == 'c' || key == 'C') {
        // Clear and reset
        curved_animating = false;
        curved_currentStep = 0;
        glutPostRedisplay();
    } else if (key == 'q' || key == 'Q' || key == 27) {  // 27 is ESC key
        exit(0);
    }
}

// Display function for OpenGL
void curved_display() {
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
    const char* instructions = "S: Start/Reset | N: Next Step | R: Draw Complete Curve | C: Clear | Q: Quit";
    for (int i = 0; instructions[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, instructions[i]);
    }

    // Draw the control points
    glColor3f(1.0f, 0.0f, 0.0f);  // Red for endpoints
    curved_drawPoint((int)curved_start_x, (int)curved_start_y, 8.0f);
    curved_drawPoint((int)curved_end_x, (int)curved_end_y, 8.0f);

    glColor3f(0.0f, 0.0f, 1.0f);  // Blue for control point
    curved_drawPoint((int)curved_control_x, (int)curved_control_y, 8.0f);

    // Draw lines connecting the control points (to show the control polygon)
    glColor3f(0.3f, 0.3f, 0.3f);  // Light gray
    glBegin(GL_LINES);
    glVertex2i((int)curved_start_x, (int)curved_start_y);
    glVertex2i((int)curved_control_x, (int)curved_control_y);
    glVertex2i((int)curved_control_x, (int)curved_control_y);
    glVertex2i((int)curved_end_x, (int)curved_end_y);
    glEnd();

    if (curved_animating) {
        // Draw the curve segments up to currentStep
        glColor3f(0.0f, 1.0f, 0.0f);  // Green for curve segments

        float prevX, prevY, nextX, nextY;
        curved_calculateQuadraticBezierPoint(0, curved_start_x, curved_start_y, curved_control_x, curved_control_y, curved_end_x, curved_end_y, &prevX, &prevY);

        for (int i = 0; i <= curved_currentStep; i++) {
            float t = (float)i / curved_curveSegments;
            curved_calculateQuadraticBezierPoint(t, curved_start_x, curved_start_y, curved_control_x, curved_control_y, curved_end_x, curved_end_y, &nextX, &nextY);

            // Draw a line segment using DDA
            curved_DDA(prevX, prevY, nextX, nextY, false);

            prevX = nextX;
            prevY = nextY;
        }

        // Display step information
        char stepInfo[50];
        sprintf(stepInfo, "Segment: %d/%d", curved_currentStep, curved_curveSegments);
        glColor3f(1.0f, 1.0f, 1.0f);  // White for info text
        glRasterPos2i(10, 460);
        for (int i = 0; stepInfo[i] != '\0'; i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, stepInfo[i]);
        }

        // If we're on the last step, show current point
        if (curved_currentStep > 0 && curved_currentStep <= curved_curveSegments) {
            float t = (float)curved_currentStep / curved_curveSegments;
            curved_calculateQuadraticBezierPoint(t, curved_start_x, curved_start_y, curved_control_x, curved_control_y, curved_end_x, curved_end_y, &nextX, &nextY);

            char coordText[30];
            sprintf(coordText, "Point at t=%.2f: (%d, %d)", t, (int)round(nextX), (int)round(nextY));
            glColor3f(1.0f, 1.0f, 0.0f);  // Yellow for text
            glRasterPos2i(10, 440);
            for (int j = 0; coordText[j] != '\0'; j++) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, coordText[j]);
            }
        }
    } else {
        // Draw the complete curve
        glColor3f(1.0f, 1.0f, 1.0f);  // White for the complete curve

        float prevX, prevY, nextX, nextY;
        curved_calculateQuadraticBezierPoint(0, curved_start_x, curved_start_y, curved_control_x, curved_control_y, curved_end_x, curved_end_y, &prevX, &prevY);

        for (int i = 1; i <= curved_curveSegments; i++) {
            float t = (float)i / curved_curveSegments;
            curved_calculateQuadraticBezierPoint(t, curved_start_x, curved_start_y, curved_control_x, curved_control_y, curved_end_x, curved_end_y, &nextX, &nextY);

            // Draw a line segment using DDA
            curved_DDA(prevX, prevY, nextX, nextY, false);

            prevX = nextX;
            prevY = nextY;
        }
    }

    glutSwapBuffers();
}

// Initialize OpenGL settings
void curved_init() {
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);  // Dark blue background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 500, 0, 500);  // Set coordinate system
    glPointSize(3.0f);  // Default point size
}

// Run function for bezier curve demo
void run_bezier_curve_demo(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("DDA Algorithm - Quadratic Bezier Curve Demo");

    printf("=== DDA Algorithm - Quadratic Bezier Curve Demo ===\n");
    printf("Controls:\n");
    printf("  S: Start/Reset the step-by-step visualization\n");
    printf("  N: Advance to the next segment\n");
    printf("  R: Draw the complete curve immediately\n");
    printf("  C: Clear the display\n");
    printf("  Q/ESC: Quit the program\n\n");

    curved_init();
    glutDisplayFunc(curved_display);
    glutKeyboardFunc(curved_keyboard);
    glutMainLoop();
}
