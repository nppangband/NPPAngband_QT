
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
#include <src/cmds.h>
#include <src/player_command.h>
#include <src/object_all_menu.h>
#include <src/messages.h>

static struct command_desc list_commands_new[] =
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
    {"Fire Ammunition",	"f"},
    {"Fire At Nearest", "F (shift-f)"},
    {"Go Down Staircase", "<"},
    {"Do Up Staircase	", ">"},
    {"Inscribe Item", "{"},
    {"Inspect All Objects", "i"},
    {"Inspect Object", "I (shift-i)"},
    {"Jam a Door", "Q (shift-q}"},
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
    {"Repeat Previous Command", "'c' or '0'"},
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
    {"Take Notes", ":"},
    {"Take Off Item", "CTRL-W"},
    {"Target Closest", "*"},
    {"Terminate Character", "(CTRL + ALT)-Q"},
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

static struct command_desc list_commands_angband[] =
{
    {"Activate", "A (shift-a)"},
    {"Aim a Wand", "a"},
    {"Alter", "+"},
    {"Bash Door or Monster", "B (shift-b)"},
    {"Browse Spellbooks", "b"},
    {"Cast Magic Spells", "m"},
    {"Center Player", "ESC, or L (shift-l)"},
    {"Close Door", "c"},
    {"Destroy Item", "k"},
    {"Disarm Trap", "D (shift-d)"},
    {"Drop Item", "d"},
    {"Eat Food", "E (shift-e)"},
    {"Equipment (view", "e"},
    {"Fire Ammunition",	"f"},
    {"Fire At Nearest",	"h"},
    {"Fuel Lantern/Torch", "F (shift-f)"},
    {"Go Down Staircase", "<"},
    {"Go Up Staircase	", ">"},
    {"Hold", "'_' or ','"},
    {"Inscribe Item", "{"},
    {"Inspect Inventory", "i"},
    {"Inspect Object", "I (shift-i)"},
    {"Jam a Door", "j"},
    {"Learn Magic Spells", "G (shift-g)"},
    {"Look", "l"},
    {"Make/Modify Trap", "O (shift-o)"},
    {"Message Log", "QTRL-P"},
    {"New Game NPPAngband", "CTRL-A"},
    {"New Game NPPMoria", "CTRL-R"},
    {"Open Door", "o"},
    {"Open Savefile", "CTRL-F"},
    {"Option Settings", "="},
    {"Pickup Items", "g"},
    {"Player Character Screen", "C (shift c)"},
    {"Pray a prayer", "p"},
    {"Quaff a Potion", "q"},
    {"Quest Desription", "ALT-Q"},
    {"Quit NPP Games", "CTRL-Q"},
    {"Read a Scroll", "r"},
    {"Repeat Previous Command", "'n' or '0'"},
    {"Rest", "R (shift-r)"},
    {"Repeat Level Feeling", "CTRL-F"},
    {"Run", "."},
    {"Save Character", "CTRL-S"},
    {"Save Character and Close Game", "CTRL-X"},
    {"Save Character As", "CTRL-W"},
    {"Search", "s"},
    {"Search (Toggle)", "S (shift-s)"},
    {"Spike A Door", "j"},
    {"Swap Weapon", "x"},
    {"Take Notes", ":"},
    {"Take Off Item", "t"},
    {"Target CLosest", "'*'' or Apostrophe"},
    {"Terminate Character", "(shift-Q)"},
    {"Throw Item", "v"},
    {"Tunnel", "T (shift-t)"},
    {"Uninscribe Item", "}"},
    {"Use a Staff", "u"},
    {"Walk (flip pickup)","-"},
    {"Walk (normal pickup)","-"},
    {"Wield Item", "w"},
    {"Wizard Mode (cheat)", "CTRL-a"},
    {"Write Note", ":"},
    {"Zap a Rod", "z"},
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
        command_desc *cmd_ptr;
        if (which_keyset == KEYSET_NEW) cmd_ptr = &list_commands_new[x++];
        else if (which_keyset == KEYSET_ANGBAND) cmd_ptr = &list_commands_angband[x++];

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
        command_desc *cmd_ptr;
        if (which_keyset == KEYSET_NEW) cmd_ptr = &list_commands_new[x++];
        else if (which_keyset == KEYSET_ANGBAND) cmd_ptr = &list_commands_angband[x++];

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



static struct command_desc dir_commands_new[] =
{
    {"Hold", "'h', or '5'"},
    {"Walk", "direction key"},
    {"Walk With Flip Pickup", "(CTRL + ALT)direction key"},
    {"Run", "(Shift)direction key"},
    {"Alter", "(ALT)direction key"},
    {"Look (Change Panel)", "(CTRL)direction key"},
    {"Tunnel (Specified Direction)", "(CTRL + SHIFT)direction key"},
    // The null entry at the end is essential for initializing the table of groups.
    {NULL, NULL},
};

static struct command_desc dir_commands_angband[] =
{
    {"Run", "(Shift)direction key"},
    {"Alter", "(ALT)direction key"},
    // The null entry at the end is essential for initializing the table of groups.
    {NULL, NULL},
};

void CommandList::add_dir_commands(QGridLayout *return_layout)
{
    int x = 0;

    while (TRUE)
    {
        command_desc *cmd_ptr;
        if (which_keyset == KEYSET_NEW) cmd_ptr = &dir_commands_new[x++];
        else if (which_keyset == KEYSET_ANGBAND) cmd_ptr = &dir_commands_angband[x++];

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

    if (which_keyset != KEYSET_ANGBAND)
    {
        QVBoxLayout *vlay_pad_dirs = new QVBoxLayout;
        add_dir_keyboard(vlay_pad_dirs, FALSE);
        top_across->addLayout(vlay_pad_dirs);
        top_across->addStretch(1);
    }

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

static void process_mov_key(int dir, bool shift_key, bool alt_key, bool ctrl_key, bool meta_key)
{
    if (!character_dungeon) return;
    (void)meta_key;

    // Flip pickup
    if (ctrl_key && alt_key) do_cmd_walk(dir, TRUE);
    else if (ctrl_key && shift_key) do_cmd_tunnel_dir(dir);
    else if (ctrl_key) ui_change_panel(dir);
    else if (alt_key) do_cmd_alter(dir);
    else if (shift_key)  do_cmd_run(dir);
    else if (meta_key) do_cmd_tunnel_dir(dir);
    else do_cmd_walk(dir, FALSE);
}

void commands_new_keyset(QKeyEvent* which_key, bool shift_key, bool alt_key, bool ctrl_key, bool meta_key)
{
    bool using_mods = FALSE;
    if (shift_key || alt_key || ctrl_key || meta_key) using_mods = TRUE;

    // Normal mode
    switch (which_key->key())
    {
        // ESCAPE
        case Qt::Key_Escape:
        {
            ui_center(p_ptr->py, p_ptr->px);
            break;
        }

        // Move down
        case Qt::Key_2:
        case Qt::Key_Down:
        case Qt::Key_B:
        {
            process_mov_key(2, shift_key, alt_key, ctrl_key, meta_key);
            break;
        }

        // Move up
        case Qt::Key_8:
        case Qt::Key_Up:
        case Qt::Key_Y:
        {
            process_mov_key(8, shift_key, alt_key, ctrl_key, meta_key);
            break;
        }

        // Move left
        case Qt::Key_4:
        case Qt::Key_Left:
        case Qt::Key_G:
        {
            process_mov_key(4, shift_key, alt_key, ctrl_key, meta_key);
            break;
        }
        // Move right
        case Qt::Key_6:
        case Qt::Key_Right:
        case Qt::Key_J:
        {
            process_mov_key(6, shift_key, alt_key, ctrl_key, meta_key);
            break;
        }
        // Move diagonally left and up
        case Qt::Key_7:
        case Qt::Key_T:
        case Qt::Key_Home:
        {
            process_mov_key(7, shift_key, alt_key, ctrl_key, meta_key);
            break;
        }
        // Move diagonally right and up
        case Qt::Key_9:
        case Qt::Key_U:
        case Qt::Key_PageUp:
        {
            process_mov_key(9, shift_key, alt_key, ctrl_key, meta_key);
            break;
        }
        // Move diagonally left and down
        case Qt::Key_1:
        case Qt::Key_V:
        case Qt::Key_End:
        {
            process_mov_key(1, shift_key, alt_key, ctrl_key, meta_key);
            break;
        }
        // Move diagonally right and down
        case Qt::Key_3:
        case Qt::Key_N:
        case Qt::Key_PageDown:
        {
            process_mov_key(3, shift_key, alt_key, ctrl_key, meta_key);
            break;
        }
        case Qt::Key_5:
        case Qt::Key_H:
        {
            do_cmd_hold();
            break;
        }
        case Qt::Key_A:
        {
            if (ctrl_key)           do_cmd_wizard_mode();
            else if (!using_mods)   do_cmd_activate();
            break;
        }
        case Qt::Key_C:
        case Qt::Key_0:
        {
            do_cmd_repeat();
            break;
        }
        case Qt::Key_D:
        {
            if (alt_key)            do_cmd_bash();
            if (shift_key)          do_cmd_close();
            else if (!using_mods)   do_cmd_open();
            break;
        }
        case Qt::Key_E:
        {
            if (!using_mods)        do_cmd_use_item();
            break;
        }
        case Qt::Key_F:
        {
            if (alt_key)            do_cmd_feeling();
            else if (shift_key)     do_cmd_fire_at_nearest();
            else if (!using_mods)   do_cmd_fire();
            break;
        }
        case Qt::Key_I:
        {
            if (shift_key)          do_cmd_examine();
            else if (!using_mods)   do_cmd_all_objects(TAB_INVEN);
            break;
        }
        case Qt::Key_K:
        {
            if (!using_mods)        do_cmd_throw();
            break;
        }
        case Qt::Key_L:
        {
            if (shift_key)          do_cmd_refuel();
            else if (!using_mods)   do_cmd_look();
            break;
        }
        case Qt::Key_M:
        {
            if (shift_key)          do_cmd_study(-1);
            else if (ctrl_key)      do_cmd_browse(-1);
            else if (!using_mods)   do_cmd_cast(-1);
            break;
        }
        case Qt::Key_O:
        {
            if (alt_key)            do_cmd_tunnel();
            else if (shift_key)     do_cmd_make_trap();
            else if (!using_mods)   do_cmd_disarm();
            break;
        }
        case Qt::Key_P:
        {
            if (!using_mods)        do_cmd_player_screen();
            break;
        }
        case Qt::Key_Q:
        {
            if (ctrl_key && alt_key)do_cmd_suicide();
            if (shift_key)          do_cmd_spike();
            else if (!using_mods)   do_cmd_quest_desc();
            break;
        }
        case Qt::Key_R:
        {
            if (alt_key && ctrl_key)do_cmd_rest_specific(REST_BOTH_SP_HP);
            else if (ctrl_key)      do_cmd_rest_specific(REST_HP);
            else if (alt_key)       do_cmd_rest_specific(REST_SP);
            else if (shift_key)     do_cmd_rest();
            else if (!using_mods)   do_cmd_rest_specific(REST_COMPLETE);
            break;
        }
        case Qt::Key_S:
        {
            if (shift_key)          do_cmd_toggle_search();
            else if (!using_mods)   do_cmd_search();
            break;
        }
        case Qt::Key_W:
        {
            if (ctrl_key)           do_cmd_takeoff();
            else if (shift_key)     do_cmd_swap_weapon();
            else if (!using_mods)   do_cmd_wield();
            break;
        }
        case Qt::Key_X:
        {
            if (!using_mods)        do_cmd_destroy();
            break;
        }
        case Qt::Key_Z:
        {
            if (!using_mods)        do_cmd_alter(DIR_UNKNOWN);
            break;
        }
        case Qt::Key_Asterisk:
        {
            target_set_closest(TARGET_KILL);
            break;
        }
        case Qt::Key_BraceLeft:
        {
            do_cmd_inscribe();
            break;
        }
        case Qt::Key_BraceRight:
        {
            do_cmd_uninscribe();
            break;
        }
        case Qt::Key_Greater:
        {
            do_cmd_go_down();
            break;
        }
        case Qt::Key_Less:
        {
            do_cmd_go_up();
            break;
        }
        case Qt::Key_Period:
        {
            do_cmd_run(DIR_UNKNOWN);
            break;
        }
        case Qt::Key_Plus:
        {
            do_cmd_pickup();
            break;
        }
        case Qt::Key_Minus:
        {
            do_cmd_drop();
            break;
        }
        case Qt::Key_Colon:
        {
            do_cmd_write_note();
            break;
        }
        default:
        {
            break;
        }
    }
}

void commands_angband_keyset(QKeyEvent* which_key, bool shift_key, bool alt_key, bool ctrl_key, bool meta_key)
{
    bool using_mods = FALSE;
    if (shift_key || alt_key || ctrl_key || meta_key) using_mods = TRUE;

    // Normal mode
    switch (which_key->key())
    {
        // ESCAPE
        case Qt::Key_Escape:
        {
            ui_center(p_ptr->py, p_ptr->px);
            break;
        }

        // Move down
        case Qt::Key_2:
        case Qt::Key_Down:
        {
            process_mov_key(2, shift_key, alt_key, ctrl_key, meta_key);
            break;
        }

        // Move up
        case Qt::Key_8:
        case Qt::Key_Up:
        {
            process_mov_key(8, shift_key, alt_key, ctrl_key, meta_key);
            break;
        }

        // Move left
        case Qt::Key_4:
        case Qt::Key_Left:
        {
            process_mov_key(4, shift_key, alt_key, ctrl_key, meta_key);
            break;
        }
        // Move right
        case Qt::Key_6:
        case Qt::Key_Right:
        {
            process_mov_key(6, shift_key, alt_key, ctrl_key, meta_key);
            break;
        }
        // Move diagonally left and up
        case Qt::Key_7:
        case Qt::Key_Home:
        {
            process_mov_key(7, shift_key, alt_key, ctrl_key, meta_key);
            break;
        }
        // Move diagonally right and up
        case Qt::Key_9:
        case Qt::Key_PageUp:
        {
            process_mov_key(9, shift_key, alt_key, ctrl_key, meta_key);
            break;
        }
        // Move diagonally left and down
        case Qt::Key_1:
        case Qt::Key_End:
        {
            process_mov_key(1, shift_key, alt_key, ctrl_key, meta_key);
            break;
        }
        // Move diagonally right and down
        case Qt::Key_3:
        case Qt::Key_PageDown:
        {
            process_mov_key(3, shift_key, alt_key, ctrl_key, meta_key);
            break;
        }
        case Qt::Key_5:
        {
            do_cmd_hold();
            break;
        }
        case Qt::Key_A:
        {
            if (shift_key)     do_cmd_activate();
            else if (!using_mods)   do_cmd_aim_wand();
            break;
        }
        case Qt::Key_B:
        {
            if (shift_key)          do_cmd_bash();
            else if (!using_mods)   do_cmd_browse(-1);
            break;
        }
        case Qt::Key_C:
        {
            if (shift_key)          do_cmd_player_screen();
            else if (!using_mods)   do_cmd_close();
            break;
        }
        case Qt::Key_D:
        {
            if (shift_key)          do_cmd_disarm();
            else if (!using_mods)   do_cmd_drop();
            break;
        }
        case Qt::Key_E:
        {
            if (shift_key)          do_cmd_all_objects(TAB_EQUIP);
            if (!using_mods)        do_cmd_eat_food();
            break;
        }
        case Qt::Key_F:
        {
            if (ctrl_key)           do_cmd_feeling();
            else if (shift_key)     do_cmd_refuel();
            else if (!using_mods)   do_cmd_fire();
            break;
        }
        case Qt::Key_G:
        {
            if (shift_key)          do_cmd_study(-1);
            else if (!using_mods)   do_cmd_pickup_from_pile(FALSE, TRUE);
            break;
        }
        case Qt::Key_H:
        {
            if (!using_mods)        do_cmd_fire_at_nearest();
            break;
        }
        case Qt::Key_I:
        {
            if (shift_key)          do_cmd_examine();
            else if (!using_mods)   do_cmd_all_objects(TAB_EQUIP);
            break;
        }
        case Qt::Key_J:
        {
            if (!using_mods)        do_cmd_spike();
            break;
        }
        case Qt::Key_K:
        {
            if (!using_mods)        do_cmd_destroy();
            break;
        }
        case Qt::Key_L:
        {
            if (shift_key)          ui_center(p_ptr->py, p_ptr->px);
            else if (!using_mods)   do_cmd_look();
            break;
        }
        case Qt::Key_M:
        {
            if (shift_key)          break; // TODO - MAP
            else if (!using_mods)   do_cmd_cast(-1);
            break;
        }
        case Qt::Key_N:
        case Qt::Key_0:
        {
            if (!using_mods)        do_cmd_repeat();
            break;
        }
        case Qt::Key_O:
        {
            if (shift_key)          do_cmd_make_trap();
            else if (!using_mods)   do_cmd_open();
            break;
        }
        case Qt::Key_P:
        {
            if (ctrl_key)           display_message_log();
            else if (!using_mods)   do_cmd_cast(-1);
            break;
        }
        case Qt::Key_Q:
        {
            if (alt_key)           do_cmd_quest_desc();
            else if (shift_key)     do_cmd_suicide();
            else if (!using_mods)   do_cmd_quaff_potion();
            break;
        }
        case Qt::Key_R:
        {
            if (ctrl_key)           ui_redraw_all();
            else if (shift_key)     do_cmd_rest();
            else if (!using_mods)   do_cmd_read_scroll();
            break;
        }
        case Qt::Key_S:
        {
            if (shift_key)          do_cmd_toggle_search();
            else if (!using_mods)   do_cmd_search();
            break;
        }
        case Qt::Key_T:
        {
            if (shift_key)          do_cmd_tunnel();
            else if (!using_mods)   do_cmd_takeoff();
            break;
        }
        case Qt::Key_U:
        {
            if (!using_mods)        do_cmd_use_staff();
            break;
        }
        case Qt::Key_V:
        {
            if (!using_mods)        do_cmd_throw();
            break;
        }
        case Qt::Key_W:
        {
            if (ctrl_key)           do_cmd_wizard_mode();
            else if (!using_mods)        do_cmd_wield();
            break;
        }
        case Qt::Key_X:
        {
            if (!using_mods)        do_cmd_swap_weapon();
            break;
        }
        case Qt::Key_Y:
        {
            break;
        }
        case Qt::Key_Z:
        {
            if (!using_mods)        do_cmd_zap_rod();
            break;
        }
        case Qt::Key_Apostrophe:
        case Qt::Key_Asterisk:
        {
            target_set_closest(TARGET_KILL);
            break;
        }
        case Qt::Key_BraceLeft:
        {
            do_cmd_inscribe();
            break;
        }
        case Qt::Key_BraceRight:
        {
            do_cmd_uninscribe();
            break;
        }
        case Qt::Key_Greater:
        {
            do_cmd_go_down();
            break;
        }
        case Qt::Key_Less:
        {
            do_cmd_go_up();
            break;
        }
        case Qt::Key_Period:
        {
            do_cmd_run(DIR_UNKNOWN);
            break;
        }
        case Qt::Key_Plus:
        {
            do_cmd_alter(DIR_UNKNOWN);
            break;
        }
        case Qt::Key_Minus:
        {
            do_cmd_walk(DIR_UNKNOWN, TRUE);
            break;
        }
        case Qt::Key_Underscore:
        case Qt::Key_Comma:
        {
            do_cmd_hold();
            break;
        }
        case Qt::Key_Semicolon:
        {
            do_cmd_walk(DIR_UNKNOWN, FALSE);
        }
        case Qt::Key_Colon:
        {
            do_cmd_write_note();
            break;
        }
        default:
        {
            break;
        }
    }
}

void commands_roguelike_keyset(QKeyEvent* which_key, bool shift_key, bool alt_key, bool ctrl_key, bool meta_key)
{
    bool using_mods = FALSE;
    if (shift_key || alt_key || ctrl_key || meta_key) using_mods = TRUE;

    // Normal mode
    switch (which_key->key())
    {

        default:
        {
            break;
        }
    }
}
