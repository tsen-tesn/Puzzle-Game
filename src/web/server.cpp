#include <iostream>
#include <string>
#include <cstdlib>      // getenv
#include <filesystem>

#include "../engine/piece_library.h"
#include "../game/level_data.h"
#include "../game/level_loader.h"
#include "solve_api.h"

#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;


std::vector<LevelInfo> g_levels;

void load_all_levels() {
    g_levels.clear();

    std::string dir = "levels";
    if (const char* p = std::getenv("LEVEL_DIR")) {
        dir = p;
    }

    std::error_code ec;
    if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec)) {
        std::cerr << "[ERROR] levels dir not found: " << dir << "\n";
        return; // 不要 throw 讓 Render 掛掉
    }

    for (const auto& entry : fs::directory_iterator(dir, ec)) {
        if (ec) {
            std::cerr << "[ERROR] directory_iterator failed: " << ec.message() << "\n";
            return;
        }
        if (entry.path().extension() != ".txt") continue;

        LevelData ld = LevelLoader::load_level(entry.path().string());

        LevelInfo info;
        info.id = entry.path().stem().string();
        info.name = info.id;
        info.width = ld.width;
        info.height = ld.height;

        for (const auto& piece : ld.pieces) {
            info.pieceIds.push_back(piece.get_id());
        }

        std::sort(info.pieceIds.begin(), info.pieceIds.end());
        info.pieceIds.erase(std::unique(info.pieceIds.begin(), info.pieceIds.end()), info.pieceIds.end());

        g_levels.push_back(std::move(info));
    }

    std::cerr << "[INFO] loaded levels: " << g_levels.size() << " from " << dir << "\n";
}

static int get_port() {
    if (const char* p = std::getenv("PORT")) {
        try {
            return std::stoi(p);
        } catch (...) {
            // fall through
        }
    }
    return 8080; // 預設
}

static void add_cors(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

static json to_json(const SolveResult& r) {
    json j;
    j["solved"] = r.solved;

    j["error"] = r.error_message;

    json placements = json::array();
    for (const auto& p : r.placements) {
        json pj;
        pj["pieceId"] = p.pieceId;
        pj["variantIndex"] = p.variantIndex; // debug 用

        json cells = json::array();
        for (const auto& c : p.cells) {
            cells.push_back({{"x", c.x}, {"y", c.y}});
        }
        pj["cells"] = std::move(cells);

        placements.push_back(std::move(pj));
    }
    j["placements"] = std::move(placements);
    return j;
}

int main() {
    httplib::Server svr;

    load_all_levels();

    svr.set_error_handler([](const httplib::Request& req, httplib::Response& res) {
        add_cors(res);

        nlohmann::json j;
        j["solved"] = false;
        j["error"] = "HTTP error " + std::to_string(res.status) + " at " + req.path;
        j["placements"] = nlohmann::json::array();

        res.set_content(j.dump(), "application/json; charset=utf-8");
    });



    svr.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res) {
        add_cors(res);
        res.status = 204;
    });


    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        add_cors(res);
        res.set_content("puzzle-backend alive", "text/plain; charset=utf-8");
        res.status = 200;
    });

    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        add_cors(res);
        res.set_content("ok", "text/plain; charset=utf-8");
        res.status = 200;
    });

    svr.Get("/pieces", [](const httplib::Request&, httplib::Response& res) {
        add_cors(res);

        json out;
        out["pieces"] = json::array();

        auto pieces = PieceLibrary::make_all_pieces();  

        for (const auto& p : pieces) {
            json pj;
            pj["pieceId"] = p.get_id();

            json cells = json::array();
            for (const auto& c : p.get_shape()) {
                cells.push_back({{"x", c.x}, {"y", c.y}});
            }
            pj["cells"] = std::move(cells);

            out["pieces"].push_back(std::move(pj));
        }

        res.set_content(out.dump(2), "application/json; charset=utf-8");
        res.status = 200;
    });

    svr.Get("/levels", [](const httplib::Request&, httplib::Response& res) {
        add_cors(res);

        json out;
        out["levels"] = json::array();

        for (const auto& l : g_levels) {
            out["levels"].push_back({
                {"id", l.id},
                {"name", l.name},
                {"width", l.width},
                {"height", l.height},
                {"pieceIds", l.pieceIds}
            });
        }
        

        res.set_content(out.dump(2), "application/json; charset=utf-8");
        res.status = 200;   
    });


    svr.Post("/solve", [](const httplib::Request& req, httplib::Response& res) {
        add_cors(res);

        try {
            json body = json::parse(req.body);

            SolveRequest sr;
            sr.width  = body.value("width", 0);
            sr.height = body.value("height", 0);

            if (body.contains("pieceIds") && body["pieceIds"].is_array()) {
                for (const auto& v : body["pieceIds"]) {
                    sr.piece_ids.push_back(v.get<int>());
                }
            }

            SolveResult result = solve_puzzle(sr);

            json out = to_json(result);
            res.set_content(out.dump(2), "application/json; charset=utf-8");
            res.status = 200;

        } catch (const std::exception& e) {
            json err;
            err["solved"] = false;
            err["error"] = std::string("Bad request: ") + e.what();
            err["placements"] = json::array();

            res.set_content(err.dump(2), "application/json; charset=utf-8");
            res.status = 400;
        }
    });

    const int port = get_port();
    std::cout << "Server listening on port " << port << "\n";
    std::cout << "GET  /\n";
    std::cout << "GET  /health\n";
    std::cout << "POST /solve\n";

    svr.listen("0.0.0.0", port);
    return 0;
}
