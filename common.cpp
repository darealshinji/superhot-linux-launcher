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
private:
  int _event_x, _event_y;

public:
  move_box(int X, int Y, int W, int H, const char *L=0)
   : Fl_Box(X, Y, W, H, L),
     _event_x(0),
     _event_y(0)
  {
    align(FL_ALIGN_INSIDE);
  }

public:
  int handle(int event);
};

class menu_button : public Fl_Menu_Button
{
private:
  int _prev;
  Fl_Menu_Item *_menu;

  void prev(int v) { _prev = v; }
  int prev() { return _prev; }
  void update_labels();

public:
  menu_button(int X, int Y, int W, int H);

  void menu(const Fl_Menu_Item *m);
  Fl_Menu_Item *menu() { return _menu; }

  int handle(int e) {
    int ret = Fl_Menu_Button::handle(e);
    update_labels();
    return ret;
  }
};

int move_box::handle(int event)
{
  int ret = Fl_Box::handle(event);
  switch (event) {
    case FL_PUSH:
      fl_cursor(FL_CURSOR_MOVE);
      _event_x = Fl::event_x();
      _event_y = Fl::event_y();
      return 1;
    case FL_DRAG:
      window()->position(Fl::event_x_root() - _event_x, Fl::event_y_root() - _event_y);
      break;
    case FL_RELEASE:
      fl_cursor(FL_CURSOR_DEFAULT);
      break;
  }
  return ret;
}

menu_button::menu_button(int X, int Y, int W, int H)
: Fl_Menu_Button(X, Y, W, H),
  _prev(0)
{
  align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);
  box(FL_THIN_DOWN_BOX);
  down_box(FL_THIN_DOWN_BOX);
  color(FL_WHITE);
  selection_color(fl_rgb_color(234, 67, 51));
  labelsize(LABELSIZE);
  clear_visible_focus();
}

void menu_button::menu(const Fl_Menu_Item *m)
{
  Fl_Menu_Button::copy(m);
  _menu = const_cast<Fl_Menu_Item *>(Fl_Menu_Button::menu());
}

void menu_button::update_labels(void)
{
  Fl_Menu_Item *m = _menu;
  if (!m) {
    return;
  }

  int v = value();

  const char *l = m[v].text;
  if (l) {
    char *ch = new char[strlen(l) + 1];
    ch[0] = ' ';
    ch[1] = 0;
    strcat(ch, l);
    copy_label(ch);
    delete ch;
  }

  m[prev()].labelfont_ = labelfont();
  m[v].labelfont_ = labelfont() | FL_BOLD;
  prev(v);
}

Fl_Double_Window *win;
bool windowed = false;

const char *resolutions[][2] = {
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

std::string itostr(int i) {
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
  int n = Fl::screen_count();

  if (n < 1) {
    return 1;
  }
  return n;
}

