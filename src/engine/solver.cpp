#include "solver.h"
#include <algorithm>  // std::fill

Solver::Solver(Board& b, const std::vector<Piece>& p)
    : board(b), pieces(p), piece_used(p.size(), 0) {}


void Solver::reset() {
    placements_path.clear();
    std::fill(piece_used.begin(), piece_used.end(), 0);
}


bool Solver::solve() {
    // 重要：每次 solve 都要重置狀態，避免上次結果污染
    placements_path.clear();
    std::fill(piece_used.begin(), piece_used.end(), 0);

    return dfs();
}

bool Solver::dfs() {
    // 1) 找到棋盤上第一個空格（目標格）
    Cell empty_cell{-1, -1};
    for (int y = 0; y < board.get_height(); ++y) {
        for (int x = 0; x < board.get_width(); ++x) {
            Cell p{x, y};
            if (board.is_empty(p)) {
                empty_cell = p;
                break;
            }
        }
        if (empty_cell.x != -1) break;
    }

    // 如果找不到空格，代表棋盤已填滿 → 成功
    if (empty_cell.x == -1) {
        return true;
    }

    // 2) 嘗試用每一個還沒使用的 piece 去覆蓋 empty_cell
    for (size_t i = 0; i < pieces.size(); ++i) {
        if (piece_used[i]) continue;

        const Piece& piece = pieces[i];
        const auto& variants = piece.get_variants();

        // 3) 對每一個 variant
        for (size_t v = 0; v < variants.size(); ++v) {
            const auto& variant = variants[v];

            // 4) 關鍵：讓 variant 的「任一個 cell」對齊 empty_cell
            // offset = empty_cell - variant_cell
            for (const auto& vc : variant) {
                Cell offset{empty_cell.x - vc.x, empty_cell.y - vc.y};

                if (!board.can_place(variant, offset))
                    continue;

                // 放上去
                board.place(piece.get_id(), variant, offset);
                piece_used[i] = 1;
                placements_path.emplace_back(piece.get_id(), (int)v, offset);

                // 遞迴
                if (dfs()) {
                    return true;
                }

                // 回溯
                placements_path.pop_back();
                piece_used[i] = 0;
                board.remove(piece.get_id(), variant, offset);
            }
        }
    }

    // 沒有任何放法能解到最後 → 失敗
    return false;
}

const std::vector<Placement>& Solver::get_placements_path() const {
    return placements_path;
}

