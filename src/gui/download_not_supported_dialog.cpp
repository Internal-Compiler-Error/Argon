#include "download_not_supported_dialog.hpp"

download_not_supported_dialog::download_not_supported_dialog()
    : Gtk::MessageDialog{ "No Content-Length present from the server response, Argon cannot accelerate this download!" }
{
  set_title("Operation Not Supported");

  set_secondary_text(
      "The http Content-Length denotes the length of the body in bytes, without such information, Argon "
      "cannot split the download into multiple sections to accelerate the download.");
}
