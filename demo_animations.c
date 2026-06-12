#define WINMAN_IMPLEMENTATION
#include "winman.h"
#include <stdio.h>
#include <windows.h> // For Sleep

// A simple custom WndProc for the demo window
LRESULT CALLBACK AnimationWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

int main() {
    printf("--- Animation Demo ---\n");

    // Create a simple window to animate
    HINSTANCE hInstance = GetModuleHandle(NULL);
    const char* class_name = "WinmanAnimationDemoClass";
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = AnimationWndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = CreateSolidBrush(RGB(0, 120, 215)); // Blue background
    wc.lpszClassName = class_name;
    RegisterClassA(&wc);

    HWND demo_hwnd = CreateWindowExA(0, class_name, "WinMan Animation Demo", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                     100, 100, 300, 200, NULL, NULL, hInstance, NULL);
    if (!demo_hwnd) {
        printf("Failed to create demo window.\n");
        return 1;
    }
    printf("Demo window created: %p\n", (void*)demo_hwnd);
    Sleep(1000);

    // Animation 1: Move and resize using winman_animate_bounds
    printf("Animating bounds to (500, 300, 400, 250) over 2 seconds...\n");
    winman_animate_bounds(demo_hwnd, 500, 300, 400, 250, 2.0f);
    printf("Animation 1 complete.\n");
    Sleep(1000);

    // Animation 2: Keyframe animation from JSON
    printf("Animating with JSON keyframes...\n");
    const char* json_keyframes = 
        "["
        "  {\"at\": 0.0, \"x\": 500, \"y\": 300, \"w\": 400, \"h\": 250, \"ease\": \"linear\"},"
        "  {\"at\": 1.0, \"x\": 100, \"y\": 500, \"w\": 200, \"h\": 150, \"ease\": \"in_cubic\"},"
        "  {\"at\": 2.0, \"x\": 700, \"y\": 100, \"w\": 500, \"h\": 300, \"ease\": \"out_cubic\"},"
        "  {\"at\": 3.0, \"x\": 100, \"y\": 100, \"w\": 300, \"h\": 200, \"ease\": \"in_out_cubic\"}"
        "]";
    winman_animate_from_json(demo_hwnd, json_keyframes);
    printf("Animation 2 complete.\n");
    Sleep(1000);

    printf("Animation demo finished. Destroying window.\n");
    DestroyWindow(demo_hwnd);
    UnregisterClassA(class_name, hInstance);
    return 0;
}