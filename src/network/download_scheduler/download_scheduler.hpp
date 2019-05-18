#ifndef ARGON_DOWNLOAD_SCHEDULER_HPP
#define ARGON_DOWNLOAD_SCHEDULER_HPP

#include <boost/beast.hpp>
#include <boost/asio.hpp>

//#include "../download/download.hpp"
#include "download.hpp"
#include <vector>
#include <deque>

namespace Argon::network {
class download_scheduler {
public:

    void add_download_target(const download&);

    download_scheduler();
    ~download_scheduler() = default;

    download_scheduler(const download_scheduler&) = delete;
    download_scheduler(download_scheduler&&) = delete;

    download_scheduler& operator=(const download_scheduler&) = delete;
    download_scheduler& operator=(download_scheduler&&) = delete;

private:
    unsigned int current_io_context;
    std::vector<boost::asio::io_context> io_contexts;
    std::vector<download> downloads;

};
} // namespace Argon
#endif //ARGON_DOWNLOAD_SCHEDULER_HPP
