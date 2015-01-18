
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
#include <QDialogButtonBox>
#include <QPixmap>
#include <QPainter>
#include <QPlainTextEdit>
#include <QLabel>

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
    { "Strength:",          1, TR1_STR,         TR2_SUST_STR, TRUE, FALSE},
    { "Intelligence:",      1, TR1_INT,         TR2_SUST_INT, TRUE, FALSE},
    { "Wisdom:",            1, TR1_WIS,         TR2_SUST_WIS, TRUE, FALSE},
    { "Dexterity:",         1, TR1_DEX,         TR2_SUST_DEX, TRUE, FALSE},
    { "Constitution:",      1, TR1_CON,         TR2_SUST_CON, TRUE, FALSE},
    { "Charisma:",          1, TR1_CHR,         TR2_SUST_CHR, TRUE, FALSE},
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
static void make_standard_label(QLabel *this_label, QString title, byte preset_color)
{
    this_label->clear();
    this_label->setText(QString("<b>%1</b>") .arg(color_string(title, preset_color)));
    this_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

}

static void make_ability_graph(QLabel *this_label, int min, int max, int value)
{
    QFontMetrics metrics(ui_current_font());
    QSize this_size = metrics.size(Qt::TextSingleLine, "MMMMMMMMMMMMMM");
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
    if (stat == A_STR) return(QString("Strength affects total number of melee blows per round, melee to-hit, melee to damage,<br>player weight carrying capacity, maximum weildable weapon weight, bashing chances, throwing range, and digging capability."));
    if (stat == A_INT)
    {
        if (game_mode == GAME_NPPANGBAND) return(QString("Intelligence affects disarming skill, and magic device usage success rate.<br>For mages, druids, rangers, and rogues, intelligence affects maximum mana, spellcasting success rate, and numbers of spells the player can learn."));
        else return(QString("Intelligence affects disarming skill, and magic device usage success rate.<br>For mages, rangers, and rogues, intelligence affects maximum mana, spellcasting success rate, and numbers of spells the player can learn."));
    }
    if (stat == A_WIS)
    {
        if (game_mode == GAME_NPPANGBAND) return(QString("Wisdom affects saving throw.  For priests, druids, rangers, and paladins, wisdon affects their mana,<br>spellcasting success rate, and numbers of spells the player can learn."));
        else return(QString("Wisdom affects saving throw.  For priests and paladins, wisdon affects their mana, spellcasting success rate, and numbers of spells the player can learn."));
    }
    if (stat == A_DEX) return(QString("Dexterity affects total number of melee blows per round, melee to-hit, the player's armor class, disarming skill,<br>a monster's success rate in stealing from the player, and the possibility of getting stunned while bashing a door or creature."));
    if (stat == A_CON) return(QString("Constitution affects maximum hit points, hit point regeneration, and recovery rate from being stunned, cuts, and posion."));
    if (stat == A_CHR) return(QString("Charisma affects the prices for transactions in the stores, and the player's ability to sleep, stun, confuse, scare, and slow a monster."));
    //whoops!
    return(" ");
}


void PlayerScreenDialog::char_basic_info(QGridLayout *return_layout)
{
    int row = 0;
    int col = 0;

    // Add basic name
    QLabel *label_player_name = new QLabel;
    make_standard_label(label_player_name, "NAME:", TERM_DARK);
    QLabel *player_name = new QLabel;
    make_standard_label(player_name, op_ptr->full_name, TERM_BLUE);
    return_layout->addWidget(label_player_name, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_name, row++, col+1, Qt::AlignRight);

    // Add gender
    QLabel *label_player_gender = new QLabel;
    make_standard_label(label_player_gender, "GENDER:", TERM_DARK);
    label_player_gender->setToolTip(QString("Player gender does not have any significant gameplay effects."));
    QLabel *player_gender = new QLabel;
    make_standard_label(player_gender, sp_ptr->title, TERM_BLUE);
    return_layout->addWidget(label_player_gender, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_gender, row++, col+1, Qt::AlignRight);

    // Add race
    QLabel *label_player_race = new QLabel;
    make_standard_label(label_player_race, "RACE:", TERM_DARK);
    label_player_race->setToolTip(QString("Different player races have adjustments to stats and abilities.<br>Some races have innate intrinsic abilities."));
    QLabel *player_race = new QLabel;
    make_standard_label(player_race, p_info[p_ptr->prace].pr_name, TERM_BLUE);
    return_layout->addWidget(label_player_race, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_race, row++, col+1, Qt::AlignRight);

    // Add class
    QLabel *label_player_class = new QLabel;
    make_standard_label(label_player_class, "CLASS:", TERM_DARK);
    label_player_class->setToolTip(QString("Different player classes have adjustments to stats, abilities, and spellcasting abilities. and abilities.<br>Some classes have innate intrinsic abilities."));
    QLabel *player_class = new QLabel;
    make_standard_label(player_class, c_info[p_ptr->pclass].cl_name, TERM_BLUE);
    return_layout->addWidget(label_player_class, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_class, row++, col+1, Qt::AlignRight);

    // Add title
    QLabel *label_player_title = new QLabel;
    make_standard_label(label_player_title, "TITLE:", TERM_DARK);
    QLabel *player_title = new QLabel;
    QString title = get_player_title();
    if (p_ptr->is_wizard) title = "[=-WIZARD-=]";
    else if (p_ptr->total_winner)  title = "**WINNER**";
    make_standard_label(player_title, title, TERM_BLUE);
    return_layout->addWidget(label_player_title, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_title, row++, col+1, Qt::AlignRight);

    // Add hit points
    QLabel *label_player_hp = new QLabel;
    make_standard_label(label_player_hp, "HIT POINTS:", TERM_DARK);
    label_player_hp->setToolTip(QString("The player dies when thier hit points go below 0."));
    QLabel *player_hp = new QLabel;
    make_standard_label(player_hp, (QString("%1/%2") .arg(p_ptr->chp) .arg(p_ptr->mhp)), TERM_BLUE);
    return_layout->addWidget(label_player_hp, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_hp, row++, col+1, Qt::AlignRight);

    // Add spell points (if applicable)
    if (cp_ptr->spell_book)
    {
        QLabel *label_player_sp = new QLabel;
        make_standard_label(label_player_sp, "SPELL POINTS:", TERM_DARK);
        label_player_sp->setToolTip(QString("Spell points are required to cast spells."));
        QLabel *player_sp = new QLabel;
        make_standard_label(player_sp, (QString("%1/%2") .arg(p_ptr->csp) .arg(p_ptr->msp)), TERM_BLUE);
        return_layout->addWidget(label_player_sp, row, col, Qt::AlignLeft);
        return_layout->addWidget(player_sp, row++, col+1, Qt::AlignRight);
    }
    else row++;

    // Add fame or skip a space
    if (!adult_no_quests)
    {
        // Add fame
        QLabel *label_player_fame = new QLabel;
        make_standard_label(label_player_fame, "FAME:", TERM_DARK);
        label_player_fame->setToolTip(QString("Player fame is gained by killing Unique creatures and completing quests.<br>The Adventurer's Guild offers better quest rewards and services as a player's game gets higher."));
        QLabel *player_fame = new QLabel;
        make_standard_label(player_fame, (QString("%1") .arg(p_ptr->q_fame)), TERM_BLUE);
        return_layout->addWidget(label_player_fame, row, col, Qt::AlignLeft);
        return_layout->addWidget(player_fame, row++, col+1, Qt::AlignRight);
    }
    else row++;

    // Add Player Gold
    QLabel *label_player_gold = new QLabel;
    make_standard_label(label_player_gold, "GOLD:", TERM_DARK);
    QLabel *player_gold = new QLabel;
    make_standard_label(player_gold, (QString("%1") .arg(number_to_formatted_string(p_ptr->au))), TERM_BLUE);
    return_layout->addWidget(label_player_gold, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_gold, row++, col+1, Qt::AlignRight);

    QLabel *filler = new QLabel("  ");
    return_layout->addWidget(filler, 0, col + 2);

    return_layout->addItem(new QSpacerItem(1,1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding), row, 0);

}

void PlayerScreenDialog::char_basic_data(QGridLayout *return_layout)
{
    int row = 0;
    int col = 0;

    // Add age
    QLabel *label_player_age = new QLabel;
    make_standard_label(label_player_age, "AGE:", TERM_DARK);
    label_player_age->setToolTip("Player age does not have any significant gameplay effects.");
    QLabel *player_age = new QLabel;
    make_standard_label(player_age, (QString("%1") .arg(p_ptr->age)), TERM_BLUE);
    return_layout->addWidget(label_player_age, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_age, row++, col+1, Qt::AlignRight);

    // Add Height
    QLabel *label_player_height = new QLabel;
    make_standard_label(label_player_height, "HEIGHT:", TERM_DARK);
    label_player_height->setToolTip("Player height does not have any significant gameplay effects.");
    QLabel *player_height = new QLabel;
    make_standard_label(player_height, (QString("%1") .arg(p_ptr->ht)), TERM_BLUE);
    return_layout->addWidget(label_player_height, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_height, row++, col+1, Qt::AlignRight);

    // Add Weight
    QLabel *label_player_weight = new QLabel;
    make_standard_label(label_player_weight, "WEIGHT:", TERM_DARK);
    label_player_weight->setToolTip("Player weight does not have any significant gameplay effects.");
    QLabel *player_weight = new QLabel;
    make_standard_label(player_weight, (QString("%1") .arg(p_ptr->wt)), TERM_BLUE);
    return_layout->addWidget(label_player_weight, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_weight, row++, col+1, Qt::AlignRight);

    // Add Social Class
    QLabel *label_player_sc = new QLabel;
    make_standard_label(label_player_sc, "SOCIAL CLASS:", TERM_DARK);
    label_player_sc->setToolTip("Player social class does not have any significant gameplay effects.");
    QLabel *player_sc = new QLabel;
    make_standard_label(player_sc, (QString("%1") .arg(p_ptr->sc)), TERM_BLUE);
    return_layout->addWidget(label_player_sc, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_sc, row++, col+1, Qt::AlignRight);

    // Add Game Turn
    QLabel *label_player_gturn = new QLabel;
    make_standard_label(label_player_gturn, "GAME TURN:", TERM_DARK);
    label_player_gturn->setToolTip(QString("10 game turns is equivalent to one player turn at normal speed.<br>The player's score is eventually reduced as turns get greater."));
    QLabel *player_gturn = new QLabel;
    make_standard_label(player_gturn, (QString("%1") .arg(number_to_formatted_string(turn))), TERM_BLUE);
    return_layout->addWidget(label_player_gturn, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_gturn, row++, col+1, Qt::AlignRight);

    // Add Player Turn
    QLabel *label_player_pturn = new QLabel;
    make_standard_label(label_player_pturn, "PLAYER TURN:", TERM_DARK);
    label_player_pturn->setToolTip(QString("Tracks the number of player turns, independent of player speed."));
    QLabel *player_pturn = new QLabel;
    make_standard_label(player_pturn, (QString("%1") .arg(number_to_formatted_string(p_ptr->p_turn))), TERM_BLUE);
    return_layout->addWidget(label_player_pturn, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_pturn, row++, col+1, Qt::AlignRight);

    // Add Max Depth
    QLabel *label_player_max_depth = new QLabel;
    make_standard_label(label_player_max_depth, "MAX DEPTH:", TERM_DARK);
    label_player_max_depth->setToolTip(QString("The deepest dungeon depth the player has reached."));
    QLabel *player_max_depth = new QLabel;
    QString max_depth = (QString("%1") .arg(p_ptr->max_depth * 50));
    if (!p_ptr->max_depth) max_depth = "TOWN";
    make_standard_label(player_max_depth, (QString("%1")) .arg(max_depth), TERM_BLUE);
    return_layout->addWidget(label_player_max_depth, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_max_depth, row++, col+1, Qt::AlignRight);

    //Infravision
    QLabel *label_player_infra = new QLabel;
    make_standard_label(label_player_infra, "INFRAVISION:", TERM_DARK);
    label_player_infra->setToolTip(QString("Number of feet outside the light radius where player can see warm-blooded creatures.  10 feet = 1 square."));
    QLabel *player_infra = new QLabel;
    QString infra_string = "NONE";
    if (p_ptr->state.see_infra) infra_string = (QString("%1 feet") .arg(p_ptr->state.see_infra * 10));
    make_standard_label(player_infra, infra_string, TERM_BLUE);
    return_layout->addWidget(label_player_infra, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_infra, row++, col+1, Qt::AlignRight);

    QLabel *filler = new QLabel("  ");
    return_layout->addWidget(filler, 0, col + 2);

    return_layout->addItem(new QSpacerItem(1,1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding), row, 0);

}

void PlayerScreenDialog::char_game_info(QGridLayout *return_layout)
{
    int row = 0;
    int col = 0;

    // Add Character Level
    QLabel *label_player_lev = new QLabel;
    make_standard_label(label_player_lev, "CHAR. LEVEL:", TERM_DARK);
    label_player_lev->setToolTip(QString("Current character level.<br>Player's hit points, spell points and abilities increase as their level increases."));
    QLabel *player_lev = new QLabel;
    make_standard_label(player_lev, (QString("%1") .arg(p_ptr->lev)), (p_ptr->lev >= p_ptr->max_lev) ? TERM_BLUE : TERM_RED);
    return_layout->addWidget(label_player_lev, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_lev, row++, col+1, Qt::AlignRight);

    // Add Player Experience
    QLabel *label_player_exp = new QLabel;
    make_standard_label(label_player_exp, "EXPERIENCE:", TERM_DARK);
    label_player_exp->setToolTip(QString("Total current player experience."));
    QLabel *player_exp = new QLabel;
    make_standard_label(player_exp, (QString("%1") .arg(number_to_formatted_string(p_ptr->exp))), (p_ptr->exp >= p_ptr->max_exp) ? TERM_BLUE : TERM_RED);
    return_layout->addWidget(label_player_exp, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_exp, row++, col+1, Qt::AlignRight);

    // Add Player Maximum Experience
    QLabel *label_player_max_exp = new QLabel;
    make_standard_label(label_player_max_exp, "MAX EXP:", TERM_DARK);
    label_player_max_exp->setToolTip(QString("Maximum player experience.<br>Maximum experience increases at 10 percent of the normal rate when player experience is drained."));
    QLabel *player_max_exp = new QLabel;
    make_standard_label(player_max_exp, (QString("%1") .arg(number_to_formatted_string(p_ptr->max_exp))), TERM_BLUE);
    return_layout->addWidget(label_player_max_exp, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_max_exp, row++, col+1, Qt::AlignRight);

    // Add Experience to Advance
    if (p_ptr->lev < z_info->max_level)
    {
        s32b advance = (get_experience_by_level(p_ptr->lev-1) * p_ptr->expfact / 100L);
        s32b exp_needed = advance - p_ptr->exp;
        QString exp_output = number_to_formatted_string(exp_needed);;

        QLabel *label_player_exp_adv = new QLabel;
        make_standard_label(label_player_exp_adv, "ADVANCE EXP:", TERM_DARK);
        QLabel *player_exp_adv = new QLabel;
        make_standard_label(player_exp_adv, (QString("%1") .arg(exp_output)), TERM_BLUE);
        player_exp_adv->setToolTip(QString("The total experience needed for the next level is %1") .arg(number_to_formatted_string(advance)));
        return_layout->addWidget(label_player_exp_adv, row, col, Qt::AlignLeft);
        return_layout->addWidget(player_exp_adv, row++, col+1, Qt::AlignRight);
    }
    else row++;

    // Add player score
    QLabel *label_player_score = new QLabel;
    make_standard_label(label_player_score, "SCORE:", TERM_DARK);
    //label_player_score->setToolTip(QString("An attempt to quantify the player accomplishments.<br>Does not have any gameplay effects.<br>Factors in uniques killed, artifats found, player fame, experience gained, how deep in the dungeon the player has gone, and total game turns."));
    QLabel *player_score = new QLabel;
    make_standard_label(player_score, (QString("%1") .arg(number_to_formatted_string(p_ptr->current_score))), TERM_BLUE);
    return_layout->addWidget(label_player_score, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_score, row++, col+1, Qt::AlignRight);

    // Add Burden
    QLabel *label_player_burden = new QLabel;
    make_standard_label(label_player_burden, "BURDEN:", TERM_DARK);
    label_player_burden->setToolTip("Total weight of all the player's equipment and inventory.");
    QLabel *player_burden = new QLabel;
    make_standard_label(player_burden, (QString("%1 lbs") .arg(formatted_weight_string(p_ptr->total_weight))), TERM_BLUE);
    return_layout->addWidget(label_player_burden, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_burden, row++, col+1, Qt::AlignRight);

    // Add Max Burden
    QLabel *label_player_burden_max = new QLabel;
    make_standard_label(label_player_burden_max, "MAX WEIGHT:", TERM_DARK);
    QLabel *player_burden_max = new QLabel;
    make_standard_label(player_burden_max, (QString("%1 lbs") .arg(formatted_weight_string(normal_speed_weight_limit()))), TERM_BLUE);
    label_player_burden_max->setToolTip("Max weight is the total weight you can carry without being slowed by the burden.");
    return_layout->addWidget(label_player_burden_max, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_burden_max, row++, col+1, Qt::AlignRight);

    // Add Burden %
    QLabel *label_player_burden_pct = new QLabel;
    make_standard_label(label_player_burden_pct, "% BURDEN:", TERM_DARK);
    label_player_burden_pct->setToolTip(QString("The player suffers from reduced speed when this percentage goes above 100."));
    QLabel *player_burden_pct = new QLabel;
    int pct = (p_ptr->total_weight * 100) / normal_speed_weight_limit();
    make_standard_label(player_burden_pct, (QString("%1%")) .arg(pct), (pct <= 100 ? TERM_BLUE : TERM_RED));
    return_layout->addWidget(label_player_burden_pct, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_burden_pct, row++, col+1, Qt::AlignRight);

    QLabel *filler = new QLabel("  ");
    return_layout->addWidget(filler, 0, col + 2);

    return_layout->addItem(new QSpacerItem(1,1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding), row, 0);
}

void PlayerScreenDialog::char_stat_info(QGridLayout *stat_layout)
{
    // add the headers
    byte row = 0;
    byte col = 0;

    QLabel *stat_header = new QLabel();
    make_standard_label(stat_header, "STAT   ", TERM_DARK);
    QLabel *self_header = new QLabel();
    make_standard_label(self_header, " SELF ", TERM_DARK);
    QLabel *race_adj_header = new QLabel();
    make_standard_label(race_adj_header, "  RA ", TERM_DARK);
    race_adj_header->setToolTip("Stat adjustments due to player race");
    QLabel *class_adj_header = new QLabel();
    make_standard_label(class_adj_header, "  CA ", TERM_DARK);
    class_adj_header->setToolTip("Stat adjustments due to player class");
    QLabel *equip_adj_header = new QLabel();
    make_standard_label(equip_adj_header, "  EA ", TERM_DARK);
    equip_adj_header->setToolTip("Stat adjustments due to player equipment");
    QLabel *reward_adj_header = new QLabel();
    make_standard_label(reward_adj_header, " QA ", TERM_DARK);
    reward_adj_header->setToolTip("Stat adjustments due to quest rewards");
    QLabel *total_stat_header = new QLabel();
    make_standard_label(total_stat_header, "  TOTAL STAT", TERM_DARK);
    stat_layout->addWidget(stat_header, row, col++, Qt::AlignLeft);
    stat_layout->addWidget(self_header, row, col++, Qt::AlignLeft);
    if (adult_maximize) stat_layout->addWidget(race_adj_header, row, col++, Qt::AlignRight);
    if (adult_maximize) stat_layout->addWidget(class_adj_header, row, col++, Qt::AlignRight);
    stat_layout->addWidget(equip_adj_header, row, col++, Qt::AlignRight);
    if (!adult_no_quests) stat_layout->addWidget(reward_adj_header, row, col++, Qt::AlignRight);
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
        make_standard_label(self_label, (QString("%1 ") .arg(p_ptr->stat_max[i])), TERM_BLUE);
        stat_layout->addWidget(self_label, row, col++, Qt::AlignLeft);

        if (adult_maximize)
        {
            QLabel *race_adj = new QLabel();
            make_standard_label(race_adj, (QString("%1 ") .arg(rp_ptr->r_adj[i])), TERM_BLUE);
            stat_layout->addWidget(race_adj, row, col++, Qt::AlignRight);

            QLabel *class_adj = new QLabel();
            make_standard_label(class_adj, (QString("%1 ") .arg(cp_ptr->c_adj[i])), TERM_BLUE);
            stat_layout->addWidget(class_adj, row, col++, Qt::AlignRight);
        }

        QLabel *equip_adj = new QLabel();
        make_standard_label(equip_adj, (QString("%1 ") .arg(p_ptr->state.stat_add[i])), TERM_BLUE);
        stat_layout->addWidget(equip_adj, row, col++, Qt::AlignRight);

        if (!adult_no_quests)
        {
            QLabel *quest_adj = new QLabel();
            make_standard_label(quest_adj, (QString("%1 ") .arg(p_ptr->stat_quest_add[i])), TERM_BLUE);
            stat_layout->addWidget(quest_adj, row, col++, Qt::AlignRight);
        }

        QLabel *stat_total = new QLabel();
        make_standard_label(stat_total, (QString("  %1 ") .arg(cnv_stat(p_ptr->state.stat_top[i]))), TERM_BLUE);
        stat_layout->addWidget(stat_total, row, col++, Qt::AlignLeft);

        //Display reduced stat if necessary
        bool need_display = FALSE;
        if (p_ptr->state.stat_use[i] < p_ptr->state.stat_top[i]) need_display = TRUE;
        QString lower_stat = cnv_stat(p_ptr->state.stat_use[i]);
        if (!need_display) lower_stat = "       ";
        QLabel *stat_reduce = new QLabel();
        make_standard_label(stat_reduce, lower_stat, need_display ? TERM_RED : TERM_BLUE);
        stat_layout->addWidget(stat_reduce, row++, col++, Qt::AlignRight);
    }

    stat_layout->addItem(new QSpacerItem(1,1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding), row, 0);
}

void PlayerScreenDialog::char_combat_info(QGridLayout *return_layout)
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
    label_player_speed->setToolTip(QString("Player rate of speed, without any temporary effects.  This has a dramatic effect on player power and survivability."));
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
    return_layout->addWidget(label_player_speed, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_speed, row++, col+1, Qt::AlignRight);

    // Add armor class
    QLabel *label_player_armor = new QLabel;
    make_standard_label(label_player_armor, "ARMOR:", TERM_DARK);
    //label_player_armor->setToolTip(QString("Known Armor class comes from dexterity and equipment (does not factor in changes from unidentified equipment).<br>The format is: [Intrinsic AC, dexterity and magical bonuses to AC]."));
    QLabel *player_armor = new QLabel;
    first_num = p_ptr->state.dis_ac;
    second_num = p_ptr->state.dis_to_a;
    first_num_string.setNum(first_num);
    second_num_string.setNum(second_num);
    if (second_num < 0) second_num_string.prepend("-");
    else second_num_string.prepend("+");
    make_standard_label(player_armor, (QString("[%1, %2]") .arg(first_num_string) .arg(second_num_string)), TERM_BLUE);
    return_layout->addWidget(label_player_armor, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_armor, row++, col+1, Qt::AlignRight);

    // Melee Weapon Stats
    QLabel *label_player_melee = new QLabel;
    make_standard_label(label_player_melee, "MELEE:", TERM_DARK);
    //label_player_melee->setToolTip(QString("Known Melee statistics comes from strength, dexterity, player class, and equipment (does not factor in changes from unidentified equipment).<br>x(number attacks), [damage dice, damage sides], (known-to-hit-adj., known-to-damage-adj).<br>Examine weapon for more detailed information."));
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
    QLabel *player_crit_hit = new QLabel;
    first_num = critical_hit_chance(o_ptr, p_ptr->state, TRUE) /  (CRIT_HIT_CHANCE / 100);
    first_num_string.setNum(first_num);
    make_standard_label(player_crit_hit, (QString("%1 %") .arg(first_num_string)), TERM_BLUE);
    label_player_crit_hit->setToolTip(QString("Percent chance of extra damage , based on weapon weight, player melee skill, and to-hit bonus"));
    return_layout->addWidget(label_player_crit_hit, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_crit_hit, row++, col+1, Qt::AlignRight);

    //Shooting weapon stats
    QLabel *label_player_shoot = new QLabel;
    make_standard_label(label_player_shoot, "SHOOT:", TERM_DARK);
    label_player_shoot->setToolTip(QString("Known shooting statistics comes from dexterity, player class, and equipment (does not factor in changes from unidentified equipment).<br>Format is: x(num shots) x(damage multiplier), [damage dice, damage sides], (known-to-hit-adj., known-to-damage-adj).<br>Examine bow or ammunition for more detailed information."));
    QLabel *player_shoot = new QLabel;

    //Make sure we are factoring in the bow and not a swap weapon
    if (adult_swap_weapons)
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
    return_layout->addWidget(label_player_shoot, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_shoot, row++, col+1, Qt::AlignRight);

    // Searching frequency - frequency is inverted
    QLabel *label_player_search_freq = new QLabel;
    make_standard_label(label_player_search_freq, "SEARCH FREQ:", TERM_DARK);
    label_player_search_freq->setToolTip(QString("Affected by player equipment race, class and level.<br>Chance of searching each player turn."));
    QLabel *player_search_freq = new QLabel;
    if (p_ptr->state.skills[SKILL_SEARCH_FREQUENCY] > SEARCH_CHANCE_MAX) first_num_string = "1 in 1";
    else first_num_string = (QString("1 in %1") .arg(SEARCH_CHANCE_MAX - p_ptr->state.skills[SKILL_SEARCH_FREQUENCY]));
    make_standard_label(player_search_freq, first_num_string, TERM_BLUE);
    return_layout->addWidget(label_player_search_freq, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_search_freq, row++, col+1, Qt::AlignRight);

    // Searching chance
    QLabel *label_player_search_chance = new QLabel;
    make_standard_label(label_player_search_chance, "SEARCH CHANCE:", TERM_DARK);
    label_player_search_chance->setToolTip(QString("Affected by player equipment race, class and level.<br>Percent chance of, if searching, noticing any hidden doors or traps that are within 10 feet of player.<br>Also affects how quickly the player gets a feeling about the level they are on."));
    QLabel *player_search_chance = new QLabel;
    if (p_ptr->state.skills[SKILL_SEARCH_CHANCE] > 100) first_num = 100;
    else first_num = p_ptr->state.skills[SKILL_SEARCH_CHANCE];
    first_num_string = (QString("%1%") .arg(first_num));
    make_standard_label(player_search_chance, first_num_string, TERM_BLUE);
    return_layout->addWidget(label_player_search_chance, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_search_chance, row++, col+1, Qt::AlignRight);

    //Digging
    QLabel *label_player_dig = new QLabel;
    make_standard_label(label_player_dig, "TUNNEL:", TERM_DARK);
    label_player_dig->setToolTip(QString("Based on player strength, weapon weight, and equipment bonuses.<br>Improves chance of sucessfully tunneling through a walls.<br>Actual chance of success depends on the terrain into which the player is tunneling."));
    QLabel *player_dig = new QLabel;
    first_num_string.setNum(p_ptr->state.skills[SKILL_DIGGING]);
    make_standard_label(player_dig, first_num_string, TERM_BLUE);
    return_layout->addWidget(label_player_dig, row, col, Qt::AlignLeft);
    return_layout->addWidget(player_dig, row++, col+1, Qt::AlignRight);

    QLabel *filler = new QLabel("  ");
    return_layout->addWidget(filler, 0, col + 2);

    return_layout->addItem(new QSpacerItem(1,1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding), row, 0);
}

void PlayerScreenDialog::char_ability_info(QGridLayout *return_layout)
{
    int row = 0;
    int col = 0;

    // Add Speed
    QLabel *label_player_speed = new QLabel;
    make_standard_label(label_player_speed, "SPEED:", TERM_DARK);
    label_player_speed->setToolTip(QString("Player current energy gain, with temporary effects, relative to the hightst and lowest possible energy gains."));
    return_layout->addWidget(label_player_speed, row, col, Qt::AlignLeft);
    QLabel *player_speed = new QLabel;
    make_ability_graph(player_speed, 0, pam_ptr->max_p_speed, calc_energy_gain(p_ptr->state.p_speed));
    return_layout->addWidget(player_speed, row++, col+1);

    // Add Saving Throw
    QLabel *label_player_save = new QLabel;
    make_standard_label(label_player_save, "SAVING THROW:", TERM_DARK);
    label_player_save->setToolTip(QString("Current Saving throw percentage against some monster magical attacks such as fear, paralysis, slowness, blindness, and confusion.<br>It is based on player race, class, level, and wisdom.<br>Note some monster attacks do not allow a saving throw, so even a perfect saving throw doesn't offer complete protection from these side effects."));
    return_layout->addWidget(label_player_save, row, col, Qt::AlignLeft);
    QLabel *player_save = new QLabel;
    make_ability_graph(player_save, 0, 100, p_ptr->state.skills[SKILL_SAVE]);
    return_layout->addWidget(player_save, row++, col+1);

    // Add Stealth - note special handling since stealth is inverted
    QLabel *label_player_stealth = new QLabel;
    make_standard_label(label_player_stealth, "STEALTH:", TERM_DARK);
    label_player_stealth->setToolTip(QString("The better the stealth, the longer it takes monsters to wake up.<br>The player can be extremely quiet, but can not achieve perfect silence."));
    return_layout->addWidget(label_player_stealth, row, col, Qt::AlignLeft);
    QLabel *player_stealth = new QLabel;
    int stealth = (WAKEUP_MAX - p_ptr->base_wakeup_chance);
    if (p_ptr->state.aggravate) stealth = WAKEUP_MAX;
    make_ability_graph(player_stealth, WAKEUP_MIN, WAKEUP_MAX, stealth);
    if (p_ptr->state.aggravate) player_stealth->setToolTip(QString("You are wearing equipment that aggravates monsters."));
    return_layout->addWidget(player_stealth, row++, col+1);

    // Add Fighting ability
    QLabel *label_player_fight = new QLabel;
    make_standard_label(label_player_fight, "FIGHTING:", TERM_DARK);
    label_player_fight->setToolTip(QString("Fighting ability is based on player race, class, and level.  It is a significant factor in making a weapon attack sucessful, and for getting a critical hit.<br>Critical hits increase total damage dice and +to-damage, and cause multiple damage rolls with the hightest calculated damage used.<br>In terms of total damage inflicted, this factor is significantly less important than # of attacks per round, player speed, player to-damage, and quality of weapon.<br>More details of total potential weapon damage can be found by inspecting the weapon."));
    return_layout->addWidget(label_player_fight, row, col, Qt::AlignLeft);
    QLabel *player_fight = new QLabel;
    make_ability_graph(player_fight, 0, pam_ptr->max_skills[SKILL_TO_HIT_MELEE], p_ptr->state.skills[SKILL_TO_HIT_MELEE]);
    return_layout->addWidget(player_fight, row++, col+1);

    // Add bow ability
    QLabel *label_player_bow = new QLabel;
    make_standard_label(label_player_bow, "SHOOTING:", TERM_DARK);
    label_player_bow->setToolTip(QString("Shooting ability is based on player race, class, and level.  It is a significant factor in making a sucessful hit, and for getting a critical hit.<br>Critical hits increase total damage dice and +to-damage, and cause multiple damage rolls with the hightest calculated damage used.<br>In terms of total damage inflicted, this factor is significantly less important than shooting speed, player speed, bow multiplier, the quality of missile launcher, and the quality of ammunition fired.<br>More details of total potential shooting damage can be found by inspecting ammunition.<br>Rogues, Brigands, and Rangers get significantly boosted shooting abilities as they advance levels."));
    return_layout->addWidget(label_player_bow, row, col, Qt::AlignLeft);
    QLabel *player_bow = new QLabel;
    make_ability_graph(player_bow, 0, pam_ptr->max_skills[SKILL_TO_HIT_BOW], p_ptr->state.skills[SKILL_TO_HIT_BOW]);
    return_layout->addWidget(player_bow, row++, col+1);

    // Add throwing ability
    QLabel *label_player_throw = new QLabel;
    make_standard_label(label_player_throw, "THROWING:", TERM_DARK);
    label_player_throw->setToolTip(QString("Throwing ability is based on player race, class, and level.  It is a significant factor in making the throw a sucessful hit, and for getting a critical hit.<br>Critical hits increase total damage dice and +to-damage, and cause multiple damage rolls with the hightest calculated damage used.<br>In terms of total damage inflicted, this factor is significantly less important than the qualitiess of the item thrown.<br>More details of total potential throwing damage can be found by inspecting the item.<br>Some potions can have interesting side effects when thrown.<br>Rogues and Brigands get significantly boosted throwing abilities as they advance levels."));
    return_layout->addWidget(label_player_throw, row, col, Qt::AlignLeft);
    QLabel *player_throw = new QLabel;
    make_ability_graph(player_throw, 0, pam_ptr->max_skills[SKILL_TO_HIT_THROW], p_ptr->state.skills[SKILL_TO_HIT_THROW]);
    return_layout->addWidget(player_throw, row++, col+1);

    // Add disarming ability
    QLabel *label_player_disarm = new QLabel;
    make_standard_label(label_player_disarm, "DISARMING:", TERM_DARK);
    label_player_disarm->setToolTip(QString("Disarming ability is based on player race, class, dexterity, and level.<br>It is used to determine if a player has sucessfully disarmed traps or picked locks."));
    return_layout->addWidget(label_player_disarm, row, col, Qt::AlignLeft);
    QLabel *player_disarm = new QLabel;
    make_ability_graph(player_disarm, 0, pam_ptr->max_skills[SKILL_DISARM], p_ptr->state.skills[SKILL_DISARM]);
    return_layout->addWidget(player_disarm, row++, col+1);

    // Add magic device
    QLabel *label_player_magic = new QLabel;
    make_standard_label(label_player_magic, "MAGIC DEVICE:", TERM_DARK);
    label_player_magic->setToolTip(QString("Magic usage is used in determining if the player has sucessfully activated an object, or used a wand, rod, or staff.<br>It is based on player race, class, intelligence, and level.<br>Object or atifact level also factors into the equation."));
    return_layout->addWidget(label_player_magic, row, col, Qt::AlignLeft);
    QLabel *player_magic = new QLabel;
    make_ability_graph(player_magic, 0, pam_ptr->max_skills[SKILL_DEVICE], p_ptr->state.skills[SKILL_DEVICE]);
    return_layout->addWidget(player_magic, row++, col+1);

    QLabel *filler = new QLabel("  ");
    return_layout->addWidget(filler, 0, col + 2);

    return_layout->addItem(new QSpacerItem(1,1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding), row, 0);
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
        else if (flag_set == FLAGS_NATIVITY) pfr_ptr = &player_nativity_table[x++];

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
                    make_standard_label(immune_label, "☑", TERM_BLUE);
                    return_layout->addWidget(immune_label, row, col, Qt::AlignCenter);

                    // Too messy to inlude in player_flag_table
                    if (pfr_ptr->extra_flag == TR2_RES_ACID) immune_label->setToolTip(QString("Acid immunity means acid based spells do not damage the player or the player's equipment and inventory."));
                    else if (pfr_ptr->extra_flag == TR2_IM_ELEC) immune_label->setToolTip(QString("Lightning immunity means lightning based spells do not damage the player or the player's inventory."));
                    else if (pfr_ptr->extra_flag == TR2_IM_FIRE) immune_label->setToolTip(QString("Fire immunity means fire based spells do not damage the player or the player's inventory."));
                    else if (pfr_ptr->extra_flag == TR2_IM_COLD) immune_label->setToolTip(QString("Cold immunity means cold or ice based spells do not damage the player or the player's inventory."));
                    else if (pfr_ptr->extra_flag == TR2_IM_POIS) immune_label->setToolTip(QString("Poison immunity means poison based spells do not damage the player."));

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
            make_standard_label(resist_label, "☑", TERM_GREEN);
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
            make_standard_label(player_label, "☑", TERM_BLUE);
            return_layout->addWidget(player_label, row, col, Qt::AlignCenter);
            did_resist = TRUE;
        }
        if (player_has_immunity)
        {
            QLabel *player_label = new QLabel;
            make_standard_label(player_label, "☑", TERM_GREEN);
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
                    make_standard_label(temp_label, "☑", TERM_PURPLE);
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
                    make_standard_label(temp_label, "☑", TERM_PURPLE);
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
                    make_standard_label(temp_label, "☑", TERM_PURPLE);
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
                    make_standard_label(temp_label, "☑", TERM_PURPLE);
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
                    make_standard_label(temp_label, "☑", TERM_PURPLE);
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
                    make_standard_label(temp_label, "☑", TERM_PURPLE);
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
                    make_standard_label(temp_label, "☑", TERM_PURPLE);
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
                    make_standard_label(temp_label, "☑", TERM_PURPLE);
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
                    make_standard_label(temp_label, "☑", TERM_PURPLE);
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
                    make_standard_label(temp_label, "☑", TERM_PURPLE);
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
                    make_standard_label(temp_label, "☑", TERM_PURPLE);
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
                    make_standard_label(temp_label, "☑", TERM_PURPLE);
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
                    make_standard_label(temp_label, "☑", TERM_PURPLE);
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
            if (pfr_ptr->this_flag == TR2_RES_ACID) line_label->setToolTip(QString("Acid attacks have a maximum damage of 1,600 hp, and can harm player inventory and equipment.<br>Having temporary or permanent resistance divides damage by 3, and helps protect equipment and inventory.<br>Having both permanent and temporary resist divides damage by 9, and gives further protection to equipment and inventory."));
            else if (pfr_ptr->this_flag == TR2_RES_ELEC) line_label->setToolTip(QString("Electricity attacks have a maximum damage of 1,600 hp, and can destroy rings, wands, and rods in the player inventory.<br>Having temporary or permanent resistance divides damage by 3, and helps protect player inventory.<br>Having both permanent and temporary resist divides damage by 9, and gives further protection to player inventory."));
            else if (pfr_ptr->this_flag == TR2_RES_FIRE) line_label->setToolTip(QString("Fire attacks have a maximum damage of 1,600 hp, and can destroy wearable equipment, destroy spellbooks, chests, staffs, and scrolls in the player inventory.<br>Having temporary or permanent resistance divides damage by 3, and helps protect player inventory.<br>Having both permanent and temporary resist divides damage by 9, and gives further protection to player inventory."));
            else if (pfr_ptr->this_flag == TR2_RES_COLD) line_label->setToolTip(QString("Ice and Cold attacks have a maximum damage of 1,600 hp, and can destroy potions, flasks, and bottles in the player inventory.<br>Having temporary or permanent resistance divides damage by 3, and helps protect player inventory.<br>Having both permanent and temporary resist divides damage by 9, and gives further protection to player inventory."));
            else if (pfr_ptr->this_flag == TR2_RES_POIS) line_label->setToolTip(QString("Poison attacks have a maximum damage of 800 hp, and can pioson the player inventory.<br>Having temporary or permanent resistance divides damage by 3, and prevents the player from being poisoned.<br>Having both permanent and temporary resist divides damage by 9, and gives further protection to player inventory."));
            else if (pfr_ptr->this_flag == TR2_RES_BLIND) line_label->setToolTip(QString("Resist blindness prevents the player from being blinded.<br>Blindness prevents the player from seeing, reading scrolls and casting spells."));
            else if (pfr_ptr->this_flag == TR2_RES_CONFU) line_label->setToolTip(QString("Confusion attacks have a maximum damage of 400, and can cause the player to be confused.<br>Resist confusion prevents the player from being confused, and reduces damage from confusion attacks by between 28 and 58 percent--damage=(damage*5)/(6+1d6)"));
            else if (pfr_ptr->this_flag == TR2_RES_NEXUS) line_label->setToolTip(QString("Nexus attacks have a maximum damage of 450, and can cause the player to be teleported or teleported off the level, or the player can permanently have two of their stats switched.<br>Resist nexus prevents the nexus side effects, and reduces damage from nexus spells by between 14 and 50 percent--damage=(damage*6)/(6+1d6)"));
            else if (pfr_ptr->this_flag == TR2_RES_NETHR) line_label->setToolTip(QString("Nether attacks have a maximum damage of 450, and can drain the player's experience.<br>Resist nether prevents prevents experience drain from nether and chaos attacks, and reduces damage from nether spells by between 14 and 50 percent--damage=(damage*6)/(6+1d6)"));
            else if (pfr_ptr->this_flag == TR2_RES_CHAOS) line_label->setToolTip(QString("Chaos attacks have a maximum damage of 500, can polymorph nearby creatures, can drain the player's experience, and causes them to hallucinate and become confused.<br>Resist chaos prevents the hallucination side effect, the confusion side efffect from chaos and confusion attacks, the experience drain side effect from nether or chaos attacks, and reduces damage from chaos spells by between 14 and 50 percent--damage=(damage*6)/(6+1d6)"));
            else if (pfr_ptr->this_flag == TR2_RES_DISEN) line_label->setToolTip(QString("Disenchantment attacks have a maximum damage of 500, and can reduce the bonus to-hit, to damage, and armor class qualities of player equiment.<br>Resist disenchantment protects the player's equipment from disenchantment, and reduces damage from disenchantment spells by between 14 and 50 percent--damage=(damage*6)/(6+1d6)"));
            else if (pfr_ptr->this_flag == TR2_RES_SOUND) line_label->setToolTip(QString("Sound attacks have a maximum damage of 500 and cause cuts on the player.<br>Resist sound protects the player from being stunned by sound attacks, and reduces damage from sound attacks by between 28 and 58 percent--damage=(damage*5)/(6+1d6)"));
            else if (pfr_ptr->this_flag == TR2_RES_SHARD) line_label->setToolTip(QString("Shard attacks have a maximum damage of 500 and can stun on the player.<br>Resist shards protects the player from cuts by shard attacks, and reduces damage from shard attacks by between 14 and 50 percent--damage=(damage*6)/(6+1d6)"));
            else if (pfr_ptr->this_flag == TR2_RES_LIGHT) line_label->setToolTip(QString("Light attacks have a maximum damage of 400 and can blind on the player.<br>Resist light protects the player from being blinded by light attacks, and reduces damage from light attacks by between 42 and 67 percent--damage=(damage*4)/(6+1d6)"));
            else if (pfr_ptr->this_flag == TR2_RES_DARK) line_label->setToolTip(QString("Darkness attacks have a maximum damage of 400 and can blind on the player.<br>Resist darkness protects the player from being blinded by darkness attacks, and reduces damage from darkness attacks by between 42 and 67 percent--damage=(damage*4)/(6+1d6)"));
            else if (pfr_ptr->this_flag == TR2_RES_FEAR) line_label->setToolTip(QString("Fear attacks prevents the player from making melee attacks.<br>Resist fear prevents the player from becoming afraid."));
        }
        else if (pfr_ptr->set == 3)
        {
            if (pfr_ptr->this_flag == TR3_SEE_INVIS) line_label->setToolTip(QString("Allows the player to see invisible creatures that are within line of sight."));
            else if (pfr_ptr->this_flag == TR3_FREE_ACT) line_label->setToolTip(QString("Prevents the player from being paralyzed, slowed, or slept.<br>If the player is heavily stunned, free action does not prevent them from passing out."));
            else if (pfr_ptr->this_flag == TR3_TELEPATHY) line_label->setToolTip(QString("Allows the player to sense creatures with an active mind.<br>Telepathy works even if the creatures are out of line of sight, or the player is blind.<br>Telepathy does not sense 'mindless' creatures."));
            else if (pfr_ptr->this_flag == TR3_HOLD_LIFE) line_label->setToolTip(QString("Protects the player from getting their experience drained.<br>Experience may sometimes be drained, however on those occasions the amount of experience drain is greatly reduced."));
            else if (pfr_ptr->this_flag == TR3_LIGHT) line_label->setToolTip(QString("Increases the player's light radius by 1 while being worn."));
            else if (pfr_ptr->this_flag == TR3_REGEN) line_label->setToolTip(QString("Doubles the rate at which a player heals, however it greatly increases the rate of player food consumption.<br>At normal speed, the rate of food consumption is quadrupled."));
            else if (pfr_ptr->this_flag == TR3_SLOW_DIGEST) line_label->setToolTip(QString("Slows the rate of player food consumption.<br>At normal speed, the rate of food consumption is cut in half."));
            else if (pfr_ptr->this_flag == TR3_FEATHER) line_label->setToolTip(QString("Prevents injury when the player hits a trap that causes them to fall."));
            else if (pfr_ptr->this_flag == TR3_TELEPORT) line_label->setToolTip(QString("Causes the player to have a 1% chance of being randomly teleported every game turn at normal speed."));
            else if (pfr_ptr->this_flag == TR3_AGGRAVATE) line_label->setToolTip(QString("Wakes up all monsters who are able to sense the player.<br>Aggravation negates all player stealth."));
            else if (pfr_ptr->this_flag == TR3_CURSE_ALL) line_label->setToolTip(QString("Once worn, a cursed item cannot be taken off until the curse is broken.<br>An item that is permanently cursed cannot be removed once wielded."));
            else if (pfr_ptr->this_flag == TR3_DRAIN_EXP) line_label->setToolTip(QString("Causes the player's experience to be drained by one point on 10% of player turns at normal speed, and cannot be prevented by hold life.<br>Experience loss is to both current experience and maximum experience."));
        }
        else if (pfr_ptr->set == 4)
        {
            if (pfr_ptr->this_flag == TN1_NATIVE_LAVA) line_label->setToolTip(QString("Being native allows the player to be on lava terrains without taking any damage, and affects the player's stealth.<br>It allows the player to move more efficiently through lava terrains, and gives the player a bonus to-hit in combat.<br>Being non-native makes the player succeptable to damage from terrain, slows their movement, and reduces their to-hit chances."));
            else if (pfr_ptr->this_flag == TN1_NATIVE_ICE) line_label->setToolTip(QString("Being native allows the player to be on icy terrains without taking any damage, and affects the player's stealth.<br>It allows the player to move more efficiently through icy terrains, and gives the player a bonus to-hit in combat.<br>Being non-native makes the player succeptable to damage from terrain, slows their movement, and reduces their to-hit chances."));
            else if (pfr_ptr->this_flag == TN1_NATIVE_OIL) line_label->setToolTip(QString("Being native allows the player to be on oily terrains without taking any damage, and affects the player's stealth.<br>It allows the player to move more efficiently through oily terrains, and gives the player a bonus to-hit in combat.<br>Being non-native makes the player succeptable to damage from terrain, slows their movement, and reduces their to-hit chances."));
            else if (pfr_ptr->this_flag == TN1_NATIVE_FIRE) line_label->setToolTip(QString("Being native allows the player to be on fiery terrains without taking any damage, and affects the player's stealth.<br>It allows the player to move more efficiently through fiery terrains, and gives the player a bonus to-hit in combat.<br>Being non-native makes the player succeptable to damage from terrain, slows their movement, and reduces their to-hit chances."));
            else if (pfr_ptr->this_flag == TN1_NATIVE_SAND) line_label->setToolTip(QString("Being native allows the player to be on sandy terrains without taking any damage, and affects the player's stealth.<br>It allows the player to move more efficiently through sandy terrains, and gives the player a bonus to-hit in combat.<br>Being non-native makes the player succeptable to damage from terrain, slows their movement, and reduces their to-hit chances."));
            else if (pfr_ptr->this_flag == TN1_NATIVE_FOREST) line_label->setToolTip(QString("Being native allows the player to be in forest terrains without taking any damage, and affects the player's stealth.<br>It allows the player to move more efficiently through forest terrains., and gives the player a bonus to-hit in combat.<br>Being non-native makes the player succeptable to damage from terrain, slows their movement, and reduces their to-hit chances."));
            else if (pfr_ptr->this_flag == TN1_NATIVE_WATER) line_label->setToolTip(QString("Being native allows the player to be on watery terrains without taking any damage, and affects the player's stealth.<br>It allows the player to move more efficiently through watery terrains, and gives the player a bonus to-hit in combat.<br>Being non-native makes the player succeptable to damage from terrain, slows their movement, and reduces their to-hit chances."));
            else if (pfr_ptr->this_flag == TN1_NATIVE_ACID) line_label->setToolTip(QString("Being native allows the player to be on acidic terrains without taking any damage, and affects the player's stealth.<br>It allows the player to move more efficiently through acidic terrains, and gives the player a bonus to-hit in combat.<br>Being non-native makes the player succeptable to damage from terrain, slows their movement, and reduces their to-hit chances."));
            else if (pfr_ptr->this_flag == TN1_NATIVE_MUD) line_label->setToolTip(QString("Being native allows the player to be on muddy terrains without taking any damage, and affects the player's stealth.<br>It allows the player to move more efficiently through muddy terrains, and gives the player a bonus to-hit in combat.<br>Being non-native makes the player succeptable to damage from terrain, slows their movement, and reduces their to-hit chances."));
            else if (pfr_ptr->this_flag == TN1_NATIVE_BMUD) line_label->setToolTip(QString("The player becomes native to boiling mud by being native to both lava and mud.<br>Being native allows the player to be on boiling mud terrains without taking any damage, and affects the player's stealth.<br>It allows the player to move more efficiently through boiling mud terrains, and gives the player a bonus to-hit in combat.<br>Being non-native makes the player succeptable to damage from terrain, slows their movement, and reduces their to-hit chances."));
            else if (pfr_ptr->this_flag == TN1_NATIVE_BWATER) line_label->setToolTip(QString("The player becomes native to boiling mud by being native to both lava and water.<br>Being native allows the player to be on boiling water terrains without taking any damage, and affects the player's stealth.<br>It allows the player to move more efficiently through biling water terrains, and gives the player a bonus to-hit in combat.<br>Being non-native makes the player succeptable to damage from terrain, slows their movement, and reduces their to-hit chances."));
        }
        return_layout->addWidget(line_label, row++, 0, Qt::AlignLeft);
    }

    return_layout->addItem(new QSpacerItem(1,1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding), row, 0);
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
            if (pfr_ptr->this_flag & (TR1_ALL_STATS)) line_label->setToolTip(stat_entry(x-1));
            else if (pfr_ptr->this_flag == TR1_INFRA) line_label->setToolTip(QString("Infravision allows the player to see warm blooded creatures, even invisible creatures, outside of their light radius.<br>One square = 10 feet."));
            else if (pfr_ptr->this_flag == TR1_STEALTH) line_label->setToolTip(QString("Stealth determines how much noise the player makes while in the dungeon.<br>Sleeping creatures will take longer to wake up when the player has high stealth.<br>Wielding an item that aggravates completely eliminates all player stealth."));
            else if (pfr_ptr->this_flag == TR1_SEARCH) line_label->setToolTip(QString("Searching affects the chance of searching each player turn, as well as the percent chance of noticing any hidden doors or traps that are within 10 feet of player while searching.<br>Also affects how quickly the player gets a feeling about the level they are on."));
            else if (pfr_ptr->this_flag == TR1_SPEED)
            {
                if (game_mode == GAME_NPPANGBAND) line_label->setToolTip(QString("Player rate of speed has a dramatic effect on player power and survivability.<br>The incremental changes in speed are reduced as the extremes are approached.<br>Normal speed = 1 player turn every 10 game turns<br>-7 speed = twice as slow as normal speed<br>+10 speed = 2x normal speed<br>+20 speed = 3x normal speed<br>+20 speed = 3x normal speed<br>+44 speed = 4x normal speed<br>+70 speed = 4.9x normal speed (maximum achievable speed)"));
                else line_label->setToolTip(QString("Player rate of speed has a dramatic effect on player power and survivability.<br>Each positive increment doubles player speed.<br>Each negative increment cuts player speed in half.<br>(+1)=2x normal speed<br>(+2)=4x normal speed<br>(+3)=8x normal speed<"));
            }
            else if (pfr_ptr->this_flag == TR1_TUNNEL) line_label->setToolTip(QString("Improves or reduces the chance of sucessfully tunneling through a walls.<br>Actual chance of success depends on the terrain into which the player is tunneling."));
            else if (pfr_ptr->this_flag == TR1_BLOWS) line_label->setToolTip(QString("Increases the number of player attacks the player has during per player turn."));
            else if (pfr_ptr->this_flag == TR1_SHOTS) line_label->setToolTip(QString("Reduces the amount of time it takes the player to fire ammunition from a bow, crossbow, or sling.<br>+1 = 1/2 of a player turn<br>+2 = 1/3 of a player turn etc."));
            else if (pfr_ptr->this_flag == TR1_MIGHT) line_label->setToolTip(QString("Increases the damage multiplier for ammunition fired from a bow, crossbow, or sling."));
        }
        return_layout->addWidget(line_label, row++, 0, Qt::AlignLeft);
    }

    QLabel *filler = new QLabel("  ");
    return_layout->addWidget(filler, 0, 5);

    return_layout->addItem(new QSpacerItem(1,1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding), row, 0);
}

PlayerScreenDialog::PlayerScreenDialog(void)
{
    QVBoxLayout *main_layout = new QVBoxLayout;

    // Title Box
    QVBoxLayout *title_line = new QVBoxLayout;
    main_layout->addLayout(title_line);
    QLabel *main_prompt = new QLabel(QString("<h2>Character Information %1 %2 </h2>") .arg(VERSION_MODE_NAME) .arg(VERSION_STRING));
    main_prompt->setAlignment(Qt::AlignCenter);
    title_line->addWidget(main_prompt);
    title_line->addWidget(new QLabel("  "));


    // Char info line
    QHBoxLayout *char_info = new QHBoxLayout;
    QGridLayout *basic_info = new QGridLayout;
    QGridLayout *basic_data = new QGridLayout;
    QGridLayout *game_info = new QGridLayout;
    QGridLayout *stat_info = new QGridLayout;
    QGridLayout *combat_info = new QGridLayout;
    QGridLayout *ability_info = new QGridLayout;

    main_layout->addLayout(char_info);
    char_basic_info(basic_info);
    char_info->addLayout(basic_info);

    char_basic_data(basic_data);
    char_info->addLayout(basic_data);

    char_game_info(game_info);
    char_info->addLayout(game_info);

    char_combat_info(combat_info);
    char_info->addLayout(combat_info);

    char_ability_info(ability_info);
    char_info->addLayout(ability_info);

    char_stat_info(stat_info);
    char_info->addLayout(stat_info);


    // Add player history
    // Title Box
    QVBoxLayout *history_box = new QVBoxLayout;
    main_layout->addLayout(history_box);
    history_box->addWidget(new QLabel("  "));
    QLabel *history = new QLabel();
    make_standard_label(history, p_ptr->history, TERM_BLUE);
    history_box->addWidget(history);
    history_box->addWidget(new QLabel("  "));

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



void do_cmd_player_screen()
{
    PlayerScreenDialog();
}
