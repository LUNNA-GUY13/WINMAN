#define WINMAN_IMPLEMENTATION
#include "winman.h"
#include <stdio.h>
#include <windows.h> // For Sleep

// A simple custom WndProc for the demo window
LRESULT CALLBACK ShapedWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    } else if (msg == WM_LBUTTONDOWN) {
        // Allow dragging the window
        ReleaseCapture();
        SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

int main() {
    printf("--- Shaped Window Demo ---\n");

    // Create a circular window
    printf("Creating a circular window...\n");
    HWND circular_hwnd = winman_create_shaped_window(
        "Circular Window", 
        50, 50, 200, 200, 
        RGB(255, 0, 0), // Red
        true, 
        ShapedWndProc
    );
    if (!circular_hwnd) {
        printf("Failed to create circular window.\n");
        return 1;
    }
    printf("Circular window created: %p\n", (void*)circular_hwnd);
    Sleep(1000);

    // Create a polygonal window
    printf("Creating a polygonal window (triangle)...\n");
    HWND polygonal_hwnd = winman_create_shaped_window(
        "Polygonal Window", 
        300, 50, 200, 200, 
        RGB(0, 255, 0), // Green
        false, // Not circular by default
        ShapedWndProc
    );
    if (!polygonal_hwnd) {
        printf("Failed to create polygonal window.\n");
        DestroyWindow(circular_hwnd);
        return 1;
    }
    printf("Polygonal window created: %p\n", (void*)polygonal_hwnd);

    // Define points for a triangle (relative to window top-left)
    POINT triangle_pts[] = {
        {100, 0},
        {0, 200},
        {200, 200}
    };
    winman_window_set_polygon_shape(polygonal_hwnd, triangle_pts, 3, WINDING);
    Sleep(1000);

    printf("Shaped window demo running. Click to drag, ESC to close.\n");
    winman_run_message_loop(); // Keep windows alive until closed

    printf("Shaped window demo finished.\n");
    return 0;
}