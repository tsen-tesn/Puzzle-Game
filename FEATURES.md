# Problem Statement

This project is a polyomino puzzle solver.  
Given a fixed 5×8 board and 8 irregular polyomino pieces, the program finds ways to place all pieces into the board such that the board is completely filled without overlap.

The solver is implemented in C++ using an object-oriented architecture and supports extensible solving strategies.

---

# Features and Tasks

## F1: Project Architecture

- [x] F1: Core architecture setup
  - [x] T1.1: Define class responsibilities (Game / Board / Piece / Placement / Solver)
  - [x] T1.2: Create base folder structure (`/src`)
  - [x] T1.3: Create basic header/source files
  - [x] T1.4: Implement main entry (`Game`)

---

## F2: Piece System

- [x] F2: Polyomino piece representation
  - [x] T2.1: Define relative coordinate representation
  - [x] T2.2: Store original piece shapes
  - [x] T2.3: Generate rotated variants (90°, 180°, 270°)
  - [x] T2.4: Generate flipped variants
  - [x] T2.5: Remove duplicated variants
  - [x] T2.6: Assign unique piece ID
  - [x] T2.7: Validate piece cell connectivity

---

## F3: Board System

- [x] F3: Board representation and validation
  - [x] T3.1: Represent board grid (5×8)
  - [x] T3.2: Implement placement validation
  - [x] T3.3: Implement piece placement
  - [x] T3.4: Implement piece removal (backtracking support)
  - [x] T3.5: Board full-check function

---

## F4: Placement Enumeration

- [x] F4: All possible placement generation
  - [x] T4.1: Generate all placements for each piece
  - [x] T4.2: Store placements in a placement pool
  - [x] T4.3: Validate placements inside board boundaries

---

## F5: Solver Engine

- [ ] F5: Search and solving
  - [ ] T5.1: Implement recursive backtracking solver
  - [ ] T5.2: Track used pieces
  - [ ] T5.3: Conflict detection
  - [ ] T5.4: Stop after first solution
  - [ ] T5.5: Count all solutions (optional)

---

## F6: Output System

- [ ] F6: Output and visualization
  - [ ] T6.1: Print solved board to console
  - [ ] T6.2: Output solution to file
  - [ ] T6.3: Display piece IDs on board

---

## F7: Performance Optimization (Future)

- [ ] F7: Optimization strategies
  - [ ] T7.1: Placement ordering heuristic
  - [ ] T7.2: Branch pruning
  - [ ] T7.3: Dancing Links (DLX) solver

---

## F8: Extensibility

- [ ] F8: Extensible system design
  - [ ] T8.1: Support different board sizes
  - [ ] T8.2: Support custom piece sets
  - [ ] T8.3: Load puzzle definition from file (JSON / TXT)

---

## F9: UI & Visualization (Future)

- [ ] F9: User interface
  - [ ] T9.1: Step-by-step solving visualization
  - [ ] T9.2: GUI interface (SFML / Qt)
  - [ ] T9.3: WebAssembly version

---

> This project aims to build a flexible polyomino puzzle solving engine suitable for algorithm research and visualization.
