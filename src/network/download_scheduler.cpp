#include "download_scheduler.hpp"
#include "download_handler.hpp"
#include "network.hpp"

#include <boost/beast.hpp>
#include <iostream>
#include <limits>
#include <memory>
#include <thread>

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
namespace ssl   = net::ssl;
using tcp       = net::ip::tcp;

namespace net = boost::asio;
using namespace boost::beast;

using namespace Argon::network;
using namespace boost::beast;
namespace net = boost::asio;

download_scheduler download_scheduler::instance{};

namespace
{
  std::string size_t_to_string(std::size_t size)
  {
    std::stringstream ss;
    ss << size;
    return ss.str();
  }

  inline std::string form_range_header(std::size_t begin, std::size_t end)
  {
    const std::string bytes  = "bytes=";
    const auto        begin_ = size_t_to_string(begin);
    const auto        end_   = size_t_to_string(end);
    return bytes + begin_ + "-" + end_;
  }

  template <typename T>
  inline size_t get_content_length(const http::response<T>& res)
  {
    std::stringstream ss;
    ss << res.base().at(http::field::content_length);
    size_t ret;
    ss >> ret;
    return ret;
  }

} // namespace

size_t download_scheduler::next_available_ctx()
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

Argon::network::download_scheduler::download_scheduler() : io_contexts_{ std::thread::hardware_concurrency() } {}

download_scheduler& download_scheduler::get_instance()
{
  return instance;
}
net::io_context& download_scheduler::get_an_io_context()
{
  return io_contexts_[next_available_ctx()];
}
void download_scheduler::add_download(download& download)
{
  using http::field;

  // all the autos are size_t
  const auto io_context_pool_size = std::thread::hardware_concurrency();
  auto&      ioc                  = get_an_io_context();
  const auto head                 = network::get_HEAD(download.get_link(), ioc);
  const auto total_size           = get_content_length(head);

  const auto each_size = (total_size - total_size % io_context_pool_size) / io_context_pool_size;
  //  const long remainder  = total_size % io_context_pool_size;

  for (size_t i = 0; i < io_context_pool_size; ++i)
  {

    const auto&       host = download.get_host();
    beast::error_code e;

    // form http request
    http::request<http::string_body> req;
    req.set(field::host, host);
    req.version(11);
    req.method(http::verb::get);
    req.target(download.get_target_path());
    req.set(field::user_agent, "Argon");

    req.set(http::field::range, form_range_header(each_size * i, each_size * (i + 1)));

    auto handler = std::make_shared<download_handler<http::string_body>>(ioc, std::move(req), download.is_https());

    std::thread t{ [=]() { handler->start(); } };
    t.join();
  }

  downloads.push_back(download);
}

void download_scheduler::add_download(download&& download)
{
  using network::download;

  class download download_ = std::move(download);

  downloads.push_back(std::move(download_));
}
