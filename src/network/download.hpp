#ifndef ARGON_DOWNLOAD_HPP
#define ARGON_DOWNLOAD_HPP

#include "download_file_body.hpp"

#include <boost/asio/ssl/context_base.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

namespace
{
void load_root_certificate(boost::asio::ssl::context& ctx, boost::system::error_code& ec)
{
    std::ifstream     root_certificate{ "root_certificate.pem", std::ios::in };
    std::stringstream ss;
    ss << root_certificate.rdbuf();
    std::string cert2;
    ss >> cert2;

    ctx.add_certificate_authority(boost::asio::buffer(cert2.data(), cert2.size()), ec);
    if (ec)
    {
        return;
    }
}
} // namespace

namespace Argon::network
{
enum class conn_type
{
    ftp,
    http,
    https,
    BitTorrent,
};

class download_scheduler;
class download
{
public:
    // friend functions for json parser
    friend void to_json(nlohmann::json& j, const download& d);
    friend void from_json(const nlohmann::json& j, download& d);

    /**
     * @brief
     * Constructs a download from a uri string. The string should be an original request that may require redirects
     * to reach the final location.
     *
     * @param str the uri string
     */
    explicit download(download_scheduler&      scheduler,
                      std::string_view         uri,
                      std::size_t              sub_download_count,
                      boost::asio::io_context& ioc) noexcept(false);

    void start();

    void append_all_sub_downloads();

    /**
     * @brief
     * this function is called by sub_download to report complete once the sub_download is done
     */
    void report_completed();

    [[nodiscard]] inline bool is_https() const noexcept { return protocol_ == conn_type::https; }

    [[nodiscard]] unsigned int get_index() const;

    void set_index(unsigned int index);

    [[nodiscard]] const std::string& get_link() const;

    [[nodiscard]] conn_type get_protocol() const;

    [[nodiscard]] const std::string& get_target() const;

    [[nodiscard]] double get_progress() const;

    void set_progress(double progress) noexcept;

    [[nodiscard]] const std::string& get_original_request_url() const;

    void set_original_request_url_(const std::string& raw_uri_);

    [[nodiscard]] const std::string& get_target_path() const;

    void set_target_path(const std::string& target_path);

    [[nodiscard]] const std::string& get_host() const;

    void set_host(const std::string& host);

    void set_original_request_url(const std::string& original_request_url);

    [[nodiscard]] size_t get_size() const;

    void set_size(size_t size);

    [[nodiscard]] size_t get_downloaded() const;

    void set_downloaded(size_t downloaded);

    [[nodiscard]] boost::asio::io_context& get_ioc() const;

private:
    unsigned int                 index_{};
    std::string                  original_request_url_;
    std::string                  link_;
    std::string                  host_;
    std::string                  target_path_;
    conn_type                    protocol_{};
    std::string                  target_;
    std::size_t                  size_{};
    std::size_t                  downloaded_{};
    double                       progress_{};
    boost::asio::io_context&     ioc_;
    network::download_scheduler& scheduler_;
    size_t                       sub_download_count_;
    size_t                       completed_sub_download_count_ = 0;

    // std::mutex mutex_;
};

// functions needed to serialize and deserialize jsons
void to_json(nlohmann::json& j, const Argon::network::download& d);
void from_json(const nlohmann::json& j, Argon::network::download& d);

NLOHMANN_JSON_SERIALIZE_ENUM(conn_type,
                             { { conn_type::ftp, "ftp" },
                               { conn_type::http, "http" },
                               { conn_type::https, "https" },
                               { conn_type::BitTorrent, "BitTorrent" } })

} // namespace Argon::network
#endif // ARGON_DOWNLOAD_HPP
