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
    {HK_NEEDS_NOTHING, "None",0},
    //HK_QUAFF_POTION
    {HK_NEEDS_OBJECT_KIND, "Quaff a Potion", TV_POTION},
    //HK_READ_SCROLL
    {HK_NEEDS_OBJECT_KIND, "Read a Scroll",TV_SCROLL},
    //HK_AIM_WAND
    {HK_NEEDS_OBJECT_KIND, "Aim a Wand",TV_WAND},
    //HK_USE_STAFF
    {HK_NEEDS_OBJECT_KIND, "Use a Staff",TV_STAFF},
    //HK_ZAP_ROD
    {HK_NEEDS_OBJECT_KIND, "Zap a Rod",TV_ROD},
    //HK_EAT_FOOD
    {HK_NEEDS_OBJECT_KIND, "Eat Food", TV_FOOD},
    //HK_TYPE_MOVE
    {HK_NEEDS_DIRECTION, "Walk",0},
    //HK_TYPE_JUMP
    {HK_NEEDS_DIRECTION, "Jump",0},
    //HK_TYPE_RUN
    {HK_NEEDS_DIRECTION, "Run",0},
    //HK_TYPE_ALTER
    {HK_NEEDS_DIRECTION, "Alter",0},
    //HK_TYPE_DISARM
    {HK_NEEDS_DIRECTION, "Disarm",0},
    //HK_TYPE_CLOSE
    {HK_NEEDS_DIRECTION, "Close",0},
    //HK_TYPE_OPEN
    {HK_NEEDS_DIRECTION, "Open",0},
    //HK_TYPE_BASH
    {HK_NEEDS_DIRECTION, "Bash",0},
    //HK_TYPE_TUNNEL
    {HK_NEEDS_DIRECTION, "Tunnel",0},
    //HK_TYPE_MAKE_TRAP
    {HK_NEEDS_DIRECTION, "Make Trap",0},
    //HK_TYPE_SPIKE
    {HK_NEEDS_DIRECTION, "Spike",0},
    //HK_TYPE_HOLD
    {HK_NEEDS_NOTHING, "Hold",0},

};

void single_hotkey::clear_hotkey(void)
{
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
        hotkey_list *hk_list_ptr = &list_hotkeys[i];
        plyr_hk_ptr->hotkey_name = hk_list_ptr->hotkey_list_name;
        plyr_hk_ptr->hotkey_button_name = hk_list_ptr->hotkey_list_name;
        plyr_hk_ptr->hotkey_button = hk_list_ptr->listed_hotkey;
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
    display_hotkey_steps();
}

// Returns the step of the current QObject.
int HotKeyDialog::get_current_step()
{
    QString item_id = QObject::sender()->objectName();
    item_id.remove("Step_Command_");
    return(item_id.toInt());
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
}

void HotKeyDialog::active_hotkey_command_changed(int this_choice)
{
    int this_step = get_current_step();
    dialog_hotkey.hotkey_steps[this_step].step_commmand = this_choice;
    dialog_hotkey.hotkey_steps[this_step].step_args.wipe();
    QString item_id = (QString("hlay_step_%1") .arg(this_step));
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

bool HotKeyDialog::accept_object_kind(int k_idx, int tval, int step)
{
    object_kind *k_ptr = &k_info[k_idx];

    hotkey_step *hks_ptr = &dialog_hotkey.hotkey_steps[step];

    /* Skip "empty" items */
    if (k_ptr->k_name.isEmpty()) return (FALSE);
    if (k_ptr->tval != tval) return (FALSE);
    if (!k_ptr->everseen)
    {
        if (k_idx != hks_ptr->step_args.k_idx) return (FALSE);
    }
    return (TRUE);
}

// Find the new object kind based on the combo box
void HotKeyDialog::active_k_idx_changed(int choice)
{
    int this_step = get_current_step();

    hotkey_step *hks_ptr = &dialog_hotkey.hotkey_steps[this_step];
    int this_tval = hotkey_actions[hks_ptr->step_commmand].tval;

    int count = 0;
    int i;
    for (i = z_info->k_max-1; i > 0; i--)
    {
        if (!accept_object_kind(i, this_tval, this_step)) continue;
        // See if we have found the right object
        if (choice == count) break;
        count++;
    }

    hks_ptr->step_args.k_idx = i;


    // TODO handle object direction
}

void HotKeyDialog::create_object_kind_dropbox(QHBoxLayout *this_layout, int step)
{
    hotkey_step *hks_ptr = &dialog_hotkey.hotkey_steps[step];

    int this_tval = hotkey_actions[hks_ptr->step_commmand].tval;

    QVBoxLayout *vlay_direction = new QVBoxLayout;
    this_layout->addLayout(vlay_direction);

    //Create the combobox
    QComboBox *combobox_object_kind = new QComboBox;
    int current_index = 0;

    int count = 0;
    for (int i = z_info->k_max-1; i > 0; i--)
    {
        object_kind *k_ptr = &k_info[i];

        if (!accept_object_kind(i, this_tval, step)) continue;

        combobox_object_kind->addItem(QString("%1") .arg(i));
        combobox_object_kind->setItemText(count, capitalize_first(k_ptr->k_name));

        // Try to find the current index.
        if (i == hks_ptr->step_args.k_idx) current_index = count;

        //Assume the first possible object
        if (!hks_ptr->step_args.k_idx && !count) hks_ptr->step_args.k_idx = i;
        count++;
    }

    // Add a header
    QLabel *header_dir = new QLabel("<b>Select Object To Use:</b>");
    vlay_direction->addWidget(header_dir, Qt::AlignCenter);
    if (!count)
    {
        header_dir->setText("No Known Objects Of This Type");
        delete combobox_object_kind;
        return;
    }

    combobox_object_kind->setCurrentIndex(current_index);
    hks_ptr->step_args.verify = TRUE;

    connect(combobox_object_kind, SIGNAL(currentIndexChanged(int)), this, SLOT(active_k_idx_changed(int)));
    combobox_object_kind->setObjectName(QString("Step_Command_%1") .arg(step));
    vlay_direction->addWidget(combobox_object_kind);
}

void HotKeyDialog::active_hotkey_direction_changed(int new_dir)
{
    dialog_hotkey.hotkey_steps[get_current_step()].step_args.direction = new_dir;
}

void HotKeyDialog::create_direction_pad(QHBoxLayout *this_layout, int step)
{
    hotkey_step *hks_ptr = &dialog_hotkey.hotkey_steps[step];

    QVBoxLayout *vlay_direction = new QVBoxLayout;
    this_layout->addLayout(vlay_direction);

    // Add a header
    QLabel *header_dir = new QLabel("<b>Direction:</b>");
    vlay_direction->addWidget(header_dir, Qt::AlignCenter);
    QGridLayout *gridlay_direction = new QGridLayout;
    vlay_direction->addLayout(gridlay_direction);

    group_directions = new QButtonGroup;
    group_directions->setExclusive(TRUE);

    // Add all the buttons
    QRadioButton *north_west = new QRadioButton;
    group_directions->addButton(north_west, DIR_NORTHWEST);
    north_west->setToolTip("NorthWest");
    north_west->setIcon(QIcon(":/icons/lib/icons/arrow-northwest.png"));
    gridlay_direction->addWidget(north_west, 0, 0, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_NORTHWEST) north_west->setChecked(TRUE);
    else north_west->setChecked(FALSE);
    north_west->setObjectName(QString("Step_Command_%1") .arg(step));

    QRadioButton *north = new QRadioButton;
    group_directions->addButton(north, DIR_NORTH);
    north->setToolTip("North");
    north->setIcon(QIcon(":/icons/lib/icons/arrow-north.png"));
    gridlay_direction->addWidget(north, 0, 1, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_NORTH) north->setChecked(TRUE);
    else north->setChecked(FALSE);
    north->setObjectName(QString("Step_Command_%1") .arg(step));

    QRadioButton *north_east = new QRadioButton;
    group_directions->addButton(north_east, DIR_NORTHEAST);
    north_east->setToolTip("NorthEast");
    north_east->setIcon(QIcon(":/icons/lib/icons/arrow-northeast.png"));
    gridlay_direction->addWidget(north_east, 0, 2, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_NORTHEAST) north_east->setChecked(TRUE);
    else north_east->setChecked(FALSE);
    north_east->setObjectName(QString("Step_Command_%1") .arg(step));

    QRadioButton *west = new QRadioButton;
    group_directions->addButton(west, DIR_WEST);
    west->setToolTip("West");
    west->setIcon(QIcon(":/icons/lib/icons/arrow-west.png"));
    gridlay_direction->addWidget(west, 1, 0, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_WEST) west->setChecked(TRUE);
    else west->setChecked(FALSE);
    west->setObjectName(QString("Step_Command_%1") .arg(step));

    QRadioButton *dir_none = new QRadioButton;
    group_directions->addButton(dir_none, DIR_UNKNOWN);
    dir_none->setToolTip("Specify Direction during command execution");
    dir_none->setIcon(QIcon(":/icons/lib/icons/target-cancel.png"));
    gridlay_direction->addWidget(dir_none, 1, 1, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_UNKNOWN) dir_none->setChecked(TRUE);
    else dir_none->setChecked(FALSE);
    dir_none->setObjectName(QString("Step_Command_%1") .arg(step));

    QRadioButton *east = new QRadioButton;
    group_directions->addButton(east, DIR_EAST);
    east->setToolTip("East");
    east->setIcon(QIcon(":/icons/lib/icons/arrow-east.png"));
    gridlay_direction->addWidget(east, 1, 2, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_EAST) east->setChecked(TRUE);
    else east->setChecked(FALSE);
    east->setObjectName(QString("Step_Command_%1") .arg(step));

    QRadioButton *south_west = new QRadioButton;
    group_directions->addButton(south_west, DIR_SOUTHWEST);
    south_west->setToolTip("SouthWest");
    south_west->setIcon(QIcon(":/icons/lib/icons/arrow-southwest.png"));
    gridlay_direction->addWidget(south_west, 2, 0, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_SOUTHWEST) south_west->setChecked(TRUE);
    else south_west->setChecked(FALSE);
    south_west->setObjectName(QString("Step_Command_%1") .arg(step));

    QRadioButton *south = new QRadioButton;
    group_directions->addButton(south, DIR_SOUTH);
    south->setToolTip("South");
    south->setIcon(QIcon(":/icons/lib/icons/arrow-south.png"));
    gridlay_direction->addWidget(south, 2, 1, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_SOUTH) south->setChecked(TRUE);
    else south->setChecked(FALSE);
    south->setObjectName(QString("Step_Command_%1") .arg(step));

    QRadioButton *south_east = new QRadioButton;
    group_directions->addButton(south_east, DIR_SOUTHEAST);
    south_east->setToolTip("SouthEast");
    south_east->setIcon(QIcon(":/icons/lib/icons/arrow-southeast.png"));
    gridlay_direction->addWidget(south_east, 2, 2, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_SOUTHEAST) south_east->setChecked(TRUE);
    else south_east->setChecked(FALSE);
    south_east->setObjectName(QString("Step_Command_%1") .arg(step));

    connect(group_directions, SIGNAL(buttonClicked(int)), this, SLOT(active_hotkey_direction_changed(int)));
}

void HotKeyDialog::create_one_hotkey_step(QHBoxLayout *this_layout, int step)
{
    clear_layout(this_layout);
    hotkey_step *hks_ptr = &dialog_hotkey.hotkey_steps[step];

    hotkey_type *ht_ptr = &hotkey_actions[hks_ptr->step_commmand];
    if (ht_ptr->hotkey_needs == HK_NEEDS_DIRECTION) create_direction_pad(this_layout, step);
    else hks_ptr->step_args.direction = 0;
    if (ht_ptr->hotkey_needs == HK_NEEDS_OBJECT_KIND) create_object_kind_dropbox(this_layout, step);
    else hks_ptr->step_args.k_idx = 0;
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

        QHBoxLayout *this_layout = new QHBoxLayout;
        this_layout->setObjectName(QString("hlay_step_%1") .arg(i));
        create_one_hotkey_step(this_layout, i);
        hlay_header->addLayout(this_layout, Qt::AlignLeft);
    }
}


HotKeyDialog::HotKeyDialog(void)
{
    // Start with the first hotkey
    load_new_hotkey(0);

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
    main_layout->addWidget(header_main, Qt::AlignCenter | Qt::AlignTop);

    add_hotkeys_header();

    // Lay Out all of the hotkey steps
    vlay_hotkey_steps = new QVBoxLayout;
    main_layout->addLayout(vlay_hotkey_steps);
    display_hotkey_steps();

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    top_layout->addWidget(buttons);

    setLayout(top_layout);
    setWindowTitle(tr("Hotkey Menu"));

    QSize this_size = QSize(width(), height());
    resize(ui_max_widget_size(this_size));
    updateGeometry();

    this->exec();

    save_current_hotkey();
}

void do_hotkey_manage()
{
    HotKeyDialog();
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

static int find_item(int k_idx, int mode)
{
    s16b this_o_idx, next_o_idx = 0;
    int i;

    bool use_inven = ((mode & USE_INVEN) ? TRUE : FALSE);
    bool use_equip = ((mode & USE_EQUIP) ? TRUE : FALSE);
    bool use_floor = ((mode & USE_FLOOR) ? TRUE : FALSE);
    bool use_quiver = ((mode & USE_QUIVER) ? TRUE : FALSE);

    /* First try to find the objects on the floor */
    if (use_floor) for (this_o_idx = dungeon_info[p_ptr->py][p_ptr->px].object_idx; this_o_idx; this_o_idx = next_o_idx)
    {
        object_type *o_ptr;

        /* Get the object */
        o_ptr = &o_list[this_o_idx];

        /* Get the next object */
        next_o_idx = o_ptr->next_o_idx;

        if (o_ptr->k_idx == k_idx) return (-this_o_idx);
    }

    // Now check the backpack
    if (use_inven) for (i = 0; i < INVEN_PACK; i++)
    {
        object_type *o_ptr = &inventory[i];

        if (o_ptr->k_idx == k_idx) return (i);
    }

    if (use_equip) for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
    {
        object_type *o_ptr = &inventory[i];

        if (o_ptr->k_idx == k_idx) return (i);
    }

    if (use_quiver) for (i = QUIVER_START; i < QUIVER_END; i++)
    {
        object_type *o_ptr = &inventory[i];

        if (o_ptr->k_idx == k_idx) return (i);
    }

    // Failed to find anything
    return (MAX_S16B);
}

// Run the active hotkey first step
// First checks if there is a hotkey to run
void run_hotkey_step()
{
    //First, make sure there are remaining hotkey steps
    if (!running_hotkey.has_commands()) return;

    hotkey_step *this_step = &running_hotkey.hotkey_steps[0];

    int command = this_step->step_commmand;
    hotkey_type *ht_ptr = &hotkey_actions[command];

    cmd_arg *arg_ptr = &this_step->step_args;

    if (command == HK_NEEDS_NOTHING)
    {
        // Do nothing
    }
    else if (command == HK_TYPE_MOVE) do_cmd_walk(arg_ptr->direction, FALSE);
    else if (command == HK_TYPE_JUMP) do_cmd_walk(arg_ptr->direction, TRUE);
    else if (command == HK_TYPE_RUN) do_cmd_run(arg_ptr->direction);
    else if (command == HK_TYPE_ALTER) do_cmd_alter(arg_ptr->direction);
    else if (command == HK_TYPE_DISARM) do_cmd_disarm(arg_ptr->direction);
    else if (command == HK_TYPE_CLOSE) do_cmd_close(arg_ptr->direction);
    else if (command == HK_TYPE_OPEN) do_cmd_open(arg_ptr->direction);
    else if (command == HK_TYPE_BASH) do_cmd_bash(arg_ptr->direction);
    else if (command == HK_TYPE_TUNNEL) do_cmd_tunnel(arg_ptr->direction);
    else if (command == HK_TYPE_MAKE_TRAP) do_cmd_make_trap(arg_ptr->direction);
    else if (command == HK_TYPE_SPIKE) do_cmd_spike(arg_ptr->direction);
    else if (command == HK_TYPE_HOLD) do_cmd_hold();

    if (ht_ptr->hotkey_needs == HK_NEEDS_OBJECT_KIND)
    {

        arg_ptr->item = find_item(arg_ptr->k_idx, USE_FLOOR | USE_INVEN);

        // Didn't find the item
        if (arg_ptr->item == MAX_S16B)
        {
            message(QString("Unable to locate %1") .arg(object_desc_from_k_idx(arg_ptr->k_idx, ODESC_FULL | ODESC_PREFIX | ODESC_SINGULAR)));
        }
        else
        {
            // Use the item
            command_use(this_step->step_args);
        }
    }

    running_hotkey.hotkey_steps.removeFirst();
}
