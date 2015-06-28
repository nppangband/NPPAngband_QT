/* File: hotkeys.cpp */

/*
 * Copyright (c) 2014 Jeff Greene, Diego Gonzalez
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
#include <src/cmds.h>
#include <src/hotkeys.h>

QVector<hotkey_step> single_hotkey;


static const hotkey_list list_hotkeys[] =
{
    {"F1", Qt::Key_F1},
    {"F2", Qt::Key_F2},
    {"F3", Qt::Key_F3},
    {"F4", Qt::Key_F4},
    {"F5", Qt::Key_F5},
    {"F6", Qt::Key_F6},
    {"F7", Qt::Key_F7},
    {"F8", Qt::Key_F8},
    {"F9", Qt::Key_F9},
    {"F10", Qt::Key_F10},
    {"F11", Qt::Key_F11},
    {"F12", Qt::Key_F12},
};

hotkey_type hotkey_actions[] =
{
    //HK_TYPE_MOVE
    {command_walk, TRUE, FALSE, FALSE, FALSE, "Walk"},
    //HK_TYPE_JUMP
    {command_walk, TRUE, FALSE, FALSE, FALSE, "Jump"},
    //HK_TYPE_RUN
    {command_walk, TRUE, FALSE, FALSE, FALSE, "Run"},
};

HotKeyDialog::HotKeyDialog(void)
{
    //Set up the main scroll bar
    QVBoxLayout *top_layout = new QVBoxLayout;
    QVBoxLayout *main_layout = new QVBoxLayout;
    top_widget = new QWidget;
    scroll_box = new QScrollArea;
    top_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    top_widget->setLayout(main_layout);
    scroll_box->setWidget(top_widget);
    scroll_box->setWidgetResizable(TRUE);
    top_layout->addWidget(scroll_box);

    //Build the header
    QLabel *header_main = new QLabel("<b><h2>Hotkey Menu</b></h2>");
    main_layout->addWidget(header_main);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    main_layout->addWidget(buttons);

    setLayout(top_layout);
    setWindowTitle(tr("Hotkey Menu"));

    this->exec();
}

void do_hotkey_manage()
{
    HotKeyDialog();
}

void do_hotkey_export()
{

}

void do_hotkey_import()
{

}
