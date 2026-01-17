#include "solve_api.h"
#include "../game/piece_library.h"
#include "../engine/board.h"
#include "../engine/solver.h"

#include <string>
#include <vector>

static int count_cells(const std::vector<Piece>& pieces) {
    int sum = 0;
    for(const auto& piece: pieces) {
        sum += (int)(piece.get_shape().size());
    }

    return sum;
}

static const Piece* find_piece_by_id(const std::vector<Piece>& pieces, int id) {
    for(const auto& piece: pieces) {
        if(piece.get_id() == id) {
            return &piece;
        }
    }
    return nullptr;
}

SolveResult solve_puzzle(const SolveRequest& req) {
    SolveResult out;

    // basic check
    if(req.width <= 0 || req.height <= 0) {
        out.solved = false; 
        out.error_message = "Invalid board size.";
        return out;
    }

    // Load pieces (get pieces fomr library)
    std::vector<Piece> pieces;
    try {
        if (req.piece_ids.empty()) {
            pieces = PieceLibrary::make_all_pieces();
        } else {
            pieces = PieceLibrary::get_piece_by_id(req.piece_ids);
        }
    } catch (const std::exception& e) {
        out.solved = false;
        out.error_message = e.what();
        return out;
    }

    // Area check 【 如果 board 面積不等於 pieces 加總的面積就無解 】
    const int board_area = req.width * req.height;
    const int pieces_area = count_cells(pieces);

    if(board_area != pieces_area) {
        out.solved = false;
        out.error_message = "Area mismatch: board = " + std::to_string(board_area) +
                            " pieces = " + std::to_string(pieces_area);
        return out;
    }

    // Solve
    Board board(req.width, req.height);
    Solver solver(board, pieces);
    
    out.solved = solver.solve();

    if(!out.solved) {
        return out;
    } 
    
    // convert placements to DTO

    const auto& path = solver.get_placements_path();
    out.placements.reserve(path.size());

    for(const auto& placement : path) {
        PlacementDTO dto;
        dto.pieceId = placement.get_piece_id();
        dto.variantIndex = placement.get_variant_index(); // debug 用

        const Piece* piece = find_piece_by_id(pieces, dto.pieceId);
        if(!piece) {
            out.solved = false;
            out.error_message = "Internal error: PieceID not found." + std::to_string(dto.pieceId);
            out.placements.clear();
            return out;;
        }

        const auto& variants = piece->get_variants();
        if (dto.variantIndex < 0 || dto.variantIndex >= (int)variants.size()) {
            out.solved = false;
            out.error_message = "Internal error: variantIndex out of range.";
            out.placements.clear();
            return out;
        }

        const auto& variant = variants[dto.variantIndex];
        const Cell& off = placement.get_offset();

        dto.cells.reserve(variant.size());
        for (const auto& c : variant) {
            CellDTO cc;
            cc.x = c.x + off.x;
            cc.y = c.y + off.y;
            dto.cells.push_back(cc);
        }

        out.placements.emplace_back(std::move(dto));
    }

    return out;
}

