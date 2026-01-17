# FUNCTION INTRODUCTION

## 專案結構

```text
src/
├── engine/             # 單純演算法
|   ├── cell.h
|   ├── piece.h
|   ├── piece.cpp
|   ├── board.h
|   ├── board.cpp
|   ├── placement.h
|   ├── solver.h
|   └── solver.cpp       
├── game/               # 遊戲層 (關卡、流程、載入)
│   ├── game.h
│   ├── game.cpp
|   ├── level_data.h
|   ├── level_loader.h
|   ├── level_loader.cpp
|   ├── piece_library.h
│   └── piece_library.cpp
└── web/                # 對外 API
```