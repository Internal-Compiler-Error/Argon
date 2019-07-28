#include "network.hpp"

#include <iostream>
#include <regex>

#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>

using namespace std;
using namespace Argon;

namespace
{
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
        "-----END CERTIFICATE-----";
    ;

    ctx.add_certificate_authority(boost::asio::buffer(cert.data(), cert.size()), ec);
    if (ec) return;
  }

  inline bool is_https(string const& str) { return str.substr(0, 5) == "https"s; }
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

    if (is_https(url)) { return uri{ host, destination, connection_type::https }; }
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

      load_root_certificates(ctx, e);
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

      beast::http::response_parser<beast::http::empty_body> res; // HEAD request has no body
      res.skip(true);                                            // tell the parser to skip body

      http::read(ssl_stream, buf, res); // read data

      try
      {
        ssl_stream.shutdown();
      }
      catch (exception& e)
      {
        // see this:
        // https://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error I'm
        // not an expert either but hey the test passes! (exception& e){
        clog << "**********EXCEPTION CAUGHT***********\n";
        clog << e.what() << '\n';
        clog << "Some say it's fine that some company don't properly shut down the connection\n";
        clog << "**********CATCH BLOCK ENDS***********\n";
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
      catch (beast::system_error& e)
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

  // 302 means resource found but redirect is needed
  // cursively look for any more redirects, until a final destination is found
  if (head.result_int() == 302)
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

  // 302 means redirect
  if (head.result_int() == 302)
  {
    auto new_location = head.at(http::field::location).to_string();
    return get_final_address(new_location, ioc);
  }
  else
  {
    return redirected_location;
  }
}
