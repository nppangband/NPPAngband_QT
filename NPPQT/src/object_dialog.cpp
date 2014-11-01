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


#include <src/object_dialog.h>


item_command item_command_info[ITEM_MAX] =
{
    // ITEM_EXAMINE
    {'a', CMD_EXAMINE},
    // ITEM_TAKEOFF
    {'b', CMD_TAKEOFF},
     // ITEM_WIELD
    {'c', CMD_WIELD},
    // ITEM_SWAP
    {'d', CMD_SWAP},
    // _ITEM_USE
    {'e', CMD_ITEM_USE},
    // ITEM_REFILL
    {'f', CMD_REFUEL},
    // ITEM_FIRE
    {'g', CMD_FIRE},
    // ITEM_FIRE_NEAR
    {'h', CMD_FIRE_NEAR},
    // ITEM_DROP
    {'i', CMD_DROP},
    // ITEM_PICKUP
    {'j', CMD_PICKUP},
    // ITEM_BROWSE
    {'k', CMD_BROWSE},
    // ITEM_STUDY
    {'l', CMD_STUDY},
    // ITEM_CAST
    {'m', CMD_CAST},
    // ITEM_DESTROY
    {'n', CMD_DESTROY},
    // ITEM_INSCRIBE
    {'o', CMD_INSCRIBE},
    // ITEM_UNINSCRIBE
    {'p', CMD_UNINSCRIBE},
    // ITEM_ACIVATE
    {'q', CMD_ACTIVATE},
    // ITEM_THROW
    {'r', CMD_THROW},
};

bool ObjectDialog::should_add_takeoff(object_type *o_ptr, s16b item_slot)
{
    if (!item_is_available(item_slot, NULL, USE_EQUIP | USE_QUIVER)) return (FALSE);
    if (o_ptr->is_known_cursed()) return (FALSE);
    return (TRUE);
}

bool ObjectDialog::should_add_wield(object_type *o_ptr, s16b item_slot)
{

    if (!item_is_available(item_slot, NULL, USE_INVEN | USE_FLOOR)) return (FALSE);
    if (!obj_can_wear(o_ptr)) return (FALSE);
    if (o_ptr->is_quest_artifact()) return (FALSE);

    return (TRUE);
}

bool ObjectDialog::should_add_swap(object_type *o_ptr, s16b item_slot)
{
    if (!adult_swap_weapons) return (FALSE);
    if (!obj_can_wear(o_ptr)) return (FALSE);

    if (item_slot >= INVEN_WIELD)
    {
        if(o_ptr->is_cursed()) return FALSE;
        if (game_mode == GAME_NPPANGBAND) return (FALSE);
        //GAME_NPPMORIA
        if ((item_slot == ITEM_WIELD) || (item_slot == ITEM_SWAP)) return TRUE;
        return (FALSE);
    }
    if (!o_ptr->inscription.contains("@x")) return FALSE;
    return TRUE;
}

bool ObjectDialog::should_add_use(object_type *o_ptr, s16b item_slot)
{
    (void)item_slot;

    if (!o_ptr->is_usable_item()) return (FALSE);


    // Make sure the wands/rods/staffs can be used.
    if (o_ptr->is_wand() || o_ptr->is_staff())
    {
        if (!o_ptr->could_have_charges()) return (FALSE);
    }
    if (o_ptr->is_rod())
    {
        if (!o_ptr->could_be_zapped())return (FALSE);
    }
    return (TRUE);
}

bool ObjectDialog::should_add_fire(object_type *o_ptr, s16b item_slot)
{
    // Only add add
    if (ammo_can_fire(o_ptr, item_slot))
    {
        if (!o_ptr->is_known_cursed()) return (TRUE);
    }

    return (FALSE);
}

bool ObjectDialog::should_add_refill(object_type *o_ptr, s16b item_slot)
{
    if (!item_is_available(item_slot, NULL, USE_INVEN | USE_FLOOR)) return (FALSE);
    return (obj_can_refill(o_ptr));
}

bool ObjectDialog::should_add_fire_near(object_type *o_ptr, s16b item_slot)
{
    if (!ammo_can_fire(o_ptr, item_slot)) return (FALSE);

    return (monster_target_exists());
}

bool ObjectDialog::should_add_drop(object_type *o_ptr, s16b item_slot)
{
    // On the floor
    if (!item_is_available(item_slot, NULL, USE_INVEN | USE_EQUIP | USE_QUIVER)) return (FALSE);

    // In the backpack
    if ((item_slot < INVEN_WIELD) && (item_slot >=0)) return (TRUE);

    if (IS_QUIVER_SLOT(item_slot) && p_ptr->state.cursed_quiver) return (FALSE);

    // OK if not known cursed
    return (!o_ptr->is_known_cursed());

}

bool ObjectDialog::should_add_pickup(object_type *o_ptr, s16b item_slot)
{
    (void)o_ptr;
    // Not the floor
    if (item_slot >= 0) return (FALSE);
    if (pack_is_full())
    {
        if (!inven_stack_okay(o_ptr, INVEN_MAX_PACK)) return (FALSE);
    }
    return (TRUE);
}

bool ObjectDialog::should_add_browse(object_type *o_ptr, s16b item_slot)
{
    (void)item_slot;
    if (!o_ptr->is_spellbook()) return (FALSE);
    if (o_ptr->tval != cp_ptr->spell_book) return (FALSE);
    if (p_ptr->timed[TMD_BLIND] || no_light()) return (FALSE);
    if (p_ptr->timed[TMD_CONFUSED]) return (FALSE);
    return (TRUE);
}

bool ObjectDialog::should_add_study(object_type *o_ptr, s16b item_slot)
{
    if (!should_add_browse(o_ptr, item_slot)) return (FALSE);
    return (player_can_use_book(o_ptr, FALSE));
}

bool ObjectDialog::should_add_cast(object_type *o_ptr, s16b item_slot)
{
    if (!should_add_browse(o_ptr, item_slot)) return (FALSE);
    return (player_can_use_book(o_ptr, TRUE));
}

bool ObjectDialog::should_add_destroy(object_type *o_ptr, s16b item_slot)
{
    if (!item_is_available(item_slot, NULL, USE_FLOOR | USE_INVEN)) return (FALSE);
    return (!o_ptr->is_known_artifact());
}

bool ObjectDialog::should_add_uninscribe(object_type *o_ptr, s16b item_slot)
{
    (void)item_slot;
    return (o_ptr->has_inscription());
}

bool ObjectDialog::should_add_activate(object_type *o_ptr, s16b item_slot)
{
    if (!obj_can_activate(o_ptr)) return (FALSE);
    if (adult_swap_weapons && item_slot == INVEN_SWAP_WEAPON) return (FALSE);
    return (TRUE);
}

bool ObjectDialog::should_add_throw(object_type *o_ptr, s16b item_slot)
{
    // On the floor
    if (!item_is_available(item_slot, NULL, USE_FLOOR | USE_INVEN | USE_QUIVER)) return (FALSE);
    if (IS_QUIVER_SLOT(item_slot) && p_ptr->state.cursed_quiver && !o_ptr->is_cursed())
    {
        return FALSE;
    }
    return (TRUE);
}

void ObjectDialog::add_examine(QGridLayout *lay, s16b item_slot, int row, int col)
{
    QString id = (QString("%1%2") .arg(item_command_info[ITEM_EXAMINE].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    new_button->setIcon(QIcon(":/icons/lib/icons/look.png"));
    new_button->setObjectName(id);
    new_button->setToolTip("Examine");
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}

void ObjectDialog::add_takeoff(QGridLayout *lay, s16b item_slot, int row, int col)
{
    QString id = (QString("%1%2") .arg(item_command_info[ITEM_TAKEOFF].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    new_button->setIcon(QIcon(":/icons/lib/icons/takeoff.png"));
    new_button->setObjectName(id);
    new_button->setToolTip("Take Off");
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}


void ObjectDialog::add_wield(QGridLayout *lay, s16b item_slot, int row, int col)
{
    QString id = (QString("%1%2") .arg(item_command_info[ITEM_WIELD].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    new_button->setIcon(QIcon(":/icons/lib/icons/wield.png"));
    new_button->setToolTip("Wield");
    new_button->setObjectName(id);
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}

void ObjectDialog::add_use(QGridLayout *lay, s16b item_slot, int row, int col)
{
    object_type *o_ptr = object_from_item_idx(item_slot);

    QString id = (QString("%1%2") .arg(item_command_info[ITEM_USE].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    if (o_ptr->tval == TV_SCROLL)
    {
        new_button->setIcon(QIcon(":/icons/lib/icons/scroll.png"));
        new_button->setToolTip("Read Scroll");
    }
    else if (o_ptr->tval == TV_POTION)
    {
        new_button->setIcon(QIcon(":/icons/lib/icons/potion.png"));
        new_button->setToolTip("Quaff Potion");
    }
    else if (o_ptr->tval == TV_WAND)
    {
        new_button->setIcon(QIcon(":/icons/lib/icons/wand.png"));
        new_button->setToolTip("Aim Wand");
    }
    else if (o_ptr->tval == TV_STAFF)
    {
        new_button->setIcon(QIcon(":/icons/lib/icons/staff.png"));
        new_button->setToolTip("Use Staff");
    }
    else if (o_ptr->tval == TV_ROD)
    {
        new_button->setIcon(QIcon(":/icons/lib/icons/rod.png"));
        new_button->setToolTip("Zap Rod");
    } //TV_FOOD
    else if (o_ptr->is_mushroom())
    {
        new_button->setIcon(QIcon(":/icons/lib/icons/mushroom.png"));
        new_button->setToolTip("Eat Mushroom");
    }
    else if (o_ptr->is_wine())
    {
        new_button->setIcon(QIcon(":/icons/lib/icons/wine.png"));
        new_button->setToolTip("Drink Wine");
    }
    else if (o_ptr->is_ale())
    {
        new_button->setIcon(QIcon(":/icons/lib/icons/ale.png"));
        new_button->setToolTip("Drink Ale");
    }
    else
    {
        new_button->setIcon(QIcon(":/icons/lib/icons/food.png"));
        new_button->setToolTip("Eat Food");
    }
    new_button->setObjectName(id);
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}


void ObjectDialog::add_swap(QGridLayout *lay, s16b item_slot, int row, int col)
{
    QString id = (QString("%1%2") .arg(item_command_info[ITEM_SWAP].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    new_button->setIcon(QIcon(":/icons/lib/icons/swap.png"));
    new_button->setToolTip("Swap");
    new_button->setObjectName(id);
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}

void ObjectDialog::add_refill(QGridLayout *lay, s16b item_slot, int row, int col)
{
    QString id = (QString("%1%2") .arg(item_command_info[ITEM_REFILL].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    new_button->setIcon(QIcon(":/icons/lib/icons/refill.png"));
    new_button->setToolTip("Refill");
    new_button->setObjectName(id);
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}

void ObjectDialog::add_fire(QGridLayout *lay, s16b item_slot, int row, int col)
{
    QString id = (QString("%1%2") .arg(item_command_info[ITEM_FIRE].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    new_button->setIcon(QIcon(":/icons/lib/icons/fire.png"));
    new_button->setToolTip("Fire");
    new_button->setObjectName(id);
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}

void ObjectDialog::add_fire_near(QGridLayout *lay, s16b item_slot, int row, int col)
{
    QString id = (QString("%1%2") .arg(item_command_info[ITEM_FIRE_NEAR].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    new_button->setIcon(QIcon(":/icons/lib/icons/fire_near.png"));
    new_button->setToolTip("Fire At Closest Target");
    new_button->setObjectName(id);
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}

void ObjectDialog::add_drop(QGridLayout *lay, s16b item_slot, int row, int col)
{
    QString id = (QString("%1%2") .arg(item_command_info[ITEM_DROP].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    new_button->setIcon(QIcon(":/icons/lib/icons/drop.png"));
    new_button->setToolTip("Drop");
    new_button->setObjectName(id);
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}

void ObjectDialog::add_pickup(QGridLayout *lay, s16b item_slot, int row, int col)
{
    QString id = (QString("%1%2") .arg(item_command_info[ITEM_PICKUP].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    new_button->setIcon(QIcon(":/icons/lib/icons/pickup.png"));
    new_button->setToolTip("Pick Up");
    new_button->setObjectName(id);
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}

void ObjectDialog::add_browse(QGridLayout *lay, s16b item_slot, int row, int col)
{
    QString id = (QString("%1%2") .arg(item_command_info[ITEM_BROWSE].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    new_button->setIcon(QIcon(":/icons/lib/icons/browse.png"));
    new_button->setToolTip("Browse");
    new_button->setObjectName(id);
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}

void ObjectDialog::add_study(QGridLayout *lay, s16b item_slot, int row, int col)
{
    QString id = (QString("%1%2") .arg(item_command_info[ITEM_STUDY].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    new_button->setIcon(QIcon(":/icons/lib/icons/study.png"));
    new_button->setToolTip("Study");
    new_button->setObjectName(id);
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}

void ObjectDialog::add_cast(QGridLayout *lay, s16b item_slot, int row, int col)
{
    QString id = (QString("%1%2") .arg(item_command_info[ITEM_CAST].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    new_button->setIcon(QIcon(":/icons/lib/icons/cast.png"));
    QString noun = cast_spell(MODE_SPELL_NOUN, cp_ptr->spell_book, 1, 0);
    QString verb = cast_spell(MODE_SPELL_VERB, cp_ptr->spell_book, 1, 0);
    new_button->setToolTip(QString("%1 a %2") .arg(verb) .arg(capitalize_first(noun)));
    new_button->setObjectName(id);
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}

void ObjectDialog::add_destroy(QGridLayout *lay, s16b item_slot, int row, int col)
{
    QString id = (QString("%1%2") .arg(item_command_info[ITEM_DESTROY].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    new_button->setIcon(QIcon(":/icons/lib/icons/destroy.png"));
    new_button->setToolTip("Destroy");
    new_button->setObjectName(id);
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}

void ObjectDialog::add_inscribe(QGridLayout *lay, s16b item_slot, int row, int col)
{
    QString id = (QString("%1%2") .arg(item_command_info[ITEM_INSCRIBE].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    new_button->setIcon(QIcon(":/icons/lib/icons/inscribe.png"));
    new_button->setToolTip("Inscribe");
    new_button->setObjectName(id);
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}

void ObjectDialog::add_uninscribe(QGridLayout *lay, s16b item_slot, int row, int col)
{
    QString id = (QString("%1%2") .arg(item_command_info[ITEM_UNINSCRIBE].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    new_button->setIcon(QIcon(":/icons/lib/icons/uninscribe.png"));
    new_button->setToolTip("Uninscribe");
    new_button->setObjectName(id);
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}

void ObjectDialog::add_activate(QGridLayout *lay, s16b item_slot, int row, int col)
{
    QString id = (QString("%1%2") .arg(item_command_info[ITEM_ACTIVATE].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    new_button->setIcon(QIcon(":/icons/lib/icons/activate.png"));
    new_button->setToolTip("Activate");
    new_button->setObjectName(id);
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}

void ObjectDialog::add_throw(QGridLayout *lay, s16b item_slot, int row, int col)
{
    QString id = (QString("%1%2") .arg(item_command_info[ITEM_THROW].action_char) .arg(item_slot));
    QPushButton *new_button = new QPushButton;
    new_button->setIcon(QIcon(":/icons/lib/icons/throw.png"));
    new_button->setToolTip("Throw");
    new_button->setObjectName(id);
    connect(new_button, SIGNAL(clicked()), this, SLOT(button_click()));
    lay->addWidget(new_button, row, col);
}

void ObjectDialog::do_buttons(QGridLayout *lay, object_type *o_ptr, s16b item_slot, s16b row, s16b col)
{
   add_examine(lay, item_slot, row, col++);
   if (should_add_takeoff(o_ptr, item_slot)) add_takeoff (lay, item_slot, row, col++);
   if (should_add_wield(o_ptr, item_slot)) add_wield(lay, item_slot, row, col++);
   if (should_add_swap(o_ptr, item_slot))  add_swap(lay, item_slot, row, col++);
   if (should_add_use(o_ptr, item_slot))  add_use(lay, item_slot, row, col++);
   if (should_add_refill(o_ptr, item_slot))  add_refill(lay, item_slot, row, col++);
   if (should_add_fire(o_ptr, item_slot))  add_fire(lay, item_slot, row, col++);
   if (should_add_fire_near(o_ptr, item_slot))  add_fire_near(lay, item_slot, row, col++);
   if (should_add_drop(o_ptr, item_slot))  add_drop(lay, item_slot, row, col++);
   if (should_add_pickup(o_ptr, item_slot))  add_pickup(lay, item_slot, row, col++);
   if (should_add_browse(o_ptr, item_slot))  add_browse(lay, item_slot, row, col++);
   if (should_add_study(o_ptr, item_slot))  add_study(lay, item_slot, row, col++);
   if (should_add_cast(o_ptr, item_slot))  add_cast(lay, item_slot, row, col++);
   if (should_add_destroy(o_ptr, item_slot))  add_destroy(lay, item_slot, row, col++);
   add_inscribe(lay, item_slot, row, col++);
   if (should_add_uninscribe(o_ptr, item_slot))  add_uninscribe(lay, item_slot, row, col++);
   if (should_add_activate(o_ptr, item_slot))  add_activate(lay, item_slot, row, col++);
   if (should_add_throw(o_ptr, item_slot))  add_throw(lay, item_slot, row, col++);

}


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



s16b ObjectDialog::idx_from_click(QString id)
{
    int o_idx = id.mid(1).toInt();
    s16b value = (s16b)o_idx;
    return (value);
}

/*
 * Receive an object click.  Figure out
 * which command it is, and then process
 * it.
 */

void ObjectDialog::object_click()
{
    QString id = QObject::sender()->objectName();
    int o_idx = idx_from_click(id);

    object_type *o_ptr = object_from_item_idx(o_idx);

    // TODO create menu for item, similar to knowledge screens;
}

void ObjectDialog::button_click()
{
    QString id = QObject::sender()->objectName();
    int o_idx = idx_from_click(id);

    QChar index = id[0];

    // Search for the matching command
    for (int i = 0; i < ITEM_MAX; i++)
    {
        QChar check = item_command_info[i].action_char;
        if (operator!=(check, index)) continue;

        process_command(o_idx, item_command_info[i].object_command);
        break;
    }
    // Do we need to update or delete the dialog?
    if (p_ptr->in_menu)update_dialog();
    else close_dialog();
}



void ObjectDialog::add_plain_label(QGridLayout *lay, QString label, int row, int col)
{
    QLabel *lb = new QLabel(label);
    lb->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    lay->addWidget(lb, row, col);
}


void ObjectDialog::add_letter_label(QGridLayout *lay, QChar location, int label_num, int row, int col)
{
    QString id = (QString("%1%2") .arg(location) .arg(label_num));

    QLabel *lb = new QLabel(QString("%1)").arg(number_to_letter(label_num)));
    lb->setProperty("item_id", QVariant(id));
    lay->addWidget(lb, row, col);
}

void ObjectDialog::add_object_button(QGridLayout *lay, object_type *o_ptr, QChar location, int label_num, int row, int col)
{
    QString desc = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);
    QString style = QString("color: %1;").arg(get_object_color(o_ptr).name());
    style.append(QString("text-align: left; font-weight: bold;"));

    QString id = (QString("%1%2") .arg(location) .arg(label_num));
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

void ObjectDialog::reset_messages(message_type last_message, QLabel *message_one, QLabel *message_two, QLabel *message_three)
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
 * Floor Layout
 *
 */
void ObjectDialog::update_floor_list(QGridLayout *lay, bool label, bool buttons)
{
    int row = 0;

    clear_grid_layout(lay);

    s16b this_o_idx, next_o_idx = 0;

    for (this_o_idx = dungeon_info[p_ptr->py][p_ptr->px].object_idx; this_o_idx; this_o_idx = next_o_idx)
    {
        object_type *o_ptr = &o_list[this_o_idx];
        if (!o_ptr->k_idx) continue;

        int col = 0;

        if (label) add_letter_label(lay, QChar('f'), this_o_idx, row, col++);
        add_object_button(lay, o_ptr, QChar('f'), this_o_idx, row, col++);
        add_weight_label(lay, o_ptr, row, col++);
        if (buttons) do_buttons(lay, o_ptr, -this_o_idx, row, col++);

        ++row;
    }
}

/*
 *
 * INVENTORY TAB
 *
 */
void ObjectDialog::update_inven_list(QGridLayout *lay, bool label, bool buttons)
{
    int row = 0;

    clear_grid_layout(lay);

    for (int i = 0; i < INVEN_MAX_PACK; i++)
    {
        object_type *o_ptr = &inventory[i];
        if (!o_ptr->k_idx) continue;

        int col = 0;

        if (label) add_letter_label(lay, QChar('i'), i, row, col++);
        add_object_button(lay, o_ptr, QChar('i'), i, row, col++);
        add_weight_label(lay, o_ptr, row, col++);
        if (buttons) do_buttons(lay, o_ptr, i, row, col++);

        ++row;
    }
}

/*
 *
 * EQUIPMENT TAB
 *
 */
void ObjectDialog::update_equip_list(QGridLayout *lay, bool label, bool buttons)
{
    int row = 0;

    clear_grid_layout(lay);

    for (int i = INVEN_WIELD; i < INVEN_TOTAL; i++)
    {
        object_type *o_ptr = &inventory[i];

        // Make an id for the item
        QString id = QString("i%1").arg(i);

        int col = 0;

        if (label) add_letter_label(lay, QChar('e'), i, row, col++);
        add_plain_label(lay, mention_use(i), row, col++);
        if (o_ptr->k_idx)
        {

            add_object_button(lay, o_ptr, QChar('e'), i, row, col++);
            add_weight_label(lay, o_ptr, row, col++);
            if (buttons) do_buttons(lay, o_ptr, i, row, col++);
        }
        else
        {
            add_plain_label(lay, QString("(nothing)"), row, col++);
        }

        ++row;
    }


}


void ObjectDialog::update_quiver_list(QGridLayout *lay, bool label, bool buttons)
{
    int row = 0;

    clear_grid_layout(lay);

    for (int i = QUIVER_START; i < QUIVER_END; i++)
    {
        object_type *o_ptr = &inventory[i];

        if (!o_ptr->k_idx) continue;

        // Make an id for the item
        QString id = QString("i%1").arg(i);

        int col = 0;

        if (label) add_letter_label(lay, QChar('e'), i, row, col++);
        add_plain_label(lay, mention_use(i), row, col++);

        add_object_button(lay, o_ptr, QChar('e'), i, row, col++);
        add_weight_label(lay, o_ptr, row, col++);
        if (buttons) do_buttons(lay, o_ptr, i, row, col++);

        ++row;
    }
}
