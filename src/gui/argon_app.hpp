#ifndef ARGON_ARGON_APP_HPP
#define ARGON_ARGON_APP_HPP

#include <gtkmm/applicationwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/liststore.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>
#include <gtkmm/entry.h>
#include <gtkmm/cellrendererprogress.h>

#include <iostream>

#include "download_columns.hpp"
#include "download_link_input.hpp"

namespace Argon::gui {
class argon_app : public Gtk::ApplicationWindow {
public:
    argon_app();
    ~argon_app() override = default;

    // deleted constructors
    const argon_app& operator=(const argon_app& rhs) = delete;
    argon_app(const argon_app& rhs) = delete;

    const argon_app& operator=(const argon_app&& rhs) = delete;
    argon_app(const argon_app&& rhs) = delete;

protected:
    // Signal handlers:
//    void on_menu_item_about() const noexcept;

private:
    // top level container
    Gtk::VBox box;

    // top menu bar
    Gtk::MenuBar menu_bar;

    // menu items
    Gtk::MenuItem download;

    // sub menu items in download
    Gtk::Menu submenu_download;
    Gtk::ImageMenuItem submenu_new_download;
    Gtk::ImageMenuItem submenu_new_download_by_url;

    // sub menu items in about
    Gtk::MenuItem about;
    Gtk::Menu submenu_about;
    Gtk::ImageMenuItem submenu_about_about;

    // view of the tree
    Gtk::TreeView view;
    Gtk::ScrolledWindow scrolled_window;
    Glib::RefPtr<Gtk::ListStore> columns_ptr;
    download_columns columns;
    Gtk::CellRendererProgress cell_renderer_progress;

//    download_link_input input;
};
} // namespace Argon::gui
#endif // ARGON_ARGON_APP_HPP
