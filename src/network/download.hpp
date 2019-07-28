#ifndef ARGON_DOWNLOAD_HPP
#define ARGON_DOWNLOAD_HPP

#include <boost/beast.hpp>
#include <boost/beast/http/message.hpp>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace net = boost::asio;
using namespace boost::beast;

namespace Argon::network
{

  enum class conn_type
  {
    ftp,
    http,
    https,
    BitTorrent,
  };

  class download
  {
  public:
    // friend functions for json parser
    friend void to_json(nlohmann::json& j, const download& d);
    friend void from_json(const nlohmann::json& j, download& d);

  public:
    // constructors and = operators
    explicit download(const std::string&) noexcept(false);

    template <typename T>
    explicit download(const http::response<T>& response);

    download();
    ~download() = default;

    download& operator=(const download& rhs) = delete;
    download& operator=(download&& rhs) = delete;

    download(const download& rhs) = default;
    download(download&& rhs)      = default;

  public:
    // equality operators
    bool operator==(download const& rhs) const;
    bool operator!=(download const& rhs) const;

  public:
    // setters and getters
    [[nodiscard]] unsigned int       get_index() const;
    void                             set_index(unsigned int index);
    [[nodiscard]] const std::string& get_link() const;
    void                             set_link(std::string const& link);
    [[nodiscard]] conn_type          get_protocol() const;
    void                             set_protocol(conn_type protocol);
    [[nodiscard]] const std::string& get_target() const;
    void                             set_target(std::string const& target);
    [[nodiscard]] unsigned int       get_progress() const;
    void                             set_progress(unsigned int progress);

  private:
    // implementation detail
    std::string raw_uri;

  public:
    [[nodiscard]] const std::string& get_raw_uri() const;
    void                             set_raw_uri(const std::string& raw_uri);
    [[nodiscard]] const std::string& get_ip() const;
    void                             set_ip(const std::string& ip);

  private:
    unsigned int index_{};
    std::string  link_;
    std::string  ip_;
    std::string  host_;
    std::string  target_path_;

  public:
    [[nodiscard]] const std::string& get_target_path() const;
    void               set_target_path(const std::string& target_path);

  public:
    [[nodiscard]] const std::string& get_host() const;
    void                             set_host(const std::string& host);

  private:
    conn_type    protocol_{};
    std::string  target_;
    unsigned int progress_{};
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
