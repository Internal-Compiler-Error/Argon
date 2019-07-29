#ifndef ARGON_DOWNLOAD_HPP
#define ARGON_DOWNLOAD_HPP

#include <boost/asio/ssl/context_base.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast.hpp>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

namespace Argon::network
{
  enum class conn_type
  {
    ftp,
    http,
    https,
    BitTorrent,
  };

  template <typename buffer = boost::beast::flat_buffer, typename response_body = boost::beast::http::vector_body<char>>
  class basic_sub_download : public std::enable_shared_from_this<basic_sub_download<buffer, response_body>>
  {
  public:
    template <typename T = boost::beast::http::request<boost::beast::http::string_body>>
    explicit basic_sub_download(boost::asio::io_context& ioc,
                                std::string_view         host,
                                std::string_view         target_path,
                                std::string_view         location,
                                std::size_t              size,
                                conn_type                protocol,
                                T&&                      request)
        : ioc_{ ioc },
          host_{ host },
          location_{ location },
          target_path_{ target_path },
          size_{ size },
          protocol_{ protocol },
          request_{ std::forward<T>(request) }
    {
      // stuff
    }

    void start()
    {
      namespace ip  = boost::asio::ip;
      namespace ssl = boost::asio::ssl;
      using namespace boost::beast;

      ip::tcp::resolver resolver{ ioc_ };
      if (protocol_ == conn_type::https)
      {
        const auto   endpoint = resolver.resolve(host_, "https");
        ssl::context ctx{ ssl::context::tls_client };
        load_root_certificate(ctx, e);
        //        ctx.set_verify_mode(ssl::verify_peer);
        ctx.set_default_verify_paths();

        ssl::stream<tcp_stream> ssl_stream{ ioc_, ctx };

        auto string = std::string{ host_ };
        if (!SSL_set_tlsext_host_name(ssl_stream.native_handle(), string.c_str()))
        {
          error_code ec{ static_cast<int>(::ERR_get_error()), net::error::get_ssl_category() };
          throw system_error{ ec };
        }

        get_lowest_layer(ssl_stream).connect(endpoint);
        ssl_stream.handshake(ssl::stream_base::client);

        http::write(ssl_stream, request_);

        http::response_parser<http::vector_body<unsigned char>> res;
        res.body_limit(std::numeric_limits<uint64_t>::max());

        http::async_read(ssl_stream, buffer_, res, [&](error_code, size_t bytes_transferred) {
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
        const auto endpoint = resolver.resolve(host_, "http");
        tcp_stream stream{ ioc_ };
        stream.connect(endpoint);

        http::write(stream, request_);

        http::response_parser<http::vector_body<char>> res;
        res.body_limit(std::numeric_limits<uint64_t>::max());

        //        http::read(stream, buf, res);

        http::async_read(stream, buffer_, res, [&](error_code&, std::size_t bytes_transferred) {
          std::fstream file{ "out" };
          auto&        content = res.get().body();

          for (const auto& item : content)
          {
            file << item;
          }
        });
      }
    }

  private:
    boost::asio::io_context& ioc_;
    buffer                   buffer_;

    std::string location_;
    //    [[deprecated]] std::string ip_;
    std::string host_;
    std::string target_path_;
    conn_type   protocol_{};
    //    std::string                target_;
    std::size_t              size_{};
    std::size_t              downloaded_{};
    double                   progress_{};
    boost::beast::error_code e;

    boost::beast::http::request<boost::beast::http::string_body> request_;
    boost::beast::http::response_parser<response_body>           response_;
  };

  using sub_download = basic_sub_download<boost::beast::flat_buffer, boost::beast::http::vector_body<char>>;

  class download : public std::enable_shared_from_this<download>
  {
  public:
    // friend functions for json parser
    friend void to_json(nlohmann::json& j, const download& d);
    friend void from_json(const nlohmann::json& j, download& d);

    /**
     * @brief
     * Constructs a download from a uri string. The string should be an original request that may require redirects to
     * reach the final location.
     *
     * @param str the uri string
     */
    explicit download(const std::string&       uri,
                      std::size_t              sub_download_count,
                      boost::asio::io_context& ioc) noexcept(false);

    template <typename T>
    explicit download(const boost::beast::http::response<T>& response);

    void start_all_child_downloads();

    download();
    ~download() = default;

    download& operator=(const download& rhs) = delete;
    download& operator=(download&& rhs) = delete;

    download(const download& rhs) = delete;
    download(download&& rhs)      = delete;

    // equality operators
    bool operator==(download const& rhs) const;
    bool operator!=(download const& rhs) const;

    [[nodiscard]] inline bool is_https() const noexcept { return protocol_ == conn_type::https; }

    // setters and getters
    [[nodiscard]] unsigned int       get_index() const;
    void                             set_index(unsigned int index);
    [[nodiscard]] const std::string& get_link() const;
    void                             set_link(std::string const& link);
    [[nodiscard]] conn_type          get_protocol() const;
    void                             set_protocol(conn_type protocol);
    [[nodiscard]] const std::string& get_target() const;
    void                             set_target(std::string const& target);
    [[nodiscard]] double             get_progress() const;
    void                             set_progress(double progress) noexcept;
    [[nodiscard]] const std::string& get_original_request_url() const;
    void                             set_original_request_url_(const std::string& raw_uri_);
    [[nodiscard]] const std::string& get_ip() const;
    void                             set_ip(const std::string& ip);
    [[nodiscard]] const std::string& get_target_path() const;
    void                             set_target_path(const std::string& target_path);
    [[nodiscard]] const std::string& get_host() const;
    void                             set_host(const std::string& host);
    void                             set_original_request_url(const std::string& original_request_url);
    [[nodiscard]] size_t             get_size() const;
    void                             set_size(size_t size);
    size_t                           get_downloaded() const;
    void                             set_downloaded(size_t downloaded);

  private:
    unsigned int index_{};
    std::string              original_request_url_;
    std::string  link_;
    std::string  ip_;
    std::string  host_;
    std::string  target_path_;
    conn_type    protocol_{};
    std::string  target_;
    std::size_t              size_{};
    std::size_t              downloaded_{};
    double                   progress_{};
    boost::asio::io_context& ioc_;

    std::vector<sub_download> sub_downloads_;
  };

  // functions needed to serialize and deserialize jsons
  void to_json(nlohmann::json& j, const Argon::network::download& d);
  void from_json(const nlohmann::json& j, Argon::network::download& d);

  NLOHMANN_JSON_SERIALIZE_ENUM(conn_type,
                               { { conn_type::ftp, "ftp" },
                                 { conn_type::http, "http" },
                                 { conn_type::https, "https" },
                                 { conn_type::BitTorrent, "BitTorrent" } })

} // namespace Argon::network
#endif // ARGON_DOWNLOAD_HPP
