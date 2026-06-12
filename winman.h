#ifndef WINMAN_H
#define WINMAN_H

/**
 * WINMAN - A Windows Window Management and Shell Manipulation Library.
 * This library provides utilities for screen information, window manipulation,
 * desktop icon scraping, and UI simulation (e.g., fake BSOD and Update screens).
 * Made By LunnaGuy13 on GitHub 
 * https://github.com/LunnaGuy13/WinMan
 * MIT 2026 : LunnaGuy13 
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef WINMAN_ENABLE_D2D
#ifndef COBJMACROS
#define COBJMACROS
#endif
#ifdef WINMAN_IMPLEMENTATION
#include <initguid.h>
#endif
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#endif

/* ==============================================================================
 * CORE UTILITIES & TYPES
 * ============================================================================== */

typedef struct {
    int left;
    int top;
    int right;
    int bottom;
} WINMAN_RECT;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} WINMAN_COLOR;

static inline int winman_rect_width(WINMAN_RECT r) { return r.right - r.left; }
static inline int winman_rect_height(WINMAN_RECT r) { return r.bottom - r.top; }

WINMAN_COLOR winman_color_from_hex(const char* hex);
COLORREF winman_color_to_colorref(WINMAN_COLOR c);

/* ==============================================================================
 * SCREEN
 * ============================================================================== */

void winman_screen_size(int* width_out, int* height_out);
WINMAN_RECT winman_screen_work_area(void);
WINMAN_RECT winman_screen_fullscreen_bounds(void);

/* ==============================================================================
 * WINDOW WRAPPER
 * ============================================================================== */

HWND winman_window_from_title(const char* title_substring);
HWND winman_window_from_foreground(void);

bool winman_window_is_valid(HWND hwnd);
bool winman_window_is_visible(HWND hwnd);
bool winman_window_is_minimized(HWND hwnd);
bool winman_window_is_maximized(HWND hwnd);
bool winman_window_is_foreground(HWND hwnd);

WINMAN_RECT winman_window_get_rect(HWND hwnd);
void winman_window_set_bounds(HWND hwnd, int x, int y, int width, int height);

void winman_window_activate(HWND hwnd);
void winman_window_show(HWND hwnd);
void winman_window_hide(HWND hwnd);
void winman_window_minimize(HWND hwnd);
void winman_window_maximize(HWND hwnd);
void winman_window_restore(HWND hwnd);
void winman_window_destroy(HWND hwnd);

void winman_window_center_on_screen(HWND hwnd);
void winman_window_fill_work_area(HWND hwnd);

/* New Feature: Click Through */
void winman_window_set_clickthrough(HWND hwnd, bool enabled);

/* New Feature: Borderless & Fullscreen */
void winman_window_set_borderless(HWND hwnd, bool enabled);
void winman_window_set_fullscreen(HWND hwnd, bool enabled);

/* New Feature: Process Cloning */
void winman_window_launch_clone(HWND source, WINMAN_RECT bounds);

/* ==============================================================================
 * LAYOUT ENGINE
 * ============================================================================== */

void winman_layout_split(WINMAN_RECT bounds, const int* ratios, int ratio_count, bool horizontal, WINMAN_RECT* out_rects);
int winman_layout_split_str(WINMAN_RECT bounds, const char* ratio_str, bool horizontal, WINMAN_RECT* out_rects, int max_rects);
void winman_layout_tile(HWND* hwnds, int hwnd_count, const int* ratios, int ratio_count, bool horizontal);

/* ==============================================================================
 * ANIMATIONS
 * ============================================================================== */

typedef enum { 
    WINMAN_EASE_LINEAR, 
    WINMAN_EASE_IN_CUBIC, 
    WINMAN_EASE_OUT_CUBIC, 
    WINMAN_EASE_IN_OUT_CUBIC, 
    WINMAN_EASE_SINE 
} WinmanEasing;

typedef struct {
    float at; // Keyframe timestamp in seconds
    WINMAN_RECT bounds;
    WinmanEasing ease;
} WINMAN_KEYFRAME;

void winman_animate_bounds(HWND hwnd, int tx, int ty, int tw, int th, float duration_seconds);
void winman_animate_keyframes(HWND hwnd, WINMAN_KEYFRAME* keyframes, int count);
void winman_animate_from_json(HWND hwnd, const char* json_string);
void winman_play_scene_from_json(const char* json_string);

/* ==============================================================================
 * NATIVE CUSTOM WINDOWS
 * ============================================================================== */

typedef LRESULT (CALLBACK *WinmanWndProc)(HWND, UINT, WPARAM, LPARAM);

HWND winman_create_shaped_window(
    const char* title, 
    int x, int y, int width, int height, 
    COLORREF bg_color, 
    bool circular,
    WinmanWndProc wndproc
);

/* Set an arbitrary polygon region on a window.
   pts     : array of POINT structs in window-relative coords.
   count   : number of points (minimum 3).
   fill_mode: WINDING or ALTERNATE (Win32 polygon fill modes).
   Returns TRUE on success. */
BOOL winman_window_set_polygon_shape(HWND hwnd, const POINT* pts, int count, int fill_mode);

/* Remove any custom region, restoring the default rectangular shape. */
void winman_window_clear_shape(HWND hwnd);

void winman_run_message_loop(void);
HWND winman_create_status_bar(HWND parent, const char* text, int height, WINMAN_COLOR bg, WINMAN_COLOR fg);

/* ==============================================================================
 * DESKTOP ICON SCRAPER
 * ============================================================================== */

typedef struct {
    char  name[MAX_PATH]; /* display name of the icon / file */
    int   x;              /* desktop x coordinate            */
    int   y;              /* desktop y coordinate            */
} WINMAN_DESKTOP_ICON;

/* Fills icons_out with up to max_icons entries.
   Returns the number of icons found, or -1 on error.
   Requires the process to have the same session as the desktop (normal for interactive apps). */
int winman_desktop_get_icon_positions(WINMAN_DESKTOP_ICON* icons_out, int max_icons);

/* Sets the visibility of the Windows taskbar and desktop icons.
   visible : true to show, false to hide. */
void winman_set_shell_visibility(bool visible);

/* ==============================================================================
 * FAKE BSOD
 * ============================================================================== */

/* Displays a convincing full-screen Windows 11-style Blue Screen of Death.
   stop_code   : UPPERCASE stop code string, e.g. "CRITICAL_PROCESS_DIED".
                 Pass NULL for the default.
   duration_ms : How many milliseconds to display the screen before returning.
                 Pass 0 to block until the user presses any key.
   The window is always-on-top and covers all monitors. */
void winman_show_fake_bsod(const char* stop_code, DWORD duration_ms);

/* --------------------------------------------------------------------------
 * FAKE WINDOWS UPDATE
 * -------------------------------------------------------------------------- */

/* Displays a convincing full-screen Windows Update progress screen.
   duration_ms : How many milliseconds to simulate the update before finishing.
                 If 0, it defaults to 60 seconds.
   The window is always-on-top, blocks input, and can be exited with any key. */
void winman_show_fake_update_screen(DWORD duration_ms);

/* ==============================================================================
 * DIRECT2D / DIRECTWRITE (GameDev & Advanced UI)
 * ============================================================================== */

#ifdef WINMAN_ENABLE_D2D

typedef struct {
    ID2D1Factory* d2d_factory;
    IDWriteFactory* dwrite_factory;
    IWICImagingFactory* wic_factory;
    ID2D1HwndRenderTarget* render_target;
    ID2D1SolidColorBrush* default_brush;
    HWND hwnd;
} WINMAN_D2D_CONTEXT;

typedef struct {
    ID2D1Bitmap* bitmap;
    int width;
    int height;
} WINMAN_D2D_BITMAP;

typedef struct {
    IDWriteTextFormat* format;
} WINMAN_DWRITE_FORMAT;

bool winman_d2d_init(HWND hwnd, WINMAN_D2D_CONTEXT* ctx);
void winman_d2d_cleanup(WINMAN_D2D_CONTEXT* ctx);
void winman_d2d_resize(WINMAN_D2D_CONTEXT* ctx, int width, int height);

void winman_d2d_begin_draw(WINMAN_D2D_CONTEXT* ctx);
bool winman_d2d_end_draw(WINMAN_D2D_CONTEXT* ctx);
void winman_d2d_clear(WINMAN_D2D_CONTEXT* ctx, WINMAN_COLOR color);

void winman_d2d_draw_line(WINMAN_D2D_CONTEXT* ctx, int x1, int y1, int x2, int y2, WINMAN_COLOR color, float stroke_width);
void winman_d2d_draw_rect(WINMAN_D2D_CONTEXT* ctx, WINMAN_RECT rect, WINMAN_COLOR color, float stroke_width);
void winman_d2d_fill_rect(WINMAN_D2D_CONTEXT* ctx, WINMAN_RECT rect, WINMAN_COLOR color);
void winman_d2d_draw_ellipse(WINMAN_D2D_CONTEXT* ctx, int cx, int cy, int rx, int ry, WINMAN_COLOR color, float stroke_width);
void winman_d2d_fill_ellipse(WINMAN_D2D_CONTEXT* ctx, int cx, int cy, int rx, int ry, WINMAN_COLOR color);

WINMAN_D2D_BITMAP winman_d2d_load_bitmap(WINMAN_D2D_CONTEXT* ctx, const char* filepath);
void winman_d2d_draw_bitmap(WINMAN_D2D_CONTEXT* ctx, WINMAN_D2D_BITMAP* bitmap, int x, int y, int width, int height, float opacity);
void winman_d2d_destroy_bitmap(WINMAN_D2D_BITMAP* bitmap);

WINMAN_DWRITE_FORMAT winman_dwrite_create_format(WINMAN_D2D_CONTEXT* ctx, const char* font_family, float size);
void winman_d2d_draw_text(WINMAN_D2D_CONTEXT* ctx, const char* text, WINMAN_DWRITE_FORMAT* format, WINMAN_RECT rect, WINMAN_COLOR color);
void winman_dwrite_destroy_format(WINMAN_DWRITE_FORMAT* format);

#endif // WINMAN_ENABLE_D2D

#ifdef __cplusplus
}
#endif

#endif // WINMAN_H

/* ==============================================================================
 * WINMAN IMPLEMENTATION
 * ============================================================================== */

#ifdef WINMAN_IMPLEMENTATION

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

WINMAN_COLOR winman_color_from_hex(const char* hex) {
    WINMAN_COLOR c = {0};
    if (hex[0] == '#') hex++;
    int len = strlen(hex);
    if (len == 6) {
        sscanf(hex, "%02hhx%02hhx%02hhx", &c.r, &c.g, &c.b);
    } else if (len == 3) {
        uint8_t r, g, b;
        sscanf(hex, "%1hhx%1hhx%1hhx", &r, &g, &b);
        c.r = (r << 4) | r;
        c.g = (g << 4) | g;
        c.b = (b << 4) | b;
    }
    return c;
}

COLORREF winman_color_to_colorref(WINMAN_COLOR c) {
    return RGB(c.r, c.g, c.b);
}

void winman_screen_size(int* width_out, int* height_out) {
    if (width_out) *width_out = GetSystemMetrics(SM_CXSCREEN);
    if (height_out) *height_out = GetSystemMetrics(SM_CYSCREEN);
}

WINMAN_RECT winman_screen_work_area(void) {
    RECT r;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &r, 0);
    WINMAN_RECT wr = {r.left, r.top, r.right, r.bottom};
    return wr;
}

WINMAN_RECT winman_screen_fullscreen_bounds(void) {
    int w, h;
    winman_screen_size(&w, &h);
    WINMAN_RECT wr = {0, 0, w, h};
    return wr;
}

typedef struct {
    const char* title_substring;
    HWND found_hwnd;
} WinmanFindData;

static BOOL CALLBACK winman_enum_windows_proc(HWND hwnd, LPARAM lparam) {
    WinmanFindData* data = (WinmanFindData*)lparam;
    if (!IsWindowVisible(hwnd)) return TRUE;

    char title[1024];
    if (GetWindowTextA(hwnd, title, sizeof(title)) > 0) {
        if (strstr(title, data->title_substring) != NULL) {
            data->found_hwnd = hwnd;
            return FALSE;
        }
    }
    return TRUE;
}

HWND winman_window_from_title(const char* title_substring) {
    WinmanFindData data;
    data.title_substring = title_substring;
    data.found_hwnd = NULL;
    EnumWindows(winman_enum_windows_proc, (LPARAM)&data);
    return data.found_hwnd;
}

HWND winman_window_from_foreground(void) {
    return GetForegroundWindow();
}

bool winman_window_is_valid(HWND hwnd) { return hwnd != NULL && IsWindow(hwnd); }
bool winman_window_is_visible(HWND hwnd) { return IsWindowVisible(hwnd); }
bool winman_window_is_minimized(HWND hwnd) { return IsIconic(hwnd); }
bool winman_window_is_maximized(HWND hwnd) { return IsZoomed(hwnd); }
bool winman_window_is_foreground(HWND hwnd) { return GetForegroundWindow() == hwnd; }

WINMAN_RECT winman_window_get_rect(HWND hwnd) {
    RECT r;
    GetWindowRect(hwnd, &r);
    WINMAN_RECT wr = {r.left, r.top, r.right, r.bottom};
    return wr;
}

void winman_window_set_bounds(HWND hwnd, int x, int y, int width, int height) {
    SetWindowPos(hwnd, NULL, x, y, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);
}

void winman_window_activate(HWND hwnd) {
    if (winman_window_is_foreground(hwnd)) return;
    if (winman_window_is_minimized(hwnd)) ShowWindow(hwnd, SW_RESTORE);

    HWND foreground = GetForegroundWindow();
    DWORD foreground_thread = GetWindowThreadProcessId(foreground, NULL);
    DWORD target_thread = GetWindowThreadProcessId(hwnd, NULL);
    DWORD current_thread = GetCurrentThreadId();

    bool attached = false;
    if (foreground_thread && foreground_thread != target_thread) {
        AttachThreadInput(foreground_thread, target_thread, TRUE);
        attached = true;
    }
    if (current_thread != target_thread) {
        AttachThreadInput(current_thread, target_thread, TRUE);
    }
    SetForegroundWindow(hwnd);
    BringWindowToTop(hwnd);

    if (current_thread != target_thread) AttachThreadInput(current_thread, target_thread, FALSE);
    if (attached) AttachThreadInput(foreground_thread, target_thread, FALSE);
}

void winman_window_show(HWND hwnd) { ShowWindow(hwnd, SW_SHOWNORMAL); }
void winman_window_hide(HWND hwnd) { ShowWindow(hwnd, SW_HIDE); }
void winman_window_minimize(HWND hwnd) { ShowWindow(hwnd, SW_SHOWMINIMIZED); }
void winman_window_maximize(HWND hwnd) { ShowWindow(hwnd, SW_SHOWMAXIMIZED); }
void winman_window_restore(HWND hwnd) { ShowWindow(hwnd, SW_RESTORE); }
void winman_window_destroy(HWND hwnd) { DestroyWindow(hwnd); }

void winman_window_center_on_screen(HWND hwnd) {
    int sw, sh;
    winman_screen_size(&sw, &sh);
    WINMAN_RECT r = winman_window_get_rect(hwnd);
    int w = winman_rect_width(r);
    int h = winman_rect_height(r);
    int x = (sw - w) / 2;
    int y = (sh - h) / 2;
    winman_window_set_bounds(hwnd, x, y, w, h);
}

void winman_window_fill_work_area(HWND hwnd) {
    WINMAN_RECT wa = winman_screen_work_area();
    winman_window_set_bounds(hwnd, wa.left, wa.top, winman_rect_width(wa), winman_rect_height(wa));
}

void winman_window_set_clickthrough(HWND hwnd, bool enabled) {
    LONG ex_style = GetWindowLong(hwnd, GWL_EXSTYLE);
    if (enabled) {
        ex_style |= (WS_EX_TRANSPARENT | WS_EX_LAYERED);
    } else {
        ex_style &= ~(WS_EX_TRANSPARENT | WS_EX_LAYERED);
    }
    SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);
    if (enabled) {
        // Needs a color key or alpha to be actually layered/transparent on newer Windows
        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    }
}

void winman_window_set_borderless(HWND hwnd, bool enabled) {
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    if (enabled) {
        style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
    } else {
        style |= (WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
    }
    SetWindowLong(hwnd, GWL_STYLE, style);
    SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

void winman_window_set_fullscreen(HWND hwnd, bool enabled) {
    if (enabled) {
        winman_window_set_borderless(hwnd, true);
        winman_window_fill_work_area(hwnd); // or winman_screen_fullscreen_bounds depending on needs
    } else {
        winman_window_set_borderless(hwnd, false);
        winman_window_center_on_screen(hwnd);
    }
}

void winman_window_launch_clone(HWND source, WINMAN_RECT bounds) {
    DWORD process_id = 0;
    GetWindowThreadProcessId(source, &process_id);
    if (!process_id) return;

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);
    if (!hProcess) return;

    char exePath[MAX_PATH] = {0};
    DWORD pathSize = MAX_PATH;
    if (QueryFullProcessImageNameA(hProcess, 0, exePath, &pathSize)) {
        STARTUPINFOA si = {0};
        si.cb = sizeof(si);
        // Hint to new process where to spawn
        si.dwFlags = STARTF_USEPOSITION | STARTF_USESIZE;
        si.dwX = bounds.left;
        si.dwY = bounds.top;
        si.dwXSize = winman_rect_width(bounds);
        si.dwYSize = winman_rect_height(bounds);

        PROCESS_INFORMATION pi = {0};
        CreateProcessA(exePath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
        if (pi.hProcess) CloseHandle(pi.hProcess);
        if (pi.hThread) CloseHandle(pi.hThread);
    }
    CloseHandle(hProcess);
}

void winman_layout_split(WINMAN_RECT bounds, const int* ratios, int ratio_count, bool horizontal, WINMAN_RECT* out_rects) {
    int total_ratio = 0;
    for (int i = 0; i < ratio_count; i++) total_ratio += ratios[i];
    if (total_ratio == 0) return;

    int bounds_x = bounds.left;
    int bounds_y = bounds.top;
    int bounds_w = winman_rect_width(bounds);
    int bounds_h = winman_rect_height(bounds);

    int current_offset = 0;
    for (int i = 0; i < ratio_count; i++) {
        out_rects[i] = bounds;
        if (horizontal) {
            int w = (bounds_w * ratios[i]) / total_ratio;
            out_rects[i].left = bounds_x + current_offset;
            out_rects[i].right = out_rects[i].left + w;
            current_offset += w;
        } else {
            int h = (bounds_h * ratios[i]) / total_ratio;
            out_rects[i].top = bounds_y + current_offset;
            out_rects[i].bottom = out_rects[i].top + h;
            current_offset += h;
        }
    }
}

int winman_layout_split_str(WINMAN_RECT bounds, const char* ratio_str, bool horizontal, WINMAN_RECT* out_rects, int max_rects) {
    int ratios[64];
    int count = 0;
    const char* p = ratio_str;
    while (*p && count < 64 && count < max_rects) {
        int val = atoi(p);
        if (val > 0) ratios[count++] = val;
        while (*p && *p != ':') p++;
        if (*p == ':') p++;
    }
    if (count > 0) {
        winman_layout_split(bounds, ratios, count, horizontal, out_rects);
    }
    return count;
}

void winman_layout_tile(HWND* hwnds, int hwnd_count, const int* ratios, int ratio_count, bool horizontal) {
    if (hwnd_count == 0 || ratio_count == 0) return;
    WINMAN_RECT wa = winman_screen_work_area();
    WINMAN_RECT* splits = (WINMAN_RECT*)malloc(ratio_count * sizeof(WINMAN_RECT));
    if (!splits) return;
    winman_layout_split(wa, ratios, ratio_count, horizontal, splits);
    int max_i = hwnd_count < ratio_count ? hwnd_count : ratio_count;
    for (int i = 0; i < max_i; i++) {
        if (hwnds[i]) {
            winman_window_set_bounds(hwnds[i], splits[i].left, splits[i].top, winman_rect_width(splits[i]), winman_rect_height(splits[i]));
        }
    }
    free(splits);
}

// Easing Functions
static double _ease_in_out_cubic(double t) {
    if (t < 0.5) return 4.0 * t * t * t;
    return 1.0 - pow(-2.0 * t + 2.0, 3.0) / 2.0;
}
static double _ease_in_cubic(double t) { return t * t * t; }
static double _ease_out_cubic(double t) { return 1.0 - pow(1.0 - t, 3.0); }
static double _ease_sine(double t) { return -(cos(3.14159265359 * t) - 1.0) / 2.0; }

static double winman_apply_ease(double t, WinmanEasing ease) {
    if (t <= 0.0) return 0.0;
    if (t >= 1.0) return 1.0;
    switch (ease) {
        case WINMAN_EASE_LINEAR: return t;
        case WINMAN_EASE_IN_CUBIC: return _ease_in_cubic(t);
        case WINMAN_EASE_OUT_CUBIC: return _ease_out_cubic(t);
        case WINMAN_EASE_IN_OUT_CUBIC: return _ease_in_out_cubic(t);
        case WINMAN_EASE_SINE: return _ease_sine(t);
        default: return t;
    }
}

void winman_animate_bounds(HWND hwnd, int tx, int ty, int tw, int th, float duration_seconds) {
    WINMAN_KEYFRAME kf[2];
    WINMAN_RECT start = winman_window_get_rect(hwnd);
    kf[0].at = 0.0f; kf[0].bounds = start; kf[0].ease = WINMAN_EASE_LINEAR;
    WINMAN_RECT end = {tx, ty, tx+tw, ty+th};
    kf[1].at = duration_seconds; kf[1].bounds = end; kf[1].ease = WINMAN_EASE_IN_OUT_CUBIC;
    winman_animate_keyframes(hwnd, kf, 2);
}

void winman_animate_keyframes(HWND hwnd, WINMAN_KEYFRAME* keyframes, int count) {
    if (count < 2) return;
    
    LARGE_INTEGER freq, t_start, t_now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t_start);

    double total_duration_ms = keyframes[count-1].at * 1000.0;

    while (1) {
        QueryPerformanceCounter(&t_now);
        double elapsed_ms = (double)(t_now.QuadPart - t_start.QuadPart) * 1000.0 / (double)freq.QuadPart;
        
        if (elapsed_ms >= total_duration_ms) {
            WINMAN_RECT end = keyframes[count-1].bounds;
            winman_window_set_bounds(hwnd, end.left, end.top, winman_rect_width(end), winman_rect_height(end));
            break;
        }

        double current_sec = elapsed_ms / 1000.0;
        
        // Find segment
        int seg = 0;
        for (int i = 0; i < count - 1; i++) {
            if (current_sec >= keyframes[i].at && current_sec <= keyframes[i+1].at) {
                seg = i;
                break;
            }
        }

        WINMAN_KEYFRAME start_kf = keyframes[seg];
        WINMAN_KEYFRAME end_kf = keyframes[seg+1];

        double span = end_kf.at - start_kf.at;
        double local_t = (span > 0) ? (current_sec - start_kf.at) / span : 1.0;
        
        double eased_t = winman_apply_ease(local_t, end_kf.ease); // Use target's ease

        int sx = start_kf.bounds.left, sy = start_kf.bounds.top;
        int sw = winman_rect_width(start_kf.bounds), sh = winman_rect_height(start_kf.bounds);

        int ex = end_kf.bounds.left, ey = end_kf.bounds.top;
        int ew = winman_rect_width(end_kf.bounds), eh = winman_rect_height(end_kf.bounds);

        int curr_x = sx + (int)((ex - sx) * eased_t);
        int curr_y = sy + (int)((ey - sy) * eased_t);
        int curr_w = sw + (int)((ew - sw) * eased_t);
        int curr_h = sh + (int)((eh - sh) * eased_t);

        winman_window_set_bounds(hwnd, curr_x, curr_y, curr_w, curr_h);
        Sleep(1);
    }
}

// Lightweight JSON Keyframe Parser
void winman_animate_from_json(HWND hwnd, const char* json_string) {
    WINMAN_KEYFRAME kf[128];
    int kf_count = 0;

    const char* p = json_string;
    while (*p && kf_count < 128) {
        if (*p == '{') {
            float at = 0.0f;
            int x = 0, y = 0, w = 0, h = 0;
            WinmanEasing ease = WINMAN_EASE_LINEAR;
            
            // Very naive scanning for demo purposes. Expects {"at": ..., "x": ..., etc}
            const char* at_p = strstr(p, "\"at\"");
            if (at_p) sscanf(at_p, "\"at\"%*[ : \t]%f", &at);
            
            const char* x_p = strstr(p, "\"x\"");
            if (x_p) sscanf(x_p, "\"x\"%*[ : \t]%d", &x);
            
            const char* y_p = strstr(p, "\"y\"");
            if (y_p) sscanf(y_p, "\"y\"%*[ : \t]%d", &y);
            
            const char* w_p = strstr(p, "\"w\"");
            if (w_p) sscanf(w_p, "\"w\"%*[ : \t]%d", &w);
            
            const char* h_p = strstr(p, "\"h\"");
            if (h_p) sscanf(h_p, "\"h\"%*[ : \t]%d", &h);
            
            const char* ease_p = strstr(p, "\"ease\"");
            if (ease_p) {
                char e_str[64] = {0};
                sscanf(ease_p, "\"ease\"%*[ : \t\"]%63[^\"]\"", e_str);
                if (strstr(e_str, "in_out_cubic")) ease = WINMAN_EASE_IN_OUT_CUBIC;
                else if (strstr(e_str, "out_cubic")) ease = WINMAN_EASE_OUT_CUBIC;
                else if (strstr(e_str, "in_cubic")) ease = WINMAN_EASE_IN_CUBIC;
                else if (strstr(e_str, "sine")) ease = WINMAN_EASE_SINE;
            }

            WINMAN_RECT r = {x, y, x+w, y+h};
            kf[kf_count].at = at;
            kf[kf_count].bounds = r;
            kf[kf_count].ease = ease;
            kf_count++;
            
            p = strchr(p, '}');
            if (!p) break;
        }
        p++;
    }
    
    if (kf_count > 0) {
        winman_animate_keyframes(hwnd, kf, kf_count);
    }
}

// Multi-Window Scene Player
typedef struct {
    HWND hwnd;
    WINMAN_KEYFRAME kf[32];
    int kf_count;
} WinmanSceneWindow;

void winman_play_scene_from_json(const char* json_string) {
    WinmanSceneWindow scene[16] = {0};
    int win_count = 0;
    
    // Naive array-of-objects parsing for demo purposes
    const char* p = json_string;
    while (*p && win_count < 16) {
        const char* win_start = strstr(p, "\"id\"");
        if (!win_start) break;
        
        // Check if create is an object
        bool create = false;
        bool circular = false;
        char title[64] = "Winman Window";
        char bg_hex[16] = "#000000";
        
        const char* create_p = strstr(win_start, "\"create\"");
        if (create_p) {
            const char* create_val = strchr(create_p, ':');
            if (create_val) {
                while (*create_val == ' ' || *create_val == ':') create_val++;
                if (*create_val == '{') {
                    create = true;
                    // Parse title, bg, circular
                    const char* title_p = strstr(create_val, "\"title\"");
                    if (title_p) sscanf(title_p, "\"title\"%*[ : \t\"]%63[^\"]\"", title);
                    const char* bg_p = strstr(create_val, "\"bg\"");
                    if (bg_p) sscanf(bg_p, "\"bg\"%*[ : \t\"]%15[^\"]\"", bg_hex);
                    const char* circ_p = strstr(create_val, "\"circular\"");
                    if (circ_p) {
                        char bool_val[10] = {0};
                        sscanf(circ_p, "\"circular\"%*[ : \t]%9[^,}]", bool_val);
                        if (strstr(bool_val, "true")) circular = true;
                    }
                }
            }
        }
        
        HWND hwnd = NULL;
        if (create) {
            WINMAN_COLOR c = winman_color_from_hex(bg_hex);
            hwnd = winman_create_shaped_window(title, -1, -1, 100, 100, winman_color_to_colorref(c), circular, NULL);
        }
        
        // Parse keyframes array
        const char* kfs_p = strstr(win_start, "\"keyframes\"");
        int kf_count = 0;
        if (kfs_p) {
            const char* kf_start = strchr(kfs_p, '[');
            const char* kf_end = strchr(kfs_p, ']');
            if (kf_start && kf_end) {
                const char* kp = kf_start;
                while (kp < kf_end && kf_count < 32) {
                    if (*kp == '{') {
                        float at = 0.0f;
                        int x=0, y=0, w=0, h=0;
                        WinmanEasing ease = WINMAN_EASE_LINEAR;
                        
                        const char* t_p = strstr(kp, "\"at\"");
                        if (t_p && t_p < kf_end) sscanf(t_p, "\"at\"%*[ : \t]%f", &at);
                        const char* x_p = strstr(kp, "\"x\"");
                        if (x_p && x_p < kf_end) sscanf(x_p, "\"x\"%*[ : \t]%d", &x);
                        const char* y_p = strstr(kp, "\"y\"");
                        if (y_p && y_p < kf_end) sscanf(y_p, "\"y\"%*[ : \t]%d", &y);
                        const char* w_p = strstr(kp, "\"w\"");
                        if (w_p && w_p < kf_end) sscanf(w_p, "\"w\"%*[ : \t]%d", &w);
                        const char* h_p = strstr(kp, "\"h\"");
                        if (h_p && h_p < kf_end) sscanf(h_p, "\"h\"%*[ : \t]%d", &h);
                        const char* e_p = strstr(kp, "\"ease\"");
                        if (e_p && e_p < kf_end) {
                            char e_str[64]={0};
                            sscanf(e_p, "\"ease\"%*[ : \t\"]%63[^\"]\"", e_str);
                            if (strstr(e_str, "in_out_cubic")) ease = WINMAN_EASE_IN_OUT_CUBIC;
                            else if (strstr(e_str, "out_cubic")) ease = WINMAN_EASE_OUT_CUBIC;
                            else if (strstr(e_str, "in_cubic")) ease = WINMAN_EASE_IN_CUBIC;
                            else if (strstr(e_str, "sine")) ease = WINMAN_EASE_SINE;
                        }
                        
                        WINMAN_RECT r = {x, y, x+w, y+h};
                        scene[win_count].kf[kf_count].at = at;
                        scene[win_count].kf[kf_count].bounds = r;
                        scene[win_count].kf[kf_count].ease = ease;
                        kf_count++;
                        
                        kp = strchr(kp, '}');
                        if (!kp) break;
                    }
                    kp++;
                }
            }
        }
        
        scene[win_count].hwnd = hwnd;
        scene[win_count].kf_count = kf_count;
        if (hwnd && kf_count > 0) {
            win_count++;
        }
        
        // Advance pointer to next window object
        const char* next_win = strstr(win_start + 4, "\"id\"");
        if (!next_win) break;
        p = next_win;
    }
    
    if (win_count == 0) return;
    
    // Find max duration
    double max_duration_ms = 0;
    for (int i=0; i<win_count; i++) {
        if (scene[i].kf_count > 0) {
            double dur = scene[i].kf[scene[i].kf_count - 1].at * 1000.0;
            if (dur > max_duration_ms) max_duration_ms = dur;
        }
    }
    
    LARGE_INTEGER freq, t_start, t_now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t_start);

    while (1) {
        QueryPerformanceCounter(&t_now);
        double elapsed_ms = (double)(t_now.QuadPart - t_start.QuadPart) * 1000.0 / (double)freq.QuadPart;
        
        if (elapsed_ms >= max_duration_ms) {
            for (int i=0; i<win_count; i++) {
                if (scene[i].kf_count > 0) {
                    WINMAN_RECT end = scene[i].kf[scene[i].kf_count - 1].bounds;
                    winman_window_set_bounds(scene[i].hwnd, end.left, end.top, winman_rect_width(end), winman_rect_height(end));
                }
            }
            break;
        }

        double current_sec = elapsed_ms / 1000.0;
        
        for (int i=0; i<win_count; i++) {
            if (scene[i].kf_count < 2) continue;
            
            // Find segment
            int seg = 0;
            for (int j = 0; j < scene[i].kf_count - 1; j++) {
                if (current_sec >= scene[i].kf[j].at && current_sec <= scene[i].kf[j+1].at) {
                    seg = j;
                    break;
                }
            }

            WINMAN_KEYFRAME start_kf = scene[i].kf[seg];
            WINMAN_KEYFRAME end_kf = scene[i].kf[seg+1];

            double span = end_kf.at - start_kf.at;
            double local_t = (span > 0) ? (current_sec - start_kf.at) / span : 1.0;
            if (current_sec < start_kf.at) local_t = 0.0; // clamp before start
            if (current_sec > end_kf.at) local_t = 1.0;   // clamp after end

            double eased_t = winman_apply_ease(local_t, end_kf.ease);

            int sx = start_kf.bounds.left, sy = start_kf.bounds.top;
            int sw = winman_rect_width(start_kf.bounds), sh = winman_rect_height(start_kf.bounds);
            int ex = end_kf.bounds.left, ey = end_kf.bounds.top;
            int ew = winman_rect_width(end_kf.bounds), eh = winman_rect_height(end_kf.bounds);

            int curr_x = sx + (int)((ex - sx) * eased_t);
            int curr_y = sy + (int)((ey - sy) * eased_t);
            int curr_w = sw + (int)((ew - sw) * eased_t);
            int curr_h = sh + (int)((eh - sh) * eased_t);

            winman_window_set_bounds(scene[i].hwnd, curr_x, curr_y, curr_w, curr_h);
        }
        Sleep(1);
    }
}

// Native Custom Controls
static LRESULT CALLBACK winman_default_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    } else if (msg == WM_KEYDOWN) {
        if (wparam == VK_ESCAPE) {
            DestroyWindow(hwnd);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

HWND winman_create_shaped_window(
    const char* title, 
    int x, int y, int width, int height, 
    COLORREF bg_color, 
    bool circular,
    WinmanWndProc wndproc
) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    const char* class_name = "WinmanCustomClass";

    WNDCLASSA wc = {0};
    wc.style = 0;
    wc.lpfnWndProc = wndproc ? wndproc : winman_default_wndproc;
    wc.hInstance = hInstance;
    wc.hbrBackground = CreateSolidBrush(bg_color);
    wc.lpszClassName = class_name;

    RegisterClassA(&wc); 

    DWORD style = WS_POPUP | WS_VISIBLE;
    if (x == -1 || y == -1) { x = 100; y = 100; }

    HWND hwnd = CreateWindowExA(0, class_name, title, style, x, y, width, height, NULL, NULL, hInstance, NULL);
    if (hwnd && circular) {
        HRGN hrgn = CreateEllipticRgn(0, 0, width, height);
        SetWindowRgn(hwnd, hrgn, TRUE);
    }
    return hwnd;
}

void winman_run_message_loop(void) {
    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

HWND winman_create_status_bar(HWND parent, const char* text, int height, WINMAN_COLOR bg, WINMAN_COLOR fg) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WINMAN_RECT r = winman_window_get_rect(parent);
    int pw = winman_rect_width(r);
    int ph = winman_rect_height(r);

    HWND hwnd = CreateWindowExA(0, "STATIC", text, WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX,
        0, ph - height, pw, height, parent, NULL, hInstance, NULL);
    
    // Note: Colors for STATIC controls require handling WM_CTLCOLORSTATIC in the parent's WndProc.
    // winman.h simplifies this by returning the HWND to the user to manage.
    return hwnd;
}

/* --------------------------------------------------------------------------
 * winman_window_set_polygon_shape
 * -------------------------------------------------------------------------- */
BOOL winman_window_set_polygon_shape(HWND hwnd, const POINT* pts, int count, int fill_mode) {
    if (!hwnd || !pts || count < 3) return FALSE;
    HRGN hrgn = CreatePolygonRgn(pts, count, fill_mode);
    if (!hrgn) return FALSE;
    /* SetWindowRgn takes ownership of the region handle; do NOT DeleteObject it. */
    return SetWindowRgn(hwnd, hrgn, TRUE) != 0;
}

void winman_window_clear_shape(HWND hwnd) {
    if (hwnd) SetWindowRgn(hwnd, NULL, TRUE);
}

/* --------------------------------------------------------------------------
 * winman_desktop_get_icon_positions
 *
 * Walk:  Progman  ->  SHELLDLL_DefView  ->  SysListView32
 * Then use cross-process VirtualAllocEx + LVM_GETITEMPOSITION to read
 * each icon's POINT from the shell listview's process space.
 * -------------------------------------------------------------------------- */
#include <commctrl.h>

int winman_desktop_get_icon_positions(WINMAN_DESKTOP_ICON* icons_out, int max_icons) {
    if (!icons_out || max_icons <= 0) return -1;

    /* 1. Find SysListView32 that backs the desktop icons. */
    HWND progman = FindWindowA("Progman", NULL);
    if (!progman) return -1;

    /* Windows 7+ can have the icons under WorkerW instead of Progman.
       Try the usual Progman child first, then walk WorkerW children. */
    HWND shell_defview = FindWindowExA(progman, NULL, "SHELLDLL_DefView", NULL);
    HWND listview      = NULL;

    if (shell_defview) {
        listview = FindWindowExA(shell_defview, NULL, "SysListView32", NULL);
    }

    if (!listview) {
        /* Scan every WorkerW for the ShellView */
        HWND worker = FindWindowExA(NULL, NULL, "WorkerW", NULL);
        while (worker && !listview) {
            HWND sv = FindWindowExA(worker, NULL, "SHELLDLL_DefView", NULL);
            if (sv) listview = FindWindowExA(sv, NULL, "SysListView32", NULL);
            worker = FindWindowExA(NULL, worker, "WorkerW", NULL);
        }
    }

    if (!listview) return -1;

    /* 2. Open the process that owns the listview. */
    DWORD pid = 0;
    GetWindowThreadProcessId(listview, &pid);
    HANDLE hProc = OpenProcess(
        PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION,
        FALSE, pid);
    if (!hProc) return -1;

    /* 3. Allocate a POINT-sized buffer inside the remote process. */
    POINT* remote_pt = (POINT*)VirtualAllocEx(hProc, NULL, sizeof(POINT),
                                               MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remote_pt) { CloseHandle(hProc); return -1; }

    /* Allocate a buffer large enough for LVITEMA (used for the label). */
    char* remote_text = (char*)VirtualAllocEx(hProc, NULL, MAX_PATH,
                                               MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remote_text) {
        VirtualFreeEx(hProc, remote_pt, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return -1;
    }

    LVITEMA* remote_lvi = (LVITEMA*)VirtualAllocEx(hProc, NULL, sizeof(LVITEMA),
                                                    MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remote_lvi) {
        VirtualFreeEx(hProc, remote_text, 0, MEM_RELEASE);
        VirtualFreeEx(hProc, remote_pt,   0, MEM_RELEASE);
        CloseHandle(hProc);
        return -1;
    }

    /* 4. Iterate items. */
    int item_count = (int)SendMessageA(listview, LVM_GETITEMCOUNT, 0, 0);
    int found = 0;

    for (int i = 0; i < item_count && found < max_icons; i++) {
        /* Get position via LVM_GETITEMPOSITION (writes POINT to remote buffer). */
        SendMessageA(listview, LVM_GETITEMPOSITION, (WPARAM)i, (LPARAM)remote_pt);
        POINT local_pt = {0};
        ReadProcessMemory(hProc, remote_pt, &local_pt, sizeof(POINT), NULL);

        /* Build a local LVITEMA with its pszText pointing into remote process. */
        LVITEMA local_lvi = {0};
        local_lvi.mask       = LVIF_TEXT;
        local_lvi.iItem      = i;
        local_lvi.iSubItem   = 0;
        local_lvi.pszText    = remote_text;  /* pointer valid in remote address space */
        local_lvi.cchTextMax = MAX_PATH;
        WriteProcessMemory(hProc, remote_lvi, &local_lvi, sizeof(LVITEMA), NULL);
        SendMessageA(listview, LVM_GETITEMA, (WPARAM)i, (LPARAM)remote_lvi);

        char local_text[MAX_PATH] = {0};
        ReadProcessMemory(hProc, remote_text, local_text, MAX_PATH - 1, NULL);

        icons_out[found].x = local_pt.x;
        icons_out[found].y = local_pt.y;
        strncpy(icons_out[found].name, local_text, MAX_PATH - 1);
        icons_out[found].name[MAX_PATH - 1] = '\0';
        found++;
    }

    /* 5. Clean up. */
    VirtualFreeEx(hProc, remote_lvi,  0, MEM_RELEASE);
    VirtualFreeEx(hProc, remote_text, 0, MEM_RELEASE);
    VirtualFreeEx(hProc, remote_pt,   0, MEM_RELEASE);
    CloseHandle(hProc);

    return found;
}

/* --------------------------------------------------------------------------
 * SIMULATION INTERNALS
 * -------------------------------------------------------------------------- */
static volatile bool _winman_sim_running = false;
static HHOOK _winman_h_kb_hook = NULL;
static HANDLE _winman_h_audio_thread = NULL;

/* --------------------------------------------------------------------------
 * winman_set_shell_visibility
 * -------------------------------------------------------------------------- */
void winman_set_shell_visibility(bool visible) {
    int cmd = visible ? SW_SHOW : SW_HIDE;

    // 1. Taskbar(s)
    HWND hTaskbar = FindWindowA("Shell_TrayWnd", NULL);
    if (hTaskbar) ShowWindow(hTaskbar, cmd);

    HWND hSecondaryTaskbar = FindWindowExA(NULL, NULL, "SecondaryTrayWnd", NULL);
    while (hSecondaryTaskbar) {
        ShowWindow(hSecondaryTaskbar, cmd);
        hSecondaryTaskbar = FindWindowExA(NULL, hSecondaryTaskbar, "SecondaryTrayWnd", NULL);
    }

    // 2. Desktop Icons (SHELLDLL_DefView)
    HWND progman = FindWindowA("Progman", NULL);
    HWND shell_defview = FindWindowExA(progman, NULL, "SHELLDLL_DefView", NULL);
    if (!shell_defview) {
        HWND worker = FindWindowExA(NULL, NULL, "WorkerW", NULL);
        while (worker && !shell_defview) {
            shell_defview = FindWindowExA(worker, NULL, "SHELLDLL_DefView", NULL);
            worker = FindWindowExA(NULL, worker, "WorkerW", NULL);
        }
    }
    if (shell_defview) ShowWindow(shell_defview, cmd);
}

/* --------------------------------------------------------------------------
 * winman_show_fake_bsod
 * -------------------------------------------------------------------------- */
typedef struct {
    const char* stop_code;
    int progress;
    DWORD start_time;
    DWORD duration;
    bool is_primary;
} WinmanBSODState;

/**
 * Low-level keyboard hook callback.
 * This blocks keys that would allow the user to minimize or escape the simulation.
 */
static LRESULT CALLBACK _winman_low_level_kb_proc(int nCode, WPARAM wp, LPARAM lp) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lp;
        bool is_sys = (p->flags & LLKHF_ALTDOWN);
        // Block Windows Keys, Alt+Tab, Alt+Esc, Ctrl+Esc
        if (p->vkCode == VK_LWIN || p->vkCode == VK_RWIN || 
           (p->vkCode == VK_TAB && is_sys) || 
           (p->vkCode == VK_ESCAPE && is_sys) ||
           (p->vkCode == VK_ESCAPE && GetKeyState(VK_CONTROL) < 0)) {
            return 1; // Sink the input
        }
    }
    return CallNextHookEx(_winman_h_kb_hook, nCode, wp, lp);
}

/**
 * Audio thread simulating the 'stuck driver' buzz.
 */
static DWORD WINAPI _winman_audio_buzz_proc(LPVOID lpParam) {
    while (_winman_sim_running) {
        Beep(440, 100); // 440Hz for 100ms
        Sleep(10);
    }
    return 0;
}

static void _winman_draw_fake_qr(HDC hdc, int x, int y, int size) {
    int modules = 21;
    int mod_size = size / modules;
    RECT rc = { x, y, x + size, y + size };
    FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
    HBRUSH blue = CreateSolidBrush(RGB(0, 120, 215));
    for (int r = 0; r < modules; r++) {
        for (int c = 0; c < modules; c++) {
            bool draw = false;
            if ((r < 7 && c < 7) || (r < 7 && c >= modules - 7) || (r >= modules - 7 && c < 7)) {
                int lr = (r >= modules - 7) ? r - (modules - 7) : r;
                int lc = (c >= modules - 7) ? c - (modules - 7) : c;
                if (lr == 0 || lr == 6 || lc == 0 || lc == 6 || (lr >= 2 && lr <= 4 && lc >= 2 && lc <= 4)) draw = true;
            } else {
                if (((r * 7 + c * 13 + (r|c)) % 3) == 0) draw = true;
                if (r == 6 || c == 6) draw = (c % 2 == 0) || (r % 2 == 0);
            }
            if (draw) {
                RECT mod_rc = { x + c * mod_size, y + r * mod_size, x + (c + 1) * mod_size, y + (r + 1) * mod_size };
                FillRect(hdc, &mod_rc, blue);
            }
        }
    }
    DeleteObject(blue);
}

static LRESULT CALLBACK _winman_bsod_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    WinmanBSODState* state = (WinmanBSODState*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);
    if (msg == WM_KEYDOWN || msg == WM_LBUTTONDOWN) {
        DestroyWindow(hwnd);
        return 0;
    }
    if (msg == WM_TIMER) {
        if (wp == 1 && state) {
            DWORD elapsed = GetTickCount() - state->start_time;
            DWORD total = state->duration > 0 ? state->duration : 10000;
            int new_prog = (int)((elapsed * 100) / total);
            if (new_prog > 100) new_prog = 100;
            if (new_prog != state->progress) {
                state->progress = new_prog;
                InvalidateRect(hwnd, NULL, FALSE);
            }
        } else if (wp == 2) {
            DestroyWindow(hwnd);
        }
        return 0;
    }
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        
        // Windows 11 BSOD Blue
        HBRUSH brush = CreateSolidBrush(RGB(0, 120, 215));
        FillRect(hdc, &rc, brush);
        DeleteObject(brush);
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);
        HFONT hSad   = CreateFontA(140, 0, 0, 0, FW_LIGHT,  FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
        HFONT hMsg   = CreateFontA(32,  0, 0, 0, FW_LIGHT,  FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
        HFONT hSmall = CreateFontA(18,  0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
        int margin = 150;
        SelectObject(hdc, hSad);
        TextOutA(hdc, margin, margin, ":(", 2);
        SelectObject(hdc, hMsg);
        TextOutA(hdc, margin, margin + 180, "Your device ran into a problem and needs to restart. We're just", 64);
        TextOutA(hdc, margin, margin + 225, "collecting some error info, and then we'll restart for you.", 59);

        char prog_str[32];
        sprintf(prog_str, "%d%% complete", state ? state->progress : 0);
        TextOutA(hdc, margin, margin + 310, prog_str, (int)strlen(prog_str));

        int qr_size = 120, qr_y = margin + 410;
        _winman_draw_fake_qr(hdc, margin, qr_y, qr_size);
        
        SelectObject(hdc, hSmall);
        int tx = margin + qr_size + 30;
        TextOutA(hdc, tx, qr_y + 10, "For more information about this issue and possible fixes, visit", 63);
        TextOutA(hdc, tx, qr_y + 35, "https://www.windows.com/stopcode", 32);

        char stop_str[256];
        sprintf(stop_str, "Stop code: %s", state ? state->stop_code : "CRITICAL_PROCESS_DIED");
        TextOutA(hdc, tx, qr_y + 80, stop_str, (int)strlen(stop_str));

        DeleteObject(hSad);
        DeleteObject(hMsg);
        DeleteObject(hSmall);
        EndPaint(hwnd, &ps);
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

/**
 * Monitor enumeration callback to create one window per screen.
 */
static BOOL CALLBACK _winman_spawn_sim_windows(HMONITOR hMon, HDC hdc, LPRECT lpRect, LPARAM lp) {
    HINSTANCE hInst = GetModuleHandle(NULL);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hMon, &mi);

    HWND hwnd = CreateWindowExA(WS_EX_TOPMOST, (const char*)lp, "WinmanSim", WS_POPUP | WS_VISIBLE,
                                mi.rcMonitor.left, mi.rcMonitor.top, 
                                mi.rcMonitor.right - mi.rcMonitor.left, 
                                mi.rcMonitor.bottom - mi.rcMonitor.top, 
                                NULL, NULL, hInst, NULL);
    
    // Mark if this is the primary monitor so we know where to draw the UI
    bool is_primary = (mi.dwFlags & MONITORINFOF_PRIMARY);
    SetWindowLongPtrA(hwnd, GWL_ID, is_primary ? 1 : 0); 
    return TRUE;
}

void winman_show_fake_bsod(const char* stop_code, DWORD duration_ms) {
    HINSTANCE hInst = GetModuleHandle(NULL);
    WNDCLASSA wc = {0};
    wc.lpfnWndProc   = _winman_bsod_proc;
    wc.hInstance     = hInst;
    wc.lpszClassName = "WinmanFakeBSOD";
    wc.hCursor       = NULL; // Hide cursor for immersion
    RegisterClassA(&wc);

    _winman_sim_running = true;
    
    // 1. Multi-Monitor setup
    EnumDisplayMonitors(NULL, NULL, _winman_spawn_sim_windows, (LPARAM)wc.lpszClassName);

    // 2. Keyboard Lock (LL Hook)
    _winman_h_kb_hook = SetWindowsHookExA(WH_KEYBOARD_LL, _winman_low_level_kb_proc, hInst, 0);

    // 3. Audio Grind Thread
    _winman_h_audio_thread = CreateThread(NULL, 0, _winman_audio_buzz_proc, NULL, 0, NULL);
    SetThreadPriority(_winman_h_audio_thread, THREAD_PRIORITY_TIME_CRITICAL);

    // Find the primary window to attach state to
    HWND hwnd = FindWindowExA(NULL, NULL, wc.lpszClassName, NULL);
    while (hwnd && GetWindowLongPtrA(hwnd, GWL_ID) == 0) hwnd = FindWindowExA(NULL, hwnd, wc.lpszClassName, NULL);
    
    WinmanBSODState state;
    state.stop_code = stop_code ? stop_code : "CRITICAL_PROCESS_DIED";
    state.progress = 0;
    state.start_time = GetTickCount();
    state.duration = duration_ms;
    state.is_primary = true;

    if (hwnd) {
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)&state);
        SetTimer(hwnd, 1, 50, NULL);
        if (duration_ms > 0) SetTimer(hwnd, 2, duration_ms, NULL);
    }

    MSG msg;
    while (IsWindow(hwnd) && GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    // Cleanup
    _winman_sim_running = false;
    if (_winman_h_kb_hook) UnhookWindowsHookEx(_winman_h_kb_hook);
    if (_winman_h_audio_thread) {
        WaitForSingleObject(_winman_h_audio_thread, 500);
        CloseHandle(_winman_h_audio_thread);
    }

    // Destroy all simulation windows
    HWND hOther;
    while ((hOther = FindWindowA(wc.lpszClassName, NULL))) DestroyWindow(hOther);

    UnregisterClassA("WinmanFakeBSOD", hInst);
}

/* --------------------------------------------------------------------------
 * winman_show_fake_update_screen
 * -------------------------------------------------------------------------- */

/**
 * Internal state structure for the fake update simulation.
 * Tracks progress percent and timing information for animation.
 */
typedef struct {
    int progress;      /* Current percentage (0-100) */
    DWORD start_time;  /* When the simulation started (GetTickCount) */
    DWORD duration;    /* Total intended duration in ms */
    float rotation;    /* Current rotation angle for the spinner animation */
    bool is_primary;   /* Whether this is the primary monitor */
} WinmanUpdateState;


/**
 * Window procedure for the fake update screen.
 * Handles rendering the blue background, spinner dots, and progress text.
 */
static LRESULT CALLBACK _winman_update_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    // Retrieve the state structure passed via GWLP_USERDATA during creation.
    WinmanUpdateState* state = (WinmanUpdateState*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);

    // Any user interaction (key or mouse) closes the fake screen.
    if (msg == WM_KEYDOWN || msg == WM_LBUTTONDOWN) {
        DestroyWindow(hwnd);
        return 0;
    }

    if (msg == WM_TIMER) {
        if (wp == 1 && state) {
            // Animation and progress update logic.
            DWORD elapsed = GetTickCount() - state->start_time;
            DWORD total = state->duration > 0 ? state->duration : 60000;
            
            // Smoothly calculate progress based on time elapsed.
            int new_prog = (int)((elapsed * 100) / total);
            if (new_prog > 100) new_prog = 100;
            
            // Increment rotation for the spinner chase effect.
            state->rotation += 0.15f;
            if (state->rotation > 2.0f * 3.14159f) state->rotation -= 2.0f * 3.14159f;

            state->progress = new_prog;
            InvalidateRect(hwnd, NULL, FALSE);
        } else if (wp == 2) {
            // Auto-close when the simulated duration is reached.
            DestroyWindow(hwnd);
        }
        return 0;
    }

    if (msg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        
        // Paint the background with the specific Windows 10/11 update blue.
        HBRUSH brush = CreateSolidBrush(RGB(0, 120, 215));
        FillRect(hdc, &rc, brush);
        DeleteObject(brush);

        // If this isn't the primary monitor, we just leave it solid (black or blue)
        if (state && !state->is_primary) {
            EndPaint(hwnd, &ps);
            return 0;
        }

        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);

        // Load Segoe UI fonts to match the Windows system look.
        HFONT hLarge = CreateFontA(42, 0, 0, 0, FW_LIGHT,  FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
        HFONT hMed   = CreateFontA(24, 0, 0, 0, FW_LIGHT,  FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");

        int cx = (rc.right - rc.left) / 2;
        int cy = (rc.bottom - rc.top) / 2;

        // Draw the iconic spinning "circle of dots".
        if (state) {
            int dot_radius = 45;
            HBRUSH dot_brush = CreateSolidBrush(RGB(255, 255, 255));
            for (int i = 0; i < 6; i++) {
                // We stagger the angle for 6 dots to create the "chase" animation.
                float angle = state->rotation + (i * 0.45f);
                int dx = cx + (int)(cos(angle) * dot_radius);
                int dy = (cy - 120) + (int)(sin(angle) * dot_radius);
                RECT dot_rc = { dx - 4, dy - 4, dx + 4, dy + 4 };
                FillRect(hdc, &dot_rc, dot_brush);
            }
            DeleteObject(dot_brush);
        }

        // Draw the primary "Working on updates X%" text.
        char prog_str[64];
        sprintf(prog_str, "Working on updates  %d%%", state ? state->progress : 0);
        SelectObject(hdc, hLarge);
        DrawTextA(hdc, prog_str, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);

        // Draw secondary warning messages below the center.
        SelectObject(hdc, hMed);
        RECT msg_rc = rc;
        msg_rc.top = cy + 80;
        DrawTextA(hdc, "Don't turn off your PC. This will take a while.", -1, &msg_rc, DT_CENTER | DT_TOP | DT_SINGLELINE | DT_NOCLIP);

        msg_rc.top += 40;
        DrawTextA(hdc, "Your computer may restart a few times.", -1, &msg_rc, DT_CENTER | DT_TOP | DT_SINGLELINE | DT_NOCLIP);

        // Clean up fonts and finish painting.
        DeleteObject(hLarge);
        DeleteObject(hMed);
        EndPaint(hwnd, &ps);
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

/**
 * winman_show_fake_update_screen
 * Launches a full-screen simulation of the Windows Update process.
 * 
 * Features:
 * - Automatically covers all monitors using SM_XVIRTUALSCREEN.
 * - Animates a spinning dot circle and progress percentage.
 * - Blocks other UI elements using WS_EX_TOPMOST.
 */
void winman_show_fake_update_screen(DWORD duration_ms) {
    HINSTANCE hInst = GetModuleHandle(NULL);
    WNDCLASSA wc = {0};
    wc.lpfnWndProc   = _winman_update_proc;
    wc.hInstance     = hInst;
    wc.lpszClassName = "WinmanFakeUpdate";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);

    // Get the dimensions of the entire virtual workspace.
    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    // Create the full-screen, always-on-top popup window.
    HWND hwnd = CreateWindowExA(WS_EX_TOPMOST, wc.lpszClassName, "Update", WS_POPUP | WS_VISIBLE,
                                vx, vy, vw, vh, NULL, NULL, hInst, NULL);

    // Setup state and timers.
    WinmanUpdateState state;
    state.progress = 0;
    state.start_time = GetTickCount();
    state.duration = duration_ms > 0 ? duration_ms : 60000;
    state.rotation = 0.0f;

    // Attach state to the window so the WndProc can use it.
    SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)&state);
    
    // Timer 1: Animation/Progress updates (~20 FPS).
    SetTimer(hwnd, 1, 50, NULL);
    
    // Timer 2: The "Finish line" timer.
    SetTimer(hwnd, 2, state.duration, NULL);

    // Standard message loop to keep the window responsive.
    MSG msg;
    while (IsWindow(hwnd) && GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    
    UnregisterClassA("WinmanFakeUpdate", hInst);
}

#ifdef __cplusplus
}
#endif

#ifdef WINMAN_ENABLE_D2D

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")

#ifdef __cplusplus
#define WINMAN_UUID_OF(type) __uuidof(type)
#else
#define WINMAN_UUID_OF(type) &IID_##type
#endif

// Helpers for COM calls in C vs C++
#ifdef __cplusplus
#define WINMAN_RELEASE(obj) do { if(obj) { (obj)->Release(); (obj)=NULL; } } while(0)
#else
#define WINMAN_RELEASE(obj) do { if(obj) { ((IUnknown*)(obj))->lpVtbl->Release((IUnknown*)(obj)); (obj)=NULL; } } while(0)
#endif

static D2D1_COLOR_F _winman_color_to_d2d(WINMAN_COLOR c) {
    D2D1_COLOR_F d2dc;
    d2dc.r = c.r / 255.0f;

    d2dc.g = c.g / 255.0f;
    d2dc.b = c.b / 255.0f;
    d2dc.a = 1.0f;
    return d2dc;
}

static D2D1_RECT_F _winman_rect_to_d2d(WINMAN_RECT r) {
    D2D1_RECT_F d2dr;
    d2dr.left = (float)r.left;
    d2dr.top = (float)r.top;
    d2dr.right = (float)r.right;
    d2dr.bottom = (float)r.bottom;
    return d2dr;
}

bool winman_d2d_init(HWND hwnd, WINMAN_D2D_CONTEXT* ctx) {
    if (!ctx) return false;
    memset(ctx, 0, sizeof(WINMAN_D2D_CONTEXT));
    ctx->hwnd = hwnd;

    // Initialize COM (required for WIC and DirectWrite)
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    // Create D2D Factory
#ifdef __cplusplus
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), NULL, (void**)&ctx->d2d_factory);
#else
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &IID_ID2D1Factory, NULL, (void**)&ctx->d2d_factory);
#endif
    if (FAILED(hr)) return false;

    // Create DWrite Factory
#ifdef __cplusplus
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&ctx->dwrite_factory);
#else
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory, (IUnknown**)&ctx->dwrite_factory);
#endif
    if (FAILED(hr)) return false;

    // Create WIC Factory
#ifdef __cplusplus
    hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (void**)&ctx->wic_factory);
#else
    hr = CoCreateInstance(&CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, &IID_IWICImagingFactory, (void**)&ctx->wic_factory);
#endif

    // Create Render Target
    RECT rc;
    GetClientRect(hwnd, &rc);
    D2D1_SIZE_U size;
    size.width = rc.right - rc.left;
    size.height = rc.bottom - rc.top;

    D2D1_RENDER_TARGET_PROPERTIES rtProps;
    rtProps.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
    rtProps.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    rtProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    rtProps.dpiX = 0.0f;
    rtProps.dpiY = 0.0f;
    rtProps.usage = D2D1_RENDER_TARGET_USAGE_NONE;
    rtProps.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;

    D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps;
    hwndProps.hwnd = hwnd;
    hwndProps.pixelSize = size;
    hwndProps.presentOptions = D2D1_PRESENT_OPTIONS_NONE;

#ifdef __cplusplus
    hr = ctx->d2d_factory->CreateHwndRenderTarget(&rtProps, &hwndProps, &ctx->render_target);
#else
    hr = ID2D1Factory_CreateHwndRenderTarget(ctx->d2d_factory, &rtProps, &hwndProps, &ctx->render_target);
#endif
    if (FAILED(hr)) return false;

    // Create Default Brush
    D2D1_COLOR_F d2dColor = {1.0f, 1.0f, 1.0f, 1.0f};
#ifdef __cplusplus
    ctx->render_target->CreateSolidColorBrush(&d2dColor, NULL, &ctx->default_brush);
#else
    ID2D1RenderTarget_CreateSolidColorBrush((ID2D1RenderTarget*)ctx->render_target, &d2dColor, NULL, &ctx->default_brush);
#endif

    return true;
}

void winman_d2d_cleanup(WINMAN_D2D_CONTEXT* ctx) {
    if (!ctx) return;
    WINMAN_RELEASE(ctx->default_brush);
    WINMAN_RELEASE(ctx->render_target);
    WINMAN_RELEASE(ctx->wic_factory);
    WINMAN_RELEASE(ctx->dwrite_factory);
    WINMAN_RELEASE(ctx->d2d_factory);
    CoUninitialize();
}

void winman_d2d_resize(WINMAN_D2D_CONTEXT* ctx, int width, int height) {
    if (ctx && ctx->render_target) {
        D2D1_SIZE_U size;
        size.width = width;
        size.height = height;
#ifdef __cplusplus
        ctx->render_target->Resize(&size);
#else
        ID2D1HwndRenderTarget_Resize(ctx->render_target, &size);
#endif
    }
}

void winman_d2d_begin_draw(WINMAN_D2D_CONTEXT* ctx) {
    if (ctx && ctx->render_target) {
#ifdef __cplusplus
        ctx->render_target->BeginDraw();
#else
        ID2D1RenderTarget_BeginDraw((ID2D1RenderTarget*)ctx->render_target);
#endif
    }
}

bool winman_d2d_end_draw(WINMAN_D2D_CONTEXT* ctx) {
    if (ctx && ctx->render_target) {
#ifdef __cplusplus
        HRESULT hr = ctx->render_target->EndDraw(NULL, NULL);
#else
        HRESULT hr = ID2D1RenderTarget_EndDraw((ID2D1RenderTarget*)ctx->render_target, NULL, NULL);
#endif
        return SUCCEEDED(hr);
    }
    return false;
}

void winman_d2d_clear(WINMAN_D2D_CONTEXT* ctx, WINMAN_COLOR color) {
    if (ctx && ctx->render_target) {
        D2D1_COLOR_F d2dc = _winman_color_to_d2d(color);
#ifdef __cplusplus
        ctx->render_target->Clear(&d2dc);
#else
        ID2D1RenderTarget_Clear((ID2D1RenderTarget*)ctx->render_target, (const void*)&d2dc);
#endif
    }
}

static void _winman_d2d_set_brush_color(WINMAN_D2D_CONTEXT* ctx, WINMAN_COLOR color) {
    D2D1_COLOR_F d2dc = _winman_color_to_d2d(color);
#ifdef __cplusplus
    ctx->default_brush->SetColor(&d2dc);
#else
    ID2D1SolidColorBrush_SetColor(ctx->default_brush, (const void*)&d2dc);
#endif
}

void winman_d2d_draw_line(WINMAN_D2D_CONTEXT* ctx, int x1, int y1, int x2, int y2, WINMAN_COLOR color, float stroke_width) {
    if (!ctx || !ctx->render_target) return;
    _winman_d2d_set_brush_color(ctx, color);
    D2D1_POINT_2F p0 = {(float)x1, (float)y1};
    D2D1_POINT_2F p1 = {(float)x2, (float)y2};
#ifdef __cplusplus
    ctx->render_target->DrawLine(p0, p1, (ID2D1Brush*)ctx->default_brush, stroke_width, NULL);
#else
    ID2D1RenderTarget_DrawLine((ID2D1RenderTarget*)ctx->render_target, p0, p1, (ID2D1Brush*)ctx->default_brush, stroke_width, NULL);
#endif
}

void winman_d2d_draw_rect(WINMAN_D2D_CONTEXT* ctx, WINMAN_RECT rect, WINMAN_COLOR color, float stroke_width) {
    if (!ctx || !ctx->render_target) return;
    _winman_d2d_set_brush_color(ctx, color);
    D2D1_RECT_F r = _winman_rect_to_d2d(rect);
#ifdef __cplusplus
    ctx->render_target->DrawRectangle(&r, (ID2D1Brush*)ctx->default_brush, stroke_width, NULL);
#else
    ID2D1RenderTarget_DrawRectangle((ID2D1RenderTarget*)ctx->render_target, (const void*)&r, (ID2D1Brush*)ctx->default_brush, stroke_width, NULL);
#endif
}

void winman_d2d_fill_rect(WINMAN_D2D_CONTEXT* ctx, WINMAN_RECT rect, WINMAN_COLOR color) {
    if (!ctx || !ctx->render_target) return;
    _winman_d2d_set_brush_color(ctx, color);
    D2D1_RECT_F r = _winman_rect_to_d2d(rect);
#ifdef __cplusplus
    ctx->render_target->FillRectangle(&r, (ID2D1Brush*)ctx->default_brush);
#else
    ID2D1RenderTarget_FillRectangle((ID2D1RenderTarget*)ctx->render_target, (const void*)&r, (ID2D1Brush*)ctx->default_brush);
#endif
}

void winman_d2d_draw_ellipse(WINMAN_D2D_CONTEXT* ctx, int cx, int cy, int rx, int ry, WINMAN_COLOR color, float stroke_width) {
    if (!ctx || !ctx->render_target) return;
    _winman_d2d_set_brush_color(ctx, color);
    D2D1_ELLIPSE e;
    e.point.x = (float)cx; e.point.y = (float)cy;
    e.radiusX = (float)rx; e.radiusY = (float)ry;
#ifdef __cplusplus
    ctx->render_target->DrawEllipse(&e, (ID2D1Brush*)ctx->default_brush, stroke_width, NULL);
#else
    ID2D1RenderTarget_DrawEllipse((ID2D1RenderTarget*)ctx->render_target, (const void*)&e, (ID2D1Brush*)ctx->default_brush, stroke_width, NULL);
#endif
}

void winman_d2d_fill_ellipse(WINMAN_D2D_CONTEXT* ctx, int cx, int cy, int rx, int ry, WINMAN_COLOR color) {
    if (!ctx || !ctx->render_target) return;
    _winman_d2d_set_brush_color(ctx, color);
    D2D1_ELLIPSE e;
    e.point.x = (float)cx; e.point.y = (float)cy;
    e.radiusX = (float)rx; e.radiusY = (float)ry;
#ifdef __cplusplus
    ctx->render_target->FillEllipse(&e, (ID2D1Brush*)ctx->default_brush);
#else
    ID2D1RenderTarget_FillEllipse((ID2D1RenderTarget*)ctx->render_target, (const void*)&e, (ID2D1Brush*)ctx->default_brush);
#endif
}

WINMAN_D2D_BITMAP winman_d2d_load_bitmap(WINMAN_D2D_CONTEXT* ctx, const char* filepath) {
    WINMAN_D2D_BITMAP out = {0};
    if (!ctx || !ctx->render_target || !ctx->wic_factory) return out;

    wchar_t wpath[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, filepath, -1, wpath, MAX_PATH);

    IWICBitmapDecoder* decoder = NULL;
#ifdef __cplusplus
    HRESULT hr = ctx->wic_factory->CreateDecoderFromFilename(wpath, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
#else
    HRESULT hr = IWICImagingFactory_CreateDecoderFromFilename(ctx->wic_factory, wpath, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
#endif
    if (FAILED(hr)) return out;

    IWICBitmapFrameDecode* frame = NULL;
#ifdef __cplusplus
    hr = decoder->GetFrame(0, &frame);
#else
    hr = IWICBitmapDecoder_GetFrame(decoder, 0, &frame);
#endif
    if (FAILED(hr)) { WINMAN_RELEASE(decoder); return out; }

    IWICFormatConverter* converter = NULL;
#ifdef __cplusplus
    hr = ctx->wic_factory->CreateFormatConverter(&converter);
#else
    hr = IWICImagingFactory_CreateFormatConverter(ctx->wic_factory, &converter);
#endif
    if (SUCCEEDED(hr)) {
#ifdef __cplusplus
        hr = converter->Initialize((IWICBitmapSource*)frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);
        if (SUCCEEDED(hr)) {
            hr = ctx->render_target->CreateBitmapFromWicBitmap((IWICBitmapSource*)converter, NULL, &out.bitmap);
        }
#else
        hr = IWICFormatConverter_Initialize(converter, (IWICBitmapSource*)frame, &GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);
        if (SUCCEEDED(hr)) {
            hr = ID2D1RenderTarget_CreateBitmapFromWicBitmap((ID2D1RenderTarget*)ctx->render_target, (IWICBitmapSource*)converter, NULL, &out.bitmap);
        }
#endif
        WINMAN_RELEASE(converter);
    }
    WINMAN_RELEASE(frame);
    WINMAN_RELEASE(decoder);

    if (out.bitmap) {
#ifdef __cplusplus
        D2D1_SIZE_F size = out.bitmap->GetSize();
#else
        D2D1_SIZE_F size = ID2D1Bitmap_GetSize(out.bitmap);
#endif
        out.width = (int)size.width;
        out.height = (int)size.height;
    }
    return out;
}

void winman_d2d_draw_bitmap(WINMAN_D2D_CONTEXT* ctx, WINMAN_D2D_BITMAP* bitmap, int x, int y, int width, int height, float opacity) {
    if (!ctx || !ctx->render_target || !bitmap || !bitmap->bitmap) return;
    D2D1_RECT_F dest = {(float)x, (float)y, (float)(x+width), (float)(y+height)};
#ifdef __cplusplus
    ctx->render_target->DrawBitmap(bitmap->bitmap, &dest, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);
#else
    ID2D1RenderTarget_DrawBitmap((ID2D1RenderTarget*)ctx->render_target, bitmap->bitmap, (const void*)&dest, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);
#endif
}

void winman_d2d_destroy_bitmap(WINMAN_D2D_BITMAP* bitmap) {
    if (bitmap && bitmap->bitmap) {
        WINMAN_RELEASE(bitmap->bitmap);
    }
}

WINMAN_DWRITE_FORMAT winman_dwrite_create_format(WINMAN_D2D_CONTEXT* ctx, const char* font_family, float size) {
    WINMAN_DWRITE_FORMAT out = {0};
    if (!ctx || !ctx->dwrite_factory) return out;

    wchar_t wfamily[64];
    MultiByteToWideChar(CP_UTF8, 0, font_family, -1, wfamily, 64);

#ifdef __cplusplus
    ctx->dwrite_factory->CreateTextFormat(
        wfamily, NULL,
        DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        size, L"en-us", &out.format);
#else
    IDWriteFactory_CreateTextFormat(ctx->dwrite_factory,
        wfamily, NULL,
        DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        size, L"en-us", &out.format);
#endif
        
    return out;
}

void winman_d2d_draw_text(WINMAN_D2D_CONTEXT* ctx, const char* text, WINMAN_DWRITE_FORMAT* format, WINMAN_RECT rect, WINMAN_COLOR color) {
    if (!ctx || !ctx->render_target || !format || !format->format) return;
    _winman_d2d_set_brush_color(ctx, color);
    
    int len = (int)strlen(text);
    wchar_t* wtext = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, text, -1, wtext, len + 1);
    
    D2D1_RECT_F r = _winman_rect_to_d2d(rect);
#ifdef __cplusplus
    ctx->render_target->DrawText(wtext, len, format->format, &r, (ID2D1Brush*)ctx->default_brush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
#else
    ID2D1RenderTarget_DrawText((ID2D1RenderTarget*)ctx->render_target, wtext, len, format->format, (const void*)&r, (ID2D1Brush*)ctx->default_brush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
#endif
    
    free(wtext);
}

void winman_dwrite_destroy_format(WINMAN_DWRITE_FORMAT* format) {
    if (format && format->format) {
        WINMAN_RELEASE(format->format);
    }
}


#endif // WINMAN_ENABLE_D2D

#endif // WINMAN_IMPLEMENTATION
