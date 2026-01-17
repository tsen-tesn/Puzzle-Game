#include <iostream>
#include <string>
#include "solve_api.h"

#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

// 把 SolveResult 轉成 JSON（你現在已經是 DTO：pieceId + cells[x,y]）
static json to_json(const SolveResult& r) {
    json j;
    j["solved"] = r.solved;
    j["error"] = r.error_message;

    json placements = json::array();
    for (const auto& p : r.placements) {
        json pj;
        pj["pieceId"] = p.pieceId;

        // 這個可留作 debug；若你不想給前端就刪掉
        pj["variantIndex"] = p.variantIndex;

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

    // 健康檢查
    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("ok", "text/plain; charset=utf-8");
    });

    // Solve endpoint
    svr.Post("/solve", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);

            SolveRequest sr;
            sr.width = body.value("width", 0);
            sr.height = body.value("height", 0);

            // pieceIds 可選：若沒提供就用預設（你 solve_api 內 req.piece_ids empty 會用全部）
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

    // 讓 React 開發時可以跨域呼叫（CORS）
    // 最小版：所有路徑都允許
    svr.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.status = 204;
    });

    // 對所有回應加 CORS header（很方便）
    svr.set_post_routing_handler([](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    });

    const int port = 8080;
    std::cout << "Server running on http://localhost:" << port << "\n";
    std::cout << "GET  /health\n";
    std::cout << "POST /solve\n";

    svr.listen("0.0.0.0", port);
    return 0;
}