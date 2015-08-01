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

single_hotkey running_hotkey;
single_hotkey player_hotkeys[NUM_HOTKEYS];

static hotkey_list list_hotkeys[NUM_HOTKEYS] =
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
    //HK_TYPE_EMPTY
    {NULL, HK_NEEDS_DIRECTION, HK_VERIFY_NO, "None"},
    //HK_TYPE_MOVE
    {command_walk, HK_NEEDS_DIRECTION, HK_VERIFY_NO, "Walk"},
    //HK_TYPE_JUMP
    {command_walk, HK_NEEDS_DIRECTION, HK_VERIFY_YES, "Jump"},
    //HK_TYPE_RUN
    {command_walk, HK_NEEDS_DIRECTION, HK_VERIFY_NO, "Run"},

};

void single_hotkey::clear_hotkey(void)
{
    hotkey_list *hk_list_ptr = &list_hotkeys[HK_TYPE_EMPTY];
    hotkey_name = hk_list_ptr->hotkey_list_name;
    hotkey_button_name = hk_list_ptr->hotkey_list_name;
    hotkey_button = hk_list_ptr->listed_hotkey;
    hotkey_steps.clear();
    hotkey_step dummy_step;
    dummy_step.step_commmand = HK_TYPE_EMPTY;
    dummy_step.step_args.wipe();
    hotkey_steps.append(dummy_step);
}

void clear_all_hotkeys()
{
    for (int i = 0; i < NUM_HOTKEYS; i++)
    {
        single_hotkey *plyr_hk_ptr = &player_hotkeys[i];
        plyr_hk_ptr->clear_hotkey();
    }
}


void single_hotkey::copy_hotkey(single_hotkey *other_hotkey)
{
    hotkey_name = other_hotkey->hotkey_name;
    hotkey_button = other_hotkey->hotkey_button;
    hotkey_button_name = other_hotkey->hotkey_button_name;
    hotkey_button = other_hotkey->hotkey_button;

    // Copy each step
    hotkey_steps.clear();
    for (int i = 0; i < other_hotkey->hotkey_steps.size(); i++)
    {
       hotkey_steps.append(other_hotkey->hotkey_steps[i]);
    }
}

bool single_hotkey::has_commands(void)
{
    if (hotkey_steps[0].step_commmand == HK_TYPE_EMPTY) return (FALSE);
    if (!hotkey_steps.size())
    {
        clear_hotkey();
        return (FALSE);
    }

    return (TRUE);
}

void HotKeyDialog::active_hotkey_name_changed(QString new_name)
{
    dialog_hotkey.hotkey_name = new_name;
}

void HotKeyDialog::save_current_hotkey()
{
    player_hotkeys[current_hotkey_int].copy_hotkey(&dialog_hotkey);
}

void HotKeyDialog::load_new_hotkey(int this_choice)
{
    current_hotkey_int = this_choice;
    dialog_hotkey.copy_hotkey(&player_hotkeys[this_choice]);
}

void HotKeyDialog::active_hotkey_changed(int new_hotkey)
{
    save_current_hotkey();
    load_new_hotkey(new_hotkey);
    current_name->setText(dialog_hotkey.hotkey_name);
    active_step = 0;
    display_hotkey_steps();

}

void HotKeyDialog::add_hotkeys_header()
{
    QHBoxLayout *hlay_header = new QHBoxLayout;
    main_layout->addLayout(hlay_header);
    current_hotkey_name = new QComboBox;
    for (int i = 0; i < NUM_HOTKEYS; i++)
    {
        current_hotkey_name->addItem(QString("%1") .arg(i));
        current_hotkey_name->setItemText(i, list_hotkeys[i].hotkey_list_name);
    }
    hlay_header->addWidget(current_hotkey_name);
    connect(current_hotkey_name, SIGNAL(currentIndexChanged(int)), this, SLOT(active_hotkey_changed(int)));
    current_name = new QLineEdit;
    current_name->setText(dialog_hotkey.hotkey_name);
    connect(current_name, SIGNAL(textChanged(QString)), this, SLOT(active_hotkey_name_changed(QString)));
    hlay_header->addWidget(current_name);

    hlay_header->addStretch(1);
}

void HotKeyDialog::active_hotkey_command_changed(int this_choice)
{
    QString item_id = QObject::sender()->objectName();
    item_id.remove("Step_Command_");
    int this_step = item_id.toInt();
    dialog_hotkey.hotkey_steps[this_step].step_commmand = this_choice;
    dialog_hotkey.hotkey_steps[this_step].step_args.wipe();
    item_id.prepend("hlay_step_");
    QList<QHBoxLayout *> hlay_list = this->findChildren<QHBoxLayout *>();
    for (int x = 0; x < hlay_list.size(); x++)
    {
        QHBoxLayout *this_hlay = hlay_list.at(x);

        QString this_name = this_hlay->objectName();

        if (this_name.contains(item_id))
        {
            create_one_hotkey_step(this_hlay, this_step);
        }
    }

}

void HotKeyDialog::active_hotkey_direction_changed(int new_dir)
{
    dialog_hotkey.hotkey_steps[active_step].step_args.direction = new_dir;
}

void HotKeyDialog::create_direction_pad(QHBoxLayout *this_layout, int step)
{
    hotkey_step *hks_ptr = &dialog_hotkey.hotkey_steps[step];

    this_layout->addStretch(1);
    QVBoxLayout *vlay_direction = new QVBoxLayout;
    this_layout->addLayout(vlay_direction);

    // Add a header
    QLabel *header_dir = new QLabel("<b>Direction:</b>");
    vlay_direction->addWidget(header_dir, Qt::AlignCenter);
    QGridLayout *gridlay_direction = new QGridLayout;
    vlay_direction->addLayout(gridlay_direction);

    group_directions = new QButtonGroup;
    group_directions->setExclusive(TRUE);
    // if no specified direction, default to north
    if (!hks_ptr->step_args.direction) hks_ptr->step_args.direction = 8;

    // Add all the buttons
    QRadioButton *north_west = new QRadioButton;
    group_directions->addButton(north_west, 7);
    north_west->setToolTip("NorthWest");
    north_west->setIcon(QIcon(":/icons/lib/icons/arrow-northwest.png"));
    gridlay_direction->addWidget(north_west, 0, 0, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == 7) north_west->setChecked(TRUE);
    else north_west->setChecked(FALSE);

    QRadioButton *north = new QRadioButton;
    group_directions->addButton(north, 8);
    north->setToolTip("North");
    north->setIcon(QIcon(":/icons/lib/icons/arrow-north.png"));
    gridlay_direction->addWidget(north, 0, 1, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == 8) north->setChecked(TRUE);
    else north->setChecked(FALSE);

    QRadioButton *north_east = new QRadioButton;
    group_directions->addButton(north_east, 9);
    north_east->setToolTip("NorthEast");
    north_east->setIcon(QIcon(":/icons/lib/icons/arrow-northeast.png"));
    gridlay_direction->addWidget(north_east, 0, 2, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == 9) north_east->setChecked(TRUE);
    else north_east->setChecked(FALSE);

    QRadioButton *west = new QRadioButton;
    group_directions->addButton(west, 4);
    west->setToolTip("West");
    west->setIcon(QIcon(":/icons/lib/icons/arrow-west.png"));
    gridlay_direction->addWidget(west, 1, 0, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == 4) west->setChecked(TRUE);
    else west->setChecked(FALSE);

    QRadioButton *east = new QRadioButton;
    group_directions->addButton(east, 6);
    east->setToolTip("East");
    east->setIcon(QIcon(":/icons/lib/icons/arrow-east.png"));
    gridlay_direction->addWidget(east, 1, 2, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == 6) east->setChecked(TRUE);
    else east->setChecked(FALSE);

    QRadioButton *south_west = new QRadioButton;
    group_directions->addButton(south_west, 1);
    south_west->setToolTip("SouthWest");
    south_west->setIcon(QIcon(":/icons/lib/icons/arrow-southwest.png"));
    gridlay_direction->addWidget(south_west, 2, 0, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == 1) south_west->setChecked(TRUE);
    else south_west->setChecked(FALSE);

    QRadioButton *south = new QRadioButton;
    group_directions->addButton(south, 2);
    south->setToolTip("South");
    south->setIcon(QIcon(":/icons/lib/icons/arrow-south.png"));
    gridlay_direction->addWidget(south, 2, 1, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == 2) south->setChecked(TRUE);
    else south->setChecked(FALSE);

    QRadioButton *south_east = new QRadioButton;
    group_directions->addButton(south_east, 3);
    south_east->setToolTip("SouthEast");
    south_east->setIcon(QIcon(":/icons/lib/icons/arrow-southeast.png"));
    gridlay_direction->addWidget(south_east, 2, 2, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == 3) south_east->setChecked(TRUE);
    else south_east->setChecked(FALSE);

    connect(group_directions, SIGNAL(buttonClicked(int)), this, SLOT(active_hotkey_direction_changed(int)));
}

void HotKeyDialog::create_one_hotkey_step(QHBoxLayout *this_layout, int step)
{
    clear_layout(this_layout);
    hotkey_step *hks_ptr = &dialog_hotkey.hotkey_steps[step];

    hotkey_type *ht_ptr = &hotkey_actions[hks_ptr->step_commmand];
    if (ht_ptr->hotkey_needs == HK_NEEDS_DIRECTION) create_direction_pad(this_layout, step);
    else hks_ptr->step_args.direction = 0;

}

void HotKeyDialog::display_hotkey_steps()
{
    clear_layout(vlay_hotkey_steps);
    for (int i = 0; i < dialog_hotkey.hotkey_steps.size(); i++)
    {
        QHBoxLayout *hlay_header = new QHBoxLayout;
        vlay_hotkey_steps->addLayout(hlay_header);

        QLabel *header_step = new QLabel(QString("<b>Step %1:  </b>") .arg(i));
        hlay_header->addWidget(header_step, Qt::AlignLeft);

        QComboBox *this_combo_box = new QComboBox;
        for (int x = 0; x < HK_TYPE_END; x++)
        {
            this_combo_box->addItem(QString("%1") .arg(x));
            this_combo_box->setItemText(x, hotkey_actions[x].name);
        }
        this_combo_box->setCurrentIndex(dialog_hotkey.hotkey_steps[i].step_commmand);
        connect(this_combo_box, SIGNAL(currentIndexChanged(int)), this, SLOT(active_hotkey_command_changed(int)));
        hlay_header->addWidget(this_combo_box, Qt::AlignLeft);
        hlay_header->addStretch(1);

        QHBoxLayout *this_layout = new QHBoxLayout;
        this_layout->setObjectName(QString("hlay_step_%1") .arg(i));
        create_one_hotkey_step(this_layout, i);
        hlay_header->addLayout(this_layout, Qt::AlignLeft);

        hlay_header->addStretch(1);
    }


}


HotKeyDialog::HotKeyDialog(void)
{
    // Start with the first hotkey
    load_new_hotkey(0);
    active_step = 0;

    //Set up the main scroll bar
    top_layout = new QVBoxLayout;
    main_layout = new QVBoxLayout;
    top_widget = new QWidget;
    scroll_box = new QScrollArea;
    top_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    top_widget->setLayout(main_layout);
    scroll_box->setWidget(top_widget);
    scroll_box->setWidgetResizable(TRUE);
    top_layout->addWidget(scroll_box);

    //Build the header
    QLabel *header_main = new QLabel("<b><h2>Hotkey Menu</b></h2>");
    main_layout->addWidget(header_main, Qt::AlignCenter);

    add_hotkeys_header();

    // Lay Out all of the hotkey steps
    vlay_hotkey_steps = new QVBoxLayout;
    main_layout->addLayout(vlay_hotkey_steps);
    display_hotkey_steps();

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    main_layout->addWidget(buttons);

    top_layout->addStretch(1);

    setLayout(top_layout);
    setWindowTitle(tr("Hotkey Menu"));

    this->exec();

    save_current_hotkey();
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

void wipe_hotkeys()
{
    running_hotkey.clear_hotkey();
    clear_all_hotkeys();
}

static bool set_up_hotkey(int which_hotkey)
{
    //paranoia
    if (which_hotkey >= NUM_HOTKEYS)return (FALSE);

    single_hotkey *plyr_hk_ptr = &player_hotkeys[which_hotkey];

    // Make sure the hotkey is set up
    if (!plyr_hk_ptr->has_commands()) return (FALSE);

    running_hotkey.copy_hotkey(plyr_hk_ptr);

    run_hotkey_step();

    return (TRUE);
}

bool check_hotkey_commands(int key_press, bool shift_key, bool alt_key, bool ctrl_key, bool meta_key)
{
    (void)shift_key;
    (void)alt_key;
    (void)ctrl_key;
    (void)meta_key;
    for (int i = 0; i < NUM_HOTKEYS; i++)
    {
        hotkey_list *hk_list_ptr = &list_hotkeys[i];

        if (key_press != hk_list_ptr->listed_hotkey) continue;
        return (set_up_hotkey(i));
    }

    return (FALSE);
}

// Run the active hotkey first step
// First checks if there is a hotkey to run
void run_hotkey_step()
{
    //First, make sure there are remaining hotkey steps
    if (!running_hotkey.has_commands()) return;

    hotkey_step *this_step = &running_hotkey.hotkey_steps[0];
    hotkey_type *this_action = &hotkey_actions[this_step->step_commmand];

    // A simple direction based hotkey
    if (this_action->hotkey_needs == HK_NEEDS_DIRECTION)
    {
        this_action->hotkey_function(this_step->step_args);
        running_hotkey.hotkey_steps.remove(0);
    }
}
