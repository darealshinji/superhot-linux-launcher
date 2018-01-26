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

#define PNG(x) \
  Fl_PNG_Image image_##x(NULL, x##_png, (int)x##_png_len);

#define GETPREFS(entry, value, default, max) \
  prefs.get(entry, value, -1); \
  if (value < 0 || value > max) { value = default; }

#define MENU_BUTTON(PTR,X,Y,ITEMS,VAL,CB) \
  { Fl_Menu_Button *o = PTR = new Fl_Menu_Button(X, Y, 245, 26); \
    o->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT); \
    o->box(FL_THIN_DOWN_BOX); \
    o->down_box(FL_THIN_DOWN_BOX); \
    o->color(FL_WHITE); \
    o->selection_color(selection_color); \
    o->menu(ITEMS); \
    o->value(VAL); \
    ITEMS[VAL].labelfont_ = FL_BOLD; \
    std::string s = " " + std::string(o->text()); \
    o->copy_label(s.c_str()); \
    o->labelsize(LABELSIZE); \
    o->clear_visible_focus(); \
    o->callback(CB); }

#define BUTTON(X,Y,W,H,CB_A,CB_B,IMG) \
    { mouse_over_box *o = new mouse_over_box(X, Y, W, H); \
      o->box(FL_NO_BOX); \
      o->down_box(FL_NO_BOX); \
      o->mouse_over_image = &IMG; \
      o->callback(CB_A, (void *)CB_B); \
      o->clear_visible_focus(); }

class move_box : public Fl_Box
{
public:
  move_box(int X, int Y, int W, int H, const char *L=0)
   : Fl_Box(X, Y, W, H, L),
     event_x(0),
     event_y(0)
  { }

  virtual ~move_box() { }

protected:
  int event_x, event_y;

public:
  int handle(int event) {
    int ret = Fl_Box::handle(event);
    switch (event) {
      case FL_PUSH:
        fl_cursor(FL_CURSOR_MOVE);
        event_x = Fl::event_x();
        event_y = Fl::event_y();
        return 1;
      case FL_DRAG:
        window()->position(Fl::event_x_root() - event_x, Fl::event_y_root() - event_y);
        break;
      case FL_RELEASE:
        fl_cursor(FL_CURSOR_DEFAULT);
        break;
    }
    return ret;
  }
};

class mouse_over_box : public Fl_Button
{
public:
  mouse_over_box(int X, int Y, int W, int H, const char *L=0)
   : Fl_Button(X, Y, W, H, L),
     mouse_over_image(0)
  { }

  virtual ~mouse_over_box() { }

  Fl_Image *mouse_over_image;

  int handle(int event) {
    int ret = Fl_Button::handle(event);
    switch (event) {
      case FL_ENTER:
        fl_cursor(FL_CURSOR_HAND);
        image(mouse_over_image);
        parent()->redraw();
        break;
      case FL_LEAVE:
        fl_cursor(FL_CURSOR_DEFAULT);
        image(0);
        parent()->redraw();
        break;
    }
    return ret;
  }
};

std::string resolutions[][2] = {
  {  "640",  "480" },
  {  "720",  "480" },
  {  "720",  "576" },
  {  "800",  "600" },
  { "1024",  "768" },
  { "1152",  "864" },
  { "1176",  "664" },
  { "1280",  "720" },
  { "1280",  "768" },
  { "1280",  "800" },
  { "1280",  "960" },
  { "1280", "1024" },
  { "1360",  "768" },
  { "1366",  "768" },
  { "1400", "1050" },
  { "1440",  "900" },
  { "1600",  "900" },
  { "1600", "1024" },
  { "1680", "1050" },
  { "1768",  "992" },
  { "1920", "1080" }
};

std::string itostr(int i) {
  std::stringstream ss;
  ss << i;
  return ss.str();
}

void open_uri_cb(Fl_Widget *, void *v) {
  char errmsg[512];
  if (!fl_open_uri((const char *)v, errmsg, sizeof(errmsg))) {
    fl_message_title("Failed to open URL");
    fl_message("%s", errmsg);
  }
}

void selection_callback(int *prev, Fl_Menu_Button *b, Fl_Menu_Item *it)
{
  if (*prev != b->value()) {
    std::string s = " " + std::string(b->text());
    b->copy_label(s.c_str());
    it[b->value()].labelfont_ = FL_BOLD;
    it[*prev].labelfont_ = 0;
    *prev = b->value();
  }
}

void checkbutton_cb(Fl_Widget *, void *v)
{
  Fl_Image *img = (Fl_Image *)v;

  if (windowed) {
    windowed = false;
    windowed_button->image(0);
  } else {
    windowed = true;
    windowed_button->image(img);
  }
  win->redraw();
}

int get_paths(std::string &exe, std::string &exedir)
{
  char *rp = realpath("/proc/self/exe", NULL);
  int errsv = errno;

  if (rp) {
    char *copy, *bn, *dn;
    copy = strdup(rp);
    if (!(bn = basename(rp)) || !(dn = dirname(copy))) {
      std::cerr << "Error: basename() and/or dirname() has returned NULL" << std::endl;
      return 1;
    }
    exe = std::string(bn);
    exedir = std::string(dn);
    free(rp);
    free(copy);
  } else {
    std::cerr << "Error: realpath() -> /proc/self/exe: " << strerror(errsv) << std::endl;
    return 1;
  }
  return 0;
}

int get_screen_count(void)
{
  Display *dp = XOpenDisplay(":0.0");
  int n = ScreenCount(dp);
  XCloseDisplay(dp);

  if (n < 1) {
    n = 1;
  } else if (n > MAX_SCREENS) {
    n = MAX_SCREENS;
  }
  return n;
}

