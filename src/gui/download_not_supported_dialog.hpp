#ifndef ARGON_DOWNLOAD_NOT_SUPPORTED_DIALOG_HPP
#define ARGON_DOWNLOAD_NOT_SUPPORTED_DIALOG_HPP

#include <gtkmm/messagedialog.h>

class download_not_supported_dialog : public Gtk::MessageDialog
{
public:
  download_not_supported_dialog();

  download_not_supported_dialog(const download_not_supported_dialog&) = delete;
  download_not_supported_dialog& operator=(const download_not_supported_dialog&) = delete;

  download_not_supported_dialog(download_not_supported_dialog&&) = default;
  download_not_supported_dialog& operator=(download_not_supported_dialog&&) = default;

  ~download_not_supported_dialog() override = default;
};

#endif // ARGON_DOWNLOAD_NOT_SUPPORTED_DIALOG_HPP
