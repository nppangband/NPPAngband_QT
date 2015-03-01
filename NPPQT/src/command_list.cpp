
/* File: ommand_list.cpp */

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

#include <src/command_list.h>
#include <QDialogButtonBox>

static struct command_desc list_commands[] =
{
    {"Activate", "a"},
    {"Alter (set direction later)", "Z"},
    {"Bash Door or Monster", "ALT-D"},
    {"Browse Spellbooks", "CTRL-M"},
    {"Cast Magic Spells", "m"},
    {"Center Player", "ESC"},
    {"Close Door", "D (shift-d)"},
    {"Destroy Item", "x"},
    {"Disarm Trap", "o"},
    {"Drop Item", "d"},
    {"Fire Ammunition	", "f"},
    {"Fire At Nearest	", "F (shift-f)"},
    {"Go Down Staircase", "<"},
    {"Do Up Staircase	", ">"},
    {"Inscribe Item", "{"},
    {"Inspect All Objects", "i"},
    {"Inspect Object", "I (shift-i)"},
    {"Learn Magic Spells", "M (shift-m)"},
    {"Look", "l"},
    {"Make/Modify Trap", "O (shift-o)"},
    {"Message Log", "QTRL-L"},
    {"New Game NPPAngband", "CTRL-A"},
    {"New Game NPPMoria", "CTRL-R"},
    {"Open Door", "d"},
    {"Open Savefile", "CTRL-F"},
    {"Option Settings", "="},
    {"Pickup Items", "+"},
    {"Player Character Screen", "p"},
    {"Quest Desription", "q"},
    {"Quit NPP Games", "CTRL-Q"},
    {"Repeat Previous Command", "c"},
    {"Rest Until Fully Healed	", "r"},
    {"Rest To Recover All HP ", "CTRL-R"},
    {"Rest To Recover All SP", "ALT-R"},
    {"Rest To Recover All HP + SP", "(CTRL + ALT)-R"},
    {"Rest (set rest mode later)", "R (shift-r)"},
    {"Repeat Level Feeling", "ALT-F"},
    {"Run (set direction later)", "."},
    {"Save Character", "CTRL-S"},
    {"Save Character and Close Game", "CTRL-X"},
    {"Save Character As", "CTRL-W"},
    {"Search", "s"},
    {"Search (Toggle)", "S (shift-s)"},
    {"Spike A Door", "shift-q"},
    {"Swap Weapon", "W (shift-w)"},
    {"Take Off Item", "CTRL-W"},
    {"Throw Item", "k"},
    {"Tunnel (set direction later)", "ALT-O"},
    {"Uninscribe Item", "}"},
    {"Use Item", "e"},
    {"Wield Item", "w"},
    {"Wizard Mode (cheat)", "CTRL-a"},
    {"Write Note", ":"},
    // The null entry at the end is essential for initializing the table of groups.
    {NULL, NULL},
};

void CommandList::add_keyboard_commands(QGridLayout *return_layout)
{

    int x = 0;

    int command_count = 0;

    // Count the number of help commands
    while (TRUE)
    {
        command_desc *cmd_ptr = &list_commands[x++];

        // Null pointer means we are done
        if (!cmd_ptr->command_title.length()) break;

        command_count++;
    }

    command_count = (command_count / 3) + 1;

    x = 0;
    int row = 0;
    int col_count = 0;

    while (TRUE)
    {
        command_desc *cmd_ptr = &list_commands[x++];


        if (row == command_count)
        {
            col_count++;
            row = 0;
        }
        int col = col_count * 4;

        // Null pointer means we are done
        if (!cmd_ptr->command_title.length()) break;

        QLabel *this_title = new QLabel();
        make_standard_label(this_title, QString(cmd_ptr->command_title), TERM_BLUE);
        return_layout->addWidget(this_title, row, col++, Qt::AlignLeft);

        QLabel *dummy = new QLabel("  ");
        return_layout->addWidget(dummy, row, col++);

        QLabel *this_key = new QLabel();
        make_standard_label(this_key, QString(cmd_ptr->command_key), TERM_BLUE);
        return_layout->addWidget(this_key, row, col++, Qt::AlignLeft);

        if (col_count < 2)
        {
            QLabel *dummy = new QLabel("    ");
            return_layout->addWidget(dummy, row, col);
        }

        row++;
    }
}



static struct command_desc dir_commands[] =
{
    {"Hold", "'h', or '5'"},
    {"Walk", "direction key"},
    {"Walk With Flip Pickup", "(CTRL + ALT)direction key"},
    {"Run", "(CTRL)direction key"},
    {"Alter", "(ALT)direction key"},
    {"Look (Change Panel)", "(shift)direction key"},
    {"Tunnel (Specified Direction)", "(CTRL + SHIFT)direction key"},
    // The null entry at the end is essential for initializing the table of groups.
    {NULL, NULL},
};

void CommandList::add_dir_commands(QGridLayout *return_layout)
{
    int x = 0;

    while (TRUE)
    {
        command_desc *cmd_ptr = &dir_commands[x];

        int col = 0;

        // Null pointer means we are done
        if (!cmd_ptr->command_title.length()) break;

        QLabel *this_title = new QLabel();
        make_standard_label(this_title, QString(cmd_ptr->command_title), TERM_BLUE);
        return_layout->addWidget(this_title, x, col++, Qt::AlignLeft);

        QLabel *dummy = new QLabel("   ");
        return_layout->addWidget(dummy, x, col++);

        QLabel *this_key = new QLabel();
        make_standard_label(this_key, QString(cmd_ptr->command_key), TERM_BLUE);
        return_layout->addWidget(this_key, x++, col++, Qt::AlignLeft);
    }

    QLabel *dummy = new QLabel("   ");
    return_layout->addWidget(dummy, x, 0);
}




void CommandList::add_dir_keyboard(QVBoxLayout *return_layout, bool keyboard)
{
    QLabel *top_label = new QLabel;

    QString this_title  = "Keypad Dirs";
    if (keyboard) this_title = "Keyboard Dirs";
    make_standard_label(top_label, this_title, TERM_DARK);
    return_layout->addWidget(top_label, Qt::AlignCenter);

    QString letters = "789456123";
    if (keyboard) letters = "tyughjvbn";

    QGridLayout *dir_keyboard = new QGridLayout;
    return_layout->addLayout(dir_keyboard);

    int which_char = 0;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            QLabel *this_label = new QLabel();
            make_standard_label(this_label, QString(letters[which_char++]), TERM_BLUE);
            dir_keyboard->addWidget(this_label, j, i, Qt::AlignCenter);
        }
    }

    QLabel *dummy = new QLabel("   ");
    return_layout->addWidget(dummy, 3, 0);

    return_layout->addStretch(1);
}

CommandList::CommandList(void)
{

    QVBoxLayout *main_layout = new QVBoxLayout;

    QLabel *main_prompt = new QLabel(QString("<h2>Directional Commands</h2>"));
    main_layout->addWidget(main_prompt, Qt::AlignCenter);

    QHBoxLayout *top_across = new QHBoxLayout;
    main_layout->addLayout(top_across);

    QVBoxLayout *vlay_key_dirs = new QVBoxLayout;
    add_dir_keyboard(vlay_key_dirs, TRUE);
    top_across->addLayout(vlay_key_dirs);
    top_across->addStretch(1);

    QVBoxLayout *vlay_pad_dirs = new QVBoxLayout;
    add_dir_keyboard(vlay_pad_dirs, FALSE);
    top_across->addLayout(vlay_pad_dirs);
    top_across->addStretch(1);

    QGridLayout *glay_dir_commands = new QGridLayout;
    add_dir_commands(glay_dir_commands);
    main_layout->addLayout(glay_dir_commands);

    QLabel *keyboard_prompt = new QLabel(QString("<h2>Keyboard Commands</h2>"));
    main_layout->addWidget(keyboard_prompt, Qt::AlignCenter);

    QGridLayout *glay_key_commands = new QGridLayout;
    add_keyboard_commands(glay_key_commands);
    main_layout->addLayout(glay_key_commands);

    QDialogButtonBox buttons;
    buttons.setStandardButtons(QDialogButtonBox::Ok);
    connect(&buttons, SIGNAL(accepted()), this, SLOT(close()));
    main_layout->addWidget(&buttons);

    setLayout(main_layout);
    setWindowTitle(tr("Command List"));

    this->exec();
}

void do_cmd_command_list(void)
{
    CommandList();
}
