#ifndef ARGON_DOWNLOAD_SCHEDULER_HPP
#define ARGON_DOWNLOAD_SCHEDULER_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "../download/download.hpp"
namespace net = boost::asio;
#include <deque>
#include <memory>
#include <vector>

namespace Argon::network {
class download_scheduler
{
public:
  void add_download(const download&);
  void add_download(download&&);
  //  template <typename T>
  void                       add_download(const std::string&);
  static download_scheduler& get_instance();
  net::io_context&           get_a_io_context();

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
