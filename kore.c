#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
Display *d;
Window root;
XEvent ev;
// Client *clients = NULL;
typedef union {
  const char **v;
  unsigned int ui;
  int i;
} Arg;

typedef struct {
  unsigned int mod;
  KeySym keysym;
  void (*func)(const Arg *arg);
  const Arg arg;
} Key;

typedef struct Client Client;
struct Client {
  Window win;
  unsigned int tags; // <- Bitmask
  Client *next;
};

Client *clients = NULL;
Client *sel = NULL;

unsigned int sel_tag = 1;
void tag(const Arg *arg);
void view(const Arg *arg);
void tile();
void spawn(const Arg *arg);
void quit(const Arg *arg);
void killclient(const Arg *arg);
void focusstack(const Arg *arg);
void focus(Client *c);
void zoom(const Arg *arg);
void detach(Client *c);
void attach(Client *c);

#include "config.h"

int xerror(Display *d, XErrorEvent *ee) {
  if (ee->error_code == BadAccess) {
    fprintf(stderr, "Error: Ya hay otro WM ejecutandose\n");
    exit(1);
  }
  fprintf(stderr, "Error: %d\n", ee->error_code);
  return 0;
}

void spawn(const Arg *arg) {
  if (fork() == 0) {
    if (fork() == 0) {
      execvp(((char **)arg->v)[0], (char **)arg->v);
    }
    exit(0);
  }
}

void quit(const Arg *arg) { exit(0); }

void killclient(const Arg *arg) {
  if (!sel)
    return;
  XKillClient(d, sel->win);
}

/*void killclient(const Arg *arg)
{
        if (!clients)
                return;

        Window w;
        int revert;
        XGetInputFocus(d, &w, &revert);

        if (w != root && w != None) {

        XEvent ev;
        ev.type = ClientMessage;
        ev.xclient.window = w;
        ev.xclient.message_type = XInternAtom(d, "WM_PROTOCOLS", True);
        ev.xclient.format = 32;
        ev.xclient.data.l[0] = XInternAtom(d, "WM_DELETE_WINDOW", True);
        ev.xclient.data.l[1] = CurrentTime;

        XSendEvent(d, w, False, NoEventMask, &ev);
        }
} */

void grabkeys(Display *d, Window root) {
  XUngrabKey(d, AnyKey, AnyModifier, root);
  for (int i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
    XGrabKey(d, XKeysymToKeycode(d, keys[i].keysym), keys[i].mod, root, True,
             GrabModeAsync, GrabModeAsync);
  }
}

void manage(Display *d, Window w) {
  Client *c = malloc(sizeof(Client));
  c->win = w;
  c->tags = sel_tag;
  c->next = clients;
  clients = c;

  XSetWindowBorderWidth(d, w, 2);

  XSetWindowBorder(d, w, COLOR_FOCUS);

  XSelectInput(d, w,
               EnterWindowMask | FocusChangeMask | PropertyChangeMask |
                   StructureNotifyMask);

  focus(c);
}

void unmanage(Display *d, Window w) {
  Client **tc = &clients;
  while (*tc && (*tc)->win != w)
    tc = &(*tc)->next;
  if (*tc) {
    Client *detach = *tc;
    *tc = (*tc)->next;
    free(detach);
  }
}

void tile() {
  XWindowAttributes wa;
  XGetWindowAttributes(d, root, &wa); // tamano de pantalla

  int n = 0;
  Client *c;
  /*for (c = clients; c; c = c->next)
     n++;
     if (n == 0)
     return; */

  for (c = clients; c; c = c->next)
    if (c->tags & sel_tag)
      n++;

  int gappx = 10;
  int mw = (n == 1) ? wa.width : wa.width * 0.6; // ancho del Master 60%
  int i = 0;

  for (c = clients; c; c = c->next) {
    if (c->tags & sel_tag) { /* si la ventana pertenece al tag actual */
      int x, y, w, h;
      int border = 2;
      if (i == 0) {
        x = gappx;
        y = gappx;
        w = mw - (gappx * (n == 1 ? 2 : 1));
        h = wa.height - (gappx * 2);
      } else { // STACK
        x = mw + gappx;
        w = wa.width - mw - (gappx * 2);

        /* altura base */
        h = (wa.height - (gappx * (n + 1))) / (n - 1);
        y = gappx + (i - 1) * (h + gappx);

        y = gappx + (i - 1) * (h + gappx);
        // si es la ultima ventana, estirarla hasta el fondo
        if (i == n - 1) {
          h = wa.height - y - gappx;
        }

        /*      x = mw + gappx;
           w = wa.width - mw - (gappx * 2);
           h = (wa.height - (gappx * (n))) / (n - 1);
           y = gappx + (i - 1) * (h + gappx); */
      }
      // int border = 2;
      XMoveResizeWindow(d, c->win, x, y, w - (border * 2), h - (border * 2));
      //			(d, c->win, border);
      // XSetWindowBorder(d, c->win, 0x81a1c1);
      XMapWindow(d, c->win);
      // XRaiseWindow(d, c->win);

      i++;
    } else {
      /*si no pertenece al tag actual */
      XMoveWindow(d, c->win, -2 * wa.width, 0);
    }
  }
}

void view(const Arg *arg) {
  if ((arg->ui & TAGMASK) == sel_tag)
    return;

  sel_tag = arg->ui & TAGMASK;
  focus(NULL);
  tile();
}

void focus(Client *c) {
  /* si ya habia una ventana enfocada, cambia el color */
  if (sel && sel != c) {
    XSetWindowBorder(d, sel->win, COLOR_UNFOCUS);
  }

  /* si la ventana existe*/
  if (c) {
    sel = c;
    XSetWindowBorder(d, c->win, COLOR_FOCUS);
    XSetInputFocus(d, c->win, RevertToParent, CurrentTime);
    XRaiseWindow(d, c->win);
  }
  XSync(d, False);
}

void tag(const Arg *arg) {
  if (sel && arg->ui & TAGMASK) {
    sel->tags = arg->ui & TAGMASK;
    focus(NULL);
    tile();
  }
}

void focusstack(const Arg *arg) {
  Client *c = NULL;

  if (!sel)
    return;
  if (arg->i > 0) {
    c = sel->next ? sel->next : clients;
  } else {
    Client *i;
    for (i = clients; i->next != sel; i = i->next)
      ;
    c = i ? i : clients;
  }
  if (c) {
    focus(c);
  }
}

void detach(Client *c) {
  Client **tc;
  for (tc = &clients; *tc && *tc != c; tc = &(*tc)->next)
    ;
  *tc = c->next;
}

void attach(Client *c) {
  c->next = clients;
  clients = c;
}

void zoom(const Arg *arg) {
  Client *c = sel;

  if (c == clients) {
    if (!(c = c->next))
      return;
  }
  detach(sel);
  attach(sel);
  focus(sel);
  tile();
}

Client *wintoclient(Window w) {
  Client *c;
  for (c = clients; c; c = c->next)
    if (c->win == w)
      return c;
  return NULL;
}

int main(void) {
  //      Display *d;
  //      XEvent ev;
  //      Window root;

  if (!(d = XOpenDisplay(NULL)))
    return 1;

  XSetErrorHandler(xerror);

  root = DefaultRootWindow(d);

  XSelectInput(d, root,
               SubstructureRedirectMask | SubstructureNotifyMask |
                   EnterWindowMask | KeyPressMask);
  grabkeys(d, root);

  XDefineCursor(d, root, XCreateFontCursor(d, 132));

  while (!XNextEvent(d, &ev)) {
    if (ev.type == KeyPress) {
      for (int i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
        if (ev.xkey.keycode == XKeysymToKeycode(d, keys[i].keysym) &&
            ev.xkey.state == keys[i].mod) {
          keys[i].func(&(keys[i].arg));
        }
      }
    } else if (ev.type == MapRequest) {
      //      XMapWindow(d, ev.xmaprequest.window);
      Window w = ev.xmaprequest.window;
      manage(d, w);
      XMapWindow(d, w);
      tile();
      XWindowChanges wc;

      //      wc.x = 50;
      //      wc.y = 50;
      //      wc.width = 640;
      //      wc.height = 480;
      //      wc.border_width = 3;

      XConfigureWindow(d, ev.xmaprequest.window,
                       CWX | CWY | CWWidth | CWHeight | CWBorderWidth, &wc);
      // XSetWindowBorder(d, ev.xmaprequest.window, 0x81a1c1);
      XMapWindow(d, ev.xmaprequest.window);
    } else if (ev.type == UnmapNotify || ev.type == DestroyNotify) {
      unmanage(d, ev.xunmap.window);
      tile();
    } else if (ev.type == EnterNotify) {
      focus(wintoclient(ev.xcrossing.window));
    }
  }

  XCloseDisplay(d);
  return 0;
}
