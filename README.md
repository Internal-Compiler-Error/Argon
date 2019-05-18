# Argon
Argon is a high performance downloader written in modern C++

## Design Goals
Downloading files from the internet takes time, sometimes unnecessarily long. All modern systems have multiples threads yet by default a browser would only download a file in one single thread instead of levering the power of modern cpus.

Argon splits a file into multiple segments and download each segments in parallel and joins them together. This can improve download performance significantly, especially for HTTP(S) connections.

## Cross Platform Support
Argon uses these following libraries, some have excellent cross platform support, some less so.
- boost asio
- boost beast
- [nlohmann json](https://github.com/nlohmann/json)
- gtkmm 
- gtest (not needed for release build)
 