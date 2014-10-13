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
    {ARG_ITEM,      command_wield, FALSE, 0},
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
    {ARG_ITEM | ARG_NUMBER,  command_pickup,   FALSE, 0},
    // CMD_BROWSE
    {ARG_ITEM,      command_browse,   FALSE, 0},
    // CMD_STUDY
    {ARG_ITEM,      command_study,   FALSE, 0},
    // CMD_CAST
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
