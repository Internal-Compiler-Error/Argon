#include "download_link_input.hpp"
#include "../../network/network/network.hpp" // todo: fix the disgusting relative path to something provided by cmake
#include "argon_app/argon_app.hpp"
#include "download_not_supported_dialog/download_not_supported_dialog.hpp"

#include <iostream>
#include <regex>
#include <type_traits>

#include <gtkmm/messagedialog.h>

using namespace Argon::gui;
using namespace Argon;
using namespace std;

download_link_input::download_link_input() : confirm_{"confirm"}, cancel_{"cancel"}
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
    using namespace boost;

    std::string text = entry_.get_text();

    if (network::can_be_accelerated(text))
    {
    }
    else
    {
      //      cout << response << '\n';
      //      cout << "on confirm\n";
      argon_app::get_instance()->scheduler.add_download(network::download{text});
    }
  });

  cancel_.signal_clicked().connect([]() { cout << "on cancel\n"; });

  add(box);
  show_all_children();
}
