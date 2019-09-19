#ifndef ARGON_DOWNLOAD_FILE_BODY_HPP
#define ARGON_DOWNLOAD_FILE_BODY_HPP

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/beast.hpp>

namespace Argon
{
class download_file_body
{
public:
    class value_type;

    class reader;
};

static_assert(boost::beast::http::is_body<download_file_body>::value, "");

class download_file_body::value_type
{
    friend class reader;
    friend class writer;
    friend class download_file_body;

public:
    [[nodiscard]] bool is_open() const;
    void               close();
    void               open();

public:
    std::uint64_t      file_size = 0;
    boost::beast::file file_;
};

class download_file_body::reader
{
public:
    template <bool is_request, typename fields>
    reader(boost::beast::http::header<is_request, fields>& header, value_type& body);

    void init(const boost::optional<std::uint64_t>& n, boost::beast::error_code& ec);

    template <typename const_buffer_sequence>
    std::size_t put(const const_buffer_sequence& buffer_sequence, boost::beast::error_code& ec);

    void finish(boost::beast::error_code& ec);

private:
    download_file_body::value_type& body_;
};

template <bool is_request, typename fields>
download_file_body::reader::reader(boost::beast::http::header<is_request, fields>& header, value_type& body)
    : body_{ body }
{
    boost::ignore_unused(header);
}

/**
 * @brief  read from a const_buffer and IGNORE header and write body to the file
 * @tparam const_buffer_sequence a const buffer sequence comforming to the boost asio ConstBufferSequence concept
 * @param buffer_sequence the incoming buffer
 * @param ec the error code to indicate error
 * @return the total bytes written in this function call
 *
 * This function could contain error concerning the size of char* on certain implementations, this should be address by
 * a later date
 * Bill Wang 2019/08/05 (yyyy/mm/dd)
 *
 */
template <typename const_buffer_sequence>
std::size_t download_file_body::reader::put(const const_buffer_sequence& buffer_sequence, boost::beast::error_code& ec)
{
    // This function must return the total number of
    // bytes transferred from the input buffers.
    std::size_t nwritten = 0;

    // Loop over all the buffers in the sequence,
    // and write each one to the file, ignoring headers
    for (auto it = boost::asio::buffer_sequence_begin(buffer_sequence);
         it != boost::asio::buffer_sequence_end(buffer_sequence);
         ++it)
    {
        // Write this buffer_sequence to the file
        boost::asio::const_buffer buffer = *it;

        // buffer.data() returns a const void*, we intentially cast it to char to see if header is present and needed to
        // be ignored
        const char* char_ptr = static_cast<const char*>(buffer.data());
        std::size_t i        = 0; // index used to access the buffer
        while (i != buffer.size())
        {
            if (!(char_ptr[i] == '\r'))
            {
                // this is not the end of the header, just continue
                ++i;
                continue;
            }
            else if (char_ptr[i] == '\r' && char_ptr[i + 1] == '\n' && char_ptr[i + 2] == '\r' &&
                     char_ptr[i + 3] == '\n')
            {
                const void* start = char_ptr + 4;
                // bingo! this is the end of the header
                // todo: proceed to write the file
                nwritten += body_.file_.write(start, buffer.size() - (i + 1), ec);
                break;
            }
            else
            {
                // no header is present in this buffer, ok, we just write the entire buffer
                nwritten += body_.file_.write(buffer.data(), buffer.size(), ec);
                break;
            }
        }
    }
    // Indicate success
    // This is required by the error_code specification
    ec = {};

    return nwritten;
}

} // namespace Argon
#endif // ARGON_DOWNLOAD_FILE_BODY_HPP
