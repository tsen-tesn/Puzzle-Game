#include <iostream>
#include <string>
#include <cstdlib>      // getenv
#include "solve_api.h"

#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

static int get_port() {
    if (const char* p = std::getenv("PORT")) {
        try {
            return std::stoi(p);
        } catch (...) {
            // fall through
        }
    }
    return 8080; // 本機預設
}

// 統一加 CORS（最穩：每個 response 都加一次）
static void add_cors(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    // 如果你之後要帶 cookie / auth，這裡就不能用 "*"，要改成指定網域並加 Allow-Credentials
}

// 把 SolveResult 轉成 JSON
static json to_json(const SolveResult& r) {
    json j;
    j["solved"] = r.solved;

    // 你這裡欄位名是 error_message（不是 error）
    j["error"] = r.error_message;

    json placements = json::array();
    for (const auto& p : r.placements) {
        json pj;
        pj["pieceId"] = p.pieceId;
        pj["variantIndex"] = p.variantIndex; // debug 可留

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

    svr.set_error_handler([](const httplib::Request&, httplib::Response& res) {
        add_cors(res);
        if (res.body.empty()) {
            res.set_content("error", "text/plain; charset=utf-8");
        }
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
