/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019, djcj <djcj@gmx.de>
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

#include <iostream>
#include <sstream>

#include <errno.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "images.h"
#include "common.cpp"

#define MAX_LANG 15

class mover_button : public Fl_Button
{
private:
  Fl_Image *mover_image_;

public:
  void mover_image(Fl_Image *img) { mover_image_ = img; }

  mover_button(int X, int Y, int W, int H, const char *L);

  int handle(int event) {
    int ret = Fl_Button::handle(event);
    switch (event) {
      case FL_ENTER:
        fl_cursor(FL_CURSOR_HAND);
        image(mover_image_);
        parent()->redraw();
        break;
      case FL_LEAVE:
        fl_cursor(FL_CURSOR_DEFAULT);
        image(NULL);
        parent()->redraw();
        break;
    }
    return ret;
  }
};

mover_button::mover_button(int X, int Y, int W, int H, const char *L=NULL)
: Fl_Button(X, Y, W, H, L),
  mover_image_(NULL)
{
  box(FL_NO_BOX);
  down_box(FL_NO_BOX);
  clear_visible_focus();
}

const char *languages[MAX_LANG][2] = {
  { "English", "EN" },
  { "Polski", "PL" },  /* Polish */
  { "Fran" "\xC3\xA7" "ais", "FR" },  /* French */
  { "Deutsch", "DE" },  /* German */
  { "\xD0\xA0\xD1\x83\xD1\x81\xD1\x81\xD0\xBA\xD0\xB8\xD0\xB9", "RU" },  /* Russian */
  { "Portugu" "\xC3\xAA" "se do Brasil", "PT" },  /* Brazilian Portuguese */
  { "Italiano", "IT" },  /* Italian */
  { "\xC4\x8C" "e" "\xC5\xA1" "tina", "CZ" },  /* Czech */
  { "Magyar", "HU" },  /* Hungarian */
  { "Espa" "\xC3\xB1" "ol", "SP" },  /* Spanish */
  { "Slovak", "SK" },  /* Slovakian */
  { "\xE6\x97\xA5\xE6\x9C\xAC\xE8\xAA\x9E", "JA" },  /* Japanese */
  { "\xE7\xB0\xA1\xE9\xAB\x94\xE4\xB8\xAD\xE6\x96\x87", "ZHS" },  /* simplified Chinese */
  { "\xE7\xB9\x81\xE9\xAB\x94\xE4\xB8\xAD\xE6\x96\x87", "ZHT" },  /* traditional Chinese */
  { "\xED\x95\x9C\xEA\xB5\xAD\xEC\x96\xB4\xEC\x9D\x98", "KR" }  /* Korean */
};

Fl_Menu_Item resolution_items[RES_COUNT + 1];
Fl_Menu_Item language_items[MAX_LANG + 1];
Fl_Menu_Item *screen_items = NULL;
menu_button *resolution_selection, *screen_selection, *language_selection;
int prev_selection_res, prev_selection_screen, prev_selection_lang;
bool launch_game = false;

int default_lang(void)
{
  char *l = getenv("LANG");

  if (!l) {
    l = getenv("LANGUAGE");
  }

  if (!l || strlen(l) < 2) {
    return 0;  /* English */
  }

  for (int i = 0; i < MAX_LANG; i++) {
    if (strncasecmp(l, languages[i][1], 2) == 0) {
      return i;
    }
  }
  return 0;  /* English */
}

void resolution_selection_cb(Fl_Widget *) {
  selection_callback(&prev_selection_res, resolution_selection, resolution_items);
}

void screen_selection_cb(Fl_Widget *) {
  selection_callback(&prev_selection_screen, screen_selection, screen_items);
}

void language_selection_cb(Fl_Widget *) {
  selection_callback(&prev_selection_lang, language_selection, language_items);
}

void close_cb(Fl_Widget *) {
  win->hide();
}

void start_cb(Fl_Widget *) {
  launch_game = true;
  win->hide();
}

int main(void)
{
  /* satisfying section 4 of the FLTK license's LGPL exception */
  std::cout << "using FLTK version " PRINT_VERSION " (http://www.fltk.org)" << std::endl;

  /* get exe full path */
  std::string exe, exedir;
  if (!get_paths(exe, exedir)) {
    return 1;
  }

  /* get configurations */
  int screens_avail = get_screen_count();
  int val_res, val_fullscreen, val_lang, val_screen;
  Fl_Preferences prefs(exedir.c_str(), "SUPERHOT team", exe.c_str());

  prefs.get("resolution", val_res, -1);
  prefs.get("fullscreen", val_fullscreen, -1);
  prefs.get("language", val_lang, -1);
  prefs.get("screen", val_screen, -1);
  if (val_res < 0 || val_res > RES_COUNT - 1) { val_res = RES_COUNT - 1; }
  if (val_fullscreen < 0 || val_fullscreen > 1) { val_fullscreen = 1; }
  if (val_lang < 0 || val_lang > MAX_LANG) { val_lang = default_lang(); }
  if (val_screen < 0 || val_screen > screens_avail - 1) { val_screen = 0; }

  windowed = (val_fullscreen == 1) ? false : true;
  prev_selection_res = val_res;
  prev_selection_screen = val_screen;
  prev_selection_lang = val_lang;

  /* create menu entries */
  std::string screens_text[screens_avail], res_text[RES_COUNT];
  screen_items = new Fl_Menu_Item[screens_avail + 1];

  for (int i = 0; i < screens_avail; i++) {
    screens_text[i] = "Screen " + itostr(i + 1);
    screen_items[i] = { screens_text[i].c_str(), 0,0,0,0, FL_NORMAL_LABEL, 0, LABELSIZE, 0 };
  }
  screen_items[screens_avail] = { 0,0,0,0,0,0,0,0,0 };

  for (int i = 0; i < RES_COUNT; i++) {
    res_text[i].append(resolutions[i][0]);
    res_text[i].push_back('x');
    res_text[i].append(resolutions[i][1]);
    resolution_items[i] = { res_text[i].c_str(), 0,0,0,0, FL_NORMAL_LABEL, 0, LABELSIZE, 0 };
  }
  resolution_items[RES_COUNT] = { 0,0,0,0,0,0,0,0,0 };

  for (int i = 0; i < MAX_LANG; i++) {
    language_items[i] = { languages[i][0], 0,0,0,0, FL_NORMAL_LABEL, 0, LABELSIZE, 0 };
  }
  language_items[MAX_LANG] = { 0,0,0,0,0,0,0,0,0 };

  /* create window */
  Fl_Double_Window::default_icon(&image_icon_128);
  Fl::background(0, 0, 0);

  win = new Fl_Double_Window(400, 600, "SUPERHOT");
  {
    { move_box *o = new move_box(0, 0, win->w(), win->h());
      o->align(FL_ALIGN_INSIDE);
      o->image(&image_window); }

    /* resolution */
    { menu_button *o = resolution_selection = new menu_button(80, 186, 245, 26);
      o->menu(resolution_items);
      o->value(val_res);
      resolution_items[val_res].labelfont_ += FL_BOLD;
      o->auto_label();
      o->callback(resolution_selection_cb); }

    /* windowed */
    { Fl_Button *o = new Fl_Button(80, 219, 92, 18);
      o->box(FL_NO_BOX);
      o->down_box(FL_NO_BOX);
      if (windowed) {
        o->image(&image_check);
      } else {
        o->image(NULL);
      }
      o->clear_visible_focus();
      o->callback(checkbutton_cb); }

    /* screen */
    { menu_button *o = screen_selection = new menu_button(80, 278, 245, 26);
      o->menu(screen_items);
      o->value(val_screen);
      screen_items[val_screen].labelfont_ += FL_BOLD;
      o->auto_label();
      o->callback(screen_selection_cb); }

    /* language */
    { menu_button *o = language_selection = new menu_button(80, 344, 245, 26);
      o->menu(language_items);
      o->value(val_lang);
      language_items[val_lang].labelfont_ += FL_BOLD;
      o->auto_label();
      o->callback(language_selection_cb); }

    const char *facebook_url = "https://www.facebook.com/dialog/feed?app_id=891295770968742&link=http%3A%2F%2Fsuperhotgame.com&redirect_uri=https://superhotgame.com&caption=About%20to%20play%20%23SUPERHOT!%20So%20excited!&display=popup";
    const char *twitter_url  = "https://twitter.com/intent/tweet?text=About%20to%20play%20%23SUPERHOT%21%20So%20excited%21&via=superhotthegame&url=http%3A%2F%2Fsuperhotgame.com&original_referer=";

    { mover_button *o = new mover_button(372, 7, 21, 21);
      o->mover_image(&image_close);
      o->callback(close_cb); }

    { mover_button *o = new mover_button(76, 410, 254, 51);
      o->mover_image(&image_start);
      o->callback(start_cb); }

    { mover_button *o = new mover_button(130, 490, 30, 31);
      o->mover_image(&image_facebook);
      o->callback(open_uri_cb, reinterpret_cast<void *>(const_cast<char *>(facebook_url))); }

    { mover_button *o = new mover_button(240, 490, 30, 31);
      o->mover_image(&image_twitter);
      o->callback(open_uri_cb, reinterpret_cast<void *>(const_cast<char *>(twitter_url))); }
  }
  win->position((Fl::w() - win->w()) / 2, (Fl::h() - win->h()) / 2);
  win->end();
  win->show();
  win->border(0);

  Fl::run();

  val_res = resolution_selection->value();
  val_lang = language_selection->value();
  val_screen = screen_selection->value();

  /* write config file */
  val_fullscreen = windowed ? 0 : 1;
  prefs.set("resolution", val_res);
  prefs.set("fullscreen", val_fullscreen);
  prefs.set("language", val_lang);
  prefs.set("screen", val_screen);

  delete[] screen_items;

  if (launch_game) {
    prefs.flush();
    std::string s = exedir + "/SUPERHOT.x86_64";

    std::cout << "\n\"" << s << "\""
      << " -adapter " << val_screen
      << " -screen-fullscreen " << val_fullscreen
      << " -screen-width " << resolutions[val_res][0]
      << " -screen-height " << resolutions[val_res][1]
      << " -language " << languages[val_lang][1]
      << "\n" << std::endl;

    execl(s.c_str(), s.c_str(),
          "-adapter", itostr(val_screen).c_str(),
          "-screen-fullscreen", itostr(val_fullscreen).c_str(),
          "-screen-width", resolutions[val_res][0],
          "-screen-height", resolutions[val_res][1],
          "-language", languages[val_lang][1],
          NULL);
    _exit(127);
  }
  return 0;
}

