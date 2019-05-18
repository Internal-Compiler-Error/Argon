#include "download_columns.hpp"
using namespace Argon::gui;

download_columns::download_columns()
{
    add(index_);
    add(link_);
    add(ip_);
    add(protocol_);
    add(target_);
    add(progress_);
}
