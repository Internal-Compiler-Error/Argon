#ifndef ARGON_ARGON_APP_HPP
#define ARGON_ARGON_APP_HPP
#include "../../network/download_scheduler/download_scheduler.hpp"

#include <gtkmm/applicationwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/cellrendererprogress.h>
#include <gtkmm/entry.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/liststore.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>

#include <iostream>
#include <vector>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace net = boost::asio;

#include "download_columns/download_columns.hpp"
#include "download_file_input/download_link_input.hpp"

namespace Argon::gui {
class argon_app : public Gtk::ApplicationWindow
{
  static std::shared_ptr<argon_app> instance;

public:
  static std::shared_ptr<argon_app> get_instance();

public:
  argon_app();
  ~argon_app() override = default;

  // deleted constructors
  const argon_app& operator=(const argon_app& rhs) = delete;
  argon_app(const argon_app& rhs)                  = delete;

  const argon_app& operator=(const argon_app&& rhs) = delete;
  argon_app(const argon_app&& rhs)                  = delete;

private:
  Gtk::VBox box; // top level container

  Gtk::MenuBar menu_bar; // top menu bar

  Gtk::MenuItem download; // menu items

  // sub menu items in download
  Gtk::Menu          submenu_download;
  Gtk::ImageMenuItem submenu_new_download;
  Gtk::ImageMenuItem submenu_new_download_by_url;

  // sub menu items in about
  Gtk::MenuItem      about;
  Gtk::Menu          submenu_about;
  Gtk::ImageMenuItem submenu_about_about;

  // view of the tree
  Gtk::TreeView                view;
  Gtk::ScrolledWindow          scrolled_window;
  Glib::RefPtr<Gtk::ListStore> columns_ptr;
  download_columns             columns;
  Gtk::CellRendererProgress    cell_renderer_progress;

public:
  network::download_scheduler& scheduler;
};
} // namespace Argon::gui
#endif // ARGON_ARGON_APP_HPP
