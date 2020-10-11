/**
 * Screenshots
 */

#include <module.h>
#include <dryos.h>
#include <property.h>
#include <bmp.h>
#include <menu.h>
#include <config.h>
#include <screenshot.h>

static int running = 0;
static int stop = 0;
static int trigger = 0;

static void scrnshot_task()
{
    running = 1;
    TASK_LOOP
    {
        msleep(1000);
        if(stop) break;
        if(trigger) take_screenshot("SS_%04d.ppm", SCREENSHOT_BMP | SCREENSHOT_YUV);
        trigger = 0;
    }
    stop = 0;
    running = 0;
}

static MENU_SELECT_FUNC(scrnshot_start)
{
    if(!running)
        task_create("scrnshot_task", 0x1c, 0x1000, scrnshot_task, (void*)0);
    else
        stop = 1;
}

static MENU_UPDATE_FUNC(scrnshot_display)
{
    if(!running)
        MENU_SET_VALUE("Start");
    else
        MENU_SET_VALUE("Stop");
}

static struct menu_entry scrnshot_menu[] =
{
    {
        .name = "Screenshots",
        .update = scrnshot_display,
        .select = scrnshot_start,
        .help  = "Take screenshots each time you press a button",
    },
};

static unsigned int scrnshot_keypress_cbr(unsigned int key)
{
    trigger = 1;
    return 1;
}

static unsigned int scrnshot_init()
{
    menu_add("Debug", scrnshot_menu, COUNT(scrnshot_menu));
    return 0;
}

static unsigned int scrnshot_deinit()
{
    return 0;
}

MODULE_INFO_START()
    MODULE_INIT(scrnshot_init)
    MODULE_DEINIT(scrnshot_deinit)
MODULE_INFO_END()

MODULE_CBRS_START()
    MODULE_CBR(CBR_KEYPRESS, scrnshot_keypress_cbr, 0)
MODULE_CBRS_END()

MODULE_CONFIGS_START()
MODULE_CONFIGS_END()
