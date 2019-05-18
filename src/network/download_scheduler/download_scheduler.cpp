#include "download_scheduler.hpp"

#include <thread>

Argon::network::download_scheduler::download_scheduler()
        :
        io_contexts{ std::thread::hardware_concurrency() },
        current_io_context{ std::thread::hardware_concurrency() }
{

}
void Argon::network::download_scheduler::add_download_target(download const&)
{

}
