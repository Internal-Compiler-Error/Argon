#include "download_file_body.hpp"

using namespace Argon;


void download_file_body::reader::finish(boost::beast::error_code& ec)
{
    ec = {};
}

void download_file_body::reader::init(const boost::optional<std::uint64_t>& n,
                                      boost::beast::error_code&             ec)
{
    ec = {};
}

bool download_file_body::value_type::is_open() const
{
    return file_.is_open();
}
