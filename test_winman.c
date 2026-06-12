#define WINMAN_IMPLEMENTATION
#include "winman.h"
#include <stdio.h>

int main(int argc, char* argv[]) {

    /* ----------------------------------------------------------------
     * 1. DESKTOP ICON SCRAPER
     * ---------------------------------------------------------------- */
    printf("=== Desktop Icon Scraper ===\n");

    WINMAN_DESKTOP_ICON icons[256];
    int icon_count = winman_desktop_get_icon_positions(icons, 256);

    if (icon_count < 0) {
        printf("  [!] Could not access the desktop SysListView32.\n"
               "      Make sure icons are visible on the desktop and you\n"
               "      are running as the interactive user (not as admin in\n"
               "      a restricted session).\n\n");
    } else {
        printf("  Found %d desktop icon(s):\n", icon_count);
        for (int i = 0; i < icon_count; i++) {
            printf("  [%3d]  %-40s  @ (%d, %d)\n",
                   i, icons[i].name, icons[i].x, icons[i].y);
        }
        printf("\n");
    }

    /* ----------------------------------------------------------------
     * 2. POLYGON SHAPED WINDOW  (star shape)
     * ---------------------------------------------------------------- */
    printf("=== Polygon Shaped Window (star) ===\n");

    /* Create a 500x500 borderless window first, then cut it into a star. */
    HWND hw = winman_create_shaped_window(
        "Poly Test", 400, 200, 500, 500,
        RGB(0x1a, 0x8c, 0xff),   /* bright blue */
        false, NULL);

    if (!hw) {
        printf("  [!] Failed to create window.\n");
        return 1;
    }

    /* 5-pointed star, scaled to fit a 500x500 box.
       Computed with outer radius 230 and inner radius 95, centred at (250,250). */
    POINT star[] = {
        {250,  20},   /* tip 0  (top)       */
        {295, 150},   /* inner right of tip 0 */
        {450, 150},   /* tip 1 right         */
        {330, 240},   /* inner right of 1    */
        {390, 400},   /* tip 2 right         */
        {250, 305},   /* centre bottom inner */
        {110, 400},   /* tip 3 left          */
        {170, 240},   /* inner left of 1     */
        { 50, 150},   /* tip 4 left          */
        {205, 150},   /* inner left of tip 0 */
    };

    BOOL ok = winman_window_set_polygon_shape(hw, star, 10, WINDING);
    printf("  Star polygon applied: %s\n\n", ok ? "OK" : "FAILED");

    printf("Press ESC on the star window to exit.\n");
    winman_run_message_loop();

    return 0;
}
