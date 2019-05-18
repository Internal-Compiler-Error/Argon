#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include "../src/download/download.hpp"

TEST(download_json_conversion, from_json_to_download)
{
    using json = nlohmann::json;
    using download = Argon::data_structure::download;

    json const json_ =
            json{
                    { "index", 0 },
                    { "link", "www.google.ca" },
                    { "protocol", "http" },
                    { "target", "index.html" },
                    { "progress", 0 }
            };

    download download_;
    download_.set_index(0);
    download_.set_link("www.google.ca");
    download_.set_protocol(Argon::data_structure::conn_type::HTTP);
    download_.set_target("index.html");
    download_.set_progress(0);

    auto const parse = json_.get<Argon::data_structure::download>();

    EXPECT_EQ(parse, download_);

}

TEST(download_json_conversion, from_download_to_json)
{
    using Argon::data_structure::download;
    using nlohmann::json;
    download download_;
    download_.set_index(0);
    download_.set_link("www.google.ca");
    download_.set_protocol(Argon::data_structure::conn_type::HTTP);
    download_.set_target("index.html");
    download_.set_progress(0);

    json const guaranteed_correct =
            json{
                    {
                            { "index", 0 },
                            { "link", "www.google.ca" },
                            { "protocol", "http" },
                            { "target", "index.html" },
                            { "progress", 0 }

                    }
            };

    json json_{ download_ };

    EXPECT_EQ(guaranteed_correct, json_);

}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}