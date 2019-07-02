#include "download_link_input.hpp"
#include "../../network/network.hpp" // todo: fix the disgusting relative path to something provided by cmake
#include "argon_app/argon_app.hpp"
#include "download_not_supported_dialog/download_not_supported_dialog.hpp"

#include <iostream>
#include <regex>
#include <type_traits>

#include <gtkmm/messagedialog.h>

using namespace Argon::gui;
using namespace Argon;
using namespace std;

download_link_input::download_link_input() : confirm_{"confirm"}, cancel_{"cancel"}
{
  auto screen = Gdk::Screen::get_default();
  set_size_request(screen->get_width() / 4, screen->get_height() / 4);
  set_title("new download");
  set_border_width(20);

  box.set_valign(Gtk::ALIGN_CENTER);

  box.pack_start(entry_, Gtk::PACK_SHRINK);

  box.pack_end(button_box_, Gtk::PACK_SHRINK);

  button_box_.pack_start(confirm_, Gtk::PACK_EXPAND_WIDGET);
  button_box_.pack_end(cancel_, Gtk::PACK_EXPAND_WIDGET);

  confirm_.signal_clicked().connect([&]() {
    using namespace boost;

    std::string text = entry_.get_text();

    if (network::can_be_accelerated(text))
    {
    }
    else
    {
      //      cout << response << '\n';
      //      cout << "on confirm\n";
      argon_app::get_instance()->scheduler.add_download(network::download{text});
    }
  });

  cancel_.signal_clicked().connect([]() { cout << "on cancel\n"; });

  add(box);
  show_all_children();
}
//
// inline http::response<http::empty_body> get_HEAD(const string& target, net::io_context& ioc)
//{
//
//  if (auto response = get_HEAD_impl(target, ioc); response.result() == http::status::found)
//  {
//    return get_HEAD_impl(string{response.base().at(http::field::location)}, ioc);
//  }
//  else
//  {
//    return response;
//  }
//}
// http::response<http::empty_body> get_HEAD_impl(const string& target, net::io_context& ioc)
//{
//  regex host_pattern{R"([a-zA-Z]*\.*[a-zA-Z]+\.[a-zA-Z]+)"};
//  regex target_pattern{R"(\/{1}(([A-z0-9.\-_])\/*)+$)"};
//  //  vector<regex> accepted_protocols{regex{"https"}, regex{"http"}};
//
//  smatch host_match; // convoluted concept in the standard library
//  regex_search(target, host_match, host_pattern);
//  string host{host_match[0].str()};
//
//  smatch target_match;
//
//  string destination{host_match[0].second, target.end()};
//
//  // form the HEAD request
//  http::request<http::empty_body> request;
//  request.version(11);
//  request.method(http::verb::head);
//  request.target(destination);
//  request.set(http::field::user_agent, "Argon");
//  request.set(http::field::host, host);
//
//  ssl::context ctx{ssl::context::tlsv12_client}; // set up ssl certificates
////  load_root_certificates(ctx);
//
//  beast::ssl_stream<beast::tcp_stream> ssl_stream{ioc, ctx}; // form a stream to carry out data
//
//  tcp::resolver resolver{ioc}; // get the actual endpoint of the target
//  auto const    results = resolver.resolve(host, "443");
//
//  beast::get_lowest_layer(ssl_stream).connect(results); // connect to it
//  ssl_stream.handshake(ssl::stream_base::client);
//  http::write(ssl_stream, request);
//
//  beast::flat_buffer                      buf;
//  http::response_parser<http::empty_body> res; // HEAD request has no body
//  res.skip(true);                              // tell the parser to skip body
//
//  http::read(ssl_stream, buf, res); // read data
//
//  //  auto response = res.release();
//  //  if (response.result() == http::status::found)
//  //  res = move(response);
//  //    cout << res.release();
//  //
//  ssl_stream.shutdown();
//  return res.release(); // revert control to the caller
//}