/*
 * File: qt_sidebar.cpp
 *
 * Copyright (c) 2015  Jeff Greene, Diego Gonzalez
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

#include <src/qt_mainwindow.h>
#include <src/npp.h>
#include <QHeaderView>
#include "tilebag.h"
#include <src/player_screen.h>
#include <src/utilities.h>
#include <src/help.h>

// Taken from 32 bit colors in defines.h
#define SBAR_NORMAL "#00FF00"
#define SBAR_DRAINED "#FFFF00"
#define SBAR_GOLD   "#FFD700"

#define SIDEBAR_MON_MAX     15

QVector<s16b> sidebar_monsters;

void MainWindow::create_sidebar()
{
    player_info_vlay = new QVBoxLayout();
    mon_health_vlay = new QVBoxLayout();

    sidebar_vlay->addLayout(player_info_vlay);
    sidebar_vlay->addLayout(mon_health_vlay);
    sidebar_vlay->addStretch(1);


    QHBoxLayout *player_info_hlay = new QHBoxLayout;
    player_info_vlay->addLayout(player_info_hlay);
    QVBoxLayout *player_info_labels = new QVBoxLayout;
    QVBoxLayout *player_info_data = new QVBoxLayout;
    player_info_hlay->addLayout(player_info_labels);
    player_info_hlay->addLayout(player_info_data);

    // Hitpoints
    QLabel *hp_label = new QLabel;
    hp_label->setText(color_string("HP", SBAR_NORMAL));
    player_info_labels->addWidget(hp_label, Qt::AlignLeft);
    hp_label->setToolTip(get_help_topic("character_info", "Hit Points"));
    QLabel *hp_info = new QLabel;
    hp_info->setObjectName("HP");
    player_info_data->addWidget(hp_info, Qt::AlignRight);

    // SpellPoints
    QLabel *sp_label = new QLabel;
    sp_label->setText(color_string("SP", SBAR_NORMAL));
    player_info_labels->addWidget(sp_label, Qt::AlignLeft);
    sp_label->setObjectName("SP_LABEL");
    sp_label->setToolTip(get_help_topic("character_info", "Spell Points"));
    QLabel *sp_info = new QLabel;
    sp_info->setObjectName("SP_INFO");
    player_info_data->addWidget(sp_info, Qt::AlignRight);
    sp_label->hide();
    sp_info->hide();

    // Current Experience
    QLabel *exp_cur_label = new QLabel;
    exp_cur_label->setText(color_string("CUR EXP ", SBAR_NORMAL));
    exp_cur_label->setObjectName("EXP_CUR_LABEL");
    exp_cur_label->setToolTip(get_help_topic("character_info", "Player Current Experience"));
    player_info_labels->addWidget(exp_cur_label, Qt::AlignLeft);
    QLabel *exp_cur_info = new QLabel;
    exp_cur_info->setObjectName("EXP_CUR_INFO");
    player_info_data->addWidget(exp_cur_info, Qt::AlignRight);

    // Max Experience
    QLabel *exp_max_label = new QLabel;
    exp_max_label->setText(color_string("MAX EXP", SBAR_NORMAL));
    exp_max_label->setObjectName("EXP_MAX_LABEL");
    exp_max_label->setToolTip(get_help_topic("character_info", "Player Maximum Experience"));
    player_info_labels->addWidget(exp_max_label, Qt::AlignLeft);
    QLabel *exp_max_info = new QLabel;
    exp_max_info->setObjectName("EXP_MAX_INFO");
    player_info_data->addWidget(exp_max_info, Qt::AlignRight);

    // Next Level
    QLabel *exp_next_label = new QLabel;
    exp_next_label->setText(color_string("EXP", SBAR_NORMAL));
    exp_next_label->setObjectName("EXP_NEXT_LABEL");
    exp_next_label->setToolTip(get_help_topic("character_info", "Player Experience Advance"));
    player_info_labels->addWidget(exp_next_label, Qt::AlignLeft);
    QLabel *exp_next_info = new QLabel;
    exp_next_info->setObjectName("EXP_NEXT_INFO");
    player_info_data->addWidget(exp_next_info, Qt::AlignRight);

    // gold
    QLabel *gold_label = new QLabel;
    gold_label->setText(color_string("GOLD", SBAR_GOLD));
    gold_label->setToolTip(get_help_topic("character_info", "Gold"));
    player_info_labels->addWidget(gold_label, Qt::AlignLeft);
    QLabel *gold_info = new QLabel;
    gold_info->setObjectName("GOLD");
    player_info_data->addWidget(gold_info, Qt::AlignRight);

    // stats
    for (int i = 0; i < A_MAX; i++)
    {
        QLabel *this_label = new QLabel;
        QString label_name = (QString("STAT_LABEL_%1") .arg(i));
        this_label->setObjectName(label_name);
        this_label->setToolTip(stat_entry(i));
        player_info_labels->addWidget(this_label, Qt::AlignLeft);
        QLabel *stat_info = new QLabel;
        stat_info->setObjectName(QString("STAT_INFO_%1") .arg(i));
        player_info_data->addWidget(stat_info, Qt::AlignRight);
    }

    // Armor Class
    QLabel *ac_label = new QLabel;
    ac_label->setText(color_string("AC", SBAR_NORMAL));
    ac_label->setToolTip(get_help_topic("character_info", "Armor Class"));
    player_info_labels->addWidget(ac_label, Qt::AlignLeft);
    QLabel *ac_info = new QLabel;
    ac_info->setObjectName("ARMOR CLASS");
    player_info_data->addWidget(ac_info, Qt::AlignRight);

    // speed
    QLabel *speed_info = new QLabel;
    speed_info->setObjectName("SPEED");
    gold_label->setToolTip(get_help_topic("character_info", "Speed"));
    player_info_vlay->addWidget(speed_info, Qt::AlignLeft);

    // depth
    QLabel *depth_info = new QLabel;
    depth_info->setObjectName("DEPTH");
    player_info_vlay->addWidget(depth_info, Qt::AlignLeft);

    // feeling
    QLabel *feeling_info = new QLabel;
    feeling_info->setObjectName("FEELING");
    player_info_vlay->addWidget(feeling_info, Qt::AlignLeft);

    // quest
    QLabel *quest_info = new QLabel;
    quest_info->setObjectName("QUEST");
    player_info_vlay->addWidget(quest_info, Qt::AlignLeft);


}



/*
 * Prints the speed of a character.
 */
static void prt_speed(QLabel *this_label)
{
    int i = p_ptr->state.p_speed;

    byte attr = TERM_WHITE;
    QString speed_lbl = " ";
    speed_lbl.clear();

    /* Hack -- Visually "undo" the Search Mode Slowdown */
    if (p_ptr->searching) i += ((game_mode == GAME_NPPMORIA) ? 1 : 10);

    // Moria is handled differently
    if (game_mode == GAME_NPPMORIA)
    {
        attr = analyze_speed_bonuses(i, TERM_L_GREEN);
        speed_lbl = moria_speed_labels(i);
    }

    /* Fast */
    else if (i > 110)
    {
        attr = analyze_speed_bonuses(i, TERM_L_GREEN);
        speed_lbl = QString("Fast (+%1)").arg(i - 110);
    }
    else if (i < 110)
    {
        attr = analyze_speed_bonuses(i, TERM_L_GREEN);
        speed_lbl = QString("Slow (-%1)").arg(110 - i);
    }

    if (!speed_lbl.length())
    {
        if (this_label->isVisible())  this_label->hide();
    }
    else
    {
        if (!this_label->isVisible())  this_label->show();
        this_label->setText(color_string(speed_lbl, attr));
    }

}

/*
 * Prints depth in stat area
 */
static void prt_feeling(QLabel *this_label)
{
    QString feel;
    byte attr = TERM_L_GREEN;

    /* No sensing things in Moria */
    if (game_mode == GAME_NPPMORIA)
    {
        feel.clear();
    }

    /* No useful feeling in town, or no feeling yet */
    else if ((!p_ptr->depth) || (!feeling) || (!do_feeling))
    {
        feel.clear();
    }

    else if (p_ptr->dungeon_type == DUNGEON_TYPE_ARENA)
    {
        attr = TERM_RED_LAVA;
        feel = "F:Arena";
    }

    /* Get color of level based on feeling  -JSV- */
    else if (p_ptr->dungeon_type == DUNGEON_TYPE_LABYRINTH)
    {
        attr = TERM_L_BLUE;
        feel = "F:Labyrinth";
    }

    /* Get color of level based on feeling  -JSV- */
    else if (p_ptr->dungeon_type == DUNGEON_TYPE_WILDERNESS)
    {
        attr = TERM_GREEN;
        feel = "F:Wilderness";
    }
    else if (p_ptr->dungeon_type == DUNGEON_TYPE_GREATER_VAULT)
    {
        attr = TERM_VIOLET;
        feel = "F:GreatVault";
    }
    else if (feeling ==  1) {attr = TERM_RED;		feel = "F:Special";}
    else if (feeling ==  2) {attr = TERM_L_RED;		feel = "F:Superb";}
    else if (feeling ==  3) {attr = TERM_ORANGE;	feel = "F:Excellent";}
    else if (feeling ==  4) {attr = TERM_ORANGE;	feel = "F:Very Good";}
    else if (feeling ==  5) {attr = TERM_YELLOW;	feel = "F:Good";}
    else if (feeling ==  6) {attr = TERM_YELLOW;	feel = "F:Lucky";}
    else if (feeling ==  7) {attr = TERM_YELLOW;	feel = "F:LuckTurning";}
    else if (feeling ==  8) {attr = TERM_L_GREEN;		feel = "F:Like Looks";}
    else if (feeling ==  9) {attr = TERM_L_GREEN;		feel = "F:Not All Bad";}
    else if (feeling == 10) {attr = TERM_L_GREEN;  	feel = "F:Boring";}

    /* (feeling >= LEV_THEME_HEAD) */
    else  					{attr = TERM_BLUE;		feel = "F:Themed";}

    if (!feel.length())
    {
        if (this_label->isVisible())  this_label->hide();
    }
    else
    {
        if (!this_label->isVisible())  this_label->show();
        this_label->setText(color_string(feel, attr));
    }
}

/*
 * Calculate the hp color separately, for ports.
 */
static int player_hp_attr(void)
{
    byte attr;

    if (p_ptr->chp >= p_ptr->mhp)
        attr = TERM_L_GREEN;
    else if (p_ptr->chp > (p_ptr->mhp * op_ptr->hitpoint_warn) / 10)
        attr = TERM_YELLOW;
    else
        attr = TERM_L_RED;

    return attr;
}

/*
 * Calculate the sp color separately, for ports.
 */
static int player_sp_attr(void)
{
    byte attr;

    if (p_ptr->csp >= p_ptr->msp)
        attr = TERM_L_GREEN;
    else if (p_ptr->csp > (p_ptr->msp * op_ptr->hitpoint_warn) / 10)
        attr = TERM_YELLOW;
    else
        attr = TERM_L_RED;

    return attr;
}

static /* Figure out which monsters to display in the sidebar */
void update_mon_sidebar_list(void)
{
    monster_type *m_ptr;
    monster_type *m2_ptr;
    monster_race *r_ptr;
    monster_race *r2_ptr;
    int i;

    QVector<s16b> adjacent_monsters;
    QVector<s16b> line_of_sight_monsters;
    QVector<s16b> visible_monsters;

    sidebar_monsters.clear();
    adjacent_monsters.clear();
    line_of_sight_monsters.clear();
    visible_monsters.clear();

    // Clear the list
    for (i = 1; i < mon_max; i++) mon_list[i].sidebar = FALSE;

    /* First list the targeted monster, if there is one */
    if (p_ptr->health_who)
    {
        /* Must be visible */
        if (mon_list[p_ptr->health_who].ml)
        {
            sidebar_monsters.append(p_ptr->health_who);

            /* We are tracking this one */
            mon_list[p_ptr->health_who].sidebar = TRUE;
        }
    }

    /* Scan the list of monsters on the level */
    for (i = 1; i < mon_max; i++)
    {
        m_ptr = &mon_list[i];
        r_ptr = &r_info[m_ptr->r_idx];

        /* Ignore dead monsters */
        if (!m_ptr->r_idx) continue;

        /* Only consider visible monsters */
        if (!m_ptr->ml) continue;

        /* Hack - ignore lurkers and trappers */
        if (r_ptr->d_char == '.') continue;

        /* Already recorded target monster */
        if (i == p_ptr->health_who) continue;

        /* Now decide which list to include them in first adjacent monsters*/
        if ((GET_SQUARE((p_ptr->py - m_ptr->fy)) + GET_SQUARE((p_ptr->px - m_ptr->fx))) < 2)
        {
            adjacent_monsters.append(i);
            continue;
        }
        /* Projectable ones next */
        if (m_ptr->project)
        {
            line_of_sight_monsters.append(i);
            continue;
        }

        /* Visible, not projectable last */
        visible_monsters.append(i);
    }

    /* Sort the lists by monster power using bubble sort */
    /*  First do adjacent monsters */
    for (i = 0; i < adjacent_monsters.size(); i++)
    {
        for (int j = i + 1; j < adjacent_monsters.size(); j++)
        {
            m_ptr = &mon_list[adjacent_monsters[i]];
            r_ptr = &r_info[m_ptr->r_idx];
            m2_ptr = &mon_list[adjacent_monsters[j]];
            r2_ptr = &r_info[m2_ptr->r_idx];

            if (r_ptr->mon_power < r2_ptr->mon_power)
            {
                int temp = adjacent_monsters[i];
                adjacent_monsters[i] = adjacent_monsters[j];
                adjacent_monsters[j] = temp;
            }
        }
    }

    /* Now add them to the list */
    for (i = 0; i < adjacent_monsters.size(); i++)
    {
        sidebar_monsters.append(adjacent_monsters[i]);

        /* We are tracking this one */
        mon_list[adjacent_monsters[i]].sidebar = TRUE;

        /* paranoia - would only happen if SIDEBAR_MONSTER_MAX was less than 10*/
        if (sidebar_monsters.size() >= SIDEBAR_MONSTER_MAX) return;
    }

    /*  Next do projectable monsters */
    for (i = 0; i < line_of_sight_monsters.size(); i++)
    {
        for (int j = i + 1; j < line_of_sight_monsters.size(); j++)
        {
            m_ptr = &mon_list[line_of_sight_monsters[i]];
            r_ptr = &r_info[m_ptr->r_idx];
            m2_ptr = &mon_list[line_of_sight_monsters[j]];
            r2_ptr = &r_info[m2_ptr->r_idx];

            if (r_ptr->mon_power < r2_ptr->mon_power)
            {
                int temp = line_of_sight_monsters[i];
                line_of_sight_monsters[i] = line_of_sight_monsters[j];
                line_of_sight_monsters[j] = temp;
            }
        }
    }

    /* Now add them to the list */
    for (i = 0; i < line_of_sight_monsters.size(); i++)
    {
        sidebar_monsters.append(line_of_sight_monsters[i]);

        /* We are tracking this one */
        mon_list[line_of_sight_monsters[i]].sidebar = TRUE;

        /* Check to see if the list is full */
        if (sidebar_monsters.size() >= SIDEBAR_MONSTER_MAX) return;
    }

    /*  Finally to other viewable monsters monsters */
    for (i = 0; i < visible_monsters.size(); i++)
    {
        for (int j = i + 1; j < visible_monsters.size(); j++)
        {
            m_ptr = &mon_list[visible_monsters[i]];
            r_ptr = &r_info[m_ptr->r_idx];
            m2_ptr = &mon_list[visible_monsters[j]];
            r2_ptr = &r_info[m2_ptr->r_idx];

            if (r_ptr->mon_power < r2_ptr->mon_power)
            {
                int temp = visible_monsters[i];
                visible_monsters[i] = visible_monsters[j];
                visible_monsters[j] = temp;
            }
        }
    }

    /* Now add them to the list */
    for (i = 0; i < visible_monsters.size(); i++)
    {
        sidebar_monsters.append(visible_monsters[i]);

        /* We are tracking this one */
        mon_list[visible_monsters[i]].sidebar = TRUE;

        /* Check to see if the list is full */
        if (sidebar_monsters.size() >= SIDEBAR_MONSTER_MAX) return;
    }
}

void MainWindow::update_sidebar_font()
{
    if (!p_ptr->playing) return;

    // Update the player sidebar info
    QList<QLabel *> lbl_list = sidebar_widget->findChildren<QLabel *>();
    {
        for (int x = 0; x < lbl_list.size(); x++)
        {
            QLabel *this_lbl = lbl_list.at(x);

            this_lbl->setFont(font_sidebar_window);
        }
    }
}


void MainWindow::update_sidebar_player()
{
    if (!p_ptr->playing) return;

    // Update the player sidebar info
    QList<QLabel *> lbl_list = sidebar_widget->findChildren<QLabel *>();
    {
        for (int x = 0; x < lbl_list.size(); x++)
        {
            QLabel *this_lbl = lbl_list.at(x);

            QString this_name = this_lbl->objectName();

            //Not a named label
            if (!this_name.length()) continue;

            // Update all of the stats
            if (this_name.contains("STAT_")) for (int i = 0; i < A_MAX; i++)
            {
                if (this_name.contains(QString("STAT_LABEL_%1") .arg(i)))
                {
                    QString stat_string;

                    QColor this_color = SBAR_NORMAL;
                    if (p_ptr->stat_base_cur[i] < p_ptr->stat_base_max[i]) this_color = SBAR_DRAINED;

                    if (p_ptr->stat_base_cur[i] < p_ptr->stat_base_max[i])
                    {
                        stat_string = stat_names_reduced[i];
                    }
                    else stat_string = stat_names[i];

                    if (p_ptr->stat_base_max[i] >= 18+100)
                    {
                        stat_string[3] = '!';
                    }

                    this_lbl->setText(color_string(stat_string, this_color));

                    continue;
                }
                if (this_name.contains(QString("STAT_INFO_%1") .arg(i)))
                {
                    QColor this_color = SBAR_NORMAL;
                    if (p_ptr->stat_base_cur[i] < p_ptr->stat_base_max[i]) this_color = SBAR_DRAINED;

                    QString stat_string = cnv_stat(p_ptr->state.stat_loaded_cur[i]);

                    this_lbl->setText(color_string(stat_string, this_color));
                    this_lbl->setAlignment(Qt::AlignRight);
                    continue;
                }
            }
            // Update the hit points
            if (this_name.operator ==("HP"))
            {
                QString hp = QString("%1/%2").arg(p_ptr->chp).arg(p_ptr->mhp);

                int this_color = player_hp_attr();

                this_lbl->setText(color_string(hp, this_color));
                this_lbl->setAlignment(Qt::AlignRight);
                continue;
            }


            //update the spell points
            if (this_name.operator ==("SP_LABEL"))
            {
                if (p_ptr->msp > 0)
                {
                    if (!this_lbl->isVisible()) this_lbl->show();
                }
                else if (this_lbl->isVisible()) this_lbl->hide();
                continue;
            }
            if (this_name.operator ==("SP_INFO"))
            {
                if (p_ptr->msp > 0)
                {
                    if (!this_lbl->isVisible()) this_lbl->show();

                    int this_color = player_sp_attr();
                    QString sp = QString("%1/%2").arg(p_ptr->csp).arg(p_ptr->msp);
                    this_lbl->setText(color_string(sp, this_color));
                    this_lbl->setAlignment(Qt::AlignRight);
                }
                else if (this_lbl->isVisible()) this_lbl->hide();
                continue;
            }
            if (this_name.operator ==("EXP_CUR_LABEL"))
            {

                QColor this_color;
                QString this_text;
                if (p_ptr->exp < p_ptr->max_exp)
                {
                    this_color = SBAR_DRAINED;
                    this_text = "Cur Exp";
                }
                else
                {
                    this_color = SBAR_NORMAL;
                    this_text = "CUR EXP";
                }
                this_lbl->setText(color_string(this_text, this_color));
                continue;
            }
            if (this_name.operator ==("EXP_CUR_INFO"))
            {

                QColor this_color;
                QString this_text = number_to_formatted_string(p_ptr->exp);

                if (p_ptr->exp < p_ptr->max_exp) this_color = SBAR_DRAINED;
                else this_color = SBAR_NORMAL;

                this_lbl->setText(color_string(this_text, this_color));
                this_lbl->setAlignment(Qt::AlignRight);
                continue;
            }
            if (this_name.operator ==("EXP_MAX_LABEL"))
            {
                if (p_ptr->exp == p_ptr->max_exp)
                {
                    if (this_lbl->isVisible()) this_lbl->hide();
                    continue;
                }

                if (!this_lbl->isVisible()) this_lbl->show();

                QColor this_color = SBAR_NORMAL;
                QString this_text = "Max Esp";
                this_lbl->setText(color_string(this_text, this_color));
                continue;
            }
            if (this_name.operator ==("EXP_MAX_INFO"))
            {
                if (p_ptr->exp == p_ptr->max_exp)
                {
                    if (this_lbl->isVisible()) this_lbl->hide();
                    continue;
                }

                if (!this_lbl->isVisible()) this_lbl->show();

                QColor this_color = SBAR_NORMAL;

                QString this_text = number_to_formatted_string(p_ptr->max_exp);

                this_lbl->setText(color_string(this_text, this_color));
                this_lbl->setAlignment(Qt::AlignRight);
                continue;
            }
            if (this_name.operator ==("EXP_NEXT_LABEL"))
            {
                if (p_ptr->lev == z_info->max_level)
                {
                    if (this_lbl->isVisible()) this_lbl->hide();
                    continue;
                }

                if (!this_lbl->isVisible()) this_lbl->show();

                QColor this_color;
                QString this_text;
                if (p_ptr->exp < p_ptr->max_exp)
                {
                    this_color = SBAR_DRAINED;
                    this_text = "Next Level";
                }
                else
                {
                    this_color = SBAR_NORMAL;
                    this_text = "NEXT LEVEL";
                }
                this_lbl->setText(color_string(this_text, this_color));
                continue;
            }
            if (this_name.operator ==("EXP_NEXT_INFO"))
            {
                if (p_ptr->lev == z_info->max_level)
                {
                    if (this_lbl->isVisible()) this_lbl->hide();
                    continue;
                }

                if (!this_lbl->isVisible()) this_lbl->show();

                QColor this_color;

                s32b xp = (get_experience_by_level(p_ptr->lev-1) * p_ptr->expfact / 100L) - p_ptr->exp;

                QString this_text = number_to_formatted_string(xp);

                if (p_ptr->exp < p_ptr->max_exp) this_color = SBAR_DRAINED;
                else this_color = SBAR_NORMAL;

                this_lbl->setText(color_string(this_text, this_color));
                this_lbl->setAlignment(Qt::AlignRight);
                continue;
            }
            if (this_name.operator ==("GOLD"))
            {
                QString gold = number_to_formatted_string(p_ptr->au);
                this_lbl->setText(color_string(gold, TERM_GOLD));
                this_lbl->setAlignment(Qt::AlignRight);
                continue;
            }
            if (this_name.operator ==("ARMOR CLASS"))
            {
                int known_ac = p_ptr->state.dis_ac + p_ptr->state.dis_to_a;
                QString ac = number_to_formatted_string(known_ac);
                this_lbl->setText(color_string(ac, SBAR_NORMAL));
                this_lbl->setAlignment(Qt::AlignRight);
                continue;
            }
            if (this_name.operator ==("SPEED"))
            {
                prt_speed(this_lbl);
                continue;
            }
            if (this_name.operator ==("DEPTH"))
            {
                QString depth;
                if (!p_ptr->depth) depth = "Town";
                else depth = QString("%1' (L%2)").arg(p_ptr->depth * 50).arg(p_ptr->depth);
                this_lbl->setText(color_string(depth, SBAR_NORMAL));
                continue;
            }
            if (this_name.operator ==("FEELING"))
            {
                prt_feeling(this_lbl);
                continue;
            }
            if (this_name.operator ==("QUEST"))
            {
                byte attr;
                QString quest = format_quest_indicator(&attr);
                if (!quest.length())
                {
                    if (this_lbl->isVisible()) this_lbl->hide();
                }
                else
                {
                    if (this_lbl->isVisible()) this_lbl->show();
                    this_lbl->setText(color_string(quest, attr));
                    this_lbl->setToolTip(describe_quest(guild_quest_level()));
                }
                continue;
            }
        }
    }
}


void MainWindow::sidebar_display_mon(int m_idx)
{
    monster_type *m_ptr = mon_list + m_idx;
    monster_race *r_ptr = r_info + m_ptr->r_idx;

    QString monster_name = color_string(QString("'%1'  %2") .arg(r_ptr->d_char) .arg(r_ptr->r_name_short), r_ptr->d_color);

    QLabel *mon_name = new QLabel(monster_name);

    if (use_graphics)
    {
        QPixmap pix = ui_get_tile(r_ptr->tile_id);
        pix = pix.scaled(cell_wid, cell_hgt);
        mon_name->setPixmap(pix);
    }
    mon_name->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    mon_name->setAlignment(Qt::AlignLeft);
    mon_name->setToolTip(get_monster_description(m_ptr->r_idx, FALSE, NULL));
    mon_name->setFont(font_sidebar_window);

    mon_health_vlay->addWidget(mon_name);

    QLabel *mon_health = new QLabel;
    mon_health->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    QSize label_size = mon_name->sizeHint();

    label_size.setWidth(sidebar_widget->width());

    int w = label_size.width() * 9/10;
    int h = label_size.height() * 4 / 5;
    QPixmap mon_health_bar(w, h);

    QPainter painter(&mon_health_bar);
    QPen pen;
    mon_health_bar.fill(Qt::darkGray);

    int h2 = h;
    if (r_ptr->mana) h2 = h / 2;

    if (m_ptr->maxhp)
    {
        int w2 = w * m_ptr->hp / m_ptr->maxhp;
        w2 = MAX(w2, 1);
        int n = m_ptr->hp * 100 / m_ptr->maxhp;
        QString color("#00FF00");
        if (n <= 50) color = "red";
        else if (n < 100) color = "yellow";
        else if (m_ptr->m_timed[MON_TMD_SLEEP] > 0) color = "#000077";
        painter.fillRect(0, 0, w2, h2, color);
    }

    if (r_ptr->mana)
    {
        int w2 = w * m_ptr->mana / r_ptr->mana;
        w2 = MAX(w2, 1);
        painter.fillRect(0, h2, w2, h2, "purple");
    }

    QString status;
    QString tooltip;
    status.clear();
    tooltip.clear();

    if (m_ptr->m_timed[MON_TMD_CONF])
    {
        status.append("C");
        tooltip.append("Confused");
    }
    if (m_ptr->m_timed[MON_TMD_STUN])
    {
        status.append("s");
        if(tooltip.length()) tooltip.append(", ");
        tooltip.append("Stunned");
    }
    if (m_ptr->m_timed[MON_TMD_SLEEP])
    {
        status.append("Z");
        if(tooltip.length()) tooltip.append(", ");
        tooltip.append("Asleep");
    }
    if (m_ptr->m_timed[MON_TMD_FEAR])
    {
        status.append("A");
        if(tooltip.length()) tooltip.append(", ");
        tooltip.append("Afraid");
    }
    if ((m_ptr->m_timed[MON_TMD_FAST]) && (!m_ptr->m_timed[MON_TMD_SLOW]))
    {
        status.append("H");
        if(tooltip.length()) tooltip.append(", ");
        tooltip.append("Hasted");
    }
    else if ((m_ptr->m_timed[MON_TMD_SLOW]) && (!m_ptr->m_timed[MON_TMD_FAST]))
    {
        status.append("S");
        if(tooltip.length()) tooltip.append(", ");
        tooltip.append("Slowed");
    }

    if (status.length())
    {
        QFont font = ui_sidebar_window_font();
        font.setPointSize(h * 2 / 3);
        painter.setFont(font);
        painter.setPen(QPen(Qt::white, 1));
        painter.setOpacity(1);
        QRect rectangle = QRect(0,0,w,h);
        painter.drawText(0,0,w,h, Qt::AlignLeft, status, &rectangle);
    }
    mon_health->setToolTip(tooltip);

    mon_health->setPixmap(mon_health_bar);

    mon_health_vlay->addWidget(mon_health);
}


void MainWindow::update_sidebar_mon()
{
    update_mon_sidebar_list();

    // Clear all the old data
    QLayoutItem *item;
    while ((item = mon_health_vlay->takeAt(0)) != 0)
    {
        QWidget *wid = item->widget();
        if (wid) delete wid;
        delete item;
    }

    for (int i = 0; i < sidebar_monsters.size(); i++)
    {
       sidebar_display_mon(sidebar_monsters.at(i));
    }
}

// show all the sidebar labels
void MainWindow::show_sidebar()
{
    // Update the player sidebar info
    QList<QLabel *> lbl_list = sidebar_widget->findChildren<QLabel *>();

    for (int x = 0; x < lbl_list.size(); x++)
    {
        QLabel *this_lbl = lbl_list.at(x);
        this_lbl->show();
    }
}

//Hide all the labels
void MainWindow::hide_sidebar()
{
    // Update the player sidebar info
    QList<QLabel *> lbl_list = sidebar_widget->findChildren<QLabel *>();

    for (int x = 0; x < lbl_list.size(); x++)
    {
        QLabel *this_lbl = lbl_list.at(x);
        this_lbl->hide();
    }
}

/*
 * Prints "title", including "wizard" or "winner" as needed.
 */
static QString prt_title()
{
    QString p;

    /* Wizard */
    if (p_ptr->is_wizard)
    {
        p = "[=-WIZARD-=]";
    }

    /* Winner */
    else if (p_ptr->total_winner || (p_ptr->lev > z_info->max_level))
    {
        p = "***WINNER***";
    }

    /* Normal */
    else
    {
        p = get_player_title();
    }

    return p;
}

void MainWindow::update_titlebar()
{
    QString str("NPPGames");

    if (character_dungeon) {
        if (game_mode == GAME_NPPANGBAND)
        {
            str = "NPPAngband";
        }
        else
        {
            str = "NPPMoria";
        }

        str += " - Playing a level ";

        str += _num(p_ptr->lev);

        str += " ";

        str += rp_ptr->pr_name;

        str += " ";

        str += cp_ptr->cl_name;

        str += " - ";

        str += prt_title();

        if (p_ptr->is_wizard) str.append("  WIZARD MODE");
    }

    this->setWindowTitle(str);
}

