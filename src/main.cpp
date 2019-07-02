#include "argon_gui.hpp"
#include <gtkmm/main.h>
using namespace Argon::gui;

int main()
{
    Gtk::Main app;

    auto window = argon_app::get_instance();
    Gtk::Main::run(*window);

    return 0;
}