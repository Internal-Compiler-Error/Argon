#include "../../src/network/download.hpp"
#include "../../src/network/network.hpp" // very dirty fix

#include <gtest/gtest.h>
#include <string>

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
  using std::string;
  const string target =
      "https://download.fedoraproject.org/pub/fedora/linux/releases/30/Workstation/x86_64/iso/"
      "Fedora-Workstation-Live-x86_64-30-1.2.iso";
  Argon::network::download d{ target };
}

TEST(network_cpp, get_final_address_works)
{
  net::io_context   ioc;
  const std::string target =
      "https://download.fedoraproject.org/pub/fedora/linux/releases/30/Workstation/x86_64/iso/"
      "Fedora-Workstation-Live-x86_64-30-1.2.iso";
  auto address = Argon::network::get_final_address(target, ioc);
}

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  for (auto i = 0; i < 100; ++i) { auto ret = RUN_ALL_TESTS(); }
}