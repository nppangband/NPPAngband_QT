/* File: player_classes.cpp */

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

#include "player_command.h"

// Check if we have a completed command
bool command_type::repeated_command_completed(void)
{
    if (!repeat_allowed) return (TRUE);
    if (!p_ptr->player_args.repeats) return (TRUE);
    return (FALSE);
}

// Check if we have a completed command
bool command_type::needs_direction()
{
    if(cmd_needs & (ARG_DIRECTION)) return (TRUE);
    return (FALSE);
}

// Check if we have a completed command
bool command_type::needs_item()
{
    if(cmd_needs & (ARG_DIRECTION)) return (TRUE);
    return (FALSE);
}

// Check if we have a completed command
bool command_type::needs_quantity()
{
    if (!needs_item()) return (FALSE);
    if(cmd_needs & (ARG_NUMBER)) return (TRUE);
    return (FALSE);
}

QString command_type::prompt(int command)
{
    QString return_string = QString("Please enter an amount");
    if (command == CMD_DROP) return_string.append("to drop.");
    else if (command == CMD_DESTROY) return_string.append("to destroy.");
    else return_string.append(".");

    return(return_string);
}


command_type command_info[] =
{   // CMD_NONE
    {0L,            NULL, FALSE, 0},
    // CMD_RESTING
    {ARG_SPECIAL,   command_rest,   TRUE,  0},
    // CMD_RUNNING
    {ARG_DIRECTION, command_run,    TRUE,  9999},
    // CMD_WALK
    {ARG_DIRECTION, command_walk,   FALSE,  0 },
    // CMD_OPEN
    {ARG_DIRECTION, command_open,   TRUE,  99 },
    // CMD_CLOSE
    {ARG_DIRECTION, command_close,  FALSE, 0 },
    // CMD_SPIKE
    {ARG_DIRECTION, command_spike,  FALSE, 0 },
    // CMD_DISARM
    {ARG_DIRECTION, command_disarm, TRUE,  99 },
    // CMD_BASH
    {ARG_DIRECTION, command_bash,   TRUE,  99 },
    // CMD_TUNNEL
    {ARG_DIRECTION, command_tunnel, TRUE,  99 },
    // CMD_ALTER
    {ARG_DIRECTION, command_alter,  TRUE,  99 },
    // CMD_SEARCH
    {ARG_DIRECTION, command_search, FALSE,  0 },
    // CMD_MAKE_TRAP
    {ARG_DIRECTION, command_make_trap, FALSE, 0},
    // CMD_HOLD
    {0L,            command_hold,   FALSE, 0},
    // CMD_TAKEOFF
    {ARG_ITEM,      command_takeoff, FALSE, 0},
    // CMD_WIELD
    {ARG_ITEM | ARG_SLOT, command_wield, FALSE, 0},
    // CMD_SWAP
    {0L,            command_swap, FALSE, 0},
    // CMD_ITEM_USE
    {ARG_ITEM,      command_use,    FALSE, 0},
    // CMD_REFUEL
    {ARG_ITEM,      command_refuel, FALSE, 0},
    // CMD_FIRE
    {ARG_ITEM | ARG_DIRECTION,command_fire,FALSE, 0},
    // CMD_FIRE_NEAR
    {ARG_ITEM,      command_fire_nearest,FALSE, 0},
    // CMD_DROP
    {ARG_ITEM | ARG_NUMBER,  command_drop,   FALSE, 0},
    // CMD_PICKUP
    {ARG_ITEM,  command_pickup,   FALSE, 0},
    // CMD_BROWSE
    {ARG_ITEM,      command_browse,   FALSE, 0},
    // CMD_STUDY
    {ARG_ITEM,      command_study,   FALSE, 0},
    // CMD_CAST  Direction handled by cast command
    {ARG_ITEM | ARG_DIRECTION, command_cast,   FALSE, 0},
    // CMD_DESTROY
    {ARG_ITEM | ARG_NUMBER, command_destroy,   FALSE, 0},
    // CMD_EXAMINE
    {ARG_ITEM,      command_examine,   FALSE, 0},
    // CMD_INSCRIBE
    {ARG_ITEM,      command_inscribe,   FALSE, 0},
    // CMD_UNINSCRIBE
    {ARG_ITEM,      command_uninscribe,   FALSE, 0},
    // CMD_ACTIVATE
    {ARG_ITEM | ARG_DIRECTION, command_use, FALSE, 0},
    // CMD_THROW
    {ARG_ITEM | ARG_DIRECTION,command_throw,FALSE, 0},
};

// Prepare a command for processing.
void process_command(int item, s16b command)
{
    // Now that we have a match, process the command.
    command_type *command_ptr = &command_info[command];
    cmd_arg args;
    args.wipe();
    object_type *o_ptr;

    /*
     * Note if the command doesn't call for an item,
     * this will point to the first item on the floor.
     */
    o_ptr = object_from_item_idx(item);
    args.item = item;

    // Get the direction, if necessary
    if (command_ptr->needs_direction())
    {
        // For objects
        if (command_ptr->needs_item())
        {
            if (obj_needs_aim(o_ptr))
            {
                bool trap_related = trap_related_object(o_ptr);
                if (!get_aim_dir(&args.direction, trap_related)) return;
            }
        }
        // For player related directional commands
        else if (!get_rep_dir(&args.direction)) return;
    }

    // Only for objects
    if(command_ptr->needs_quantity())
    {
        args.number = get_quantity(command_ptr->prompt(command), o_ptr->number);
    }

}
