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
#include <src/player_info.h>



void InvenDialog::update_header_label()
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

    QString label_text = (QString("<b><big>(Player Inventory)</big>    Burden: %1 lbs (%2% capacity)</b>")
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

void InvenDialog::clear_grid_layout(QGridLayout *lay)
{
    QLayoutItem *item;
    while ((item = lay->takeAt(0)) != 0)
    {
        QWidget *wid = item->widget();
        if (wid) delete wid;
        delete item;
    }
}

void InvenDialog::inven_click()
{
    QObject *obj = QObject::sender();

    QString id = obj->property("item_id").toString();
    //process_item(id);
}

void InvenDialog::help_click()
{
    QString id = QObject::sender()->objectName();
    int o_idx = id.mid(1).toInt();

    object_type *o_ptr = &inventory[o_idx];
    object_info_screen(o_ptr);
}


InvenDialog::InvenDialog(void)
{
    main_layout = new QVBoxLayout;

    //Build the headers
    header_inven = new QLabel("Header");
    update_header_label();

    main_layout->addWidget(header_inven);
    int row = 0;

    inven_list = new QGridLayout;
    clear_grid_layout(inven_list);

    main_layout->addLayout(inven_list);

    for (int i = 0; i < INVEN_MAX_PACK; i++)
    {
        object_type *o_ptr = &inventory[i];
        if (!o_ptr->k_idx) continue;

        // Make an id for the item
        QString id = QString("i%1").arg(i);
        QLabel *lb = new QLabel(QString("%1)").arg(number_to_letter(i)));
        lb->setProperty("item_id", QVariant(id));
        inven_list->addWidget(lb, row, 0);

        QString desc = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);
        QString style = QString("color: %1;").arg(get_object_color(o_ptr).name());
        style.append(QString("text-align: left; font-weight: bold;"));

        QPushButton *inven_button = new QPushButton(desc);
        inven_button->setProperty("item_id", QVariant(id));

        inven_button->setStyleSheet(style);
        inven_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(inven_button, SIGNAL(clicked()), this, SLOT(inven_click()));
        inven_list->addWidget(inven_button, row, 1);

        QPushButton *help_button = new QPushButton;
        help_button->setIcon(QIcon(":/icons/lib/icons/help.png"));
        help_button->setObjectName(id);
        connect(help_button, SIGNAL(clicked()), this, SLOT(help_click()));
        inven_list->addWidget(help_button, row, 2);

        ++row;
    }

    main_layout->addLayout(inven_list);

    QPushButton *btn_close = new QPushButton("Close");
    main_layout->addWidget(btn_close);
    connect(btn_close, SIGNAL(clicked()), this, SLOT(reject()));

    setLayout(main_layout);
    setWindowTitle(tr("Inventory Menu"));

    this->exec();
}

void do_cmd_inventory(void)
{
    InvenDialog();
}
