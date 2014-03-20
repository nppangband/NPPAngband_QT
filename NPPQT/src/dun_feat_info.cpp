
/* File: was feature.c */

/*
 * Copyright (c) 2006 Jeff Greene, Diego Gonzalez
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

#include "src/npp.h"

/*
 * Return a string that describes the type of a feature
 */
static QString get_feature_type(const feature_lore *f_l_ptr)
{
    if (f_l_ptr->f_l_flags1 & FF1_SHOP)		return (" shop entrance");
    else if (f_l_ptr->f_l_flags1 & FF1_DOOR)	return (" door");
    else if (f_l_ptr->f_l_flags1 & FF1_TRAP)	return (" trap");
    else if (f_l_ptr->f_l_flags1 & FF1_STAIRS)	return (" staircase");
    else if (f_l_ptr->f_l_flags1 & FF1_GLYPH)	return (" glyph");
    else if (f_l_ptr->f_l_flags1 & FF1_FLOOR)	return (" floor");
    else if (f_l_ptr->f_l_flags1 & FF1_WALL)	return (" wall");
    else if (f_l_ptr->f_l_flags3 & FF3_TREE)	return (" tree");
    else if (f_l_ptr->f_l_flags2 & FF2_EFFECT)	return (" effect");

    /*Default*/
    else return (" feature");
}


static QString describe_feature_basic(int f_idx, const feature_lore *f_l_ptr)
{
    const feature_type *f_ptr = &f_info[f_idx];
    QString flags[50];
    QString type;
    int i = 0;
    u16b n = 0;

    QString output;
    output.clear();

    text_out("This is a");

    /* Collect some flags */
    if (f_l_ptr->f_l_flags2 & FF2_SHALLOW)		if (n < N_ELEMENTS(flags)) flags[n++] = " shallow";
    if (f_l_ptr->f_l_flags2 & FF2_DEEP)			if (n < N_ELEMENTS(flags)) flags[n++] = " deep";

    if ((f_l_ptr->f_l_flags2 & FF2_GLOW) &&
         (!(f_l_ptr->f_l_flags1 & FF1_SHOP)))	if (n < N_ELEMENTS(flags)) flags[n++] = " glowing";

    if (f_l_ptr->f_l_flags3 & FF3_LAVA)			if (n < N_ELEMENTS(flags)) flags[n++] = " lava";
    if (f_l_ptr->f_l_flags3 & FF3_ICE)			if (n < N_ELEMENTS(flags)) flags[n++] = " icy";
    if (f_l_ptr->f_l_flags3 & FF3_OIL)			if (n < N_ELEMENTS(flags)) flags[n++] = " oil";
    if (f_l_ptr->f_l_flags3 & FF3_FIRE)			if (n < N_ELEMENTS(flags)) flags[n++] = " fire";
    if (f_l_ptr->f_l_flags3 & FF3_SAND)			if (n < N_ELEMENTS(flags)) flags[n++] = " sandy";
    if (f_l_ptr->f_l_flags3 & FF3_FOREST)		if (n < N_ELEMENTS(flags)) flags[n++] = " forest";
    if (f_l_ptr->f_l_flags3 & FF3_WATER)		if (n < N_ELEMENTS(flags)) flags[n++] = " water";
    if (f_l_ptr->f_l_flags3 & FF3_ACID)			if (n < N_ELEMENTS(flags)) flags[n++] = " acid";
    if (f_l_ptr->f_l_flags3 & FF3_MUD)			if (n < N_ELEMENTS(flags)) flags[n++] = " mud";

    /* Print the collected flags */
    for (i = 0; i < n; i++)
    {
        /* Append a 'n' to 'a' if the first flag begins with a vowel */
        if ((i == 0) && begins_with_vowel(flags[i])) text_out("n");

        text_out_c(TERM_L_BLUE, flags[i]);
    }

    /* Get the feature type name */
    type = get_feature_type(f_l_ptr);

    /* Append a 'n' to 'a' if the type name begins with a vowel */
    if ((n == 0) && begins_with_vowel(type)) text_out("n");

    /* Describe the feature type */
    text_out(type);

    /* Describe location */
    if (f_ptr->f_flags2 & FF2_EFFECT)
    {
        /* Do nothing */
    }

    else if (f_l_ptr->f_l_flags1 & FF1_SHOP)
    {
        text_out_c(TERM_L_GREEN, " that is found in the town");
    }
    else if ((f_l_ptr->f_l_flags2 & FF2_TRAP_MON) ||
    (f_l_ptr->f_l_flags1 & FF1_GLYPH))
    {
        text_out_c(TERM_L_GREEN, " that is set by the player");
    }
    else if (f_l_ptr->f_l_sights > 10)
    {
        text_out(" that");

        if (f_l_ptr->f_l_sights > 20)
        {
            if (f_ptr->f_rarity >= 4) text_out(" rarely");
            else if (f_ptr->f_rarity >= 2) text_out(" occasionally");
            else text_out(" commonly");
        }

        if (f_ptr->f_level == 0)
        {
            text_out_c(TERM_L_GREEN, " appears in both the town and dungeon");
        }
        else if (f_ptr->f_level == 1)
        {
            text_out_c(TERM_L_GREEN, " appears throughout the dungeon");
        }
        else
        {
            text_out_c(TERM_L_GREEN, " appears");

            text_out_c(TERM_L_GREEN, format(" at depths of %d feet and below",
                                        f_ptr->f_level * 50));
        }

    }

    /* Little Information Yet */
    else
    {
        text_out_c(TERM_L_GREEN, " that is found in the dungeon");
    }

    /* End this sentence */
    text_out(".");

    /* More misc info */
    if (f_l_ptr->f_l_flags1 & FF1_DROP)
    {
        text_out("  This");
        /*Describe the feature type*/
        describe_feature_type(f_l_ptr);
        text_out(" can hold objects.");
    }
    if (f_l_ptr->f_l_flags1 & FF1_HAS_GOLD)
    {
        text_out("  This");
        describe_feature_type(f_l_ptr);
        text_out(" may be hiding treasure.");
    }
    if (f_l_ptr->f_l_flags1 & FF1_HAS_ITEM)
    {
        text_out("  This");
        describe_feature_type(f_l_ptr);
        text_out(" may be hiding an object.");
    }
}


static void describe_feature_move_see_cast(int f_idx, const feature_lore *f_l_ptr)
{

    int vn, n;
    cptr vp[6];

    /* Collect special abilities. */
    vn = 0;

    if (f_l_ptr->f_l_flags1 & FF1_MOVE)
    {
        if feat_ff1_match(f_idx, FF1_FLOOR)	vp[vn++] = "move over";
        else vp[vn++] = "move through";
    }
    if (f_l_ptr->f_l_flags1 & FF1_LOS) vp[vn++] = "see through";
    if (f_l_ptr->f_l_flags1 & FF1_RUN) vp[vn++] = "run past";
    if (f_l_ptr->f_l_flags1 & FF1_PROJECT)
    {
        vp[vn++] = "cast magic through";
        vp[vn++] = "fire projectiles through";
    }

    /* Describe special abilities. */
    if (vn)
    {
        /* Intro */
        text_out("  You ");

        /* Scan */
        for (n = 0; n < vn; n++)
        {
            /* Intro */
            if (n == 0) text_out("can ");
            else if (n < vn-1) text_out(", ");
            else text_out(" and ");

            /* Dump */
            text_out_c(TERM_GREEN, vp[n]);
        }

        /* End */
        text_out(" this");
        describe_feature_type(f_l_ptr);
        text_out(".");
    }

    if (f_l_ptr->f_l_flags2 & FF2_CAN_FLY)
    {
        text_out("  Creatures who have the ability to do so can fly over this");
        describe_feature_type(f_l_ptr);
        text_out(".");
    }
    if (f_l_ptr->f_l_flags2 & FF2_COVERED)
    {
        text_out("  Native creatures can hide in this");
        describe_feature_type(f_l_ptr);
        text_out(".");
    }
}


static void describe_feature_stairs(const feature_lore *f_l_ptr)
{
    text_out("  This");

    if (f_l_ptr->f_l_flags2 & FF2_SHAFT)	text_out_c(TERM_L_BLUE, " shaft");
    else text_out_c(TERM_L_BLUE, " staircase");

    text_out(" will take you");

    if (f_l_ptr->f_l_flags1 & FF1_LESS)		text_out_c(TERM_L_BLUE, " up");
    if (f_l_ptr->f_l_flags1 & FF1_MORE)		text_out_c(TERM_L_BLUE, " down");

    if (f_l_ptr->f_l_flags2 & FF2_SHAFT)	text_out_c(TERM_L_BLUE, " two levels.");
    else text_out_c(TERM_L_BLUE, " one level.");
}


static void describe_feature_trap(int f_idx, const feature_lore *f_l_ptr)
{
    /*Describe passive traps the player can set off*/
    if (f_l_ptr->f_l_flags2 & FF2_TRAP_PASSIVE) hit_trap(f_idx, 0, 0, MODE_DESCRIBE);

    /*Describe passive traps the player can set off*/
    if (f_l_ptr->f_l_flags2 & FF2_TRAP_MON) apply_monster_trap(f_idx, 0, 0, MODE_DESCRIBE);

    /*Describe smart traps */
    if (f_l_ptr->f_l_flags2 & FF2_TRAP_SMART)fire_trap_smart(f_idx, 0, 0, MODE_DESCRIBE);
}


static void describe_feature_interaction(int f_idx, const feature_lore *f_l_ptr)
{
    const feature_type *f_ptr = &f_info[f_idx];

    int vn, n, i;
    cptr vp[15];

    u32b filtered_flag1 = f_l_ptr->f_l_flags1;

    /* Collect special abilities. */
    vn = 0;

    /*
     * First get rid of redundant messages, if they are to be
     * described later in describe_feature_transitions.
     */
    for (i = 0; i < MAX_FEAT_STATES; i++)
    {
        /* There isn't a recorded action here */
        if (f_ptr->state[i].fs_action == FS_FLAGS_END) continue;

        /* Have we seen it yet? */
        if (f_l_ptr->f_l_state[i] == 0) continue;

        switch (f_ptr->state[i].fs_action)
        {

            /* Make sure we don't list something redundantly and more specifically later*/
            case 	FS_OPEN:		{filtered_flag1 &= ~(FF1_CAN_OPEN); 	continue;}
            case	FS_CLOSE:		{filtered_flag1 &= ~(FF1_CAN_CLOSE); 	continue;}
            case	FS_BASH:		{filtered_flag1 &= ~(FF1_CAN_BASH); 	continue;}
            case	FS_SPIKE:		{filtered_flag1 &= ~(FF1_CAN_SPIKE); 	continue;}
            case	FS_TUNNEL:		{filtered_flag1 &= ~(FF1_CAN_TUNNEL); 	continue;}

            /*All others*/
            default: continue;
        }
    }

    if (filtered_flag1 & FF1_CAN_OPEN) vp[vn++] = "open";
    if (filtered_flag1 & FF1_CAN_CLOSE) vp[vn++] = "close";
    if (filtered_flag1 & FF1_CAN_BASH) vp[vn++] = "bash";
    if (filtered_flag1 & FF1_CAN_SPIKE) vp[vn++] = "spike";
    if (filtered_flag1 & FF1_CAN_DISARM) vp[vn++] = "disarm";
    if (filtered_flag1 & FF1_CAN_TUNNEL) vp[vn++] = "tunnel into";

    /* Describe special abilities. */
    if (vn)
    {
        /* Intro */
        text_out("  You ");

        /* Scan */
        for (n = 0; n < vn; n++)
        {
            /* Intro */
            if (n == 0) text_out("can ");
            else if (n < vn-1) text_out(", ");
            else text_out(" and ");

            /* Dump */
            text_out_c(TERM_YELLOW, vp[n]);
        }

        /* End */
        text_out(" this");
        describe_feature_type(f_l_ptr);
        text_out(".");
    }
}


static void describe_feature_vulnerabilities(const feature_lore *f_l_ptr)
{

    int vn, n;
    cptr vp[15];

    /* Collect special abilities. */
    vn = 0;

    if (f_l_ptr->f_l_flags2 & FF2_HURT_ROCK) vp[vn++] = "stone-to-mud";
    if (f_l_ptr->f_l_flags2 & FF2_HURT_FIRE) vp[vn++] = "fire";
    if (f_l_ptr->f_l_flags2 & FF2_HURT_FIRE) vp[vn++] = "lava";
    if (f_l_ptr->f_l_flags2 & FF2_HURT_FIRE) vp[vn++] = "plasma";
    if (f_l_ptr->f_l_flags2 & FF2_HURT_COLD) vp[vn++] = "cold";
    if (f_l_ptr->f_l_flags2 & FF2_HURT_COLD) vp[vn++] = "ice";
    if (f_l_ptr->f_l_flags2 & FF2_HURT_ACID) vp[vn++] = "acid";
    if (f_l_ptr->f_l_flags2 & FF2_HURT_ELEC) vp[vn++] = "electricity";
    if (f_l_ptr->f_l_flags2 & FF2_HURT_WATER) vp[vn++] = "water";
    if (f_l_ptr->f_l_flags3 & FF3_HURT_BOIL_WATER) vp[vn++] = "boiling water";
    if (f_l_ptr->f_l_flags3 & FF3_HURT_BOIL_WATER) vp[vn++] = "steam";
    if (f_l_ptr->f_l_flags3 & FF3_HURT_POIS) vp[vn++] = "poison";

    /* Describe special abilities. */
    if (vn)
    {
        /* Intro */
        text_out("  This");

        describe_feature_type(f_l_ptr);

        text_out(" is affected ");

        /* Scan */
        for (n = 0; n < vn; n++)
        {
            /* Intro */
            if (n == 0) text_out("by ");
            else if (n < vn-1) text_out(", ");
            else text_out(" and ");

            /* Dump */
            text_out_c(TERM_L_RED, vp[n]);
        }

        /* End */
        text_out(".");
    }
}


/*
 * Returns true if special language is used
 * returns false if the standard output should follow this.
 * This function assumes it is the start of the sentence.
 * Returning false follows this function by "causes this feature to change to"
 */

static bool describe_transition_action(int action)
{

    switch (action)
    {

        case 	FS_SECRET:		{text_out_c(TERM_YELLOW, "  Once discovered, this feature is revealed as ");return (TRUE);}
        case 	FS_OPEN:		{text_out_c(TERM_YELLOW, "  Opening"); 		break;}
        case	FS_CLOSE:		{text_out_c(TERM_YELLOW, "  Closing"); 		break;}
        case	FS_BASH:		{text_out_c(TERM_YELLOW, "  Bashing"); 		break;}
        case	FS_SPIKE:		{text_out_c(TERM_YELLOW, "  Spiking"); 		break;}
        case	FS_TUNNEL:		{text_out_c(TERM_YELLOW, "  Tunneling"); 		break;}
        case	FS_TRAP:		{text_out_c(TERM_YELLOW, "  Creating a trap"); break;}
        case	FS_GLYPH:		{text_out_c(TERM_YELLOW, "  This feature can change into a "); return (TRUE);}
        case	FS_FLOOR:		{text_out_c(TERM_YELLOW, "  A plain floor"); 	break;}
        case	FS_BRIDGE:		{text_out_c(TERM_YELLOW, "  Bridging"); 		break;}
        case	FS_HIT_TRAP:	{text_out_c(TERM_YELLOW, "  De-activating this trap"); break;}
        case	FS_HURT_ROCK:	{text_out_c(TERM_YELLOW, "  Stone-to-mud"); 	break;}
        case	FS_HURT_FIRE:	{text_out_c(TERM_YELLOW, "  Fire, smoke, lava or plasma"); break;}
        case	FS_HURT_COLD:	{text_out_c(TERM_YELLOW, "  Cold or ice"); 	break;}
        case	FS_HURT_ACID:	{text_out_c(TERM_YELLOW, "  Acid"); 			break;}
        case	FS_HURT_ELEC:	{text_out_c(TERM_YELLOW, "  Electricity"); 	break;}
        case	FS_HURT_WATER:	{text_out_c(TERM_YELLOW, "  Water"); 			break;}
        case	FS_HURT_BWATER:	{text_out_c(TERM_YELLOW, "  Boiling water"); 	break;}
        case	FS_HURT_POIS:	{text_out_c(TERM_YELLOW, "  Poison"); 			break;}
        case	FS_TREE:		{text_out_c(TERM_YELLOW, "  Forest creation"); break;}
        case   	FS_NEED_TREE: 	{text_out_c(TERM_YELLOW, "  Forest destruction"); break;}

        /*Paranoia*/
        default:				{text_out_c(TERM_RED, "  ERROR - Unspecified Action"); break;}
    }

    return (FALSE);
}


static void describe_feature_transitions(int f_idx, const feature_lore *f_l_ptr)
{
    feature_type *f_ptr = &f_info[f_idx];

    char feature_name[80];

    int i;

    bool other_trans = FALSE;

    /*Handle permanent features*/
    if (f_l_ptr->f_l_flags1 & FF1_PERMANENT)
    {
        text_out("  This is a permanent feature.");
    }

    /*Mention the mimic, if known*/
    if (f_ptr->f_mimic != f_idx)
    {
        /* Remember we have a transition we are reporting */
        other_trans = TRUE;

        /*Describe it*/
        text_out("  Until discovered, this feature appears as ");
        feature_desc(feature_name, sizeof(feature_name), f_ptr->f_mimic, TRUE, FALSE);
        text_out(format("%s.", feature_name));
    }

    /* Search the action */
    for (i = 0; i < MAX_FEAT_STATES; i++)
    {
        /* There isn't a recorded action here */
        if (f_ptr->state[i].fs_action == FS_FLAGS_END) continue;

        /* The feature isn't changing */
        if (f_ptr->state[i].fs_result == f_idx)
        {
            if (f_l_ptr->f_l_flags3 & (FF3_PICK_DOOR))
            {
                text_out(" Discovering this ");
                describe_feature_type(f_l_ptr);
                text_out(" reveals a closed door.");
            }

            continue;
        }

        /* Have we seen it yet? */
        if (f_l_ptr->f_l_state[i] == 0) continue;

        /* Remember we have a transition we are reporting */
        other_trans = TRUE;

        /* Describe it, followed by standard output */
        if(!describe_transition_action(f_ptr->state[i].fs_action))
        {
            text_out(" changes this");
            describe_feature_type(f_l_ptr);
            text_out(" to ");
        }

        feature_desc(feature_name, sizeof(feature_name), f_ptr->state[i].fs_result, TRUE, FALSE);
        text_out(format("%s.", feature_name));
    }

    /*Mention the default if it is different*/
    if ((f_l_ptr->f_l_defaults > 0) && (f_ptr->defaults != f_idx))
    {
        /*Describe this transition, handle differently if we have described a transition above*/
        if (other_trans)
        {
            text_out("  For all other effects, this");
        }
        else text_out("  This");
        describe_feature_type(f_l_ptr);
        text_out(" changes to ");
        feature_desc(feature_name, sizeof(feature_name), f_ptr->defaults, TRUE, FALSE);
        text_out(format("%s.", feature_name));
    }
}


static void describe_feature_damage(int f_idx, const feature_lore *f_l_ptr)
{
    feature_type *f_ptr = &f_info[f_idx];

    cptr action = "hurts";

    /* No damage, or no damage seen yet */
    if (f_ptr->dam_non_native == 0) return;
    if (!f_l_ptr->f_l_dam_non_native) return;

    /* Specify the damage type from certain features */
    if (_feat_ff3_match(f_ptr, FF3_ICE))
    {
        action = "freezes";
    }
    else if (_feat_ff3_match(f_ptr, FF3_LAVA | FF3_FIRE))
    {
        action = "burns";
    }

    /* Intro */
    text_out("  This");
    describe_feature_type(f_l_ptr);
    text_out(format(" %s any non-native creature", action));

    /* Slightly more information when the player has seen it several times */
    if (f_l_ptr->f_l_dam_non_native > 10)
    {
        text_out(format(" for %d damage", f_ptr->dam_non_native));
    }

    text_out(" who stays on this feature for one turn at normal speed.");
}


static void describe_feature_movement_effects(int f_idx, const feature_lore *f_l_ptr)
{
    feature_type *f_ptr = &f_info[f_idx];

    if (feat_ff2_match(f_idx, FF2_EFFECT)) return;

    /*Describe movement by native creatures*/
    if ((f_ptr->native_energy_move != BASE_ENERGY_MOVE) && (f_l_ptr->f_l_native_moves > 0))
    {
        int percent_movement = (ABS(BASE_ENERGY_MOVE - f_ptr->native_energy_move) * 100) / BASE_ENERGY_MOVE;

        text_out("  A creature native to this terrain uses");

        /*More information for who have observed movement more*/
        if (f_l_ptr->f_l_native_moves > 20)
        {
            if (f_ptr->native_energy_move > BASE_ENERGY_MOVE)
            {
                text_out_c(TERM_BLUE, format(" %d percent more", percent_movement));
            }
            else text_out_c(TERM_BLUE, format(" %d percent less", percent_movement));
        }
        else
        {
            if (percent_movement  > 15) text_out_c(TERM_BLUE, " significantly");

            if (f_ptr->native_energy_move > BASE_ENERGY_MOVE)  text_out_c(TERM_BLUE, " more");
            else text_out_c(TERM_BLUE, " less");

        }

        text_out(" energy moving into this terrain.");
    }

    /*Describe movement by non-native creatures*/
    if ((f_ptr->non_native_energy_move != BASE_ENERGY_MOVE) && (f_l_ptr->f_l_non_native_moves > 0))
    {
        int percent_movement = (ABS(BASE_ENERGY_MOVE - f_ptr->non_native_energy_move) * 100) / BASE_ENERGY_MOVE;

        text_out("  A creature who is not native to this terrain uses");

        /*More information for who have observed movement more*/
        if (f_l_ptr->f_l_non_native_moves > 20)
        {
            if (f_ptr->non_native_energy_move > BASE_ENERGY_MOVE)
            {
                text_out_c(TERM_BLUE, format(" %d percent more", percent_movement));
            }
            else text_out_c(TERM_BLUE, format(" %d percent less", percent_movement));
        }
        else
        {
            if (percent_movement  > 15) text_out_c(TERM_BLUE, " significantly");

            if (f_ptr->non_native_energy_move > BASE_ENERGY_MOVE)  text_out_c(TERM_BLUE, " more");
            else text_out_c(TERM_BLUE, " less");

        }

        text_out(" energy moving into this terrain.");
    }
}


static void describe_feature_combat_effects(int f_idx, const feature_lore *f_l_ptr)
{
    feature_type *f_ptr = &f_info[f_idx];

    if (feat_ff2_match(f_idx, FF2_EFFECT)) return;

    /* Describe movement by native creatures */
    if ((f_ptr->native_to_hit_adj != 100) && (f_l_ptr->f_l_native_to_hit_adj > 0))
    {
        text_out("  A native creature is");

        /* More information for who have observed movement more */
        if (f_l_ptr->f_l_native_to_hit_adj > 100)
        {
            if (f_ptr->native_to_hit_adj  > 100)
            {
                text_out_c(TERM_BLUE, " %d percent more", (f_ptr->native_to_hit_adj  - 100));
            }
            else text_out_c(TERM_BLUE, " %d percent less",
                (100 - f_ptr->native_to_hit_adj));

        }
        else
        {
            if (ABS(f_ptr->native_to_hit_adj  - 100) > 15) text_out_c(TERM_BLUE, " significantly");

            if (f_ptr->native_to_hit_adj  > 100)  text_out_c(TERM_BLUE, " more");
            else text_out_c(TERM_BLUE, " less");

        }

        text_out(" successful in attacking and defending while fighting in this terrain.");
    }

    /*Describe movement by non_native creatures*/
    if ((f_ptr->non_native_to_hit_adj != 100) && (f_l_ptr->f_l_non_native_to_hit_adj > 0))
    {
        text_out("  A non-native creature is");

        /*More information for who have observed movement more*/
        if (f_l_ptr->f_l_non_native_to_hit_adj > 100)
        {
            if (f_ptr->non_native_to_hit_adj > 100)
            {
                text_out_c(TERM_BLUE, format(" %d percent more", (f_ptr->non_native_to_hit_adj - 100)));
            }
            else text_out_c(TERM_BLUE, format(" %d percent less",
                (100 - f_ptr->non_native_to_hit_adj)));

        }
        else
        {
            if (ABS(f_ptr->non_native_to_hit_adj - 100) > 15) text_out_c(TERM_BLUE, " significantly");

            if (f_ptr->non_native_to_hit_adj > BASE_ENERGY_MOVE)  text_out_c(TERM_BLUE, " more");
            else text_out_c(TERM_BLUE, " less");

        }

        text_out(" successful in attacking and defending while fighting in this terrain.");
    }
}


static void describe_feature_stealth_effects(int f_idx, const feature_lore *f_l_ptr)
{
    feature_type *f_ptr = &f_info[f_idx];

    if (feat_ff2_match(f_idx, FF2_EFFECT)) return;

    /*Describe stealth effects*/
    if ((f_ptr->f_stealth_adj != 0) && (f_l_ptr->f_l_stealth_adj > 0))
    {

        text_out("  Walking through this terrain");

        if (f_l_ptr->f_l_stealth_adj > 100)
        {

            if (ABS(f_ptr->f_stealth_adj) >= 3) text_out(" significantly");
            else if (ABS(f_ptr->f_stealth_adj) == 2) text_out(" considerably");
            else if (ABS(f_ptr->f_stealth_adj) == 1) text_out(" somewhat");
        }

        if (f_ptr->f_stealth_adj  > 0)  text_out(" improves");
        else text_out(" reduces");

        text_out(" the player's stealth.");
    }
}


/*
 * This section describes dynamic features.
 * The description for each feature needs to be described on a case-by-case basis.
 * This function should be consistent with process_dynamic_terrain_aux for its output.
 */
static void describe_feature_dynamic(int f_idx, const feature_lore *f_l_ptr)
{
    feature_type *f_ptr = &f_info[f_idx];

    /*
     * Hack - describe Ethereal WallTeleport player
     * TODO - figure out how to remember this in feature_lore.
     */
    if (f_idx == FEAT_ETHEREAL_WALL)
    {
        text_out("  This wall can teleport you across the dungeon, or explode in a burst of light.");
    }

    /* TODO - figure out how to remember this in feature_lore.*/
    if (f_idx == FEAT_WALL_INSCRIPTION)
    {
        text_out("  This wall can cast a spell at the player,");
        text_out(" or reveal some useful information about the current dungeon level.");

        return;
    }

    /* Has not been observed */
    if (!(f_l_ptr->f_l_flags3 & (FF3_DYNAMIC))) return;

    /* Dynamic fire can spread smoke and fire */
    if (_feat_ff3_match(f_ptr, FF3_FIRE))
    {
        text_out("  This terrain can spread fire and smoke to adjacent terrains.");

        return;
    }

    if (f_idx == FEAT_GEYSER)
    {
        text_out("  The geyser can explode in a burst of boiling water!");

        /* Done */
        return;
    }

    if (f_idx == FEAT_FSOIL_DYNAMIC)
    {
        text_out("  This feature can slowly spread across the dungeon.");

        /* Done */
        return;
    }

    /* Sniper flowers */
    if (f_idx == FEAT_PUTRID_FLOWER)
    {
        text_out("  This flower can fire spikes or spit poison at you.");

        /* Done */
        return;
    }

    /* Silent watchers */
    if (f_idx == FEAT_SILENT_WATCHER)
    {
        text_out("  The silent watcher can aggravate nearly monsters.");

        /* Done */
        return;
    }

    /* Dynamic lava can spread fire */
    if (_feat_ff3_match(f_ptr, TERRAIN_MASK) == (ELEMENT_LAVA))
    {
        text_out("  This terrain can spread fire to adjacent terrains.");

        return;
    }
}

/*
 * Hack -- display feature information using "roff()"
 *
 *
 * This function should only be called with the cursor placed at the
 * left edge of the screen or line, on a cleared line, in which the output is
 * to take place.  One extra blank line is left after the recall.
 */
void describe_feature(int f_idx, bool spoilers)
{
    feature_lore lore;

    feature_lore save_mem;

    /* Get the race and lore */
    const feature_type *f_ptr = &f_info[f_idx];
    feature_lore *f_l_ptr = &f_l_list[f_idx];

    /* Cheat -- know everything */
    if (cheat_know)
    {
        /* XXX XXX XXX */

        /* Hack -- save memory */
        COPY(&save_mem, f_l_ptr, feature_lore);
    }

    /* Hack -- create a copy of the monster-memory */
    COPY(&lore, f_l_ptr, feature_lore);

    /* Assume some "obvious" flags */
    lore.f_l_flags1 |= (f_ptr->f_flags1 & FF1_OBVIOUS_MASK);
    lore.f_l_flags2 |= (f_ptr->f_flags2 & FF2_OBVIOUS_MASK);
    lore.f_l_flags3 |= (f_ptr->f_flags3 & FF3_OBVIOUS_MASK);

    /* Cheat -- know everything*/
    if (cheat_know || spoilers)
    {
        cheat_feature_lore(f_idx, &lore);
    }

    /* Describe the movement and level of the monster */
    describe_feature_basic(f_idx, &lore);

    /* Describe the movement, LOS, and projection */
    describe_feature_move_see_cast(f_idx, &lore);

    /* Describe stairs */
    if (lore.f_l_flags1 & FF1_STAIRS) describe_feature_stairs(&lore);

    /* Describe trap */
    if (lore.f_l_flags1 & FF1_TRAP) describe_feature_trap(f_idx, &lore);

    describe_feature_interaction(f_idx, &lore);

    describe_feature_vulnerabilities(&lore);

    describe_feature_transitions(f_idx, &lore);

    describe_feature_damage(f_idx, &lore);

    describe_feature_movement_effects(f_idx, &lore);

    describe_feature_combat_effects(f_idx, &lore);

    describe_feature_stealth_effects(f_idx, &lore);

    describe_feature_dynamic(f_idx, &lore);

    /* All done */
    text_out("\n");
}

