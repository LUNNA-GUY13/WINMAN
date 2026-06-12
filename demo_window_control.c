#define WINMAN_IMPLEMENTATION
#include "winman.h"
#include <stdio.h>
#include <windows.h> // For Sleep

int main() {
    printf("--- Window Control Demo ---\n");

    // Get the foreground window
    HWND foreground_hwnd = winman_window_from_foreground();
    if (!winman_window_is_valid(foreground_hwnd)) {
        printf("No foreground window found or invalid. Please open a window and run again.\n");
        return 1;
    }
    printf("Foreground window found: %p\n", (void*)foreground_hwnd);

    // Get initial rect
    WINMAN_RECT initial_rect = winman_window_get_rect(foreground_hwnd);
    printf("Initial bounds: L:%d T:%d R:%d B:%d\n", initial_rect.left, initial_rect.top, initial_rect.right, initial_rect.bottom);

    // Center on screen
    printf("Centering window...\n");
    winman_window_center_on_screen(foreground_hwnd);
    Sleep(1000); // Wait a bit to see the effect

    // Resize to a specific size (e.g., 800x600)
    printf("Resizing window to 800x600...\n");
    winman_window_set_bounds(foreground_hwnd, initial_rect.left, initial_rect.top, 800, 600);
    Sleep(1000);

    // Minimize
    printf("Minimizing window...\n");
    winman_window_minimize(foreground_hwnd);
    Sleep(1000);

    // Restore
    printf("Restoring window...\n");
    winman_window_restore(foreground_hwnd);
    Sleep(1000);

    // Maximize
    printf("Maximizing window...\n");
    winman_window_maximize(foreground_hwnd);
    Sleep(1000);

    // Restore again
    printf("Restoring window again...\n");
    winman_window_restore(foreground_hwnd);
    Sleep(1000);

    // Set back to initial bounds
    printf("Setting window back to initial bounds...\n");
    winman_window_set_bounds(foreground_hwnd, initial_rect.left, initial_rect.top, winman_rect_width(initial_rect), winman_rect_height(initial_rect));
    Sleep(1000);

    printf("Window control demo finished.\n");
    return 0;
}