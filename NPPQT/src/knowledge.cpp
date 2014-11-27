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
