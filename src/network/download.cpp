#include "download.hpp"

#include "network.hpp"

#include <algorithm>
#include <iterator>
#include <regex>

#include <boost/beast.hpp>

using json = nlohmann::json;
using namespace Argon::network;
using namespace std;

namespace
{
  unsigned  index_g{};
  conn_type str_to_conn_type(const string& str)
  {
    if (str == "https"s) { return conn_type::https; }
    else if (str == "http"s)
    {
      return conn_type::http;
    }
    else if (str == "ftp"s)
    {
      return conn_type::ftp;
    }
    else
    {
      return {};
    }
  }
} // namespace

bool download::operator==(download const& rhs) const
{
  return index_ == rhs.index_ && link_ == rhs.link_ && ip_ == rhs.ip_ && protocol_ == rhs.protocol_ &&
         target_ == rhs.target_ && progress_ == rhs.progress_;
}

bool download::operator!=(download const& rhs) const
{
  return !(rhs == *this);
}
unsigned int download::get_index() const
{
  return index_;
}
void download::set_index(unsigned int index)
{
  index_ = index;
}
std::string const& download::get_link() const
{
  return link_;
}
void download::set_link(std::string const& link)
{
  link_ = link;
}
conn_type download::get_protocol() const
{
  return protocol_;
}
void download::set_protocol(conn_type protocol)
{
  protocol_ = protocol;
}
std::string const& download::get_target() const
{
  return target_;
}
void download::set_target(std::string const& target)
{
  target_ = target;
}
unsigned int download::get_progress() const
{
  return progress_;
}
void download::set_progress(unsigned int progress)
{
  progress_ = progress;
}

void Argon::network::to_json(nlohmann::json& j, const download& d)
{
  j = json{ { "index_g", d.get_index() },
            { "link", d.get_link() },
            { "protocol", d.get_protocol() },
            { "target", d.get_target() },
            { "progress", d.get_progress() }

  };
}
void Argon::network::from_json(const nlohmann::json& j, download& d)
{
  j.at("index_g").get_to(d.index_);
  j.at("link").get_to(d.link_);
  j.at("protocol").get_to(d.protocol_);
  j.at("target").get_to(d.target_);
  j.at("progress").get_to(d.progress_);
}
download::download(const std::string& str) noexcept(false) : index_{ ++index_g }
{
  net::io_context ioc;
  auto            final_url = network::get_final_address(str, ioc);
  regex  url_pattern{ R"(^((http[s]?|ftp):\/)?\/?([^:\/\s]+)((\/\w+)*\/)([\w\-\.]+[^#?\s]+)(.*)?(#[\w\-]+)?$)" };
  smatch matches;

  regex_match(final_url, matches, url_pattern);

  protocol_    = str_to_conn_type(matches[2]);
  link_        = matches[0];
  target_      = matches[6];
  host_        = matches[3];
  target_path_ = matches[4].str() + matches[6].str();
}
download::download() : index_{ ++index_g } {}

template <typename T>
download::download(const http::response<T>& response)
{
  link_ = response.at(http::field::host) + response.at(http::field::location);
}
const string& download::get_raw_uri() const
{
  return raw_uri;
}
void download::set_raw_uri(const string& raw_uri)
{
  download::raw_uri = raw_uri;
}
const string& download::get_ip() const
{
  return ip_;
}
void download::set_ip(const string& ip)
{
  ip_ = ip;
}
const string& download::get_host() const
{
  return host_;
}
void download::set_host(const string& host)
{
  download::host_ = host;
}
const string& download::get_target_path() const
{
  return target_path_;
}
void download::set_target_path(const string& target_path)
{
  target_path_ = target_path;
}
