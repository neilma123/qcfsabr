#include "yahoo.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace qe {
namespace {

long long epoch(const std::string& date) {
    std::tm tm{};
    char dash1 = 0, dash2 = 0;
    std::istringstream in(date);
    in >> tm.tm_year >> dash1 >> tm.tm_mon >> dash2 >> tm.tm_mday;
    if (!in || dash1 != '-' || dash2 != '-') throw std::runtime_error("Date must be YYYY-MM-DD");
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
#ifdef _WIN32
    return static_cast<long long>(_mkgmtime(&tm));
#else
    return static_cast<long long>(timegm(&tm));
#endif
}

bool safe_symbol(const std::string& symbol) {
    if (symbol.empty()) return false;
    for (char c : symbol) {
        if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '^' || c == '=' || c == '.' || c == '-')) return false;
    }
    return true;
}

std::string download(const std::string& url) {
#ifdef _WIN32
    const std::string command = "curl.exe -fsSL -A \"Mozilla/5.0\" \"" + url + "\"";
    FILE* pipe = _popen(command.c_str(), "rb");
#else
    const std::string command = "curl -fsSL \"" + url + "\"";
    FILE* pipe = popen(command.c_str(), "r");
#endif
    if (!pipe) throw std::runtime_error("Unable to start curl");
    std::string output;
    std::array<char, 8192> buffer{};
    while (const std::size_t n = std::fread(buffer.data(), 1, buffer.size(), pipe)) output.append(buffer.data(), n);
#ifdef _WIN32
    const int status = _pclose(pipe);
#else
    const int status = pclose(pipe);
#endif
    if (status != 0 || output.empty()) throw std::runtime_error("Yahoo request failed");
    return output;
}

std::vector<std::string> json_array(const std::string& json, const std::string& key, std::size_t start = 0) {
    const std::string token = "\"" + key + "\":[";
    const std::size_t begin = json.find(token, start);
    if (begin == std::string::npos) return {};
    const std::size_t content = begin + token.size();
    const std::size_t end = json.find(']', content);
    if (end == std::string::npos) return {};
    std::vector<std::string> values;
    std::istringstream input(json.substr(content, end - content));
    std::string value;
    while (std::getline(input, value, ',')) values.push_back(value);
    return values;
}

std::string iso_date(long long timestamp) {
    const std::time_t t = static_cast<std::time_t>(timestamp);
    std::tm tm{};
#ifdef _WIN32
    gmtime_s(&tm, &t);
#else
    gmtime_r(&t, &tm);
#endif
    char buffer[11]{};
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm);
    return buffer;
}

}  // namespace

int print_yahoo_history(const std::string& symbol, const std::string& start_date,
                        const std::string& end_date) {
    if (!safe_symbol(symbol)) {
        std::cerr << "Invalid Yahoo symbol.\n";
        return 2;
    }
    const auto period1 = epoch(start_date);
    const auto period2 = epoch(end_date) + 86400;
    std::ostringstream url;
    url << "https://query2.finance.yahoo.com/v8/finance/chart/" << symbol
        << "?period1=" << period1 << "&period2=" << period2
        << "&interval=1d&events=history";
    const std::string json = download(url.str());
    if (json.find("\"result\":null") != std::string::npos) {
        std::cerr << "Yahoo returned no data for " << symbol << ". The contract may be expired or unavailable.\n";
        return 3;
    }
    const auto timestamps = json_array(json, "timestamp");
    const std::size_t quote_section = json.find("\"quote\":[{");
    const auto opens = json_array(json, "open", quote_section);
    const auto highs = json_array(json, "high", quote_section);
    const auto lows = json_array(json, "low", quote_section);
    const auto closes = json_array(json, "close", quote_section);
    const std::size_t n = std::min({timestamps.size(), opens.size(), highs.size(), lows.size(), closes.size()});
    if (n == 0) {
        std::cerr << "Yahoo response did not contain price bars.\n";
        return 3;
    }
    std::cout << "date,open,high,low,close\n";
    for (std::size_t i = 0; i < n; ++i) {
        if (timestamps[i] == "null" || closes[i] == "null") continue;
        std::cout << iso_date(std::stoll(timestamps[i])) << ',' << opens[i] << ',' << highs[i]
                  << ',' << lows[i] << ',' << closes[i] << '\n';
    }
    return 0;
}

}  // namespace qe
