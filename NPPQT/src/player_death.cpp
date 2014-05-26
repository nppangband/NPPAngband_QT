/*
 * File: player_death.cpp
 *
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *                    Jeff Greene, Diego Gonzalez
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

#include "src/player_death.h"



void PlayerDeathDialog::death_info(void)
{
    //TODO death info
}

void PlayerDeathDialog::death_messsages(void)
{
    // TODO display messages
}

void PlayerDeathDialog::death_file_dump(void)
{
    // TODO make file dump
}

void PlayerDeathDialog::death_scores(void)
{
    // TODO display scores
}

void PlayerDeathDialog::death_examine(void)
{
    do_cmd_observe();
}

void PlayerDeathDialog::death_notes(void)
{
    // TODO display notes
}

void PlayerDeathDialog::death_spoilers(void)
{
    // TODO spoilers
}

PlayerDeathDialog::PlayerDeathDialog(void)
{
    QVBoxLayout *vlay = new QVBoxLayout;

    QLabel *obj_label = new QLabel(QString("<b><big>Press 'Close' when ready:</b></big>"));
    obj_label->setAlignment(Qt::AlignCenter);
    vlay->addWidget(obj_label);

    vlay->addStretch();

    // Add the "Information" button
    QPushButton *info_button = new QPushButton("Player Information");
    info_button->setToolTip("View player screen, equipment, inventory, and contents of player's home.");
    connect(info_button, SIGNAL(clicked()), this, SLOT(death_info()));
    vlay->addWidget(info_button);

    // Add the "Messages" button
    QPushButton *message_button = new QPushButton("Messages");
    message_button->setToolTip("View recent messages.");
    connect(message_button, SIGNAL(clicked()), this, SLOT(death_messages()));
    vlay->addWidget(message_button);

    // Add the "Character Dump" button
    QPushButton *file_dump_button = new QPushButton("Character Dump");
    file_dump_button->setToolTip("Create a final character dump.");
    connect(file_dump_button, SIGNAL(clicked()), this, SLOT(death_file_dump()));
    vlay->addWidget(file_dump_button);

    // Add the "Scores" button
    QPushButton *scores_button = new QPushButton("View Scores");
    scores_button->setToolTip("View the scores for all characters.");
    connect(scores_button, SIGNAL(clicked()), this, SLOT(death_scores()));
    vlay->addWidget(scores_button);

    // Add the "Examine" button
    QPushButton *examine_button = new QPushButton("Examine Items");
    examine_button->setToolTip("Examine Items in Player Inventory and Backpack.");
    connect(examine_button, SIGNAL(clicked()), this, SLOT(death_examine()));
    vlay->addWidget(examine_button);

    // Add the "Examine" button
    QPushButton *notes_button = new QPushButton("View Notes");
    notes_button->setToolTip("View a log of all notes from the game.");
    connect(notes_button, SIGNAL(clicked()), this, SLOT(death_notes()));
    vlay->addWidget(notes_button);

    // Add the "Spoilers" button
    QPushButton *spoilers_button = new QPushButton("Create Spoilers");
    spoilers_button->setToolTip("Create spoiler files.");
    connect(spoilers_button, SIGNAL(clicked()), this, SLOT(death_spoilers()));
    vlay->addWidget(spoilers_button);

    QPushButton *close_button = new QPushButton(tr("&Close"));
    connect(close_button, SIGNAL(clicked()), this, SLOT(close()));

    vlay->addStretch();
    vlay->addWidget(close_button);

    setLayout(vlay);
    setWindowTitle("Press 'Close' when done:");
    this->exec();
}

/*
 * Display the winner crown
 */
static void display_winner(void)
{
    //TODO - make a winner display screen
}

/*
 * Display the tombstone
 */
static void print_tomb(void)
{
    //TODO - make a splash screen
}


/*
 * Know inventory and home items upon death
 */
static void death_knowledge(void)
{
    store_type *st_ptr = &store[STORE_HOME];
    object_type *o_ptr;

    int i;

    /* Know everything in the inven/equip */
    for (i = 0; i < ALL_INVEN_TOTAL; i++)
    {
        o_ptr = &inventory[i];
        if (!o_ptr->k_idx) continue;

        object_aware(o_ptr);
        object_known(o_ptr);
    }

    /* Know everything in the home */
    for (i = 0; i < st_ptr->stock_num; i++)
    {
        o_ptr = &st_ptr->stock[i];
        if (!o_ptr->k_idx) continue;

        object_aware(o_ptr);
        object_known(o_ptr);

        /* Fully known */
        o_ptr->ident |= (IDENT_MENTAL);
    }

    /* Hack -- Recalculate bonuses */
    p_ptr->update |= (PU_BONUS);
    handle_stuff();
}

void player_death(void)
{
    /* Try to make a player ghost template */
    add_player_ghost_entry();

    /* Retire in the town in a good state */
    if (p_ptr->total_winner)
    {
        p_ptr->depth = 0;
        p_ptr->died_from = "Ripe Old Age";
        p_ptr->exp = p_ptr->max_exp;
        p_ptr->lev = p_ptr->max_lev;
        p_ptr->au += 10000000L;

        display_winner();
    }

    /* Save dead player */
    if (!save_player())
    {
        message(QString("death save failed!"));
    }

    //TODO automatic screenshot and character dump?
    QDate today = QDate::currentDate();
    QTime right_now = QTime::currentTime();
    QString long_day = QString("%1 at %2") .arg(today.toString() .arg(right_now.toString()));

    write_note(QString("%1 the %2 %3 was killed by %4 on %5.")
                   .arg(op_ptr->full_name) .arg(p_info[p_ptr->prace].pr_name) .arg(c_info[p_ptr->pclass].cl_name)
                   .arg(p_ptr->died_from) .arg(long_day), p_ptr->depth);

    print_tomb();
    death_knowledge();
    // TODO enter_score(&right_now);

    PlayerDeathDialog();
}