#ifndef ARGON_DOWNLOAD_HPP
#define ARGON_DOWNLOAD_HPP

#include <boost/beast.hpp>
#include <boost/beast/http/message.hpp>
#include <nlohmann/json.hpp>
#include <string>

namespace net = boost::asio;
using namespace boost::beast;

namespace Argon::network {

enum class conn_type
{
  FTP,
  HTTP,
  HTTPS,
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
  explicit download(const http::response<T>& resposce);

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
  unsigned int       get_index() const;
  void               set_index(unsigned int index);
  std::string const& get_link() const;
  void               set_link(std::string const& link);
  conn_type          get_protocol() const;
  void               set_protocol(conn_type protocol);
  std::string const& get_target() const;
  void               set_target(std::string const& target);
  unsigned int       get_progress() const;
  void               set_progress(unsigned int progress);

private:
  // implementation detail
  unsigned int index_{};
  std::string  link_;
  std::string  ip_;
  conn_type    protocol_{};
  std::string  target_;
  unsigned int progress_{};
};
// functions needed to serialize and deserialize jsons
void to_json(nlohmann::json& j, const Argon::network::download& d);
void from_json(const nlohmann::json& j, Argon::network::download& d);

NLOHMANN_JSON_SERIALIZE_ENUM(conn_type,
                             {{conn_type::FTP, "ftp"},
                              {conn_type::HTTP, "http"},
                              {conn_type::HTTPS, "https"},
                              {conn_type::BitTorrent, "BitTorrent"}})

} // namespace Argon::network
#endif // ARGON_DOWNLOAD_HPP
