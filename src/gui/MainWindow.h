#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QTimer>
#include <QVariantAnimation>

#include "../core/BSTree.h"
#include "../core/AVLTree.h"
#include "TreeScene.h"

/**
 * @brief Main application window with collapsible sidebar,
 *        rotation overlay, and canvas-based insertion.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onInsert();
    void onInsertFromCanvas();
    void onDeleteFromCanvas(int value);
    void onDelete();
    void onSearch();
    void onClear();
    void onModeChanged(int index);
    void onTraversal(const QString& type);
    void onSpeedChanged(int value);
    void onGenerateRandom();

private:
    void setupUI();
    void refreshScene();
    void showRotationOverlay(const std::vector<RotationStep>& steps);
    void positionRotationOverlay();
    QPushButton* makeButton(const QString& text, const QString& objectName);

    void expandSidebar();
    void collapseSidebar();

    // Tree backends
    BSTree*  bst_  = nullptr;
    AVLTree* avl_  = nullptr;
    bool     isAVL_ = false;

    // GUI
    TreeScene*     scene_;
    QGraphicsView* view_;
    QComboBox*     modeCombo_;
    QSpinBox*      valueSpin_;
    QLabel*        traversalOutput_;
    QLabel*        infoLabel_;
    QSlider*       speedSlider_;

    // Sidebar collapse
    QWidget*  sidebar_ = nullptr;
    bool      sidebarExpanded_ = true;
    bool      sidebarAnimating_ = false;
    QTimer*   collapseTimer_ = nullptr;
    QTimer*   mouseTracker_ = nullptr;
    static constexpr int kSidebarWidth = 290;

    // Rotation overlay
    QLabel* rotationOverlay_ = nullptr;
    QTimer* rotationHideTimer_ = nullptr;
};

#endif // MAINWINDOW_H
