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
#include <QTableWidget>
#include <QHeaderView>
#include "tilebag.h"
#include <src/player_screen.h>
#include <src/utilities.h>

// Taken from 32 bit colors in defines.h
#define SBAR_NORMAL "#00FF00"
#define SBAR_DRAINED "#FFFF00"
#define SBAR_GOLD   "#FFD700"

#define SIDEBAR_MON_MAX     15


void MainWindow::create_sidebar()
{
    int row = 0;

    // Hitpoints
    QGridLayout *player_info = new QGridLayout;
    sidebar->addLayout(player_info);

    QLabel *hp_label = new QLabel;
    hp_label->setText(color_string("HP", SBAR_NORMAL));
    player_info->addWidget(hp_label, row, 0, Qt::AlignLeft);
    QLabel *hp_info = new QLabel;
    hp_info->setObjectName("HP");
    player_info->addWidget(hp_info, row++, 1, Qt::AlignRight);

    // SpellPoints
    QLabel *sp_label = new QLabel;
    sp_label->setText(color_string("SP", SBAR_NORMAL));
    player_info->addWidget(sp_label, row, 0, Qt::AlignLeft);
    sp_label->setObjectName("SP_LABEL");
    QLabel *sp_info = new QLabel;
    sp_info->setObjectName("SP_INFO");
    player_info->addWidget(sp_info, row++, 1, Qt::AlignRight);
    sp_label->hide();
    sp_info->hide();

    // Experience
    QLabel *exp_label = new QLabel;
    exp_label->setText(color_string("EXP", SBAR_NORMAL));
    exp_label->setObjectName("EXP_LABEL");
    player_info->addWidget(exp_label, row, 0, Qt::AlignLeft);
    QLabel *exp_info = new QLabel;
    exp_info->setObjectName("EXP_INFO");
    player_info->addWidget(exp_info, row++, 1, Qt::AlignRight);

    // gold
    QLabel *gold_label = new QLabel;
    gold_label->setText(color_string("GOLD", SBAR_GOLD));
    player_info->addWidget(gold_label, row, 0, Qt::AlignLeft);
    QLabel *gold_info = new QLabel;
    gold_info->setObjectName("GOLD");
    player_info->addWidget(gold_info, row++, 1, Qt::AlignRight);

    // stats
    for (int i = 0; i < A_MAX; i++)
    {
        QLabel *this_label = new QLabel;
        QString label_name = (QString("STAT_LABEL_%1") .arg(i));
        this_label->setObjectName(label_name);
        player_info->addWidget(this_label, row, 0, Qt::AlignLeft);
        QLabel *stat_info = new QLabel;
        stat_info->setObjectName(QString("STAT_INFO_%1") .arg(i));
        player_info->addWidget(stat_info, row++, 1, Qt::AlignRight);
    }

    // speed
    QLabel *speed_info = new QLabel;
    speed_info->setObjectName("SPEED");
    player_info->addWidget(speed_info, row++, 0, 1, 2, Qt::AlignLeft);

    // depth
    QLabel *depth_info = new QLabel;
    depth_info->setObjectName("DEPTH");
    player_info->addWidget(depth_info, row++, 0, 1, 2, Qt::AlignLeft);

    // feeling
    QLabel *feeling_info = new QLabel;
    feeling_info->setObjectName("FEELING");
    player_info->addWidget(feeling_info, row++, 0, 1, 2, Qt::AlignLeft);

    // quest
    QLabel *quest_info = new QLabel;
    quest_info->setObjectName("QUEST");
    player_info->addWidget(quest_info, row++, 0, 1, 2, Qt::AlignLeft);

    sidebar_mon = new QTableWidget;

    QPalette this_pal;
    this_pal.setColor(QPalette::Background, Qt::black);
    sidebar_mon->setAutoFillBackground(TRUE);
    sidebar_mon->setPalette(this_pal);

    sidebar_mon->insertColumn(0);
    sidebar_mon->verticalHeader()->setVisible(false);
    sidebar_mon->horizontalHeader()->setVisible(false);
    sidebar_mon->setShowGrid(false);
    sidebar_mon->setStyleSheet(QString("background-color: black; color: %1;").arg(SBAR_NORMAL));
    QFontMetrics metrics(sidebar_mon->font());
    sidebar_mon->setMinimumSize(metrics.width("MMMMMMMMMMMMMMMMMM"), 10);
    sidebar_mon->setEditTriggers(0);

    sidebar->addWidget(sidebar_mon);


    for (row = 0; row < SIDEBAR_MON_MAX; row++)
    {
        QWidget *wid = new QWidget;
        QGridLayout *lay = new QGridLayout;
        lay->setContentsMargins(2, 2, 2, 2);
        wid->setLayout(lay);
        QLabel *lb = new QLabel("");
        lb->setObjectName("tile");
        lay->addWidget(lb, 0, 0);
        lb = new QLabel("");
        lb->setObjectName("name");
        lb->setAlignment(Qt::AlignRight);
        lay->addWidget(lb, 0, 1);
        lb = new QLabel("");
        lb->setObjectName("health");
        lay->addWidget(lb, 1, 0, 1, 2);
        sidebar_mon->insertRow(row);
        sidebar_mon->setCellWidget(row, 0, wid);
        sidebar_mon->setRowHidden(row, true);
    }


}



bool my_less_than(const int &i1, const int &i2)
{
    return mon_list[i1].cdis < mon_list[i2].cdis;
}

QList<int> get_visible_monsters()
{
    QList<int> items;

    for (int y = p_ptr->py - MAX_SIGHT; y <= p_ptr->py + MAX_SIGHT; y++) {
        for (int x = p_ptr->px - MAX_SIGHT; x <= p_ptr->px + MAX_SIGHT; x++) {
            if (!in_bounds(y, x)) continue;
            int m_idx = dungeon_info[y][x].monster_idx;
            if (m_idx < 1) continue;
            monster_type *m_ptr = mon_list + m_idx;
            if (!m_ptr->ml) continue;
            items.append(m_idx);
        }
    }

    qSort(items.begin(), items.end(), my_less_than);

    return items;
}

static void display_mon(QTableWidget *sidebar_mon, int row, int m_idx)
{
    monster_type *m_ptr = mon_list + m_idx;
    monster_race *r_ptr = r_info + m_ptr->r_idx;

    QWidget *wid;
    wid = sidebar_mon->cellWidget(row, 0);
    QLabel *lb = wid->findChild<QLabel *>("tile");

    if (use_graphics && !main_window->do_pseudo_ascii)
    {
        QPixmap pix = current_tiles->get_tile(r_ptr->tile_id);
        pix = pix.scaled(24, 24);
        lb->setPixmap(pix);
    }
    else
    {
        lb->setText(r_ptr->d_char);
        lb->setStyleSheet(QString("color: %1;").arg(r_ptr->d_color.name()));
    }

    lb = wid->findChild<QLabel *>("name");
    lb->setText(r_ptr->r_name_short);

    int w = 100;
    int h = 6;
    QImage img(w, h, QImage::Format_ARGB32);
    QPainter p(&img);
    p.fillRect(0, 0, w, h, "black");

    int h2 = h;
    if (r_ptr->mana > 0) h2 = h / 2;

    if (m_ptr->maxhp > 0)
    {
        int w2 = w * m_ptr->hp / m_ptr->maxhp;
        w2 = MAX(w2, 1);
        int n = m_ptr->hp * 100 / m_ptr->maxhp;
        QString color("#00FF00");
        if (n <= 50) color = "red";
        else if (n < 100) color = "yellow";
        else if (m_ptr->m_timed[MON_TMD_SLEEP] > 0) color = "#000077";
        p.fillRect(0, 0, w2, h2, color);
    }

    if (r_ptr->mana > 0)
    {
        int w2 = w * m_ptr->mana / r_ptr->mana;
        w2 = MAX(w2, 1);
        p.fillRect(0, h2, w2, h2, "purple");
    }

    //Note monster conditions  - this doesn't work -  text comes out far too big.
    /*QString status;
    status.clear();
    if (m_ptr->m_timed[MON_TMD_CONF]) status.append("C");
    if (m_ptr->m_timed[MON_TMD_STUN]) status.append("s");
    if (m_ptr->m_timed[MON_TMD_SLEEP])status.append("Z");
    if (m_ptr->m_timed[MON_TMD_FEAR])status.append("A");
    if ((m_ptr->m_timed[MON_TMD_FAST]) && (!m_ptr->m_timed[MON_TMD_SLOW])) status.append("H");
    else if ((m_ptr->m_timed[MON_TMD_SLOW]) && (!m_ptr->m_timed[MON_TMD_FAST])) status.append("S");

    if (status.length())
    {
        QFont font = ui_current_font();
        font.setPointSize(h);
        p.setFont(font);
        p.setPen(QPen(Qt::white, 1));
        p.setOpacity(1);
        QRect rectangle = QRect(0,0,w,h);
        p.drawText(0,0,w,h, Qt::AlignLeft | Qt::AlignTop, status, &rectangle);
    }*/

    lb = wid->findChild<QLabel *>("health");
    lb->setPixmap(QPixmap::fromImage(img));

    sidebar_mon->setRowHidden(row, false);
    sidebar_mon->setRowHeight(row, wid->sizeHint().height() + 4);

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



void MainWindow::update_sidebar()
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
                    continue;
                }
            }
            // Update the hit points
            if (this_name.operator ==("HP"))
            {
                QString hp = QString("%1/%2").arg(p_ptr->chp).arg(p_ptr->mhp);

                int this_color = player_hp_attr();

                this_lbl->setText(color_string(hp, this_color));
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
                }
                else if (this_lbl->isVisible()) this_lbl->hide();
                continue;
            }

            if (this_name.operator ==("EXP_LABEL"))
            {
                bool max_lev = (p_ptr->lev == z_info->max_level);
                QColor this_color;
                QString this_text;
                if (p_ptr->exp < p_ptr->max_exp)
                {
                    this_color = SBAR_DRAINED;
                    this_text = (max_lev ? "Exp " : "Next ");
                }
                else
                {
                    this_color = SBAR_NORMAL;
                    this_text = (max_lev ? "EXP " : "NEXT ");
                }
                this_lbl->setText(color_string(this_text, this_color));
                continue;
            }
            if (this_name.operator ==("EXP_INFO"))
            {
                bool max_lev = (p_ptr->lev == z_info->max_level);
                QColor this_color;

                s32b xp = p_ptr->exp;
                if (!max_lev) xp = (get_experience_by_level(p_ptr->lev-1) * p_ptr->expfact / 100L) - p_ptr->exp;

                QString this_text = number_to_formatted_string(xp);

                if (p_ptr->exp < p_ptr->max_exp) this_color = SBAR_DRAINED;
                else this_color = SBAR_NORMAL;

                this_lbl->setText(color_string(this_text, this_color));
                continue;
            }
            if (this_name.operator ==("GOLD"))
            {
                QString gold = number_to_formatted_string(p_ptr->au);
                this_lbl->setText(color_string(gold, TERM_GOLD));
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
                }
                continue;
            }
        }
    }


    // MONSTERS

    for (int i = 0; i < SIDEBAR_MON_MAX; i++)
    {
        sidebar_mon->setRowHidden(i, true);
    }

    QList<int> monsters = get_visible_monsters();

    for (int i = 0; i < monsters.size(); i++)
    {
        if (i >= SIDEBAR_MON_MAX) break;

        int m_idx = monsters.at(i);

        display_mon(sidebar_mon, i, m_idx);
    }

    sidebar_mon->resizeColumnToContents(0);
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
        if (game_mode == GAME_NPPANGBAND) {
            str = "NPPAngband";
        }
        else {
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
