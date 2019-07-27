#ifndef ARGON_NETWORK_HPP
#define ARGON_NETWORK_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <string>

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
namespace ssl   = net::ssl;
using tcp       = net::ip::tcp;

namespace Argon::network
{
  class invalid_uri : public std::exception
  {
    const char* what() const noexcept override;
  };

  bool can_be_accelerated(const std::string& url, net::io_context& ioc);

  bool can_be_accelerated(const std::string& url);

  boost::beast::http::response<boost::beast::http::empty_body> get_HEAD(const std::string& target,
                                                                        net::io_context&   ioc);

  std::string get_final_address(const std::string& redirected_location, net::io_context&);

} // namespace Argon::network
#endif // ARGON_NETWORK_HPP
