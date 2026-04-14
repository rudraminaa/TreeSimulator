#include "MainWindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QStatusBar>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QRandomGenerator>
#include <QInputDialog>
#include <QScrollArea>
#include <QMouseEvent>
#include <QCursor>
#include <QApplication>
#include <sstream>

// ══════════════════════════════════════════════════════════════
//  Construction
// ══════════════════════════════════════════════════════════════

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    bst_ = new BSTree();
    avl_ = new AVLTree();
    scene_ = new TreeScene(this);

    setupUI();
    refreshScene();

    setWindowTitle("Tree Simulator");
    resize(1340, 800);
    setMinimumSize(800, 560);
}

MainWindow::~MainWindow() {
    delete bst_;
    delete avl_;
}

// ══════════════════════════════════════════════════════════════
//  Helpers
// ══════════════════════════════════════════════════════════════

QPushButton* MainWindow::makeButton(const QString& text, const QString& objectName) {
    auto* btn = new QPushButton(text);
    btn->setObjectName(objectName);
    btn->setCursor(QCursor(Qt::PointingHandCursor));
    return btn;
}

static QFrame* makeDivider() {
    auto* f = new QFrame();
    f->setObjectName("divider");
    f->setFrameShape(QFrame::HLine);
    f->setFixedHeight(1);
    return f;
}

// ══════════════════════════════════════════════════════════════
//  UI Setup
// ══════════════════════════════════════════════════════════════

void MainWindow::setupUI() {
    auto* central = new QWidget(this);
    auto* mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ── Sidebar ──────────────────────────────────────────────
    sidebar_ = new QWidget();
    sidebar_->setObjectName("sidebar");
    sidebar_->setFixedWidth(kSidebarWidth);

    auto* sideLayout = new QVBoxLayout(sidebar_);
    sideLayout->setContentsMargins(18, 22, 18, 18);
    sideLayout->setSpacing(5);

    // Title
    auto* titleLabel = new QLabel("Tree Simulator");
    titleLabel->setObjectName("titleLabel");
    sideLayout->addWidget(titleLabel);

    // Subtitle
    auto* subLabel = new QLabel("BST & AVL Visualization");
    subLabel->setObjectName("subtitleLabel");
    sideLayout->addWidget(subLabel);

    sideLayout->addSpacing(6);
    sideLayout->addWidget(makeDivider());
    sideLayout->addSpacing(2);

    // ── Mode ─────────────────────────────────────────────────
    auto* modeGroup = new QGroupBox("MODE");
    auto* modeLayout = new QVBoxLayout(modeGroup);
    modeLayout->setSpacing(6);
    modeCombo_ = new QComboBox();
    modeCombo_->addItems({"Binary Search Tree", "AVL Tree"});
    modeCombo_->setObjectName("modeCombo");
    connect(modeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onModeChanged);
    modeLayout->addWidget(modeCombo_);
    sideLayout->addWidget(modeGroup);

    // ── Operations ───────────────────────────────────────────
    auto* opsGroup = new QGroupBox("OPERATIONS");
    auto* opsLayout = new QVBoxLayout(opsGroup);
    opsLayout->setSpacing(8);

    auto* inputRow = new QHBoxLayout();
    inputRow->setSpacing(8);
    auto* valLabel = new QLabel("Value");
    valLabel->setStyleSheet("font-weight: 500; color: #6B7185;");
    valueSpin_ = new QSpinBox();
    valueSpin_->setRange(-999, 999);
    valueSpin_->setValue(0);
    valueSpin_->setObjectName("valueSpin");
    inputRow->addWidget(valLabel);
    inputRow->addWidget(valueSpin_, 1);
    opsLayout->addLayout(inputRow);

    auto* r1 = new QHBoxLayout(); r1->setSpacing(6);
    auto* insertBtn = makeButton("Insert", "insertBtn");
    auto* deleteBtn = makeButton("Delete", "deleteBtn");
    r1->addWidget(insertBtn);
    r1->addWidget(deleteBtn);
    opsLayout->addLayout(r1);

    auto* r2 = new QHBoxLayout(); r2->setSpacing(6);
    auto* searchBtn = makeButton("Search", "searchBtn");
    auto* clearBtn  = makeButton("Clear",  "clearBtn");
    r2->addWidget(searchBtn);
    r2->addWidget(clearBtn);
    opsLayout->addLayout(r2);

    auto* randomBtn = makeButton("Random (5)", "randomBtn");
    opsLayout->addWidget(randomBtn);

    connect(insertBtn, &QPushButton::clicked, this, &MainWindow::onInsert);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDelete);
    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::onSearch);
    connect(clearBtn,  &QPushButton::clicked, this, &MainWindow::onClear);
    connect(randomBtn, &QPushButton::clicked, this, &MainWindow::onGenerateRandom);

    sideLayout->addWidget(opsGroup);

    // ── Traversals ───────────────────────────────────────────
    auto* travGroup = new QGroupBox("TRAVERSALS");
    auto* travLayout = new QVBoxLayout(travGroup);
    travLayout->setSpacing(6);

    auto* tr1 = new QHBoxLayout(); tr1->setSpacing(5);
    auto* inOrderBtn  = makeButton("In-Order",  "inOrderBtn");
    auto* preOrderBtn = makeButton("Pre-Order", "preOrderBtn");
    tr1->addWidget(inOrderBtn);
    tr1->addWidget(preOrderBtn);
    travLayout->addLayout(tr1);

    auto* tr2 = new QHBoxLayout(); tr2->setSpacing(5);
    auto* postOrderBtn  = makeButton("Post-Order",  "postOrderBtn");
    auto* levelOrderBtn = makeButton("Level-Order", "levelOrderBtn");
    tr2->addWidget(postOrderBtn);
    tr2->addWidget(levelOrderBtn);
    travLayout->addLayout(tr2);

    traversalOutput_ = new QLabel("—");
    traversalOutput_->setObjectName("traversalOutput");
    traversalOutput_->setWordWrap(true);
    traversalOutput_->setMinimumHeight(42);
    travLayout->addWidget(traversalOutput_);

    connect(inOrderBtn,    &QPushButton::clicked, this, [this]() { onTraversal("InOrder"); });
    connect(preOrderBtn,   &QPushButton::clicked, this, [this]() { onTraversal("PreOrder"); });
    connect(postOrderBtn,  &QPushButton::clicked, this, [this]() { onTraversal("PostOrder"); });
    connect(levelOrderBtn, &QPushButton::clicked, this, [this]() { onTraversal("LevelOrder"); });

    sideLayout->addWidget(travGroup);

    // ── Speed ────────────────────────────────────────────────
    auto* speedGroup = new QGroupBox("SPEED");
    auto* speedLayout = new QVBoxLayout(speedGroup);
    speedSlider_ = new QSlider(Qt::Horizontal);
    speedSlider_->setRange(100, 1200);
    speedSlider_->setValue(400);
    speedSlider_->setObjectName("speedSlider");
    speedLayout->addWidget(speedSlider_);
    auto* sl = new QHBoxLayout();
    auto* f = new QLabel("Fast"); f->setStyleSheet("font-size:10px;color:#6B7185;");
    auto* s = new QLabel("Slow"); s->setStyleSheet("font-size:10px;color:#6B7185;");
    sl->addWidget(f); sl->addStretch(); sl->addWidget(s);
    speedLayout->addLayout(sl);
    connect(speedSlider_, &QSlider::valueChanged, this, &MainWindow::onSpeedChanged);
    sideLayout->addWidget(speedGroup);

    // ── Footer ───────────────────────────────────────────────
    sideLayout->addStretch();
    sideLayout->addWidget(makeDivider());
    infoLabel_ = new QLabel("Nodes: 0  ·  Height: 0");
    infoLabel_->setObjectName("infoLabel");
    sideLayout->addWidget(infoLabel_);

    mainLayout->addWidget(sidebar_);

    // ── Graphics View ────────────────────────────────────────
    view_ = new QGraphicsView(scene_);
    view_->setObjectName("treeView");
    view_->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform
                          | QPainter::TextAntialiasing);
    view_->setDragMode(QGraphicsView::ScrollHandDrag);
    view_->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    mainLayout->addWidget(view_, 1);

    setCentralWidget(central);

    // ── Canvas signals ───────────────────────────────────────
    connect(scene_, &TreeScene::insertValueRequested,
            this, &MainWindow::onInsertFromCanvas);
    connect(scene_, &TreeScene::deleteValueRequested,
            this, &MainWindow::onDeleteFromCanvas);

    // ── Rotation overlay (floating card on the canvas) ───────
    rotationOverlay_ = new QLabel(view_);
    rotationOverlay_->setObjectName("rotationOverlay");
    rotationOverlay_->setWordWrap(true);
    rotationOverlay_->hide();

    rotationHideTimer_ = new QTimer(this);
    rotationHideTimer_->setSingleShot(true);
    connect(rotationHideTimer_, &QTimer::timeout, rotationOverlay_, &QLabel::hide);

    // ── Sidebar collapse mouse tracker ───────────────────────
    collapseTimer_ = new QTimer(this);
    collapseTimer_->setSingleShot(true);
    collapseTimer_->setInterval(500);
    connect(collapseTimer_, &QTimer::timeout, this, &MainWindow::collapseSidebar);

    mouseTracker_ = new QTimer(this);
    mouseTracker_->setInterval(80);
    connect(mouseTracker_, &QTimer::timeout, this, [this]() {
        if (sidebarAnimating_) return;

        QPoint globalPos = QCursor::pos();
        QPoint localPos = mapFromGlobal(globalPos);

        // Only track if cursor is within the window
        if (!rect().contains(localPos)) {
            return;
        }

        if (!sidebarExpanded_ && localPos.x() < 18) {
            collapseTimer_->stop();
            expandSidebar();
        } else if (sidebarExpanded_ && localPos.x() > kSidebarWidth + 40) {
            if (!collapseTimer_->isActive()) {
                collapseTimer_->start();
            }
        } else if (sidebarExpanded_ && localPos.x() <= kSidebarWidth) {
            // Mouse over sidebar — cancel any pending collapse
            collapseTimer_->stop();
        }
    });
    mouseTracker_->start();

    // Status bar
    statusBar()->showMessage("Ready — insert a value or click + on the canvas");
}

// ══════════════════════════════════════════════════════════════
//  Sidebar collapse / expand
// ══════════════════════════════════════════════════════════════

void MainWindow::collapseSidebar() {
    if (!sidebarExpanded_ || sidebarAnimating_) return;
    sidebarExpanded_ = false;
    sidebarAnimating_ = true;

    auto* anim = new QVariantAnimation(this);
    anim->setDuration(260);
    anim->setStartValue(kSidebarWidth);
    anim->setEndValue(0);
    anim->setEasingCurve(QEasingCurve::InOutCubic);
    connect(anim, &QVariantAnimation::valueChanged, [this](const QVariant& v) {
        sidebar_->setFixedWidth(v.toInt());
    });
    connect(anim, &QVariantAnimation::finished, [this, anim]() {
        sidebarAnimating_ = false;
        sidebar_->hide();
        anim->deleteLater();
    });
    anim->start();
}

void MainWindow::expandSidebar() {
    if (sidebarExpanded_ || sidebarAnimating_) return;
    sidebarExpanded_ = true;
    sidebarAnimating_ = true;
    sidebar_->show();

    auto* anim = new QVariantAnimation(this);
    anim->setDuration(260);
    anim->setStartValue(0);
    anim->setEndValue(kSidebarWidth);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &QVariantAnimation::valueChanged, [this](const QVariant& v) {
        sidebar_->setFixedWidth(v.toInt());
    });
    connect(anim, &QVariantAnimation::finished, [this, anim]() {
        sidebarAnimating_ = false;
        anim->deleteLater();
    });
    anim->start();
}

// ══════════════════════════════════════════════════════════════
//  Rotation Overlay
// ══════════════════════════════════════════════════════════════

void MainWindow::showRotationOverlay(const std::vector<RotationStep>& steps) {
    if (steps.empty()) return;

    QString html = "<div style='line-height:1.5;'>";
    for (const auto& s : steps) {
        QString typeStr, icon, color;
        switch (s.type) {
            case RotationStep::Right:
                typeStr = "Right Rotation (LL case)";
                icon = "↻"; color = "#7C5CFC";
                break;
            case RotationStep::Left:
                typeStr = "Left Rotation (RR case)";
                icon = "↺"; color = "#7C5CFC";
                break;
            case RotationStep::LeftRight:
                typeStr = "Left-Right Rotation (LR case)";
                icon = "↻↺"; color = "#F39C12";
                break;
            case RotationStep::RightLeft:
                typeStr = "Right-Left Rotation (RL case)";
                icon = "↺↻"; color = "#F39C12";
                break;
        }
        html += QString("<span style='color:%1;font-size:16px;'>%2</span> "
                        "<b style='color:#ECEDF0;'>%3</b><br>"
                        "<span style='color:#6B7185;font-size:11px;'>Pivot: node %4</span><br>")
                    .arg(color, icon, typeStr).arg(s.pivotValue);
    }
    html += "</div>";

    rotationOverlay_->setText(html);
    rotationOverlay_->adjustSize();
    positionRotationOverlay();
    rotationOverlay_->show();

    // Highlight pivot nodes in the scene
    std::vector<int> pivots;
    for (const auto& s : steps) pivots.push_back(s.pivotValue);
    scene_->highlightPivots(pivots, 2500);

    rotationHideTimer_->start(4000);
}

void MainWindow::positionRotationOverlay() {
    if (!rotationOverlay_ || !view_) return;
    int x = view_->width() - rotationOverlay_->width() - 16;
    int y = 16;
    rotationOverlay_->move(x, y);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    positionRotationOverlay();
}

// ══════════════════════════════════════════════════════════════
//  Slots
// ══════════════════════════════════════════════════════════════

void MainWindow::onInsert() {
    int val = valueSpin_->value();
    BSTree* tree = isAVL_ ? static_cast<BSTree*>(avl_) : bst_;
    if (tree->search(val)) {
        statusBar()->showMessage(QString("Value %1 already exists").arg(val));
        return;
    }
    if (isAVL_) avl_->clearRotationLog();
    tree->insert(val);
    refreshScene();

    if (isAVL_ && !avl_->rotationLog().empty()) {
        showRotationOverlay(avl_->rotationLog());
    }
    statusBar()->showMessage(QString("Inserted %1").arg(val));
}

void MainWindow::onInsertFromCanvas() {
    bool ok = false;
    int val = QInputDialog::getInt(this, "Insert Node",
                                   "Enter value to insert:", 0, -999, 999, 1, &ok);
    if (!ok) return;

    BSTree* tree = isAVL_ ? static_cast<BSTree*>(avl_) : bst_;
    if (tree->search(val)) {
        statusBar()->showMessage(QString("Value %1 already exists").arg(val));
        return;
    }
    if (isAVL_) avl_->clearRotationLog();
    tree->insert(val);
    refreshScene();

    if (isAVL_ && !avl_->rotationLog().empty()) {
        showRotationOverlay(avl_->rotationLog());
    }
    statusBar()->showMessage(QString("Inserted %1").arg(val));
}

void MainWindow::onDeleteFromCanvas(int value) {
    BSTree* tree = isAVL_ ? static_cast<BSTree*>(avl_) : bst_;
    if (!tree->search(value)) return;

    if (isAVL_) avl_->clearRotationLog();
    tree->remove(value);
    refreshScene();

    if (isAVL_ && !avl_->rotationLog().empty()) {
        showRotationOverlay(avl_->rotationLog());
    }
    statusBar()->showMessage(QString("Deleted %1").arg(value));
}

void MainWindow::onDelete() {
    int val = valueSpin_->value();
    BSTree* tree = isAVL_ ? static_cast<BSTree*>(avl_) : bst_;
    if (!tree->search(val)) {
        statusBar()->showMessage(QString("Value %1 not found").arg(val));
        scene_->highlightNode(val, false);
        return;
    }
    if (isAVL_) avl_->clearRotationLog();
    tree->remove(val);
    refreshScene();

    if (isAVL_ && !avl_->rotationLog().empty()) {
        showRotationOverlay(avl_->rotationLog());
    }
    statusBar()->showMessage(QString("Deleted %1").arg(val));
}

void MainWindow::onSearch() {
    int val = valueSpin_->value();
    BSTree* tree = isAVL_ ? static_cast<BSTree*>(avl_) : bst_;
    bool found = tree->search(val);
    scene_->highlightNode(val, found, speedSlider_->value() * 3);
    statusBar()->showMessage(found ? QString("Found %1").arg(val)
                                   : QString("Value %1 not found").arg(val));
}

void MainWindow::onClear() {
    if (isAVL_) avl_->clear(); else bst_->clear();
    refreshScene();
    traversalOutput_->setText("—");
    rotationOverlay_->hide();
    statusBar()->showMessage("Tree cleared");
}

void MainWindow::onModeChanged(int index) {
    isAVL_ = (index == 1);
    refreshScene();
    rotationOverlay_->hide();
    statusBar()->showMessage(isAVL_ ? "AVL mode — auto-balancing enabled"
                                    : "BST mode — no balancing");
}

void MainWindow::onTraversal(const QString& type) {
    BSTree* tree = isAVL_ ? static_cast<BSTree*>(avl_) : bst_;
    std::vector<int> order;

    if (type == "InOrder")        order = tree->inOrder();
    else if (type == "PreOrder")  order = tree->preOrder();
    else if (type == "PostOrder") order = tree->postOrder();
    else                          order = tree->levelOrder();

    std::ostringstream oss;
    for (size_t i = 0; i < order.size(); ++i) {
        if (i) oss << "  →  ";
        oss << order[i];
    }
    traversalOutput_->setText(order.empty() ? "Empty tree"
                                            : QString::fromStdString(oss.str()));

    scene_->animateTraversal(order, speedSlider_->value());
    statusBar()->showMessage(
        QString("%1 traversal · %2 nodes").arg(type).arg(order.size()));
}

void MainWindow::onSpeedChanged(int value) {
    scene_->setAnimationSpeed(value);
}

void MainWindow::onGenerateRandom() {
    BSTree* tree = isAVL_ ? static_cast<BSTree*>(avl_) : bst_;
    if (isAVL_) avl_->clearRotationLog();

    int count = 0;
    for (int i = 0; i < 5; ++i) {
        int val = QRandomGenerator::global()->bounded(-99, 100);
        if (!tree->search(val)) {
            tree->insert(val);
            ++count;
        }
    }
    refreshScene();

    if (isAVL_ && !avl_->rotationLog().empty()) {
        showRotationOverlay(avl_->rotationLog());
    }
    statusBar()->showMessage(QString("Inserted %1 random nodes").arg(count));
}

// ══════════════════════════════════════════════════════════════
//  Refresh
// ══════════════════════════════════════════════════════════════

void MainWindow::refreshScene() {
    BSTree* tree = isAVL_ ? static_cast<BSTree*>(avl_) : bst_;

    std::function<int(BSTNode*)> bfGetter = nullptr;
    if (isAVL_) {
        bfGetter = [this](BSTNode* n) { return avl_->getBalanceFactor(n); };
    }

    scene_->rebuildFromTree(tree->getRoot(), isAVL_, bfGetter);

    BSTNode* root = tree->getRoot();
    int h = root ? root->height : 0;
    infoLabel_->setText(QString("Nodes: %1  ·  Height: %2")
                            .arg(tree->size()).arg(h));

    if (tree->getRoot()) {
        view_->fitInView(scene_->sceneRect(), Qt::KeepAspectRatio);
    }
}
