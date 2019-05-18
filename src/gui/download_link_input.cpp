#include "download_link_input.hpp"
#include <iostream>
using namespace Argon::gui;
using namespace std;
download_link_input::download_link_input() :confirm_{ "confirm" }, cancel_{ "cancel" }
{
    auto screen = Gdk::Screen::get_default();
    set_size_request(screen->get_width() / 4, screen->get_height() / 4);
    set_title("new download");
    set_border_width(20);

    box.set_valign(Gtk::ALIGN_CENTER);

    box.pack_start(entry_, Gtk::PACK_SHRINK);

    box.pack_end(button_box_, Gtk::PACK_SHRINK);

    button_box_.pack_start(confirm_, Gtk::PACK_EXPAND_WIDGET);
    button_box_.pack_end(cancel_, Gtk::PACK_EXPAND_WIDGET);

    confirm_.signal_clicked().connect([&]() {
      cout << entry_.get_text() << '\n';
      cout << "on confirm\n";
    });

    cancel_.signal_clicked().connect([]() {
      cout << "on cancel\n";
    });

    add(box);
    show_all_children();
}
