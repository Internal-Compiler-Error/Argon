#ifndef ARGON_NETWORK_HPP
#define ARGON_NETWORK_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <string>
#include <string_view>

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
namespace ssl   = net::ssl;
using tcp       = net::ip::tcp;

namespace Argon::network
{
void load_root_certificates(ssl::context& ctx, boost::system::error_code& ec);

bool can_be_accelerated(const std::string& url, net::io_context& ioc);

bool can_be_accelerated(const std::string& url);

bool can_be_accelerated(std::string_view url, net::io_context& ioc);

bool can_be_accelerated(std::string_view url);

boost::beast::http::response<boost::beast::http::empty_body> get_HEAD(const std::string& target, net::io_context& ioc);

} // namespace Argon::network

namespace Argon::network::details
{
boost::beast::http::response<boost::beast::http::empty_body> get_HEAD_impl(const std::string& target,
                                                                           net::io_context&   ioc);

boost::beast::http::response<boost::beast::http::empty_body> get_final_dest_HEAD(const std::string& target,
                                                                                 net::io_context&   ioc);
} // namespace Argon::network::details
#endif // ARGON_NETWORK_HPP
