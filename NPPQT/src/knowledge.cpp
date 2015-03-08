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
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>




DisplayNotesFile::DisplayNotesFile(void)
{
    QVBoxLayout *main_layout = new QVBoxLayout;
    QGridLayout *notes_info = new QGridLayout;

    main_layout->addLayout(notes_info);

    QFontMetrics metrics(ui_current_font());
    QSize this_size = metrics.size(Qt::TextSingleLine, "MMMMMMMMMMMMMMMMM");

    int row = 0;
    int col = 0;

    QLabel *header_turn = new QLabel("<b><u>GAME TURN</u>  </b>");
    QLabel *header_depth = new QLabel("<b>  <u>DUNGEON DEPTH</u>  </b>");
    QLabel *header_level = new QLabel("<b>  <u>PLAYER LEVEL</u>  </b>");
    QLabel *header_event = new QLabel("<b>  <u>EVENT</u>  </b>");
    header_event->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    header_event->setMinimumWidth(this_size.width() * 2);
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
        QLabel *player_level = new QLabel(QString("%1 ") .arg(notes_ptr->player_level));
        notes_info->addWidget(player_level, row, col++, Qt::AlignRight | Qt::AlignTop);
        QLabel *game_event = new QLabel(notes_ptr->recorded_note);
        game_event->setWordWrap(TRUE);
        game_event->setMinimumWidth(this_size.width() * 2);
        notes_info->addWidget(game_event, row, col++, Qt::AlignLeft | Qt::AlignTop);

    }

    main_layout->addStretch(1);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, SIGNAL(rejected()), this, SLOT(close()));
    main_layout->addWidget(buttons);

    setLayout(main_layout);
    setWindowTitle(tr("Notes and Accomplishments"));

    this->exec();
}

void display_notes_file(void)
{
    // Paranoia
    if (!p_ptr->playing) return;

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
    QDialogButtonBox buttons;
    buttons.setStandardButtons(QDialogButtonBox::Close);
    connect(&buttons, SIGNAL(rejected()), this, SLOT(close()));
    main_layout->addWidget(&buttons);

    setLayout(main_layout);
    setWindowTitle(tr("Home Inventory"));

    this->exec();
}

void display_home_inventory(void)
{
    // Paranoia
    if (!p_ptr->playing) return;

    DisplayHomeInven();
}



DisplayScores::DisplayScores(void)
{
    scores_proxy_model = new QSortFilterProxyModel;
    scores_proxy_model->setSortCaseSensitivity(Qt::CaseSensitive);
    QVBoxLayout *main_layout = new QVBoxLayout;

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


    int col = 0;

    //Set up the headers
    scores_table = new QTableWidget(0, 8, this);
    scores_table->setAlternatingRowColors(TRUE);

    QTableWidgetItem *score_header = new QTableWidgetItem("Player Score");
    score_header->setTextAlignment(Qt::AlignRight);
    scores_table->setHorizontalHeaderItem(col++, score_header);
    QTableWidgetItem *character_header = new QTableWidgetItem("Character");
    character_header->setTextAlignment(Qt::AlignLeft);
    scores_table->setHorizontalHeaderItem(col++, character_header);
    QTableWidgetItem *killed_by_header = new QTableWidgetItem("Killed By");
    killed_by_header->setTextAlignment(Qt::AlignLeft);
    scores_table->setHorizontalHeaderItem(col++, killed_by_header);
    QTableWidgetItem *level_header = new QTableWidgetItem("Level");
    level_header->setTextAlignment(Qt::AlignRight);
    scores_table->setHorizontalHeaderItem(col++, level_header);
    QTableWidgetItem *exp_header = new QTableWidgetItem("Experience");
    exp_header->setTextAlignment(Qt::AlignRight);
    scores_table->setHorizontalHeaderItem(col++, exp_header);
    QTableWidgetItem *turn_header = new QTableWidgetItem("Game Turns");
    turn_header->setTextAlignment(Qt::AlignRight);
    scores_table->setHorizontalHeaderItem(col++, turn_header);
    QTableWidgetItem *fame_header = new QTableWidgetItem("Fame");
    fame_header->setTextAlignment(Qt::AlignRight);
    scores_table->setHorizontalHeaderItem(col++, fame_header);
    QTableWidgetItem *version_header = new QTableWidgetItem("Version");
    version_header->setTextAlignment(Qt::AlignLeft);
    scores_table->setHorizontalHeaderItem(col++, version_header);

    // Add the data
    for (int i = 0; i < score_list.size(); i++)
    {
        high_score *score_ptr = &score_list[i];
        col = 0;
        scores_table->insertRow(i);

        // Score
        QTableWidgetItem *score = new QTableWidgetItem(QString("%1") .arg(score_ptr->score));
        score->setTextAlignment(Qt::AlignRight);
        scores_table->setItem(i, col++, score);

        // Player name, race, class, gender
        QString player_name = (QString("%1 the %2 %3 (%4)  ")
                          .arg(score_ptr->p_name) .arg(score_ptr->p_race)
                          .arg(score_ptr->p_class) .arg(score_ptr->p_sex));
        QTableWidgetItem *pl_name = new QTableWidgetItem(player_name);
        pl_name->setTextAlignment(Qt::AlignLeft);
        scores_table->setItem(i, col++, pl_name);


        // Killed by and dungeon depth
        QString died_by = (QString(" %1 ") .arg(score_ptr->death_how));
        if (score_ptr->cur_depth)
        {
            died_by.append(QString("on dungeon level %1 ") .arg(score_ptr->cur_depth * 50));
        }
        else died_by.append("in the town ");

        if (score_ptr->cur_depth != score_ptr->max_depth)
        {
            died_by.append(QString(" (Max Depth %1) ") .arg(score_ptr->max_depth));
        }
        QTableWidgetItem *killed_by = new QTableWidgetItem(died_by);
        killed_by->setTextAlignment(Qt::AlignLeft);
        scores_table->setItem(i, col++, killed_by);

        // Player level
        QString level_str = (QString("%1") .arg(score_ptr->cur_level));
        if (score_ptr->max_level != score_ptr->cur_level)
        {
            level_str.append(QString("  Max Level %1") .arg(score_ptr->max_level));
        }
        QTableWidgetItem *level = new QTableWidgetItem(level_str);
        level->setTextAlignment(Qt::AlignRight);
        scores_table->setItem(i, col++, level);

        // Player Experience
        QString experience = (QString("%1  ").arg(score_ptr->cur_exp));
        if (score_ptr->max_exp != score_ptr->cur_exp)
        {
            experience.append(color_string((QString("  Max Exp %1  ") .arg(score_ptr->max_exp)), TERM_BLUE));
        }
        QTableWidgetItem *exp = new QTableWidgetItem(QString("%1") .arg(experience));
        exp->setTextAlignment(Qt::AlignRight);
        scores_table->setItem(i, col++, exp);

        // Turns
        QTableWidgetItem *turns = new QTableWidgetItem(QString("%1") .arg(score_ptr->turns));
        turns->setTextAlignment(Qt::AlignRight);
        scores_table->setItem(i, col++, turns);

        // Fame
        QTableWidgetItem *fame = new QTableWidgetItem(QString("%1") .arg(score_ptr->fame));
        fame->setTextAlignment(Qt::AlignRight);
        scores_table->setItem(i, col++, fame);

        // Version
        QTableWidgetItem *version = new QTableWidgetItem(score_ptr->version);
        version->setTextAlignment(Qt::AlignLeft);
        scores_table->setItem(i, col++, version);
    }

    scores_table->setSortingEnabled(TRUE);
    scores_table->resizeColumnsToContents();
    scores_table->sortByColumn(0, Qt::DescendingOrder);
    scores_table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    main_layout->addWidget(scores_table);
    scores_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //Add a close button on the right side
    QDialogButtonBox buttons;
    buttons.setStandardButtons(QDialogButtonBox::Close);
    connect(&buttons, SIGNAL(rejected()), this, SLOT(close()));
    main_layout->addWidget(&buttons);

    resize(QSize(width() * 2, height() * 4 / 3));

    setLayout(main_layout);
    setWindowTitle("Player Scores");

    this->exec();
}

void display_player_scores(void)
{
    // Paranoia
    if (!p_ptr->playing) return;

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

    kill_count_proxy_model = new QSortFilterProxyModel;
    kill_count_proxy_model->setSortCaseSensitivity(Qt::CaseSensitive);
    QVBoxLayout *main_layout = new QVBoxLayout;

    int col = 0;

    //Set up the headers
    kill_count_table = new QTableWidget(0, 4, this);
    kill_count_table->setAlternatingRowColors(FALSE);

    QTableWidgetItem *race_header = new QTableWidgetItem("Monster Race");
    race_header->setTextAlignment(Qt::AlignLeft);
    kill_count_table->setHorizontalHeaderItem(col++, race_header);
    QTableWidgetItem *symbol_header = new QTableWidgetItem("Symbol");
    symbol_header->setTextAlignment(Qt::AlignCenter);
    kill_count_table->setHorizontalHeaderItem(col++, symbol_header);
    QTableWidgetItem *depth_header = new QTableWidgetItem("Native Depth");
    depth_header->setTextAlignment(Qt::AlignRight);
    kill_count_table->setHorizontalHeaderItem(col++, depth_header);
    QTableWidgetItem *kills_header = new QTableWidgetItem("Total Kills");
    kills_header->setTextAlignment(Qt::AlignRight);
    kill_count_table->setHorizontalHeaderItem(col++, kills_header);

    // Add the data
    for (int i = 0; i < mon_kill_list.size(); i++)
    {
        mon_kills *mk_ptr = &mon_kill_list[i];
        monster_race *r_ptr = &r_info[mk_ptr->mon_idx];
        col = 0;
        kill_count_table->insertRow(i);

        // Race
        QString this_mon_race = r_ptr->r_name_full;
        if (mk_ptr->total_kills > 1) this_mon_race = plural_aux(this_mon_race);
        this_mon_race = capitalize_first(this_mon_race);
        QTableWidgetItem *race = new QTableWidgetItem(this_mon_race);
        race->setTextAlignment(Qt::AlignLeft);
        kill_count_table->setItem(i, col++, race);

        // Symbol
        QString mon_symbol = (QString("'%1'") .arg(r_ptr->d_char));
        QTableWidgetItem *mon_ltr = new QTableWidgetItem(mon_symbol);
        mon_ltr->setTextColor(r_ptr->d_color);
        mon_ltr->setTextAlignment(Qt::AlignCenter);
        kill_count_table->setItem(i, col++, mon_ltr);

        // dungeon depth
        QString mon_level = (QString("%1'") .arg(r_ptr->level * 50));
        if (!r_ptr->level) mon_level = QString("Town");
        QTableWidgetItem *mon_lvl = new QTableWidgetItem(mon_level);
        mon_lvl->setTextAlignment(Qt::AlignRight);
        kill_count_table->setItem(i, col++, mon_lvl);

        // Monster Kills
        QTableWidgetItem *total_kills = new QTableWidgetItem(QString("%1") .arg(mk_ptr->total_kills));
        total_kills->setTextAlignment(Qt::AlignRight);
        kill_count_table->setItem(i, col++, total_kills);
    }

    kill_count_table->setSortingEnabled(FALSE);
    kill_count_table->resizeColumnsToContents();
    kill_count_table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    main_layout->addWidget(kill_count_table);
    kill_count_table->setEditTriggers(QAbstractItemView::NoEditTriggers);


    //Add a close button on the right side
    QDialogButtonBox buttons;
    buttons.setStandardButtons(QDialogButtonBox::Close);
    connect(&buttons, SIGNAL(rejected()), this, SLOT(close()));
    main_layout->addWidget(&buttons);

    resize(QSize(width(), height() * 4 / 3));

    setLayout(main_layout);
    setWindowTitle(QString("Monster Kill Count"));

    this->exec();
}


void display_mon_kill_count(void)
{
    // Paranoia
    if (!p_ptr->playing) return;

    DisplayMonKillCount();
}
