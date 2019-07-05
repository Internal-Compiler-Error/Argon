#include "download.hpp"

#include <boost/beast.hpp>
//#include <network/uri.hpp>

#include <algorithm>
#include <iterator>
using json = nlohmann::json;
using namespace Argon::network;
using namespace std;

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
  j = json{{"index", d.get_index()},
           {"link", d.get_link()},
           {"protocol", d.get_protocol()},
           {"target", d.get_target()},
           {"progress", d.get_progress()}

  };
}
void Argon::network::from_json(const nlohmann::json& j, download& d)
{
  j.at("index").get_to(d.index_);
  j.at("link").get_to(d.link_);
  j.at("protocol").get_to(d.protocol_);
  j.at("target").get_to(d.target_);
  j.at("progress").get_to(d.progress_);
}
download::download(const std::string& str) noexcept(false) : index_{++index_}
{
  link_ = str;
}
download::download() : index_{++index_} {}
template <typename T>
download::download(const http::response<T>& response)
{
  link_ = response.at(http::field::host) + response.at(http::field::location);
}
