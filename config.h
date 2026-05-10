#define MODKEY Mod4Mask
#define TAGMASK ((1 << 9) - 1)
#define TAGKEYS(KEY, TAG)                                                      \
  {MODKEY, KEY, view, {.ui = 1 << TAG}},                                       \
      {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},
#define COLOR_FOCUS 0x81a1c1
#define COLOR_UNFOCUS 0x4c566a
static const char *tags[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};
static const char *termcmd[] = {"st", NULL};
static const char *exitcmd[] = {"pkill", "kore", NULL};
static const char *browser[] = {"firefox", NULL};

static Key keys[] = {{MODKEY, XK_Return, spawn, {.v = termcmd}},
                     {MODKEY, XK_c, killclient, {0}},
                     {MODKEY, XK_b, spawn, {.v = browser}},
                     {MODKEY, XK_Tab, zoom, {0}},
                     {MODKEY, XK_q, quit, {0}},
                     TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2)
                         TAGKEYS(XK_4, 3) TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5)
                             TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7)
                                 TAGKEYS(XK_9, 8)};
