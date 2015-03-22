
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

#include <src/npp.h>
#include <src/object_all_menu.h>
#include <src/messages.h>
#include <QButtonGroup>
#include <QKeyEvent>
#include <QDialogButtonBox>
#include <QScrollArea>

/*
 *
 *
 * OBJECTS DIALOG
 *
 *
 */

void AllObjectsDialog::move_left()
{
    int which_tab = object_tabs->currentIndex();
    object_tabs->setCurrentIndex(which_tab - 1);
}

void AllObjectsDialog::move_right()
{
    int which_tab = object_tabs->currentIndex();
    object_tabs->setCurrentIndex(which_tab + 1);
}

// See if the user selected a button bia a keypress.
void AllObjectsDialog::keyPressEvent(QKeyEvent* which_key)
{
    // Handle escape key
    if (which_key->key() == Qt::Key_Escape)
    {
        this->close();
        return;
    }

    if (which_key->key() == Qt::Key_Less)
    {
        move_left();
        return;
    }

    if (which_key->key() == Qt::Key_Greater)
    {
        move_right();
        return;
    }
}

void AllObjectsDialog::update_header()
{
    //max capactity in ounces
    u16b max_capacity = normal_speed_weight_limit();

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
    p_ptr->message_append_stop();
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

    p_ptr->message_append_stop();

    update_floor_list(floor_list, FALSE, TRUE);
    update_inven_list(inven_list, FALSE, TRUE);
    update_equip_list(equip_list, FALSE, TRUE);
    update_quiver_list(quiver_list, FALSE, TRUE);
    hide_or_show_tabs();
    update_message_area(message_area, 3);
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


AllObjectsDialog::AllObjectsDialog(bool do_buttons, int start_screen)
{
    confirm_tabs();

    // Handle no available objects.
    if (no_objects())
    {
        pop_up_message_box("You have no objects to manage");
        return;
    }

    // Set up the main layout
    QVBoxLayout *main_layout = new QVBoxLayout;

    //Build the header
    header_main = new QLabel("<b><h2>Object Menu</b></h2>");
    header_main->setAlignment(Qt::AlignCenter);
    header_weight1 = new QLabel;
    header_weight2 = new QLabel;
    main_layout->addWidget(header_main);
    main_layout->addWidget(header_weight1);
    main_layout->addWidget(header_weight2);
    update_header();

    // add the message area
    message_area = new QLabel;
    message_area->setWordWrap(TRUE);
    message_area->setAutoFillBackground(TRUE);
    QPalette this_palette;
    this_palette.setColor(QPalette::Window, QColor(Qt::black));
    message_area->setPalette(this_palette);
    main_layout->addWidget(message_area);
    message_area->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    update_message_area(message_area, 3);

    // Set up the tabs
    object_tabs = new QTabWidget;
    main_layout->addWidget(object_tabs);
    floor_tab = new QWidget;
    inven_tab = new QWidget;
    equip_tab = new QWidget;

    // Add the list of floor items
    QVBoxLayout *floor_vlay = new QVBoxLayout;
    floor_tab->setLayout(floor_vlay);
    header_floor = new QLabel(QString("<b><h1>Floor Items</b></h1>"));
    header_floor->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    floor_vlay->addWidget(header_floor);
    floor_list = new QGridLayout;
    update_floor_list(floor_list, FALSE, do_buttons);
    floor_vlay->addLayout(floor_list);
    floor_vlay->addStretch(1);

    // Add the list of inventory
    QVBoxLayout *inven_vlay = new QVBoxLayout;
    inven_tab->setLayout(inven_vlay);
    header_inven = new QLabel(QString("<b><h1>Inventory</b></h1>"));
    header_inven->setAlignment(Qt::AlignCenter | Qt::AlignTop);
    inven_vlay->addWidget(header_inven);
    inven_list = new QGridLayout;
    update_inven_list(inven_list, FALSE, do_buttons);
    inven_vlay->addLayout(inven_list);
    inven_vlay->addStretch(1);

    // Add the equipment
    QVBoxLayout *equip_and_quiver_vlay = new QVBoxLayout;
    QVBoxLayout *equip_vlay = new QVBoxLayout;
    equip_tab->setLayout(equip_and_quiver_vlay);
    equip_and_quiver_vlay->addLayout(equip_vlay);
    header_equip = new QLabel(QString("<b><h1>Equipment</b></h1>"));
    header_equip->setAlignment(Qt::AlignCenter | Qt::AlignTop);
    equip_vlay->addWidget(header_equip);
    equip_list = new QGridLayout;
    update_equip_list(equip_list, FALSE, do_buttons);
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
    update_quiver_list(quiver_list, FALSE, do_buttons);
    quiver_vlay->addLayout(quiver_list);
    equip_and_quiver_vlay->addStretch(1);

    main_layout->addStretch(1);

    QDialogButtonBox *buttons = new QDialogButtonBox();
    QPushButton *button_left = new QPushButton();
    button_left->setText("<");
    button_left->setToolTip("Pressing '<' also moves the active tab to the left.");
    connect(button_left, SIGNAL(clicked()), this, SLOT(move_left()));
    buttons->addButton(button_left, QDialogButtonBox::ActionRole);
    QPushButton *button_right = new QPushButton();
    button_right->setText(">");
    button_right->setToolTip("Pressing '>' also moves the active tab to the right.");
    connect(button_right, SIGNAL(clicked()), this, SLOT(move_right()));
    buttons->addButton(button_right, QDialogButtonBox::ActionRole);
    buttons->addButton(QDialogButtonBox::Close);
    connect(buttons, SIGNAL(rejected()), this, SLOT(close()));


    main_layout->addWidget(buttons);
    main_layout->addStretch(1);

    hide_or_show_tabs();

    setLayout(main_layout);
    setWindowTitle(tr("Object Menu"));

    this->exec();
}


void do_cmd_all_objects(int start_screen)
{
    p_ptr->in_menu = TRUE;
    AllObjectsDialog(TRUE, start_screen);
    p_ptr->in_menu = FALSE;
    p_ptr->message_append_stop();
}



