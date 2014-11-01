
/*
 * Copyright (c) 2014 Jeff Greene, Diego Gonzalez
 *
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


#include <src/object_all_menu.h>


/*
 *
 *
 * OBJECTS DIALOG
 *
 *
 */

void AllObjectsDialog::update_header()
{
    //max capactity in ounces
    u16b max_capacity;

    if (game_mode == GAME_NPPMORIA)
    {
        max_capacity = adj_str_wgt[p_ptr->state.stat_ind[A_STR]] *5;
    }
    else //game_mode == GAME_NPPANGBAND
    {
        // Slowing starts at 60% of max_weight
        // and increases by 1 every extra 10%
        max_capacity = 60 * adj_str_wgt[p_ptr->state.stat_ind[A_STR]];
    }

    u32b weight_percent = p_ptr->total_weight * 100 / max_capacity;

    header_weight1->setText((QString("<b><big>Burden: %1 lbs (%2% capacity)</big></b>")
                             .arg(formatted_weight_string(p_ptr->total_weight)) .arg(weight_percent)));
    if (p_ptr->total_weight > max_capacity)
    {
        int overweight = p_ptr->total_weight - max_capacity;
        header_weight2->setText(QString("(%1 lbs overweight)")
                        .arg((formatted_weight_string(overweight))));
    }
    else if (p_ptr->total_weight < max_capacity)
    {
        int underweight = max_capacity - p_ptr->total_weight;
        header_weight2->setText(QString("(%1 lbs underweight)")
                        .arg(formatted_weight_string(underweight)));
    }
}

void AllObjectsDialog::add_message_area()
{
    last_message = message_list[0];
    lay_message = new QVBoxLayout;
    message_area->setLayout(lay_message);
    message_one = new QLabel("msg_one");
    message_two = new QLabel("msg_two");
    message_three = new QLabel("msg_three");
    lay_message->addWidget(message_one);
    lay_message->addWidget(message_two);
    lay_message->addWidget(message_three);
}

// Confirm which tabs should be displayed.
void AllObjectsDialog::confirm_tabs()
{
    allow_floor = FALSE;
    allow_inven = FALSE;
    allow_equip = FALSE;
    allow_quiver = FALSE;

    // Confirm there is a floor item
    if (dungeon_info[p_ptr->py][p_ptr->px].has_object()) allow_floor = TRUE;

    /* Confirm there are objects in the inventory */
    for (int i = 0; i < (INVEN_WIELD - 1); i++)
    {
        /* Get the object */
        object_type *o_ptr = &inventory[i];

        /* Skip empty slots */
        if (!o_ptr->k_idx) continue;

        allow_inven = TRUE;
        break;
    }

    /* Confirm the player is wielding equipment */
    for (int i = INVEN_WIELD; i < QUIVER_START; i++)
    {
        /* Get the object */
        object_type *o_ptr = &inventory[i];

        /* Skip empty slots */
        if (!o_ptr->k_idx) continue;

        allow_equip = TRUE;
        break;
    }

    /* Scan all objects in the quiver */
    for (int i = QUIVER_START; i < QUIVER_END; i++)
    {
        /* Get the object */
        object_type *o_ptr = &inventory[i];

        /* Skip empty slots */
        if (!o_ptr->k_idx) continue;

        allow_quiver = TRUE;
        break;
    }
}



bool AllObjectsDialog::no_objects()
{
    if (allow_floor) return (FALSE);
    if (allow_inven) return (FALSE);
    if (allow_equip) return (FALSE);
    if (allow_quiver) return (FALSE);
    return (TRUE);
}


void AllObjectsDialog::close_dialog()
{
    this->reject();
}

void AllObjectsDialog::update_dialog()
{
    update_header();
    confirm_tabs();

    if (no_objects())
    {
        close_dialog();
        return;
    }

    update_floor_list(floor_list, TRUE);
    update_inven_list(inven_list, TRUE);
    update_equip_list(equip_list, TRUE);
    update_quiver_list(quiver_list, TRUE);
    reset_messages(last_message, message_one, message_two, message_three);
    hide_or_show_tabs();
}

void AllObjectsDialog::update_active_tabs()
{
    floor_tab_idx = object_tabs->indexOf(floor_tab);
    inven_tab_idx = object_tabs->indexOf(inven_tab);
    equip_tab_idx = object_tabs->indexOf(equip_tab);
}

/*
 * Figure out which tabs to show or hide.  Try to keep
 * the current activated tab activated.
 * */
void AllObjectsDialog::hide_or_show_tabs()
{
    confirm_tabs();
    if (no_objects())
    {
        //Paranoia
        close_dialog();
        return;
    }

    int active_tab = object_tabs->currentIndex();

    update_active_tabs();

    int current_tab = TABS_MAX;

    // Fine the current active tab.
    if (active_tab < 0) current_tab = TAB_INVEN;
    else if (floor_tab_idx == active_tab) current_tab = TAB_FLOOR;
    else if (inven_tab_idx == active_tab) current_tab = TAB_INVEN;
    else if (equip_tab_idx == active_tab) current_tab = TAB_EQUIP;
    else
    {
        //extreme paranoia
        close_dialog();
        return;
    }

    object_tabs->clear();

    if (allow_floor) object_tabs->addTab(floor_tab, "&Floor Items");
    if (allow_inven) object_tabs->addTab(inven_tab, "&Inventory");
    if (allow_equip || allow_quiver)
    {
        object_tabs->addTab(equip_tab, "&Equipment");
        if (allow_equip) header_equip->show();
        else header_equip->hide();
        if (allow_quiver) header_quiver->show();
        else header_quiver->hide();
        if (allow_equip && allow_quiver) empty_space->show();
        else empty_space->hide();
    }

    update_active_tabs();
    if (current_tab == TAB_FLOOR && allow_floor) object_tabs->setCurrentIndex(floor_tab_idx);
    else if (current_tab == TAB_INVEN && allow_inven) object_tabs->setCurrentIndex(inven_tab_idx);
    else if (current_tab == TAB_EQUIP && allow_equip) object_tabs->setCurrentIndex(equip_tab_idx);
    else if (allow_inven) object_tabs->setCurrentIndex(inven_tab_idx);
    else if (allow_equip) object_tabs->setCurrentIndex(equip_tab_idx);
    else if (allow_floor) object_tabs->setCurrentIndex(floor_tab_idx);
    else object_tabs->setCurrentIndex(0);
}


AllObjectsDialog::AllObjectsDialog(bool buttons)
{
    confirm_tabs();

    // Handle no available objects.
    if (no_objects())
    {
        pop_up_message_box("You have no objects to manage");
        return;
    }

    // Set up the main layout
    main_layout = new QVBoxLayout;

    //Build the header
    header_main = new QLabel("<b><h2>Object Menu</b></h2>");
    header_weight1 = new QLabel("Weight 1");
    header_weight2 = new QLabel("Weight 2");
    main_layout->addWidget(header_main);
    main_layout->addWidget(header_weight1);
    main_layout->addWidget(header_weight2);
    update_header();

    // add the message area
    message_area = new QWidget;
    main_layout->addWidget(message_area);
    add_message_area();
    reset_messages(last_message, message_one, message_two, message_three);

    // Set up the tabs
    object_tabs = new QTabWidget;
    main_layout->addWidget(object_tabs);
    floor_tab = new QWidget;
    inven_tab = new QWidget;
    equip_tab = new QWidget;

    QSpacerItem *vspacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Add the list of floor items
    QVBoxLayout *floor_vlay = new QVBoxLayout;
    floor_tab->setLayout(floor_vlay);
    header_floor = new QLabel(QString("<b><h1>Floor Items</b></h1>"));
    header_floor->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    floor_vlay->addWidget(header_floor);
    floor_list = new QGridLayout;
    update_floor_list(floor_list, buttons);
    floor_vlay->addLayout(floor_list);
    floor_vlay->addSpacerItem(vspacer);


    // Add the list of inventory
    QVBoxLayout *inven_vlay = new QVBoxLayout;
    inven_tab->setLayout(inven_vlay);
    header_inven = new QLabel(QString("<b><h1>Inventory</b></h1>"));
    header_inven->setAlignment(Qt::AlignCenter | Qt::AlignTop);
    inven_vlay->addWidget(header_inven);
    inven_list = new QGridLayout;
    update_inven_list(inven_list, buttons);
    inven_vlay->addLayout(inven_list);
    inven_vlay->addSpacerItem(vspacer);

    // Add the equipment
    QVBoxLayout *equip_and_quiver_vlay = new QVBoxLayout;
    QVBoxLayout *equip_vlay = new QVBoxLayout;
    equip_tab->setLayout(equip_and_quiver_vlay);
    equip_and_quiver_vlay->addLayout(equip_vlay);
    header_equip = new QLabel(QString("<b><h1>Equipment</b></h1>"));
    header_equip->setAlignment(Qt::AlignCenter | Qt::AlignTop);
    equip_vlay->addWidget(header_equip);
    equip_list = new QGridLayout;
    update_equip_list(equip_list, buttons);
    equip_vlay->addLayout(equip_list);

    // Add a space
    empty_space = new QLabel("empty");
    empty_space->setText(" ");
    equip_and_quiver_vlay->addWidget(empty_space);

    // Add the quiver
    QVBoxLayout *quiver_vlay = new QVBoxLayout;
    equip_and_quiver_vlay->addLayout(quiver_vlay);
    header_quiver = new QLabel(QString("<b><h1>Quiver</b></h1>"));
    header_quiver->setAlignment(Qt::AlignCenter | Qt::AlignTop);
    quiver_vlay->addWidget(header_quiver);
    quiver_list = new QGridLayout;
    update_quiver_list(quiver_list, buttons);
    quiver_vlay->addLayout(quiver_list);
    equip_and_quiver_vlay->addSpacerItem(vspacer);

    main_layout->addSpacerItem(vspacer);

    QPushButton *btn_close = new QPushButton("Close");
    main_layout->addWidget(btn_close);
    connect(btn_close, SIGNAL(clicked()), this, SLOT(reject()));

    hide_or_show_tabs();

    setLayout(main_layout);
    setWindowTitle(tr("Object Menu"));

    this->exec();
}


void do_cmd_all_objects(void)
{
    p_ptr->in_menu = TRUE;
    AllObjectsDialog(TRUE);
    p_ptr->in_menu = FALSE;
}



