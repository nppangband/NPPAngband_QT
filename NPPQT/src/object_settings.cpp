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
#include <src/squelch.h>

void ObjectSettingsDialog::update_ego_setting(int id)
{
    ego_item_type *e_ptr = &e_info[o_ptr->ego_num];
    if (id) e_ptr->squelch = TRUE;
    else e_ptr->squelch = FALSE;
}

void ObjectSettingsDialog::add_ego_buttons()
{
    if (!o_ptr->ego_num) return;

    ego_item_type *e_ptr = &e_info[o_ptr->ego_num];

    ego_group = new QButtonGroup();
    QLabel *ego_label = new QLabel(QString("<b><big>Ego Item Settings</b></big>"));
    ego_label->setAlignment(Qt::AlignCenter);
    ego_label->setToolTip("The settings below allow the player to specify if this type of ego-item should be automatically destroyed upon identification.");
    QLabel *ego_name = new QLabel(QString("<b>%1</b>") .arg(get_ego_name(o_ptr)));
    ego_name->setAlignment(Qt::AlignCenter);
    ego_buttons->addWidget(ego_label);
    ego_buttons->addWidget(ego_name);

    ego_no = new QRadioButton("Do Not Destroy");
    ego_no->setToolTip("Do not destroy this ego-item upon identification.");
    ego_yes = new QRadioButton("Destroy");
    squelch_pickup_no->setToolTip("Destroy this ego-item upon identification.");

    if (!e_ptr->squelch) ego_no->setChecked(true);
    else  ego_yes->setChecked(true);

    ego_group->addButton(ego_no, FALSE);
    ego_group->addButton(ego_yes, TRUE);

    ego_buttons->addWidget(ego_no);
    ego_buttons->addWidget(ego_yes);
    ego_buttons->addSpacerItem(vspacer);

    connect(ego_group, SIGNAL(buttonClicked(int)), this, SLOT(update_ego_setting(int)));

}

void ObjectSettingsDialog::update_quality_setting(int id)
{
    byte squelch_type = squelch_type_of(o_ptr);
    squelch_level[squelch_type] = id;
}

void ObjectSettingsDialog::add_quality_buttons()
{
    // First make sure we need the object uses these settings
    byte squelch_type = squelch_type_of(o_ptr);
    if (squelch_type == PS_TYPE_MAX) return;

    bool limited_types = FALSE;
    if (squelch_type == PS_TYPE_AMULET) limited_types = TRUE;
    else if (squelch_type == PS_TYPE_RING) limited_types = TRUE;

    quality_group = new QButtonGroup();
    QLabel *quality_label = new QLabel(QString("<b><big>Quality Squelch Settings</b></big>"));
    quality_label->setAlignment(Qt::AlignCenter);
    quality_label->setToolTip("The settings below allow the player to automatically destroy an item on identification, or pseudo-id, based on the quality of that item.");
    QLabel *quality_name = new QLabel(QString("<b>%1</b>") .arg(quality_squelch_type_label(o_ptr)));
    quality_name->setAlignment(Qt::AlignCenter);
    quality_buttons->addWidget(quality_label);
    quality_buttons->addWidget(quality_name);
    quality_none = new QRadioButton(quality_values[SQUELCH_NONE].name);
    squelch_never->setToolTip("Do not automatically destroy items of this type based on the quality of that item.");
    quality_cursed = new QRadioButton(quality_values[SQUELCH_CURSED].name);
    quality_cursed->setToolTip("Automatically destroy cursed or broken items upon identification or pseudo-id.");
    quality_average = new QRadioButton(quality_values[SQUELCH_AVERAGE].name);
    quality_average->setToolTip("Automatically destroy cursed or average items of this type  upon identification or pseudo-id.");
    quality_good_strong = new QRadioButton(quality_values[SQUELCH_GOOD_STRONG].name);
    quality_good_strong->setToolTip("Automatically destroy cursed, average, or good items of this type upon identification or pseudo-id.");
    quality_good_weak = new QRadioButton(quality_values[SQUELCH_GOOD_WEAK].name);
    quality_good_weak->setToolTip("Automatically destroy cursed, average, or good items of this type upon identification or pseudo-id.");
    quality_all_but_artifact = new QRadioButton(quality_values[SQUELCH_ALL].name);
    quality_all_but_artifact->setToolTip("Automatically destroy all items of this type, except artifacts, upon identification or pseudo-id.");
    if (squelch_level[squelch_type] == SQUELCH_NONE) quality_none->setChecked(TRUE);
    else if (squelch_level[squelch_type] == SQUELCH_CURSED) quality_cursed->setChecked(TRUE);
    else if (squelch_level[squelch_type] == SQUELCH_AVERAGE) quality_average->setChecked(TRUE);
    else if (squelch_level[squelch_type] == SQUELCH_GOOD_STRONG) quality_good_strong->setChecked(TRUE);
    else if (squelch_level[squelch_type] == SQUELCH_GOOD_WEAK) quality_good_weak->setChecked(TRUE);
    else quality_all_but_artifact->setChecked(TRUE); // SQUELCH_ALL

    quality_group->addButton(quality_none, SQUELCH_NONE);
    quality_group->addButton(quality_cursed, SQUELCH_CURSED);
    quality_group->addButton(quality_average, SQUELCH_AVERAGE);
    quality_group->addButton(quality_good_strong, SQUELCH_GOOD_STRONG);
    quality_group->addButton(quality_good_weak, SQUELCH_GOOD_WEAK);
    quality_group->addButton(quality_all_but_artifact, SQUELCH_ALL);

    quality_buttons->addWidget(quality_none);
    quality_buttons->addWidget(quality_cursed);
    if (!limited_types)
    {
        quality_buttons->addWidget(quality_average);
        if (cp_ptr->pseudo_id_heavy()) quality_buttons->addWidget(quality_good_strong);
        else quality_buttons->addWidget(quality_good_weak);
    }
    quality_buttons->addWidget(quality_all_but_artifact);

    connect(quality_group, SIGNAL(buttonClicked(int)), this, SLOT(update_quality_setting(int)));
}

void ObjectSettingsDialog::update_squelch_setting(int id)
{
    k_ptr->squelch = id;
}


void ObjectSettingsDialog::add_squelch_buttons()
{
    squelch_group = new QButtonGroup();
    QLabel *squelch_label = new QLabel(QString("<b><big>Object Squelch Settings</b></big>"));
    squelch_label->setAlignment(Qt::AlignCenter);
    squelch_label->setToolTip("The settings below allow the player to specify if they want to automatically destroy, pickup, or ignore an item when the player walks over it.");
    squelch_buttons->addWidget(squelch_label);
    squelch_never = new QRadioButton("Never Destroy");
    squelch_never->setToolTip("Never destroy this item.  Object Pickup is determined by the pickup options under the user interface section.");
    squelch_pickup_no = new QRadioButton("Never Pickup");
    squelch_pickup_no->setToolTip("Never pickup this item, regardless of the various Pickup Option settings.  Item is not destroyed.");
    squelch_pickup_yes = new QRadioButton("Always Pickup");
    squelch_pickup_yes->setToolTip("Always pickup this item, regardless of the various Pickup Option settings.");
    squelch_always = new QRadioButton("Always Destroy");
    squelch_always->setToolTip("Automatically destroy this item when the player walks over it.");

    if (k_ptr->squelch == SQUELCH_NEVER) squelch_never->setChecked(true);
    else if (k_ptr->squelch == NO_SQUELCH_NEVER_PICKUP) squelch_pickup_no->setChecked(true);
    else if (k_ptr->squelch == NO_SQUELCH_ALWAYS_PICKUP) squelch_pickup_yes->setChecked(true);
    else  squelch_always->setChecked(true);  // SQUELCH_ALWAYS

    squelch_group->addButton(squelch_never, SQUELCH_NEVER);
    squelch_group->addButton(squelch_pickup_no, NO_SQUELCH_NEVER_PICKUP);
    squelch_group->addButton(squelch_pickup_yes, NO_SQUELCH_ALWAYS_PICKUP);
    squelch_group->addButton(squelch_always, SQUELCH_ALWAYS);


    squelch_buttons->addWidget(squelch_never);
    squelch_buttons->addWidget(squelch_pickup_no);
    squelch_buttons->addWidget(squelch_pickup_yes);
    squelch_buttons->addWidget(squelch_always);
    squelch_buttons->addSpacerItem(vspacer);

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
    squelch_vlay = new QVBoxLayout;
    vspacer = new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);

    QLabel *header_main = new QLabel("<b><h2>Object Settings Menu</b></h2>");
    header_main->setAlignment(Qt::AlignCenter);
    QLabel *object_name = new QLabel(QString("<big>%1</big>") .arg(object_desc(o_ptr, ODESC_FULL | ODESC_SINGULAR)));
    object_name->setAlignment(Qt::AlignCenter);
    main_layout->addWidget(header_main);
    main_layout->addWidget(object_name);
    main_layout->addLayout(main_across);
    main_across->addLayout(squelch_vlay);

    // Add squelch settings, except for the instant artifacts
    if (!(k_ptr->k_flags3 & TR3_INSTA_ART))
    {
        squelch_buttons = new QVBoxLayout;
        squelch_vlay->addLayout(squelch_buttons);
        add_squelch_buttons();

        quality_buttons = new QVBoxLayout;
        squelch_vlay->addLayout(quality_buttons);
        add_quality_buttons();

        ego_buttons = new QVBoxLayout;
        squelch_vlay->addLayout(ego_buttons);
        add_ego_buttons();
    }

    buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, SIGNAL(rejected()), this, SLOT(close()));
    main_layout->addStretch();
    main_layout->addWidget(buttons);

    setLayout(main_layout);
    setWindowTitle(tr("Object Menu"));
}

void object_settings(int o_idx)
{
    ObjectSettingsDialog *dlg = new ObjectSettingsDialog(o_idx);
    dlg->exec();
    delete dlg;
}
