/* File: knowledge.cpp */

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
#include <src/knowledge.h>
#include <src/utilities.h>
#include <src/store.h>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

DisplayNotesFile::DisplayNotesFile(void)
{
    QVBoxLayout *main_layout = new QVBoxLayout;
    QGridLayout *notes_info = new QGridLayout;

    QLabel *obj_label = new QLabel(QString("<b><big>%1 Notes and Accomplishments</big></b>") .arg(op_ptr->full_name));
    obj_label->setAlignment(Qt::AlignCenter);

    main_layout->addWidget(obj_label);
    main_layout->addLayout(notes_info);

    int row = 0;
    int col = 0;

    QLabel *header_turn = new QLabel("<b><u>GAME TURN</u>  </b>");
    QLabel *header_depth = new QLabel("<b>  <u>DUNGEON DEPTH</u>  </b>");
    QLabel *header_level = new QLabel("<b>  <u>PLAYER LEVEL</u>  </b>");
    QLabel *header_event = new QLabel("<b>  <u>EVENT</u>  </b>");
    notes_info->addWidget(header_turn, row, col++, Qt::AlignRight);
    notes_info->addWidget(header_depth, row, col++, Qt::AlignRight);
    notes_info->addWidget(header_level, row, col++, Qt::AlignRight);
    notes_info->addWidget(header_event, row++, col++, Qt::AlignLeft);

    // Print out all the notes
    for (int i = 0; i < notes_log.size(); i++)
    {
        QString depth_note = (QString("Town"));
        row++;
        col = 0;
        notes_type *notes_ptr = &notes_log[i];
        QLabel *game_turn = new QLabel(number_to_formatted_string(notes_ptr->game_turn));
        notes_info->addWidget(game_turn, row, col++, Qt::AlignRight | Qt::AlignTop);
        // Format the depth, unless the player is in town
        if (notes_ptr->dun_depth) depth_note = number_to_formatted_string(notes_ptr->dun_depth * 50);
        QLabel *game_depth = new QLabel(depth_note);
        notes_info->addWidget(game_depth, row, col++, Qt::AlignRight | Qt::AlignTop);
        QLabel *player_level = new QLabel(QString("%1") .arg(notes_ptr->player_level));
        notes_info->addWidget(player_level, row, col++, Qt::AlignRight | Qt::AlignTop);
        QLabel *game_event = new QLabel(notes_ptr->recorded_note);
        game_event->setWordWrap(TRUE);
        notes_info->addWidget(game_event, row, col++, Qt::AlignLeft | Qt::AlignTop);
    }

    //Add a close button on the right side
    QHBoxLayout *close_across = new QHBoxLayout;
    main_layout->addLayout(close_across);
    close_across->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, SIGNAL(rejected()), this, SLOT(close()));
    close_across->addWidget(buttons);

    setLayout(main_layout);
    setWindowTitle(tr("Display Notes"));

    this->exec();
}

void display_notes_file(void)
{
    DisplayNotesFile();
}

DisplayHomeInven::DisplayHomeInven(void)
{
    // First handle an empty home
    store_type *st_ptr = &store[STORE_HOME];
    if (!st_ptr->stock_num)
    {
        pop_up_message_box("Your Home Is Empty");
        return;
    }

    QVBoxLayout *main_layout = new QVBoxLayout;

    QLabel *home_label = new QLabel(QString("<h1><b>Home Inventory</b></h1>"));
    home_label->setAlignment(Qt::AlignCenter);
    main_layout->addWidget(home_label);


    /* Display contents of the home */
    for (int i = 0; i < st_ptr->stock_num; i++)
    {
        QChar prefix = number_to_letter(i);
        object_type *o_ptr = &st_ptr->stock[i];
        QString o_name = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);
        QString o_desc = identify_random_gen(o_ptr);

        QLabel *name_label = new QLabel(QString("<h3>%1) %2</h3>") .arg(prefix) .arg(o_name));
        QLabel *desc_label = new QLabel(o_desc);
        desc_label->setWordWrap(TRUE);
        desc_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        main_layout->addWidget(name_label);
        main_layout->addWidget(desc_label);
    }

     //Add a close button on the right side
    QHBoxLayout *close_across = new QHBoxLayout;
    main_layout->addLayout(close_across);
    close_across->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, SIGNAL(rejected()), this, SLOT(close()));
    close_across->addWidget(buttons);

    setLayout(main_layout);
    setWindowTitle(tr("Contents Of Your Home"));

    this->exec();
}

void display_home_inventory(void)
{
    DisplayHomeInven();
}

DisplayMonKillCount::DisplayMonKillCount(void)
{
    QVector<mon_kills> mon_kill_list;

    /* Collect matching monsters */
    for (int i = 1; i < z_info->r_max - 1; i++)
    {
        monster_race *r_ptr = &r_info[i];
        monster_lore *l_ptr = &l_list[i];

        /* Require non-unique monsters */
        if (r_ptr->flags1 & RF1_UNIQUE) continue;

        /* Collect "appropriate" monsters */
        if (l_ptr->pkills == 0) continue;

        mon_kills mon_body;

        mon_body.mon_idx = i;
        mon_body.total_kills = l_ptr->pkills;
        mon_kill_list.append(mon_body);

    }

    // Make sure they have killed something first
    if (!mon_kill_list.size())
    {
        pop_up_message_box("You have not yet killed any creatures.");
        return;
    }

    //bubble sort, largest kill count at top
    for (int i = 0; i < mon_kill_list.size(); i++)
    {
        for (int j = i+1; j < mon_kill_list.size(); j++)
        {
            if (mon_kill_list[i].total_kills >= mon_kill_list[j].total_kills) continue;

            mon_kills temp = mon_kill_list[j];
            mon_kill_list[j] = mon_kill_list[i];
            mon_kill_list[i] = temp;

        }
    }

    QVBoxLayout *main_layout = new QVBoxLayout;
    QGridLayout *mon_kill_info = new QGridLayout;

    QLabel *mon_label = new QLabel(QString("<b><big>Monster Kills by Race</big></b>"));
    mon_label->setAlignment(Qt::AlignCenter);

    main_layout->addWidget(mon_label);
    main_layout->addLayout(mon_kill_info);

    int row = 0;
    int col = 0;

    QLabel *mon_race = new QLabel("<b><u>Monster Race</u>  </b>");
    QLabel *num_kills = new QLabel("<b>  <u>Number of Kills</u>  </b>");
    mon_kill_info->addWidget(mon_race, row, col++, Qt::AlignLeft);
    mon_kill_info->addWidget(num_kills, row++, col++, Qt::AlignRight);

    // Print out all the monster races
    for (int i = 0; i < mon_kill_list.size(); i++)
    {
        row++;
        col = 0;
        mon_kills *mk_ptr = &mon_kill_list[i];
        QString this_mon_race = r_info[mk_ptr->mon_idx].r_name_full;
        if (mk_ptr->total_kills > 1) this_mon_race = plural_aux(this_mon_race);
        QLabel *this_race = new QLabel(this_mon_race);
        mon_kill_info->addWidget(this_race, row, col++, Qt::AlignLeft);
        QLabel *total_kills = new QLabel(number_to_formatted_string(mk_ptr->total_kills));
        mon_kill_info->addWidget(total_kills, row, col++, Qt::AlignRight);
    }

    //Add a close button on the right side
    QHBoxLayout *close_across = new QHBoxLayout;
    main_layout->addLayout(close_across);
    close_across->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, SIGNAL(rejected()), this, SLOT(close()));
    close_across->addWidget(buttons);

    setLayout(main_layout);
    setWindowTitle(tr("Monster Kill Count"));

    this->exec();
}


void display_mon_kill_count(void)
{
    DisplayMonKillCount();
}
