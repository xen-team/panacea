#pragma once
#include <cpr/cpr.h>
#include <simdjson.h>
#include <vector>

#include "picosha2.h"

namespace panacea {
class Leaderboard {
public:
    struct LeaderboardEntry {
        std::string name;
        int score;
    };

public:
    void submit(std::string const& player_name, int score)
    {
        std::string data_to_hash = player_name + std::to_string(score) + SECRET_KEY;

        std::string hash_hex_str;
        picosha2::hash256_hex_string(data_to_hash, hash_hex_str);

        xen::Log::info("Submitting score for ", player_name, " with hash: ", hash_hex_str);

        cpr::Response r = cpr::Post(
            cpr::Url{"http://" + SERVER_IP + "/add_score.php"},
            cpr::Payload{{"playerName", player_name}, {"score", std::to_string(score)}, {"hash", hash_hex_str}}
        );

        if (r.status_code == 200) {
            xen::Log::info("Success: ", r.text);
        }
        else {
            xen::Log::error("Error submitting score. Status: ", r.status_code);
            xen::Log::error("Response: ", r.text);
        }
    }

    void fetch_leaderboard()
    {
        std::vector<LeaderboardEntry> new_leaderboard;
        simdjson::ondemand::parser parser;

        cpr::Header api_header = {{"X-Api-Key", SECRET_KEY}};

        cpr::Response r = cpr::Get(cpr::Url{"http://" + SERVER_IP + "/get_leaderboard.php"}, api_header);

        if (r.status_code != 200) {
            xen::Log::error("Error fetching leaderboard. Status: ", r.status_code);
            xen::Log::error("Response: ", r.text);
            return;
        }

        simdjson::padded_string json_string(r.text);
        try {
            simdjson::ondemand::document doc = parser.iterate(json_string);
            for (auto element : doc.get_array()) {
                simdjson::ondemand::object obj = element.get_object();
                new_leaderboard.emplace_back(
                    std::string(obj["player_name"].get_string().value()), static_cast<int>(obj["score"].get_int64())
                );
            }
        }
        catch (simdjson::simdjson_error const& e) {
            xen::Log::error("JSON parsing error: ", e.what());
        }

        leaderboard = std::move(new_leaderboard);
    }

    [[nodiscard]] auto const& get_leaderboard() const { return leaderboard; }

private:
    std::vector<LeaderboardEntry> leaderboard;
    std::string const SERVER_IP = "185.207.133.40";
    std::string const SECRET_KEY = "SFEC";
};
}