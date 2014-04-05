/* File: wizard1.cpp */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 * 						Jeff Greene, Diego Gonzalez
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
 * Hack -- Create a "forged" artifact
 */
bool make_fake_artifact(object_type *o_ptr, byte art_num)
{
    int i;

    artifact_type *a_ptr = &a_info[art_num];

    /* Ignore "empty" artifacts */
    if (a_ptr->tval + a_ptr->sval == 0) return FALSE;

    /* Get the "kind" index */
    i = lookup_kind(a_ptr->tval, a_ptr->sval);

    /* Oops */
    if (!i) return (FALSE);

    /* Create the artifact */
    object_prep(o_ptr, i);

    /* Save the name */
    o_ptr->art_num = art_num;

    /* Extract the fields */
    o_ptr->pval = a_ptr->pval;
    o_ptr->ac = a_ptr->ac;
    o_ptr->dd = a_ptr->dd;
    o_ptr->ds = a_ptr->ds;
    o_ptr->to_a = a_ptr->to_a;
    o_ptr->to_h = a_ptr->to_h;
    o_ptr->to_d = a_ptr->to_d;
    o_ptr->weight = a_ptr->weight;

    /*identify it*/
    object_known(o_ptr);

    /*make it a store item*/
    o_ptr->ident |= IDENT_STORE;

    /* Hack -- extract the "cursed" flag */
    if (a_ptr->a_flags3 & (TR3_LIGHT_CURSE)) o_ptr->ident |= (IDENT_CURSED);

    /* Success */
    return (TRUE);
}
