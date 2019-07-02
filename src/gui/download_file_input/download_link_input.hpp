#ifndef ARGON_DOWNLOAD_LINK_INPUT_HPP
#define ARGON_DOWNLOAD_LINK_INPUT_HPP

#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/hvbox.h>
#include <gtkmm/label.h>
#include <gtkmm/window.h>

namespace Argon::gui {
class download_link_input : public Gtk::Window
{
public:
  download_link_input();
  ~download_link_input() override = default;

  download_link_input& operator=(const download_link_input&) = delete;
  download_link_input& operator=(download_link_input&&) = delete;

  download_link_input(const download_link_input&) = delete;
  download_link_input(download_link_input&&)      = delete;

protected:
private:
  Gtk::VBox   box;
  Gtk::HBox   button_box_;
  Gtk::Entry  entry_;
  Gtk::Button confirm_;
  Gtk::Button cancel_;
};
} // namespace Argon::gui
#endif // ARGON_DOWNLOAD_LINK_INPUT_HPP
