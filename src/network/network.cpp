#include "network.hpp"

#include <iostream>
#include <regex>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/detect_ssl.hpp>
#include <boost/beast/http.hpp>
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
    bool   valid = regex_search(url, matches, url_pattern);

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
      ssl::context ctx{ ssl::context::tlsv12_client };

      load_root_certificates(ctx, e);

      // form a stream to carry out data
      beast::ssl_stream<beast::tcp_stream> ssl_stream{ ioc, ctx };

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
        cerr << "**********EXCEPTION CAUGHT***********\n";
        cerr << e.what() << '\n';
        cerr << "they say it's fine that some company don't properly shut down the connection\n";
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
        cerr << "**********EXCEPTION CAUGHT***********\n";
        cout << e.what() << '\n';
      }
      catch (beast::error_code& e)
      {
        cerr << "**********EXCEPTION CAUGHT***********\n";
        cout << e.message() << '\n';
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
