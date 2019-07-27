#include "download_scheduler.hpp"
#include "network.hpp" // todo: apart from this thing works, I am DISGUSTED BY IT!

#include <boost/beast.hpp>
#include <iostream>
#include <thread>
using namespace Argon::network;
using namespace boost::beast;
namespace net = boost::asio;

download_scheduler download_scheduler::instance{};

template <typename T>
inline size_t get_content_length(const http::response<T>& res)
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

Argon::network::download_scheduler::download_scheduler() : io_contexts{ std::thread::hardware_concurrency() } {}

download_scheduler& download_scheduler::get_instance()
{
  return instance;
}
net::io_context& download_scheduler::get_an_io_context()
{
  return io_contexts[next_available_ctx()];
}
void download_scheduler::add_download(const download& download)
{
  using http::field;
  const long io_context_pool_size = std::thread::hardware_concurrency();

  const auto head       = network::get_HEAD(download.get_raw_uri(), get_an_io_context());
  const long total_size = std::stol(head.at(http::field::content_length).to_string());
  const long each_size  = (total_size - total_size % io_context_pool_size) / io_context_pool_size;
  const long remainder  = total_size % io_context_pool_size;

  for (auto i = 0; i < io_context_pool_size; ++i)
  {
    auto future = std::async(std::launch::async, [&]() {
      auto&                  ioc = get_an_io_context();
      net::ip::tcp::resolver resolver{ ioc };
      const auto             address = resolver.resolve(download.get_link());

      http::request<http::empty_body> req;
      req.set(field::host, download.get_host());
      req.version(11);
      req.method(http::verb::get);
      req.target(download.get_target_path());
      req.set(field::user_agent, "Argon");

      std::stringstream ss;

      ss << each_size * (i + 1);
      std::string begin;
      ss >> begin;
      ss << each_size * (i + 2);
      std::string end;
      ss >> end;
      req.set(http::field::range, begin + std::string{ "-" } + end);

      beast::flat_buffer buf;
      beast::tcp_stream  stream{ ioc };
      stream.connect(address);

      http::write(stream, req);

      http::response_parser<http::string_body> res;
      http::read(stream, buf, res);
      std::cout << res.release();
    });
  }

  downloads.push_back(download);
}

void download_scheduler::add_download(download&& download)
{
  downloads.push_back(std::move(download));
}
