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
{
    {0L,            NULL, FALSE, 0},            // CMD_NONE
    {ARG_SPECIAL,   command_rest, TRUE, 0},    // CMD_RESTING
    {ARG_DIRECTION, NULL, FALSE, 0}             // CMD_RUNNING

};
