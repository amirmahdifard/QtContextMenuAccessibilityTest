#include <QApplication>
#include <QDebug>
#include <QHeaderView>
#include <QLabel>
#include <QMainWindow>
#include <QPalette>
#include <QSettings>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStyleFactory>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>

class MyQApplication : public QApplication
{
public:
    MyQApplication(int& argc, char** argv)
        : QApplication(argc, argv)
    {
        // Use the native Windows style.
        // This avoids the Fusion style accessibility issue we previously found.
        setStyle(QStyleFactory::create("Windows"));

        // Dark palette, matching the previous TeamTalk test environment.
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
        darkPalette.setColor(
            QPalette::Disabled,
            QPalette::Text,
            disabledColor);

        darkPalette.setColor(QPalette::Button, darkColor);
        darkPalette.setColor(QPalette::ButtonText, Qt::white);

        darkPalette.setColor(
            QPalette::Disabled,
            QPalette::ButtonText,
            disabledColor);

        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

        darkPalette.setColor(
            QPalette::Highlight,
            QColor(42, 130, 218));

        darkPalette.setColor(
            QPalette::HighlightedText,
            Qt::black);

        darkPalette.setColor(
            QPalette::Disabled,
            QPalette::HighlightedText,
            disabledColor);

        setPalette(darkPalette);

        setStyleSheet(
            "QToolTip { "
            "color: #ffffff; "
            "background-color: #2a82da; "
            "border: 1px solid white; "
            "}");
    }
};

class TestWindow : public QMainWindow
{
public:
    TestWindow()
    {
        setWindowTitle(
            "Qt Tree Accessibility Bug Finder");

        resize(900, 650);

        auto* central = new QWidget(this);
        auto* layout = new QVBoxLayout(central);

        auto* instructions = new QLabel(
            "Qt Tree Accessibility Test\n\n"

            "This test reproduces the channel hierarchy "
            "used by TeamTalk.\n\n"

            "Suggested reproduction sequence:\n"

            "1. Expand Channel 1.\n"
            "2. Navigate down into its users.\n"
            "3. Navigate several users deep.\n"
            "4. Press Left Arrow to collapse Channel 1.\n"
            "5. Press Down Arrow to navigate through Channel 2, "
            "Channel 3, Channel 4, etc.\n\n"

            "With NVDA, check whether every channel is announced "
            "correctly after collapsing Channel 1.\n\n"

            "Also test different depths:\n"
            "- Navigate to User 1, collapse, test.\n"
            "- Navigate to User 2, collapse, test.\n"
            "- Navigate to User 3, collapse, test.\n"
            "- Navigate to User 4, collapse, test.\n\n"

            "The original TeamTalk issue reported that the number "
            "of unreadable sibling channels may depend on how "
            "deeply the user navigated before collapsing."
        );

        instructions->setWordWrap(true);

        layout->addWidget(instructions);

        m_tree = new QTreeView(this);

        m_model = new QStandardItemModel(this);

        m_model->setHorizontalHeaderLabels(
            {"Channel / User"});

        buildTree();

        m_tree->setModel(m_model);

        m_tree->setHeaderHidden(false);

        m_tree->header()->setStretchLastSection(true);

        m_tree->setUniformRowHeights(false);

        m_tree->setAnimated(false);

        m_tree->setAllColumnsShowFocus(true);

        layout->addWidget(m_tree);

        setCentralWidget(central);

        // Start with the root channel expanded so the test
        // can begin immediately.
        QModelIndex rootIndex =
            m_model->index(0, 0);

        m_tree->expand(rootIndex);

        // Give keyboard focus to the tree.
        m_tree->setFocus();
    }

private:

    void buildTree()
    {
        QStandardItem* root =
            new QStandardItem("Root Channel");

        m_model->appendRow(root);

        // -------------------------------------------------
        // Channel 1
        // -------------------------------------------------

        QStandardItem* channel1 =
            new QStandardItem("Channel 1");

        root->appendRow(channel1);

        // Users directly inside Channel 1.

        channel1->appendRow(
            new QStandardItem("User 1"));

        channel1->appendRow(
            new QStandardItem("User 2"));

        channel1->appendRow(
            new QStandardItem("User 3"));

        channel1->appendRow(
            new QStandardItem("User 4"));

        channel1->appendRow(
            new QStandardItem("User 5"));

        // -------------------------------------------------
        // Subchannel 1.1
        // -------------------------------------------------

        QStandardItem* subchannel11 =
            new QStandardItem("Subchannel 1.1");

        channel1->appendRow(subchannel11);

        subchannel11->appendRow(
            new QStandardItem("User 1.1.1"));

        subchannel11->appendRow(
            new QStandardItem("User 1.1.2"));

        subchannel11->appendRow(
            new QStandardItem("User 1.1.3"));

        // -------------------------------------------------
        // Subchannel 1.1.1
        // -------------------------------------------------

        QStandardItem* subchannel111 =
            new QStandardItem("Subchannel 1.1.1");

        subchannel11->appendRow(subchannel111);

        subchannel111->appendRow(
            new QStandardItem("User 1.1.1.1"));

        subchannel111->appendRow(
            new QStandardItem("User 1.1.1.2"));

        // -------------------------------------------------
        // Subchannel 1.2
        // -------------------------------------------------

        QStandardItem* subchannel12 =
            new QStandardItem("Subchannel 1.2");

        channel1->appendRow(subchannel12);

        subchannel12->appendRow(
            new QStandardItem("User 1.2.1"));

        subchannel12->appendRow(
            new QStandardItem("User 1.2.2"));

        // -------------------------------------------------
        // Channel 2
        // -------------------------------------------------

        QStandardItem* channel2 =
            new QStandardItem("Channel 2");

        root->appendRow(channel2);

        channel2->appendRow(
            new QStandardItem("User 2.1"));

        channel2->appendRow(
            new QStandardItem("User 2.2"));

        // -------------------------------------------------
        // Channel 3
        // -------------------------------------------------

        QStandardItem* channel3 =
            new QStandardItem("Channel 3");

        root->appendRow(channel3);

        channel3->appendRow(
            new QStandardItem("User 3.1"));

        channel3->appendRow(
            new QStandardItem("User 3.2"));

        // -------------------------------------------------
        // Channel 4
        // -------------------------------------------------

        QStandardItem* channel4 =
            new QStandardItem("Channel 4");

        root->appendRow(channel4);

        channel4->appendRow(
            new QStandardItem("User 4.1"));

        channel4->appendRow(
            new QStandardItem("User 4.2"));

        // -------------------------------------------------
        // Channel 5
        // -------------------------------------------------

        QStandardItem* channel5 =
            new QStandardItem("Channel 5");

        root->appendRow(channel5);

        channel5->appendRow(
            new QStandardItem("User 5.1"));

        channel5->appendRow(
            new QStandardItem("User 5.2"));

        // -------------------------------------------------
        // Channel 6
        // -------------------------------------------------

        QStandardItem* channel6 =
            new QStandardItem("Channel 6");

        root->appendRow(channel6);

        channel6->appendRow(
            new QStandardItem("User 6.1"));

        channel6->appendRow(
            new QStandardItem("User 6.2"));

        // -------------------------------------------------
        // Channel 7
        // -------------------------------------------------

        QStandardItem* channel7 =
            new QStandardItem("Channel 7");

        root->appendRow(channel7);

        channel7->appendRow(
            new QStandardItem("User 7.1"));

        channel7->appendRow(
            new QStandardItem("User 7.2"));
    }

private:

    QTreeView* m_tree{};
    QStandardItemModel* m_model{};
};

int main(int argc, char* argv[])
{
    MyQApplication app(argc, argv);

    qDebug()
        << "Qt version:"
        << qVersion();

    qDebug()
        << "Style:"
        << app.style()
               ->metaObject()
               ->className();

    TestWindow window;

    window.show();

    return app.exec();
}