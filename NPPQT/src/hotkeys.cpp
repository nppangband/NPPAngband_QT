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
#include <QPushButton>

single_hotkey running_hotkey;
single_hotkey player_hotkeys[NUM_HOTKEYS];

#define STEP_MULT   1000

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
    //HK_TYPE_CAST
    {HK_NEEDS_SPELL, "Cast Spell", 0},
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

void single_hotkey::clear_hotkey_steps(void)
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
        plyr_hk_ptr->clear_hotkey_steps();
    }
}



void single_hotkey::copy_hotkey(single_hotkey *other_hotkey)
{
    hotkey_name = other_hotkey->hotkey_name;
    hotkey_button = other_hotkey->hotkey_button;
    hotkey_button_name = other_hotkey->hotkey_button_name;

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
int HotKeyDialog::get_current_step(QString item_id)
{
    item_id.remove("Step_Command_", Qt::CaseInsensitive);

    // Remove anything before the phrase below
    QString this_index = "_step_";
    if (item_id.contains(this_index, Qt::CaseInsensitive))
    {
        int location = item_id.indexOf(this_index, Qt::CaseInsensitive) + this_index.length();
        item_id.remove(0,location);

    }
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
    QString sender_id = QObject::sender()->objectName();
    int this_step = get_current_step(sender_id);
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



// Find the new object kind based on the combo box
void HotKeyDialog::active_spell_changed(int choice)
{
    QString sender_id = QObject::sender()->objectName();
    int this_step = get_current_step(sender_id);

    hotkey_step *hks_ptr = &dialog_hotkey.hotkey_steps[this_step];

    bool old_spell_dir = spell_needs_aim(cp_ptr->spell_book, hks_ptr->step_args.number);

    // Remember the object choice
    hks_ptr->step_args.number = spell_list.at(choice);

    bool new_spell_dir = spell_needs_aim(cp_ptr->spell_book, hks_ptr->step_args.number);

    // Remove the target box if no longer needed
    if (!new_spell_dir && old_spell_dir)
    {
        delete_targeting_choices(this_step);
        hks_ptr->step_args.direction = DIR_UNKNOWN;
    }

    // Make a new target box
    if (new_spell_dir && !old_spell_dir)
    {
        hks_ptr->step_args.direction = DIR_UNKNOWN;

        // Find the hbox layout
        QString item_id = (QString("hlay_step_%1") .arg(this_step));
        QList<QHBoxLayout *> hlay_list = this->findChildren<QHBoxLayout *>();
        for (int x = 0; x < hlay_list.size(); x++)
        {
            QHBoxLayout *this_hlay = hlay_list.at(x);

            QString this_name = this_hlay->objectName();

            if (this_name.contains(item_id))
            {
                create_targeting_choices(this_hlay, this_step);
                break;
            }
        }
    }
}

// Manuallly turning the buttons true or false is necessary
// because the button group covers more than one step
void HotKeyDialog::hotkey_step_target_changed(int new_target)
{
    int step = new_target / STEP_MULT;
    int new_choice = new_target % STEP_MULT;

    hotkey_step *hks_ptr = &dialog_hotkey.hotkey_steps[step];

    hks_ptr->step_args.direction = new_choice;

    QString button_name = QString("Target Closest");

    if (new_choice == DIR_TARGET) button_name = QString("Use Current Target");
    else if (new_choice == DIR_UNKNOWN) button_name = QString("Specify During Use");

    // Manually turn the buttons true or false.
    QList<QRadioButton *> radio_list = this->findChildren<QRadioButton *>();

    for (int x = 0; x < radio_list.size(); x++)
    {
        QRadioButton *this_radio = radio_list.at(x);

        // Not the right step.
        QString item_id = this_radio->objectName();
        item_id.remove("Step_Command_", Qt::CaseInsensitive);
        int which_step = item_id.toInt();
        if (step != which_step) continue;

        // Turn on or off
        QString this_name = this_radio->text();
        if (this_name.contains(button_name)) this_radio->setChecked(TRUE);
        else this_radio->setChecked(FALSE);
    }
}

void HotKeyDialog::delete_targeting_choices(int this_step)
{
    // First, remove the radio buttons from the group
    QList<QRadioButton *> radio_list = this->findChildren<QRadioButton *>();
    QString radio_id = (QString("targeting_step_%1") .arg(this_step));
    for (int x = 0; x < radio_list.size(); x++)
    {
        QRadioButton *this_radio = radio_list.at(x);

        QString this_name = this_radio->objectName();

        if (this_name.contains(radio_id))
        {
            // Remove it from the target group
            target_choices->removeButton(this_radio);
            break;
        }
    }

    // Now remove the vbox layout
    QString item_id = (QString("vlay_targeting_step_%1") .arg(this_step));
    QList<QVBoxLayout *> vlay_list = this->findChildren<QVBoxLayout *>();
    for (int x = 0; x < vlay_list.size(); x++)
    {
        QVBoxLayout *this_vlay = vlay_list.at(x);

        QString this_name = this_vlay->objectName();

        if (this_name.contains(item_id))
        {
            clear_layout(this_vlay);
            this_vlay->deleteLater();
            break;
        }
    }
}

void HotKeyDialog::create_targeting_choices(QHBoxLayout *this_layout, int step)
{
    target_choices = new QButtonGroup;

    // Buttons will be manually turned on and off when changed.
    target_choices->setExclusive(FALSE);

    hotkey_step *hks_ptr = &dialog_hotkey.hotkey_steps[step];

    QVBoxLayout *vlay_targets = new QVBoxLayout;
    vlay_targets->setObjectName(QString("vlay_targeting_step_%1") .arg(step));
    this_layout->addLayout(vlay_targets);

    QLabel *header_targets = new QLabel("<b>Select Targeting Method</b>");
    vlay_targets->addWidget(header_targets, Qt::AlignCenter);

    QRadioButton *radio_closest = new QRadioButton("Target Closest");
    radio_closest->setObjectName(QString("targeting_step_%1") .arg(step));
    radio_closest->setChecked(FALSE);
    vlay_targets->addWidget(radio_closest);
    target_choices->addButton(radio_closest, (step * STEP_MULT + DIR_CLOSEST));

    QRadioButton *radio_current = new QRadioButton("Use Current Target");
    radio_current->setObjectName(QString("targeting_step_%1") .arg(step));
    radio_current->setChecked(FALSE);
    vlay_targets->addWidget(radio_current);
    target_choices->addButton(radio_current, (step * STEP_MULT + DIR_TARGET));

    QRadioButton *radio_specify_use = new QRadioButton("Specify During Use");
    radio_specify_use->setObjectName(QString("targeting_step_%1") .arg(step));
    radio_specify_use->setChecked(FALSE);
    vlay_targets->addWidget(radio_specify_use);
    target_choices->addButton(radio_specify_use, (step * STEP_MULT + DIR_UNKNOWN));

    if (hks_ptr->step_args.direction == DIR_CLOSEST) radio_closest->setChecked(TRUE);
    else if (hks_ptr->step_args.direction == DIR_TARGET) radio_current->setChecked(TRUE);
    else radio_specify_use->setChecked(TRUE);

    connect(target_choices, SIGNAL(buttonClicked(int)), this, SLOT(hotkey_step_target_changed(int)));
}

void HotKeyDialog::create_spell_choice_dropbox(QHBoxLayout *this_layout, int step)
{
    hotkey_step *hks_ptr = &dialog_hotkey.hotkey_steps[step];

    QVBoxLayout *vlay_direction = new QVBoxLayout;
    this_layout->addLayout(vlay_direction);

    //Create the combobox
    QComboBox *combobox_spell_choice = new QComboBox;
    int max_spellbooks = (game_mode == GAME_NPPANGBAND ? BOOKS_PER_REALM_ANGBAND : BOOKS_PER_REALM_MORIA);

    spell_list.clear();
    int current_index = -1;

    if (cp_ptr->spell_book) for (int i = 0; i < max_spellbooks; i++)
    {
        for (int j = 0; j < SPELLS_PER_BOOK; j++)
        {
            int spell = get_spell_from_list(i, j);

            if (spell == -1) continue;

            if (!spell_okay(spell, TRUE)) continue;

            combobox_spell_choice->addItem(QString("%1") .arg(spell));
            combobox_spell_choice->setItemText(spell_list.size(), cast_spell(MODE_SPELL_NAME, cp_ptr->spell_book, spell, DIR_UNKNOWN));

            // Try to find the current index.
            if (spell == hks_ptr->step_args.number) current_index = spell_list.size();

            //Assume the first possible spell
            if (!hks_ptr->step_args.number && !spell_list.size()) hks_ptr->step_args.number = spell;

            spell_list.append(spell);
        }
    }

    QString noun = cast_spell(MODE_SPELL_NOUN, cp_ptr->spell_book, 1, 0);
    QString verb = cast_spell(MODE_SPELL_VERB, cp_ptr->spell_book, 1, 0);
    noun.append("s");
    QString label_text = (QString("Select a %1 to %2") .arg(noun) .arg(verb));

    // Add a header
    QLabel *header_dir = new QLabel(label_text);
    vlay_direction->addWidget(header_dir, Qt::AlignCenter);
    if (!spell_list.size())
    {
        if (!cp_ptr->spell_book)  header_dir->setText("You cannot cast spells");
        else header_dir->setText(QString("You do not know any %1") .arg(noun));
        delete combobox_spell_choice;
        return;
    }

    combobox_spell_choice->setCurrentIndex(current_index);
    hks_ptr->step_args.verify = TRUE;

    connect(combobox_spell_choice, SIGNAL(currentIndexChanged(int)), this, SLOT(active_spell_changed(int)));
    combobox_spell_choice->setObjectName(QString("Step_Command_%1") .arg(step));
    vlay_direction->addWidget(combobox_spell_choice);
}

// Helper function to determine if the current object kind
// should be included in a ComboBox object kinds
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

// Helper function to find the current k_idx
// selection in a combobox of object kinds
int HotKeyDialog::find_selected_k_idx(int choice, int step)
{
    hotkey_step *hks_ptr = &dialog_hotkey.hotkey_steps[step];
    int this_tval = hotkey_actions[hks_ptr->step_commmand].tval;

    int current_index = 0;

    for (int i = z_info->k_max-1; i > 0; i--)
    {
        if (!accept_object_kind(i, this_tval, step)) continue;

        if (choice == current_index) return (i);

        current_index++;
    }

    // Whoops!  Shouldn't happen.
    return (1);
}

// Find the new object kind based on the combo box
void HotKeyDialog::active_k_idx_changed(int choice)
{
    QString sender_id = QObject::sender()->objectName();
    int this_step = get_current_step(sender_id);

    hotkey_step *hks_ptr = &dialog_hotkey.hotkey_steps[this_step];

    bool old_obj_dir = obj_kind_needs_aim(hks_ptr->step_args.k_idx);

    // Remember the object choice
    hks_ptr->step_args.k_idx = find_selected_k_idx(choice, this_step);

    bool new_obj_dir = obj_kind_needs_aim(hks_ptr->step_args.k_idx);

    // Remove the direction box
    if (new_obj_dir && !old_obj_dir)
    {
        delete_targeting_choices(this_step);
        hks_ptr->step_args.direction = DIR_UNKNOWN;
    }

    // Add the direction box
    if (new_obj_dir && !old_obj_dir)
    {
        hks_ptr->step_args.direction = DIR_UNKNOWN;

        // Find the hbox layout
        QString item_id = (QString("hlay_step_%1") .arg(this_step));
        QList<QHBoxLayout *> hlay_list = this->findChildren<QHBoxLayout *>();
        for (int x = 0; x < hlay_list.size(); x++)
        {
            QHBoxLayout *this_hlay = hlay_list.at(x);

            QString this_name = this_hlay->objectName();

            if (this_name.contains(item_id))
            {
                create_targeting_choices(this_hlay, this_step);
                break;
            }
        }
    }
}

void HotKeyDialog::create_object_kind_dropbox(QHBoxLayout *this_layout, int this_step)
{
    hotkey_step *hks_ptr = &dialog_hotkey.hotkey_steps[this_step];

    int this_tval = hotkey_actions[hks_ptr->step_commmand].tval;

    QVBoxLayout *vlay_obj_kind = new QVBoxLayout;
    vlay_obj_kind->setObjectName(QString("vlay_obj_kind_step_%1") .arg(this_step));
    this_layout->addLayout(vlay_obj_kind);

    //Create the combobox
    QComboBox *this_combobox = new QComboBox;
    this_combobox->setObjectName(QString("obj_kind_combo_step_%1") .arg(this_step));
    int current_index = 0;
    int count = 0;

    for (int i = z_info->k_max-1; i > 0; i--)
    {
        object_kind *k_ptr = &k_info[i];

        if (!accept_object_kind(i, this_tval, this_step)) continue;

        this_combobox->addItem(QString("%1") .arg(STEP_MULT * this_step + i));
        this_combobox->setItemText(count, capitalize_first(k_ptr->k_name));

        // Try to find the current index.
        if (i == hks_ptr->step_args.k_idx) current_index = count;

        count++;
    }

    // Add a header
    QLabel *header_dir = new QLabel("<b>Select Object To Use:</b>");
    vlay_obj_kind->addWidget(header_dir, Qt::AlignCenter);
    if (!count)
    {
        header_dir->setText("No Known Objects Of This Type");
        delete this_combobox;
        return;
    }

    hks_ptr->step_args.k_idx = find_selected_k_idx(current_index, this_step);

    this_combobox->setCurrentIndex(current_index);
    hks_ptr->step_args.verify = TRUE;

    connect(this_combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(active_k_idx_changed(int)));
    vlay_obj_kind->addWidget(this_combobox);
}

// Manuallly turning the buttons true or false is necessary
// because the button group covers more than one step
void HotKeyDialog::hotkey_step_direction_changed(int new_dir)
{
    int step = new_dir / STEP_MULT;
    int this_dir = new_dir % STEP_MULT;

    hotkey_step *hks_ptr = &dialog_hotkey.hotkey_steps[step];

    hks_ptr->step_args.direction = this_dir;

    QString button_dir = QString("Specify Direction during command execution");
    if (this_dir == DIR_NORTHWEST) button_dir = QString("NorthWest");
    else if (this_dir == DIR_NORTH) button_dir = QString("North");
    else if (this_dir == DIR_NORTHEAST) button_dir = QString("NorthEast");
    else if (this_dir == DIR_EAST) button_dir = QString("East");
    else if (this_dir == DIR_WEST) button_dir = QString("West");
    else if (this_dir == DIR_SOUTHEAST) button_dir = QString("SouthEast");
    else if (this_dir == DIR_SOUTHWEST) button_dir = QString("SouthWest");
    else if (this_dir == DIR_SOUTH) button_dir = QString("South");

    // Manually turn the buttons true or false.
    QList<QRadioButton *> radio_list = this->findChildren<QRadioButton *>();

    for (int x = 0; x < radio_list.size(); x++)
    {
        QRadioButton *this_radio = radio_list.at(x);

        // Not the right step.
        QString item_id = this_radio->objectName();
        item_id.remove("direction_step_", Qt::CaseInsensitive);
        int which_step = item_id.toInt();
        if (step != which_step) continue;

        // Turn on or off if it is a match
        QString this_name = this_radio->toolTip();
        if (this_name.contains(button_dir, Qt::CaseInsensitive) && (this_name.indexOf(button_dir, Qt::CaseInsensitive) == 0))
        {
            this_radio->setChecked(TRUE);
        }
        else this_radio->setChecked(FALSE);
    }
}

void HotKeyDialog::delete_direction_pad(int step)
{
    // First, remove the radio buttons from the group
    QList<QRadioButton *> radio_list = this->findChildren<QRadioButton *>();
    QString radio_id = (QString("direction_step_%1") .arg(step));
    for (int x = 0; x < radio_list.size(); x++)
    {
        QRadioButton *this_radio = radio_list.at(x);

        QString this_name = this_radio->objectName();

        if (this_name.contains(radio_id))
        {
            // Remove it from the target group
            group_directions->removeButton(this_radio);
            break;
        }
    }

    // Now remove the vbox layout
    QString item_id = (QString("vlay_direction_step_%1") .arg(step));
    QList<QVBoxLayout *> vlay_list = this->findChildren<QVBoxLayout *>();
    for (int x = 0; x < vlay_list.size(); x++)
    {
        QVBoxLayout *this_vlay = vlay_list.at(x);

        QString this_name = this_vlay->objectName();

        if (this_name.contains(item_id))
        {
            clear_layout(this_vlay);
            this_vlay->deleteLater();
            break;
        }
    }
}

void HotKeyDialog::create_direction_pad(QHBoxLayout *this_layout, int step)
{
    hotkey_step *hks_ptr = &dialog_hotkey.hotkey_steps[step];

    QVBoxLayout *vlay_direction = new QVBoxLayout;
    this_layout->addLayout(vlay_direction);
    vlay_direction->setObjectName(QString("vlay_direction_step_%1") .arg(step));

    // Add a header
    QLabel *header_dir = new QLabel("<b>Direction:</b>");
    vlay_direction->addWidget(header_dir, Qt::AlignCenter);
    QGridLayout *gridlay_direction = new QGridLayout;
    vlay_direction->addLayout(gridlay_direction);

    // Radiobuttons are turned on and off manually
    group_directions = new QButtonGroup;
    group_directions->setExclusive(FALSE);

    // Add all the buttons
    QRadioButton *north_west = new QRadioButton;
    group_directions->addButton(north_west, step * STEP_MULT + DIR_NORTHWEST);
    north_west->setToolTip("NorthWest");
    north_west->setIcon(QIcon(":/icons/lib/icons/arrow-northwest.png"));
    gridlay_direction->addWidget(north_west, 0, 0, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_NORTHWEST) north_west->setChecked(TRUE);
    else north_west->setChecked(FALSE);
    north_west->setObjectName(QString("direction_step_%1") .arg(step));

    QRadioButton *north = new QRadioButton;
    group_directions->addButton(north, step * STEP_MULT + DIR_NORTH);
    north->setToolTip("North");
    north->setIcon(QIcon(":/icons/lib/icons/arrow-north.png"));
    gridlay_direction->addWidget(north, 0, 1, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_NORTH) north->setChecked(TRUE);
    else north->setChecked(FALSE);
    north->setObjectName(QString("direction_step_%1") .arg(step));

    QRadioButton *north_east = new QRadioButton;
    group_directions->addButton(north_east, step * STEP_MULT + DIR_NORTHEAST);
    north_east->setToolTip("NorthEast");
    north_east->setIcon(QIcon(":/icons/lib/icons/arrow-northeast.png"));
    gridlay_direction->addWidget(north_east, 0, 2, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_NORTHEAST) north_east->setChecked(TRUE);
    else north_east->setChecked(FALSE);
    north_east->setObjectName(QString("direction_step_%1") .arg(step));

    QRadioButton *west = new QRadioButton;
    group_directions->addButton(west, step * STEP_MULT + DIR_WEST);
    west->setToolTip("West");
    west->setIcon(QIcon(":/icons/lib/icons/arrow-west.png"));
    gridlay_direction->addWidget(west, 1, 0, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_WEST) west->setChecked(TRUE);
    else west->setChecked(FALSE);
    west->setObjectName(QString("direction_step_%1") .arg(step));

    QRadioButton *dir_none = new QRadioButton;
    group_directions->addButton(dir_none, step * STEP_MULT + DIR_UNKNOWN);
    dir_none->setToolTip("Specify Direction during command execution");
    dir_none->setIcon(QIcon(":/icons/lib/icons/target-cancel.png"));
    gridlay_direction->addWidget(dir_none, 1, 1, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_UNKNOWN) dir_none->setChecked(TRUE);
    else dir_none->setChecked(FALSE);
    dir_none->setObjectName(QString("direction_step_%1") .arg(step));

    QRadioButton *east = new QRadioButton;
    group_directions->addButton(east, step * STEP_MULT + DIR_EAST);
    east->setToolTip("East");
    east->setIcon(QIcon(":/icons/lib/icons/arrow-east.png"));
    gridlay_direction->addWidget(east, 1, 2, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_EAST) east->setChecked(TRUE);
    else east->setChecked(FALSE);
    east->setObjectName(QString("direction_step_%1") .arg(step));

    QRadioButton *south_west = new QRadioButton;
    group_directions->addButton(south_west, step * STEP_MULT + DIR_SOUTHWEST);
    south_west->setToolTip("SouthWest");
    south_west->setIcon(QIcon(":/icons/lib/icons/arrow-southwest.png"));
    gridlay_direction->addWidget(south_west, 2, 0, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_SOUTHWEST) south_west->setChecked(TRUE);
    else south_west->setChecked(FALSE);
    south_west->setObjectName(QString("direction_step_%1") .arg(step));

    QRadioButton *south = new QRadioButton;
    group_directions->addButton(south, step * STEP_MULT + DIR_SOUTH);
    south->setToolTip("South");
    south->setIcon(QIcon(":/icons/lib/icons/arrow-south.png"));
    gridlay_direction->addWidget(south, 2, 1, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_SOUTH) south->setChecked(TRUE);
    else south->setChecked(FALSE);
    south->setObjectName(QString("direction_step_%1") .arg(step));

    QRadioButton *south_east = new QRadioButton;
    group_directions->addButton(south_east, step * STEP_MULT + DIR_SOUTHEAST);
    south_east->setToolTip("SouthEast");
    south_east->setIcon(QIcon(":/icons/lib/icons/arrow-southeast.png"));
    gridlay_direction->addWidget(south_east, 2, 2, Qt::AlignCenter);
    if (hks_ptr->step_args.direction == DIR_SOUTHEAST) south_east->setChecked(TRUE);
    else south_east->setChecked(FALSE);
    south_east->setObjectName(QString("direction_step_%1") .arg(step));

    connect(group_directions, SIGNAL(buttonClicked(int)), this, SLOT(hotkey_step_direction_changed(int)));
}

// Add buttons for inserting and deleting steps
void HotKeyDialog::create_step_buttons(QHBoxLayout *this_layout, int step)
{
    QVBoxLayout *vlay_direction = new QVBoxLayout;
    this_layout->addLayout(vlay_direction);
    vlay_direction->setObjectName(QString("vlay_buttons_step_%1") .arg(step));

    QPushButton *insert_step_button = new QPushButton;
    insert_step_button->setIcon(QIcon(":/icons/lib/icons/arrow-up-double.png"));
    insert_step_button->setToolTip("Insert a new hotkey step before this step.");
    insert_step_button->setObjectName(QString("Step_Command_%1") .arg(step));
    connect(insert_step_button, SIGNAL(pressed()), this, SLOT(insert_step()));
    vlay_direction->addWidget(insert_step_button);

    // Add an "delete step" button for all steps
    QPushButton *delete_step_button = new QPushButton;
    delete_step_button->setIcon(QIcon(":/icons/lib/icons/destroy.png"));
    delete_step_button->setToolTip("Delete this hotkey step.");
    delete_step_button->setObjectName(QString("Step_Command_%1") .arg(step));
    vlay_direction->addWidget(delete_step_button);
    connect(delete_step_button, SIGNAL(pressed()), this, SLOT(delete_step()));

    QPushButton *add_step_button = new QPushButton;
    add_step_button->setIcon(QIcon(":/icons/lib/icons/arrow-down-double.png"));
    add_step_button->setToolTip("Add a new hotkey step after this step.");
    add_step_button->setObjectName(QString("Step_Command_%1") .arg(step));
    connect(add_step_button, SIGNAL(pressed()), this, SLOT(add_step()));
    vlay_direction->addWidget(add_step_button);

    vlay_direction->addStretch(1);
}

// Insert a step before the current one
void HotKeyDialog::insert_step()
{
    QString sender_id = QObject::sender()->objectName();
    int this_step = get_current_step(sender_id);

    hotkey_step dummy_step;
    dummy_step.step_commmand = HK_TYPE_EMPTY;
    dummy_step.step_args.wipe();

    dialog_hotkey.hotkey_steps.insert(this_step, dummy_step);


    display_hotkey_steps();
}

// Delete the current step
void HotKeyDialog::delete_step()
{
    QString sender_id = QObject::sender()->objectName();
    int this_step = get_current_step(sender_id);

    if (dialog_hotkey.hotkey_steps.size() == 1)
    {
        dialog_hotkey.clear_hotkey_steps();
    }
    else
    {
        dialog_hotkey.hotkey_steps.remove(this_step);
    }

    display_hotkey_steps();
}
// Add a step after the current one
void HotKeyDialog::add_step()
{
    QString sender_id = QObject::sender()->objectName();
    int this_step = get_current_step(sender_id) + 1;

    hotkey_step dummy_step;
    dummy_step.step_commmand = HK_TYPE_EMPTY;
    dummy_step.step_args.wipe();

    dialog_hotkey.hotkey_steps.insert(this_step, dummy_step);

    display_hotkey_steps();
}

void HotKeyDialog::create_one_hotkey_step(QHBoxLayout *this_layout, int step)
{
    // Make sure any radio buttons are removed form their group
    delete_direction_pad(step);
    delete_targeting_choices(step);
    clear_layout(this_layout);

    hotkey_step *hks_ptr = &dialog_hotkey.hotkey_steps[step];

    hotkey_type *ht_ptr = &hotkey_actions[hks_ptr->step_commmand];

    if (ht_ptr->hotkey_needs == HK_NEEDS_DIRECTION)
    {

        hks_ptr->step_args.k_idx = 0;
        hks_ptr->step_args.number = 0;
        create_direction_pad(this_layout, step);

    }
    else if (ht_ptr->hotkey_needs == HK_NEEDS_OBJECT_KIND)
    {
        create_object_kind_dropbox(this_layout, step);
        hks_ptr->step_args.number = 0;

        if (obj_kind_needs_aim(hks_ptr->step_args.k_idx))
        {
            create_targeting_choices(this_layout, step);
        }
        else hks_ptr->step_args.direction = 0;
    }
    else if (ht_ptr->hotkey_needs == HK_NEEDS_SPELL)
    {
        hks_ptr->step_args.k_idx = 0;
        create_spell_choice_dropbox(this_layout, step);

        if (spell_list.size() && spell_needs_aim(cp_ptr->spell_book, hks_ptr->step_args.number))
        {
            create_targeting_choices(this_layout, step);
        }
        else hks_ptr->step_args.direction = 0;
    }

    create_step_buttons(this_layout, step);

}

// Wipe and redraw the hotkey steps
void HotKeyDialog::display_hotkey_steps()
{
    // First, make sure all radio buttons are cleared from each group
    for (int i = 0; i < dialog_hotkey.hotkey_steps.size(); i++)
    {
        delete_direction_pad(i);
        delete_targeting_choices(i);
    }

    clear_layout(vlay_hotkey_steps);
    for (int i = 0; i < dialog_hotkey.hotkey_steps.size(); i++)
    {
        QHBoxLayout *hlay_header = new QHBoxLayout;
        vlay_hotkey_steps->addLayout(hlay_header);

        QLabel *header_step = new QLabel(QString("<b>Step %1:  </b>") .arg(i + 1));
        hlay_header->addWidget(header_step, Qt::AlignLeft);

        QComboBox *this_combo_box = new QComboBox;
        this_combo_box->setObjectName(QString("Step_Command_%1") .arg(i));
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

// Get the intended direction for a hotkey that requires one
static int extract_hotkey_dir(int dir, bool trap_spell)
{
    /* Check for confusion */
    if (p_ptr->timed[TMD_CONFUSED])
    {
        message(QString("You are confused."));
        return(ddd[randint0(8)]);
    }

    // First target closest, if there is anything there
    if (dir == DIR_CLOSEST)
    {
        if (target_set_closest(TARGET_KILL | TARGET_QUIET))
        {
            return(DIR_TARGET);
        }
    }
    if (dir == DIR_TARGET)
    {
        if (target_okay()) return (DIR_TARGET);
    }

    if (!get_aim_dir(&dir, trap_spell)) return (DIR_UNKNOWN);

    return (dir);
}

/*
 * Run the specified active hotkey step
 * First checks if there is a hotkey to run
 */
static void run_hotkey_step(int step)
{
    //First, make sure there are remaining hotkey steps
    if (!running_hotkey.has_commands()) return;

    hotkey_step *this_step = &running_hotkey.hotkey_steps[step];

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

    else if (ht_ptr->hotkey_needs == HK_NEEDS_OBJECT_KIND)
    {
        arg_ptr->item = find_item(arg_ptr->k_idx, USE_FLOOR | USE_INVEN);

        // Didn't find the item
        if (arg_ptr->item == MAX_S16B)
        {
            message(QString("Unable to locate %1") .arg(object_desc_from_k_idx(arg_ptr->k_idx, ODESC_FULL | ODESC_PREFIX | ODESC_SINGULAR)));
        }
        else
        {
            bool do_command = TRUE;

            if (obj_kind_needs_aim(arg_ptr->k_idx))
            {
                bool trap_obj = k_info[arg_ptr->k_idx].is_trap_object_kind();

                arg_ptr->direction = extract_hotkey_dir(arg_ptr->direction, trap_obj);

                if (arg_ptr->direction == DIR_UNKNOWN) do_command = FALSE;
            }

            // Use the item
            if (do_command) command_use(this_step->step_args);
        }
    }

    else if (ht_ptr->hotkey_needs == HK_NEEDS_SPELL)
    {
        if (p_ptr->can_cast())
        {
            bool do_command = TRUE;

            if (spell_needs_aim(cp_ptr->spell_book, arg_ptr->number))
            {
                bool trap_spell = is_trap_spell(cp_ptr->spell_book, arg_ptr->number);

                arg_ptr->direction = extract_hotkey_dir(arg_ptr->direction, trap_spell);

                if (arg_ptr->direction == DIR_UNKNOWN) do_command = FALSE;
            }

            // Use the item
            if (do_command) cast_spell(this_step->step_args);
        }

    }
}

//Loop to run the complete hotkey
static void run_hotkey_steps()
{
    for (int i = 0; i < running_hotkey.hotkey_steps.size(); i++)
    {
        run_hotkey_step(i);
    }

    running_hotkey.clear_hotkey_steps();
}

static bool set_up_hotkey(int which_hotkey)
{
    //paranoia
    if (which_hotkey >= NUM_HOTKEYS)return (FALSE);

    single_hotkey *plyr_hk_ptr = &player_hotkeys[which_hotkey];

    // Make sure the hotkey is set up
    if (!plyr_hk_ptr->has_commands()) return (FALSE);

    running_hotkey.clear_hotkey_steps();
    running_hotkey.copy_hotkey(plyr_hk_ptr);

    run_hotkey_steps();

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
