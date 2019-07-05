#include "download_scheduler.hpp"
#include "../network/network.hpp" // todo: apart from this thing works, I am DISGUSTED BY IT!

#include <boost/beast.hpp>
#include <thread>
using namespace Argon::network;
using namespace boost::beast;
namespace net = boost::asio;
download_scheduler download_scheduler::instance{};

template <typename T>
inline size_t get_content_lenth(const http::response<T>& res)
{
  std::stringstream ss;
  ss << res.base().at(http::field::content_length);
  size_t ret;
  ss >> ret;
  return ret;
}

int download_scheduler::next_available_ctx()
{
  if (current_io_context == std::thread::hardware_concurrency())
  {
    current_io_context = 0;
    return current_io_context;
  }
  else
  {
    return ++current_io_context;
  }
}

Argon::network::download_scheduler::download_scheduler() : io_contexts{std::thread::hardware_concurrency()} {}

download_scheduler& download_scheduler::get_instance()
{
  return instance;
}
net::io_context& download_scheduler::get_a_io_context()
{
  return io_contexts[next_available_ctx()];
}
void download_scheduler::add_download(const download& download)
{
  downloads.push_back(download);
}

// template <typename T>
void download_scheduler::add_download(const std::string& link)
{

  auto head   = network::details::get_final_dest_HEAD(link, get_a_io_context());
  auto length = get_content_lenth(head);

  auto remainder         = length % io_contexts.size();
  auto each_section_size = (length - remainder) / io_contexts.size();

  for (auto i = 0; i != io_contexts.size(); ++i)
  {
    flat_buffer                       buf;
    http::request<http::dynamic_body> request;
    if (i == io_contexts.size() - 1)
    {
      request.set(http::field::range, std::to_string(each_section_size * (i + 1) + remainder));
    }
    else
    {
      request.set(http::field::range, std::to_string(each_section_size * (i + 1)));
    }
  }
}

void download_scheduler::add_download(download&& download)
{
  downloads.push_back(std::move(download));
}
