#include "download.hpp"
#include "download_scheduler.hpp"
#include "network.hpp"

#include <chrono>
#include <gtest/gtest.h>
#include <string>
#include <thread>
#include <type_traits>

TEST(network_cpp, get_HEAD_does_not_throw)
{
    net::io_context ioc;
    auto            head = Argon::network::get_HEAD(
        "https://download.fedoraproject.org/pub/fedora/linux/releases/30/Workstation/x86_64/iso/"
        "Fedora-Workstation-Live-x86_64-30-1.2.iso",
        ioc);
}

TEST(network_cpp, download_successfully_creates)
{
    //  using std::string;
    //  const string target =
    //      "https://download.fedoraproject.org/pub/fedora/linux/releases/30/Workstation/x86_64/iso/"
    //      "Fedora-Workstation-Live-x86_64-30-1.2.iso";
    //  Argon::network::download d{ target }(<#initializer #>, 0);
}

TEST(network_cpp, get_final_address_works)
{
    net::io_context   ioc;
    const std::string target =
        "https://download.fedoraproject.org/pub/fedora/linux/releases/30/Workstation/x86_64/iso/"
        "Fedora-Workstation-Live-x86_64-30-1.2.iso";
    auto address = Argon::network::get_final_address(target, ioc);
}

TEST(nework_cpp, add_download_works)
{
    using namespace Argon;
    using namespace std;

    const string target =
        "https://download.fedoraproject.org/pub/fedora/linux/releases/30/Workstation/x86_64/iso/"
        "Fedora-Workstation-Live-x86_64-30-1.2.iso";
    auto& instance = network::download_scheduler::get_instance();

    instance.add_download(target);
    instance.deque();
}


TEST(network_cpp, download_is_move_construcable)
{
    EXPECT_TRUE(std::is_move_constructible<Argon::network::download>::value);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}