
/*
 * Copyright (c) 2014 Jeff Greene, Diego Gonzalez
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

#include <src/player_screen.h>
#include <src/help.h>
#include <src/utilities.h>
#include <QDialogButtonBox>
#include <QPixmap>
#include <QPainter>
#include <QPlainTextEdit>
#include <QList>
#include <QPushButton>
#include <QScrollArea>

// The null line is there to prevent crashes as the data is read;
static struct player_flag_record player_resist_table[] =
{
    { "Resist Acid",        2, TR2_RES_ACID,	TR2_IM_ACID, TRUE, FALSE},
    { "Resist Lightning",   2, TR2_RES_ELEC,	TR2_IM_ELEC, TRUE, FALSE},
    { "Resist Fire",        2, TR2_RES_FIRE,	TR2_IM_FIRE, TRUE, FALSE},
    { "Resist Cold",        2, TR2_RES_COLD,	TR2_IM_COLD, TRUE, FALSE},
    { "Resist Poison",      2, TR2_RES_POIS,	TR2_IM_POIS, FALSE, FALSE},
    { "Resist Blind",       2, TR2_RES_BLIND,	0, TRUE, FALSE},
    { "Resist Confusion",	2, TR2_RES_CONFU,	0, FALSE, FALSE},
    { "Resist Nexus",       2, TR2_RES_NEXUS,	0, FALSE, FALSE},
    { "Resist Nether",      2, TR2_RES_NETHR,	0, FALSE, FALSE},
    { "Resist Chaos",       2, TR2_RES_CHAOS,	0, FALSE, FALSE},
    { "R. Disenchantment",	2, TR2_RES_DISEN,	0, FALSE, FALSE},
    { "Resist Sound",       2, TR2_RES_SOUND,	0, FALSE, FALSE},
    { "Resist Shards",      2, TR2_RES_SHARD,	0, FALSE, FALSE},
    { "Resist Light",       2, TR2_RES_LIGHT,	0, FALSE, FALSE},
    { "Resist Darkness",    2, TR2_RES_DARK,	0, FALSE, FALSE},
    { "Resist Fear",        2, TR2_RES_FEAR,	0, FALSE, FALSE},
    { NULL,                 0, 0,               0, FALSE, FALSE},
};

// The null line is there to prevent crashes as the data is read;
static struct player_flag_record player_abilities_table[] =
{
    { "See Invisible",      3, TR3_SEE_INVIS, 	0, TRUE, FALSE},
    { "Free Action",        3, TR3_FREE_ACT, 	0, TRUE, FALSE},
    { "Telepathy",          3, TR3_TELEPATHY, 	0, TRUE, FALSE},
    { "Hold Life",          3, TR3_HOLD_LIFE, 	0, TRUE, FALSE},
    { "Permanent Light",    3, TR3_LIGHT, 		0, TRUE, FALSE},
    { "Regeneration",       3, TR3_REGEN, 		0, TRUE, FALSE},
    { "Slow Digestion",     3, TR3_SLOW_DIGEST,	0, TRUE, FALSE},
    { "Feather Fall",       3, TR3_FEATHER, 	0, TRUE, FALSE},
    { "Teleportation",      3, TR3_TELEPORT, 	0, TRUE, TRUE},
    { "Aggravate",          3, TR3_AGGRAVATE,	0, TRUE, TRUE},
    { "Cursed",             3, TR3_CURSE_ALL,   0, TRUE, TRUE},
    { "Drain Exp.",         3, TR3_DRAIN_EXP,	0, FALSE, TRUE},
    { NULL,                 0, 0,               0, FALSE, FALSE},
};

// The null line is there to prevent crashes as the data is read;
//The stats need to come first and in order for the stat tooltips to work properly
static struct player_flag_record player_pval_table[] =
{
    { "Strength",           1, TR1_STR,         TR2_SUST_STR, TRUE, FALSE},
    { "Intelligence",       1, TR1_INT,         TR2_SUST_INT, TRUE, FALSE},
    { "Wisdom",             1, TR1_WIS,         TR2_SUST_WIS, TRUE, FALSE},
    { "Dexterity",          1, TR1_DEX,         TR2_SUST_DEX, TRUE, FALSE},
    { "Constitution",       1, TR1_CON,         TR2_SUST_CON, TRUE, FALSE},
    { "Charisma",           1, TR1_CHR,         TR2_SUST_CHR, TRUE, FALSE},
    { "Infravision",        1, TR1_INFRA,		0, TRUE, FALSE},
    { "Stealth",            1, TR1_STEALTH,		0, TRUE, FALSE},
    { "Searching",          1, TR1_SEARCH,		0, TRUE, FALSE},
    { "Speed",              1, TR1_SPEED,		0, TRUE, FALSE},
    { "Tunneling",          1, TR1_TUNNEL,      0, TRUE, FALSE},
    { "Extra attacks",      1, TR1_BLOWS,		0, TRUE, FALSE},
    { "Extra Shots",        1, TR1_SHOTS,		0, TRUE, FALSE},
    { "Shooting Power",     1, TR1_MIGHT,		0, TRUE, FALSE},
    { NULL,                 0, 0,               0, FALSE, FALSE},
};

// The null line is there to prevent crashes as the data is read;
static struct player_flag_record player_nativity_table[]=
{
    { "Lava",   4,  TN1_NATIVE_LAVA,    0, FALSE, FALSE},
    { "Ice",    4,  TN1_NATIVE_ICE,     0, FALSE, FALSE},
    { "Oil",    4,  TN1_NATIVE_OIL,		0, FALSE, FALSE},
    { "Fire",   4,  TN1_NATIVE_FIRE,    0, FALSE, FALSE},
    { "Sand",   4,  TN1_NATIVE_SAND,    0, FALSE, FALSE},
    { "Forest", 4,  TN1_NATIVE_FOREST,  0, FALSE, FALSE},
    { "Water",  4,  TN1_NATIVE_WATER,  0, FALSE, FALSE},
    { "Acid",   4,  TN1_NATIVE_ACID,    0, FALSE, FALSE},
    { "Mud",    4,  TN1_NATIVE_MUD,		0, FALSE, FALSE},
    { "Boiling Water",    4,  TN1_NATIVE_BWATER,		0, FALSE, FALSE},
    { "Boiling Mud",    4,  TN1_NATIVE_BMUD,		0, FALSE, FALSE},
    { NULL,       0,    0,              0, FALSE, FALSE},
};

QString moria_speed_labels(int speed)
{

    // Boundry Control
    if (speed < NPPMORIA_LOWEST_SPEED) speed = NPPMORIA_LOWEST_SPEED;
    else if (speed > NPPMORIA_MAX_SPEED) speed = NPPMORIA_MAX_SPEED;

    switch (speed)
    {
        case NPPMORIA_LOWEST_SPEED:
        {
            return("Very slow");
        }
        case (NPPMORIA_LOWEST_SPEED + 1):
        {
            return("Slow");
        }
        case NPPMORIA_MAX_SPEED:
        {
            return("Max speed");
        }
        case (NPPMORIA_MAX_SPEED - 1):
        {
            return("Very fast");
        }
        case (NPPMORIA_MAX_SPEED - 2):
        {
            return("Fast");
        }
        default: return("Normal");
    }
}

/*
 * Hack - Modify the color based on speed bonuses. -DG-
 */
byte analyze_speed_bonuses(int speed, byte default_attr)
{
    if (game_mode == GAME_NPPMORIA)
    {
        if (speed > NPPMORIA_NORMAL_SPEED)	return (TERM_GREEN);
        if (speed < NPPMORIA_NORMAL_SPEED)	return (TERM_UMBER);
        return (default_attr);
    }

    // GAME_NPPANGBAND
    if (speed > 110) return (TERM_GREEN);
    if (speed < 110) return (TERM_UMBER);
    return (default_attr);
}

// set up a standard label
void make_standard_label(QLabel *this_label, QString title, byte preset_color)
{
    this_label->clear();
    this_label->setText(QString("<b>%1</b>") .arg(color_string(title, preset_color)));
    this_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
}

static void make_ability_graph(QLabel *this_label, int min, int max, int value)
{
    QFont this_font = ui_message_window_font();
    this_font.setPointSize(12);

    QFontMetrics metrics(this_font);
    QSize this_size = metrics.size(Qt::TextSingleLine, "MMMMMMMMM");
    this_size.setHeight(this_size.height() *2/ 3);
    QPixmap this_img(this_size);
    QPainter paint(&this_img);
    QPen pen;

    this_img.fill(Qt::lightGray);

    // avoid crashes and set values
    if (max <= min) return;
    if (value < 0) return;
    int this_max = max - min;
    int this_value = value - min;
    if (this_value > this_max) this_value = this_max;

    int this_percent = this_value * 100 / this_max;

    //Draw an outside border
    paint.setPen(pen);
    pen.setWidthF(this_img.height() / 5);
    pen.setColor(Qt::black);

    QRect filler(0,0,this_size.width(), this_size.height());
    paint.drawRect(filler);

    // Fill based on % between min and max
    QColor this_color = defined_colors[TERM_RED];
    if (this_percent >= 100) this_color = defined_colors[TERM_GREEN];
    else if (this_percent >= 90) this_color = defined_colors[TERM_L_GREEN];
    else if (this_percent >= 75) this_color = defined_colors[TERM_BLUE];
    else if (this_percent >= 50) this_color = defined_colors[TERM_SKY_BLUE];
    else if (this_percent >= 25) this_color = defined_colors[TERM_ORANGE];
    else if (this_percent > 10) this_color = defined_colors[TERM_YELLOW];

    // Draw progress towards 100%
    filler.setWidth(filler.width() * this_percent / 100);
    paint.fillRect(filler, this_color);

    //draw a box around the progress bar.
    paint.drawRect(filler);

    this_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    this_label->setPixmap(this_img);
}

// Draw the equipment labels
static void draw_equip_labels(QGridLayout *return_layout, int row, int col, bool do_player, bool do_temp)
{
    // Leave one column for the labels.
    col ++;

    for (int i = INVEN_WIELD; i < INVEN_TOTAL; i++, col++)
    {
        object_type *o_ptr = &inventory[i];

        if (!o_ptr->k_idx) continue;

        object_kind *k_ptr = &k_info[o_ptr->k_idx];

        // Set up a tooltip and pixture and add it to the layout.
        QLabel *obj_label = new QLabel;
        make_standard_label(obj_label, k_ptr->d_char, k_ptr->color_num);
        QString obj_text = QString("%1: ") .arg(mention_use(i));
        obj_text.append(object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL));
        obj_label->setToolTip(obj_text);
        return_layout->addWidget(obj_label, row, col, Qt::AlignCenter);
    }

    col++;

    if (do_player)
    {
        QLabel *person_label = new QLabel;
        make_standard_label(person_label, "@", TERM_DARK);
        person_label->setToolTip("Innate character traits.");
        return_layout->addWidget(person_label, row, col++);
    }

    if (do_temp)
    {
        QLabel *temp_label = new QLabel(" ");
        temp_label->setToolTip("Temporary resistances.");
        return_layout->addWidget(temp_label, row, col++);
    }

    QLabel *filler = new QLabel("  ");
    return_layout->addWidget(filler, 0, col);
}

QString stat_entry(int stat)
{

    // Paranoia
    if (stat >= A_MAX) return ("invalid stat");
    if (stat < A_STR) return ("invalid stat");

    return(get_help_topic("character_info", stat_names_full[stat]));
}

void PlayerScreenInfo::name_change(void)
{
    QString new_name = get_string("Please enter a new name for your character.", "Enter new name", op_ptr->full_name);

    if (!new_name.length()) return;

    op_ptr->full_name = new_name;

    // Update the new name
    QList<QLabel *> lbl_list = this->findChildren<QLabel *>();
    for (int i = 0; i < lbl_list.size(); i++)
    {
        QLabel *this_lbl = lbl_list.at(i);

        QString this_name = this_lbl->objectName();

        if (this_name.operator ==("PLYR_Name"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(op_ptr->full_name), TERM_BLUE));
            return;
        }
    }
}

// Go through all the labels and update the ones that show data.
void PlayerScreenInfo::update_char_screen(void)
{
    QList<QLabel *> lbl_list = this->findChildren<QLabel *>();
    for (int i = 0; i < lbl_list.size(); i++)
    {
        QLabel *this_lbl = lbl_list.at(i);

        QString this_name = this_lbl->objectName();

        QString first_num_string;
        QString second_num_string;
        s32b first_num;
        s32b second_num;

        //Not a named label
        if (!this_name.length()) continue;

        // Update all of the stats
        for (int i = 0; i < A_MAX; i++)
        {
            if (this_name.contains(QString("st_base_%1") .arg(i)))
            {
                QString base_num = (QString("<b>%1 </b>") .arg(p_ptr->stat_base_max[i]));
                base_num = color_string(base_num, TERM_BLUE);
                this_lbl->setText(base_num);

                continue;
            }

            if (this_name.contains(QString("st_race_%1") .arg(i)))
            {
                QString race_num = format_stat(rp_ptr->r_adj[i]);
                race_num.append(" ");
                race_num = color_string(race_num, (rp_ptr->r_adj[i] < 0 ? TERM_RED : TERM_BLUE));
                this_lbl->setText(race_num);
                continue;
            }
            if (this_name.contains(QString("st_class_%1") .arg(i)))
            {
                QString class_num = format_stat(cp_ptr->c_adj[i]);
                class_num.append(" ");
                class_num = color_string(class_num, (cp_ptr->c_adj[i] < 0 ? TERM_RED : TERM_BLUE));
                this_lbl->setText(class_num);
                continue;
            }
            if (this_name.contains(QString("st_equip_%1") .arg(i)))
            {
                int equip = p_ptr->state.stat_equip[i];

                QString equip_num = format_stat(equip);
                equip_num.append(" ");
                equip_num = color_string(equip_num, (equip < 0 ? TERM_RED : TERM_BLUE));
                this_lbl->setText(equip_num);
                continue;
            }
            if (this_name.contains(QString("st_quest_%1") .arg(i)))
            {
                QString quest_num = format_stat(p_ptr->stat_quest_add[i]);
                quest_num.append(" ");
                quest_num = color_string(quest_num, TERM_BLUE);
                this_lbl->setText(quest_num);
                continue;
            }
            if (this_name.contains(QString("st_total_%1") .arg(i)))
            {
                QString quest_num = format_stat(p_ptr->stat_quest_add[i]);
                quest_num.append(" ");
                quest_num = color_string(quest_num, TERM_BLUE);
                this_lbl->setText(quest_num);
                continue;
            }
            if (this_name.contains(QString("st_reduce_%1") .arg(i)))
            {
                bool need_display = FALSE;
                if (p_ptr->state.stat_loaded_cur[i] < p_ptr->state.stat_loaded_max[i]) need_display = TRUE;
                QString lower_stat = cnv_stat(p_ptr->state.stat_loaded_cur[i]);
                if (!need_display) lower_stat = "       ";
                lower_stat = color_string(lower_stat, TERM_RED);
                this_lbl->setText(lower_stat);
                continue;
            }
        }

        if (this_name.operator ==("PLYR_Name"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(op_ptr->full_name), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("PLYR_Sex"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(sp_ptr->title), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("PLYR_Race"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(p_info[p_ptr->prace].pr_name), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("PLYR_Class"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(c_info[p_ptr->pclass].cl_name), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("PLYR_Title"))
        {
            QString title = get_player_title();
            if (p_ptr->is_wizard) title = "[=-WIZARD-=]";
            else if (p_ptr->total_winner)  title = "**WINNER**";
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(title), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("PLYR_HP"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(QString("%1/%2") .arg(p_ptr->chp) .arg(p_ptr->mhp)), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("PLYR_SP"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(QString("%1/%2") .arg(p_ptr->csp) .arg(p_ptr->msp)), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("PLYR_Fame"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(p_ptr->q_fame), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("PLYR_Gold"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(number_to_formatted_string(p_ptr->au)), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("PLYR_Age"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(p_ptr->age), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("PLYR_Height"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(p_ptr->ht), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("PLYR_Weight"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(p_ptr->wt), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("PLYR_SC"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(p_ptr->sc), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("TURN_Game"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(number_to_formatted_string(p_ptr->game_turn)), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("TURN_Player"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(number_to_formatted_string(p_ptr->p_turn)), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("DEPTH_Cur"))
        {
            QString cur_depth = (QString("%1") .arg(p_ptr->depth * 50));
            if (!p_ptr->max_depth) cur_depth = "TOWN";
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(cur_depth), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("DEPTH_Max"))
        {
            QString max_depth = (QString("%1") .arg(p_ptr->max_depth * 50));
            if (!p_ptr->max_depth) max_depth = "TOWN";
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(max_depth), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("PLYR_Infra"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg((QString("%1 feet") .arg(p_ptr->state.see_infra * 10))), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("PLYR_Speed"))
        {
            make_ability_graph(this_lbl, 0, pam_ptr->max_p_speed, calc_energy_gain(p_ptr->state.p_speed));
            continue;
        }
        if (this_name.operator ==("PLYR_Save"))
        {
            make_ability_graph(this_lbl, 0, 100, p_ptr->state.skills[SKILL_SAVE]);
            continue;
        }
        if (this_name.operator ==("PLYR_Stealth"))
        {
            int stealth = (WAKEUP_MAX - p_ptr->base_wakeup_chance);
            if (p_ptr->state.aggravate) stealth = WAKEUP_MAX;
            make_ability_graph(this_lbl, WAKEUP_MIN, WAKEUP_MAX, stealth);
            continue;
        }
        if (this_name.operator ==("PLYR_Stealth"))
        {
            int stealth = (WAKEUP_MAX - p_ptr->base_wakeup_chance);
            if (p_ptr->state.aggravate) stealth = WAKEUP_MAX;
            make_ability_graph(this_lbl, WAKEUP_MIN, WAKEUP_MAX, stealth);
            continue;
        }
        if (this_name.operator ==("PLYR_Fight"))
        {
            make_ability_graph(this_lbl, 0, pam_ptr->max_skills[SKILL_TO_HIT_MELEE], p_ptr->state.skills[SKILL_TO_HIT_MELEE]);
            continue;
        }
        if (this_name.operator ==("PLYR_Bow"))
        {
            make_ability_graph(this_lbl, 0, pam_ptr->max_skills[SKILL_TO_HIT_BOW], p_ptr->state.skills[SKILL_TO_HIT_BOW]);
        }
        if (this_name.operator ==("PLYR_Throw"))
        {
            make_ability_graph(this_lbl, 0, pam_ptr->max_skills[SKILL_TO_HIT_THROW], p_ptr->state.skills[SKILL_TO_HIT_THROW]);
        }
        if (this_name.operator ==("PLYR_Disarm"))
        {
            make_ability_graph(this_lbl, 0, pam_ptr->max_skills[SKILL_DISARM], p_ptr->state.skills[SKILL_DISARM]);
        }
        if (this_name.operator ==("PLYR_Magic"))
        {
            make_ability_graph(this_lbl, 0, pam_ptr->max_skills[SKILL_DEVICE], p_ptr->state.skills[SKILL_DEVICE]);
        }
        if (this_name.operator ==("PLYR_Level"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(p_ptr->lev), (p_ptr->lev >= p_ptr->max_lev) ? TERM_BLUE : TERM_RED));
            continue;
        }
        if (this_name.operator ==("PLYR_Cur_Exp"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(number_to_formatted_string(p_ptr->exp)), (p_ptr->exp >= p_ptr->max_exp) ? TERM_BLUE : TERM_RED));
            continue;
        }
        if (this_name.operator ==("PLYR_Max_Exp"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(number_to_formatted_string(p_ptr->max_exp)), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("PLYR_Advance"))
        {
            if (p_ptr->lev == z_info->max_level)
            {
                this_lbl->setText(color_string("---------", TERM_BLUE));
                continue;
            }

            s32b advance = (get_experience_by_level(p_ptr->lev-1) * p_ptr->expfact / 100L);
            s32b exp_needed = advance - p_ptr->exp;
            QString exp_output = number_to_formatted_string(exp_needed);
            QString exp_advance = (QString("The total experience needed for the next level is %1") .arg(number_to_formatted_string(advance)));
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(exp_output), TERM_BLUE));
            this_lbl->setToolTip(exp_advance);
            continue;
        }
        if (this_name.operator ==("PLYR_Score"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(number_to_formatted_string(p_ptr->current_score)), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("BURDEN_Cur"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(formatted_weight_string(p_ptr->total_weight)), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("BURDEN_Max"))
        {
            this_lbl->setText(color_string(QString("<b>%1</b>") .arg(formatted_weight_string(normal_speed_weight_limit())), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("BURDEN_Percent"))
        {
            int pct = (p_ptr->total_weight * 100) / normal_speed_weight_limit();
            this_lbl->setText(color_string(QString("<b>%1%</b>") .arg(pct), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("BASE_Speed"))
        {

            // Undo temporary effects
            if (p_ptr->searching) first_num += (game_mode == GAME_NPPMORIA ? 1 : 10);
            if (p_ptr->timed[TMD_FAST]) first_num -= (game_mode == GAME_NPPMORIA ? 1 : 10);
            if (p_ptr->timed[TMD_SLOW]) first_num += (game_mode == GAME_NPPMORIA ? 1 : 10);
            // Speed is different in the different games
            if (game_mode == GAME_NPPMORIA)
            {
                first_num_string = moria_speed_labels(first_num);
                second_num = analyze_speed_bonuses(first_num, TERM_BLUE);

            }
            else
            {
                second_num = analyze_speed_bonuses(first_num, TERM_BLUE);
                if (first_num > 110)
                {
                    first_num_string = (QString("Fast (%1)") .arg(first_num - 110));
                    first_num_string.prepend("+");
                }
                else if (first_num < 110)
                {
                    first_num_string = (QString("Slow (%1)") .arg(110 - first_num));
                }
                else first_num_string = "Normal";
            }
            this_lbl->setText(color_string(first_num_string, second_num));
            continue;
        }
        if (this_name.operator ==("PLYR_AC"))
        {
            first_num = p_ptr->state.dis_ac;
            second_num = p_ptr->state.dis_to_a;
            first_num_string.setNum(first_num);
            second_num_string.setNum(second_num);
            if (second_num < 0) second_num_string.prepend("-");
            else second_num_string.prepend("+");
            this_lbl->setText(color_string((QString("[%1, %2]") .arg(first_num_string) .arg(second_num_string)), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("PLYR_MELEE"))
        {
            first_num = p_ptr->state.dis_to_h;
            second_num = p_ptr->state.dis_to_d;

            object_type *o_ptr = &inventory[INVEN_WIELD];
            QString dd; ;
            QString ds;
            dd.setNum(o_ptr->dd);
            ds.setNum(o_ptr->ds);
            // Player is punching monsters
            if (!o_ptr->tval)
            {
                dd.setNum(1);
                ds.setNum(1);
            }
            else if (object_known_p(o_ptr))
            {
                first_num += o_ptr->to_h;
                second_num += o_ptr->to_d;
            }
            first_num_string.setNum(first_num);
            second_num_string.setNum(second_num);
            if (first_num < 0) first_num_string.prepend("-");
            else first_num_string.prepend("+");
            if (second_num < 0) second_num_string.prepend("-");
            else second_num_string.prepend("+");
            this_lbl->setText(color_string((QString("x(%1) [%2d%3] (%4, %5)") .arg(p_ptr->state.num_blow) .arg(dd) .arg(ds) .arg(first_num_string) .arg(second_num_string)), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("HIT_Critical"))
        {
            object_type *o_ptr = &inventory[INVEN_WIELD];
            first_num = critical_hit_chance(o_ptr, p_ptr->state, TRUE) /  (CRIT_HIT_CHANCE / 100);
            first_num_string.setNum(first_num);
            this_lbl->setText(color_string((QString("%1 %") .arg(first_num_string)), TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("SHOOTING_Stats"))
        {
            object_type *o_ptr;
            object_type object_type_body;
            object_type_body.object_wipe();
            //Make sure we are factoring in the bow and not a swap weapon
            if (birth_swap_weapons)
            {
                if (inventory[INVEN_MAIN_WEAPON].tval == TV_BOW) o_ptr = &inventory[INVEN_MAIN_WEAPON];

                /* A bow is not wielded, just set up a "dummy, blank" object and point to that */
                else o_ptr = &object_type_body;
            }
            else o_ptr = &inventory[INVEN_BOW];

            first_num = p_ptr->state.dis_to_h;
            second_num = 0;
            s32b mult = p_ptr->state.ammo_mult;
            if (object_known_p(o_ptr))
            {
                first_num += o_ptr->to_h;
                second_num += o_ptr->to_d;
            }

            // Factor in Rogue and brigand combat bonuses, if applicable
            mult += rogue_shot(o_ptr, &first_num, p_ptr->state);
            mult += brigand_shot(o_ptr, 0L, FALSE, p_ptr->state);

            first_num_string.setNum(first_num);
            second_num_string.setNum(second_num);
            if (first_num < 0) first_num_string.prepend("-");
            else first_num_string.prepend("+");
            if (second_num < 0) second_num_string.prepend("-");
            else second_num_string.prepend("+");
            QString output = (QString("xS(%1) xM(%2) (%3, %4)") .arg(p_ptr->state.num_fire) .arg(mult) .arg(first_num_string) .arg(second_num_string));
            //No bow
            if (!mult) output = "---------";
            this_lbl->setText(color_string(output, TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("SEARCH_Freq"))
        {
            if (p_ptr->state.skills[SKILL_SEARCH_CHANCE] > 100) first_num = 100;
            else first_num = p_ptr->state.skills[SKILL_SEARCH_CHANCE];
            first_num_string = (QString("%1%") .arg(first_num));
            this_lbl->setText(color_string(first_num_string, TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("SEARCH_Chance"))
        {
            if (p_ptr->state.skills[SKILL_SEARCH_CHANCE] > 100) first_num = 100;
            else first_num = p_ptr->state.skills[SKILL_SEARCH_CHANCE];
            first_num_string = (QString("%1%") .arg(first_num));
            this_lbl->setText(color_string(first_num_string, TERM_BLUE));
            continue;
        }
        if (this_name.operator ==("PLYR_Tunnel"))
        {
            first_num_string.setNum(p_ptr->state.skills[SKILL_DIGGING]);
            this_lbl->setText(color_string(first_num_string, TERM_BLUE));
            continue;
        }
    }
}


void PlayerScreenInfo::char_basic_info(QGridLayout *return_layout)
{
    int row = 0;
    int col = 0;

    // Add basic name
    QPushButton *label_player_name = new QPushButton("NAME:");
    QPalette pushbutton_palette;
    pushbutton_palette.setColor(QPalette::ButtonText, defined_colors[TERM_DARK]);
    label_player_name->setPalette(pushbutton_palette);
    label_player_name->setToolTip("Press to change player name");
    connect(label_player_name, SIGNAL(clicked()), this, SLOT(name_change()));
    QLabel *player_name = new QLabel;
    make_standard_label(player_name, op_ptr->full_name, TERM_BLUE);
    player_name->setObjectName("PLYR_Name");
    return_layout->addWidget(label_player_name, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_name, row++, col+1, Qt::AlignRight);

    // Add gender
    QLabel *label_player_gender = new QLabel;
    make_standard_label(label_player_gender, "GENDER:", TERM_DARK);
    label_player_gender->setToolTip(get_help_topic("character_info", "Gender"));
    QLabel *player_gender = new QLabel;
    make_standard_label(player_gender, sp_ptr->title, TERM_BLUE);
    player_gender->setObjectName("PLYR_Sex");
    return_layout->addWidget(label_player_gender, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_gender, row++, col+1, Qt::AlignRight);

    // Add race
    QLabel *label_player_race = new QLabel;
    make_standard_label(label_player_race, "RACE:", TERM_DARK);
    label_player_race->setToolTip(get_help_topic("race_class_info", "Race"));
    QLabel *player_race = new QLabel;
    make_standard_label(player_race, p_info[p_ptr->prace].pr_name, TERM_BLUE);
    player_race->setObjectName("PLYR_Race");
    return_layout->addWidget(label_player_race, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_race, row++, col+1, Qt::AlignRight);

    // Add class
    QLabel *label_player_class = new QLabel;
    make_standard_label(label_player_class, "CLASS:", TERM_DARK);
    label_player_class->setToolTip(get_help_topic("race_class_info", "Class"));
    QLabel *player_class = new QLabel;
    make_standard_label(player_class, c_info[p_ptr->pclass].cl_name, TERM_BLUE);
    player_class->setObjectName("PLYR_Class");
    return_layout->addWidget(label_player_class, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_class, row++, col+1, Qt::AlignRight);

    // Add title
    QLabel *label_player_title = new QLabel;
    make_standard_label(label_player_title, "TITLE:", TERM_DARK);
    label_player_title->setToolTip(get_help_topic("character_info", "Title"));
    QLabel *player_title = new QLabel;
    make_standard_label(player_title, get_player_title(), TERM_BLUE);
    player_title->setObjectName("PLYR_Title");
    return_layout->addWidget(label_player_title, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_title, row++, col+1, Qt::AlignRight);

    // Add hit points
    QLabel *label_player_hp = new QLabel;
    make_standard_label(label_player_hp, "HIT POINTS:", TERM_DARK);
    label_player_hp->setToolTip(get_help_topic("character_info", "Hit Points"));
    QLabel *player_hp = new QLabel;
    make_standard_label(player_hp, (QString("%1/%2") .arg(p_ptr->chp) .arg(p_ptr->mhp)), TERM_BLUE);
    player_hp->setObjectName("PLYR_HP");
    return_layout->addWidget(label_player_hp, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_hp, row++, col+1, Qt::AlignRight);

    // Add spell points (if applicable)
    if (cp_ptr->spell_book)
    {
        QLabel *label_player_sp = new QLabel;
        make_standard_label(label_player_sp, "SPELL POINTS:", TERM_DARK);
        label_player_sp->setToolTip(get_help_topic("character_info", "Spell Points"));
        QLabel *player_sp = new QLabel;
        make_standard_label(player_sp, (QString("%1/%2") .arg(p_ptr->csp) .arg(p_ptr->msp)), TERM_BLUE);
        player_sp->setObjectName("PLYR_SP");
        return_layout->addWidget(label_player_sp, row, col, Qt::AlignLeft);
        return_layout->addWidget(player_sp, row++, col+1, Qt::AlignRight);
    }
    else row++;

    // Add fame or skip a space
    if (!birth_no_quests)
    {
        // Add fame
        QLabel *label_player_fame = new QLabel;
        make_standard_label(label_player_fame, "FAME:", TERM_DARK);
        label_player_fame->setToolTip(get_help_topic("character_info", "Fame"));
        QLabel *player_fame = new QLabel;
        make_standard_label(player_fame, (QString("%1") .arg(p_ptr->q_fame)), TERM_BLUE);
        player_fame->setObjectName("PLYR_Fame");
        return_layout->addWidget(label_player_fame, row, col, Qt::AlignLeft);
        return_layout->addWidget(player_fame, row++, col+1, Qt::AlignRight);
    }
    else row++;

    // Add Player Gold
    QLabel *label_player_gold = new QLabel;
    make_standard_label(label_player_gold, "GOLD:", TERM_DARK);
    label_player_gold->setToolTip(get_help_topic("character_info", "Gold"));
    QLabel *player_gold = new QLabel;
    make_standard_label(player_gold, (QString("%1") .arg(number_to_formatted_string(p_ptr->au))), TERM_BLUE);
    player_gold->setObjectName("PLYR_Gold");
    return_layout->addWidget(label_player_gold, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_gold, row++, col+1, Qt::AlignRight);

    QLabel *filler = new QLabel("  ");
    return_layout->addWidget(filler, 0, col + 2);

}

void PlayerScreenInfo::char_basic_data(QGridLayout *return_layout)
{
    int row = 0;
    int col = 0;

    // Add age
    QLabel *label_player_age = new QLabel;
    make_standard_label(label_player_age, "AGE:", TERM_DARK);
    label_player_age->setToolTip(get_help_topic("character_info", "Player Age"));
    QLabel *player_age = new QLabel;
    make_standard_label(player_age, (QString("%1") .arg(p_ptr->age)), TERM_BLUE);
    player_age->setObjectName("PLYR_Age");
    return_layout->addWidget(label_player_age, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_age, row++, col+1, Qt::AlignRight);

    // Add Height
    QLabel *label_player_height = new QLabel;
    make_standard_label(label_player_height, "HEIGHT:", TERM_DARK);
    label_player_height->setToolTip(get_help_topic("character_info", "Player Height"));
    QLabel *player_height = new QLabel;
    make_standard_label(player_height, (QString("%1") .arg(p_ptr->ht)), TERM_BLUE);
    player_height->setObjectName("PLYR_Height");
    return_layout->addWidget(label_player_height, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_height, row++, col+1, Qt::AlignRight);

    // Add Weight
    QLabel *label_player_weight = new QLabel;
    make_standard_label(label_player_weight, "WEIGHT:", TERM_DARK);
    label_player_weight->setToolTip(get_help_topic("character_info", "Player Weight"));
    QLabel *player_weight = new QLabel;
    make_standard_label(player_weight, (QString("%1") .arg(p_ptr->wt)), TERM_BLUE);
    player_weight->setObjectName("PLYR_Weight");
    return_layout->addWidget(label_player_weight, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_weight, row++, col+1, Qt::AlignRight);

    // Add Social Class
    QLabel *label_player_sc = new QLabel;
    make_standard_label(label_player_sc, "SOCIAL CLASS:", TERM_DARK);
    label_player_sc->setToolTip(get_help_topic("character_info", "Social Class"));
    QLabel *player_sc = new QLabel;
    make_standard_label(player_sc, (QString("%1") .arg(p_ptr->sc)), TERM_BLUE);
    player_sc->setObjectName("PLYR_SC");
    return_layout->addWidget(label_player_sc, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_sc, row++, col+1, Qt::AlignRight);

    // Add Game Turn
    QLabel *label_player_gturn = new QLabel;
    make_standard_label(label_player_gturn, "GAME TURN:", TERM_DARK);
    label_player_gturn->setToolTip(get_help_topic("character_info", "Game Turn"));
    QLabel *player_gturn = new QLabel;
    make_standard_label(player_gturn, (QString("%1") .arg(number_to_formatted_string(p_ptr->game_turn))), TERM_BLUE);
    player_gturn->setObjectName("TURN_Game");
    return_layout->addWidget(label_player_gturn, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_gturn, row++, col+1, Qt::AlignRight);

    // Add Player Turn
    QLabel *label_player_pturn = new QLabel;
    make_standard_label(label_player_pturn, "PLAYER TURN:", TERM_DARK);
    label_player_pturn->setToolTip(get_help_topic("character_info", "Player Turn"));
    QLabel *player_pturn = new QLabel;
    make_standard_label(player_pturn, (QString("%1") .arg(number_to_formatted_string(p_ptr->p_turn))), TERM_BLUE);
    player_pturn->setObjectName("TURN_Player");
    return_layout->addWidget(label_player_pturn, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_pturn, row++, col+1, Qt::AlignRight);

    // Add current Depth
    QLabel *label_player_cur_depth = new QLabel;
    make_standard_label(label_player_cur_depth, "CUR DEPTH:", TERM_DARK);

    label_player_cur_depth->setToolTip(get_help_topic("character_info", "Current Depth"));
    QLabel *player_cur_depth = new QLabel;
    QString cur_depth = (QString("%1") .arg(p_ptr->depth * 50));
    if (!p_ptr->depth) cur_depth = "TOWN";
    make_standard_label(player_cur_depth, (QString("%1")) .arg(cur_depth), TERM_BLUE);
    player_cur_depth->setObjectName("DEPTH_Cur");
    return_layout->addWidget(label_player_cur_depth, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_cur_depth, row++, col+1, Qt::AlignRight);

    // Add Max Depth
    QLabel *label_player_max_depth = new QLabel;
    make_standard_label(label_player_max_depth, "MAX DEPTH:", TERM_DARK);
    label_player_max_depth->setToolTip(get_help_topic("character_info", "Max Depth"));
    QLabel *player_max_depth = new QLabel;
    QString max_depth = (QString("%1") .arg(p_ptr->max_depth * 50));
    if (!p_ptr->max_depth) max_depth = "TOWN";
    make_standard_label(player_max_depth, (QString("%1")) .arg(max_depth), TERM_BLUE);
    player_max_depth->setObjectName("DEPTH_Max");
    return_layout->addWidget(label_player_max_depth, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_max_depth, row++, col+1, Qt::AlignRight);

    //Infravision
    QLabel *label_player_infra = new QLabel;
    make_standard_label(label_player_infra, "INFRAVISION:", TERM_DARK);
    label_player_infra->setToolTip(get_help_topic("character_info", "Infravision"));
    QLabel *player_infra = new QLabel;
    QString infra_string = "NONE";
    if (p_ptr->state.see_infra) infra_string = (QString("%1 feet") .arg(p_ptr->state.see_infra * 10));
    make_standard_label(player_infra, infra_string, TERM_BLUE);
    player_infra->setObjectName("PLYR_Infra");
    return_layout->addWidget(label_player_infra, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_infra, row++, col+1, Qt::AlignRight);

    QLabel *filler = new QLabel("  ");
    return_layout->addWidget(filler, 0, col + 2);
}

void PlayerScreenInfo::char_game_info(QGridLayout *return_layout)
{
    int row = 0;
    int col = 0;

    // Add Character Level
    QLabel *label_player_lev = new QLabel;
    make_standard_label(label_player_lev, "CHAR. LEVEL:", TERM_DARK);
    label_player_lev->setToolTip(get_help_topic("character_info", "Player Level"));
    QLabel *player_lev = new QLabel;
    make_standard_label(player_lev, (QString("%1") .arg(p_ptr->lev)), (p_ptr->lev >= p_ptr->max_lev) ? TERM_BLUE : TERM_RED);
    player_lev->setObjectName("PLYR_Level");
    return_layout->addWidget(label_player_lev, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_lev, row++, col+1, Qt::AlignRight);

    // Add Player Experience
    QLabel *label_player_exp = new QLabel;
    make_standard_label(label_player_exp, "EXPERIENCE:", TERM_DARK);
    label_player_exp->setToolTip(get_help_topic("character_info", "Player Current Experience"));
    QLabel *player_exp = new QLabel;
    player_exp->setObjectName("PLYR_Cur_Exp");
    make_standard_label(player_exp, (QString("%1") .arg(number_to_formatted_string(p_ptr->exp))), (p_ptr->exp >= p_ptr->max_exp) ? TERM_BLUE : TERM_RED);
    return_layout->addWidget(label_player_exp, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_exp, row++, col+1, Qt::AlignRight);

    // Add Player Maximum Experience
    QLabel *label_player_max_exp = new QLabel;
    make_standard_label(label_player_max_exp, "MAX EXP:", TERM_DARK);
    label_player_max_exp->setToolTip(get_help_topic("character_info", "Player Maximum Experience"));
    QLabel *player_max_exp = new QLabel;
    player_max_exp->setObjectName("PLYR_Max_Exp");
    make_standard_label(player_max_exp, (QString("%1") .arg(number_to_formatted_string(p_ptr->max_exp))), TERM_BLUE);
    return_layout->addWidget(label_player_max_exp, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_max_exp, row++, col+1, Qt::AlignRight);

    // Add Experience to Advance
    if (p_ptr->lev < z_info->max_level)
    {
        s32b advance = (get_experience_by_level(p_ptr->lev-1) * p_ptr->expfact / 100L);
        s32b exp_needed = advance - p_ptr->exp;
        QString exp_output = number_to_formatted_string(exp_needed);

        QLabel *label_player_exp_adv = new QLabel;
        make_standard_label(label_player_exp_adv, "ADVANCE EXP:", TERM_DARK);
        label_player_max_exp->setToolTip(get_help_topic("character_info", "Player Experience Advance"));
        QLabel *player_exp_adv = new QLabel;
        make_standard_label(player_exp_adv, (QString("%1") .arg(exp_output)), TERM_BLUE);
        player_exp_adv->setToolTip(QString("The total experience needed for the next level is %1") .arg(number_to_formatted_string(advance)));
        player_exp_adv->setObjectName("PLYR_Advance");
        return_layout->addWidget(label_player_exp_adv, row, col, Qt::AlignLeft);
        return_layout->addWidget(player_exp_adv, row++, col+1, Qt::AlignRight);
    }
    else row++;

    // Add player score
    QLabel *label_player_score = new QLabel;
    make_standard_label(label_player_score, "SCORE:", TERM_DARK);
    label_player_score->setToolTip(get_help_topic("character_info", "Player Score"));
    QLabel *player_score = new QLabel;
    make_standard_label(player_score, (QString("%1") .arg(number_to_formatted_string(p_ptr->current_score))), TERM_BLUE);
    player_score->setObjectName("PLYR_Score");
    return_layout->addWidget(label_player_score, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_score, row++, col+1, Qt::AlignRight);

    // Add Burden
    QLabel *label_player_burden = new QLabel;
    make_standard_label(label_player_burden, "BURDEN:", TERM_DARK);
    label_player_burden->setToolTip(get_help_topic("character_info", "Player Burden"));
    QLabel *player_burden = new QLabel;
    make_standard_label(player_burden, (QString("%1 lbs") .arg(formatted_weight_string(p_ptr->total_weight))), TERM_BLUE);
    player_burden->setObjectName("BURDEN_Cur");
    return_layout->addWidget(label_player_burden, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_burden, row++, col+1, Qt::AlignRight);

    // Add Max Burden
    QLabel *label_player_burden_max = new QLabel;
    make_standard_label(label_player_burden_max, "MAX WEIGHT:", TERM_DARK);
    label_player_burden_max->setToolTip(get_help_topic("character_info", "Max Burden"));
    QLabel *player_burden_max = new QLabel;
    make_standard_label(player_burden_max, (QString("%1 lbs") .arg(formatted_weight_string(normal_speed_weight_limit()))), TERM_BLUE);
    player_burden_max->setObjectName("BURDEN_Max");
    return_layout->addWidget(label_player_burden_max, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_burden_max, row++, col+1, Qt::AlignRight);

    // Add Burden %
    QLabel *label_player_burden_pct = new QLabel;
    make_standard_label(label_player_burden_pct, "% BURDEN:", TERM_DARK);
    label_player_burden_pct->setToolTip(get_help_topic("character_info", "Percent Burden"));
    QLabel *player_burden_pct = new QLabel;
    int pct = (p_ptr->total_weight * 100) / normal_speed_weight_limit();
    make_standard_label(player_burden_pct, (QString("%1%")) .arg(pct), (pct <= 100 ? TERM_BLUE : TERM_RED));
    player_burden_pct->setObjectName("BURDEN_Percent");
    return_layout->addWidget(label_player_burden_pct, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_burden_pct, row++, col+1, Qt::AlignRight);

    QLabel *filler = new QLabel("  ");
    return_layout->addWidget(filler, 0, col + 2);
}

void PlayerScreenInfo::char_stat_info(QGridLayout *stat_layout)
{
    // add the headers
    byte row = 0;
    byte col = 0;

    QLabel *stat_header = new QLabel();
    make_standard_label(stat_header, "STAT   ", TERM_DARK);
    QLabel *self_header = new QLabel();
    make_standard_label(self_header, " SELF ", TERM_DARK);
    QLabel *equip_adj_header = new QLabel();
    make_standard_label(equip_adj_header, "  EA ", TERM_DARK);
    equip_adj_header->setToolTip("Stat adjustments due to player equipment");
    QLabel *total_stat_header = new QLabel();
    make_standard_label(total_stat_header, "  TOTAL STAT", TERM_DARK);
    stat_layout->addWidget(stat_header, row, col++, Qt::AlignLeft);
    stat_layout->addWidget(self_header, row, col++, Qt::AlignLeft);
    if (birth_maximize)
    {
        QLabel *race_adj_header = new QLabel();
        make_standard_label(race_adj_header, "  RA ", TERM_DARK);
        race_adj_header->setToolTip("Stat adjustments due to player race");
        QLabel *class_adj_header = new QLabel();
        make_standard_label(class_adj_header, "  CA ", TERM_DARK);
        class_adj_header->setToolTip("Stat adjustments due to player class");
        stat_layout->addWidget(race_adj_header, row, col++, Qt::AlignRight);
        stat_layout->addWidget(class_adj_header, row, col++, Qt::AlignRight);
    }
    stat_layout->addWidget(equip_adj_header, row, col++, Qt::AlignRight);
    if (!birth_no_quests)
    {
        QLabel *reward_adj_header = new QLabel();
        make_standard_label(reward_adj_header, " QA ", TERM_DARK);
        reward_adj_header->setToolTip("Stat adjustments due to quest rewards");
        stat_layout->addWidget(reward_adj_header, row, col++, Qt::AlignRight);
    }
    stat_layout->addWidget(total_stat_header, row, col++, Qt::AlignLeft);

    row++;

    for (int i = 0; i < A_MAX; i++)
    {
        col = 0;


        // Stat label
        QLabel *stat_label = new QLabel();
        make_standard_label(stat_label, stat_names[i], TERM_DARK);
        stat_label->setToolTip(stat_entry(i));
        stat_layout->addWidget(stat_label, row, col++, Qt::AlignLeft);

        QLabel *self_label = new QLabel();
        make_standard_label(self_label, (QString("%1 ") .arg(p_ptr->stat_base_max[i])), TERM_BLUE);
        self_label->setObjectName(QString("st_base_%1") .arg(i));
        stat_layout->addWidget(self_label, row, col++, Qt::AlignLeft);

        if (birth_maximize)
        {
            QLabel *race_adj = new QLabel();
            make_standard_label(race_adj, (QString("%1 ") .arg(rp_ptr->r_adj[i])), TERM_BLUE);
            race_adj->setObjectName(QString("st_race_%1") .arg(i));
            stat_layout->addWidget(race_adj, row, col++, Qt::AlignRight);

            QLabel *class_adj = new QLabel();
            make_standard_label(class_adj, (QString("%1 ") .arg(cp_ptr->c_adj[i])), TERM_BLUE);
            class_adj->setObjectName(QString("st_class_%1") .arg(i));
            stat_layout->addWidget(class_adj, row, col++, Qt::AlignRight);
        }

        QLabel *equip_adj = new QLabel();
        make_standard_label(equip_adj, (QString("%1 ") .arg(p_ptr->state.stat_equip[i])), TERM_BLUE);
        equip_adj->setObjectName(QString("st_equip_%1") .arg(i));
        stat_layout->addWidget(equip_adj, row, col++, Qt::AlignRight);

        if (!birth_no_quests)
        {
            QLabel *quest_adj = new QLabel();
            make_standard_label(quest_adj, (QString("%1 ") .arg(p_ptr->stat_quest_add[i])), TERM_BLUE);
            quest_adj->setObjectName(QString("st_quest_%1") .arg(i));
            stat_layout->addWidget(quest_adj, row, col++, Qt::AlignRight);
        }

        QLabel *stat_total = new QLabel();
        make_standard_label(stat_total, (QString("  %1 ") .arg(cnv_stat(p_ptr->state.stat_loaded_max[i]))), TERM_BLUE);
        stat_total->setObjectName(QString("st_total_%1") .arg(i));
        stat_layout->addWidget(stat_total, row, col++, Qt::AlignLeft);

        //Display reduced stat if necessary
        bool need_display = FALSE;
        if (p_ptr->state.stat_loaded_cur[i] < p_ptr->state.stat_loaded_max[i]) need_display = TRUE;
        QString lower_stat = cnv_stat(p_ptr->state.stat_loaded_cur[i]);
        if (!need_display) lower_stat = "       ";
        QLabel *stat_reduce = new QLabel();
        make_standard_label(stat_reduce, lower_stat, TERM_RED);
        stat_reduce->setObjectName(QString("st_reduce_%1") .arg(i));
        stat_layout->addWidget(stat_reduce, row++, col++, Qt::AlignRight);
    }
}

void PlayerScreenInfo::char_combat_info(QGridLayout *return_layout)
{
    int row = 0;
    int col = 0;

    object_type object_type_body;
    object_type_body.object_wipe();

    QString first_num_string;
    QString second_num_string;
    s32b first_num;
    s32b second_num;
    s32b mult;

    // Add Speed
    QLabel *label_player_speed = new QLabel;
    make_standard_label(label_player_speed, "SPEED:", TERM_DARK);
    label_player_speed->setToolTip(get_help_topic("character_info", "Speed"));
    QLabel *player_speed = new QLabel;
    first_num = p_ptr->state.p_speed;
    // Undo temporary effects
    if (p_ptr->searching) first_num += (game_mode == GAME_NPPMORIA ? 1 : 10);
    if (p_ptr->timed[TMD_FAST]) first_num -= (game_mode == GAME_NPPMORIA ? 1 : 10);
    if (p_ptr->timed[TMD_SLOW]) first_num += (game_mode == GAME_NPPMORIA ? 1 : 10);
    // Speed is different in the different games
    if (game_mode == GAME_NPPMORIA)
    {
        first_num_string = moria_speed_labels(first_num);
        second_num = analyze_speed_bonuses(first_num, TERM_BLUE);

    }
    else
    {
        second_num = analyze_speed_bonuses(first_num, TERM_BLUE);
        if (first_num > 110)
        {
            first_num_string = (QString("Fast (%1)") .arg(first_num - 110));
            first_num_string.prepend("+");
        }
        else if (first_num < 110)
        {
            first_num_string = (QString("Slow (%1)") .arg(110 - first_num));
        }
        else first_num_string = "Normal";
    }

    make_standard_label(player_speed, first_num_string, second_num);
    player_speed->setObjectName("BASE_Speed");
    return_layout->addWidget(label_player_speed, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_speed, row++, col+1, Qt::AlignRight);

    // Add armor class
    QLabel *label_player_armor = new QLabel;
    make_standard_label(label_player_armor, "ARMOR:", TERM_DARK);
    label_player_armor->setToolTip(get_help_topic("character_info", "Armor Class"));
    QLabel *player_armor = new QLabel;
    first_num = p_ptr->state.dis_ac;
    second_num = p_ptr->state.dis_to_a;
    first_num_string.setNum(first_num);
    second_num_string.setNum(second_num);
    if (second_num < 0) second_num_string.prepend("-");
    else second_num_string.prepend("+");
    make_standard_label(player_armor, (QString("[%1, %2]") .arg(first_num_string) .arg(second_num_string)), TERM_BLUE);
    player_armor->setObjectName("PLYR_AC");
    return_layout->addWidget(label_player_armor, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_armor, row++, col+1, Qt::AlignRight);

    // Melee Weapon Stats
    QLabel *label_player_melee = new QLabel;
    make_standard_label(label_player_melee, "MELEE:", TERM_DARK);
    label_player_melee->setToolTip(get_help_topic("character_info", "Melee"));
    QLabel *player_melee = new QLabel;
    first_num = p_ptr->state.dis_to_h;
    second_num = p_ptr->state.dis_to_d;

    object_type *o_ptr = &inventory[INVEN_WIELD];
    QString dd; ;
    QString ds;
    dd.setNum(o_ptr->dd);
    ds.setNum(o_ptr->ds);
    // Player is punching monsters
    if (!o_ptr->tval)
    {
        dd.setNum(1);
        ds.setNum(1);
    }
    else if (object_known_p(o_ptr))
    {
        first_num += o_ptr->to_h;
        second_num += o_ptr->to_d;
    }
    first_num_string.setNum(first_num);
    second_num_string.setNum(second_num);
    if (first_num < 0) first_num_string.prepend("-");
    else first_num_string.prepend("+");
    if (second_num < 0) second_num_string.prepend("-");
    else second_num_string.prepend("+");
    make_standard_label(player_melee, (QString("x(%1) [%2d%3] (%4, %5)") .arg(p_ptr->state.num_blow) .arg(dd) .arg(ds) .arg(first_num_string) .arg(second_num_string)), TERM_BLUE);
    return_layout->addWidget(label_player_melee, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_melee, row++, col+1, Qt::AlignRight);

    // Add critical hit %
    QLabel *label_player_crit_hit = new QLabel;
    make_standard_label(label_player_crit_hit, "CRIT. HIT %:", TERM_DARK);
    label_player_crit_hit->setToolTip(get_help_topic("character_info", "Critical Hit"));
    QLabel *player_crit_hit = new QLabel;
    first_num = critical_hit_chance(o_ptr, p_ptr->state, TRUE) /  (CRIT_HIT_CHANCE / 100);
    first_num_string.setNum(first_num);
    make_standard_label(player_crit_hit, (QString("%1 %") .arg(first_num_string)), TERM_BLUE);
    player_crit_hit->setObjectName("HIT_Critical");
    return_layout->addWidget(label_player_crit_hit, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_crit_hit, row++, col+1, Qt::AlignRight);

    //Shooting weapon stats
    QLabel *label_player_shoot = new QLabel;
    make_standard_label(label_player_shoot, "SHOOT:", TERM_DARK);
    label_player_shoot->setToolTip(get_help_topic("character_info", "Shooting Stats"));
    QLabel *player_shoot = new QLabel;

    //Make sure we are factoring in the bow and not a swap weapon
    if (birth_swap_weapons)
    {
        if (inventory[INVEN_MAIN_WEAPON].tval == TV_BOW) o_ptr = &inventory[INVEN_MAIN_WEAPON];

        /* A bow is not wielded, just set up a "dummy, blank" object and point to that */
        else o_ptr = &object_type_body;
    }
    else o_ptr = &inventory[INVEN_BOW];

    first_num = p_ptr->state.dis_to_h;
    second_num = 0;
    mult = p_ptr->state.ammo_mult;
    if (object_known_p(o_ptr))
    {
        first_num += o_ptr->to_h;
        second_num += o_ptr->to_d;
    }

    // Factor in Rogue and brigand combat bonuses, if applicable
    mult += rogue_shot(o_ptr, &first_num, p_ptr->state);
    mult += brigand_shot(o_ptr, 0L, FALSE, p_ptr->state);

    first_num_string.setNum(first_num);
    second_num_string.setNum(second_num);
    if (first_num < 0) first_num_string.prepend("-");
    else first_num_string.prepend("+");
    if (second_num < 0) second_num_string.prepend("-");
    else second_num_string.prepend("+");
    QString output = (QString("xS(%1) xM(%2) (%3, %4)") .arg(p_ptr->state.num_fire) .arg(mult) .arg(first_num_string) .arg(second_num_string));
    //No bow
    if (!mult) output = "---------";
    make_standard_label(player_shoot, output, TERM_BLUE);
    player_shoot->setObjectName("SHOOTING_Stats");
    return_layout->addWidget(label_player_shoot, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_shoot, row++, col+1, Qt::AlignRight);

    // Searching frequency - frequency is inverted
    QLabel *label_player_search_freq = new QLabel;
    make_standard_label(label_player_search_freq, "SEARCH FREQ:", TERM_DARK);
    label_player_search_freq->setToolTip(get_help_topic("character_info", "Search Frequency"));
    QLabel *player_search_freq = new QLabel;
    if (p_ptr->state.skills[SKILL_SEARCH_FREQUENCY] > SEARCH_CHANCE_MAX) first_num_string = "1 in 1";
    else first_num_string = (QString("1 in %1") .arg(SEARCH_CHANCE_MAX - p_ptr->state.skills[SKILL_SEARCH_FREQUENCY]));
    make_standard_label(player_search_freq, first_num_string, TERM_BLUE);
    player_search_freq->setObjectName("SEARCH_Freq");
    return_layout->addWidget(label_player_search_freq, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_search_freq, row++, col+1, Qt::AlignRight);

    // Searching chance
    QLabel *label_player_search_chance = new QLabel;
    make_standard_label(label_player_search_chance, "SEARCH CHANCE:", TERM_DARK);
    label_player_search_chance->setToolTip(get_help_topic("character_info", "Search Chance"));
    QLabel *player_search_chance = new QLabel;
    if (p_ptr->state.skills[SKILL_SEARCH_CHANCE] > 100) first_num = 100;
    else first_num = p_ptr->state.skills[SKILL_SEARCH_CHANCE];
    first_num_string = (QString("%1%") .arg(first_num));
    make_standard_label(player_search_chance, first_num_string, TERM_BLUE);
    player_search_chance->setObjectName("SEARCH_Chance");
    return_layout->addWidget(label_player_search_chance, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_search_chance, row++, col+1, Qt::AlignRight);

    //Digging
    QLabel *label_player_dig = new QLabel;
    make_standard_label(label_player_dig, "TUNNEL:", TERM_DARK);
    label_player_dig->setToolTip(get_help_topic("character_info", "Tunneling"));
    QLabel *player_dig = new QLabel;
    first_num_string.setNum(p_ptr->state.skills[SKILL_DIGGING]);
    make_standard_label(player_dig, first_num_string, TERM_BLUE);
    player_dig->setObjectName("PLYR_Tunnel");
    return_layout->addWidget(label_player_dig, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_dig, row++, col+1, Qt::AlignRight);

    QLabel *filler = new QLabel("  ");
    return_layout->addWidget(filler, 0, col + 2);
}

void PlayerScreenInfo::char_ability_info(QGridLayout *return_layout)
{
    int row = 0;
    int col = 0;

    // Add Speed
    QLabel *label_player_speed = new QLabel;
    make_standard_label(label_player_speed, "SPEED:", TERM_DARK);
    label_player_speed->setToolTip(get_help_topic("character_info", "Speed"));
    return_layout->addWidget(label_player_speed, row, col, Qt::AlignLeft);
    QLabel *player_speed = new QLabel;
    player_speed->setObjectName("PLYR_Speed");
    make_ability_graph(player_speed, 0, pam_ptr->max_p_speed, calc_energy_gain(p_ptr->state.p_speed));
    return_layout->addWidget(player_speed, row++, col+1);

    // Add Saving Throw
    QLabel *label_player_save = new QLabel;
    make_standard_label(label_player_save, "SAVING THROW:", TERM_DARK);
    label_player_save->setToolTip(get_help_topic("character_info", "Saving Throw"));
    return_layout->addWidget(label_player_save, row, col, Qt::AlignLeft);
    QLabel *player_save = new QLabel;
    player_save->setObjectName("PLYR_Save");
    make_ability_graph(player_save, 0, 100, p_ptr->state.skills[SKILL_SAVE]);
    return_layout->addWidget(player_save, row++, col+1);

    // Add Stealth - note special handling since stealth is inverted
    QLabel *label_player_stealth = new QLabel;
    make_standard_label(label_player_stealth, "STEALTH:", TERM_DARK);
    label_player_stealth->setToolTip(get_help_topic("character_info", "Stealth"));
    return_layout->addWidget(label_player_stealth, row, col, Qt::AlignLeft);
    QLabel *player_stealth = new QLabel;
    player_stealth->setObjectName("PLYR_Stealth");
    int stealth = (WAKEUP_MAX - p_ptr->base_wakeup_chance);
    if (p_ptr->state.aggravate) stealth = WAKEUP_MAX;
    make_ability_graph(player_stealth, WAKEUP_MIN, WAKEUP_MAX, stealth);
    return_layout->addWidget(player_stealth, row++, col+1);

    // Add Fighting ability
    QLabel *label_player_fight = new QLabel;
    make_standard_label(label_player_fight, "FIGHTING:", TERM_DARK);
    label_player_fight->setToolTip(get_help_topic("character_info", "Fighting Ability"));
    return_layout->addWidget(label_player_fight, row, col, Qt::AlignLeft);
    QLabel *player_fight = new QLabel;
    player_fight->setObjectName("PLYR_Fight");
    make_ability_graph(player_fight, 0, pam_ptr->max_skills[SKILL_TO_HIT_MELEE], p_ptr->state.skills[SKILL_TO_HIT_MELEE]);
    return_layout->addWidget(player_fight, row++, col+1);

    // Add bow ability
    QLabel *label_player_bow = new QLabel;
    make_standard_label(label_player_bow, "SHOOTING:", TERM_DARK);
    label_player_bow->setToolTip(get_help_topic("character_info", "Shooting Ability"));
    return_layout->addWidget(label_player_bow, row, col, Qt::AlignLeft);
    QLabel *player_bow = new QLabel;
    player_bow->setObjectName("PLYR_Bow");
    make_ability_graph(player_bow, 0, pam_ptr->max_skills[SKILL_TO_HIT_BOW], p_ptr->state.skills[SKILL_TO_HIT_BOW]);
    return_layout->addWidget(player_bow, row++, col+1);

    // Add throwing ability
    QLabel *label_player_throw = new QLabel;
    make_standard_label(label_player_throw, "THROWING:", TERM_DARK);
    label_player_throw->setToolTip(get_help_topic("character_info", "Throwing Ability"));
    return_layout->addWidget(label_player_throw, row, col, Qt::AlignLeft);
    QLabel *player_throw = new QLabel;
    player_throw->setObjectName("PLYR_Disarm");
    make_ability_graph(player_throw, 0, pam_ptr->max_skills[SKILL_TO_HIT_THROW], p_ptr->state.skills[SKILL_TO_HIT_THROW]);
    return_layout->addWidget(player_throw, row++, col+1);

    // Add disarming ability
    QLabel *label_player_disarm = new QLabel;
    make_standard_label(label_player_disarm, "DISARMING:", TERM_DARK);
    label_player_disarm->setToolTip(get_help_topic("character_info", "Disarming"));
    return_layout->addWidget(label_player_disarm, row, col, Qt::AlignLeft);
    QLabel *player_disarm = new QLabel;
    player_disarm->setObjectName("PLYR_Disarm");
    make_ability_graph(player_disarm, 0, pam_ptr->max_skills[SKILL_DISARM], p_ptr->state.skills[SKILL_DISARM]);
    return_layout->addWidget(player_disarm, row++, col+1);

    // Add magic device
    QLabel *label_player_magic = new QLabel;
    make_standard_label(label_player_magic, "MAGIC DEVICE:", TERM_DARK);
    label_player_magic->setToolTip(get_help_topic("character_info", "Magic Device"));
    return_layout->addWidget(label_player_magic, row, col, Qt::AlignLeft);
    QLabel *player_magic = new QLabel;
    player_magic->setObjectName("PLYR_Magic");
    make_ability_graph(player_magic, 0, pam_ptr->max_skills[SKILL_DEVICE], p_ptr->state.skills[SKILL_DEVICE]);
    return_layout->addWidget(player_magic, row++, col+1);

    QLabel *filler = new QLabel("  ");
    return_layout->addWidget(filler, 0, col + 2);
}

// Prints out the various equipment flags, depending on the specified flag set
void PlayerScreenDialog::equip_flag_info(QGridLayout *return_layout, int flag_set)
{
    int row = 0;
    draw_equip_labels(return_layout, row, 0, TRUE, TRUE);

    int x = 0;


    u32b f1, f2, f3, fn;

    /* Extract the player flags */
    player_flags(&f1, &f2, &f3, &fn);

    if (flag_set == FLAGS_NATIVITY)
    {
        QLabel *nativity_to = new QLabel();
        make_standard_label(nativity_to, "Nativity To:", TERM_BLUE);
        return_layout->addWidget(nativity_to, row, 0, Qt::AlignLeft);
    }

    while (TRUE)
    {
        int col = 1;
        player_flag_record *pfr_ptr;
        if (flag_set == FLAGS_RESIST) pfr_ptr = &player_resist_table[x++];
        else if (flag_set == FLAGS_ABILITY) pfr_ptr = &player_abilities_table[x++];
        else pfr_ptr = &player_nativity_table[x++];  // (flag_set == FLAGS_NATIVITY)

        // We are done
        if (pfr_ptr->name.isNull()) break;

        // If in Moria, make sure the flag is used.
        if (game_mode == GAME_NPPMORIA)
        {
            if (!pfr_ptr->moria_flag) continue;
        }

        bool did_resist = FALSE;
        bool did_temp_resist = FALSE;
        bool did_immunity = FALSE;

        row++;

        for (int i = INVEN_WIELD; i < INVEN_TOTAL; i++, col++)
        {
            object_type *o_ptr = &inventory[i];

            if (!o_ptr->tval) continue;

            // First, check for immunity
            if (pfr_ptr->extra_flag)
            {
                if (o_ptr->known_obj_flags_2 & (pfr_ptr->extra_flag))
                {

                    QLabel *immune_label = new QLabel();
                    make_standard_label(immune_label, "✔", TERM_BLUE);
                    return_layout->addWidget(immune_label, row, col, Qt::AlignCenter);

                    // Too messy to inlude in player_flag_table
                    if (pfr_ptr->extra_flag == TR2_RES_ACID) immune_label->setToolTip(get_help_topic("character_info", "Acid Immunity"));
                    else if (pfr_ptr->extra_flag == TR2_IM_ELEC) immune_label->setToolTip(get_help_topic("character_info", "Lightning Immunity"));
                    else if (pfr_ptr->extra_flag == TR2_IM_FIRE) immune_label->setToolTip(get_help_topic("character_info", "Fire Immunity"));
                    else if (pfr_ptr->extra_flag == TR2_IM_COLD) immune_label->setToolTip(get_help_topic("character_info", "Cold Immunity"));
                    else if (pfr_ptr->extra_flag == TR2_IM_POIS) immune_label->setToolTip(get_help_topic("character_info", "Poison Immunity"));

                    did_immunity = TRUE;
                    continue;
                }
            }

            if (pfr_ptr->set == 1)
            {
                if ((o_ptr->known_obj_flags_1 & (pfr_ptr->this_flag)) != pfr_ptr->this_flag) continue;
            }

            if (pfr_ptr->set == 2)
            {
                if ((o_ptr->known_obj_flags_2 & (pfr_ptr->this_flag)) != pfr_ptr->this_flag) continue;
            }

            if (pfr_ptr->set == 3)
            {
                // Hack - special handling for cursed items
                if (pfr_ptr->set & (TR3_CURSE_ALL))
                {
                    if (!(o_ptr->ident & (IDENT_CURSED))) continue;
                }
                else if ((o_ptr->known_obj_flags_3 & (pfr_ptr->this_flag)) != pfr_ptr->this_flag) continue;
            }
            if (pfr_ptr->set == 4)
            {
                if ((o_ptr->known_obj_flags_native & (pfr_ptr->this_flag)) != pfr_ptr->this_flag) continue;
            }

            did_resist = TRUE;

            QLabel *resist_label = new QLabel();
            make_standard_label(resist_label, "✔", TERM_GREEN);
            return_layout->addWidget(resist_label, row, col, Qt::AlignCenter);
        }

        // Mark player resists
        col++;

        bool player_has_flag = FALSE;
        bool player_has_immunity = FALSE;

        if (pfr_ptr->set == 1)
        {
            if ((f1 & (pfr_ptr->this_flag)) == pfr_ptr->this_flag) player_has_flag = TRUE;
            if (f1 & (pfr_ptr->extra_flag))  player_has_immunity = TRUE;


        }
        if (pfr_ptr->set == 2)
        {
            if ((f2 & (pfr_ptr->this_flag)) == pfr_ptr->this_flag) player_has_flag = TRUE;
            if (f2 & (pfr_ptr->extra_flag))  player_has_immunity = TRUE;
        }
        if (pfr_ptr->set == 3)
        {
            if ((f3 & (pfr_ptr->this_flag)) == pfr_ptr->this_flag) player_has_flag = TRUE;
            if (f3 & (pfr_ptr->extra_flag))  player_has_immunity = TRUE;
        }
        if (pfr_ptr->set == 4)
        {
            if ((fn & (pfr_ptr->this_flag)) == pfr_ptr->this_flag) player_has_flag = TRUE;
            if (fn & (pfr_ptr->extra_flag))  player_has_immunity = TRUE;

            // Special hack for boiling mud and boiling water
            if (flag_set == FLAGS_NATIVITY)
            {
                if (pfr_ptr->this_flag == ELEMENT_BMUD)
                {
                    if (p_ptr->state.native_boiling_mud) player_has_flag = TRUE;
                }
                else if (pfr_ptr->this_flag == ELEMENT_BWATER)
                {
                    if (p_ptr->state.native_boiling_water) player_has_flag = TRUE;
                }
            }
        }

        if (player_has_flag)
        {
            QLabel *player_label = new QLabel();
            make_standard_label(player_label, "✔", TERM_BLUE);
            return_layout->addWidget(player_label, row, col, Qt::AlignCenter);
            did_resist = TRUE;
        }
        if (player_has_immunity)
        {
            QLabel *player_label = new QLabel;
            make_standard_label(player_label, "✔", TERM_GREEN);
            player_label->setToolTip("Inate immunity");
            return_layout->addWidget(player_label, row, col, Qt::AlignCenter);
            did_immunity = TRUE;
        }

        // Note corresponding temporary resists
        col++;
        if (flag_set == FLAGS_RESIST)
        {
            if (pfr_ptr->this_flag == TR2_RES_ACID)
            {
                if (p_ptr->timed[TMD_OPP_ACID] && !redundant_timed_event(TMD_OPP_ACID))
                {
                    QLabel *temp_label = new QLabel;
                    make_standard_label(temp_label, "✔", TERM_PURPLE);
                    temp_label->setToolTip("You temporarily resist fire.");
                    return_layout->addWidget(temp_label, row, col, Qt::AlignCenter);
                    did_temp_resist = TRUE;
                }
            }
            else if (pfr_ptr->this_flag == TR2_RES_ELEC)
            {
                if (p_ptr->timed[TMD_OPP_ELEC] && !redundant_timed_event(TMD_OPP_ELEC))
                {
                    QLabel *temp_label = new QLabel;
                    make_standard_label(temp_label, "✔", TERM_PURPLE);
                    temp_label->setToolTip("You temporarily resist lightning.");
                    return_layout->addWidget(temp_label, row, col, Qt::AlignCenter);
                    did_temp_resist = TRUE;
                }
            }
            else if (pfr_ptr->this_flag == TR2_RES_FIRE)
            {
                if (p_ptr->timed[TMD_OPP_FIRE] && !redundant_timed_event(TMD_OPP_FIRE))
                {
                    QLabel *temp_label = new QLabel;
                    make_standard_label(temp_label, "✔", TERM_PURPLE);
                    temp_label->setToolTip("You temporarily resist fire.");
                    return_layout->addWidget(temp_label, row, col, Qt::AlignCenter);
                    did_temp_resist = TRUE;
                }
            }
            else if (pfr_ptr->this_flag == TR2_RES_COLD)
            {
                if (p_ptr->timed[TMD_OPP_COLD] && !redundant_timed_event(TMD_OPP_COLD))
                {
                    QLabel *temp_label = new QLabel;
                    make_standard_label(temp_label, "✔", TERM_PURPLE);
                    temp_label->setToolTip("You temporarily resist cold.");
                    return_layout->addWidget(temp_label, row, col, Qt::AlignCenter);
                    did_temp_resist = TRUE;
                }
            }
            else if (pfr_ptr->this_flag == TR2_RES_POIS)
            {
                if (p_ptr->timed[TMD_OPP_POIS] && !redundant_timed_event(TMD_OPP_POIS))
                {
                    QLabel *temp_label = new QLabel;
                    make_standard_label(temp_label, "✔", TERM_PURPLE);
                    temp_label->setToolTip("You temporarily resist poison.");
                    return_layout->addWidget(temp_label, row, col, Qt::AlignCenter);
                    did_temp_resist = TRUE;
                }
            }
            else if (pfr_ptr->this_flag == TR2_RES_FEAR)
            {
                if (p_ptr->timed[TMD_HERO] || p_ptr->timed[TMD_BERSERK])
                {
                    QLabel *temp_label = new QLabel;
                    make_standard_label(temp_label, "✔", TERM_PURPLE);
                    temp_label->setToolTip("You are temporarily immune to fear.");
                    return_layout->addWidget(temp_label, row, col, Qt::AlignCenter);
                    did_temp_resist = TRUE;
                }
            }
        }
        else if (flag_set == FLAGS_ABILITY)
        {
            if (pfr_ptr->this_flag == TR3_SEE_INVIS)
            {
                if ((p_ptr->timed[TMD_SINVIS])  && !redundant_timed_event(TMD_SINVIS))
                {
                    QLabel *temp_label = new QLabel;
                    make_standard_label(temp_label, "✔", TERM_PURPLE);
                    temp_label->setToolTip("You can temporarily see invisible creatures.");
                    return_layout->addWidget(temp_label, row, col, Qt::AlignCenter);
                    did_temp_resist = TRUE;
                }
            }
        }
        else if (flag_set == FLAGS_NATIVITY)
        {
            if (pfr_ptr->this_flag == TN1_NATIVE_LAVA)
            {
                if ((p_ptr->timed[TMD_NAT_LAVA]) && !redundant_timed_event(TMD_NAT_LAVA))
                {
                    QLabel *temp_label = new QLabel;
                    make_standard_label(temp_label, "✔", TERM_PURPLE);
                    temp_label->setToolTip("You are temporarily native to lava terrains.");
                    return_layout->addWidget(temp_label, row, col, Qt::AlignCenter);
                    did_temp_resist = TRUE;
                }
            }
            else if (pfr_ptr->this_flag == TN1_NATIVE_OIL)
            {
                if ((p_ptr->timed[TMD_NAT_OIL]) && !redundant_timed_event(TMD_NAT_OIL))
                {
                    QLabel *temp_label = new QLabel;
                    make_standard_label(temp_label, "✔", TERM_PURPLE);
                    temp_label->setToolTip("You are temporarily native to oily terrains.");
                    return_layout->addWidget(temp_label, row, col, Qt::AlignCenter);
                    did_temp_resist = TRUE;
                }
            }
            else if (pfr_ptr->this_flag == TN1_NATIVE_SAND)
            {
                if ((p_ptr->timed[TMD_NAT_SAND]) && !redundant_timed_event(TMD_NAT_SAND))
                {
                    QLabel *temp_label = new QLabel;
                    make_standard_label(temp_label, "✔", TERM_PURPLE);
                    temp_label->setToolTip("You are temporarily native to sandy terrains.");
                    return_layout->addWidget(temp_label, row, col, Qt::AlignCenter);
                    did_temp_resist = TRUE;
                }
            }
            else if (pfr_ptr->this_flag == TN1_NATIVE_FOREST)
            {
                if ((p_ptr->timed[TMD_NAT_TREE]) && !redundant_timed_event(TMD_NAT_TREE))
                {
                    QLabel *temp_label = new QLabel;
                    make_standard_label(temp_label, "✔", TERM_PURPLE);
                    temp_label->setToolTip("You are temporarily native to forest terrains.");
                    return_layout->addWidget(temp_label, row, col, Qt::AlignCenter);
                    did_temp_resist = TRUE;
                }
            }
            else if (pfr_ptr->this_flag == TN1_NATIVE_WATER)
            {
                if ((p_ptr->timed[TMD_NAT_WATER]) && !redundant_timed_event(TMD_NAT_WATER))
                {
                    QLabel *temp_label = new QLabel;
                    make_standard_label(temp_label, "✔", TERM_PURPLE);
                    temp_label->setToolTip("You are temporarily native to watery terrains.");
                    return_layout->addWidget(temp_label, row, col, Qt::AlignCenter);
                    did_temp_resist = TRUE;
                }
            }
            else if (pfr_ptr->this_flag == TN1_NATIVE_MUD)
            {
                if ((p_ptr->timed[TMD_NAT_MUD]) && !redundant_timed_event(TMD_NAT_MUD))
                {
                    QLabel *temp_label = new QLabel;
                    make_standard_label(temp_label, "✔", TERM_PURPLE);
                    temp_label->setToolTip("You are temporarily native to muddy terrains.");
                    return_layout->addWidget(temp_label, row, col, Qt::AlignCenter);
                    did_temp_resist = TRUE;
                }
            }
        }

        int attr = TERM_RED;
        if (did_immunity) attr = TERM_BLUE;
        else if (did_resist && did_temp_resist) attr = TERM_PURPLE;
        else if (did_resist || did_temp_resist) attr = TERM_GREEN;

        // Hack reverse the colors for bad flags
        if (pfr_ptr->bad_flag)
        {
            if (attr == TERM_RED) attr = TERM_GREEN;
            else attr = TERM_RED;
        }

        QLabel *line_label = new QLabel;
        make_standard_label(line_label, pfr_ptr->name, attr);
        if (pfr_ptr->set == 2)
        {
            // Too messy to include in the charts
            if (pfr_ptr->this_flag == TR2_RES_ACID) line_label->setToolTip(get_help_topic("character_info", "Resist Acid"));
            else if (pfr_ptr->this_flag == TR2_RES_ELEC) line_label->setToolTip(get_help_topic("character_info", "Resist Electricity"));
            else if (pfr_ptr->this_flag == TR2_RES_FIRE) line_label->setToolTip(get_help_topic("character_info", "Resist Fire"));
            else if (pfr_ptr->this_flag == TR2_RES_COLD) line_label->setToolTip(get_help_topic("character_info", "Resist Cold"));
            else if (pfr_ptr->this_flag == TR2_RES_POIS) line_label->setToolTip(get_help_topic("character_info", "Resist Poison"));
            else if (pfr_ptr->this_flag == TR2_RES_BLIND) line_label->setToolTip(get_help_topic("character_info", "Resist Blindness"));
            else if (pfr_ptr->this_flag == TR2_RES_CONFU) line_label->setToolTip(get_help_topic("character_info", "Resist Confusion"));
            else if (pfr_ptr->this_flag == TR2_RES_NEXUS) line_label->setToolTip(get_help_topic("character_info", "Resist Nexus"));
            else if (pfr_ptr->this_flag == TR2_RES_NETHR) line_label->setToolTip(get_help_topic("character_info", "Resist Nether"));
            else if (pfr_ptr->this_flag == TR2_RES_CHAOS) line_label->setToolTip(get_help_topic("character_info", "Resist Chaos"));
            else if (pfr_ptr->this_flag == TR2_RES_DISEN) line_label->setToolTip(get_help_topic("character_info", "Resist Disenchantment"));
            else if (pfr_ptr->this_flag == TR2_RES_SOUND) line_label->setToolTip(get_help_topic("character_info", "Resist Sound"));
            else if (pfr_ptr->this_flag == TR2_RES_SHARD) line_label->setToolTip(get_help_topic("character_info", "Resist Shards"));
            else if (pfr_ptr->this_flag == TR2_RES_LIGHT) line_label->setToolTip(get_help_topic("character_info", "Resist Light"));
            else if (pfr_ptr->this_flag == TR2_RES_DARK) line_label->setToolTip(get_help_topic("character_info", "Resist Darkness"));
            else if (pfr_ptr->this_flag == TR2_RES_FEAR) line_label->setToolTip(get_help_topic("character_info", "Resist Fear"));
        }
        else if (pfr_ptr->set == 3)
        {
            if (pfr_ptr->this_flag == TR3_SEE_INVIS) line_label->setToolTip(get_help_topic("character_info", "See Invisible"));
            else if (pfr_ptr->this_flag == TR3_FREE_ACT) line_label->setToolTip(get_help_topic("character_info", "Free Action"));
            else if (pfr_ptr->this_flag == TR3_TELEPATHY) line_label->setToolTip(get_help_topic("character_info", "Telepathy"));
            else if (pfr_ptr->this_flag == TR3_HOLD_LIFE) line_label->setToolTip(get_help_topic("character_info", "Hold Life"));
            else if (pfr_ptr->this_flag == TR3_LIGHT) line_label->setToolTip(get_help_topic("character_info", "Permanent Light"));
            else if (pfr_ptr->this_flag == TR3_REGEN) line_label->setToolTip(get_help_topic("character_info", "Regeneration"));
            else if (pfr_ptr->this_flag == TR3_SLOW_DIGEST) line_label->setToolTip(get_help_topic("character_info", "Slow Digestion"));
            else if (pfr_ptr->this_flag == TR3_FEATHER) line_label->setToolTip(get_help_topic("character_info", "Feather Falling"));
            else if (pfr_ptr->this_flag == TR3_TELEPORT) line_label->setToolTip(get_help_topic("character_info", "Teleportation"));
            else if (pfr_ptr->this_flag == TR3_AGGRAVATE) line_label->setToolTip(get_help_topic("character_info", "Aggravation"));
            else if (pfr_ptr->this_flag == TR3_CURSE_ALL) line_label->setToolTip(get_help_topic("character_info", "Cursed Items"));
            else if (pfr_ptr->this_flag == TR3_DRAIN_EXP) line_label->setToolTip(get_help_topic("character_info", "Drain Experience"));
        }
        else if (pfr_ptr->set == 4)
        {
            if (pfr_ptr->this_flag == TN1_NATIVE_LAVA) line_label->setToolTip(get_help_topic("character_info", "Native Lava"));
            else if (pfr_ptr->this_flag == TN1_NATIVE_ICE) line_label->setToolTip(get_help_topic("character_info", "Native Ice"));
            else if (pfr_ptr->this_flag == TN1_NATIVE_OIL) line_label->setToolTip(get_help_topic("character_info", "Native Oil"));
            else if (pfr_ptr->this_flag == TN1_NATIVE_FIRE) line_label->setToolTip(get_help_topic("character_info", "Native Fire"));
            else if (pfr_ptr->this_flag == TN1_NATIVE_SAND) line_label->setToolTip(get_help_topic("character_info", "Native Sand"));
            else if (pfr_ptr->this_flag == TN1_NATIVE_FOREST) line_label->setToolTip(get_help_topic("character_info", "Native Forest"));
            else if (pfr_ptr->this_flag == TN1_NATIVE_WATER) line_label->setToolTip(get_help_topic("character_info", "Native Water"));
            else if (pfr_ptr->this_flag == TN1_NATIVE_ACID) line_label->setToolTip(get_help_topic("character_info", "Native Mud"));
            else if (pfr_ptr->this_flag == TN1_NATIVE_MUD) line_label->setToolTip(get_help_topic("character_info", "Native Mud"));
            else if (pfr_ptr->this_flag == TN1_NATIVE_BMUD) line_label->setToolTip(get_help_topic("character_info", "Native Boiling Mud"));
            else if (pfr_ptr->this_flag == TN1_NATIVE_BWATER) line_label->setToolTip(get_help_topic("character_info", "Native Boiling Water"));
        }
        return_layout->addWidget(line_label, row++, 0, Qt::AlignLeft);
    }
}


void PlayerScreenDialog::equip_modifier_info(QGridLayout *return_layout)
{
    int row = 0;
    draw_equip_labels(return_layout, row, 0, FALSE, FALSE);

    int x = 0;

    while (TRUE)
    {
        int col = 1;
        player_flag_record *pfr_ptr;
        pfr_ptr = &player_pval_table[x++];

        // We are done
        if (pfr_ptr->name.isNull()) break;

        // If in Moria, make sure the flag is used.
        if (game_mode == GAME_NPPMORIA)
        {
            if (!pfr_ptr->moria_flag) continue;
        }

        bool has_extra_flag = FALSE;
        int cumulative = 0;

        row++;

        for (int i = INVEN_WIELD; i < INVEN_TOTAL; i++, col++)
        {
            object_type *o_ptr = &inventory[i];

            bool this_flag = FALSE;
            bool this_extra_flag = FALSE;

            if (!o_ptr->tval) continue;

            // First, check for sustain
            if (pfr_ptr->extra_flag)
            {
                if (o_ptr->known_obj_flags_2 & (pfr_ptr->extra_flag))
                {
                    this_extra_flag = has_extra_flag = TRUE;
                }
            }

            if (pfr_ptr->set == 1)
            {
                if (o_ptr->known_obj_flags_1 & (pfr_ptr->this_flag)) this_flag = TRUE;
            }
            else if (pfr_ptr->set == 2)
            {
                if (o_ptr->known_obj_flags_2 & (pfr_ptr->this_flag)) this_flag = TRUE;
            }
            else if (pfr_ptr->set == 3)
            {
                if (o_ptr->known_obj_flags_3 & (pfr_ptr->this_flag)) this_flag = TRUE;
            }
            else if (pfr_ptr->set == 4)
            {
                if (o_ptr->known_obj_flags_native & (pfr_ptr->this_flag)) this_flag = TRUE;
            }

            // Nothing to mark
            if (!this_flag && !this_extra_flag) continue;

            int attr = TERM_GREEN;
            if (o_ptr->pval < 0) attr = TERM_RED;
            QString pval_num = (QString("%1") .arg(o_ptr->pval));
            if (o_ptr->tval > 0) pval_num.prepend("+");
            if (this_extra_flag)
            {
                pval_num = (QString("<u>%1</u>") .arg(pval_num));
                if (o_ptr->pval < 0) attr = TERM_ORANGE;
                else attr = TERM_BLUE;
            }

            QLabel *pval_label = new QLabel();
            make_standard_label(pval_label, pval_num, attr);
            return_layout->addWidget(pval_label, row, col, Qt::AlignCenter);

            cumulative += o_ptr->pval;
        }    

        int attr = TERM_GREEN;
        if (cumulative < 0) attr = TERM_RED;
        if (has_extra_flag)
        {
            if (cumulative < 0) attr = TERM_ORANGE;
            else attr = TERM_BLUE;
        }

        QLabel *line_label = new QLabel;
        make_standard_label(line_label, pfr_ptr->name, attr);

        if (pfr_ptr->set == 1)
        {
            // Too messy to include in the charts
            if (pfr_ptr->this_flag == TR1_INFRA)        line_label->setToolTip(get_help_topic("character_info", "Infravision"));
            else if (pfr_ptr->this_flag == TR1_STEALTH) line_label->setToolTip(get_help_topic("character_info", "Stealth"));
            else if (pfr_ptr->this_flag == TR1_SEARCH)  line_label->setToolTip(get_help_topic("character_info", "Searching"));
            else if (pfr_ptr->this_flag == TR1_SPEED)   line_label->setToolTip(get_help_topic("character_info", "Speed"));
            else if (pfr_ptr->this_flag == TR1_TUNNEL)  line_label->setToolTip(get_help_topic("character_info", "Tunneling"));
            else if (pfr_ptr->this_flag == TR1_BLOWS)   line_label->setToolTip(get_help_topic("character_info", "Extra Blows"));
            else if (pfr_ptr->this_flag == TR1_SHOTS)   line_label->setToolTip(get_help_topic("character_info", "Extra Shots"));
            else if (pfr_ptr->this_flag == TR1_MIGHT)   line_label->setToolTip(get_help_topic("character_info", "Extra Might"));
        }
        return_layout->addWidget(line_label, row++, 0, Qt::AlignLeft);
    }

    QLabel *filler = new QLabel("  ");
    return_layout->addWidget(filler, 0, 5);

}

PlayerScreenDialog::PlayerScreenDialog(void)
{
    QVBoxLayout *main_layout = new QVBoxLayout;

    QScrollArea *scroll_area = new QScrollArea;
    scroll_area->setWidgetResizable(TRUE);
    scroll_area->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    main_layout->addWidget(scroll_area);
    main_layout->setContentsMargins(0,0,0,0);

    // Title Box
    QVBoxLayout *title_line = new QVBoxLayout;
    main_layout->addLayout(title_line);
    QLabel *main_prompt = new QLabel(QString("<h2>Character Information %1 %2 </h2>") .arg(VERSION_MODE_NAME) .arg(VERSION_STRING));
    title_line->addWidget(main_prompt, Qt::AlignCenter);
    main_layout->addStretch(1);


    // Char info line
    QHBoxLayout *char_info = new QHBoxLayout;
    main_layout->addLayout(char_info);


    QVBoxLayout *vlay_basic = new QVBoxLayout;
    char_info->addLayout(vlay_basic);
    QGridLayout *basic_info = new QGridLayout;
    char_basic_info(basic_info);
    vlay_basic->addLayout(basic_info);
    vlay_basic->addStretch(1);

    QVBoxLayout *vlay_data = new QVBoxLayout;
    char_info->addLayout(vlay_data);
    QGridLayout *basic_data = new QGridLayout;
    char_basic_data(basic_data);
    vlay_data->addLayout(basic_data);
    vlay_data->addStretch(1);

    QVBoxLayout *vlay_game_info = new QVBoxLayout;
    char_info->addLayout(vlay_game_info);
    QGridLayout *game_info = new QGridLayout;
    char_game_info(game_info);
    vlay_game_info->addLayout(game_info);
    vlay_game_info->addStretch(1);

    QVBoxLayout *vlay_combat_info = new QVBoxLayout;
    char_info->addLayout(vlay_combat_info);
    QGridLayout *combat_info = new QGridLayout;
    char_combat_info(combat_info);
    vlay_combat_info->addLayout(combat_info);
    vlay_combat_info->addStretch(1);

    QVBoxLayout *vlay_ability_info = new QVBoxLayout;
    char_info->addLayout(vlay_ability_info);
    QGridLayout *ability_info = new QGridLayout;
    char_ability_info(ability_info);
    vlay_ability_info->addLayout(ability_info);
    vlay_ability_info->addStretch(1);

    QVBoxLayout *vlay_stat_info = new QVBoxLayout;
    char_info->addLayout(vlay_stat_info);
    QGridLayout *stat_info = new QGridLayout;
    char_stat_info(stat_info);
    vlay_stat_info->addLayout(stat_info);
    vlay_stat_info->addStretch(1);


    // Add player history
    // Title Box
    QVBoxLayout *history_box = new QVBoxLayout;
    main_layout->addStretch();
    main_layout->addLayout(history_box);
    QLabel *history = new QLabel();
    make_standard_label(history, p_ptr->history, TERM_BLUE);
    history_box->addWidget(history);
    main_layout->addStretch(1);

    // Object Info
    QGridLayout *equip_info = new QGridLayout;
    main_layout->addLayout(equip_info);

    QGridLayout *resist_flags = new QGridLayout;
    QGridLayout *ability_flags = new QGridLayout;
    QGridLayout *equip_mods = new QGridLayout;
    QGridLayout *nativity_flags = new QGridLayout;


    QLabel *resist_label = new QLabel("<h3>Resistance Information</h3>");
    resist_label->setToolTip(QString("Blue represents elemental immunity, green represents resistance, and purple represents double resistance."));
    equip_info->addWidget(resist_label, 0, 0, Qt::AlignCenter);
    equip_flag_info(resist_flags, FLAGS_RESIST);
    equip_info->addLayout(resist_flags, 1, 0);

    QLabel *ability_label = new QLabel("<h3>Ability Information</h3>");
    equip_info->addWidget(ability_label, 0, 1, Qt::AlignCenter);
    equip_flag_info(ability_flags, FLAGS_ABILITY);
    equip_info->addLayout(ability_flags,  1, 1);

    QLabel *nativity_label = new QLabel("<h3>Nativity Information</h3>");
    equip_info->addWidget(nativity_label, 0, 2, Qt::AlignCenter);
    equip_flag_info(nativity_flags, FLAGS_NATIVITY);
    equip_info->addLayout(nativity_flags, 1, 2);

    QLabel *modifier_label = new QLabel("<h3>Equipment Modifiers</h3>");
    equip_info->addWidget(modifier_label, 0, 3, Qt::AlignCenter);
    equip_modifier_info(equip_mods);
    equip_info->addLayout(equip_mods, 1, 3);

    QDialogButtonBox buttons;
    buttons.setStandardButtons(QDialogButtonBox::Close);
    connect(&buttons, SIGNAL(rejected()), this, SLOT(close()));
    main_layout->addWidget(&buttons);

    setLayout(main_layout);
    setWindowTitle(tr("Player Information"));

    this->exec();
}



void do_cmd_character_screen()
{
    PlayerScreenDialog();
}
