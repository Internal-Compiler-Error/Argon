#ifndef ARGON_SUB_DOWNLOAD_HPP
#define ARGON_SUB_DOWNLOAD_HPP

#include "download.hpp"
#include "download_file_body.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

namespace Argon::network
{

class sub_download : public std::enable_shared_from_this<sub_download>
{
public:
    // todo: use some kind of pattern to simplify construction
    explicit sub_download(int                                                            index,
                          download&                                                      initiator,
                          boost::asio::io_context&                                       ioc,
                          std::string_view                                               host,
                          std::string_view                                               target_path,
                          std::string_view                                               target,
                          std::string_view                                               location,
                          std::size_t                                                    size,
                          conn_type                                                      protocol,
                          boost::beast::http::request<boost::beast::http::string_body>&& request);

    void start();

    void on_http_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type result);

    void on_http_connect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::endpoint_type endpoint);

    void on_http_write(boost::beast::error_code ec, std::size_t bytes_transferred);

    void on_https_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type result);

    void on_https_handshake(boost::beast::error_code ec);

    void on_https_write(boost::beast::error_code ec, std::size_t bytes_transferred);

public:
    [[nodiscard]] const std::string& sub_download_file_name() const { return sub_download_file_name_; }

private:
    int                index_;
    network::download& initiator_;

    boost::asio::io_context&                           ioc_;
    boost::beast::flat_buffer                          buffer_;
    boost::asio::ip::tcp::resolver                     resolver_;
    boost::asio::ssl::context                          ctx_;
    boost::asio::ssl::stream<boost::beast::tcp_stream> ssl_stream_;
    boost::beast::tcp_stream                           tcp_stream_;

    std::string location_;
    std::string host_;
    std::string target_path_;
    std::string target_;
    std::string sub_download_file_name_;
    conn_type   protocol_{};
    std::size_t size_{};

    boost::beast::error_code                                           e;
    boost::beast::http::request<boost::beast::http::empty_body>        request_;
    boost::beast::http::response_parser<boost::beast::http::file_body> response_parser_;
    boost::beast::http::response<boost::beast::http::file_body>        response_;
};

} // namespace Argon::network

#endif // ARGON_SUB_DOWNLOAD_HPP
