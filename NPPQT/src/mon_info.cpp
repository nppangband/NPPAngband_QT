/* File: mon_desc.cpp */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 * 						Jeff Greene, Diego Gonzalez
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

#include "src/npp.h"

/*
 * Pronoun arrays, by gender.
 */
static QString wd_he[3] =
{ "it", "he", "she" };
static QString wd_his[3] =
{ "its", "his", "her" };

/*
 * Determine if the "armor" is known
 * The higher the level, the fewer kills needed.
 */
static bool know_armour(int r_idx, s32b kills)
{
    const monster_race *r_ptr = &r_info[r_idx];

    s32b level = r_ptr->level;

    /* Normal monsters */
    if (kills > 304 / (4 + level)) return (TRUE);

    /* Skip non-uniques */
    if (!(r_ptr->flags1 & (RF1_UNIQUE))) return (FALSE);

    /* Unique monsters */
    if (kills > 304 / (38 + (5*level) / 4)) return (TRUE);

    /* Assume false */
    return (FALSE);
}


/*
 * Determine if the "mana" is known
 * The higher the level, the fewer kills needed.
 */
static bool know_mana_or_spells(int r_idx)
{
    monster_race *r_ptr = &r_info[r_idx];

    s32b level = r_ptr->level;

    s32b kills = l_list[r_idx].tkills;

    /*Hack - always know about ghosts*/
    if (r_ptr->flags2 & (RF2_PLAYER_GHOST)) return (TRUE);

    /* Mages learn quickly. */
    if (cp_ptr->spell_book == TV_MAGIC_BOOK) kills *= 2;

    /* Normal monsters */
    if (kills > 304 / (4 + level)) return (TRUE);

    /* Skip non-uniques */
    if (!(r_ptr->flags1 & RF1_UNIQUE)) return (FALSE);

    /* Unique monsters */
    if (kills > 304 / (38 + (5 * level) / 4)) return (TRUE);

    /* Assume false */
    return (FALSE);
}


/*
 * Determine if the "damage" of the given attack is known
 * the higher the level of the monster, the fewer the attacks you need,
 * the more damage an attack does, the more attacks you need
 */
static bool know_damage(int r_idx, const monster_lore *l_ptr, int i)
{
    const monster_race *r_ptr = &r_info[r_idx];

    s32b level = r_ptr->level;

    s32b a = l_ptr->blows[i];

    s32b d1 = r_ptr->blow[i].d_dice;
    s32b d2 = r_ptr->blow[i].d_side;

    s32b d = d1 * d2;

    /* Hack - keep the target number reasonable */
    if (d > 100) d = 100;

    /* Normal monsters */
    if ((4 + level) * a > 80 * d) return (TRUE);

    /* Skip non-uniques */
    if (!(r_ptr->flags1 & RF1_UNIQUE)) return (FALSE);

    /* Unique monsters */
    if ((4 + level) * (2 * a) > 80 * d) return (TRUE);

    /* Assume false */
    return (FALSE);
}


static QString describe_monster_desc(int r_idx)
{
    const monster_race *r_ptr = &r_info[r_idx];
    QString output;

    /* Simple method */
    output =  r_ptr->r_text;

    /* Dump it */
    output.append("<br><br>");
    return (output);
}


QString describe_monster_spells(int r_idx, const monster_lore *l_ptr)
{
    QString output;
    const monster_race *r_ptr = &r_info[r_idx];
    int m, n;
    int msex = 0;
    int spower;
    bool breath = FALSE;
    bool magic = FALSE;
    int vn;
    QString vp[64];

    output.clear();

    /* Extract a gender (if applicable) */
    if (r_ptr->flags1 & RF1_FEMALE) msex = 2;
    else if (r_ptr->flags1 & RF1_MALE) msex = 1;

    /* Get spell power */
    spower = r_ptr->spell_power;

    /* Collect innate attacks */
    vn = 0;

    if (l_ptr->r_l_flags4 & (RF4_SHRIEK))		vp[vn++] = "shriek for help";

    if (l_ptr->r_l_flags4 & (RF4_LASH))
    {
        if ((l_ptr->r_l_flags3 & (RF3_ANIMAL)) || (r_ptr->blow[0].effect == RBE_ACID))
            vp[vn++] = "spit at you from a distance";
        else
            vp[vn++] = "lash you if nearby";
    }

    if (l_ptr->r_l_flags4 & (RF4_BOULDER))
    {
        if (spower < 8) vp[vn++] = "throw rocks";
        else vp[vn++] = "throw boulders";
    }

    if (l_ptr->r_l_flags4 & (RF4_SHOT))
    {
        if (spower < 4) vp[vn++] = "sling pebbles";
        else if (spower < 10) vp[vn++] = "sling leaden pellets";
        else vp[vn++] = "sling seeker shots";
    }

    if (l_ptr->r_l_flags4 & (RF4_ARROW))
    {
        if (spower < 4) vp[vn++] = "shoot little arrows";
        else if (spower < 10) vp[vn++] = "shoot arrows";
        else vp[vn++] = "shoot seeker arrows";
    }

    if (l_ptr->r_l_flags4 & (RF4_BOLT))
    {
        if (spower < 4) vp[vn++] = "fire bolts";
        else if (spower < 10) vp[vn++] = "fire crossbow quarrels";
        else vp[vn++] = "fire seeker bolts";
    }

    if (l_ptr->r_l_flags4 & (RF4_MISSL))
    {
        if (spower < 4) vp[vn++] = "fire little missiles";
        else if (spower < 10) vp[vn++] = "fire missiles";
        else vp[vn++] = "fire heavy missiles";
    }

    if (l_ptr->r_l_flags4 & (RF4_PMISSL)) vp[vn++] = "whip poisoned darts";

    /* Describe innate attacks */
    if (vn)
    {
        /* Intro */
        output.append(QString("%1") .arg(capitalize_first(wd_he[msex])));

        /* Scan */
        for (n = 0; n < vn; n++)
        {
            /* Intro */
            if (n == 0)
            {
                output.append(" may ");
            }

            else if (n < vn-1) output.append(", ");
            else if (n == 1) output.append(" or ");
            else output.append(", or ");


            /* Dump */
            output.append(color_string(capitalize_first(vp[n]), TERM_RED));
        }

        /* End */
        output.append(".<br><br>");
    }

    /* Collect breaths */
    vn = 0;

    if (l_ptr->r_l_flags4 & (RF4_BRTH_ACID))       vp[vn++] = "acid";
    if (l_ptr->r_l_flags4 & (RF4_BRTH_ELEC))       vp[vn++] = "lightning";
    if (l_ptr->r_l_flags4 & (RF4_BRTH_FIRE))       vp[vn++] = "fire";
    if (l_ptr->r_l_flags4 & (RF4_BRTH_COLD))       vp[vn++] = "frost";
    if (l_ptr->r_l_flags4 & (RF4_BRTH_POIS))       vp[vn++] = "poison";
    if (l_ptr->r_l_flags4 & (RF4_BRTH_PLAS))       vp[vn++] = "plasma";

    if (l_ptr->r_l_flags4 & (RF4_BRTH_LIGHT))       vp[vn++] = "light";
    if (l_ptr->r_l_flags4 & (RF4_BRTH_DARK))	   vp[vn++] = "darkness";
    if (l_ptr->r_l_flags4 & (RF4_BRTH_CONFU))      vp[vn++] = "confusion";
    if (l_ptr->r_l_flags4 & (RF4_BRTH_SOUND))      vp[vn++] = "sound";
    if (l_ptr->r_l_flags4 & (RF4_BRTH_SHARD))      vp[vn++] = "shards";
    if (l_ptr->r_l_flags4 & (RF4_BRTH_INER))       vp[vn++] = "inertia";
    if (l_ptr->r_l_flags4 & (RF4_BRTH_GRAV))       vp[vn++] = "gravity";
    if (l_ptr->r_l_flags4 & (RF4_BRTH_FORCE))      vp[vn++] = "force";

    if (l_ptr->r_l_flags4 & (RF4_BRTH_NEXUS))      vp[vn++] = "nexus";
    if (l_ptr->r_l_flags4 & (RF4_BRTH_NETHR))      vp[vn++] = "nether";
    if (l_ptr->r_l_flags4 & (RF4_BRTH_CHAOS))      vp[vn++] = "chaos";
    if (l_ptr->r_l_flags4 & (RF4_BRTH_DISEN))      vp[vn++] = "disenchantment";
    if (l_ptr->r_l_flags4 & (RF4_BRTH_TIME))       vp[vn++] = "time";
    if (l_ptr->r_l_flags4 & (RF4_BRTH_MANA))       vp[vn++] = "mana";

    if (l_ptr->r_l_flags4 & (RF4_RF4XXX1))         vp[vn++] = "something";
    if (l_ptr->r_l_flags4 & (RF4_RF4XXX2))            vp[vn++] = "something";
    if (l_ptr->r_l_flags4 & (RF4_RF4XXX3))            vp[vn++] = "something";

    /* Describe breaths */
    if (vn)
    {
        /* Note breath */
        breath = TRUE;

        /* Intro */
        output.append(QString("%1") .arg(capitalize_first(wd_he[msex])));

        /* Scan */
        for (n = 0; n < vn; n++)
        {
            /* Intro */
            if (n == 0) output.append(" may breathe ");
            else if (n < vn-1) output.append(", ");
            else if (n == 1) output.append(" or ");
            else output.append(", or ");

            /* Dump */
            output.append(color_string(vp[n], TERM_RED));
        }

        /*note powerful*/
        if (l_ptr->r_l_flags2 & (RF2_POWERFUL)) output.append(" powerfully");
    }


    /* Collect spells */
    vn = 0;

    if (l_ptr->r_l_flags5 & (RF5_BALL_ACID))
    {
        if (r_ptr->flags4 & (RF4_BRTH_ACID))
        {
            if (spower < 40)	vp[vn++] = "breathe acid balls";
            else 				vp[vn++] = "breathe enormous acid balls";
        }
        else if (spower < 10) vp[vn++] = "produce small acid balls";
        else if (spower < 40) vp[vn++] = "produce acid balls";
        else vp[vn++] = "produce acid storms";
    }

    if (l_ptr->r_l_flags5 & (RF5_BALL_ELEC))
    {
        if (r_ptr->flags4 & (RF4_BRTH_ELEC))
        {
            if (spower < 40)	vp[vn++] = "breathe lightning balls";
            else 				vp[vn++] = "breathe enormous lightning balls";
        }
        else if (spower < 10) vp[vn++] = "produce small lightning balls";
        else if (spower < 40) vp[vn++] = "produce lightning balls";
        else vp[vn++] = "produce lightning storms";
    }

    if (l_ptr->r_l_flags5 & (RF5_BALL_FIRE))
    {
        if (r_ptr->flags4 & (RF4_BRTH_FIRE))
        {
            if (spower < 40)	vp[vn++] = "breathe balls of flames";
            else 				vp[vn++] = "breathe enormous balls of flames";
        }
        else if (spower < 10) vp[vn++] = "produce small fire balls";
        else if (spower < 40) vp[vn++] = "produce fire balls";
        else vp[vn++] = "produce fire storms";
    }

    if (l_ptr->r_l_flags5 & (RF5_BALL_COLD))
    {
        if (r_ptr->flags4 & (RF4_BRTH_COLD))
        {
            if (spower < 40)	vp[vn++] = "breathe balls of frost";
            else 				vp[vn++] = "breathe enormous balls of frost";
        }
        else if (spower < 10) vp[vn++] = "produce small frost balls";
        else if (spower < 40) vp[vn++] = "produce frost balls";
        else vp[vn++] = "produce frost storms";
    }

    if (l_ptr->r_l_flags5 & (RF5_BALL_POIS))
    {
        if (r_ptr->flags4 & (RF4_BRTH_POIS))
        {
            if (spower < 40)	vp[vn++] = "breathe balls of poison";
            else 				vp[vn++] = "breathe enormous balls of poison";
        }
        else if (spower < 10) vp[vn++] = "produce stinking clouds";
        else if (spower < 40) vp[vn++] = "produce poison balls";
        else vp[vn++] = "produce storms of poison";
    }

    if (l_ptr->r_l_flags5 & (RF5_BALL_LIGHT))
    {
        if (r_ptr->flags4 & (RF4_BRTH_LIGHT))
        {
            if (spower < 40)	vp[vn++] = "breathe balls of light";
            else 				vp[vn++] = "breathe brilliant balls of light";
        }
        else if (spower < 10) vp[vn++] = "produce spheres of light";
        else if (spower < 40) vp[vn++] = "produce explosions of light";
        else vp[vn++] = "produce powerful explosions of light";
    }

    if (l_ptr->r_l_flags5 & (RF5_BALL_DARK))
    {
        if (r_ptr->flags4 & (RF4_BRTH_DARK))
        {
            if (spower < 40)	vp[vn++] = "breathe balls of darkness";
            else 				vp[vn++] = "breathe enormous balls of darkness";
        }
        else if (spower < 20) vp[vn++] = "produce balls of darkness";
        else if (spower < 70) vp[vn++] = "produce storms of darkness";
        else vp[vn++] = "produce powerful storms of darkness";
    }

    if (l_ptr->r_l_flags5 & (RF5_BALL_CONFU))
    {
        if (r_ptr->flags4 & (RF4_BRTH_CONFU))
        {
            if (spower < 40)	vp[vn++] = "breathe balls of confusion";
            else 				vp[vn++] = "breathe massive balls of confusion";
        }
        else if (spower < 10) vp[vn++] = "produce balls of confusion";
        else if (spower < 40) vp[vn++] = "produce storms of confusion";
        else vp[vn++] = "produce powerful storms of confusion";
    }

    if (l_ptr->r_l_flags5 & (RF5_BALL_SOUND))
    {
        if (r_ptr->flags4 & (RF4_BRTH_SOUND))
        {
            if (spower < 40)	vp[vn++] = "breathe balls of noise";
            else 				vp[vn++] = "breathe ear-splitting balls of noise";
        }
        else if (spower < 10) vp[vn++] = "produce blasts of sound";
        else if (spower < 40) vp[vn++] = "produce thunderclaps";
        else vp[vn++] = "unleash storms of sound";
    }

    if (l_ptr->r_l_flags5 & (RF5_BALL_SHARD))
    {
        if (r_ptr->flags4 & (RF4_BRTH_SHARD))
        {
            if (spower < 40)	vp[vn++] = "breathe balls of shards";
            else 				vp[vn++] = "breathe enormous balls of shards";
        }
        else if (spower < 10) vp[vn++] = "produce blasts of shards";
        else if (spower < 50) vp[vn++] = "produce whirlwinds of shards";
        else vp[vn++] = "call up storms of knives";
    }
    if (l_ptr->r_l_flags5 & (RF5_BALL_METEOR))
    {
        if (spower < 10) vp[vn++] = "produce meteor showers";
        else if (spower < 50) vp[vn++] = "produce meteor storms";
        else vp[vn++] = "produce violent meteor storms";
    }

    if (l_ptr->r_l_flags5 & (RF5_BALL_STORM))
    {
        if (spower < 22) vp[vn++] = "produce little storms";
        else if (spower < 40) vp[vn++] = "produce whirlpools";
        else vp[vn++] = "call up raging storms";
    }

    if (l_ptr->r_l_flags5 & (RF5_BALL_NETHR))
    {
        if (r_ptr->flags4 & (RF4_BRTH_NETHR))
        {
            if (spower < 40)	vp[vn++] = "breathe nether balls";
            else 				vp[vn++] = "breathe enormous nether balls";
        }
        else if (spower < 22) vp[vn++] = "produce nether orbs";
        else if (spower < 40) vp[vn++] = "produce nether balls";
        else vp[vn++] = "invoke nether storms";
    }

    if (l_ptr->r_l_flags5 & (RF5_BALL_CHAOS))
    {
        if (r_ptr->flags4 & (RF4_BRTH_CHAOS))
        {
            if (spower < 40)	vp[vn++] = "breathe balls of chaos";
            else 				vp[vn++] = "breathe enormous balls of chaos";
        }
        else if (spower < 13) vp[vn++] = "produce spheres of chaos";
        else if (spower < 40) vp[vn++] = "produce explosions of chaos";
        else vp[vn++] = "call up maelstroms of raw chaos";
    }

    if (l_ptr->r_l_flags5 & (RF5_BALL_MANA))
    {
        if (spower < 25) vp[vn++] = "produce manabursts";
        else if (spower < 50) vp[vn++] = "produce balls of mana";
        else vp[vn++] = "invoke mana storms";
    }

    if (l_ptr->r_l_flags5 & (RF5_BALL_WATER))
    {
        if (spower < 16) vp[vn++] = "produce water balls";
        else if (spower < 40) vp[vn++] = "produce water balls";
        else vp[vn++] = "produce storms of water balls";
    }

    if (l_ptr->r_l_flags5 & (RF5_HOLY_ORB))
    {
        if (spower < 25) vp[vn++] = "produce orbs of draining";
        else if (spower < 50) vp[vn++] = "produce powerful orbs of draining";
        else vp[vn++] = "produce large orbs of holy might";
    }

    if (l_ptr->r_l_flags5 & (RF5_BOLT_ACID))		vp[vn++] = "produce acid bolts";
    if (l_ptr->r_l_flags5 & (RF5_BOLT_ELEC))		vp[vn++] = "produce lightning bolts";
    if (l_ptr->r_l_flags5 & (RF5_BOLT_FIRE))		vp[vn++] = "produce fire bolts";
    if (l_ptr->r_l_flags5 & (RF5_BOLT_COLD))		vp[vn++] = "produce frost bolts";
    if (l_ptr->r_l_flags5 & (RF5_BOLT_POIS))		vp[vn++] = "produce poison bolts";
    if (l_ptr->r_l_flags5 & (RF5_BOLT_PLAS))		vp[vn++] = "produce plasma bolts";
    if (l_ptr->r_l_flags5 & (RF5_BOLT_ICE))		vp[vn++] = "produce ice bolts";
    if (l_ptr->r_l_flags5 & (RF5_BOLT_WATER))	vp[vn++] = "produce water bolts";
    if (l_ptr->r_l_flags5 & (RF5_BOLT_NETHR))
    {
        if (spower < 40) vp[vn++] = "produces a nether bolt";
        else vp[vn++] = "hurls black bolts of nether";
    }

    if (l_ptr->r_l_flags5 & (RF5_BOLT_MANA))
    {
        if (spower < 5) vp[vn++] = "fire magic missiles";
        else vp[vn++] = "fire mana bolts";
    }
    if (l_ptr->r_l_flags5 & (RF5_BOLT_GRAV))
    {
        if (spower < 5) vp[vn++] = "fires gravity bolts";
        else vp[vn++] = "shoots powerful bolts of gravity";
    }

    if (l_ptr->r_l_flags5 & (RF5_BEAM_ELEC))
    {
        if (r_ptr->flags4 & (RF4_BRTH_ELEC))
        {
            vp[vn++] = "breathe lightning bolts";
        }
        else vp[vn++] = "shoot sparks of lightning";
    }
    if (l_ptr->r_l_flags5 & (RF5_BEAM_ICE))
    {
        if (r_ptr->flags4 & (RF4_BRTH_ELEC))
        {
            vp[vn++] = "breathe spears of ice";
        }
        else 	vp[vn++] = "shoot lances of ice";
    }

    if (l_ptr->r_l_flags5 & (RF5_BEAM_NETHR))
    {
        if (r_ptr->flags4 & (RF4_BRTH_NETHR))
        {
            vp[vn++] = "breathe beams of nether";
        }
        else if (spower < 25) vp[vn++] = "shoot beams of nether";
        else if (spower < 50) vp[vn++] = "hurl lances of nether";
        else vp[vn++] = "shoot rays of death";
    }
    if (l_ptr->r_l_flags5 & (RF5_BEAM_LAVA))
    {
        /* SLightly different message for breathers */
        if (r_ptr->flags4 & (RF4_BRTH_ALL))
        {
            vp[vn++] = "breathe streams of fiery lava";
        }
        else if (spower < 25) vp[vn++] = "shoots beams of molten magma";
        else if (spower < 50) vp[vn++] = "shoots jets of lava";
        else vp[vn++] = "shoots searing jets of lava";
    }

    if (l_ptr->r_l_flags6 & RF6_HASTE)       vp[vn++] = "haste-self";
    if (l_ptr->r_l_flags6 & (RF6_ADD_MANA))		vp[vn++] = "restore mana";
    if (l_ptr->r_l_flags6 & RF6_HEAL)        vp[vn++] = "heal-self";
    if (l_ptr->r_l_flags6 & (RF6_CURE))		vp[vn++] = "cure what ails it";
    if (l_ptr->r_l_flags6 & RF6_BLINK)       vp[vn++] = "blink-self";
    if (l_ptr->r_l_flags6 & RF6_TPORT)       vp[vn++] = "teleport-self";
    if (l_ptr->r_l_flags6 & (RF6_TELE_SELF_TO))	vp[vn++] = "teleport toward you";
    if (l_ptr->r_l_flags6 & RF6_TELE_TO)     vp[vn++] = "teleport to";
    if (l_ptr->r_l_flags6 & RF6_TELE_AWAY)   vp[vn++] = "teleport away";
    if (l_ptr->r_l_flags6 & RF6_TELE_LEVEL)  vp[vn++] = "teleport level";
    if (l_ptr->r_l_flags6 & RF6_DARKNESS)    vp[vn++] = "create darkness";
    if (l_ptr->r_l_flags6 & RF6_TRAPS)       vp[vn++] = "create traps";

    if (l_ptr->r_l_flags6 & (RF6_DRAIN_MANA))	vp[vn++] = "drain mana";
    if (l_ptr->r_l_flags6 & (RF6_MIND_BLAST))	vp[vn++] = "cause mind blasting";
    if (l_ptr->r_l_flags6 & (RF6_BRAIN_SMASH))	vp[vn++] = "cause brain smashing";
    if (l_ptr->r_l_flags6 & (RF6_WOUND))
    {
        if (spower < 4) vp[vn++] = "cause light wounds";
        else if (spower < 10) vp[vn++] = "cause medium wounds";
        else if (spower < 20) vp[vn++] = "cause serious wounds";
        else if (spower < 35) vp[vn++] = "cause critical wounds";
        else vp[vn++] = "cause mortal wounds";
    }
    if (l_ptr->r_l_flags6 & (RF6_HUNGER))		vp[vn++] = "cause hunger";
    if (l_ptr->r_l_flags6 & (RF6_SCARE))		vp[vn++] = "terrify";
    if (l_ptr->r_l_flags6 & (RF6_BLIND))		vp[vn++] = "blind";
    if (l_ptr->r_l_flags6 & (RF6_CONF))		vp[vn++] = "confuse";
    if (l_ptr->r_l_flags6 & (RF6_SLOW))		vp[vn++] = "slow";
    if (l_ptr->r_l_flags6 & (RF6_HOLD))		vp[vn++] = "paralyze";

    m = vn;

    /* Summons are described somewhat differently. */
    if (l_ptr->r_l_flags7)
    {

        /* Summons */
        if (l_ptr->r_l_flags7 & (RF7_S_KIN))
        {
            if (r_ptr->flags1 & (RF1_UNIQUE))
            {
                if (r_ptr->flags1 & (RF1_FEMALE)) vp[vn++] = "her minions";
                else if (r_ptr->flags1 & (RF1_MALE)) vp[vn++] = "his minions";
                else vp[vn++] = "its minions";
            }
            else
                vp[vn++] = "similar monsters";
        }
        if (l_ptr->r_l_flags7 & (RF7_S_MONSTER))		vp[vn++] = "a monster";
        if (l_ptr->r_l_flags7 & (RF7_S_MONSTERS))	vp[vn++] = "monsters";
        if (l_ptr->r_l_flags7 & (RF7_S_ANT))		vp[vn++] = "ants";
        if (l_ptr->r_l_flags7 & (RF7_S_SPIDER))		vp[vn++] = "spiders";
        if (l_ptr->r_l_flags7 & (RF7_S_HOUND))		vp[vn++] = "hounds";
        if (l_ptr->r_l_flags7 & (RF7_S_ANIMAL))		vp[vn++] = "natural creatures";
        if (l_ptr->r_l_flags7 & (RF7_S_HYDRA))		vp[vn++] = "hydras";
        if (l_ptr->r_l_flags7 & (RF7_S_THIEF))		vp[vn++] = "thieves";
        if (l_ptr->r_l_flags7 & (RF7_S_BERTBILLTOM))	vp[vn++] = "his friends";
        if (l_ptr->r_l_flags7 & (RF7_S_DRAGON))		vp[vn++] = "a dragon";
        if (l_ptr->r_l_flags7 & (RF7_S_HI_DRAGON))	vp[vn++] = "Ancient Dragons";
        if (l_ptr->r_l_flags7 & (RF7_S_AINU))		vp[vn++] = "a maia";
        if (l_ptr->r_l_flags7 & (RF7_S_DEMON))		vp[vn++] = "a demon";
        if (l_ptr->r_l_flags7 & (RF7_S_HI_DEMON))	vp[vn++] = "Greater Demons";
        if (l_ptr->r_l_flags7 & (RF7_S_UNIQUE))		vp[vn++] = "Unique Monsters";
        if (l_ptr->r_l_flags7 & (RF7_S_HI_UNIQUE))	vp[vn++] = "Greater Unique Monsters";
        if (l_ptr->r_l_flags7 & (RF7_S_UNDEAD))		vp[vn++] = "an undead";
        if (l_ptr->r_l_flags7 & (RF7_S_HI_UNDEAD))	vp[vn++] = "Greater Undead";
        if (l_ptr->r_l_flags7 & (RF7_S_WRAITH))		vp[vn++] = "the Ringwraiths";

    }



    /* Describe spells */
    if (vn)
    {
        /* Note magic */
        magic = TRUE;

        /* Intro */
        if (breath)
        {
            output.append(", and is also");
        }
        else
        {
            output.append(QString("%1 is") .arg(capitalize_first(wd_he[msex])));
        }

        /* Verb Phrase */
        output.append(" magical, casting spells");

        /* Adverb */
        if (l_ptr->r_l_flags2 & RF2_SMART) output.append(color_string(" intelligently", TERM_ORANGE));

        /* Normal spells */
        for (n = 0; n < m; n++)
        {
            if (n == 0)       output.append(" which ");
            else if (n < m-1) output.append(", ");
            else if (n != 1)  output.append(", or ");
            else              output.append(" or ");

            /* Dump */
            output.append(color_string(vp[n], TERM_RED));
        }

        /* Summons */
        for (n = m; n < vn; n++)
        {
            if (n == 0) output.append(" which summon ");
            else if (n == m) output.append(", or summon ");
            else if (n < vn-1) output.append(", ");
            else if (n == m+1) output.append(" or ");
            else output.append(", or ");

            /* Dump */
            output.append(color_string(vp[n], TERM_L_RED));
        }
    }


    /* End the sentence about innate/other spells */
    if (breath || magic)
    {
        /* Total casting */
        m = l_ptr->ranged;

        /* Average frequency */
        n = (r_ptr->freq_ranged);

        /*players don't hone in on spell frequency right away*/
        if (m < 75)
        {
            /*sometimes minus, sometimes plus*/
            if (n % 2) n -= ((100 - m) / 10);
            else n += ((100 - m) / 10);

            /*boundry control*/
            if (n > 100) n = 100;
            if (n < 1) n = 1;

        }

        /* Describe the spell frequency */
        if (m > 30)
        {
            output.append(QString(" about %1 percent of the time") .arg(n));
        }

        /* Describe monster mana and spellpower*/
        if (((r_ptr->mana) || (r_ptr->spell_power)) && know_mana_or_spells(r_idx))
        {
            output.append(" with");

            /* Mana */
            if (r_ptr->mana)
            {
                output.append(QString(" a mana rating of %1") .arg(r_ptr->mana));

                if (r_ptr->spell_power) output.append(" and");
            }

            /* spell power */
            if (r_ptr->spell_power)
            {
                output.append(QString(" a spell power of %1") .arg(r_ptr->spell_power));
            }
        }

        /* End this sentence */
        output.append(".");
    }

    if (!output.isEmpty()) output.append("<br><br>");

    return (output);
}


static QString describe_monster_drop(int r_idx, const monster_lore *l_ptr)
{
    const monster_race *r_ptr = &r_info[r_idx];

    bool sin = FALSE;

    int n;

    QString output, p;

    int msex = 0;

    output.clear();
    p.clear();

    /* Extract a gender (if applicable) */
    if (r_ptr->flags1 & RF1_FEMALE) msex = 2;
    else if (r_ptr->flags1 & RF1_MALE) msex = 1;

    /* No Drops gold and/or items */
    if (!l_ptr->drop_gold && !l_ptr->drop_item) return (output);

    /* Intro */
    output.append(QString("%1 may carry") .arg(capitalize_first(wd_he[msex])));

    /* Count maximum drop */
    n = MAX(l_ptr->drop_gold, l_ptr->drop_item);

    /* One drop (may need an "n") */
    if (n == 1)
    {
        output.append(" a");
        sin = TRUE;
    }

    /* Two drops */
    else if (n == 2)
    {
        output.append(" one or two");
    }

    /* Many drops */
    else
    {
        output.append(QString(" up to %1") .arg(n));
    }


    /* Chests are not noted as good or great
     * (no "n" needed)
     */
    if (l_ptr->r_l_flags1 & RF1_DROP_CHEST)
    {
        sin = FALSE;
    }

    /* Great */
    else if (l_ptr->r_l_flags1 & RF1_DROP_GREAT)
    {
        p.append(" exceptional");
    }

    /* Good (no "n" needed) */
    else if (l_ptr->r_l_flags1 & RF1_DROP_GOOD)
    {
        p.append(" good");
        sin = FALSE;
    }

    /* Objects */
    if (l_ptr->drop_item)
    {
        /* Handle singular "an" */
        if (sin) output.append("n");
        sin = FALSE;

        /* Dump "object(s)" */
        if (!p.isEmpty()) output.append(p);

        /*specify chests where needed*/
        if (l_ptr->r_l_flags1 & RF1_DROP_CHEST) output.append(" chest");
        else output.append(" object");
        if (n != 1) output.append("s");

        /* Conjunction replaces variety, if needed for "gold" below */
        p = " or";
    }

    /* Treasures */
    if (l_ptr->drop_gold)
    {
        /* Cancel prefix */
        if (p.isEmpty()) sin = FALSE;

        /* Handle singular "an" */
        if (sin) output.append("n");

        /* Dump "treasure(s)" */
        if (!p.isEmpty()) output.append(p);
        output.append(" treasure");
        if (n != 1) output.append("s");
    }
    /* End this sentence */
    output.append(".<br><br>  ");
    return (output);

}


static QString describe_monster_attack(int r_idx, const monster_lore *l_ptr)
{
    const monster_race *r_ptr = &r_info[r_idx];
    int m, r, n;
    QString p, q;
    QString output;
    output.clear();
    p.clear();
    q.clear();

    int msex = 0;

    /* Extract a gender (if applicable) */
    if (r_ptr->flags1 & RF1_FEMALE) msex = 2;
    else if (r_ptr->flags1 & RF1_MALE) msex = 1;

    /* Count the number of "known" attacks */
    for (n = 0, m = 0; m < MONSTER_BLOW_MAX; m++)
    {
        /* Skip non-attacks */
        if (!r_ptr->blow[m].method) continue;

        /* Count known attacks */
        if ((l_ptr->blows[m]) || (l_ptr->sights == MAX_SHORT) ||
                                  (l_ptr->ranged == MAX_UCHAR)) n++;
    }

    /* Examine (and count) the actual attacks */
    for (r = 0, m = 0; m < MONSTER_BLOW_MAX; m++)
    {
        int method, effect, d1, d2;

        /* Skip non-attacks */
        if (!r_ptr->blow[m].method) continue;

        /* Skip unknown attacks */
        if (!l_ptr->blows[m]) continue;

        /* Extract the attack info */
        method = r_ptr->blow[m].method;
        effect = r_ptr->blow[m].effect;
        d1 = r_ptr->blow[m].d_dice;
        d2 = r_ptr->blow[m].d_side;

        /* Get the method */
        switch (method)
        {
            case RBM_HIT:           p = "hit"; break;
            case RBM_TOUCH:         p = "touch"; break;
            case RBM_PUNCH:         p = "punch"; break;
            case RBM_KICK:          p = "kick"; break;
            case RBM_CLAW:          p = "claw"; break;
            case RBM_BITE:          p = "bite"; break;
            case RBM_PECK:          p = "peck"; break;
            case RBM_STING:         p = "sting"; break;
            case RBM_BREATHE:       p = "breathe";  break;
            case RBM_BUTT:          p = "butt"; break;
            case RBM_CRUSH:         p = "crush"; break;
            case RBM_ENGULF:        p = "engulf"; break;
            case RBM_CRAWL:         p = "crawl on you"; break;
            case RBM_DROOL:         p = "drool on you"; break;
            case RBM_SPIT:          p = "spit"; break;
            case RBM_SLIME:         p = "slime"; break;
            case RBM_GAZE:          p = "gaze"; break;
            case RBM_WAIL:          p = "wail"; break;
            case RBM_SPORE:         p = "release spores"; break;
            case RBM_TRAMPLE:       p = "tramples you"; break;break;
            case RBM_BEG:           p = "beg"; break;
            case RBM_INSULT:        p = "insult"; break;
            case RBM_XXX5:          break;
            case RBM_XXX6:			break;
        }

        /* Get the effect */
        switch (effect)
        {
            case RBE_HURT:          q = "attack"; break;
            case RBE_WOUND:         q = "wound"; break;
            case RBE_BATTER:        q = "stun"; break;
            case RBE_SHATTER:       q = "shatter"; break;

            case RBE_UN_BONUS:      q = "disenchant"; break;
            case RBE_UN_POWER:      q = "drain charges"; break;
            case RBE_LOSE_MANA:     q = "drain mana"; break;
            case RBE_EAT_GOLD:      q = "steal gold"; break;
            case RBE_EAT_ITEM:      q = "steal items"; break;
            case RBE_EAT_FOOD:      q = "eat your food"; break;
            case RBE_EAT_LIGHT:      q = "absorb light"; break;
            case RBE_HUNGER:        q = "cause hunger"; break;

            case RBE_POISON:        q = "poison"; break;
            case RBE_ACID:          q = "shoot acid"; break;
            case RBE_ELEC:          q = "electrocute"; break;
            case RBE_FIRE:          q = "burn"; break;
            case RBE_COLD:          q = "freeze"; break;

            case RBE_BLIND:         q = "blind"; break;
            case RBE_CONFUSE:       q = "confuse"; break;
            case RBE_TERRIFY:       q = "terrify"; break;
            case RBE_PARALYZE:      q = "paralyze"; break;
            case RBE_HALLU:         q = "induce hallucinations"; break;
            case RBE_DISEASE:       q = "cause disease"; break;

            case RBE_LOSE_STR:      q = "reduce strength"; break;
            case RBE_LOSE_INT:      q = "reduce intelligence"; break;
            case RBE_LOSE_WIS:      q = "reduce wisdom"; break;
            case RBE_LOSE_DEX:      q = "reduce dexterity"; break;
            case RBE_LOSE_CON:      q = "reduce constitution"; break;
            case RBE_LOSE_CHR:      q = "reduce charisma"; break;
            case RBE_LOSE_ALL:      q = "reduce all stats"; break;

            case RBE_EXP_10:        q = "lower experience (by 10d6+)"; break;
            case RBE_EXP_20:        q = "lower experience (by 20d6+)"; break;
            case RBE_EXP_40:        q = "lower experience (by 40d6+)"; break;
            case RBE_EXP_80:        q = "lower experience (by 80d6+)"; break;
        }

        /* Introduce the attack description */
        if (!r)
        {
            output.append(QString("%1 can ") .arg(capitalize_first(wd_he[msex])));
        }
        else if (r < n-1)
        {
            output.append(", ");
        }
        else
        {
            output.append(", and ");
        }


        /* Hack -- force a method */
        if (p.isEmpty()) p = "do something weird";

        /* Describe the method */
        output.append(p);

        /* Describe the effect (if any) */
        if (!q.isEmpty())
        {
            /* Describe the attack type */
            output.append(" to ");
            output.append(color_string(q, TERM_RED));

            /* Describe damage (if known) */
            if (d1 && d2 && ((know_damage(r_idx, l_ptr, m)) || (l_ptr->sights == MAX_SHORT) ||
                                  (l_ptr->ranged == MAX_UCHAR)))
            {
                /* Display the damage */
                output.append(" with damage");
                output.append(QString(" %1d%2") .arg(d1) .arg(d2));
            }
        }


        /* Count the attacks as printed */
        r++;
    }

    /* Finish sentence above */
    if (r)
    {
        output.append(".  ");
    }

    /* Notice lack of attacks */
    else if (l_ptr->r_l_flags1 & RF1_NEVER_BLOW)
    {
        output.append(QString("%1 has no physical attacks.  ") .arg(capitalize_first(wd_he[msex])));
    }

    /* Or describe the lack of knowledge */
    else
    {
        output.append(QString("Nothing is known about %1 attack.  ") .arg(wd_his[msex]));
    }

    if (!output.isEmpty()) output.append("<br><br>");
    return (output);
}


static QString describe_monster_abilities(int r_idx, const monster_lore *l_ptr)
{
    const monster_race *r_ptr = &r_info[r_idx];

    int n;

    int vn;
    QString vp[64];
    QString output;
    int msex = 0;

    output.clear();

    /* Extract a gender (if applicable) */
    if (r_ptr->flags1 & RF1_FEMALE) msex = 2;
    else if (r_ptr->flags1 & RF1_MALE) msex = 1;

    /* Collect special abilities. */
    vn = 0;
    if (l_ptr->r_l_flags2 & RF2_HAS_LIGHT)
    {
        QString tester = "hkoOTtPp";

        /*humaniods carry torches, others glow*/
        if (!tester.contains(r_ptr->d_char)) vp[vn++] = "radiate natural light";
        else vp[vn++] = "use a light source";
    }
    if (l_ptr->r_l_flags2 & RF2_EVASIVE) vp[vn++] = "dodge attacks";
    if (l_ptr->r_l_flags2 & RF2_OPEN_DOOR) vp[vn++] = "open doors";
    if (l_ptr->r_l_flags2 & RF2_BASH_DOOR) vp[vn++] = "bash down doors";
    if (l_ptr->r_l_flags2 & RF2_PASS_WALL) vp[vn++] = "pass through walls";
    if (l_ptr->r_l_flags2 & RF2_KILL_WALL) vp[vn++] = "bore through walls";
    if (l_ptr->r_l_flags2 & RF2_KILL_BODY) vp[vn++] = "destroy weaker monsters";
    if (l_ptr->r_l_flags2 & RF2_TAKE_ITEM) vp[vn++] = "pick up objects";
    if (l_ptr->r_l_flags2 & RF2_KILL_ITEM) vp[vn++] = "destroy objects";

    /* Describe special abilities. */
    if (vn)
    {
        /* Intro */
        output.append(QString("%1") .arg(capitalize_first(wd_he[msex])));

        /* Scan */
        for (n = 0; n < vn; n++)
        {
            /* Intro */
            if (n == 0) output.append(" can ");
            else if (n < vn-1) output.append(", ");
            else output.append(" and ");

            /* Dump */
            output.append(vp[n]);
        }
        output.append(".  ");
    }

    /*note if this is an unused ghost template*/
    if ((r_ptr->flags2 & (RF2_PLAYER_GHOST)) && (r_ptr->cur_num == 0))
    {
        output.append(QString("%1 is a player ghost template.  ") .arg(capitalize_first(wd_he[msex])));
    }

    /* Describe special abilities. */
    if (l_ptr->r_l_flags2 & RF2_INVISIBLE)
    {
        output.append(QString("%1 is invisible.  ") .arg(capitalize_first(wd_he[msex])));
    }
    if (l_ptr->r_l_flags2 & RF2_COLD_BLOOD)
    {
        output.append(QString("%1 is cold blooded.  ") .arg(capitalize_first(wd_he[msex])));
    }
    if (l_ptr->r_l_flags2 & RF2_STAY_NATIVE)
    {
        output.append(QString("%1 does not leave %2 native terrain.  ") .arg(capitalize_first(wd_he[msex])) .arg(wd_his[msex]));
    }
    if (l_ptr->r_l_flags2 & RF2_EMPTY_MIND)
    {
        output.append(QString("%1 is not detected by telepathy.  ") .arg(capitalize_first(wd_he[msex])));
    }
    if (l_ptr->r_l_flags2 & RF2_WEIRD_MIND)
    {
        output.append(QString("%1 is rarely detected by telepathy.  ") .arg(capitalize_first(wd_he[msex])));
    }
    if (l_ptr->r_l_flags2 & RF2_MULTIPLY)
    {
        output.append(QString("%1 breeds explosively.  ") .arg(capitalize_first(wd_he[msex])));
    }
    if (l_ptr->r_l_flags2 & RF2_REGENERATE)
    {
        output.append(QString("%1 regenerates quickly.  ") .arg(capitalize_first(wd_he[msex])));
    }

    if (l_ptr->r_l_flags2 & (RF2_CLOUD_SURROUND))
    {
        int typ = 0, dam = 0, rad = 0;

        /* Get type of cloud */
        cloud_surround(r_idx, &typ, &dam, &rad);

        /*hack - alter type for char-attr monster*/

        if ((r_ptr->flags1 & (RF1_ATTR_MULTI)) &&
            (r_ptr->flags4 & (RF4_BRTH_FIRE)) &&
            (r_ptr->flags4 & (RF4_BRTH_POIS)) &&
            (r_ptr->flags4 & (RF4_BRTH_ACID)) &&
            (r_ptr->flags4 & (RF4_BRTH_ELEC)) &&
            (r_ptr->flags4 & (RF4_BRTH_COLD)))
            {
                output.append(QString("%1 is surrounded by an ever-changing cloud of elements.  ") .arg(capitalize_first(wd_he[msex])));
            }


        /* We emit something */
        else if (typ)
        {
            output.append(QString("%1 is surrounded by ") .arg(capitalize_first(wd_he[msex])));

            /* Describe cloud */
            if (typ == GF_SPORE)     output.append("spores");
            else if (typ == GF_DARK)      output.append("darkness");
            else if (typ == GF_DARK_WEAK) output.append("darkness");
            else                          output.append("powerful forces");
            output.append(".<br><br>");
        }
    }

    /* Collect susceptibilities */
    vn = 0;
    if (l_ptr->r_l_flags3 & RF3_HURT_POIS) vp[vn++] = "poison";
    if (l_ptr->r_l_flags3 & RF3_HURT_ACID) vp[vn++] = "acid";
    if (l_ptr->r_l_flags3 & RF3_HURT_ROCK) vp[vn++] = "rock remover";
    if (l_ptr->r_l_flags3 & RF3_HURT_LIGHT) vp[vn++] = "bright light";
    if (l_ptr->r_l_flags3 & RF3_HURT_FIRE) vp[vn++] = "fire";
    if (l_ptr->r_l_flags3 & RF3_HURT_COLD) vp[vn++] = "cold";

    /* Describe susceptibilities */
    if (vn)
    {
        /* Intro */
        output.append(QString("%1") .arg(capitalize_first(wd_he[msex])));

        /* Scan */
        for (n = 0; n < vn; n++)
        {
            /* Intro */
            if (n == 0) output.append(" is hurt by ");
            else if (n < vn-1) output.append(", ");
            else output.append(" and ");

            /* Dump */
            output.append(color_string(vp[n], TERM_GOLD));
        }

        /* End */
        output.append(".<br><br>");
    }


    /* Collect immunities */
    vn = 0;
    if (l_ptr->r_l_flags3 & RF3_IM_ACID) vp[vn++] = "acid";
    if (l_ptr->r_l_flags3 & RF3_IM_ELEC) vp[vn++] = "lightning";
    if (l_ptr->r_l_flags3 & RF3_IM_FIRE) vp[vn++] = "fire";
    if (l_ptr->r_l_flags3 & RF3_IM_COLD) vp[vn++] = "cold";
    if (l_ptr->r_l_flags3 & RF3_IM_POIS) vp[vn++] = "poison";
    if (l_ptr->r_l_flags3 & RF3_RES_CHAOS) vp[vn++] = "chaos";
    if (l_ptr->r_l_flags3 & RF3_RES_NETHR) vp[vn++] = "nether";
    if (l_ptr->r_l_flags3 & RF3_RES_WATER) vp[vn++] = "water";
    if (l_ptr->r_l_flags3 & RF3_RES_PLAS) vp[vn++] = "plasma";
    if (l_ptr->r_l_flags3 & RF3_RES_NEXUS) vp[vn++] = "nexus";
    if (l_ptr->r_l_flags3 & RF3_RES_DISEN) vp[vn++] = "disenchantment";

    /* Describe immunities */
    if (vn)
    {
        /* Intro */
        output.append(QString("%1") .arg(capitalize_first(wd_he[msex])));

        /* Scan */
        for (n = 0; n < vn; n++)
        {
            /* Intro */
            if (n == 0) output.append(" resists ");
            else if (n < vn-1) output.append(", ");
            else output.append(" and ");

            /* Dump */
            output.append(color_string(vp[n], TERM_GREEN));
        }

        /* End */
        output.append(".  ");
    }

    /* Collect non-effects */
    vn = 0;
    if (l_ptr->r_l_flags3 & RF3_NO_SLOW) vp[vn++] = "slowed";
    if (l_ptr->r_l_flags3 & RF3_NO_STUN) vp[vn++] = "stunned";
    if (l_ptr->r_l_flags3 & RF3_NO_FEAR) vp[vn++] = "frightened";
    if (l_ptr->r_l_flags3 & RF3_NO_CONF) vp[vn++] = "confused";
    if (l_ptr->r_l_flags3 & RF3_NO_SLEEP) vp[vn++] = "slept";

    /* Describe non-effects */
    if (vn)
    {
        /* Intro */
        output.append(QString("%1") .arg(capitalize_first(wd_he[msex])));

        /* Scan */
        for (n = 0; n < vn; n++)
        {
            /* Intro */
            if (n == 0) output.append(" is highly resistant to being ");
            else if (n < vn-1) output.append(", ");
            else output.append(" or ");

            /* Dump */
            output.append(color_string(vp[n], TERM_GOLD));
        }

        /* End */
        output.append(".<br><br>");
    }

    return (output);
}


static QString describe_monster_kills(int r_idx, const monster_lore *l_ptr)
{
    const monster_race *r_ptr = &r_info[r_idx];
    QString output;
    int msex = 0;

    bool out = TRUE;
    output.clear();

    /* Extract a gender (if applicable) */
    if (r_ptr->flags1 & RF1_FEMALE) msex = 2;
    else if (r_ptr->flags1 & RF1_MALE) msex = 1;

    /* Treat uniques differently */
    if (l_ptr->r_l_flags1 & RF1_UNIQUE)
    {
        /* Hack -- Determine if the unique is "dead" */
        bool dead = (r_ptr->max_num == 0) ? TRUE : FALSE;

        /* We've been killed... */
        if (l_ptr->deaths)
        {
            /* Killed ancestors */
            output.append(QString("%1 has slain %2 of your ancestors")
                        .arg(capitalize_first(wd_he[msex])) .arg(l_ptr->deaths));

            /* But we've also killed it */
            if (dead)
            {
                output.append(", but you have taken revenge!  ");
            }

            /* Unavenged (ever) */
            else
            {
                QString remains;
                if (l_ptr->deaths > 1) remains = "remain";
                else remains = "remains";

                output.append(QString(", who %1 unavenged.  ") .arg(remains));
            }
        }

        /* Dead unique who never hurt us */
        else if (dead)
        {
            output.append("You have slain this foe.  ");
        }
        else
        {
            /* Alive and never killed us */
            out = FALSE;
        }
    }

    /* Not unique, but killed us */
    else if (l_ptr->deaths)
    {
        QString remains;
        if (l_ptr->deaths > 1) remains = "remain";
        else remains = "remains";

        /* Dead ancestors */
        output.append(QString("%1 of your ancestors %2 been killed by this creature, ")
                    .arg(l_ptr->deaths) .arg(remains));

        /* Some kills this life */
        if (l_ptr->pkills)
        {
            output.append(QString("and you have exterminated at least %1 of the creatures.  ") .arg(l_ptr->pkills));
        }

        /* Some kills past lives */
        else if (l_ptr->tkills)
        {
            output.append(QString("and your ancestors have exterminated at least %d of the creatures.  ") .arg(l_ptr->tkills));
        }

        /* No kills */
        else
        {
            output.append(color_string(QString("and %1 is not ever known to have been defeated.  ") .arg(capitalize_first(wd_he[msex])), TERM_RED));
        }

        output.append("<br><br>");
    }

    /* Normal monsters */
    else
    {
        /* Killed some this life */
        if (l_ptr->pkills)
        {
            output.append(QString("You have killed at least %1 of these creatures.  ") .arg(l_ptr->pkills));
        }

        /* Killed some last life */
        else if (l_ptr->tkills)
        {
            output.append(QString("Your ancestors have killed at least %1 of these creatures.  ") .arg(l_ptr->tkills));
        }

        /* Killed none */
        else
        {
            output.append("No battles to the death are recalled.  ");
        }
    }

    /* Separate */
    if (out) output.append("<br>");

    return (output);
}


static QString describe_monster_toughness(int r_idx, const monster_lore *l_ptr)
{
    const monster_race *r_ptr = &r_info[r_idx];
    QString output;
    int msex = 0;
    output.clear();

    /* Extract a gender (if applicable) */
    if (r_ptr->flags1 & RF1_FEMALE) msex = 2;
    else if (r_ptr->flags1 & RF1_MALE) msex = 1;

    /* Describe monster "toughness" */
    if ((know_armour(r_idx, l_ptr->tkills)) || (l_ptr->sights == MAX_SHORT) ||
             (l_ptr->ranged == MAX_UCHAR))
    {
        /* Armor */
        output.append(QString("%1 has an armor rating of %2") .arg(capitalize_first(wd_he[msex])) .arg(r_ptr->ac));

        /* Maximized hitpoints */
        if (l_ptr->r_l_flags1 & (RF1_FORCE_MAXHP))
        {
            output.append(QString(" and a life rating of %1.  ") .arg(r_ptr->hdice * r_ptr->hside));
        }

        /* Variable hitpoints */
        else
        {
            output.append(QString(" and a life rating of %1d%2.  ") .arg(r_ptr->hdice) .arg(r_ptr->hside));
        }

        output.append("<br><br>");
    }

    return (output);
}


static QString describe_monster_exp(int r_idx, const monster_lore *l_ptr)
{
    const monster_race *r_ptr = &r_info[r_idx];
    QString output;
    QString p, q;

    long i, j;

    output.clear();

    /* Describe experience if known */
    if (l_ptr->tkills)
    {
        /* Introduction */
        if (l_ptr->r_l_flags1 & RF1_UNIQUE)
            output.append("Killing");
        else
            output.append("A kill of");

        output.append(" this creature");

        /* calculate the integer exp part */
        i = (long)r_ptr->mexp * r_ptr->level / p_ptr->lev;

        /* calculate the fractional exp part scaled by 100, */
        /* must use long arithmetic to avoid overflow */
        j = ((((long)r_ptr->mexp * r_ptr->level % p_ptr->lev) *
              (long)1000 / p_ptr->lev + 5) / 10);

        /* Mention the experience */
        output.append(QString(" is worth %1.%2 point") .arg(number_to_formatted_string(i)) .arg(j));
        if ((i != 1) || (j != 0)) output.append("s");

        /* Take account of annoying English */
        p = "th";
        i = p_ptr->lev % 10;
        if ((p_ptr->lev / 10) == 1) /* nothing */;
        else if (i == 1) p = "st";
        else if (i == 2) p = "nd";
        else if (i == 3) p = "rd";

        /* Take account of "leading vowels" in numbers */
        q = "";
        i = p_ptr->lev;
        if ((i == 8) || (i == 11) || (i == 18)) q = "n";

        /* Mention the dependance on the player's level */
        output.append(QString(" for a%1 %2%3 level character.<br><br>") .arg(q) .arg(i) .arg(p));
    }

    return (output);
}


static QString describe_monster_movement(int r_idx, const monster_lore *l_ptr)
{
    const monster_race *r_ptr = &r_info[r_idx];
    byte energy_gain = calc_energy_gain(r_ptr->r_speed);
    QString output;
    bool old = FALSE;
    output.clear();

    int msex = 0;

    /* Extract a gender (if applicable) */
    if (r_ptr->flags1 & RF1_FEMALE) msex = 2;
    else if (r_ptr->flags1 & RF1_MALE) msex = 1;

    output.append("This");

    if (l_ptr->r_l_flags3 & RF3_FLYING) output.append(color_string(" flying", TERM_NAVY_BLUE));
    if (l_ptr->r_l_flags3 & RF3_ANIMAL) output.append(color_string(" natural", TERM_NAVY_BLUE));
    if (l_ptr->r_l_flags3 & RF3_EVIL) output.append(color_string(" evil", TERM_NAVY_BLUE));
    if (l_ptr->r_l_flags3 & RF3_UNDEAD) output.append(color_string(" undead", TERM_NAVY_BLUE));

    if (l_ptr->r_l_flags3 & RF3_DRAGON) output.append(color_string(" dragon", TERM_NAVY_BLUE));
    else if (l_ptr->r_l_flags3 & RF3_DEMON) output.append(color_string(" demon", TERM_NAVY_BLUE));
    else if (l_ptr->r_l_flags3 & RF3_GIANT) output.append(color_string(" giant", TERM_NAVY_BLUE));
    else if (l_ptr->r_l_flags3 & RF3_TROLL) output.append(color_string(" troll", TERM_NAVY_BLUE));
    else if (l_ptr->r_l_flags3 & RF3_ORC) output.append(color_string(" orc", TERM_NAVY_BLUE));
    else output.append(" creature");

    /* Describe location */
    if (r_ptr->level == 0)
    {
        output.append(color_string(" lives in the town", TERM_SLATE));
        old = TRUE;
    }
    else if ((l_ptr->tkills)  || (l_ptr->sights == MAX_SHORT) ||
             (l_ptr->ranged == MAX_UCHAR))
    {
        if (l_ptr->r_l_flags1 & RF1_FORCE_DEPTH) output.append(color_string(" is found", TERM_SLATE));
        else output.append(color_string(" is normally found", TERM_SLATE));

        output.append(color_string(QString(" at depths of %1 feet") .arg(r_ptr->level * 50), TERM_SLATE));

        old = TRUE;
    }

    if (old) output.append(", and");

    output.append(" moves");

    /* Random-ness */
    if ((l_ptr->r_l_flags1 & RF1_RAND_50) || (l_ptr->r_l_flags1 & RF1_RAND_25))
    {
        /* Adverb */
        if ((l_ptr->r_l_flags1 & RF1_RAND_50) && (l_ptr->r_l_flags1 & RF1_RAND_25))
        {
            output.append(" extremely");
        }
        else if (l_ptr->r_l_flags1 & RF1_RAND_50)
        {
            output.append(" somewhat");
        }
        else if (l_ptr->r_l_flags1 & RF1_RAND_25)
        {
            output.append(" a bit");
        }

        /* Adjective */
        output.append(" erratically");

        /* Hack -- Occasional conjunction */
        if (energy_gain != STANDARD_ENERGY_GAIN) output.append(", and");
    }

    /* Speed */
    if (energy_gain > STANDARD_ENERGY_GAIN)
    {

        if (energy_gain > extract_energy_nppangband[139]) output.append(color_string(" incredibly", TERM_GREEN));
        else if (energy_gain > extract_energy_nppangband[134]) output.append(color_string(" extremely", TERM_GREEN));
        else if (energy_gain > extract_energy_nppangband[129]) output.append(color_string(" very", TERM_GREEN));
        else if (energy_gain > extract_energy_nppangband[124]) output.append(color_string(" exceedingly", TERM_GREEN));
        else if (energy_gain < extract_energy_nppangband[120]) output.append(color_string(" somewhat", TERM_GREEN));
        output.append(color_string(" quickly", TERM_GREEN));

    }
    else if (energy_gain < STANDARD_ENERGY_GAIN)
    {
        if (energy_gain < extract_energy_nppangband[90]) output.append(color_string(" incredibly", TERM_GREEN));
        else if (energy_gain < extract_energy_nppangband[100]) output.append(color_string(" very", TERM_GREEN));
        output.append(color_string(" slowly", TERM_GREEN));
    }
    else
    {
        output.append(color_string(" at normal speed", TERM_GREEN));
    }

    /* The code above includes "attack speed" */
    if (l_ptr->r_l_flags1 & RF1_NEVER_MOVE)
    {
        output.append(", but does not deign to chase intruders");
    }

    output.append(".  ");

    /* Do we know how aware it is? */
    if ((((int)l_ptr->wake * (int)l_ptr->wake) > r_ptr->sleep) ||
        (l_ptr->ignore == MAX_UCHAR) ||
        ((r_ptr->sleep == 0) && (l_ptr->tkills >= 10)))
    {
        QString act;

        if (r_ptr->sleep > 200)
        {
            act = "prefers to ignore";
        }
        else if (r_ptr->sleep > 95)
        {
            act = "pays very little attention to";
        }
        else if (r_ptr->sleep > 75)
        {
            act = "pays little attention to";
        }
        else if (r_ptr->sleep > 45)
        {
            act = "tends to overlook";
        }
        else if (r_ptr->sleep > 25)
        {
            act = "takes quite a while to see";
        }
        else if (r_ptr->sleep > 10)
        {
            act = "takes a while to see";
        }
        else if (r_ptr->sleep > 5)
        {
            act = "is fairly observant of";
        }
        else if (r_ptr->sleep > 3)
        {
            act = "is observant of";
        }
        else if (r_ptr->sleep > 1)
        {
            act = "is very observant of";
        }
        else if (r_ptr->sleep > 0)
        {
            act = "is vigilant for";
        }
        else
        {
            act = "is ever vigilant for";
        }

        output.append(QString("%1 %2 intruders, which %3 may notice from %4 feet.  ")
                    .arg(capitalize_first(wd_he[msex])) .arg(act) .arg(wd_he[msex]) .arg(10 * r_ptr->aaf));
    }

    /* Describe escorts */
    if ((l_ptr->r_l_flags1 & RF1_ESCORT) || (l_ptr->r_l_flags1 & RF1_ESCORTS))
    {
        output.append(QString("%1 usually appears with escorts.  ")
                     .arg(capitalize_first(wd_he[msex])));
    }

    /* Describe friends */
    else if ((l_ptr->r_l_flags1 & RF1_FRIEND) || (l_ptr->r_l_flags1 & RF1_FRIENDS))
    {
        output.append(QString("%1 usually appears in groups.  ")
                     .arg(capitalize_first(wd_he[msex])));
    }

    /*Print out the known native terrains*/
    if (l_ptr->r_l_native)
    {
        int vn = 0;
        int n;
        QString vp[16];

        /* Intro */
        output.append(QString("%1 is native to") .arg(capitalize_first(wd_he[msex])));

        if (l_ptr->r_l_native & (RN1_N_LAVA)) vp[vn++] = "lava";
        if (l_ptr->r_l_native & (RN1_N_ICE)) vp[vn++] = "ice";
        if (l_ptr->r_l_native & (RN1_N_OIL)) vp[vn++] = "oil";
        if (l_ptr->r_l_native & (RN1_N_FIRE)) vp[vn++] = "fire";
        if (l_ptr->r_l_native & (RN1_N_SAND)) vp[vn++] = "sand";
        if (l_ptr->r_l_native & (RN1_N_FOREST)) vp[vn++] = "forests";
        if (l_ptr->r_l_native & (RN1_N_WATER)) vp[vn++] = "water";
        if (l_ptr->r_l_native & (RN1_N_ACID)) vp[vn++] = "acid";
        if (l_ptr->r_l_native & (RN1_N_MUD)) vp[vn++] = "mud";

        /* Scan */
        for (n = 0; n < vn; n++)
        {

            /* Dump */
            output.append(color_string(vp[n], TERM_BLUE));

            if (vn == n + 1) break;
            else if (vn == n + 2) output.append(" and ");
            else output.append(", ");
        }

        output.append(".");
    }

    if (!output.isEmpty()) output.append("<br><br>");

    return (output);
}



/*
 * Learn everything about a monster (by cheating)
 */
static void cheat_monster_lore(int r_idx, monster_lore *l_ptr)
{
    const monster_race *r_ptr = &r_info[r_idx];

    int i;

    /* Hack -- Maximal kills */
    l_ptr->tkills = MAX_SHORT;

    /* Hack -- Maximal info */
    l_ptr->wake = l_ptr->ignore = MAX_UCHAR;

    /* Observe "maximal" attacks */
    for (i = 0; i < MONSTER_BLOW_MAX; i++)
    {
        /* Examine "actual" blows */
        if (r_ptr->blow[i].effect || r_ptr->blow[i].method)
        {
            /* Hack -- maximal observations */
            l_ptr->blows[i] = MAX_UCHAR;
        }
    }

    /* Hack -- maximal drops */
    l_ptr->drop_gold = l_ptr->drop_item =
    (((r_ptr->flags1 & RF1_DROP_4D2) ? 8 : 0) +
     ((r_ptr->flags1 & RF1_DROP_3D2) ? 6 : 0) +
     ((r_ptr->flags1 & RF1_DROP_2D2) ? 4 : 0) +
     ((r_ptr->flags1 & RF1_DROP_1D2) ? 2 : 0) +
     ((r_ptr->flags1 & RF1_DROP_90)  ? 1 : 0) +
     ((r_ptr->flags1 & RF1_DROP_60)  ? 1 : 0));

    /* Hack -- but only "valid" drops */
    if (r_ptr->flags1 & RF1_ONLY_GOLD) l_ptr->drop_item = 0;
    if (r_ptr->flags1 & RF1_ONLY_ITEM) l_ptr->drop_gold = 0;

    /* Hack -- observe many spells */
    l_ptr->ranged = MAX_UCHAR;

    /* Hack -- know all the flags */
    l_ptr->r_l_flags1 = r_ptr->flags1;
    l_ptr->r_l_flags2 = r_ptr->flags2;
    l_ptr->r_l_flags3 = r_ptr->flags3;
    l_ptr->r_l_flags4 = r_ptr->flags4;
    l_ptr->r_l_flags5 = r_ptr->flags5;
    l_ptr->r_l_flags6 = r_ptr->flags6;
    l_ptr->r_l_flags7 = r_ptr->flags7;
    l_ptr->r_l_native = r_ptr->r_native;
}


/*
 */
void describe_monster(int r_idx, bool spoilers, QString extra_message)
{
    monster_lore lore;
    QString output;
    monster_lore save_mem;
    output.clear();

    /* Get the race and lore */
    const monster_race *r_ptr = &r_info[r_idx];
    monster_lore *l_ptr = &l_list[r_idx];

    QString mon_name = monster_desc_race(r_idx);

    /* Cheat -- know everything */
    if ((cheat_know) || (r_ptr->flags2 & (RF2_PLAYER_GHOST)))
    {
        if (cheat_know) p_ptr->is_wizard = TRUE;

        /* Hack -- save memory */
        COPY(&save_mem, l_ptr, monster_lore);
    }

    /* Hack -- create a copy of the monster-memory */
    COPY(&lore, l_ptr, monster_lore);

    /* Assume some "obvious" flags */
    lore.r_l_flags1 |= (r_ptr->flags1 & RF1_OBVIOUS_MASK);

    /* Killing a monster reveals some properties */
    if (lore.tkills)
    {
        /* Know "race" flags */
        lore.r_l_flags3 |= (r_ptr->flags3 & RF3_RACE_MASK);

        /* Know "forced" flags */
        lore.r_l_flags1 |= (r_ptr->flags1 & (RF1_FORCE_DEPTH | RF1_FORCE_MAXHP));
    }

    /* Cheat -- know everything */
    if (cheat_know || spoilers || (r_ptr->flags2 & (RF2_PLAYER_GHOST)))
    {
        if (cheat_know) p_ptr->is_wizard = TRUE;

        cheat_monster_lore(r_idx, &lore);
    }

    QString mon_symbol = color_string(r_ptr->d_char, r_ptr->d_color);

    /* Print, in colour */
    output.append(QString("<b><h1><span style='background-color: black;'>'%1'</span> - %2</h1></b><br><br>") .arg(mon_symbol) .arg(mon_name));

    /* Show kills of monster vs. player(s) */
    if (!spoilers)	output.append(describe_monster_kills(r_idx, &lore));

    /* Monster description */
    output.append(describe_monster_desc(r_idx));

    /* Describe the movement and level of the monster */
    output.append(describe_monster_movement(r_idx, &lore));

    /* Describe experience */
    if (!spoilers) output.append(describe_monster_exp(r_idx, &lore));

    /* Describe spells and innate attacks */
    output.append(describe_monster_spells(r_idx, &lore));

    /* Describe monster "toughness" */
    if (!spoilers) output.append(describe_monster_toughness(r_idx, &lore));

    /* Describe the abilities of the monster */
    output.append(describe_monster_abilities(r_idx, &lore));

    /* Describe the monster drop */
    output.append(describe_monster_drop(r_idx, &lore));

    /* Describe the known attacks */
    output.append(describe_monster_attack(r_idx, &lore));

    /* Notice "Quest" monsters */
    if (lore.r_l_flags1 & RF1_QUESTOR)
    {
        output.append("<big>You feel an intense desire to kill this monster...  </big><br><br>");
    }

    /* Cheat -- know everything */
    if ((cheat_know) || (r_ptr->flags2 & (RF2_PLAYER_GHOST)))
    {
        if (cheat_know) p_ptr->is_wizard = TRUE;

        /* Hack -- restore memory */
        COPY(l_ptr, &save_mem, monster_lore);
    }

    if (!extra_message.isEmpty())
    {
        output.append(QString("%1  <br><br>") .arg(extra_message));
    }

    /* Finally, display it */
    display_info_window(DISPLAY_INFO_MONSTER, r_idx, output);
}



