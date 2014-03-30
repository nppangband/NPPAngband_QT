/* File: was cmd2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *                    Jeff Greene, Diego Gonzalez
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 2, or
 *
 * b) the "Angband licence":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 */

#include "src/npp.h"


/*
 * Check if action permissible here.
 */
bool do_cmd_test(int y, int x, int action, bool do_message)
{
    u32b bitzero = 0x01;
    u32b flag;

    QString act = "";

    QString here = ((p_ptr->px == x ) && (p_ptr->py == y)) ? "here": "there";

    feature_type *f_ptr;

    /* Must have knowledge */
    if (!(dungeon_info[y][x].cave_info & (CAVE_MARK)))
    {
        /* Message */
        if (do_message) message(QString("You see nothing %1.") .arg(here));

        /* Nope */
        return (FALSE);
    }

    /* Get memorised feature */
    f_ptr = &f_info[dungeon_info[y][x].feat];

    switch (action)
    {
        case FS_SECRET:							break;
        case FS_OPEN:	act = " to open";		break;
        case FS_CLOSE:	act = " to close";		break;
        case FS_BASH:	act = " to bash";		break;
        case FS_SPIKE:	act = " to spike";		break;
        case FS_TUNNEL:	act = " to tunnel";		break;
        case FS_FLOOR:	act = " to set a trap on";	break;
        default: break;
    }

    if (action < FS_FLAGS2)
    {
        flag = bitzero << (action - FS_FLAGS1);
        if (!(f_ptr->f_flags1 & flag))
        {
            if (do_message) message(QString("You see nothing %1%2.") .arg(here) .arg(act));
            return (FALSE);
        }
    }

    else if (action < FS_FLAGS3)
    {
        flag = bitzero << (action - FS_FLAGS2);
        if (!(f_ptr->f_flags2 & flag))
        {
            if (do_message) message(QString("You see nothing %1%2.") .arg(here) .arg(act));
            return (FALSE);
        }
    }

    else if (action < FS_FLAGS_END)
    {
        flag = bitzero << (action - FS_FLAGS3);
        if (!(f_ptr->f_flags2 & flag))
        {
            if (do_message) message(QString("You see nothing %1%2.") .arg(here) .arg(act));
            return (FALSE);
        }
    }

    return (TRUE);
}


/*
 * Go up one level
 */
void do_cmd_go_up(void)
{
    QString out_val;
    byte quest;

    int decrease = 0;

    // Paranoia
    if (!p_ptr->playing) return;

    feature_type *f_ptr= &f_info[dungeon_info[p_ptr->py][p_ptr->px].feat];

    /* Verify stairs */
    if (!cave_up_stairs(p_ptr->py, p_ptr->px))
    {
        message(QString("I see no up staircase here."));

        return;
    }

    /* Ironman */
    if (adult_ironman)
    {
        message(QString("Nothing happens!"));
        return;
    }

    /* Verify leaving normal quest level */
    if ((verify_leave_quest) && quest_might_fail_if_leave_level())
    {
        out_val = "Really risk failing your quest? ";
        if (!get_check(out_val)) return;
    }

    /* Verify leaving normal quest level */
    if ((verify_leave_quest) && quest_shall_fail_if_leave_level())
    {
        out_val = "Really fail your quest? ";
        if (!get_check(out_val)) return;
    }

    /* Success */
    message(QString("You enter a maze of up staircases."));
    if (game_mode == GAME_NPPMORIA) message(QString("You pass through a one-way door."));

    /* Create a way back */
    if (adult_connected_stairs) p_ptr->create_stair = FEAT_MORE;

    /* New depth */
    decrease++;

    /*find out of entering a quest level (unusual going up)*/
    quest = quest_check(p_ptr->depth);

    /*go up another level if it is a shaft*/
    if ((f_ptr->f_flags2 & (FF2_SHAFT)) &&
        (!quest) && (p_ptr->depth > 0))
    {
        decrease++;

        /* Create a way back (usually) */
        if (adult_connected_stairs) p_ptr->create_stair = FEAT_MORE_SHAFT;
    }

    /* Change level */
    dungeon_change_level(p_ptr->depth - decrease);

    process_player_energy(BASE_ENERGY_MOVE);
}


/*
 * Go down one level
 */
void do_cmd_go_down(void)
{
    byte quest;
    QString out_val;

    int increase = 0;

    // Paranoia
    if (!p_ptr->playing) return;

    feature_type *f_ptr= &f_info[dungeon_info[p_ptr->py][p_ptr->px].feat];

    /*find out if entering a quest level*/
    quest = quest_check(p_ptr->depth);

    /* Verify stairs */
    if (!cave_down_stairs(p_ptr->py, p_ptr->px))
    {
        message(QString("I see no down staircase here."));
        return;
    }

    /* Verify leaving normal quest level */
    if ((verify_leave_quest) && quest_might_fail_if_leave_level())
    {
        out_val = "Really risk failing your quest? ";
        if (!get_check(out_val)) return;
    }

    /* Verify leaving normal quest level */
    if ((verify_leave_quest) && quest_shall_fail_if_leave_level())
    {
        out_val = "Really fail your quest? ";
        if (!get_check(out_val)) return;
    }

    /* Success */
    message(QString("You enter a maze of down staircases."));
    if (game_mode == GAME_NPPMORIA) message(QString("You pass through a one-way door."));

    /* Create a way back (usually) */
    if (adult_connected_stairs) p_ptr->create_stair = FEAT_LESS;

    /* New level */
    increase++;

    /*find out if entering a quest level*/
    quest = quest_check(p_ptr->depth);

    /* Go down a shaft if allowed */
    if ((f_ptr->f_flags2 & (FF2_SHAFT)) &&
        (!quest) && (p_ptr->depth < MAX_DEPTH - 1))
    {
        increase++;

        /* Create a way back (usually) */
        if (adult_connected_stairs) p_ptr->create_stair = FEAT_LESS_SHAFT;
    }

    /* Change level */
    dungeon_change_level(p_ptr->depth + increase);

    process_player_energy(BASE_ENERGY_MOVE);
}

/*
 * Perform the basic "open" command on doors
 *
 * Assume there is no monster blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */
static bool do_cmd_open_aux(int y, int x)
{
    int i, j;

    bool more = FALSE;

    int feat = dungeon_info[y][x].feat;

    int door_power;

    /* Verify legality */
    if (!do_cmd_test(y, x, FS_OPEN, TRUE)) return (FALSE);

    /* Secrets on doors */
    if (feat_ff1_match(feat, FF1_DOOR | FF1_SECRET) == (FF1_DOOR | FF1_SECRET))
    {
        /* Reveal */
        find_secret(y, x);

        /* Get the new door */
        feat = dungeon_info[y][x].feat;

        /* Update the visuals */
        p_ptr->update |= (PU_UPDATE_VIEW | PU_MONSTERS);
    }

    /* Jammed door */
    if (feat_ff3_match(feat, FF3_DOOR_JAMMED))
    {
        /* Stuck */
        message("The door appears to be stuck.");
    }

    /* Locked door */
    else if (feat_ff3_match(feat, FF3_DOOR_LOCKED) &&
        ((door_power = feat_state_power(feat, FS_OPEN)) > 0))
    {
        /*Mark the feature lore*/
        feature_lore *f_l_ptr = &f_l_list[feat];
        f_l_ptr->f_l_flags1 |= (FF1_CAN_OPEN);

        /* Disarm factor */
        i = p_ptr->state.skills[SKILL_DISARM];

        /* Penalize some conditions */
        if (p_ptr->timed[TMD_BLIND] || no_light()) i = i / 10;
        if (p_ptr->timed[TMD_CONFUSED] || p_ptr->timed[TMD_IMAGE]) i = i / 10;

        /* Extract the lock power */
        /* door_power must be between 1 and 7 */
        j = i - (door_power * 4);

        /* Always have a small chance of success */
        if (j < 2) j = 2;

        /* Success */
        if (rand_int(100) < j)
        {
            /* Message */
            message("You have picked the lock.");

            /* Open the door */
            cave_alter_feat(y, x, FS_OPEN);

            /* Update the visuals */
            p_ptr->update |= (PU_FORGET_VIEW | PU_UPDATE_VIEW | PU_MONSTERS | PU_FLOW_DOORS | PU_FLOW_NO_DOORS);

            /* Experience */
            gain_exp(1);
        }

        /* Failure */
        else
        {
            /* Failure */
            //if (flush_failure) flush();

            /* Message */
            message("You failed to pick the lock.");

            /* We may keep trying */
            more = TRUE;
        }
    }

    /* Closed door */
    else
    {
        /* Open the door */
        cave_alter_feat(y, x, FS_OPEN);

        /* Update the visuals */
        p_ptr->update |= (PU_FORGET_VIEW | PU_UPDATE_VIEW | PU_MONSTERS | PU_FLOW_NO_DOORS | PU_FLOW_DOORS);

        /* Sound */
        //sound(MSG_OPENDOOR);
    }

    update_stuff();

    /* Result */
    return (more);
}

/*
 * Open a closed/locked/jammed door or a closed/locked chest.
 *
 * Unlocking a locked door/chest is worth one experience point.
 */
void command_open(cmd_arg args)
{
    int cy, cx, y, x;
    int dir = args.direction;

    /* Count chests (locked) */
    int num_chests = 0, o_idx = 0;

    bool more = FALSE;

    /* Get location */
    cy = y = p_ptr->py + ddy[dir];
    cx = x = p_ptr->px + ddx[dir];

    /* Check for chests */
    //num_chests = count_chests(&y, &x, FALSE);
    //o_idx = chest_check(y, x, FALSE);

    /* Verify legality */
    if (!o_idx && !do_cmd_test(y, x, FS_OPEN, TRUE)) return;

    /* Apply confusion */
    if (confuse_dir(&dir))
    {
        /* Get location */
        cy = y = p_ptr->py + ddy[dir];
        cy = x = p_ptr->px + ddx[dir];

        /* Check for chest */
        //num_chests = count_chests(&y, &x, FALSE);
        //o_idx = chest_check(y, x, FALSE);
    }

#if 0
    /* Allow repeated command */
    if (p_ptr->command_arg)
    {
        /* Set repeat count */
        p_ptr->command_rep = p_ptr->command_arg - 1;

        /* Redraw the state */
        p_ptr->redraw |= (PR_STATE);

        /* Cancel the arg */
        p_ptr->command_arg = 0;
    }
#endif

    /* Monster */
    if (dungeon_info[y][x].monster_idx > 0)
    {
        /* Message */
        message("There is a monster in the way!");

        /* Attack */
        py_attack(y, x);
    }

#if 0
    /* Chest */
    else if (num_chests)
    {
        /* Get top chest */
        o_idx = chest_check(y, x, FALSE);

        /* Open the chest if confused, or only one */
        if ((p_ptr->timed[TMD_CONFUSED]) || (num_chests == 1))  more = do_cmd_open_chest(y, x, o_idx);

        /* More than one */
        else
        {
            cptr q, s;

            o_idx = 0;

            /* Get an item */
            q = "Open which chest? ";
            s = "There are no chests in that direction!";

            /*clear the restriction*/
            item_tester_hook = obj_is_openable_chest;

            /*player chose escape*/
            if (!get_item_beside(&o_idx, q, s, cy, cx)) more = 0;

            /* Open the chest */
            else more = do_cmd_open_chest(cy, cx, -o_idx);
        }
    }
#endif

    /* Door */
    else
    {
        /* Open the door */
        more = do_cmd_open_aux(cy, cx);
    }

    /* Cancel repeat unless we may continue */
    if (!more) disturb(0, 0);

    process_player_energy(BASE_ENERGY_MOVE);
}

void do_cmd_open(void)
{
    int dir;

    if (!get_rep_dir(&dir)) return;

    cmd_arg args;
    args.direction = dir;

    command_open(args);
}
