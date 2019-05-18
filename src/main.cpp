#include "argon_gui.hpp"
#include <gtkmm/main.h>
using namespace Argon::gui;

int main()
{
    Gtk::Main app;

    argon_app window;
    Gtk::Main::run(window);

    return 0;
}