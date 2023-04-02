#include <QApplication>
#include <QMainWindow>
#include <QFrame>
#include <QLabel>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create a main window to hold the frame
    QMainWindow mainWindow;
    mainWindow.setWindowTitle("Frame with Title");

    // Create a frame and set its properties
    QFrame *frame = new QFrame(&mainWindow);
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Sunken);
    frame->setLineWidth(1);

    // Create a label to serve as the frame's title
    QLabel *titleLabel = new QLabel("My Frame Title", &mainWindow);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold;");
    titleLabel->setGeometry(frame->x(), frame->y() - 25, frame->width(), 20);

    // Set the main window's central widget to the frame
    mainWindow.setCentralWidget(frame);

    mainWindow.show();
    return app.exec();
}
