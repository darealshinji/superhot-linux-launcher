/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016, djcj <djcj@gmx.de>
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
#include <FL/Fl_Choice.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Tiled_Image.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <FL/filename.H>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

#ifndef ENABLE_BINRELOC
#  define ENABLE_BINRELOC
#endif
#include "binreloc.h"

#define STREQ(x, y) (strcmp(x, y) == 0)

#define ITEM(x)    { x, 0, 0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0 }
#define ITEM_INIT  { 0, 0, 0, 0, 0,               0, 0,  0, 0 }

#include <limits.h>
#ifndef EXEEXT
#  if (__WORDSIZE == 64)
#    define EXEEXT "x86_64"
#  else
#    define EXEEXT "x86"
#  endif
#endif

#include "resources.h"
#define BGIMAGE      launcher_png
#define BGIMAGE_LEN  launcher_png_len
#define ICON         icon_png
#define ICON_LEN     icon_png_len

//#define DEBUG 1  /* makes boxes visible */


static Fl_Window *win;
static bool checkbutton_set;
static bool launch_game = false;
static int val_quality;

class uri_box : public Fl_Box
{
  public:

    uri_box(int X, int Y, int W, int H, const char *L=0)
      : Fl_Box(X, Y, W, H, L) { }

    virtual ~uri_box() { }

    int handle(int event);
};

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

struct resolutionData {
  int w;
  int h;
  const char *l;
};

static const int resolutionsLength = 17;
static struct resolutionData resolutions[resolutionsLength] = {
  {  640,  480,   "640x480" },
  {  720,  480,   "720x480" },
  {  720,  576,   "720x576" },
  {  800,  600,   "800x600" },
  { 1024,  768,  "1024x768" },
  { 1152,  864,  "1152x864" },
  { 1280,  720,  "1280x720" },
  { 1280,  768,  "1280x768" },
  { 1280,  800,  "1280x800" },
  { 1280,  960,  "1280x960" },
  { 1280, 1024, "1280x1024" },
  { 1360,  768,  "1360x768" },
  { 1366,  768,  "1366x768" },
  { 1440,  900,  "1440x900" },
  { 1600,  900,  "1600x900" },
  { 1680, 1050, "1680x1050" },
  { 1920, 1080, "1920x1080" },
};

static const char *l10n[][2] = {
  { "EN", "English" },
  { "PL", "Polski" },
  { "FR", "Fran" "\xc3\xa7" "ais" },  /* Français */
  { "DE", "Deutsch" },
  { "RU", "\xd0\xa0" "\xd1\x83" "\xd1\x81" "\xd1\x81" "\xd0\xba" "\xd0\xb8" "\xd0\xb9" },  /* Русский */
  { "PT", "Portugu" "\xc3\xaa" "s do Brasil" },  /* Portuguêse do Brasil */
  { "IT", "Italiano" },
  { "CZ", "\xc4\x8c" "e" "\xc5\xa1" "tina" },  /* Čeština */
  { "HU", "Magyar" },
  { "SP", "Espa" "\xc3\xb1" "ol" }  /* Español */
};

static Fl_Menu_Item resolution_items[] = {
  ITEM(resolutions[0].l),
  ITEM(resolutions[1].l),
  ITEM(resolutions[2].l),
  ITEM(resolutions[3].l),
  ITEM(resolutions[4].l),
  ITEM(resolutions[5].l),
  ITEM(resolutions[6].l),
  ITEM(resolutions[7].l),
  ITEM(resolutions[8].l),
  ITEM(resolutions[9].l),
  ITEM(resolutions[10].l),
  ITEM(resolutions[11].l),
  ITEM(resolutions[12].l),
  ITEM(resolutions[13].l),
  ITEM(resolutions[14].l),
  ITEM(resolutions[15].l),
  ITEM(resolutions[16].l),
  ITEM_INIT
};

static Fl_Menu_Item language_items[] = {
  ITEM(l10n[0][1]),
  ITEM(l10n[1][1]),
  ITEM(l10n[2][1]),
  ITEM(l10n[3][1]),
  ITEM(l10n[4][1]),
  ITEM(l10n[5][1]),
  ITEM(l10n[6][1]),
  ITEM(l10n[7][1]),
  ITEM(l10n[8][1]),
  ITEM(l10n[9][1]),
  ITEM_INIT
};

static std::string itostr(int i)
{
  std::stringstream ss;
  ss << i;
  return ss.str();
}

static int readconf(std::string conf, std::string conffile, int defaultVal)
{
  FILE *readconf;
  /* I find forking a shell and using sed+head much easier and safer */
  std::string command = "(sed -n 's/^" + conf + "=//p' '" + conffile +
    "' 2>/dev/null||printf " + itostr(defaultVal) + ")|head -n1|head -c6";
  char getvar[16];
  int val = defaultVal;

  readconf = popen(command.c_str(), "r");
  int p = fscanf(readconf, "%s", getvar);
  pclose(readconf);

  if (p)
  {
    val = (int) strtol(getvar, NULL, 10);
  }

  if (val < 0)
  {
    val = defaultVal;
  }
  return val;
}

static int get_number_of_screens()
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

static void launch_cb(Fl_Widget *)
{
  launch_game = true;
  win->hide();
}

static void close_cb(Fl_Widget *)
{
  win->hide();
}

static void checkbutton_cb(Fl_Widget *)
{
  checkbutton_set = (checkbutton_set) ? false : true;
}

static void rb_callback(Fl_Widget *, long p)
{
  val_quality = (int) p;
}


int main(int argc, char **argv)
{
  Fl_Choice *resolution_selection, *screen_selection, *language_selection;
  Fl_Menu_Item monitor_items[128];
  std::string monitor_entry[128];

  BrInitError brError;
  char *exe, *exedir;
  std::string aux, defPath, conffile;
  int val_res, val_screen, screens_avail, is_fullscreen, val_lang;

  /* satisfying section 4 of the FLTK license's LGPL exception */
  std::cout << "using FLTK version " PRINT_VERSION " rev" PRINT_REVISION " (http://www.fltk.org)" << std::endl;

  /* get exe full path */
  if (!br_init(&brError))
  {
    std::cerr << "*** BinReloc failed to initialize. Error: ";
    switch (brError)
    {
      case BR_INIT_ERROR_NOMEM:
        std::cerr << "cannot allocate memory" << std::endl;
      case BR_INIT_ERROR_OPEN_MAPS:
        std::cerr << "unable to open /proc/self/maps" << std::endl;
      case BR_INIT_ERROR_READ_MAPS:
        std::cerr << "unable to read from /proc/self/maps" << std::endl;
      case BR_INIT_ERROR_INVALID_MAPS:
        std::cerr << "the file format of /proc/self/maps is invalid" << std::endl;
      default:
        std::cerr << brError << std::endl;
    }
  }
  exe = br_find_exe(argv[0]);
  aux = std::string(argv[0]);
  int pos  = aux.rfind('/');
  defPath  = aux.substr(0, pos + 1);
  exedir   = br_find_exe_dir(defPath.c_str());
  conffile = std::string(exe) + ".config";

  /* check configurations */
  val_res       = readconf("resolution", conffile, resolutionsLength - 1);
  is_fullscreen = readconf("fullscreen", conffile, 1);
  val_lang      = readconf("language", conffile, 0);
  val_quality   = readconf("quality", conffile, 0);
  val_screen    = readconf("screen", conffile, 0);
  screens_avail = get_number_of_screens();

  if (val_screen > (screens_avail - 1) || val_screen < 0)
  {
    val_screen = 0;
  }

  /* test screen selection */
  //screens_avail = 5;

  /* Needs C++11:
   * launcher.cpp:*: warning: extended initializer lists only available with -std=c++11 or -std=gnu++11
   */
  for (int i = 0; i < screens_avail; ++i)
  {
    monitor_entry[i] = "Screen " + itostr(i + 1);
    monitor_items[i] = ITEM(monitor_entry[i].c_str());
  }
  monitor_items[screens_avail] = ITEM_INIT;

  Fl_PNG_Image win_icon(NULL, ICON, (int) ICON_LEN);
  Fl_Window::default_icon(&win_icon);

  Fl_Color selection_color = fl_rgb_color(200, 18, 0);
  Fl::background(200, 18, 0);

  win = new Fl_Window(400, 600, "SUPERHOT Launcher");
  win->callback(close_cb);
  {
    Fl_Group *g        = new Fl_Group(0, 0, 400, 600);
    Fl_PNG_Image *bg   = new Fl_PNG_Image(NULL, BGIMAGE, (int) BGIMAGE_LEN);
    Fl_Tiled_Image *wp = new Fl_Tiled_Image(bg);
    g->image(wp);
    g->align(FL_ALIGN_INSIDE);
    {
      { Fl_Choice *o = resolution_selection = new Fl_Choice(80, 146, 246, 26, "RESOLUTION");
        o->align(FL_ALIGN_TOP_LEFT);
        o->labelcolor(FL_WHITE);
        o->selection_color(selection_color);
        o->clear_visible_focus();
        o->menu(resolution_items);
        o->value(val_res); }

      { Fl_Check_Button *o = new Fl_Check_Button(78, 176, 116, 26, " WINDOWED");
#if (DEBUG == 1)
        o->box(FL_UP_BOX);
        o->color(FL_BLUE);
#endif
        o->labelcolor(FL_WHITE);
        o->clear_visible_focus();
        if (is_fullscreen == 0)
        {
          checkbutton_set = true;
          o->setonly();
        }
        o->callback(checkbutton_cb); }

      { Fl_Choice *o = screen_selection = new Fl_Choice(80, 238, 246, 26, "SELECT MONITOR");
        o->align(FL_ALIGN_TOP_LEFT);
        o->labelcolor(FL_WHITE);
        o->selection_color(selection_color);
        o->clear_visible_focus();
        if (screens_avail == 1)
        {
          o->deactivate();
        }
        o->menu(monitor_items);
        o->value(val_screen); }

      { Fl_Choice *o = language_selection = new Fl_Choice(80, 304, 246, 26, "LANGUAGE");
        o->align(FL_ALIGN_TOP_LEFT);
        o->labelcolor(FL_WHITE);
        o->selection_color(selection_color);
        o->clear_visible_focus();
        o->menu(language_items);
        o->value(val_lang); }

      { Fl_Box *o = new Fl_Box(78, 368, 246, 26, "QUALITY");
        o->align(FL_ALIGN_TOP_LEFT);
        o->box(FL_NO_BOX);
        o->labelcolor(FL_WHITE); }

      { Fl_Radio_Round_Button *o[2];
        o[0] = new Fl_Radio_Round_Button(78, 370, 70, 26, " HIGH");
        o[0]->labelcolor(FL_WHITE);
        o[0]->clear_visible_focus();
        o[0]->callback(rb_callback, 0);
        o[1] = new Fl_Radio_Round_Button(160, 370, 66, 26, " LOW");
        o[1]->labelcolor(FL_WHITE);
        o[1]->clear_visible_focus();
        o[1]->callback(rb_callback, 1);
#if (DEBUG == 1)
        o[0]->box(FL_UP_BOX);
        o[1]->box(FL_UP_BOX);
        o[0]->color(FL_BLUE);
        o[1]->color(FL_BLUE);
#endif
        o[val_quality]->setonly(); }

      { Fl_Button *o = new Fl_Button(80, 434, 246, 50, "START SUPERHOT");
        o->align(FL_ALIGN_CENTER);
        o->box(FL_FLAT_BOX);
        o->down_box(FL_FLAT_BOX);
        o->labelcolor(FL_WHITE);
        Fl_Color button_color = fl_rgb_color(93, 0, 0);
        o->color(button_color);
        o->down_color(button_color);
        o->clear_visible_focus();
        o->callback(launch_cb); }

      { uri_box *o = new uri_box(119, 526, 43, 37, NULL);
#if (DEBUG == 1)
        o->box(FL_UP_BOX);
        o->color(FL_BLUE);
#else
        o->box(FL_NO_BOX);
#endif
        o->clear_visible_focus();
        o->callback(open_uri_cb, (void *)"https://twitter.com/superhotthegame"); }

      { uri_box *o = new uri_box(188, 523, 26, 43, NULL);
#if (DEBUG == 1)
        o->box(FL_UP_BOX);
        o->color(FL_BLUE);
#else
        o->box(FL_NO_BOX);
#endif
        o->clear_visible_focus();
        o->callback(open_uri_cb, (void *)"https://www.facebook.com/superhotgame"); }

      { uri_box *o = new uri_box(240, 527, 43, 34, NULL);
#if (DEBUG == 1)
        o->box(FL_UP_BOX);
        o->color(FL_BLUE);
#else
        o->box(FL_NO_BOX);
#endif
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
  Fl::run();

  if (launch_game)
  {
    val_res       = resolution_selection->value();
    val_lang      = language_selection->value();
    val_screen    = screen_selection->value();
    is_fullscreen = (checkbutton_set) ? 0 : 1;

    /* write config file */
    std::ofstream of;
    of.open(conffile.c_str());
    of << "resolution=" << itostr(val_res)       << "\n"
       << "screen="     << itostr(val_screen)    << "\n"
       << "fullscreen=" << itostr(is_fullscreen) << "\n"
       << "language="   << itostr(val_lang)      << "\n"
       << "quality="    << itostr(val_quality)   << "\n";
    of.close();

    std::string quality[] = { "HighEnd", "LowEnd" };

    std::string command = "'" + std::string(exedir) + "/SUPERHOT." EXEEXT + "'"
      " -adapter "           + itostr(val_screen) +
      " -screen-fullscreen " + itostr(is_fullscreen) +
      " -screen-width "      + itostr(resolutions[val_res].w) +
      " -screen-height "     + itostr(resolutions[val_res].h) +
      " -language "          + l10n[val_lang][0] +
      " -screen-quality "    + quality[val_quality];

    Fl::check();

    if (argc > 1 && (STREQ(argv[1], "-verbose") || STREQ(argv[1], "--verbose")))
    {
      std::cout << "execute command: " << command << std::endl;
    }

    return system(command.c_str());
  }

  return 0;
}

