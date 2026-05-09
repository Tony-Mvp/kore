#define MODKEY Mod4Mask
#define COLOR_FOCUS 0x81a1c1
#define COLOR_UNFOCUS 0x4c566a

static const char *termcmd[] = {"st", NULL};
static const char *exitcmd[] = {"pkill", "kore", NULL};
static const char *browser[] = {"firefox", NULL};

static Key keys[] = {
    {MODKEY, XK_Return, spawn, {.v = termcmd}},
    {MODKEY, XK_c, killclient, {0}},
    {MODKEY, XK_b, spawn, {.v = browser}},
    {MODKEY, XK_Tab, zoom, {0}},
    {MODKEY, XK_q, quit, {0}},
};
