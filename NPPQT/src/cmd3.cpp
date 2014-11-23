/* File: cmd3.cpp */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *                    Jeff Greene, Diego Gonzalez
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
#include "src/player_command.h"
#include "src/dun_traps.h"
#include "src/cmds.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>


void TrapSelectDialog::trap_choice_sturdy(void)     {trap_choice = FEAT_MTRAP_STURDY;       this->accept();}
void TrapSelectDialog::trap_choice_slowing(void)    {trap_choice = FEAT_MTRAP_SLOWING;      this->accept();}
void TrapSelectDialog::trap_choice_confusion(void)  {trap_choice = FEAT_MTRAP_CONFUSION;    this->accept();}
void TrapSelectDialog::trap_choice_poison(void)     {trap_choice = FEAT_MTRAP_POISON;       this->accept();}
void TrapSelectDialog::trap_choice_life_drain(void) {trap_choice = FEAT_MTRAP_DRAIN_LIFE;   this->accept();}
void TrapSelectDialog::trap_choice_lightning(void)  {trap_choice = FEAT_MTRAP_ELEC;         this->accept();}
void TrapSelectDialog::trap_choice_explosive(void)  {trap_choice = FEAT_MTRAP_EXPLOSIVE;    this->accept();}
void TrapSelectDialog::trap_choice_portal(void)     {trap_choice = FEAT_MTRAP_PORTAL;       this->accept();}
void TrapSelectDialog::trap_choice_dispell(void)    {trap_choice = FEAT_MTRAP_DISPEL_M;     this->accept();}

void TrapSelectDialog::on_dialog_buttons_pressed(QAbstractButton *)
{
    trap_choice = -1;
    this->reject();
}


/*
 * Choose advanced monster trap type
 */
TrapSelectDialog::TrapSelectDialog(void)
{
    int num = 1 + (p_ptr->lev / 6);

    trap_choice = -1;

    QLabel *main_prompt = new QLabel(QString("<b><big>Please select an advanced trap</big></b><br>"));
    main_prompt->setAlignment(Qt::AlignCenter);

    QVBoxLayout *vlay = new QVBoxLayout;

    vlay->addWidget(main_prompt);

    vlay->addStretch();

    // Add the "Sturdy Trap" button
    QPushButton *sturdy_button = new QPushButton("&Resilient Trap");
    sturdy_button->setToolTip(apply_monster_trap(FEAT_MTRAP_STURDY, 0, 0, MODE_DESCRIBE));
    connect(sturdy_button, SIGNAL(clicked()), this, SLOT(trap_choice_sturdy()));
    vlay->addWidget(sturdy_button);

    if (num > 1)
    {
        // Add the "Slowing Trap" button
        QPushButton *slowing_button = new QPushButton("&Slowing Trap");
        slowing_button->setToolTip(apply_monster_trap(FEAT_MTRAP_SLOWING, 0, 0, MODE_DESCRIBE));
        connect(slowing_button, SIGNAL(clicked()), this, SLOT(trap_choice_slowing()));
        vlay->addWidget(slowing_button);
    }

    if (num > 2)
    {
        // Add the "Confusion Trap" button
        QPushButton *cofusion_button = new QPushButton("&Confusion Trap");
        cofusion_button->setToolTip(apply_monster_trap(FEAT_MTRAP_CONFUSION, 0, 0, MODE_DESCRIBE));
        connect(cofusion_button, SIGNAL(clicked()), this, SLOT(trap_choice_confusion()));
        vlay->addWidget(cofusion_button);
    }

    if (num > 3)
    {
        // Add the "Poison Trap" button
        QPushButton *poison_button = new QPushButton("&Poison Trap");
        poison_button->setToolTip(apply_monster_trap(FEAT_MTRAP_POISON, 0, 0, MODE_DESCRIBE));
        connect(poison_button, SIGNAL(clicked()), this, SLOT(trap_choice_poison()));
        vlay->addWidget(poison_button);
    }

    if (num > 4)
    {
        // Add the "Life Draining Trap" button
        QPushButton *life_drain_button = new QPushButton("&Drain Life Trap");
        life_drain_button->setToolTip(apply_monster_trap(FEAT_MTRAP_DRAIN_LIFE, 0, 0, MODE_DESCRIBE));
        connect(life_drain_button, SIGNAL(clicked()), this, SLOT(trap_choice_life_drain()));
        vlay->addWidget(life_drain_button);
    }

    if (num > 5)
    {
        // Add the "Lightning Trap" button
        QPushButton *lightning_button = new QPushButton("&Lightning Ball Trap");
        lightning_button->setToolTip(apply_monster_trap(FEAT_MTRAP_ELEC, 0, 0, MODE_DESCRIBE));
        connect(lightning_button, SIGNAL(clicked()), this, SLOT(trap_choice_lightning()));
        vlay->addWidget(lightning_button);
    }

    if (num > 6)
    {
        // Add the "Explosive Trap" button
        QPushButton *explosive_button = new QPushButton("&Explosive Trap");
        explosive_button->setToolTip(apply_monster_trap(FEAT_MTRAP_EXPLOSIVE, 0, 0, MODE_DESCRIBE));
        connect(explosive_button, SIGNAL(clicked()), this, SLOT(trap_choice_explosive()));
        vlay->addWidget(explosive_button);
    }

    if (num > 7)
    {
        // Add the "Portal Trap" button
        QPushButton *portal_button = new QPushButton("&Teleport Trap");
        portal_button->setToolTip(apply_monster_trap(FEAT_MTRAP_PORTAL, 0, 0, MODE_DESCRIBE));
        connect(portal_button, SIGNAL(clicked()), this, SLOT(trap_choice_portal()));
        vlay->addWidget(portal_button);
    }

     if (num > 8)
    {
        // Add the "Dispel Monster Trap" button
        QPushButton *dispel_button = new QPushButton("&Monster Dispelling Trap");
        dispel_button->setToolTip(apply_monster_trap(FEAT_MTRAP_DISPEL_M, 0, 0, MODE_DESCRIBE));
        connect(dispel_button, SIGNAL(clicked()), this, SLOT(trap_choice_dispell()));
        vlay->addWidget(dispel_button);
    }


    vlay->addStretch();

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel);
    connect(buttons, SIGNAL(clicked(QAbstractButton*)), this,
            SLOT(on_dialog_buttons_pressed(QAbstractButton*)));
    vlay->addWidget(buttons);

    setLayout(vlay);

    setWindowTitle(tr("Trap Selection Screen"));
    this->exec();
}

int return_trap_choice(void)
{
    TrapSelectDialog dlg;
    return (dlg.trap_choice);
}

/*
 * Look command
 */
void do_cmd_look(void)
{
    if (!character_dungeon) return;

    /* Look around */
    if (target_set_interactive(TARGET_LOOK, -1, -1))
    {
        message("Target Selected.");
        p_ptr->redraw |= (PR_MONLIST);
        redraw_stuff();
    }
}

static int quiver_wield(int item, object_type *o_ptr)
{
    int slot = 0;
    int num;
    QString o_name;

    object_type object_type_body;
    object_type *i_ptr;

    /* was: num = get_quantity(NULL, o_ptr->number);*/
    num = o_ptr->number;

    /* Cancel */
    if (num <= 0) return 0;

    /* Check free space */
    if (!quiver_carry_okay(o_ptr, num, item))
    {
        message(QString("Your quiver needs more space."));
        return 0;
    }

    /* Get local object */
    i_ptr = &object_type_body;

    /* Copy to the local object */
    i_ptr->object_copy(o_ptr);

    /* Modify quantity */
    i_ptr->number = num;

    /* No longer in use */
    i_ptr->obj_in_use = FALSE;

    /* Describe the result */
    o_name = object_desc(i_ptr, ODESC_PREFIX | ODESC_FULL);

    /*Mark it to go in the quiver */
    o_ptr->ident |= (IDENT_QUIVER);

    slot = wield_slot_ammo(o_ptr);

    /* Handle errors (paranoia) */
    if (slot == QUIVER_END)
    {
        /* No space. */
        message(QString("Your quiver is full."));
        return 0;
    }

    /* Decrease the item (from the pack) */
    if (item >= 0)
    {
        inven_item_increase(item, -num);
        inven_item_optimize(item);
    }

    /* Decrease the item (from the floor) */
    else
    {
        floor_item_increase(0 - item, -num);
        floor_item_optimize(0 - item);
    }

    /* Get the wield slot */
    o_ptr = &inventory[slot];

    /* Describe the result */
    o_name = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);

    /* Take off existing item */
    if (o_ptr->k_idx)
    {
        /* Take off existing item */
        o_ptr->number += i_ptr->number;
    }

    /* Wear the new stuff */
    else o_ptr->object_copy(i_ptr);

    /* Describe the result */
    o_name = object_desc(i_ptr, ODESC_PREFIX | ODESC_FULL);

    /* Message */
    sound(MSG_WIELD);
    message(QString("You have readied %1 (%2).") .arg(o_name) .arg(index_to_label(slot)));

    /* Cursed! */
    if (o_ptr->is_cursed())
    {
        /* Warn the player */
        sound(MSG_CURSED);
        message(QString("Oops! It feels deathly cold!"));

        /* Remove special inscription, if any */
        if (o_ptr->discount >= INSCRIP_NULL) o_ptr->discount = 0;

        /* Sense the object if allowed */
        if (o_ptr->discount == 0) o_ptr->discount = INSCRIP_CURSED;

        /* The object has been "sensed" */
        o_ptr->ident |= (IDENT_SENSE);
    }

    slot = sort_quiver(slot);

    /* Save quiver size */
    save_quiver_size();

    /* See if we have to overflow the pack */
    pack_overflow();

    /* Recalculate bonuses */
    p_ptr->notice |= (PN_SORT_QUIVER);
    p_ptr->update |= (PU_BONUS | PU_TORCH | PU_MANA | PU_NATIVE);
    p_ptr->redraw |= (PR_INVEN | PR_EQUIP | PR_ITEMLIST);

    /* Reorder the quiver and return the perhaps modified slot */
    return (slot);
}


/*
 * Wield or wear a single item from the pack or floor.
 * Does not process the player energy.
 */
void wield_item(object_type *o_ptr, int item, int slot)
{
    object_type object_type_body;
    object_type *i_ptr = &object_type_body;

    QString fmt;
    QString o_name;

    /* Describe the result */
    o_name = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);

    /* Ammo goes in quiver slots, which have special rules. */
    if (IS_QUIVER_SLOT(slot))
    {
        /* Put the ammo in the quiver */
        slot = quiver_wield(item, o_ptr);

        // Succcess
        return;
    }

    /* Hack - Don't wield mimic objects */
    if (o_ptr->mimic_r_idx) return;

    /* Get local object */
    i_ptr = &object_type_body;

    /* Obtain local object */
    i_ptr->object_copy(o_ptr);

    /* Modify quantity */
    i_ptr->number = 1;

    /* No longer in use */
    i_ptr->obj_in_use = FALSE;

    /* Decrease the item (from the pack) */
    if (item >= 0)
    {
        inven_item_increase(item, -1);
        inven_item_optimize(item);
    }

    /* Decrease the item (from the floor) */
    else
    {
        floor_item_increase(0 - item, -1);
        floor_item_optimize(0 - item);

    }

    /* Get the wield slot */
    o_ptr = &inventory[slot];

    /* Take off existing item */
    if (o_ptr->k_idx)
    {
        /* Take off existing item */
        (void)inven_takeoff(slot, 255);
    }

    /* Wear the new stuff */
    o_ptr->object_copy(i_ptr);

    /* Increment the equip counter by hand */
    p_ptr->equip_cnt++;

    /* Autoinscribe if the object was on the floor */
    if (item < 0) apply_autoinscription(o_ptr);

    /* Describe the result */
    o_name = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);

    /* Where is the item now */
    if ((slot == INVEN_WIELD) || (slot == INVEN_BOW))
    {
        if (o_ptr->is_bow()) fmt = (QString("You are shooting with %1 (%2).") .arg(o_name) .arg(index_to_label(slot)));
        else fmt = (QString("You are wielding %1 (%2).") .arg(o_name) .arg(index_to_label(slot)));

    }
    else if (slot == INVEN_LIGHT) fmt = (QString("Your light source is %1 (%2).") .arg(o_name) .arg(index_to_label(slot)));

    else fmt = (QString("You are wearing %1 (%2).") .arg(o_name) .arg(index_to_label(slot)));

    /* Message */
    sound(MSG_WIELD);
    message(fmt);

    /* Cursed! */
    if (o_ptr->is_cursed())
    {
        /* Warn the player */
        sound(MSG_CURSED);
        message(QString("Oops! It feels deathly cold!"));

        /* Remove special inscription, if any */
        if (o_ptr->discount >= INSCRIP_NULL) o_ptr->discount = 0;

        /* Sense the object if allowed */
        if (o_ptr->discount == 0) o_ptr->discount = INSCRIP_CURSED;

        /* The object has been "sensed" */
        o_ptr->ident |= (IDENT_SENSE);
    }

    /* Save quiver size */
    save_quiver_size();

    /* See if we have to overflow the pack */
    pack_overflow();

    /* Recalculate bonuses, torch, mana */
    p_ptr->notice |= (PN_SORT_QUIVER);
    p_ptr->update |= (PU_BONUS | PU_TORCH | PU_MANA | PU_NATIVE);
    p_ptr->redraw |= (PR_INVEN | PR_EQUIP | PR_ITEMLIST);
}



/*
 * create a monster trap, currently used from a scroll
 */
bool make_monster_trap(void)
{
    int y, x, dir;

    /* Get a direction */
    if (!get_rep_dir(&dir)) return (FALSE);

    /* Get location */
    y = p_ptr->py + ddy[dir];
    x = p_ptr->px + ddx[dir];

    /*check for an empty floor, monsters, and objects*/
    if (!cave_trappable_bold(y, x) || dungeon_info[y][x].monster_idx)
    {
        message(QString("You can only make a trap on an empty floor space."));

        return(FALSE);
    }

    /*set the trap*/
    py_set_trap(y, x);

    return(TRUE);
}



/*
 * Rogues may set traps, or they can be set from a scroll.
 * Only one such trap may exist at any one time,
 * but an old trap can be disarmed to free up equipment for a new trap.
 * -LM-
 */
void py_set_trap(int y, int x)
{

    if (p_ptr->timed[TMD_BLIND] || no_light())
    {
        message(QString("You can not see to set a trap."));
        return;
    }

    if (p_ptr->timed[TMD_CONFUSED] || p_ptr->timed[TMD_IMAGE])
    {
        message(QString("You are too confused."));
        return;
    }

    /* Set the trap, and draw it. */
    set_effect_trap_player(FEAT_MTRAP_BASE, y, x);

    /* Remember the location */
    dungeon_info[y][x].cave_info |= (CAVE_MARK);

    /* Notify the player. */
    message(QString("You set a monster trap."));

    light_spot(y, x);

    /*make the monsters who saw wary*/
    (void)project_los(y, x, 0, GF_MAKE_WARY);

    /* Increment the number of monster traps. */
    num_trap_on_level++;

}





/*
 * Turn a basic monster trap into an advanced one -BR-
 */
bool py_modify_trap(int y, int x)
{


    if (p_ptr->timed[TMD_BLIND] || no_light())
    {
        message(QString("You can not see to modify your trap."));
        return (FALSE);
    }

    /*no modifying traps on top of monsters*/
    if (dungeon_info[y][x].monster_idx > 0)
    {
        message(QString("There is a creature in the way."));
        return (FALSE);
    }

    int which_trap = return_trap_choice();

    if (which_trap == -1) return (FALSE);

    /* Set the trap, and draw it. */
    x_list[dungeon_info[y][x].effect_idx].x_f_idx = which_trap;

    /*check if player did not modify trap*/
    if (x_list[dungeon_info[y][x].effect_idx].x_f_idx == FEAT_MTRAP_BASE) return(FALSE);

    /*Mark it*/
    light_spot(y,x);

    /* Notify the player. */
    message(QString("You modify the monster trap."));

    /*make the monsters who saw wary*/
    (void)project_los(y, x, 0, GF_MAKE_WARY);

    return (TRUE);
}

void command_make_trap(cmd_arg args)
{
    int max_traps;

    int dir = args.direction;

    /* Get location */
    int y = p_ptr->py + ddy[dir];
    int x = p_ptr->px + ddx[dir];

    /* Oops */
    if (!(cp_ptr->flags & CF_SET_TRAPS))
    {
        message(QString("You don't have the ability to set traps!"));
        return;
    }

    /* Hack XXX XXX XXX */
    if (p_ptr->timed[TMD_CONFUSED] || p_ptr->timed[TMD_IMAGE])
    {
        message(QString("You are too confused!"));
        return;
    }

    if (!in_bounds(y, x)) return;

    if (cave_trappable_bold(y, x) && !dungeon_info[y][x].monster_idx > 0)
    {
        /*two traps for advanced rogues, one for lower level*/
        if (p_ptr->lev >= 26) max_traps = 2;
        else max_traps = 1;

        /*if not at max traps, set a trap, else fail*/
        if (num_trap_on_level < max_traps)
        {
            py_set_trap(y, x);
        }
        else
        {
            /*give a message and don't burn any energy*/
            message(QString("You must disarm an existing trap to free up your equipment."));

            return;
        }
    }

    /* Only rogues can modify basic monster traps */
    else if (cave_monster_trap_bold(y, x))
    {
        /* Modify */
        if (!py_modify_trap(y, x))return;
    }

    /*empty floor space*/
    else
    {
        message(QString("You can not set a trap here."));
        return;
    }

    // Update the last command
    p_ptr->player_previous_command_update(CMD_MAKE_TRAP, args);

    process_player_energy(BASE_ENERGY_MOVE);
}

void do_cmd_make_trap()
{
    if (!character_dungeon) return;

    int dir;

    if (!get_rep_dir(&dir)) return;

    cmd_arg args;
    args.wipe();
    args.direction = dir;

    command_make_trap(args);
}

