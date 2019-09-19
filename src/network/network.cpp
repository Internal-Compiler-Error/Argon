#include "network.hpp"

#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <fstream>
#include <iostream>
#include <regex>

using namespace std;
using namespace Argon;

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
namespace ssl   = net::ssl;
using tcp       = net::ip::tcp;

namespace
{
inline bool is_https(string const& str)
{
    return str.substr(0, 5) == "https";
}
enum class connection_type
{
    http,
    https
};

struct uri
{
    string          host;
    string          destination;
    connection_type type;
};

// form a uri from a string
[[nodiscard]] uri get_uri(const string& url) noexcept(false)
{
    regex url_pattern{ R"(^((http[s]?|ftp):\/\/)?\/?([^:\/\s]+)((\/\w+)*\/)([\w\-\.]+[^#?\s]+)(.*)?(#[\w\-]+)?$)" };

    smatch matches;
    regex_search(url, matches, url_pattern);

    //    if (!valid) { throw network::invalid_uri{}; }
    //    else
    //    {
    // see https://regexr.com/4i6so for details
    string host{ matches[3].str() };

    string destination{ matches[4].str() + matches[6].str() };

    if (is_https(url))
    {
        return uri{ host, destination, connection_type::https };
    }
    else
    {
        return uri{ host, destination, connection_type::http };
    }
    //    }
}

http::response<http::empty_body> get_current_HEAD(const string& url, net::io_context& ioc)
{
    uri uri = get_uri(url);

    // form the HEAD request
    beast::http::request<beast::http::empty_body> request;
    request.version(11);
    request.method(beast::http::verb::head);
    request.target(uri.destination);
    request.set(beast::http::field::user_agent, "Argon");
    request.set(beast::http::field::host, uri.host);

    beast::error_code e;

    if (uri.type == connection_type::https)
    {
        tcp::resolver      resolver{ ioc };
        beast::flat_buffer buf;

        // set up ssl certificates
        ssl::context ctx{ ssl::context::tls_client };

        network::load_root_certificate(ctx, e);
        //      ctx.set_verify_mode(ssl::verify_peer);
        ctx.set_default_verify_paths();

        // form a stream to carry out data
        beast::ssl_stream<beast::tcp_stream> ssl_stream{ ioc, ctx };

        if (!SSL_set_tlsext_host_name(ssl_stream.native_handle(), uri.host.c_str()))
        {
            beast::error_code ec{ static_cast<int>(::ERR_get_error()), net::error::get_ssl_category() };
            throw beast::system_error{ ec };
        }

        // get the actual endpoint of the target
        auto const address = resolver.resolve(uri.host, "https");

        // connect to it
        beast::get_lowest_layer(ssl_stream).connect(address);

        ssl_stream.handshake(ssl::stream_base::client);

        beast::http::write(ssl_stream, request);

        http::response_parser<beast::http::empty_body> res; // HEAD request has no body
        res.skip(true);                                     // tell the parser to skip body

        http::read(ssl_stream, buf, res); // read data

        try
        {
            ssl_stream.shutdown(e);
            if (e == net::error::eof)
            {
                // see this:
                // https://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
                // I'm not an expert either but hey the test passes!
                clog << "not a real error \n";
                clog << e.message();

                e = {};
            }
            if (e)
            {
                throw beast::system_error{ e };
            }
        }
        catch (exception& e)
        {
            clog << e.what() << '\n';
        }

        return res.release(); // revert control to the caller
    }
    else
    {
        tcp::resolver      resolver{ ioc };
        beast::flat_buffer buf;
        beast::tcp_stream  stream{ ioc };

        // get address of the server
        auto const address = resolver.resolve(uri.host, "http");

        // connect to the server
        stream.connect(address); // many many hours of paining debugging led me realize I missed this line...

        // write the HEAD request to the server
        http::write(stream, request);

        http::response_parser<http::empty_body> res; // HEAD request has no body
        res.skip(true);                              // tell the parser to skip body

        try
        {
            // read the response back from the server
            http::read(stream, buf, res);
        }
        catch (std::exception& e)
        {
            clog << "**********EXCEPTION CAUGHT***********\n";
            cout << e.what() << '\n';
            clog << "**********CATCH BLOCK ENDS***********\n";
        }
        return res.release();
    }
}

} // namespace

const char* network::invalid_uri::what() const noexcept
{
    return "mal-formed uri, this is not legal";
}

bool network::can_be_accelerated(const std::string& url, net::io_context& ioc)
{
    auto head = network::get_HEAD(url, ioc);
    return head.has_content_length();
}

bool Argon::network::can_be_accelerated(const std::string& url)
{
    net::io_context ioc;
    return can_be_accelerated(url, ioc);
}

http::response<http::empty_body> network::get_HEAD(const std::string& target, net::io_context& ioc)
{
    auto head = get_current_HEAD(target, ioc);

    // cursively look for any more redirects, until a final destination is found
    if (const auto& result = head.result(); result == http::status::found ||
                                            result == http::status::permanent_redirect ||
                                            result == http::status::temporary_redirect)
    {
        string location = head.at(http::field::location).to_string();
        return get_HEAD(location, ioc);
    }
    else
    {
        return head;
    }
}
std::string network::get_final_address(const std::string& redirected_location, net::io_context& ioc)
{
    auto head = get_current_HEAD(redirected_location, ioc);

    if (const auto& result = head.result(); result == http::status::found ||
                                            result == http::status::permanent_redirect ||
                                            result == http::status::temporary_redirect)
    {
        auto new_location = head.at(http::field::location).to_string();
        return get_final_address(new_location, ioc);
    }
    else
    {
        return redirected_location;
    }
}
void network::load_root_certificate(ssl::context& ctx, boost::system::error_code& ec)
{
    std::ifstream     root_certificate{ "root_certificate.pem", std::ios::in };
    std::stringstream ss;
    ss << root_certificate.rdbuf();
    std::string cert2;
    ss >> cert2;

    ctx.add_certificate_authority(boost::asio::buffer(cert2.data(), cert2.size()), ec);
    if (ec)
    {
        return;
    }
}
bool network::is_valid_uri(const std::string& str)
{
    regex pattern{ R"(^((http[s]?|ftp):\/\/)?\/?([^:\/\s]+)((\/\w+)*\/)([\w\-\.]+[^#?\s]+)(.*)?(#[\w\-]+)?$)" };
    return regex_match(str, pattern);
}
