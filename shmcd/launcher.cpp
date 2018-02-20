/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018, djcj <djcj@gmx.de>
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

#define SHMCD_VERSION "ALPHA 2.0.1m"

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/fl_draw.H>
#include <FL/filename.H>
#include <FL/x.H>

#include <iostream>
#include <sstream>

#include <errno.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.cpp"
#include "background.h"
#include "images.h"

class mouse_over_button : public Fl_Button
{
public:
  mouse_over_button(int X, int Y, int W, int H, const char *L=0)
   : Fl_Button(X, Y, W, H, L),
     default_color(0),
     default_labelcolor(0),
     mouse_over_color(0),
     mouse_over_labelcolor(0),
     default_image(0),
     mouse_over_image(0)
  { }

  virtual ~mouse_over_button() { }

  Fl_Color default_color;
  Fl_Color default_labelcolor;
  Fl_Color mouse_over_color;
  Fl_Color mouse_over_labelcolor;
  Fl_Image *default_image;
  Fl_Image *mouse_over_image;

  int handle(int event) {
    int ret = Fl_Button::handle(event);
    switch (event) {
      case FL_ENTER:
        fl_cursor(FL_CURSOR_HAND);
        color(mouse_over_color);
        labelcolor(mouse_over_labelcolor);
        image(mouse_over_image);
        parent()->redraw();
        break;
      case FL_LEAVE:
        fl_cursor(FL_CURSOR_DEFAULT);
        color(default_color);
        labelcolor(default_labelcolor);
        image(default_image);
        parent()->redraw();
        break;
    }
    return ret;
  }
};

Fl_Menu_Item resolution_items[RES_COUNT + 1], screen_items[MAX_SCREENS];
Fl_Menu_Button *resolution_selection, *screen_selection;
int prev_selection_res, prev_selection_screen;
bool launch_game = false;

void resolution_selection_cb(Fl_Widget *, void *) {
  selection_callback(&prev_selection_res, resolution_selection, resolution_items);
}

void screen_selection_cb(Fl_Widget *, void *) {
  selection_callback(&prev_selection_screen, screen_selection, screen_items);
}

void close_cb(Fl_Widget *, void *) {
  win->hide();
}

void start_cb(Fl_Widget *, void *) {
  launch_game = true;
  win->hide();
}

int main(void)
{
  /* satisfying section 4 of the FLTK license's LGPL exception */
  std::cout << "using FLTK version " PRINT_VERSION " (http://www.fltk.org)" << std::endl;

  /* get exe full path */
  std::string exe, exedir;
  if (get_paths(exe, exedir) == 1) {
    return 1;
  }

  /* get number of screens */
  int screens_avail = get_screen_count();

  /* get configurations */
  int val_res, val_screen, val_fullscreen;
  Fl_Preferences prefs(exedir.c_str(), "SUPERHOT team", exe.c_str());
  GETPREFS("resolution", val_res, RES_COUNT - 1, RES_COUNT - 1);
  GETPREFS("fullscreen", val_fullscreen, 1, 1);
  GETPREFS("screen", val_screen, 0, screens_avail - 1);
  windowed = (val_fullscreen == 1) ? false : true;
  prev_selection_res = val_res;
  prev_selection_screen = val_screen;

  /* create menu entries */
  std::string screens_text[MAX_SCREENS], res_text[RES_COUNT];
  for (int i = 0; i < MAX_SCREENS; i++) {
    if (i < screens_avail) {
      screens_text[i] = "Screen " + itostr(i + 1);
      screen_items[i] = { screens_text[i].c_str(), 0,0,0,0, FL_NORMAL_LABEL, 0, LABELSIZE, 0 };
    } else {
      screen_items[i] = { 0,0,0,0,0,0,0,0,0 };
    }
  }
  for (int i = 0; i < RES_COUNT; i++) {
    res_text[i] = resolutions[i][0] + "x" + resolutions[i][1];
    resolution_items[i] = { res_text[i].c_str(), 0,0,0,0, FL_NORMAL_LABEL, 0, LABELSIZE, 0 };
  }
  resolution_items[RES_COUNT] = { 0,0,0,0,0,0,0,0,0 };

  /* create window */
  Fl_Color selection_color = fl_rgb_color(234, 67, 51);
  Fl_Double_Window::default_icon(&image_icon_128);
  Fl::background(0,0,0);

  win = new Fl_Double_Window(1020, 560, "SUPERHOT: MIND CONTROL DELETE");
  {
    { move_box *o = new move_box(0,0, 1020, 560);
      o->align(FL_ALIGN_INSIDE);
      o->image(new Fl_JPEG_Image(NULL, shmcd_background_jpg)); }

    /* close button */
    { mouse_over_button *o = new mouse_over_button(957, 10, 24, 24, "×");
      o->box(FL_NO_BOX);
      o->down_box(FL_NO_BOX);
      o->labelfont(FL_HELVETICA_BOLD);
      o->labelcolor(fl_rgb_color(180));
      o->default_labelcolor = o->labelcolor();
      o->mouse_over_labelcolor = FL_WHITE;
      o->labelsize(30);
      o->callback(close_cb);
      o->clear_visible_focus(); }

    /* resolution */
    { Fl_Box *o = new Fl_Box(728, 128, 1, 1, "RESOLUTION");
      o->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);
      o->labelcolor(FL_WHITE);
      o->labelsize(LABELSIZE); }
    MENU_BUTTON(resolution_selection, 728, 138, 249, 26, resolution_items, val_res, resolution_selection_cb)

    /* windowed */
    { Fl_Box *o = new Fl_Box(728, 171, 18, 18, " WINDOWED");
      o->box(FL_FLAT_BOX);
      o->color(FL_WHITE);
      o->align(FL_ALIGN_RIGHT);
      o->labelcolor(FL_WHITE);
      o->labelsize(LABELSIZE); }
    { Fl_Button *o = new Fl_Button(728, 171, 104, 18);
      o->box(FL_NO_BOX);
      o->down_box(FL_NO_BOX);
      if (windowed) {
        o->image(&image_check);
      } else {
        o->image(0);
      }
      o->clear_visible_focus();
      o->callback(checkbutton_cb, &image_check); }

    /* screen */
    { Fl_Box *o = new Fl_Box(728, 235, 1, 1, "SELECT MONITOR");
      o->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);
      o->labelcolor(FL_WHITE);
      o->labelsize(LABELSIZE); }
    MENU_BUTTON(screen_selection, 728, 245, 249, 26, screen_items, val_screen, screen_selection_cb)

    /* start game */
    { mouse_over_button *o = new mouse_over_button(728, 287, 249, 51, "START GAME");
      o->box(FL_FLAT_BOX);
      o->down_box(FL_FLAT_BOX);
      o->labelcolor(FL_WHITE);
      o->default_labelcolor = FL_WHITE;
      o->mouse_over_labelcolor = FL_WHITE;
      o->color(fl_rgb_color(25, 23, 24));
      o->down_color(fl_rgb_color(93, 0, 0));
      o->default_color = o->color();
      o->mouse_over_color = o->down_color();
      o->callback(start_cb);
      o->clear_visible_focus(); }

    { Fl_Box *o = new Fl_Box(728, 368, 249, 51, "JOIN THE DISCUSSION\n\nAND HELP SHAPE THE GAME");
      o->align(FL_ALIGN_CENTER);
      o->labelcolor(FL_WHITE);
      o->labelsize(LABELSIZE-2); }

    { Fl_Box *o = new Fl_Box(6, 514, 1, 1, "MIND CONTROL DELETE IS IN EARLY ACCESS\n\nSHMCD " SHMCD_VERSION);
      o->align(FL_ALIGN_RIGHT_TOP);
      o->labelcolor(fl_rgb_color(113));
      o->labelsize(LABELSIZE-3); }

    const char *facebook_url = "https://www.facebook.com/dialog/feed?app_id=891295770968742&link=http%3A%2F%2Fsuperhotgame.com/mcd/&redirect_uri=https://superhotgame.com/mcd/&caption=About%20to%20play%20%23SUPERHOT!%20So%20excited!&display=popup";
    const char *twitter_url  = "https://twitter.com/share?url=http://superhot___mind_control_delete.trbt.it/launcher/twitter&text=About%20to%20play%20%23MINDCONTROLDELETE!%20So%20excited!&via=superhotthegame";
    const char *reddit_url   = "https://www.reddit.com/r/superhot/";
    const char *feedback_url = "https://docs.google.com/forms/d/e/1FAIpQLSdOdCozSWAi9MXoovLgjIW-IP9DsALlQjrKBdSFXIAI7saYPQ/viewform";
    const char *discord_url  = "https://discordapp.com/invite/NJKgwvx";
    const char *twitch_url   = "https://www.twitch.tv/superhotthegame";
    const char *caffeine_url = "https://www.caffeine.tv/superhotthegame/profile";

    URL_BUTTON(760, 439, facebook)
    URL_BUTTON(810, 439, twitter)
    URL_BUTTON(860, 439, reddit)
    URL_BUTTON(910, 439, feedback)
    URL_BUTTON(785, 486, discord)
    URL_BUTTON(835, 486, twitch)
    URL_BUTTON(885, 486, caffeine)
  }
  win->position((Fl::w() - 1020) / 2, (Fl::h() - 560) / 2);
  win->end();
  win->show();
  win->border(0);

  Fl::run();

  val_res = resolution_selection->value();
  val_screen = screen_selection->value();

  /* write config file */
  val_fullscreen = windowed ? 0 : 1;
  prefs.set("resolution", val_res);
  prefs.set("fullscreen", val_fullscreen);
  prefs.set("screen", val_screen);

  if (launch_game) {
    prefs.flush();
    std::string s = exedir + "/SHMCD.x86_64";
    execl(s.c_str(), s.c_str(),
          "-adapter", itostr(val_screen).c_str(),
          "-screen-fullscreen", itostr(val_fullscreen).c_str(),
          "-screen-width", resolutions[val_res][0].c_str(),
          "-screen-height", resolutions[val_res][1].c_str(),
          (char *)0);
    _exit(127);
  }
  return 0;
}

