#include <iostream>
#include <string>
#include <cstdlib>      // getenv
#include <filesystem>
#include <system_error>

#include "../engine/piece_library.h"
#include "../game/level_data.h"
#include "../game/level_loader.h"
#include "solve_api.h"

#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;


struct LevelGroup {
    std::string id;      // group folder name 
    std::string name;    // 先同 id
    std::vector<LevelInfo> levels;
};

std::vector<LevelGroup> g_groups;

void load_all_levels() {
    g_groups.clear();

    std::cerr << "[LEVEL] load_all_levels() enter\n";

    std::error_code ec; 
    std::string root = "levels";
    if (const char* p = std::getenv("LEVEL_DIR")) root = p;

    std::cerr << "[LEVEL] cwd=" << fs::current_path(ec).string() << "\n";
    std::cerr << "[LEVEL] root=" << root << "\n";
    std::cerr << "[LEVEL] exists=" << (fs::exists(root, ec) ? "yes" : "no") << "\n";
    std::cerr << "[LEVEL] is_dir=" << (fs::is_directory(root, ec) ? "yes" : "no") << "\n";
    for (const auto& groupEntry : fs::directory_iterator(root, ec)) {
        if (ec) {
            std::cerr << "[LEVEL] iterator error: " << ec.message() << "\n";
            return;
        }

        if (!groupEntry.is_directory()) continue;

        const auto groupPath = groupEntry.path();
        const auto groupName = groupPath.filename().string();
        std::cerr << "[GROUP] " << groupName << "\n";
        LevelGroup group;
        group.id = groupName;
        group.name = groupName;

        for (const auto& fileEntry : fs::directory_iterator(groupPath, ec)) {
            if (ec) {
                std::cerr << "[LEVEL] iterator error: " << ec.message() << "\n";
                return;
            }
            if (!fileEntry.is_regular_file()) continue;
            if (fileEntry.path().extension() != ".txt") continue;

            // std::cerr << "  - " << fileEntry.path().filename().string() << "\n";
            try {
                LevelData ld = LevelLoader::load_level(fileEntry.path().string());
                std::cerr << "  - " << fileEntry.path().filename().string()
                        << " (" << ld.width << "x" << ld.height
                        << ", pieces=" << ld.pieces.size() << ")\n";
                LevelInfo li;
                li.id = fileEntry.path().stem().string();      // level1
                li.name = li.id;
                li.width = ld.width;
                li.height = ld.height;

                // pieceIds：從 ld.pieces 抽 id，並去重 + 排序
                std::vector<int> ids;
                ids.reserve(ld.pieces.size());
                for (const auto& pc : ld.pieces) {
                    ids.push_back(pc.get_id());
                }
                std::sort(ids.begin(), ids.end());
                ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
                li.pieceIds = std::move(ids);

                group.levels.push_back(std::move(li));
            } catch (const std::exception& e) {
                std::cerr << "  - " << fileEntry.path().filename().string()
                        << " [LOAD FAIL] " << e.what() << "\n";
            }

            
        }
        g_groups.emplace_back(std::move(group));
    }

    std::sort(g_groups.begin(), g_groups.end(),
            [](const LevelGroup& a, const LevelGroup& b) {
                return a.name < b.name;
            });

    std::cerr << "[INFO] loaded groups=" << g_groups.size() << "\n";
    for (const auto& g : g_groups) {
        std::cerr << "  [G] " << g.name << " levels=" << g.levels.size() << "\n";
    }
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

    std::cerr << "[BOOT] before load_all_levels\n";
    load_all_levels();
    std::cerr << "[BOOT] after load_all_levels\n";

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

    svr.Get("/groups", [](const httplib::Request&, httplib::Response& res) {
        add_cors(res);

        json out;
        out["groups"] = json::array();

        for (const auto& g : g_groups) {
            json gj;
            gj["groupId"] = g.id;
            gj["name"] = g.name;

            json levels = json::array();
            for (const auto& lv : g.levels) {
                levels.push_back({
                    {"id", lv.id},
                    {"name", lv.name},
                    {"width", lv.width},
                    {"height", lv.height},
                    {"pieceIds", lv.pieceIds}
                });
            }
            gj["levels"] = std::move(levels);

            out["groups"].push_back(std::move(gj));
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
