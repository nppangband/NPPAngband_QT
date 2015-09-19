/*
 * Copyright (c) 2015 Jeff Greene, Diego Gonzalez
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 3, or
 *
 * b) the "Angband licence":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 */

#include <src/npp.h>
#include <src/qt_mainwindow.h>
#include <src/messages.h>
#include <QHeaderView>
#include <QFontDialog>

void MainWindow::set_font_win_messages(QFont newFont)
{
    font_win_messages = newFont;
    win_messages_update();
}

void MainWindow::win_messages_font()
{
    bool selected;
    QFont font = QFontDialog::getFont(&selected, font_win_messages, this );

    if (selected)
    {
        set_font_win_messages(font);
    }
}

// For when savefiles close but the game doesn't.
void MainWindow::win_messages_wipe()
{
    if (!show_messages_win) return;
    if (!character_generated) return;

    win_messages_area->clear();

}

void MainWindow::win_messages_update()
{
    if (!show_messages_win) return;
    if (!character_generated) return;
    update_message_area(win_messages_area, 30, font_win_messages);
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
    win_messages_vlay->addWidget(win_messages_area);
    win_messages_menubar = new QMenuBar;
    win_messages_vlay->setMenuBar(win_messages_menubar);
    window_messages->setWindowTitle("Messages Window");
    messages_win_settings = win_messages_menubar->addMenu(tr("&Settings"));
    win_messages_set_font = new QAction(tr("Set Messages Window Font"), this);
    win_messages_set_font->setStatusTip(tr("Set the font for the Messages Window."));
    connect(win_messages_set_font, SIGNAL(triggered()), this, SLOT(win_messages_font()));
    messages_win_settings->addAction(win_messages_set_font);

    //Disable the x button from closing the widget
    window_messages->setWindowFlags(Qt::WindowTitleHint);
}


void MainWindow::win_messages_destroy()
{
    if (!show_messages_win) return;
    delete window_messages;
}

void MainWindow::toggle_win_messages()
{
    if (!show_messages_win)
    {
        win_messages_create();
        show_messages_win = TRUE;
        win_messages->setText("Hide Message Display Window");
        window_messages->show();
        win_messages_update();
    }
    else
    {
        win_messages_destroy();
        show_messages_win = FALSE;
        win_messages->setText("Show Message Display Window");
    }
}

