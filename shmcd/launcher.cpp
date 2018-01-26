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

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_PNG_Image.H>
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

#define LABELSIZE 13
#define MAX_SCREENS 16
#define RES_COUNT 21
#define WIN_W 1020
#define WIN_H 560

Fl_Double_Window *win;
Fl_Menu_Item resolution_items[RES_COUNT + 1], screen_items[MAX_SCREENS];
Fl_Menu_Button *resolution_selection, *screen_selection;
Fl_Button *windowed_button;
int prev_selection_res, prev_selection_screen;
bool launch_game = false, windowed = false;

#include "common.cpp"
#include "images.h"

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

  win = new Fl_Double_Window(WIN_W, WIN_H, "SUPERHOT: MIND CONTROL DELETE");
  {
    { move_box *o = new move_box(0,0, WIN_W, WIN_H);
      o->align(FL_ALIGN_INSIDE);
      o->image(&image_window); }

    { Fl_Box *o = new Fl_Box(0, 510, 260, 50);
      o->image(&image_version); }

    { Fl_Box *o = new Fl_Box(728, 362, 292, 198);
      o->image(&image_links_area); }

    /* resolution */
    MENU_BUTTON(resolution_selection, 728, 138, resolution_items, val_res, resolution_selection_cb)

    /* windowed */
    { Fl_Button *o = windowed_button = new Fl_Button(728, 170, 94, 23);
      o->box(FL_NO_BOX);
      o->down_box(FL_NO_BOX);
      o->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);
      if (windowed) {
        o->image(&image_check);
      } else {
        o->image(0);
      }
      o->clear_visible_focus();
      o->callback(checkbutton_cb, &image_check); }

    /* screen */
    MENU_BUTTON(screen_selection, 728, 245, screen_items, val_screen, screen_selection_cb)

    const char *facebook_url = "https://www.facebook.com/dialog/feed?app_id=891295770968742&link=http%3A%2F%2Fsuperhotgame.com/mcd/&redirect_uri=https://superhotgame.com/mcd/&caption=About%20to%20play%20%23SUPERHOT!%20So%20excited!&display=popup";
    const char *twitter_url  = "https://twitter.com/share?url=http://superhot___mind_control_delete.trbt.it/launcher/twitter&text=About%20to%20play%20%23MINDCONTROLDELETE!%20So%20excited!&via=superhotthegame";
    const char *reddit_url   = "https://www.reddit.com/r/superhot/";
    const char *feedback_url = "https://docs.google.com/forms/d/e/1FAIpQLSdOdCozSWAi9MXoovLgjIW-IP9DsALlQjrKBdSFXIAI7saYPQ/viewform";
    const char *discord_url  = "https://discordapp.com/invite/NJKgwvx";
    const char *twitch_url   = "https://www.twitch.tv/superhotthegame";
    const char *caffeine_url = "https://www.caffeine.tv/superhotthegame/profile";

    BUTTON(958, 12, 23, 23, close_cb, NULL, image_close)
    BUTTON(728, 287, 249, 51, start_cb, &launch_game, image_start)
    BUTTON(760, 439, 30, 31, open_uri_cb, facebook_url, image_facebook)
    BUTTON(810, 439, 30, 31, open_uri_cb, twitter_url, image_twitter)
    BUTTON(860, 439, 30, 31, open_uri_cb, reddit_url, image_reddit)
    BUTTON(910, 439, 30, 31, open_uri_cb, feedback_url, image_feedback)
    BUTTON(785, 486, 30, 31, open_uri_cb, discord_url, image_discord)
    BUTTON(835, 486, 30, 31, open_uri_cb, twitch_url, image_twitch)
    BUTTON(885, 486, 30, 31, open_uri_cb, caffeine_url, image_caffeine)
  }
  win->position((Fl::w() - WIN_W) / 2, (Fl::h() - WIN_H) / 2);
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

