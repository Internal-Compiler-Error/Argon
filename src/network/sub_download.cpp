#include "sub_download.hpp"

using namespace Argon::network;

sub_download::sub_download(int                                                            index,
                           download&                                                      initiator,
                           boost::asio::io_context&                                       ioc,
                           std::string_view                                               host,
                           std::string_view                                               target_path,
                           std::string_view                                               target,
                           std::string_view                                               location,
                           std::size_t                                                    size,
                           conn_type                                                      protocol,
                           boost::beast::http::request<boost::beast::http::string_body>&& request)
    : index_{ index }
    , initiator_{ initiator }
    , ioc_{ ioc }
    , resolver_{ ioc_ }
    , ctx_{ boost::asio::ssl::context::tls_client }
    , ssl_stream_{ ioc_, ctx_ }
    , tcp_stream_{ ioc_ }
    , location_{ location }
    , host_{ host }
    , target_path_{ target_path }
    , target_{ target }
    , protocol_{ protocol }
    , size_{ size }
    , request_{ std::move(request) }
    , response_parser_{}
    , response_{}
{
    response_parser_.body_limit(std::numeric_limits<std::uint64_t>::max());
    sub_download_file_name_ = target_.append("-part-").append(std::to_string(index_));
    load_root_certificate(ctx_, e);
    ctx_.set_default_verify_paths();

    if (!SSL_set_tlsext_host_name(ssl_stream_.native_handle(), host_.c_str()))
    {
        boost::beast::error_code ec{ static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category() };
        throw boost::system::system_error{ ec };
    }
}

void sub_download::start()
{
    namespace ip  = boost::asio::ip;
    namespace ssl = boost::asio::ssl;
    using namespace boost::beast;

    if (protocol_ == conn_type::https)
    {
        resolver_.async_resolve(
            host_,
            "https",
            [self = shared_from_this()](boost::beast::error_code ec, ip::tcp::resolver::results_type results) {
                self->on_https_resolve(ec, results);
            });
    }
    else
    {

        resolver_.async_resolve(host_,
                                "http",
                                [self = shared_from_this()](boost::beast::error_code                     ec,
                                                            boost::asio::ip::tcp::resolver::results_type result) {
                                    self->on_http_resolve(ec, result);
                                });
    }
}

// http related operations
void sub_download::on_http_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type result)
{
    if (ec)
    {
        std::clog << ec.message() << '\n';
        return;
    }

    tcp_stream_.async_connect(
        result,
        [self = shared_from_this()](boost::beast::error_code                                    ec,
                                    boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoint) {
            self->on_http_connect(ec, endpoint);
        });
}

void sub_download::on_http_connect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::endpoint_type endpoint)
{
    if (ec)
    {
        std::clog << ec.message() << '\n';
        return;
    }

    boost::beast::http::async_write(
        tcp_stream_, request_, [self = shared_from_this()](boost::beast::error_code ec, std::size_t bytes_transferred) {
            self->on_http_write(ec, bytes_transferred);
        });
}

void sub_download::on_http_write(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);
    if (ec)
    {
        std::clog << ec.message() << '\n';
        return;
    }

    response_parser_.get().body().open(sub_download_file_name_.c_str(), boost::beast::file_mode::write, e);
    // response_parser_.get().body().file_.open(sub_download_file_name_.c_str(), boost::beast::file_mode::write, e);
    boost::beast::http::async_read(
        tcp_stream_,
        buffer_,
        response_parser_,
        [self = shared_from_this()](const boost::beast::error_code& e, std::size_t bytes_transferred) {
            self->response_ = self->response_parser_.release();
            self->initiator_.report_completed();
            std::cout << "index:" << self->index_ << '\n';
            std::cout << "any error: " << e.message() << '\n';
            std::cout << "bytes transferred: " << bytes_transferred << '\n';
            std::cout << "buffer size: " << self->buffer_.size() << '\n';
        });
}

// https related operations

void sub_download::on_https_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type result)
{
    if (ec)
    {
        std::clog << ec.message() << '\n';
        return;
    }

    boost::beast::get_lowest_layer(ssl_stream_).connect(result);

    ssl_stream_.async_handshake(
        boost::asio::ssl::stream_base::client,
        [self = shared_from_this()](boost::beast::error_code ec) { self->on_https_handshake(ec); });
}

void sub_download::on_https_handshake(boost::beast::error_code ec)
{
    if (ec)
    {
        std::clog << ec.message() << 'n';
        return;
    }

    boost::beast::http::async_write(
        ssl_stream_, request_, [self = shared_from_this()](boost::beast::error_code ec, std::size_t bytes_transferred) {
            self->on_https_write(ec, bytes_transferred);
        });
}

void sub_download::on_https_write(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);
    if (ec)
    {
        std::clog << ec.message() << '\n';
        return;
    }

    response_parser_.get().body().open(sub_download_file_name_.c_str(), boost::beast::file_mode::write, e);
    // response_parser_.get().body().file_.open(sub_download_file_name_.c_str(), boost::beast::file_mode::write, e);
    boost::beast::http::async_read(
        ssl_stream_,
        buffer_,
        response_parser_,
        [self = shared_from_this()](const boost::beast::error_code& e, size_t bytes_transferred) {
            self->initiator_.report_completed();
            std::cout << "index:" << self->index_ << '\n';
            std::cout << "any error: " << e.message() << '\n';
            std::cout << "bytes transferred: " << bytes_transferred << '\n';
            std::cout << "buffer size: " << self->buffer_.size() << '\n';
        });
}
