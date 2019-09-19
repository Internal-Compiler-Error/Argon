#include "download_scheduler.hpp"

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

download_scheduler download_scheduler::instance;

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

Argon::network::download_scheduler::download_scheduler()
    : io_contexts_{ std::thread::hardware_concurrency() }
{
}

download_scheduler& download_scheduler::get_instance()
{
    // static download_scheduler instance_;
    return instance;
}

net::io_context& download_scheduler::get_an_io_context()
{
    return io_contexts_[next_available_ctx()];
}

void download_scheduler::add_download(std::string_view uri)
{
    downloads.emplace_back(
        std::make_unique<network::download>(*this, std::string{ uri }, io_contexts_.size(), get_an_io_context()));

}

void download_scheduler::deque()
{
    for (auto& download : downloads)
    {
        thread_pool_.emplace_back([&]() { download->get_ioc().run(); });
    }
    for (auto& thread : thread_pool_)
    {
        thread.join();
    }
    for (auto& download : downloads)
    {
        download->append_all_sub_downloads();
    }
}
