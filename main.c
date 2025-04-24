
#include <stdio.h>
#include <stdlib.h>
#include "straight.h"
#include "curved.h"

int main(int argc, char** argv) {
    int choice;

    printf("===== DDA Line Algorithm Demonstrations =====\n");
    printf("1. Straight Line DDA Algorithm Demo\n");
    printf("2. Quadratic Bezier Curve Demo using DDA\n");
    printf("Enter your choice (1 or 2): ");

    if (scanf("%d", &choice) != 1) {
        printf("Invalid input. Exiting.\n");
        return 1;
    }

    switch (choice) {
        case 1:
            run_straight_line_demo(argc, argv);
            break;
        case 2:
            run_bezier_curve_demo(argc, argv);
            break;
        default:
            printf("Invalid choice. Exiting.\n");
            return 1;
    }

    return 0;
}
