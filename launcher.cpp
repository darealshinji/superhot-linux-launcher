/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2017, djcj <djcj@gmx.de>
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

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Tiled_Image.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <FL/filename.H>

#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <libgen.h>
#include <X11/Xlib.h>

#include <limits.h>
#ifndef EXEEXT
#  if (__WORDSIZE == 64)
#    define EXEEXT "x86_64"
#  else
#    define EXEEXT "x86"
#  endif
#endif

#define VENDOR "SUPERHOT team"
#define MAX_MONITORS 32

#ifndef ENABLE_BINRELOC
#  define ENABLE_BINRELOC
#endif
#include "binreloc.h"

#include "resources.h"

#ifndef WINDOW_DECORATION
#  define WINDOW_DECORATION 0
#endif

//#define DEBUG 1  /* makes boxes visible */
#if (DEBUG == 1)
#  define SETBOXTYPE(o)  o->box(FL_FLAT_BOX); o->color(FL_BLUE)
#else
#  define SETBOXTYPE(o)  o->box(FL_NO_BOX)
#endif

#define GETPREFS(entry, value, default, max) \
  prefs.get(entry, value, -1); \
  if (value < 0 || value > max) { value = default; }


class uri_box : public Fl_Box
{
  public:

    uri_box(int X, int Y, int W, int H, const char *L=0)
      : Fl_Box(X, Y, W, H, L) { }

    virtual ~uri_box() { }

    int handle(int event);
};

#if (WINDOW_DECORATION == 0)
class move_box : public Fl_Box
{
  public:

    move_box(int X, int Y, int W, int H, const char *L=0)
      : Fl_Box(X, Y, W, H, L) { }

    virtual ~move_box() { }

    int handle(int event);
};
#endif

int uri_box::handle(int event)
{
  int ret = Fl_Box::handle(event);
  switch (event)
  {
    case FL_PUSH:
      do_callback();
      break;
    case FL_MOVE:
      fl_cursor(FL_CURSOR_HAND);
      break;
    case FL_LEAVE:
      fl_cursor(FL_CURSOR_DEFAULT);
      break;
  }
  return ret;
}

#if (WINDOW_DECORATION == 0)
int event_x_pos = 0;
int event_y_pos = 0;

int move_box::handle(int event)
{
  int ret = Fl_Box::handle(event);
  switch (event)
  {
    case FL_PUSH:
      fl_cursor(FL_CURSOR_MOVE);
      event_x_pos = Fl::event_x();
      event_y_pos = Fl::event_y();
      ret = event;  /* must be non-zero */
      break;
    case FL_DRAG:
      do_callback();
      break;
    case FL_RELEASE:
      fl_cursor(FL_CURSOR_DEFAULT);
      break;
  }
  return ret;
}
#endif


Fl_Window *win;
Fl_Menu_Button *resolution_selection, *screen_selection, *language_selection;
bool checkbutton_set;
bool launch_game = false;
int val_quality;

std::string selection_text[3];
enum {
  TEXT_RES,
  TEXT_SCREEN,
  TEXT_LANG
};

struct resolutionData {
  int w;
  int h;
  const char *l;
};

#include "menu_item_lists.h"


int default_lang()
{
  std::string lang = std::string(getenv("LANG")).substr(0,2);

  if (lang == "")
  {
    lang = std::string(getenv("LANGUAGE")).substr(0,2);
  }
  CHECK_L10N;
}

std::string itostr(int i)
{
  std::stringstream ss;
  ss << i;
  return ss.str();
}

int get_number_of_screens()
{
  Display *dp = XOpenDisplay(":0.0");
  int val = ScreenCount(dp);
  XCloseDisplay(dp);

  if (val < 1)
  {
    val = 1;
  }
  return val;
}

static void open_uri_cb(Fl_Widget *, void *p)
{
  const char *uri = (char *)p;
  char errmsg[512];

  if (!fl_open_uri(uri, errmsg, sizeof(errmsg)))
  {
    std::cerr << "Error: " << errmsg << std::endl;
  }
}

static void resolution_selection_cb(Fl_Widget *)
{
  selection_text[TEXT_RES] = " " + std::string(resolution_selection->text());
  resolution_selection->label(selection_text[TEXT_RES].c_str());
}

static void screen_selection_cb(Fl_Widget *)
{
  selection_text[TEXT_SCREEN] = " " + std::string(screen_selection->text());
  screen_selection->label(selection_text[TEXT_SCREEN].c_str());
}

static void language_selection_cb(Fl_Widget *)
{
  selection_text[TEXT_LANG] = " " + std::string(language_selection->text());
  language_selection->label(selection_text[TEXT_LANG].c_str());
}

static void checkbutton_cb(Fl_Widget *)
{
  checkbutton_set = (checkbutton_set) ? false : true;
}

static void rb_callback(Fl_Widget *, long p)
{
  val_quality = (int) p;
}

#if (WINDOW_DECORATION == 0)
static void move_box_cb(Fl_Widget *)
{
  int x = Fl::event_x_root() - event_x_pos;
  int y = Fl::event_y_root() - event_y_pos;
  win->position(x, y);
}
#endif

static void launch_cb(Fl_Widget *)
{
  launch_game = true;
  win->hide();
}

static void close_cb(Fl_Widget *)
{
  win->hide();
}


int main(int argc, char **argv)
{
  Fl_Menu_Item monitor_items[MAX_MONITORS];
  std::string monitor_entry[MAX_MONITORS];

  BrInitError brError;
  int val_res, val_screen, screens_avail, is_fullscreen, val_lang;

  /* satisfying section 4 of the FLTK license's LGPL exception */
  std::cout << "using FLTK version " PRINT_VERSION " (http://www.fltk.org)" << std::endl;

  /* get exe full path */
  if (!br_init(&brError))
  {
    std::cerr << "*** BinReloc failed to initialize. Error: " << brError << std::endl;
  }
  char *exe = basename(argv[0]);
  char *exedir = br_find_exe_dir(dirname(argv[0]));

  /* check configurations */
  Fl_Preferences prefs(exedir, VENDOR, exe);
  GETPREFS("resolution", val_res, MAX_RES, MAX_RES);
  GETPREFS("fullscreen", is_fullscreen, 1, 1);
  GETPREFS("language", val_lang, default_lang(), MAX_LANG);
  GETPREFS("quality", val_quality, 0, 1);
  screens_avail = get_number_of_screens();
  GETPREFS("screen", val_screen, 0, screens_avail - 1);

#if (DEBUG == 1)
  /* test screen selection */
  screens_avail = 5;
#endif

  for (int i = 0; i <= screens_avail; ++i)
  {
    if (i < screens_avail)
    {
      monitor_entry[i] = "Screen " + itostr(i + 1);
      monitor_items[i].text = monitor_entry[i].c_str();
    }
    else
    {
      monitor_items[i].text = 0;
    }
    monitor_items[i].shortcut_ = 0;
    monitor_items[i].callback_ = 0;
    monitor_items[i].user_data_ = 0;
    monitor_items[i].flags = 0;
    monitor_items[i].labeltype_ = (i == screens_avail) ? 0 : FL_NORMAL_LABEL;
    monitor_items[i].labelfont_ = 0;
    monitor_items[i].labelsize_ = (i == screens_avail) ? 0 : 14;
    monitor_items[i].labelcolor_ = 0;
  }

  Fl_PNG_Image win_icon(NULL, icon_png, (int) icon_png_len);
  Fl_Window::default_icon(&win_icon);

  Fl_Color selection_color = fl_rgb_color(200, 18, 0);
  Fl::background(200, 18, 0);

  win = new Fl_Window(400, 600, "Launch SUPERHOT");
  win->callback(close_cb);
  {
    Fl_Group *g        = new Fl_Group(0, 0, 400, 600);
    Fl_PNG_Image *bg   = new Fl_PNG_Image(NULL, launcher_png, (int) launcher_png_len);
    Fl_Tiled_Image *wp = new Fl_Tiled_Image(bg);
    g->image(wp);
    g->align(FL_ALIGN_INSIDE);
    {
#if (WINDOW_DECORATION == 0)
      { move_box *o = new move_box(0, 0, 400, 600);
        o->box(FL_NO_BOX);
        o->callback(move_box_cb); }

      { Fl_Button *o = new Fl_Button(366, 10, 24, 24);
        o->box(FL_NO_BOX);
        o->clear_visible_focus();
        o->callback(close_cb); }
      { uri_box *o = new uri_box(366, 10, 24, 24, "X");
        SETBOXTYPE(o);
        o->labelcolor(fl_rgb_color(250, 115, 105));
        o->labelsize(30);
        o->labelfont(FL_HELVETICA_BOLD);
        o->clear_visible_focus(); }
#endif

      { Fl_Box *o = new Fl_Box(83, 142, 246, 26, "RESOLUTION");
        o->align(FL_ALIGN_TOP_LEFT);
        o->box(FL_NO_BOX);
        o->labelcolor(FL_WHITE); }
      { Fl_Menu_Button *o = resolution_selection = new Fl_Menu_Button(83, 146, 246, 26);
        o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
        o->box(FL_THIN_DOWN_BOX);
        o->down_box(FL_THIN_DOWN_BOX);
        o->color(FL_WHITE);
        o->selection_color(selection_color);
        o->clear_visible_focus();
        o->menu(resolution_items);
        o->value(val_res);
        selection_text[TEXT_RES] = " " + std::string(o->text());
        o->label(selection_text[TEXT_RES].c_str());
        o->callback(resolution_selection_cb); }

      { Fl_Check_Button *o = new Fl_Check_Button(81, 176, 110, 26, " WINDOWED");
        SETBOXTYPE(o);
        o->labelcolor(FL_WHITE);
        o->clear_visible_focus();
        if (is_fullscreen == 0)
        {
          checkbutton_set = true;
          o->setonly();
        }
        o->callback(checkbutton_cb); }

      { Fl_Box *o = new Fl_Box(83, 234, 246, 26, "SELECT MONITOR");
        o->align(FL_ALIGN_TOP_LEFT);
        o->box(FL_NO_BOX);
        o->labelcolor(FL_WHITE); }
      { Fl_Menu_Button *o = screen_selection = new Fl_Menu_Button(83, 238, 246, 26);
        o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
        o->box(FL_THIN_DOWN_BOX);
        o->down_box(FL_THIN_DOWN_BOX);
        o->color(FL_WHITE);
        o->selection_color(selection_color);
        o->clear_visible_focus();
        if (screens_avail == 1)
        {
          o->deactivate();
        }
        o->menu(monitor_items);
        o->value(val_screen);
        selection_text[TEXT_SCREEN] = " " + std::string(o->text());
        o->label(selection_text[TEXT_SCREEN].c_str());
        o->callback(screen_selection_cb); }

      { Fl_Box *o = new Fl_Box(83, 300, 246, 26, "LANGUAGE");
        o->align(FL_ALIGN_TOP_LEFT);
        o->box(FL_NO_BOX);
        o->labelcolor(FL_WHITE); }
      { Fl_Menu_Button *o = language_selection = new Fl_Menu_Button(83, 304, 246, 26);
        o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
        o->box(FL_THIN_DOWN_BOX);
        o->down_box(FL_THIN_DOWN_BOX);
        o->color(FL_WHITE);
        o->selection_color(selection_color);
        o->clear_visible_focus();
        o->menu(language_items);
        o->value(val_lang);
        selection_text[TEXT_LANG] = " " + std::string(o->text());
        o->label(selection_text[TEXT_LANG].c_str());
        o->callback(language_selection_cb); }

      { Fl_Box *o = new Fl_Box(83, 368, 246, 26, "QUALITY");
        o->align(FL_ALIGN_TOP_LEFT);
        o->box(FL_NO_BOX);
        o->labelcolor(FL_WHITE); }
      { Fl_Radio_Round_Button *o[2];
        o[0] = new Fl_Radio_Round_Button(81, 370, 62, 26, " HIGH");
        o[0]->labelcolor(FL_WHITE);
        o[0]->clear_visible_focus();
        o[0]->callback(rb_callback, 0);
        o[1] = new Fl_Radio_Round_Button(165, 370, 58, 26, " LOW");
        o[1]->labelcolor(FL_WHITE);
        o[1]->clear_visible_focus();
        o[1]->callback(rb_callback, 1);
        SETBOXTYPE(o[0]);
        SETBOXTYPE(o[1]);
        o[val_quality]->setonly(); }

      { Fl_Button *o = new Fl_Button(83, 434, 246, 50, "START SUPERHOT");
        o->align(FL_ALIGN_CENTER);
        o->box(FL_FLAT_BOX);
        o->down_box(FL_FLAT_BOX);
        o->labelcolor(FL_WHITE);
        Fl_Color button_color = fl_rgb_color(93, 0, 0);
        o->color(button_color);
        o->down_color(button_color);
        o->clear_visible_focus();
        o->callback(launch_cb); }
      { /* change cursor to FL_CURSOR_HAND above start button */
        uri_box *o = new uri_box(83, 434, 246, 50);
        o->box(FL_NO_BOX);
        o->clear_visible_focus(); }

      { uri_box *o = new uri_box(83, 531, 29, 23);
        SETBOXTYPE(o);
        o->tooltip("Twitter");
        o->clear_visible_focus();
        o->callback(open_uri_cb, (void *)"https://twitter.com/superhotthegame"); }

      { uri_box *o = new uri_box(154, 530, 30, 30);
        SETBOXTYPE(o);
        o->tooltip("Facebook");
        o->clear_visible_focus();
        o->callback(open_uri_cb, (void *)"https://www.facebook.com/superhotgame"); }

      { uri_box *o = new uri_box(228, 531, 29, 29);
        SETBOXTYPE(o);
        o->tooltip("Instagram");
        o->clear_visible_focus();
        o->callback(open_uri_cb, (void *)"https://www.instagram.com/superhotthegame/"); }

      { uri_box *o = new uri_box(300, 535, 30, 22);
        SETBOXTYPE(o);
        o->tooltip("Mail");
        o->clear_visible_focus();
        o->callback(open_uri_cb, (void *)"mailto:people@superhotgame.com"); }
    }
    g->end();
  }

  /* center window */
  int pos_x = (Fl::w() - win->w()) / 2;
  int pos_y = (Fl::h() - win->h()) / 2;
  win->position(pos_x, pos_y);

  win->end();
  win->show();

#if (WINDOW_DECORATION == 0)
  /* Use this _after_ show() to remove the WM decoration
   * but keep the taskbar entry */
  win->border(0);
#endif

  Fl::run();

  val_res       = resolution_selection->value();
  val_lang      = language_selection->value();
  val_screen    = screen_selection->value();
  is_fullscreen = (checkbutton_set) ? 0 : 1;

  /* write config file */
  prefs.set("resolution", val_res);
  prefs.set("fullscreen", is_fullscreen);
  prefs.set("language", val_lang);
  prefs.set("quality", val_quality);
  prefs.set("screen", val_screen);

  if (launch_game)
  {
    Fl::check();

    std::string quality[] = { "HighEnd", "LowEnd" };

    std::string command = "'" + std::string(exedir) + "/SUPERHOT." EXEEXT + "'"
      " -adapter "           + itostr(val_screen) +
      " -screen-fullscreen " + itostr(is_fullscreen) +
      " -screen-width "      + itostr(resolutions[val_res].w) +
      " -screen-height "     + itostr(resolutions[val_res].h) +
      " -language "          + l10n[val_lang][0] +
      " -screen-quality "    + quality[val_quality];

    if (argc > 1 && (std::string(argv[1]) == "--verbose" ||
                     std::string(argv[1]) == "-verbose"))
    {
      std::cout << "execute command: " << command << std::endl;
    }

    command += " ; echo \"exit code: $?\"";
    return system(command.c_str());
  }

  return 0;
}

