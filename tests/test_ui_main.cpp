/// @file test_ui_main.cpp
/// Custom main for astrofind_ui_tests: creates QApplication before Catch2
/// so that any test in this executable can construct Qt widgets.
#include <catch2/catch_session.hpp>
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    return Catch::Session().run(argc, argv);
}
