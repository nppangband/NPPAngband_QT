/* File: player_classes.cpp */

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

#include "src/npp.h"
#include "src/player_command.h"
#include "src/utilities.h"

player_class::player_class()
{
    player_class_wipe();
}

void player_class::player_class_wipe()
{
    cl_name.clear();
    for (int i = 0; i < PY_MAX_LEVEL; i++) {
        cl_title[i].clear();
    }
    C_WIPE(c_adj, A_MAX, s16b);
    c_dis = c_dev = c_sav = c_stl = c_srh = c_fos = c_thn = c_thb = 0;
    x_dis = x_dev = x_sav = x_stl = x_srh = x_fos = x_thn = x_thb = 0;
    c_mhp = c_exp = flags = c_native = max_attacks = min_weight = att_multiply = 0;
    spell_book = spell_first = spell_weight = sense_base = sense_div = 0;
    for (int i = 0; i < MAX_START_ITEMS; i++) {
        start_items[i].max = start_items[i].min = start_items[i].sval = start_items[i].tval = 0;
    }
    for (int i = 0; i < PY_MAX_SPELLS; i++) {
        magic_type *m = spells.info + i;
        m->sexp = m->sfail = m->slevel = m->smana = 0;
    }
}



/*
 * Wipe the player state class.
 * This function shoudld be used instead of WIPE command.
 * All variables in player state should be re-set in this function.
 */
void player_state::player_state_wipe()
{
    int i;
    p_speed = 0;
    num_blow = num_fire = 0;
    ammo_mult = ammo_tval = 0;
    for (i = 0; i < A_MAX; i++)
    {
        stat_add[i] = 0;
        stat_ind[i] = 0;
        stat_use[i] = 0;
        stat_top[i] = 0;
    }
    dis_ac = ac = dis_to_a = to_a = to_h = dis_to_h = to_d = dis_to_d = 0;
    see_infra = 0;
    for (i = 0; i < SKILL_MAX; i++) skills[i] = 0;
    noise = cur_light = 0;
    sustain_str = sustain_int = sustain_wis = sustain_dex = sustain_con = sustain_chr = FALSE;
    immune_acid = immune_elec = immune_fire = immune_cold = immune_pois = FALSE;
    resist_acid = resist_elec = resist_fire = resist_cold = resist_pois = FALSE;
    resist_fear = resist_light = resist_dark = resist_blind = resist_confu = resist_sound = FALSE;
    resist_shard = resist_nexus = resist_nethr = resist_chaos = resist_disen = FALSE;
    slow_digest = ffall = regenerate = telepathy = see_inv = free_act = hold_life = afraid = light = FALSE;
    impact = aggravate = teleport = exp_drain = bless_blade = cursed_quiver = FALSE;
    cumber_armor = cumber_glove = heavy_wield = heavy_shoot = icky_wield = FALSE;
}



/*
 * Wipe the player other class.
 * This function shoudld be used instead of WIPE command.
 * All variables in player other should be re-set in this function.
 */
void player_other::player_other_wipe()
{            
    full_name.clear();
    base_name.clear();
    for (int i = 0; i < OPT_MAX; i++) op_ptr->opt[i] = options[i].normal;
    for (int i = 0; i < ANGBAND_TERM_MAX; i++) op_ptr->window_flag[i] = 0L;
    hitpoint_warn = delay_factor = 0;
}

void player_type::player_command_wipe()
{
    command_current = CMD_NONE;
    player_args.wipe();
}

void player_type::player_previous_command_wipe()
{
    command_previous = CMD_NONE;
    command_previous_args.wipe();
}

void player_type::player_previous_command_update(s16b command, cmd_arg args)
{
    player_previous_command_wipe();
    command_previous = command;
    command_previous_args = args;
}


bool player_type::is_running()
{
    if (p_ptr->command_current == CMD_RUNNING) return (TRUE);
    return (FALSE);
}

bool player_type::is_resting()
{
    if (p_ptr->command_current == CMD_RESTING) return (TRUE);
    return (FALSE);
}

/* Determine if the player should stop resting
 */
bool player_type::should_stop_resting()
{
    if (!is_resting()) return(FALSE);

    if (player_args.choice == REST_BOTH_SP_HP)
    {
        if (chp != mhp) return (FALSE);
        if (csp != msp) return (FALSE);
        return (TRUE);
    }

    if (player_args.choice == REST_HP)
    {
        if (chp != mhp) return (FALSE);
        return (TRUE);
    }
    if (player_args.choice == REST_SP)
    {
        if (csp != msp) return (FALSE);
        return (TRUE);
    }

    if ((player_args.choice == REST_COMPLETE) ||
        (player_args.choice == REST_TURNCOUNT))
    {
        if (chp != mhp) return (FALSE);
        if (csp != msp) return (FALSE);
        if (timed[TMD_BLIND])  return (FALSE);
        if (timed[TMD_CONFUSED])  return (FALSE);
        if (timed[TMD_AFRAID])  return (FALSE);
        if (timed[TMD_STUN])  return (FALSE);
        if (timed[TMD_SLOW])  return (FALSE);
        if (timed[TMD_PARALYZED])  return (FALSE);
        if (timed[TMD_IMAGE])  return (FALSE);
        if (word_recall)  return (FALSE);
        if (food < PY_FOOD_UPPER)  return (FALSE);

        if (player_args.choice == REST_TURNCOUNT)
        {
            if (player_args.repeats) return (FALSE);
        }
        return (TRUE);
    }

    // Oops!  Code shouldn't get this far.
    return TRUE;
}

/* Start appending messages
 */
void player_type::message_append_start()
{
    message_append = TRUE;
    message_first_append = TRUE;
}

/* Stop appending messages
 */
void player_type::message_append_stop()
{
    message_append = FALSE;
    message_first_append = FALSE;
    if (!message_list.empty()) message_list[0].append = FALSE;
}

void player_type::player_type_wipe()
{
    int i;
    py = px = 0;
    flow_center_y = flow_center_x = 0;
    update_center_y = update_center_x = 0;
    psex = prace = pclass = 0;
    tile_32x32_y = tile_32x32_x = tile_8x8_y = tile_8x8_x = 0;
    hitdie = expfact = 0;
    age = ht = wt = sc = au = 0;
    q_fame = deferred_rewards = quest_depth = max_depth = depth = recall_depth = 0;
    max_lev = lev = max_exp = exp = exp_frac = 0;
    mhp = chp = chp_frac = msp = csp = csp_frac = 0;
    for (i = 0; i < A_MAX; i++) stat_max[i] = stat_cur[i] = stat_quest_add[i] = 0;
    for (i = 0; i < TMD_MAX; i++) timed[i] = 0;
    word_recall = p_energy = food = 0;
    confusing = searching = 0;
    base_wakeup_chance = 0;
    for (i = 0; i < PY_MAX_SPELLS; i++)     spell_flags[i] = spell_order[i] = 0;
    for (i = 0; i < PY_MAX_LEVEL; i++)      player_hp[i] = 0;
    died_from.clear();
    history.clear();
    total_winner = panic_save = 0;
    is_dead = player_turn = is_wizard = FALSE;
    playing =  in_store = in_menu = message_append = message_first_append = leaving_level = autosave = FALSE;
    create_stair = cur_map_hgt = cur_map_wid = FALSE;
    total_weight = 0;
    inven_cnt = equip_cnt = pack_size_reduce = quiver_remainder = quiver_slots = 0;
    target_set = target_who = target_row = target_col = health_who = monster_race_idx = 0;
    object_idx = object_kind_idx = feature_kind_idx = 0;
    running_withpathfind = FALSE;
    run_cur_dir = run_old_dir = 0;
    run_unused = run_open_area = run_break_right = run_break_left = FALSE;
    player_command_wipe();
    player_previous_command_wipe();
    new_spells = 0;
    notice = update = redraw = window = 0;
    p_native = p_native_known = 0;
    au_birth = 0;
    for (i = 0; i < A_MAX; i++) stat_birth[i] = 0;
    ht_birth = wt_birth = sc_birth = 0;

    state.player_state_wipe();

    vulnerability = next_quest = cumulative_terrain_damage = 0;
    p_turn = 0;
    dungeon_type = 0;

    tile_id.clear();
}

/*
 * Is the player capable of casting a spell?
 */
bool player_type::can_cast(void)
{
    if (!cp_ptr->spell_book)
    {
        message(QString("You cannot cast spells!"));
        return (FALSE);
    }

    if (p_ptr->timed[TMD_BLIND] || no_light())
    {
        message(QString("You cannot see!"));
        return (FALSE);
    }

    if (p_ptr->timed[TMD_CONFUSED])
    {
        message(QString("You are too confused!"));
        return (FALSE);
    }

    return (TRUE);
}

/*
 * Is the player capable of studying?
 */
bool player_type::can_study(void)
{
    if (!can_cast()) return (FALSE);

    if (!new_spells)
    {
        QString p = cast_spell(MODE_SPELL_NOUN, cp_ptr->spell_book, 1, 0);
        message(QString("You cannot learn any new %1s!") .arg(p));
        return (FALSE);
    }

    return (TRUE);
}

/*
 * Is the player capable of studying?
 */
bool player_type::chooses_spells(void)
{
    if (!cp_ptr->spell_book) return (FALSE);

    if (cp_ptr->flags & (CF_CHOOSE_SPELLS)) return TRUE;

    return (FALSE);
}

