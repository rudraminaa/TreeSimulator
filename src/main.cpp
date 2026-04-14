#include <QApplication>
#include <QFile>
#include <QFont>
#include <QIcon>
#include "gui/MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set window icon
    app.setWindowIcon(QIcon(":/icon.png"));

    // Load global stylesheet
    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        app.setStyleSheet(styleFile.readAll());
        styleFile.close();
    }

    // Set default font
    QFont defaultFont("Segoe UI", 10);
    app.setFont(defaultFont);

    MainWindow window;
    window.show();

    return app.exec();
}
