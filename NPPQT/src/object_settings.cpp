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
#include <src/player_command.h>
#include <src/squelch.h>
#include <QCheckBox>

verify_data verification_data[] =
{
    //  VERIFY_DESTROY
    {CMD_DESTROY, "Confirm Destroy", "Require confirmation before destroying this item."},
    // VERIFY_SELL
    {CMD_MAX, "Confirm Sell", "Require verification before selling this item in a store."},
    // VERIFY_USE
    {CMD_ITEM_USE, "Confirm Use", "Require confirmation before using this item."},
    // VERIFY_TAKEOFF
    {CMD_TAKEOFF, "Confirm Take Off", "Require confirmation before removing when this item is worn."},
    // VERIFY_WIELD
    {CMD_WIELD, "Confirm Wield", "Require confirmation before wielding this item."},
    // VERIFY_THROW
    {CMD_THROW, "Confirm Throw", "Require confirmation before throwing this item"},
    // VERIFY_DROP
    {CMD_DROP, "Confirm Drop", "Require confirmation before dropping this item"},
    // VERIFY_PICKUP
    {CMD_PICKUP, "Confirm Pick Up", "Require confirmation before picking up this item"},
    // VERIFY_ACTIVATE
    {CMD_ACTIVATE, "Confirm Activate", "Require confirmation before activating this item."},
    // VERIFY_FIRE
    {CMD_FIRE, "Confirm Fire", "Require confirmation before firing this ammunition"},
    // VERIFY_FIRE_NEAR
    {CMD_FIRE_NEAR, "Confirm Fire Near", "Require confirmation before firing this ammunition at the nearest target."},
    // VERIFY_REFILL
    {CMD_REFUEL, "Confirm Refill", "Require confirmation before using this item to refill your light source."},
    // VERIFY_STUDY
    {CMD_STUDY, "Confirm Study", "Require confirmation before learning a spell from this book."},
    // VERIFY_CAST
    {CMD_CAST, "Confirm Cast", "Require confirmation before casting a spell from this book."},
    // VERIFY_SWAP
    {CMD_SWAP, "Use as Swap Weapon", "Set up this weapon to be wielded with the swap command."},
    // VERIFY_WIELD_QUIVER
    {CMD_MAX, "Put in Quiver", "Choose to put throwing weapon in quiver rather than wield.  Automatically put ammunition in the quiver."},
     // VERIFY_ALL
    {CMD_MAX, "Confirm All", "Confirm before all commands using this item."},
     // RECHARGE_NOTIFY
    {CMD_MAX, "Notify when recharged", "Notify the player when the item is recharged"},
     // VERIFY_UNUSED_1
    {CMD_MAX, "Unused", "Unused"},
     // VERIFY_UNUSED_2
    {CMD_MAX, "Unused", "Unused"},
};


void ObjectSettingsDialog::update_object_type_settings(int id, bool checked)
{
    o_ptr->use_verify[id] = checked;
}

void ObjectSettingsDialog::update_object_kind_settings(int id, bool checked)
{
    k_ptr->use_verify[id] = checked;
}

void ObjectSettingsDialog::add_kind_checkbox(byte which_ver)
{
    verify_data *v_ptr = &verification_data[which_ver];

    QCheckBox *this_checkbox = new QCheckBox(v_ptr->box_label);
    this_checkbox->setToolTip(v_ptr->box_tooltip);
    if (k_ptr->use_verify[which_ver]) this_checkbox->setChecked(TRUE);
    else this_checkbox->setChecked(FALSE);
    object_kind_group->addButton(this_checkbox, which_ver);
    object_kind_ver->addWidget(this_checkbox);
}

void ObjectSettingsDialog::add_type_checkbox(byte which_ver)
{
    verify_data *v_ptr = &verification_data[which_ver];

    QCheckBox *this_checkbox = new QCheckBox(v_ptr->box_label);
    this_checkbox->setToolTip(v_ptr->box_tooltip);
    if (o_ptr->use_verify[which_ver]) this_checkbox->setChecked(TRUE);
    else this_checkbox->setChecked(FALSE);
    object_type_group->addButton(this_checkbox, which_ver);
    object_type_ver->addWidget(this_checkbox);

    add_kind_checkbox(which_ver);
}

void ObjectSettingsDialog::add_object_verifications()
{
    object_type_group = new QButtonGroup();
    object_type_group->setExclusive(FALSE);
    QLabel *object_type_label = new QLabel(QString("<b><big>   Object Settings   </big></b>"));
    object_type_label->setAlignment(Qt::AlignCenter);
    object_type_label->setToolTip("Check boxes below to enable these options for this patticular object.");
    object_type_ver->addWidget(object_type_label);

    object_kind_group = new QButtonGroup();
    object_kind_group->setExclusive(FALSE);
    QLabel *object_kind_label = new QLabel(QString("<b><big>   Object Template Settings   </big></b>"));
    object_kind_label->setAlignment(Qt::AlignCenter);
    object_kind_label->setToolTip("Check boxes below to enable these options for ALL objects of this type.");
    object_kind_ver->addWidget(object_kind_label);


    if (!o_ptr->is_artifact())   add_type_checkbox(VERIFY_DESTROY);
    add_type_checkbox(VERIFY_SELL);
    if (o_ptr->is_usable_item()) add_type_checkbox(VERIFY_USE);

    if (o_ptr->is_wearable())
    {
        add_type_checkbox(VERIFY_TAKEOFF);
        add_type_checkbox(VERIFY_WIELD);
    }

    add_type_checkbox(VERIFY_THROW);
    add_type_checkbox(VERIFY_DROP);
    add_type_checkbox(VERIFY_PICKUP);

    if (obj_is_activatable(o_ptr))  add_type_checkbox(VERIFY_ACTIVATE);

    if (o_ptr->is_ammo())
    {
        add_type_checkbox(VERIFY_FIRE);
        add_type_checkbox(VERIFY_FIRE_NEAR);
    }

    if (o_ptr->is_fuel())  add_type_checkbox(VERIFY_REFILL);

    if (o_ptr->is_spellbook())  add_type_checkbox(VERIFY_STUDY);

    if (o_ptr->is_weapon())   add_type_checkbox(AUTO_SWAP);

    if (o_ptr->is_ammo() || is_throwing_weapon(o_ptr))
    {
        add_type_checkbox(AUTO_WIELD_QUIVER);
    }

    if (o_ptr->is_rod() || obj_is_activatable(o_ptr))
    {
        add_type_checkbox(RECHARGE_NOTIFY);
    }
    add_type_checkbox(VERIFY_ALL);

    connect(object_type_group, SIGNAL(buttonToggled(int, bool)), this, SLOT(update_object_type_settings(int, bool)));
    connect(object_kind_group, SIGNAL(buttonToggled(int, bool)), this, SLOT(update_object_kind_settings(int, bool)));

    QSpacerItem *vspacer = new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
    object_type_ver->addSpacerItem(vspacer);
    QSpacerItem *vspacer1 = new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
    object_kind_ver->addSpacerItem(vspacer1);
}

void ObjectSettingsDialog::update_ego_setting(int id)
{
    ego_item_type *e_ptr = &e_info[o_ptr->ego_num];
    if (id) e_ptr->squelch = TRUE;
    else e_ptr->squelch = FALSE;
}

void ObjectSettingsDialog::add_ego_buttons(QVBoxLayout *ego_buttons)
{
    if (!o_ptr->ego_num) return;

    ego_item_type *e_ptr = &e_info[o_ptr->ego_num];

    ego_group = new QButtonGroup();

    QLabel *ego_label = new QLabel(QString("<b><big>   Ego Item Settings   </big></b>"));
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

    QSpacerItem *vspacer = new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
    ego_buttons->addSpacerItem(vspacer);

    connect(ego_group, SIGNAL(buttonClicked(int)), this, SLOT(update_ego_setting(int)));
}

void ObjectSettingsDialog::update_quality_setting(int id)
{
    byte squelch_type = squelch_type_of(o_ptr);
    squelch_level[squelch_type] = id;
}

void ObjectSettingsDialog::add_quality_buttons(QVBoxLayout *quality_buttons)
{
    // First make sure we need the object uses these settings
    byte squelch_type = squelch_type_of(o_ptr);
    if (squelch_type == PS_TYPE_MAX) return;

    bool limited_types = FALSE;
    if (squelch_type == PS_TYPE_AMULET) limited_types = TRUE;
    else if (squelch_type == PS_TYPE_RING) limited_types = TRUE;
    quality_group = new QButtonGroup();
    QLabel *quality_label = new QLabel(QString("<b><big>   Quality Squelch Settings   </big></b>"));
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

    QSpacerItem *vspacer = new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
    quality_buttons->addSpacerItem(vspacer);

    connect(quality_group, SIGNAL(buttonClicked(int)), this, SLOT(update_quality_setting(int)));
}

void ObjectSettingsDialog::update_squelch_setting(int id)
{
    k_ptr->squelch = id;
}

void ObjectSettingsDialog::add_squelch_buttons(QVBoxLayout *squelch_buttons)
{
    squelch_group = new QButtonGroup();
    QLabel *squelch_label = new QLabel(QString("<b><big>   Object Squelch Settings   </big></b>"));
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

    QSpacerItem *vspacer = new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);

    squelch_buttons->addWidget(squelch_never);
    squelch_buttons->addWidget(squelch_pickup_no);
    squelch_buttons->addWidget(squelch_pickup_yes);
    squelch_buttons->addWidget(squelch_always);
    squelch_buttons->addSpacerItem(vspacer);

    connect(squelch_group, SIGNAL(buttonClicked(int)), this, SLOT(update_squelch_setting(int)));

}

ObjectSettingsDialog::ObjectSettingsDialog(s16b o_idx)
{
    o_ptr = object_from_item_idx(o_idx);

    // Paranoia
    if (!o_ptr->k_idx) return;

    k_ptr = &k_info[o_ptr->k_idx];

    QVBoxLayout *main_layout = new QVBoxLayout;

    QLabel *header_main = new QLabel("<b><h2>Object Settings Menu</b></h2>");
    header_main->setAlignment(Qt::AlignCenter);
    main_layout->addWidget(header_main);

    QLabel *object_name = new QLabel(QString("<big>%1</big>") .arg(object_desc(o_ptr, ODESC_FULL | ODESC_SINGULAR)));
    object_name->setAlignment(Qt::AlignCenter);
    main_layout->addWidget(object_name);

    QHBoxLayout *main_across = new QHBoxLayout;
    QVBoxLayout *squelch_vlay = new QVBoxLayout;
    object_type_ver = new QVBoxLayout;
    object_kind_ver = new QVBoxLayout;
    main_layout->addLayout(main_across);
    main_across->addLayout(object_type_ver);
    main_across->addLayout(object_kind_ver);
    main_across->addLayout(squelch_vlay);

    add_object_verifications();

    // Add squelch settings, except for the instant artifacts
    if (!(k_ptr->k_flags3 & TR3_INSTA_ART))
    {
        QVBoxLayout *squelch_buttons = new QVBoxLayout;
        squelch_vlay->addLayout(squelch_buttons);
        add_squelch_buttons(squelch_buttons);

        QVBoxLayout *quality_buttons = new QVBoxLayout;
        squelch_vlay->addLayout(quality_buttons);
        add_quality_buttons(quality_buttons);

        QVBoxLayout *ego_buttons = new QVBoxLayout;
        squelch_vlay->addLayout(ego_buttons);
        add_ego_buttons(ego_buttons);
    }

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, SIGNAL(rejected()), this, SLOT(close()));
    main_layout->addStretch();
    main_layout->addWidget(buttons);

    setLayout(main_layout);
    setWindowTitle(tr("Object Menu"));
}

void object_settings(s16b o_idx)
{

    ObjectSettingsDialog *dlg = new ObjectSettingsDialog(o_idx);
    dlg->exec();
    delete dlg;
}

/*
 * Verify the choice of an item.
 *
 * The item can be negative to mean "item on floor".
 */
static bool verify_item(int item, int command)
{

    QString prompt;

    object_type *o_ptr = object_from_item_idx(item);
    bool do_plural = FALSE;

    QString o_name;

    if (command == VERIFY_USE)
    {
        if (o_ptr->tval == TV_WAND) prompt = "Really aim ";
        else if (o_ptr->tval == TV_FOOD) prompt = "Really eat ";
        else if (o_ptr->tval == TV_POTION) prompt = "Really quaff ";
        else if (o_ptr->tval == TV_ROD) prompt = "Really zap ";
        else if (o_ptr->tval == TV_SCROLL) {prompt = "Really read ";}
        else /* TV_STAFF) */prompt = "Really use ";
    }

    /* Get the possible command prompts */
    else switch (command)
    {

        case VERIFY_ACTIVATE:	{prompt = "Really activate ";break;}
        case VERIFY_DROP:	{prompt = "Really drop "; do_plural = TRUE;	break;}
        case VERIFY_FIRE_NEAR:
        case VERIFY_FIRE:	{prompt = "Really fire ";	break;}
        case VERIFY_REFILL:	{prompt = "Really fuel ";	break;}
        case VERIFY_DESTROY:	{prompt = "Really destroy "; do_plural = TRUE;	break;}
        case VERIFY_TAKEOFF:	{prompt = "Really take off ";break;}
        case VERIFY_THROW:	{prompt = "Really throw ";	break;}
        case VERIFY_PICKUP:  {prompt = "Really pick up "; do_plural = TRUE;	break;}
        case VERIFY_STUDY:  {prompt = "Really study from ";	break;}
        case VERIFY_CAST:
        {
            QString noun = cast_spell(MODE_SPELL_NOUN, cp_ptr->spell_book, 1, 0);
            QString verb = cast_spell(MODE_SPELL_VERB, cp_ptr->spell_book, 1, 0);
            prompt = (QString("Really %1 a %2 from ") .arg(verb) .arg(noun));	break;
        }
        case VERIFY_WIELD:
        {
            int slot = wield_slot(o_ptr);

            /* Where would the item go? INVEN_MAIN_WEAPON */
            if (slot == INVEN_WIELD)
            {
                if (o_ptr->is_bow()) prompt = "Really shoot with ";
                else prompt = "Really wield ";
            }
            else if (slot == INVEN_BOW) 	prompt = "Really shoot with ";
            else if ((slot == INVEN_LIGHT)	|| (slot == INVEN_ARM))
            {
                prompt = "Really hold ";
            }
            else if ((slot == INVEN_LEFT) || (slot == INVEN_RIGHT) || (slot == INVEN_NECK))
            {
                prompt = "Really put on ";
            }
            else if (slot >= QUIVER_START)
            {
                do_plural = TRUE;
                prompt = "Really place in quiver ";
            }
            else prompt = "Really wear ";

            break;

        }
        default: 	{prompt = "Really try "; 	break;}
    }

    if (do_plural) o_name = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);
    else o_name = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL | ODESC_SINGULAR);

    // Add the object name
    prompt.append(o_name);
    prompt.append("?");

    /* Query */
    return (get_check(prompt));
}

/*
 * allow user to "verify" certain choices.
 *
 * The item can be negative to mean "item on floor".
 */
bool get_item_allow(int item, int verify_command)
{
    object_type *o_ptr = object_from_item_idx(item);

    int last_command = VERIFY_MAX;

    /* Check for a verify command */
    for (int i = 0; i < VERIFY_MAX; i++)
    {
        // Find matching command
        if (verify_command != i) continue;

        if (!o_ptr->use_verify[i]) continue;

        if (!verify_item(item, verify_command)) return (FALSE);

        last_command = i;
    }

    if (o_ptr->use_verify[VERIFY_ALL])
    {
        if (!verify_item(item, last_command)) return (FALSE);
    }

    /* Allow it */
    return (TRUE);
}

// Copy the default object kind settings to the object
void apply_object_kind_settings(object_type *o_ptr)
{
    // Paranoia
    if (!o_ptr->k_idx) return;

    object_kind *k_ptr = &k_info[o_ptr->k_idx];

    // Only copy the true ones.
    for (int i = 0; i < VERIFY_MAX; i++)
    {
        if (!k_ptr->use_verify[i]) continue;
        o_ptr->use_verify[i] = TRUE;
    }
}