#ifndef ARGON_DOWNLOAD_SCHEDULER_HPP
#define ARGON_DOWNLOAD_SCHEDULER_HPP
#include "download.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <deque>
#include <memory>
#include <vector>

namespace net = boost::asio;

namespace Argon::network
{
class download_scheduler
{
public:
    void add_download(std::string_view uri);

    static download_scheduler& get_instance();
    net::io_context&           get_an_io_context();

    void deque();

    download_scheduler();
    ~download_scheduler() = default;

    download_scheduler(const download_scheduler&)     = delete;
    download_scheduler(download_scheduler&&) noexcept = delete;

    download_scheduler& operator=(const download_scheduler&) = delete;
    download_scheduler& operator=(download_scheduler&&) noexcept = delete;
    inline std::size_t  next_available_ctx();

private:
    std::size_t                            current_io_context{};
    std::vector<net::io_context>           io_contexts_;
    std::vector<std::thread>               thread_pool_;
    std::vector<std::unique_ptr<download>> downloads;
    static download_scheduler              instance;
};
} // namespace Argon::network
#endif // ARGON_DOWNLOAD_SCHEDULER_HPP
