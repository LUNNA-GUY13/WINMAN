#define WINMAN_IMPLEMENTATION
#define WINMAN_ENABLE_D2D
#include "winman.h"
#include <stdio.h>

WINMAN_D2D_CONTEXT g_ctx;

LRESULT CALLBACK D2DTestWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_PAINT: {
            winman_d2d_begin_draw(&g_ctx);
            winman_d2d_clear(&g_ctx, (WINMAN_COLOR){20, 20, 25}); // Deep dark background

            // Draw some test shapes using Direct2D
            winman_d2d_draw_line(&g_ctx, 50, 50, 400, 300, (WINMAN_COLOR){0, 255, 127}, 2.0f);
            winman_d2d_draw_rect(&g_ctx, (WINMAN_RECT){100, 100, 300, 250}, (WINMAN_COLOR){255, 255, 0}, 1.5f);
            winman_d2d_fill_ellipse(&g_ctx, 500, 200, 80, 60, (WINMAN_COLOR){0, 120, 215});
            
            // Draw some hardware-accelerated text
            WINMAN_DWRITE_FORMAT font = winman_dwrite_create_format(&g_ctx, "Consolas", 24.0f);
            if (font.format) {
                winman_d2d_draw_text(&g_ctx, "WinMan Direct2D Active", &font, (WINMAN_RECT){50, 400, 600, 500}, (WINMAN_COLOR){255, 255, 255});
                winman_dwrite_destroy_format(&font);
            }

            winman_d2d_end_draw(&g_ctx);
            ValidateRect(hwnd, NULL);
            return 0;
        }
        case WM_SIZE: {
            if (g_ctx.render_target) {
                winman_d2d_resize(&g_ctx, LOWORD(lp), HIWORD(lp));
            }
            return 0;
        }
        case WM_DESTROY:
            winman_d2d_cleanup(&g_ctx);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

int main() {
    printf("--- Direct2D Linking Test ---\n");

    // Create a native window using the WinMan helper
    HWND hwnd = winman_create_shaped_window(
        "Direct2D Render Test", 
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, 
        RGB(0, 0, 0), false, D2DTestWndProc
    );

    if (!hwnd || !winman_d2d_init(hwnd, &g_ctx)) {
        printf("Critical Error: Could not initialize window or D2D context.\n");
        return 1;
    }

    printf("D2D Initialized. Close the window to exit.\n");
    winman_run_message_loop();
    return 0;
}
