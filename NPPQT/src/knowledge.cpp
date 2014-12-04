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
#include <src/player_scores.h>
#include <src/store.h>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

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

DisplayScores::DisplayScores(void)
{
    QVBoxLayout *main_layout = new QVBoxLayout;
    QGridLayout *all_scores = new QGridLayout;

    QLabel *score_label = new QLabel(QString("<h1><b>All Player Scores</b></h1>"));
    score_label->setAlignment(Qt::AlignCenter);
    main_layout->addWidget(score_label);
    main_layout->addLayout(all_scores);

    //Copy the vector, add the player and sort it.
    QVector<high_score> score_list;
    for (int i = 0; i < player_scores_list.size(); i++)
    {
        score_list.append(player_scores_list[i]);
    }

    if (!p_ptr->is_wizard)
    {
        high_score player_current = build_score("Still Alive");
        player_current.death_how = QString("nobody (yet)!");
        score_list.append(player_current);
    }
    if (!score_list.size())
    {
        pop_up_message_box("There are no player scores yet.");
        return;
    }

    for (int i = 0; i < score_list.size(); i++)
    {
        for (int j = i+1; j < score_list.size(); j++)
        {
            if (score_list[i].score >= score_list[j].score) continue;

            high_score temp = score_list[j];
            score_list[j] = score_list[i];
            score_list[i] = temp;

        }
    }
    int row = 0;
    int col = 0;

    QLabel *header_num = new QLabel(QString("<big><b>Index  </b></big>"));
    all_scores->addWidget(header_num, row, col++, Qt::AlignLeft);
    QLabel *header_score = new QLabel(QString("<big><b>  Player Score  </b></big>"));
    all_scores->addWidget(header_score, row, col++, Qt::AlignRight);
    QLabel *header_basic = new QLabel(QString("<big><b>Character</b></big>"));
    all_scores->addWidget(header_basic, row, col++, Qt::AlignCenter);
    QLabel *header_status = new QLabel(QString("<big><b>  Killed By </b></big>"));
    all_scores->addWidget(header_status, row, col++, Qt::AlignLeft);
    QLabel *header_level = new QLabel(QString("<big><b>  Level</b></big>"));
    all_scores->addWidget(header_level, row, col++, Qt::AlignLeft);
    QLabel *header_exp = new QLabel(QString("<big><b>  Experience </b></big>"));
    all_scores->addWidget(header_exp, row, col++, Qt::AlignRight);
    QLabel *header_turns = new QLabel(QString("<big><b>  Game Turns </b></big>"));
    all_scores->addWidget(header_turns, row, col++, Qt::AlignRight);
    QLabel *header_fame = new QLabel(QString("<big><b>  Fame </b></big>"));
    all_scores->addWidget(header_fame, row, col++, Qt::AlignRight);
    QLabel *header_version = new QLabel(QString("<big><b>  Version </b></big>"));
    all_scores->addWidget(header_version, row, col++, Qt::AlignLeft);

    // Print out all the scores
    for (int i = 0; i < score_list.size(); i++)
    {
        high_score *score_ptr = &score_list[i];
        row++;
        col = 0;

        // Entry number
        QLabel *entry_num = new QLabel(QString("%1)  ") .arg(number_to_letter(i)));
        all_scores->addWidget(entry_num, row, col++, Qt::AlignLeft | Qt::AlignTop);

        // Score
        QLabel *entry_score = new QLabel(QString("%1  ") .arg(number_to_formatted_string(score_ptr->score)));
        all_scores->addWidget(entry_score, row, col++, Qt::AlignRight | Qt::AlignTop);

        // Player name, race, class
        QLabel *entry_basic = new QLabel("basic");
        entry_basic->setText(QString("%1 the %2 %3 (%4)  ")
                          .arg(score_ptr->p_name) .arg(score_ptr->p_race) .arg(score_ptr->p_class) .arg(score_ptr->p_sex));
        entry_basic->setWordWrap(TRUE);
        all_scores->addWidget(entry_basic, row, col++, Qt::AlignLeft | Qt::AlignTop);

        //Player Status
        QString died_by = (QString(" %1 ") .arg(score_ptr->death_how));
        if (score_ptr->cur_depth)
        {
            died_by.append(QString("on dungeon level %1 ") .arg(number_to_formatted_string(score_ptr->cur_depth * 50) ));
        }
        else died_by.append("in the town ");

        if (score_ptr->cur_depth != score_ptr->max_depth)
        {
            died_by.append(color_string((QString(" (Max Depth %1) ") .arg(score_ptr->max_depth)), TERM_BLUE));
        }

        died_by.append(QString(" %1.     ") .arg(score_ptr->date_time));
        QLabel *death_info = new QLabel(died_by);
        death_info->setWordWrap(TRUE);
        all_scores->addWidget(death_info, row, col++, Qt::AlignLeft | Qt::AlignTop);

        // Player Level
        QString level = (QString("%1") .arg(score_ptr->cur_level));
        if (score_ptr->max_level != score_ptr->cur_level)
        {
            level.append(color_string((QString("  Max Level %1") .arg(score_ptr->max_level)), TERM_BLUE));
        }
        QLabel *entry_level = new QLabel(level);
        all_scores->addWidget(entry_level, row, col++, Qt::AlignRight | Qt::AlignTop);

        // Player Experience
        QString experience = (QString("%1  ") .arg(number_to_formatted_string(score_ptr->cur_exp)));
        if (score_ptr->max_exp != score_ptr->cur_exp)
        {
            experience.append(color_string((QString("  Max Exp %1  ") .arg(number_to_formatted_string(score_ptr->max_exp))), TERM_BLUE));
        }
        QLabel *entry_exp = new QLabel(experience);
        all_scores->addWidget(entry_exp, row, col++, Qt::AlignRight | Qt::AlignTop);

        // Turns
        QLabel *entry_turns = new QLabel(QString("%1  ") .arg(number_to_formatted_string(score_ptr->turns)));
        all_scores->addWidget(entry_turns, row, col++, Qt::AlignRight);

        // Fame
        QLabel *entry_fame = new QLabel(QString("%1  ") .arg(number_to_formatted_string(score_ptr->fame)));
        all_scores->addWidget(entry_fame, row, col++, Qt::AlignRight | Qt::AlignTop);

        // Version
        QLabel *entry_version = new QLabel(score_ptr->version);
        all_scores->addWidget(entry_version, row, col++, Qt::AlignLeft | Qt::AlignTop);

    }

    //Add a close button on the right side
    QHBoxLayout *close_across = new QHBoxLayout;
    main_layout->addLayout(close_across);
    close_across->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, SIGNAL(rejected()), this, SLOT(close()));
    close_across->addWidget(buttons);

    setLayout(main_layout);
    setWindowTitle(tr("Player Scores"));

    this->exec();
}

void display_player_scores(void)
{
    DisplayScores();
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
