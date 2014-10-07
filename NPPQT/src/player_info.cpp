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


#include <src/player_info.h>


/*
 *  Generic functions to be
 * used across all object dialog boxes.
 */
void ObjectDialog::clear_grid_layout(QGridLayout *lay)
{
    QLayoutItem *item;
    while ((item = lay->takeAt(0)) != 0)
    {
        QWidget *wid = item->widget();
        if (wid) delete wid;
        delete item;
    }
}

void ObjectDialog::object_click()
{
    QObject *obj = QObject::sender();

    QString id = obj->property("item_id").toString();
    //process_item(id);
}

void ObjectDialog::help_click()
{
    QString id = QObject::sender()->objectName();
    int o_idx = id.mid(1).toInt();

    object_type *o_ptr = &inventory[o_idx];
    object_info_screen(o_ptr);
}

void ObjectDialog::add_letter_label(QGridLayout *lay, QString id, int label_num, int row, int col)
{
    QLabel *lb = new QLabel(QString("%1)").arg(number_to_letter(label_num)));
    lb->setProperty("item_id", QVariant(id));
    lay->addWidget(lb, row, col);
}

void ObjectDialog::add_object_button(QGridLayout *lay, object_type *o_ptr, QString id, int row, int col)
{
    QString desc = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);
    QString style = QString("color: %1;").arg(get_object_color(o_ptr).name());
    style.append(QString("text-align: left; font-weight: bold;"));

    QPushButton *object_button = new QPushButton(desc);
    object_button->setProperty("item_id", QVariant(id));
    object_button->setStyleSheet(style);
    object_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(object_button, SIGNAL(clicked()), this, SLOT(object_click()));
    lay->addWidget(object_button, row, col);
}

void ObjectDialog::add_weight_label(QGridLayout *lay, object_type *o_ptr, int row, int col)
{
    // Add the weight
    QString weight_printout = (formatted_weight_string(o_ptr->weight * o_ptr->number));
    weight_printout.append(" lbs");
    QLabel *weight = new QLabel(weight_printout);
    weight->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    lay->addWidget(weight, row, col);
}

void ObjectDialog::add_help_label(QGridLayout *lay, QString id, int row, int col)
{
    QPushButton *help_button = new QPushButton;
    help_button->setIcon(QIcon(":/icons/lib/icons/help.png"));
    help_button->setObjectName(id);
    connect(help_button, SIGNAL(clicked()), this, SLOT(help_click()));
    lay->addWidget(help_button, row, col);
}

void ObjectDialog::add_message_area()
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

void ObjectDialog::reset_messages()
{

    int which_message = 1;

    /* Show the messages if they exist.
     * Check carefully to avoid crashes from
     * pointers larger than message list size.
     * We only want messages generated while in
     * the store->
     */

    message_one->setText(" ");
    message_two->setText(" ");
    message_three->setText(" ");

    int msg_size = message_list.size();

    for (int i = 0; i < msg_size; i++)
    {

        if (which_message > 3) break;
        bool next_line = FALSE;

        // Point to the last message
        message_type *current_message = &message_list[i];

        /* Stop when we hit messages that were posted
         * before the player went into the store.
         */
        if (operator==(current_message->message, last_message.message)) break;


        if (which_message == 1)
        {
            message_one->setText(QString("%1 %2") .arg(message_one->text()) .arg(current_message->message));
            if (message_one->text().length() > 120) next_line = TRUE;
        }
        else if (which_message == 2)
        {
            message_two->setText(QString("%1 %2") .arg(message_two->text()) .arg(current_message->message));
            if (message_two->text().length() > 120) next_line = TRUE;
        }
        else if (which_message == 3)
        {
            message_three->setText(QString("%1 %2") .arg(message_three->text()) .arg(current_message->message));
            if (message_three->text().length() > 120) next_line = TRUE;
        }

        // Skip down to the next line if necessary.
        if (!current_message->append || next_line) which_message++;
    }

}

/*
 *
 *
 * INVENTORY DIALOG
 *
 *
 */

void InvenDialog::update_inven_header()
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

    QString label_text = (QString("<b><h1>(Player Inventory)</h1><br><b><big>Burden: %1 lbs (%2% capacity)</big></b>")
                             .arg(formatted_weight_string(p_ptr->total_weight)) .arg(weight_percent));
    if (p_ptr->total_weight > max_capacity)
    {
        int overweight = p_ptr->total_weight - max_capacity;
        label_text.append(QString("<br>(%1 lbs overweight)")
                        .arg((formatted_weight_string(overweight))));
    }
    else if (p_ptr->total_weight < max_capacity)
    {
        int underweight = max_capacity - p_ptr->total_weight;
        label_text.append(QString("<br>(%1 lbs underweight)")
                        .arg(formatted_weight_string(underweight)));
    }
    header_inven->setText(label_text);
    header_inven->setAlignment(Qt::AlignCenter);
}

void InvenDialog::update_inven_list()
{
    int row = 0;

    clear_grid_layout(inven_list);

    for (int i = 0; i < INVEN_MAX_PACK; i++)
    {
        object_type *o_ptr = &inventory[i];
        if (!o_ptr->k_idx) continue;

        // Make an id for the item
        QString id = QString("i%1").arg(i);

        add_letter_label(inven_list, id, i, row, 0);
        add_object_button(inven_list, o_ptr, id, row, 1);
        add_weight_label(inven_list, o_ptr, row, 2);
        add_help_label(inven_list, id, row, 3);

        ++row;
    }
}

InvenDialog::InvenDialog(void)
{
    main_layout = new QVBoxLayout;

    //Build the header
    header_inven = new QLabel("Header");
    update_inven_header();
    main_layout->addWidget(header_inven);

    // add the messages
    message_area = new QWidget;
    main_layout->addWidget(message_area);
    add_message_area();
    reset_messages();

    this->reset_messages();

    // Add the list of inventory
    inven_list = new QGridLayout;
    update_inven_list();
    main_layout->addLayout(inven_list);

    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    main_layout->addItem(spacer);

    QPushButton *btn_close = new QPushButton("Close");
    main_layout->addWidget(btn_close);
    connect(btn_close, SIGNAL(clicked()), this, SLOT(reject()));

    setLayout(main_layout);
    setWindowTitle(tr("Inventory Menu"));

    this->exec();
}

void InvenDialog::inventory_update()
{
    update_inven_header();
    update_inven_list();
    reset_messages();
}

void do_cmd_inventory(void)
{
    InvenDialog();
    do_cmd_equipment();
}

/*
 *
 *
 * EQUIPMENT DIALOG
 *
 *
 */

void EquipDialog::update_equip_header()
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

    QString label_text = (QString("<b><h1>(Player Equipment)</h1><br><b><big>Burden: %1 lbs (%2% capacity)</big></b>")
                             .arg(formatted_weight_string(p_ptr->total_weight)) .arg(weight_percent));
    if (p_ptr->total_weight > max_capacity)
    {
        int overweight = p_ptr->total_weight - max_capacity;
        label_text.append(QString("<br>(%1 lbs overweight)")
                        .arg((formatted_weight_string(overweight))));
    }
    else if (p_ptr->total_weight < max_capacity)
    {
        int underweight = max_capacity - p_ptr->total_weight;
        label_text.append(QString("<br>(%1 lbs underweight)")
                        .arg(formatted_weight_string(underweight)));
    }
    header_equip->setText(label_text);
    header_equip->setAlignment(Qt::AlignCenter);
}

void EquipDialog::update_equip_list()
{
    int row = 0;

    clear_grid_layout(equip_list);

    for (int i = INVEN_WIELD; i < INVEN_TOTAL; i++)
    {
        object_type *o_ptr = &inventory[i];

        // Make an id for the item
        QString id = QString("i%1").arg(i);

        add_letter_label(equip_list, id, i, row, 0);
        if (o_ptr->k_idx)
        {
            add_object_button(equip_list, o_ptr, id, row, 1);
            add_weight_label(equip_list, o_ptr, row, 2);
            add_help_label(equip_list, id, row, 3);
        }
        else
        {
        //{sfhfhxfgxnxn}
        }

        ++row;
    }
}

EquipDialog::EquipDialog(void)
{
    main_layout = new QVBoxLayout;

    //Build the header
    header_equip = new QLabel("Header");
    update_equip_header();
    main_layout->addWidget(header_equip);

    // add the messages
    message_area = new QWidget;
    main_layout->addWidget(message_area);
    add_message_area();
    reset_messages();

    // Add the list of inventory
    equip_list = new QGridLayout;
    update_equip_list();
    main_layout->addLayout(equip_list);

    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    main_layout->addItem(spacer);

    QPushButton *btn_close = new QPushButton("Close");
    main_layout->addWidget(btn_close);
    connect(btn_close, SIGNAL(clicked()), this, SLOT(reject()));

    setLayout(main_layout);
    setWindowTitle(tr("Equipment Menu"));

    this->exec();
}

void EquipDialog::equipment_update()
{
    update_equip_header();
    update_equip_list();
    reset_messages();
}

void do_cmd_equipment(void)
{
    EquipDialog();

}
