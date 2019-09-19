#include "download.hpp"

#include "download_scheduler.hpp"
#include "network.hpp"
#include "sub_download.hpp"

#include <algorithm>
#include <boost/beast.hpp>
#include <iterator>
#include <regex>
#include <type_traits>

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
namespace ssl   = net::ssl;
using tcp       = net::ip::tcp;

namespace net = boost::asio;
using namespace boost::beast;

using json = nlohmann::json;
using namespace Argon::network;
using namespace std;

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

unsigned index_g{};

// todo: throw an exception for unhandled cases instead of a default constructed conn_type
conn_type str_to_conn_type(const string& str)
{
    if (str == "https"s)
    {
        return conn_type::https;
    }
    else if (str == "http"s)
    {
        return conn_type::http;
    }
    else if (str == "ftp"s)
    {
        return conn_type::ftp;
    }
    else
    {
        return {};
    }
}

template <typename T>
inline size_t get_content_length(const boost::beast::http::response<T>& res)
{
    std::stringstream ss;
    ss << res.base().at(boost::beast::http::field::content_length);
    size_t ret;
    ss >> ret;
    return ret;
}

} // namespace

unsigned int download::get_index() const
{
    return index_;
}

void download::set_index(unsigned int index)
{
    index_ = index;
}

std::string const& download::get_link() const
{
    return link_;
}

conn_type download::get_protocol() const
{
    return protocol_;
}

std::string const& download::get_target() const
{
    return target_;
}

double download::get_progress() const
{
    return progress_;
}

void download::set_progress(double progress) noexcept
{
    progress_ = progress;
}

void Argon::network::to_json(nlohmann::json& j, const download& d)
{
    j = json{ { "index_g", d.get_index() },
              { "link", d.get_link() },
              { "protocol", d.get_protocol() },
              { "target", d.get_target() },
              { "progress", d.get_progress() }

    };
}

void Argon::network::from_json(const nlohmann::json& j, download& d)
{
    j.at("index_g").get_to(d.index_);
    j.at("link").get_to(d.link_);
    j.at("protocol").get_to(d.protocol_);
    j.at("target").get_to(d.target_);
    j.at("progress").get_to(d.progress_);
}

download::download(download_scheduler&      scheduler,
                   std::string_view         str,
                   std::size_t              sub_download_count,
                   boost::asio::io_context& ioc) noexcept(false)
    : index_{ ++index_g }
    , scheduler_{ scheduler }
    , original_request_url_{ str }
    , ioc_{ ioc }
    , sub_download_count_{ sub_download_count }
{
    // todo: fix later
    start();
}

const string& download::get_original_request_url() const
{
    return original_request_url_;
}

void download::set_original_request_url_(const string& original_request_url)
{
    download::original_request_url_ = original_request_url;
}

const string& download::get_host() const
{
    return host_;
}

void download::set_host(const string& host)
{
    download::host_ = host;
}

const string& download::get_target_path() const
{
    return target_path_;
}

void download::set_target_path(const string& target_path)
{
    target_path_ = target_path;
}

void download::set_original_request_url(const string& original_request_url)
{
    original_request_url_ = original_request_url;
}

size_t download::get_size() const
{
    return size_;
}

void download::set_size(size_t size)
{
    download::size_ = size;
}

size_t download::get_downloaded() const
{
    return downloaded_;
}

void download::set_downloaded(size_t downloaded)
{
    downloaded_ = downloaded;
}

boost::asio::io_context& download::get_ioc() const
{
    return ioc_;
}

void download::append_all_sub_downloads()
{
    // todo: fix later
    // std::ofstream download_file{ target_, std::ios_base::trunc & std::ios_base::binary & std::ios_base::out };
    //
    // for (auto& sub_download : sub_downloads_)
    // {
    //     std::ifstream sub_download_file{ sub_download.sub_download_file_name() };
    //
    //     download_file << sub_download_file.rdbuf() << std::flush;
    // }
}

void download::report_completed()
{
    // std::lock_guard<std::mutex> lock_guard{ mutex_ };
    ++completed_sub_download_count_;
}

void download::start()
{
    using namespace boost::beast;
    namespace ssl = boost::asio::ssl;
    namespace net = boost::asio;

    const auto  final_url = network::get_final_address(original_request_url_, ioc_);
    const auto  head      = network::get_HEAD(original_request_url_, ioc_);
    const regex url_pattern{ R"(^((http[s]?|ftp):\/)?\/?([^:\/\s]+)((\/\w+)*\/)([\w\-\.]+[^#?\s]+)(.*)?(#[\w\-]+)?$)" };
    smatch      matches;

    regex_match(final_url, matches, url_pattern);

    protocol_    = str_to_conn_type(matches[2]);
    link_        = matches[0];
    target_      = matches[6];
    host_        = matches[3];
    target_path_ = matches[4].str() + matches[6].str();
    size_        = std::stoul(head.at(http::field::content_length).to_string());

    const auto total_size = get_content_length(head);
    const auto remainder  = total_size % sub_download_count_;
    const auto each_size  = (total_size - remainder) / sub_download_count_;

    for (std::size_t i = 0; i != sub_download_count_; ++i)
    {
        using http::field;
        http::request<http::string_body> req;
        req.set(field::host, host_);
        req.version(11);
        req.method(http::verb::get);
        req.target(target_path_);
        req.set(field::user_agent, "Argon");

        if (i == 0)
        {
            req.set(http::field::range, form_range_header(each_size * i, each_size * (i + 1)));
        }
        else if (i == (sub_download_count_ - 1))
        {
            req.set(http::field::range, form_range_header(each_size * i, each_size * (i + 1) + remainder));
        }
        else
        {
            req.set(http::field::range, form_range_header(each_size * i + 1, each_size * (i + 1)));
        }

        auto ptr  = make_shared<network::sub_download>(i,
                                                      *this,
                                                      scheduler_.get_instance().get_an_io_context(),
                                                      host_,
                                                      target_path_,
                                                      target_,
                                                      link_,
                                                      each_size,
                                                      protocol_,
                                                      std::move(req));
        auto more = ptr->shared_from_this();
        more->start();
    }
}
