#include "download_link_input.hpp"

#include "../network/download.hpp"
#include "../network/network.hpp" // todo: fix the disgusting relative path to something provided by cmake
#include "argon_app.hpp"

#include <gtkmm/messagedialog.h>
#include <iostream>

using namespace Argon::gui;
using namespace Argon;
using namespace std;

namespace
{

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string& s)
{
    return rtrim(ltrim(s));
}
} // namespace

download_link_input::download_link_input()
    : confirm_{ "confirm" }
    , cancel_{ "cancel" }
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
        text             = trim(text);

        if (!network::is_valid_uri(text))
        {
            cerr << "try again\n";
        }
        else if (network::can_be_accelerated(text))
        {
            argon_app::get_instance()->scheduler.add_download(text);
            std::cout << "begin sleep\n";
            std::this_thread::sleep_for(std::chrono::minutes{ 2 });
            std::cout << "after sleep\n";
            argon_app::get_instance()->scheduler.deque();
        }
        else
        {
        }
    });

    cancel_.signal_clicked().connect([]() { cout << "on cancel\n"; });

    add(box);
    show_all_children();
}
