#ifndef ARGON_DOWNLOAD_HANDLER_HPP
#define ARGON_DOWNLOAD_HANDLER_HPP

#include "network.hpp"
#include <fstream>
#include <memory>

#include "download.hpp"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>

namespace Argon::network
{
  template <typename T>
  class download_handler : protected download, public std::enable_shared_from_this<download_handler<T>>
  {
  public:
    explicit download_handler(boost::asio::io_context& ioc, boost::beast::http::request<T>&& req, bool is_https)
        : ioc_{ ioc }, request_{ std::move(req) }, is_https_{ is_https }
    {}
    void start()
    {
      namespace beast = boost::beast;
      namespace http  = beast::http;
      namespace net   = boost::asio;
      namespace ssl   = net::ssl;
      using tcp       = net::ip::tcp;

      namespace net = boost::asio;
      using namespace boost::beast;

      using namespace Argon::network;
      using namespace boost::beast;
      namespace net = boost::asio;

      net::ip::tcp::resolver resolver{ ioc_ };
      //      const auto             protocol = request_.at(http::field::protocol);
      const auto host = std::string{ request_.at(http::field::host) };

      beast::error_code e;

      if (is_https_)
      {
        const auto   endpoint = resolver.resolve(host, "https");
        ssl::context ctx{ ssl::context::tls_client };
        network::load_root_certificate(ctx, e);
        //        ctx.set_verify_mode(ssl::verify_peer);
        ctx.set_default_verify_paths();

        ssl::stream<tcp_stream> ssl_stream{ ioc_, ctx };

        auto string = std::string{ host };
        if (!SSL_set_tlsext_host_name(ssl_stream.native_handle(), string.c_str()))
        {
          beast::error_code ec{ static_cast<int>(::ERR_get_error()), net::error::get_ssl_category() };
          throw beast::system_error{ ec };
        }
        beast::get_lowest_layer(ssl_stream).connect(endpoint);
        ssl_stream.handshake(ssl::stream_base::client);

        http::write(ssl_stream, request_);

        http::response_parser<http::vector_body<unsigned char>> res;
        res.body_limit(std::numeric_limits<uint64_t>::max());

        http::async_read(ssl_stream, buf, res, [&](beast::error_code, size_t bytes_transferred) {
          std::fstream file{ "out" };
          auto&        content = res.get().body();

          for (const auto& item : content)
          {

            file << item;
          }
        });
      }
      else
      {
        const auto        endpoint = resolver.resolve(host, "http");
        beast::tcp_stream stream{ ioc_ };
        stream.connect(endpoint);

        http::write(stream, request_);

        http::response_parser<http::vector_body<char>> res;
        res.body_limit(std::numeric_limits<uint64_t>::max());

        //        http::read(stream, buf, res);

        http::async_read(stream, buf, res, [&](const beast::error_code&, std::size_t bytes_transferred) {
          std::fstream file{ "out" };
          auto&        content = res.get().body();

          for (const auto& item : content)
          {

            file << item;
          }
        });
      }

      //     boost::beast::http::async_read()
      ioc_.run();
    }

  private:
    boost::beast::http::request<T> request_;
    boost::beast::flat_buffer      buf;
    boost::asio::io_context&       ioc_;
    bool                           is_https_;
  };

} // namespace Argon::network

#endif // ARGON_DOWNLOAD_HANDLER_HPP
