#ifndef ARGON_DOWNLOAD_FILE_CHOOSER_HPP
#define ARGON_DOWNLOAD_FILE_CHOOSER_HPP

#include <gtkmm/filechooser.h>
class download_file_chooser : Gtk::FileChooser
{
public:
    download_file_chooser()           = default;
    ~download_file_chooser() override = default;

    download_file_chooser& operator=(const download_file_chooser&) = delete;
    download_file_chooser& operator=(download_file_chooser&&) = delete;

    download_file_chooser(const download_file_chooser&&) = delete;
    download_file_chooser(download_file_chooser&&)       = delete;
};

#endif // ARGON_DOWNLOAD_FILE_CHOOSER_HPP
