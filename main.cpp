#include <QApplication>
#include <QContextMenuEvent>
#include <QDebug>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QPalette>
#include <QSettings>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStyleFactory>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

class MyQApplication : public QApplication
{
public:
    MyQApplication(int& argc, char** argv)
        : QApplication(argc, argv)
    {
        setStyle(QStyleFactory::create("Windows"));

        QPalette darkPalette;
        QColor darkColor = QColor(45, 45, 45);
        QColor disabledColor = QColor(127, 127, 127);

        darkPalette.setColor(QPalette::Window, darkColor);
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(18, 18, 18));
        darkPalette.setColor(QPalette::AlternateBase, darkColor);
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
        darkPalette.setColor(QPalette::Button, darkColor);
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);

        setPalette(darkPalette);

        setStyleSheet(
            "QToolTip { color: #ffffff; "
            "background-color: #2a82da; "
            "border: 1px solid white; }");
    }
};

class TestWindow : public QMainWindow
{
public:
    TestWindow()
    {
        setWindowTitle("Qt Context Menu Accessibility Test");
        resize(700, 500);

        auto* central = new QWidget(this);
        auto* layout = new QVBoxLayout(central);

        layout->addWidget(new QLabel(
            "Test 1: Right-click the empty area.\n"
            "Test 2: Right-click inside the table.\n"
            "Test 3: Focus the edit box and press the Applications key.\n\n"
            "Disabled items should receive keyboard focus and NVDA should announce 'unavailable'."));

        m_table = new QTableView(this);

        auto* model = new QStandardItemModel(5, 2, this);
        model->setHorizontalHeaderLabels({"Name", "Value"});

        for (int i = 0; i < 5; ++i)
        {
            model->setItem(i, 0, new QStandardItem(QString("Item %1").arg(i + 1)));
            model->setItem(i, 1, new QStandardItem(QString::number(i + 1)));
        }

        m_table->setModel(model);
        m_table->horizontalHeader()->setStretchLastSection(true);
        m_table->setContextMenuPolicy(Qt::CustomContextMenu);

        connect(m_table,
                &QWidget::customContextMenuRequested,
                this,
                &TestWindow::showTableMenu);

        layout->addWidget(m_table);

        m_edit = new QLineEdit(this);
        m_edit->setText("Right click or press the Applications key here.");
        layout->addWidget(m_edit);

        setCentralWidget(central);
    }

protected:
    void contextMenuEvent(QContextMenuEvent* e) override
    {
        if (childAt(e->pos()))
        {
            QMainWindow::contextMenuEvent(e);
            return;
        }

        QMenu menu(this);

        menu.addAction("Enabled");

        QAction* disabled = menu.addAction("Disabled");
        disabled->setEnabled(false);

        menu.addAction("Another Enabled");

        menu.exec(e->globalPos());
    }

private:
    void showTableMenu(const QPoint& point)
    {
        QMenu menu(this);

        menu.addAction("Open");

        QAction* disabled = menu.addAction("Disabled");
        disabled->setEnabled(false);

        menu.addAction("Rename");
        menu.addAction("Delete");

        menu.exec(m_table->viewport()->mapToGlobal(point));
    }

    QTableView* m_table{};
    QLineEdit* m_edit{};
};

int main(int argc, char* argv[])
{
MyQApplication app(argc, argv);

    qDebug() << "Qt version:" << qVersion();
    qDebug() << "Style:" << app.style()->metaObject()->className();

    TestWindow w;
    w.show();

    return app.exec();
}