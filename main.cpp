#include <QApplication>
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QStandardItemModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

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
            "There are three independent tests.\n\n"
            "1. Right-click anywhere in the empty area below this text.\n"
            "2. Right-click inside the table.\n"
            "3. Focus the edit box and press the Applications key or Shift+F10.\n\n"
            "With NVDA, disabled menu items should receive focus and be announced as unavailable."
        ));

        m_table = new QTableView(this);

        auto* model = new QStandardItemModel(5, 2, this);
        model->setHorizontalHeaderLabels({"Name", "Value"});

        for (int r = 0; r < 5; ++r)
        {
            model->setItem(r, 0, new QStandardItem(QString("Item %1").arg(r + 1)));
            model->setItem(r, 1, new QStandardItem(QString::number(r + 1)));
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
        m_edit->setPlaceholderText(
            "Press the Applications key or Shift+F10 here.\n"
            "Qt's built-in Edit context menu should appear.");

        m_edit->setText("Select some text here and invoke the context menu.");

        layout->addWidget(m_edit);

        setCentralWidget(central);
    }

protected:
    void contextMenuEvent(QContextMenuEvent* event) override
    {
        // Ignore right-clicks that belong to child widgets.
        if (childAt(event->pos()))
        {
            QMainWindow::contextMenuEvent(event);
            return;
        }

        QMenu menu(this);

        QAction* enabled = menu.addAction("Enabled");
        Q_UNUSED(enabled);

        QAction* disabled = menu.addAction("Disabled");
        disabled->setEnabled(false);

        QAction* enabled2 = menu.addAction("Another Enabled");
        Q_UNUSED(enabled2);

        menu.exec(event->globalPos());
    }

private:
    void showTableMenu(const QPoint& point)
    {
        QMenu menu(this);

        QAction* open = menu.addAction("Open");
        Q_UNUSED(open);

        QAction* disabled = menu.addAction("Disabled");
        disabled->setEnabled(false);

        QAction* rename = menu.addAction("Rename");
        Q_UNUSED(rename);

        QAction* deleteItem = menu.addAction("Delete");
        Q_UNUSED(deleteItem);

        menu.exec(m_table->viewport()->mapToGlobal(point));
    }

    QTableView* m_table{};
    QLineEdit* m_edit{};
};

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    TestWindow w;
    w.show();

    return app.exec();
}