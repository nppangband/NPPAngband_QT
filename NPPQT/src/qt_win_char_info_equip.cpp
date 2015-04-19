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
#include <src/player_screen.h>
#include <QHeaderView>
#include <QFontDialog>



void MainWindow::update_label_equip_font()
{
    QList<QLabel *> lbl_list = window_char_info_equip->findChildren<QLabel *>();
    for (int i = 0; i < lbl_list.size(); i++)
    {
        QLabel *this_lbl = lbl_list.at(i);
        this_lbl->setFont(font_char_equip_info);
    }
}

void MainWindow::set_font_char_info_equip(QFont newFont)
{
    font_char_equip_info = newFont;
    update_label_equip_font();

}

void MainWindow::win_char_info_equip_font()
{
    bool selected;
    QFont font = QFontDialog::getFont( &selected, font_char_equip_info, this);

    if (selected)
    {
        set_font_char_info_equip(font);
    }
}

// For when savefiles close but the game doesn't.
void MainWindow::win_char_info_equip_wipe()
{
    if (!show_char_info_equip) return;
    if (!character_generated) return;
    clear_layout(main_vlay_char_equip);
}


void MainWindow::win_char_info_equip_update()
{
    if (!character_generated) return;
    if (!show_char_info_equip) return;
    equip_flag_info_update(resist_widget, resist_flags, FLAGS_RESIST, font_char_equip_info);
    equip_flag_info_update(ability_widget, ability_flags, FLAGS_ABILITY, font_char_equip_info);
    equip_flag_info_update(nativity_widget, nativity_flags, FLAGS_NATIVITY, font_char_equip_info);
    equip_modifier_info_update(equip_widget, equip_mods, font_char_equip_info);
}

void MainWindow::create_win_char_equip()
{
    if (!character_generated) return;
    if (!show_char_info_equip) return;

    // Object Info
    QGridLayout *equip_info = new QGridLayout;
    main_vlay_char_equip->addLayout(equip_info);

    QVBoxLayout *resist_vlay = new QVBoxLayout;
    QVBoxLayout *ability_vlay = new QVBoxLayout;
    QVBoxLayout *equip_vlay = new QVBoxLayout;
    QVBoxLayout *nativity_vlay = new QVBoxLayout;
    resist_flags = new QGridLayout;
    ability_flags = new QGridLayout;
    equip_mods = new QGridLayout;
    nativity_flags = new QGridLayout;
    resist_widget = new QWidget;
    ability_widget = new QWidget;
    equip_widget = new QWidget;
    nativity_widget = new QWidget;
    resist_vlay->addWidget(resist_widget);
    ability_vlay->addWidget(ability_widget);
    equip_vlay->addWidget(equip_widget);
    nativity_vlay->addWidget(nativity_widget);
    resist_widget->setLayout(resist_flags);
    ability_widget->setLayout(ability_flags);
    equip_widget->setLayout(equip_mods);
    nativity_widget->setLayout(nativity_flags);
    resist_vlay->addStretch(1);
    ability_vlay->addStretch(1);
    equip_vlay->addStretch(1);
    nativity_vlay->addStretch(1);

    QLabel *resist_label = new QLabel("<h3>Resistance Information</h3>");
    resist_label->setToolTip(QString("Blue represents elemental immunity, green represents resistance, and purple represents double resistance."));
    equip_info->addWidget(resist_label, 0, 0, Qt::AlignCenter);
    equip_info->addLayout(resist_vlay, 1, 0);

    QLabel *ability_label = new QLabel("<h3>Ability Information</h3>");
    equip_info->addWidget(ability_label, 0, 1, Qt::AlignCenter);
    equip_info->addLayout(ability_vlay,  1, 1);

    QLabel *nativity_label = new QLabel("<h3>Nativity Information</h3>");
    equip_info->addWidget(nativity_label, 0, 2, Qt::AlignCenter);
    equip_info->addLayout(nativity_vlay, 1, 2);

    QLabel *modifier_label = new QLabel("<h3>Equipment Modifiers</h3>");
    equip_info->addWidget(modifier_label, 0, 3, Qt::AlignCenter);
    equip_info->addLayout(equip_vlay, 1, 3);

    equip_flag_info(resist_widget, resist_flags, FLAGS_RESIST, font_char_equip_info);
    equip_flag_info(ability_widget, ability_flags, FLAGS_ABILITY, font_char_equip_info);
    equip_flag_info(nativity_widget, nativity_flags, FLAGS_NATIVITY, font_char_equip_info);
    equip_modifier_info(equip_widget, equip_mods, font_char_equip_info);

    win_char_info_equip_update();
    update_label_equip_font();
}

/*
 *  Make the equip shell
 *  The game crashes if the labels are drawn before the character is created
 *  So that is filled after a character is created.
 */
void MainWindow::win_char_info_equip_create()
{
    window_char_info_equip = new QWidget();
    main_vlay_char_equip = new QVBoxLayout;
    window_char_info_equip->setLayout(main_vlay_char_equip);

    char_info_equip_menubar = new QMenuBar;
    main_vlay_char_equip->setMenuBar(char_info_equip_menubar);
    window_char_info_equip->setWindowTitle("Character Equipment Information");
    char_info_equip_settings = char_info_equip_menubar->addMenu(tr("&Settings"));
    char_info_equip_font = new QAction(tr("Set Basic Character Screen Font"), this);
    char_info_equip_font->setStatusTip(tr("Set the font for the Basic Character Information screen."));
    connect(char_info_equip_font, SIGNAL(triggered()), this, SLOT(win_char_info_equip_font()));
    char_info_equip_settings->addAction(char_info_equip_font);

    //Disable the x button from closing the widget
    window_char_info_equip->setWindowFlags(Qt::WindowTitleHint);
}

void MainWindow::win_char_info_equip_destroy()
{
    if (!show_char_info_equip) return;
    delete window_char_info_equip;
}

void MainWindow::toggle_win_char_equip_frame()
{
    if (!show_char_info_equip)
    {
        win_char_info_equip_create();
        show_char_info_equip = TRUE;
        create_win_char_equip();
        win_char_equip->setText("Hide Character Equipment Information");
        window_char_info_equip->show();
    }
    else

    {
        win_char_info_equip_destroy();
        show_char_info_equip = FALSE;
        win_char_equip->setText("Show Character Equipment Information");
    }
}


