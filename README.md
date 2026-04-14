# 🌳 Tree Simulator

<img width="1659" height="962" alt="image" src="https://github.com/user-attachments/assets/d6d1dc71-d06a-4b03-bf96-d6477c18bb24" />

An interactive desktop application for visualizing **Binary Search Trees (BST)** and **AVL Trees**, built with **C++17** and **Qt 6**.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus)
![Qt 6](https://img.shields.io/badge/Qt-6.11-green?logo=qt)
![License](https://img.shields.io/badge/License-MIT-yellow)

---

## ✨ Features

| Feature | Description |
|---------|-------------|
| **Dual Mode** | Switch between BST and AVL tree modes |
| **Interactive Canvas** | Click `+` buttons on the canvas to insert nodes directly |
| **AVL Rotation Viz** | Watch LL, RR, LR, RL rotations with animated overlays and pivot highlighting |
| **Traversals** | In-order, Pre-order, Post-order, Level-order with step-by-step node highlighting |
| **Context Menu** | Right-click any node to delete it or insert a new value |
| **Collapsible Sidebar** | Auto-hides when you move away; reappears on hover at the left edge |
| **Balance Factor Display** | AVL mode shows BF chips above each node (green = balanced, red = imbalanced) |
| **Speed Control** | Adjust animation speed with the slider |
| **Dark Theme** | Strello-inspired deep navy UI with purple accents |

---

## 📸 UI Overview

- **Left Sidebar** — Mode selector, operations (Insert / Delete / Search / Clear), traversals, speed slider
- **Canvas** — Interactive tree visualization with animated nodes, glow effects, and `+` insertion buttons
- **Rotation Overlay** — Floating card at top-right showing rotation type and pivot when AVL balancing occurs
- **Status Bar** — Feedback on every operation

---

## 🛠️ Tech Stack

- **Language:** C++17
- **GUI Framework:** Qt 6 (Widgets, QGraphicsView)
- **Build System:** CMake + Ninja
- **Compiler:** MinGW (GCC 13.1)
- **Styling:** QSS (Qt Style Sheets)

---

## 📁 Project Structure

```
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── core/
│   │   ├── BSTNode.h            # Node struct (value, height, children)
│   │   ├── BSTree.h / .cpp      # Binary Search Tree (insert, delete, search, traversals)
│   │   └── AVLTree.h / .cpp     # AVL Tree (rotations, balancing, rotation logging)
│   └── gui/
│       ├── MainWindow.h / .cpp  # Main window with collapsible sidebar & rotation overlay
│       ├── TreeScene.h / .cpp   # QGraphicsScene managing layout, animations, plus buttons
│       ├── NodeItem.h / .cpp    # Custom node rendering (glow, gradients, context menu)
│       ├── EdgeItem.h / .cpp    # Parent→child edge lines
│       └── PlusButtonItem.h/.cpp# "+" insertion buttons at empty child slots
└── resources/
    ├── style.qss                # Strello dark theme stylesheet
    └── resources.qrc            # Qt resource bundle
```

---

## 🚀 Build & Run

### Prerequisites

- [Qt 6](https://www.qt.io/download) (Widgets module)
- CMake ≥ 3.16
- A C++17 compiler (GCC, MSVC, Clang)

### Windows (with Qt installed at `C:\Qt`)

```powershell
# Add Qt tools to PATH
$env:PATH = "C:\Qt\Tools\CMake_64\bin;C:\Qt\Tools\Ninja;C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.11.0\mingw_64\bin;$env:PATH"

# Configure
cmake -G Ninja -DCMAKE_PREFIX_PATH="C:/Qt/6.11.0/mingw_64" -DCMAKE_CXX_COMPILER="C:/Qt/Tools/mingw1310_64/bin/g++.exe" -B build .

# Build
cmake --build build

# Run
.\build\TreeSimulator.exe
```

### Linux / macOS

```bash
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64
cmake --build build
./build/TreeSimulator
```

---

## 🎮 Usage

### Sidebar Controls
1. **Mode** — Select "Binary Search Tree" or "AVL Tree"
2. **Value** — Enter an integer, then click Insert / Delete / Search
3. **Traversals** — Click any traversal button to animate the walk order
4. **Speed** — Drag the slider to control animation speed

### Canvas Interactions
- **Click `+`** at any empty child position to insert a new node
- **Right-click** a node → Delete or Insert
- **Drag** the canvas to pan
- **Hover** over the left edge to re-open a collapsed sidebar

### AVL Rotations
Switch to AVL mode and insert values that trigger imbalance. A purple overlay card will appear showing:
- Rotation type (e.g., `↻ Right Rotation (LL case)`)
- The pivot node that was rebalanced
- Pivot nodes glow gold on the canvas

---

## 📄 License

This project is open source under the [MIT License](LICENSE).
