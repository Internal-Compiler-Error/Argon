#ifndef ARGON_NETWORK_HPP
#define ARGON_NETWORK_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <string>

namespace Argon::network
{
class invalid_uri : public std::exception
{
    const char* what() const noexcept override;
};

void load_root_certificate(boost::asio::ssl::context& ctx, boost::system::error_code& ec);

bool is_valid_uri(const std::string& str);

bool can_be_accelerated(const std::string& url, boost::asio::io_context& ioc);

bool can_be_accelerated(const std::string& url);

boost::beast::http::response<boost::beast::http::empty_body> get_HEAD(const std::string&       target,
                                                                      boost::asio::io_context& ioc);

std::string get_final_address(const std::string& redirected_location, boost::asio::io_context&);

} // namespace Argon::network
#endif // ARGON_NETWORK_HPP
