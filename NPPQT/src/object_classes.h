#ifndef OBJECT_TYPE_CLASS_H
#define OBJECT_TYPE_CLASS_H

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
#include <src/object.h>
#include <QString>


/*
 * Object information, for a specific object.
 *
 * Note that a "discount" on an item is permanent and never goes away.
 *
 * Note that "object" records are "copied" and wiped frequently.
 * It is important to use the methods below instead of C commands like memset.
 *
 * Note that "object flags" must now be derived from the object kind,
 * the artifact and ego-item indexes, and the two "xtra" fields.
 *
 * Each cave grid points to one (or zero) objects via the "o_idx"
 * field (above).  Each object then points to one (or zero) objects
 * via the "next_o_idx" field, forming a singly linked list, which
 * in game terms, represents a "stack" of objects in the same grid.
 *
 * Each monster points to one (or zero) objects via the "hold_o_idx"
 * field (below).  Each object then points to one (or zero) objects
 * via the "next_o_idx" field, forming a singly linked list, which
 * in game terms, represents a pile of objects held by the monster.
 *
 * The "held_m_idx" field is used to indicate which monster, if any,
 * is holding the object.  Objects being held have "ix=0" and "iy=0".
 */
class object_type
{
    public:
    s16b k_idx;			/* Kind index (zero if "dead") */

    byte iy;			/* Y-position on map, or zero */
    byte ix;			/* X-position on map, or zero */

    byte tval;			/* Item type (from kind) */
    byte sval;			/* Item sub-type (from kind) */

    s16b pval;			/* Item extra-parameter */

    byte discount;		/* Discount (if any) */

    byte number;		/* Number of items */

    s16b weight;		/* Item weight */

    byte art_num;		/* Artifact type, if any */
    byte ego_num;		/* Ego-Item type, if any */

    byte xtra1;			/* Extra info type */
    u32b xtra2;			/* Extra info index */

    s16b to_h;			/* Plusses to hit */
    s16b to_d;			/* Plusses to damage */
    s16b to_a;			/* Plusses to AC */

    s16b ac;			/* Normal AC */

    byte dd, ds;		/* Damage dice/sides */

    s16b timeout;		/* Timeout Counter */

    u32b ident;			/* Special flags (was byte) */

    byte marked;		/* Object is marked */
    bool obj_in_use; 	/* Object is in use */

    QString inscription;		/* Inscription  */

    s16b next_o_idx;	/* Next object in stack (if any) */

    s16b held_m_idx;	/* Monster holding us (if any) */

        /* Object history - DRS */

    byte origin_nature;	/* ORIGIN_* */
    s16b origin_dlvl;	/* Depth */
    s16b origin_r_idx;	/* Monster race */
    QString origin_m_name;	/* monster name. Used only for player ghosts */

    s16b mimic_r_idx;	/* Object is a mimic */

    // Methods - all in object_classs.cpp
    void object_wipe();
    void object_copy (object_type *j_ptr);
    bool has_hidden_powers();
    bool is_wearable();
    bool is_easy_know();
    bool is_known();
    bool is_flavor_known();
    bool can_be_pseudo_ided();
};

#endif // OBJECT_TYPE_CLASS_H
