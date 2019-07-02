#include "download_link_input.hpp"
#include "argon_app.hpp"
#include "download_not_supported_dialog.hpp"

#include <iostream>
#include <regex>
#include <type_traits>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>

#include <gtkmm/messagedialog.h>

#ifndef BOOST_BEAST_EXAMPLE_COMMON_ROOT_CERTIFICATES_HPP
#define BOOST_BEAST_EXAMPLE_COMMON_ROOT_CERTIFICATES_HPP

#include <boost/asio/ssl.hpp>
#include <string>

namespace ssl = boost::asio::ssl; // from <boost/asio/ssl.hpp>

namespace details {

// The template argument is gratuituous, to
// allow the implementation to be header-only.
//
template <class = void>
void load_root_certificates(ssl::context& ctx, boost::system::error_code& ec)
{
  std::string const cert =
      /*  This is the DigiCert root certificate.

          CN = DigiCert High Assurance EV Root CA
          OU = www.digicert.com
          O = DigiCert Inc
          C = US
          Valid to: Sunday, ?November ?9, ?2031 5:00:00 PM

          Thumbprint(sha1):
          5f b7 ee 06 33 e2 59 db ad 0c 4c 9a e6 d3 8f 1a 61 c7 dc 25
      */
      "-----BEGIN CERTIFICATE-----\n"
      "MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n"
      "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
      "d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n"
      "ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n"
      "MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n"
      "LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n"
      "RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n"
      "+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n"
      "PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n"
      "xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n"
      "Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n"
      "hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n"
      "EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n"
      "MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n"
      "FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n"
      "nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n"
      "eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n"
      "hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n"
      "Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n"
      "vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n"
      "+OkuE6N36B9K\n"
      "-----END CERTIFICATE-----\n"
      "-----BEGIN CERTIFICATE-----\n"
      "MIIDaDCCAlCgAwIBAgIJAO8vBu8i8exWMA0GCSqGSIb3DQEBCwUAMEkxCzAJBgNV\n"
      "BAYTAlVTMQswCQYDVQQIDAJDQTEtMCsGA1UEBwwkTG9zIEFuZ2VsZXNPPUJlYXN0\n"
      "Q049d3d3LmV4YW1wbGUuY29tMB4XDTE3MDUwMzE4MzkxMloXDTQ0MDkxODE4Mzkx\n"
      "MlowSTELMAkGA1UEBhMCVVMxCzAJBgNVBAgMAkNBMS0wKwYDVQQHDCRMb3MgQW5n\n"
      "ZWxlc089QmVhc3RDTj13d3cuZXhhbXBsZS5jb20wggEiMA0GCSqGSIb3DQEBAQUA\n"
      "A4IBDwAwggEKAoIBAQDJ7BRKFO8fqmsEXw8v9YOVXyrQVsVbjSSGEs4Vzs4cJgcF\n"
      "xqGitbnLIrOgiJpRAPLy5MNcAXE1strVGfdEf7xMYSZ/4wOrxUyVw/Ltgsft8m7b\n"
      "Fu8TsCzO6XrxpnVtWk506YZ7ToTa5UjHfBi2+pWTxbpN12UhiZNUcrRsqTFW+6fO\n"
      "9d7xm5wlaZG8cMdg0cO1bhkz45JSl3wWKIES7t3EfKePZbNlQ5hPy7Pd5JTmdGBp\n"
      "yY8anC8u4LPbmgW0/U31PH0rRVfGcBbZsAoQw5Tc5dnb6N2GEIbq3ehSfdDHGnrv\n"
      "enu2tOK9Qx6GEzXh3sekZkxcgh+NlIxCNxu//Dk9AgMBAAGjUzBRMB0GA1UdDgQW\n"
      "BBTZh0N9Ne1OD7GBGJYz4PNESHuXezAfBgNVHSMEGDAWgBTZh0N9Ne1OD7GBGJYz\n"
      "4PNESHuXezAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQCmTJVT\n"
      "LH5Cru1vXtzb3N9dyolcVH82xFVwPewArchgq+CEkajOU9bnzCqvhM4CryBb4cUs\n"
      "gqXWp85hAh55uBOqXb2yyESEleMCJEiVTwm/m26FdONvEGptsiCmF5Gxi0YRtn8N\n"
      "V+KhrQaAyLrLdPYI7TrwAOisq2I1cD0mt+xgwuv/654Rl3IhOMx+fKWKJ9qLAiaE\n"
      "fQyshjlPP9mYVxWOxqctUdQ8UnsUKKGEUcVrA08i1OAnVKlPFjKBvk+r7jpsTPcr\n"
      "9pWXTO9JrYMML7d+XRSZA1n3856OqZDX4403+9FnXCvfcLZLLKTBvwwFgEFGpzjK\n"
      "UEVbkhd5qstF6qWK\n"
      "-----END CERTIFICATE-----\n";
  /*  This is the GeoTrust root certificate.

      CN = GeoTrust Global CA
      O = GeoTrust Inc.
      C = US
      Valid to: Friday, ‎May ‎20, ‎2022 9:00:00 PM

      Thumbprint(sha1):
      ‎de 28 f4 a4 ff e5 b9 2f a3 c5 03 d1 a3 49 a7 f9 96 2a 82 12
  */
  ;

  ctx.add_certificate_authority(boost::asio::buffer(cert.data(), cert.size()), ec);
  if (ec) return;
}

} // namespace details

// Load the root certificates into an ssl::context
//
// This function is inline so that its easy to take
// the address and there's nothing weird like a
// gratuituous template argument; thus it appears
// like a "normal" function.
//

inline void load_root_certificates(ssl::context& ctx, boost::system::error_code& ec)
{
  details::load_root_certificates(ctx, ec);
}

inline void load_root_certificates(ssl::context& ctx)
{
  boost::system::error_code ec;
  details::load_root_certificates(ctx, ec);
  if (ec) throw boost::system::system_error{ec};
}

#endif

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http  = beast::http;  // from <boost/beast/http.hpp>
namespace net   = boost::asio;  // from <boost/asio.hpp>
namespace ssl   = net::ssl;     // from <boost/asio/ssl.hpp>
using tcp       = net::ip::tcp; // from <boost/asio/ip/tcp.hpp>
using namespace Argon::gui;
using namespace std;

http::response<http::empty_body> get_HEAD(const string& target, net::io_context& ioc);
http::response<http::empty_body> get_HEAD_impl(const string& target, net::io_context& ioc);

download_link_input::download_link_input() : confirm_{"confirm"}, cancel_{"cancel"}
{
  auto screen = Gdk::Screen::get_default();
  set_size_request(screen->get_width() / 4, screen->get_height() / 4);
  set_title("new download");
  set_border_width(20);

  box.set_valign(Gtk::ALIGN_CENTER);

  box.pack_start(entry_, Gtk::PACK_SHRINK);

  box.pack_end(button_box_, Gtk::PACK_SHRINK);

  button_box_.pack_start(confirm_, Gtk::PACK_EXPAND_WIDGET);
  button_box_.pack_end(cancel_, Gtk::PACK_EXPAND_WIDGET);

  confirm_.signal_clicked().connect([&]() {
    using namespace boost;

    std::string text = entry_.get_text();
    //
    //    // I have no idea if this is right or not
    //    std::regex url_pattern{R"(^[a-z][a-z0-9+\-.]*://([a-z0-9\-._~%!$&'()*+,;=]+@)?([a-z0-9\-._~%]+|↵
    //\[[a-f0-9:.]+\]|\[v[a-f0-9][a-z0-9\-._~%!$&'()*+,;=:]+\])(:[0-9]+)?↵
    //(/[a-z0-9\-._~%!$&'()*+,;=:@]+)*/?(\?[a-z0-9\-._~%!$&'()*+,;=:@/?]*)?↵
    //(#[a-z0-9\-._~%!$&'()*+,;=:@/?]*)?$)",
    //                           std::regex::ECMAScript};
    //    smatch     result;

    if (auto response = get_HEAD(text, argon_app::get_instance()->scheduler.get_a_io_context());
        !response.has_content_length())
    {
      cerr << "No Content-Length present from the server response, Argon cannot accelerate this download! \n";
      download_not_supported_dialog dialog;
      dialog.run();
    }
    else
    {
      //      cout << response << '\n';
      //      cout << "on confirm\n";
      argon_app::get_instance()->scheduler.add_download(network::download{text});
    }
  });

  cancel_.signal_clicked().connect([]() { cout << "on cancel\n"; });

  add(box);
  show_all_children();
}

inline http::response<http::empty_body> get_HEAD(const string& target, net::io_context& ioc)
{

  if (auto response = get_HEAD_impl(target, ioc); response.result() == http::status::found)
  {
    return get_HEAD_impl(string{response.base().at(http::field::location)}, ioc);
  }
  else
  {
    return response;
  }
}
http::response<http::empty_body> get_HEAD_impl(const string& target, net::io_context& ioc)
{
  regex host_pattern{R"([a-zA-Z]*\.*[a-zA-Z]+\.[a-zA-Z]+)"};
  regex target_pattern{R"(\/{1}(([A-z0-9.\-_])\/*)+$)"};
  //  vector<regex> accepted_protocols{regex{"https"}, regex{"http"}};

  smatch host_match; // convoluted concept in the standard library
  regex_search(target, host_match, host_pattern);
  string host{host_match[0].str()};

  smatch target_match;

  string destination{host_match[0].second, target.end()};

  // form the HEAD request
  http::request<http::empty_body> request;
  request.version(11);
  request.method(http::verb::head);
  request.target(destination);
  request.set(http::field::user_agent, "Argon");
  request.set(http::field::host, host);

  ssl::context ctx{ssl::context::tlsv12_client}; // set up ssl certificates
  load_root_certificates(ctx);

  beast::ssl_stream<beast::tcp_stream> ssl_stream{ioc, ctx}; // form a stream to carry out data

  tcp::resolver resolver{ioc}; // get the actual endpoint of the target
  auto const    results = resolver.resolve(host, "443");

  beast::get_lowest_layer(ssl_stream).connect(results); // connect to it
  ssl_stream.handshake(ssl::stream_base::client);
  http::write(ssl_stream, request);

  beast::flat_buffer                      buf;
  http::response_parser<http::empty_body> res; // HEAD request has no body
  res.skip(true);                              // tell the parser to skip body

  http::read(ssl_stream, buf, res); // read data

  //  auto response = res.release();
  //  if (response.result() == http::status::found)
  //  res = move(response);
  //    cout << res.release();
  //
  ssl_stream.shutdown();
  return res.release(); // revert control to the caller
}