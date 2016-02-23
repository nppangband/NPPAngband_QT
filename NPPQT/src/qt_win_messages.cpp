/*
 * Copyright (c) 2015 Jeff Greene, Diego Gonzalez
 *
 * Please see copyright.txt for complete copyright and licensing restrictions.
 *
 */

#include <src/npp.h>
#include <src/qt_mainwindow.h>
#include <src/messages.h>
#include <QHeaderView>
#include <QFontDialog>

void MainWindow::set_font_win_messages(QFont newFont)
{
    win_message_settings.win_font = newFont;
    win_messages_update();
}

void MainWindow::win_messages_font()
{
    bool selected;
    QFont font = QFontDialog::getFont(&selected, win_message_settings.win_font, this );

    if (selected)
    {
        set_font_win_messages(font);
    }
}

// For when savefiles close but the game doesn't.
void MainWindow::win_messages_wipe()
{
    if (!win_message_settings.win_show) return;
    if (!character_generated) return;

    win_messages_area->clear();

}

void MainWindow::win_messages_update()
{
    if (!win_message_settings.win_show) return;
    if (!character_generated) return;
    update_message_window(win_messages_area, win_message_settings.win_font);
}



/*
 *  Show widget is called after this to allow
 * the settings to restore the save geometry.
 */
void MainWindow::win_messages_create()
{
    window_messages = new QWidget();
    win_messages_vlay = new QVBoxLayout;
    window_messages->setLayout(win_messages_vlay);
    win_messages_area = new QTextEdit;
    win_messages_area->setReadOnly(TRUE);
    win_messages_area->setStyleSheet("background-color: black;");
    win_messages_area->setTextInteractionFlags(Qt::TextSelectableByMouse);
    win_messages_vlay->addWidget(win_messages_area);
    win_messages_menubar = new QMenuBar;
    win_messages_vlay->setMenuBar(win_messages_menubar);
    window_messages->setWindowTitle("Messages Window");
    win_messages_win_settings = win_messages_menubar->addMenu(tr("&Settings"));
    win_messages_set_font_act = new QAction(tr("Set Messages Window Font"), this);
    win_messages_set_font_act->setStatusTip(tr("Set the font for the Messages Window."));
    connect(win_messages_set_font_act, SIGNAL(triggered()), this, SLOT(win_messages_font()));
    win_messages_win_settings->addAction(win_messages_set_font_act);

    window_messages->setAttribute(Qt::WA_DeleteOnClose);
    connect(window_messages, SIGNAL(destroyed(QObject*)), this, SLOT(win_messages_destroy(QObject*)));

    reset_message_display_marks();
}

void MainWindow::win_messages_destroy(QObject *this_object)
{
    (void)this_object;
    if (!win_message_settings.win_show) return;
    if (!window_messages) return;
    win_message_settings.get_widget_settings(window_messages);
    window_messages->deleteLater();
    win_message_settings.win_show = FALSE;
    win_messages_act->setText("Show Message Display Window");

    reset_message_display_marks();
}

void MainWindow::win_messages_close()
{
    bool was_open = win_message_settings.win_show;
    win_messages_destroy(window_messages);
    win_message_settings.win_show = was_open;
}

void MainWindow::toggle_win_messages()
{
    if (!win_message_settings.win_show)
    {
        win_messages_create();
        win_message_settings.win_show = TRUE;
        window_messages->setGeometry(win_message_settings.win_geometry);
        win_messages_act->setText("Hide Message Display Window");
        if (win_message_settings.win_maximized) window_messages->showMaximized();
        else window_messages->show();
        win_messages_update();
    }
    else win_messages_destroy(window_messages);
}

