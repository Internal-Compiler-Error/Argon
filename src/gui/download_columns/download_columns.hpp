#ifndef ARGON_DOWNLOAD_COLUMNS_HPP
#define ARGON_DOWNLOAD_COLUMNS_HPP

#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/hvbox.h>

namespace Argon::gui {
class download_columns : public Gtk::TreeModelColumnRecord {
public:
    download_columns();
    ~download_columns() override = default;

    download_columns& operator=(const download_columns&) = delete;
    download_columns& operator=(download_columns&&) = delete;

    download_columns(const download_columns&) = delete;
    download_columns(download_columns&&) = delete;

public:
    Gtk::TreeModelColumn<int> index_;

    Gtk::VBox box;
    Gtk::TreeModelColumn<Glib::ustring> link_;

    // todo: create an ip_ abstraction to replace ustring
    Gtk::TreeModelColumn<Glib::ustring> ip_;

    // todo: make conn_type work with TreeModelColumn
    Gtk::TreeModelColumn<Glib::ustring> protocol_;
    Gtk::TreeModelColumn<Glib::ustring> target_;
//    Gtk::TreeModelColumn<Gtk::ProgressBar> progress_;
    Gtk::TreeModelColumn<int> progress_;
};
} // namespace Argon::gui
#endif // ARGON_DOWNLOAD_COLUMNS_HPP
