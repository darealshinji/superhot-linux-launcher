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

#define LABELSIZE 13

class move_box : public Fl_Box
{
public:
  move_box(int X, int Y, int W, int H, const char *L=0)
   : Fl_Box(X, Y, W, H, L),
     event_x(0),
     event_y(0)
  { }

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

class menu_button : public Fl_Menu_Button
{
public:
  menu_button(int X, int Y, int W, int H, const char *L);
  void auto_label();
};

menu_button::menu_button(int X, int Y, int W, int H, const char *L=NULL)
: Fl_Menu_Button(X, Y, W, H, L)
{
  align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);
  box(FL_THIN_DOWN_BOX);
  down_box(FL_THIN_DOWN_BOX);
  color(FL_WHITE);
  selection_color(fl_rgb_color(234, 67, 51));
  labelsize(LABELSIZE);
  clear_visible_focus();
}

void menu_button::auto_label(void)
{
  const char *l = this->text();
  if (l) {
    char ch[strlen(l) + 1];
    ch[0] = ' ';
    ch[1] = 0;
    strcat(ch, l);
    copy_label(ch);
  }
}

Fl_Double_Window *win;
bool windowed = false;

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
#define RES_COUNT 21

inline std::string itostr(int i) {
  std::stringstream ss;
  ss << i;
  return ss.str();
}

void open_uri_cb(Fl_Widget *, void *v) {
  char errmsg[512];
  if (!fl_open_uri(reinterpret_cast<char *>(v), errmsg, sizeof(errmsg))) {
    fl_message_title("Failed to open URL");
    fl_message("%s", errmsg);
  }
}

void selection_callback(int *prev, menu_button *b, Fl_Menu_Item *it)
{
  if (*prev != b->value()) {
    b->auto_label();
    it[b->value()].labelfont_ += FL_BOLD;
    it[*prev].labelfont_ -= FL_BOLD;
    *prev = b->value();
  }
}

void checkbutton_cb(Fl_Widget *o)
{
  Fl_Button *b = dynamic_cast<Fl_Button *>(o);

  if (windowed) {
    windowed = false;
    b->image(NULL);
  } else {
    windowed = true;
    b->image(&image_check);
  }
  b->parent()->redraw();
}

bool get_paths(std::string &exe, std::string &exedir)
{
  char *rp = realpath("/proc/self/exe", NULL);
  int errsv = errno;

  if (rp) {
    char *copy, *bn, *dn;
    copy = strdup(rp);
    if (!(bn = basename(rp)) || !(dn = dirname(copy))) {
      std::cerr << "Error: basename() and/or dirname() has returned NULL" << std::endl;
      return false;
    }
    exe = std::string(bn);
    exedir = std::string(dn);
    free(rp);
    free(copy);
  } else {
    std::cerr << "Error: realpath() -> /proc/self/exe: " << strerror(errsv) << std::endl;
    return false;
  }
  return true;
}

int get_screen_count(void)
{
  Display *dp = XOpenDisplay(NULL);
  XineramaScreenInfo *xsi = NULL;
  int event_base, error_base, n;

  if (XineramaQueryExtension(dp, &event_base, &error_base)) {
    xsi = XineramaQueryScreens(dp, &n);
  } else {
    //n = Fl::screen_count();
    n = XScreenCount(dp);
  }

  if (n < 1) {
    n = 1;
  }

  if (xsi) {
    XFree(xsi);
  }
  XCloseDisplay(dp);

  return n;
}

