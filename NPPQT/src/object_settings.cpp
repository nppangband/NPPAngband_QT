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
#include <src/object_settings.h>


void ObjectSettingsDialog::close_dialog()
{
    this->accept();
}

void ObjectSettingsDialog::update_squelch_setting(int id)
{
    k_ptr->squelch = id;
}

void ObjectSettingsDialog::add_squelch_buttons()
{

    QSpacerItem *vspacer = new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
    squelch_group = new QButtonGroup();
    squelch_never = new QRadioButton("Never Squelch");
    squelch_never->setToolTip("Never Squelch this item.  Object Pickup is determined by the pickup options under the user interface section.");
    squelch_pickup_no = new QRadioButton("Never Pickup");
    squelch_pickup_no->setToolTip("Never pickup this item, regardless of the various Pickup Option settings.  Item is not squelched.");
    squelch_pickup_yes = new QRadioButton("Always Pickup");
    squelch_pickup_yes->setToolTip("Always pickup this item, regardless of the various Pickup Option settings.  Item is not squelched.");
    squelch_always = new QRadioButton("Always Squelch");
    squelch_always->setToolTip("Automatically destroy this item when the player walks over it.");
    if (k_ptr->squelch == SQUELCH_NEVER) squelch_never->setChecked(true);
    else if (k_ptr->squelch == NO_SQUELCH_NEVER_PICKUP) squelch_pickup_no->setChecked(true);
    else if (k_ptr->squelch == NO_SQUELCH_ALWAYS_PICKUP) squelch_pickup_yes->setChecked(true);
    else  squelch_always->setChecked(true);  // SQUELCH_ALWAYS

    squelch_group->addButton(squelch_never, SQUELCH_NEVER);
    squelch_group->addButton(squelch_pickup_no, NO_SQUELCH_NEVER_PICKUP);
    squelch_group->addButton(squelch_pickup_yes, NO_SQUELCH_ALWAYS_PICKUP);
    squelch_group->addButton(squelch_always, SQUELCH_ALWAYS);

    squelch_vlay->addWidget(squelch_never);
    squelch_vlay->addWidget(squelch_pickup_no);
    squelch_vlay->addWidget(squelch_pickup_yes);
    squelch_vlay->addWidget(squelch_always);
    squelch_vlay->addSpacerItem(vspacer);
    connect(squelch_group, SIGNAL(buttonClicked(int)), this, SLOT(update_squelch_setting(int)));

}


ObjectSettingsDialog::ObjectSettingsDialog(int o_idx)
{
    o_ptr = object_from_item_idx(o_idx);

    // Paranoia
    if (!o_ptr->k_idx) return;

    object_index = o_idx;

    k_ptr = &k_info[o_ptr->k_idx];

    main_layout = new QVBoxLayout;
    main_across = new QHBoxLayout;


    header_main = new QLabel("<b><h2>Object Settings Menu</b></h2>");
    header_main->setAlignment(Qt::AlignCenter);
    main_layout->addWidget(header_main);
    main_layout->addLayout(main_across);

    // Add squelch settings, except for the instant artifacts
    if (!(k_ptr->k_flags3 & TR3_INSTA_ART))
    {
        squelch_vlay = new QVBoxLayout;
        main_across->addLayout(squelch_vlay);
        add_squelch_buttons();
    }

    setLayout(main_layout);
    setWindowTitle(tr("Object Menu"));

    this->exec();
}

void object_settings(int o_idx)
{
    ObjectSettingsDialog *dlg = new ObjectSettingsDialog(o_idx);
    delete dlg;
}
