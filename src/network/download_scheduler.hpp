#ifndef ARGON_DOWNLOAD_SCHEDULER_HPP
#define ARGON_DOWNLOAD_SCHEDULER_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "download.hpp"

#include <deque>
#include <memory>
#include <vector>

namespace net = boost::asio;

namespace Argon::network
{
  class download_scheduler
  {
  public:
    void add_download(const download&);
    void add_download(download&&);

    static download_scheduler& get_instance();
    net::io_context&           get_an_io_context();

  public:
    download_scheduler();
    ~download_scheduler() = default;

    download_scheduler(const download_scheduler&) = delete;
    download_scheduler(download_scheduler&&)      = delete;

    download_scheduler& operator=(const download_scheduler&) = delete;
    download_scheduler& operator=(download_scheduler&&) = delete;
    inline int          next_available_ctx();

  private:
    int                          current_io_context{};
    std::vector<net::io_context> io_contexts;
    std::vector<download>        downloads;
    static download_scheduler    instance;
  };
} // namespace Argon::network
#endif // ARGON_DOWNLOAD_SCHEDULER_HPP
