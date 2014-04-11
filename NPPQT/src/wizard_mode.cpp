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

#include "src/wizard_mode.h"


EditCharacterDialog::EditCharacterDialog(void)
{
    QGridLayout *edit_info = new QGridLayout;

    // First allow the 6 stats to be edited
    // Strength
    QLabel *str_label = new QLabel("Strength");
    QSpinBox *str_spinner = new QSpinBox;
    str_spinner->setRange(3,118);
    str_spinner->setValue(p_ptr->stat_cur[A_STR]);
    edit_info->addWidget(str_label, 1, 0);
    edit_info->addWidget(str_spinner, 1, 1);

    // Intelligence
    QLabel *int_label = new QLabel("Intelligence");
    QSpinBox *int_spinner = new QSpinBox;
    int_spinner->setRange(3,118);
    int_spinner->setValue(p_ptr->stat_cur[A_INT]);
    edit_info->addWidget(int_label, 2, 0);
    edit_info->addWidget(int_spinner, 2, 1);

    // Wisdom
    QLabel *wis_label = new QLabel("Wisdom");
    QSpinBox *wis_spinner = new QSpinBox;
    wis_spinner->setRange(3,118);
    wis_spinner->setValue(p_ptr->stat_cur[A_WIS]);
    edit_info->addWidget(wis_label, 3, 0);
    edit_info->addWidget(wis_spinner, 3, 1);

    // Dexterity
    QLabel *dex_label = new QLabel("Dexterity");
    QSpinBox *dex_spinner = new QSpinBox;
    dex_spinner->setRange(3,118);
    dex_spinner->setValue(p_ptr->stat_cur[A_DEX]);
    edit_info->addWidget(dex_label, 4, 0);
    edit_info->addWidget(dex_spinner, 4, 1);

    // Constitution
    QLabel *con_label = new QLabel("Constitution");
    QSpinBox *con_spinner = new QSpinBox;
    con_spinner->setRange(3,118);
    con_spinner->setValue(p_ptr->stat_cur[A_CON]);
    edit_info->addWidget(con_label, 5, 0);
    edit_info->addWidget(con_spinner, 5, 1);

    // Charisma
    QLabel *chr_label = new QLabel("Charisma");
    QSpinBox *chr_spinner = new QSpinBox;
    chr_spinner->setRange(3,118);
    chr_spinner->setValue(p_ptr->stat_cur[A_CHR]);
    edit_info->addWidget(chr_label, 6, 0);
    edit_info->addWidget(chr_spinner, 6, 1);

    QPushButton *close_button = new QPushButton(tr("&Close"));
    connect(close_button, SIGNAL(clicked()), this, SLOT(close()));
    edit_info->addWidget(close_button, 14, 2);

    setLayout(edit_info);
    setWindowTitle(tr("Character Edit Screen"));
    this->exec();

    p_ptr->stat_cur[A_STR] = p_ptr->stat_max[A_STR] = str_spinner->value();
    p_ptr->stat_cur[A_INT] = p_ptr->stat_max[A_INT] = int_spinner->value();
    p_ptr->stat_cur[A_WIS] = p_ptr->stat_max[A_WIS] = wis_spinner->value();
    p_ptr->stat_cur[A_DEX] = p_ptr->stat_max[A_DEX] = dex_spinner->value();
    p_ptr->stat_cur[A_CON] = p_ptr->stat_max[A_CON] = con_spinner->value();
    p_ptr->stat_cur[A_CHR] = p_ptr->stat_max[A_CHR] = chr_spinner->value();


    //TODO update everything

}

// Completely cure the player
void WizardModeDialog::wiz_cure_all(void)
{
    /*
     * Cure everything instantly
     */
    /* Remove curses */
    (void)remove_all_curse();

    /* Restore stats */
    (void)res_stat(A_STR);
    (void)res_stat(A_INT);
    (void)res_stat(A_WIS);
    (void)res_stat(A_CON);
    (void)res_stat(A_DEX);
    (void)res_stat(A_CHR);

    /* Restore the level */
    (void)restore_level();

    /* Heal the player */
    p_ptr->chp = p_ptr->mhp;
    p_ptr->chp_frac = 0;

    /* Restore mana */
    p_ptr->csp = p_ptr->msp;
    p_ptr->csp_frac = 0;

    /* Cure stuff */
    (void)clear_timed(TMD_BLIND, TRUE);
    (void)clear_timed(TMD_CONFUSED, TRUE);
    (void)clear_timed(TMD_POISONED, TRUE);
    (void)clear_timed(TMD_AFRAID, TRUE);
    (void)clear_timed(TMD_PARALYZED, TRUE);
    (void)clear_timed(TMD_IMAGE, TRUE);
    (void)clear_timed(TMD_STUN, TRUE);
    (void)clear_timed(TMD_CUT, TRUE);
    (void)clear_timed(TMD_SLOW, TRUE);

    /* No longer hungry */
    (void)set_food(PY_FOOD_MAX - 1);

    this->accept();

    /* Redraw everything */
    // TODO do_cmd_redraw();



}

// Know every object kind, ego item, feature, and monster
void WizardModeDialog::wiz_know_all(void)
{

    int i, j;

    /* Knowledge of every object */
    for (i = 1; i < z_info->k_max; i++)
    {
        k_info[i].aware = TRUE;
        k_info[i].everseen = TRUE;
        k_info[i].tried = TRUE;

    }
    /* Knowledge of every ego-item */
    for (i = 1; i < z_info->e_max; i++)
    {
        e_info[i].everseen = TRUE;
    }
    /* Full knowledge of every monster */
    for (i = 1; i < z_info->r_max; i++)
    {
        monster_race *r_ptr = &r_info[i];
        monster_lore *l_ptr = &l_list[i];

        /* Know all flags */
        l_ptr->r_l_flags1 = r_ptr->flags1;
        l_ptr->r_l_flags2 = r_ptr->flags2;
        l_ptr->r_l_flags3 = r_ptr->flags3;
        l_ptr->r_l_flags4 = r_ptr->flags4;
        l_ptr->r_l_flags5 = r_ptr->flags5;
        l_ptr->r_l_flags6 = r_ptr->flags6;
        l_ptr->r_l_flags7 = r_ptr->flags7;
        l_ptr->r_l_native = r_ptr->r_native;

        /* Know max sightings, sleeping habits, spellcasting, and combat blows. */
        l_ptr->sights = MAX_SHORT;
        l_ptr->ranged = MAX_UCHAR;
        for (j = 0; j < MONSTER_BLOW_MAX; j++)
        {
            l_ptr->blows[j] = MAX_UCHAR;
        }
        l_ptr->wake = l_ptr->ignore = MAX_UCHAR;

        /* know the treasure drops*/
        l_ptr->drop_gold = l_ptr->drop_item =
        (((r_ptr->flags1 & RF1_DROP_4D2) ? 8 : 0) +
         ((r_ptr->flags1 & RF1_DROP_3D2) ? 6 : 0) +
         ((r_ptr->flags1 & RF1_DROP_2D2) ? 4 : 0) +
         ((r_ptr->flags1 & RF1_DROP_1D2) ? 2 : 0) +
         ((r_ptr->flags1 & RF1_DROP_90)  ? 1 : 0) +
         ((r_ptr->flags1 & RF1_DROP_60)  ? 1 : 0));

        /* But only "valid" treasure drops */
        if (r_ptr->flags1 & RF1_ONLY_GOLD) l_ptr->drop_item = 0;
        if (r_ptr->flags1 & RF1_ONLY_ITEM) l_ptr->drop_gold = 0;
    }

    /* Full knowledge of every feature */
    for (i = 1; i < z_info->f_max; i++)
    {
        feature_type *f_ptr = &f_info[i];
        feature_lore *f_l_ptr = &f_l_list[i];

        /* Know all flags and sites */
        f_l_ptr->f_l_flags1 = f_ptr->f_flags1;
        f_l_ptr->f_l_flags2 = f_ptr->f_flags2;
        f_l_ptr->f_l_flags3 = f_ptr->f_flags3;
        f_l_ptr->f_l_sights = MAX_UCHAR;

        /* Know all transitions */
        f_l_ptr->f_l_defaults = MAX_UCHAR;
        for (j = 0; j < MAX_FEAT_STATES; j++)
        {
            /*There isn't an action here*/
            if (f_ptr->state[j].fs_action == FS_FLAGS_END) continue;

            /* Hack -- we have seen this transition */
            f_l_ptr->f_l_state[j] = MAX_UCHAR;
        }
        /*Know movement, damage to non-native, and stealth.....*/
        f_l_ptr->f_l_dam_non_native = MAX_UCHAR;
        f_l_ptr->f_l_native_moves = MAX_UCHAR;
        f_l_ptr->f_l_non_native_moves = MAX_UCHAR;
        f_l_ptr->f_l_stealth_adj = MAX_UCHAR;
        f_l_ptr->f_l_native_to_hit_adj = MAX_UCHAR;
        f_l_ptr->f_l_non_native_to_hit_adj = MAX_UCHAR;
    }

    this->accept();
}

// Jump to a new dungeon level
void WizardModeDialog::wiz_jump(void)
{
    int new_level;

    /* Prompt */
    QString prompt = QString("Jump to level (0-%1): ") .arg(MAX_DEPTH-1);

    new_level = get_quantity(prompt, MAX_DEPTH - 1, p_ptr->depth);

    // Same depth - quit
    if (new_level == p_ptr->depth) return;

    /* Accept request */
    message(QString("You jump to dungeon level %1.") .arg(new_level));

    this->accept();

    /* New depth */
    dungeon_change_level(new_level);

    // Update everything
    //TODO don't use any player energy
    process_player_energy(BASE_ENERGY_MOVE);
}

void WizardModeDialog::wiz_teleport_to_target(void)
{
    /* Must have a target */
    if (target_okay())
    {
        /* Teleport to the target */
        teleport_player_to(p_ptr->target_row, p_ptr->target_col);

        this->accept();
    }
    else this->reject();
}

void WizardModeDialog::wiz_phase_door(void)
{
    teleport_player(10, FALSE);
    this->accept();
}

void WizardModeDialog::wiz_teleport(void)
{
    teleport_player(100, FALSE);
    this->accept();
}


// Summon one monster
void WizardModeDialog::wiz_summon(void)
{
    int py = p_ptr->py;
    int px = p_ptr->px;

    (void)summon_specific(py, px, p_ptr->depth, 0, 0L);
    this->accept();
}

// Summon one monster
void WizardModeDialog::wiz_banish(void)
{
    int i;

    /* Banish everyone nearby */
    for (i = 1; i < mon_max; i++)
    {
        monster_type *m_ptr = &mon_list[i];
        monster_race *r_ptr = &r_info[m_ptr->r_idx];

        /* Skip dead monsters */
        if (!m_ptr->r_idx) continue;

        /* Skip distant monsters */
        if (m_ptr->cdis > (MAX_SIGHT+10)) continue;

        /* Hack -- Skip unique monsters */
        if (r_ptr->flags1 & (RF1_UNIQUE)) continue;

        /* Quest monsters can only be "killed" by the player */
        if (m_ptr->mflag & (MFLAG_QUEST)) continue;

        /* Delete the monster */
        delete_monster_idx(i);
    }

    /* Update monster list window */
    p_ptr->redraw |= PR_MONLIST;

    this->accept();
}

void WizardModeDialog::wiz_detect_all_monsters(void)
{
    int i;

    /* Process monsters */
    for (i = 1; i < mon_max; i++)
    {
        monster_type *m_ptr = &mon_list[i];

        /* Skip dead monsters */
        if (!m_ptr->r_idx) continue;

        /* Optimize -- Repair flags */
        repair_mflag_mark = TRUE;
        repair_mflag_show = TRUE;

        /* Detect the monster */
        m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

        /* Update the monster */
        update_mon(i, FALSE);
    }

    this->accept();

}

void WizardModeDialog::wiz_edit_character(void)
{
    EditCharacterDialog();
    this->accept();
}

void WizardModeDialog::wiz_detection(void)
{
    wiz_light();
    (void)detect(DETECT_RADIUS, DETECT_ALL);  
    this->accept();
}

void WizardModeDialog::wiz_magic_mapping(void)
{
    detect(DETECT_RADIUS + 10, DETECT_MAP);    
    this->accept();
}

void WizardModeDialog::wiz_level_light(void)
{
    wiz_light();
    this->accept();
}

void WizardModeDialog::wiz_redraw_dungeon(void)
{
    p_ptr->leaving_level = TRUE;
    p_ptr->autosave = TRUE;

    this->accept();

    // TODO re-set the dungeon.  Does this work?
}

void WizardModeDialog::wiz_mass_create_items(void)
{
    int i;
    object_type object_type_body;

    object_type *i_ptr;

    for(i=0; i < 25; i++)
    {
        /* Get local object */
        i_ptr = &object_type_body;

        /* Wipe the object */
        i_ptr->object_wipe();

        /* Make a object (if possible) */
        if (!make_object(i_ptr, FALSE, FALSE, DROP_TYPE_UNTHEMED, FALSE)) continue;

        /* Drop the object */
        drop_near(i_ptr, -1, p_ptr->py, p_ptr->px);
    }
    this->accept();
}


void WizardModeDialog::wiz_create_good_item(void)
{
    acquirement(p_ptr->py, p_ptr->px, 1, FALSE);
    this->accept();
}

void WizardModeDialog::wiz_create_great_item(void)
{
    acquirement(p_ptr->py, p_ptr->px, 1, TRUE);
    this->accept();
}

void WizardModeDialog::wiz_mass_identify_items(void)
{
    (void)mass_identify(4);
    this->accept();
}

WizardModeDialog::WizardModeDialog(void)
{
    int row = 0;

    // Paranoia
    if (!p_ptr->playing) return;

    if (!p_ptr->is_wizard)
    {
        QString prompt = color_string(QString ("<b><big>You are about to use 'Wizard Mode' commands.</big></b><br><br>"), TERM_RED);
        prompt.append("Wizard Mode contains many powerful 'cheat' commands.  ");
        prompt.append("Your savefile will be marked as a wizard mode character and will not be scored.<br><br>");
        prompt.append("Really use Wizard Mode?");
        if (!get_check(prompt)) return;
        p_ptr->is_wizard = TRUE;
    }

    main_prompt = new QLabel(QString("<b><big>Please select a command</big></b><br>"));
    main_prompt->setAlignment(Qt::AlignCenter);

    QVBoxLayout *vlay = new QVBoxLayout;

    vlay->addWidget(main_prompt);

    // Add the player related commands
    QGridLayout *wizard_layout = new QGridLayout;

    player_section = new QLabel("<b>Player commands</b>");
    player_section->setAlignment(Qt::AlignCenter);
    wizard_layout->addWidget(player_section, row, 1);

    row++;

    // Add the "cure all" button
    QPushButton *heal_button = new QPushButton("Heal Player");
    heal_button->setToolTip("Completely heal and restore the player.");
    connect(heal_button, SIGNAL(clicked()), this, SLOT(wiz_cure_all()));
    wizard_layout->addWidget(heal_button, row, 0);

    // Add the "know all" button
    QPushButton *know_button = new QPushButton("Know All");
    know_button->setToolTip("Know everything about every feature, object, and monster in the game.");
    connect(know_button, SIGNAL(clicked()), this, SLOT(wiz_know_all()));
    wizard_layout->addWidget(know_button, row, 1);

    // Add the "jump" button
    QPushButton *jump_button = new QPushButton("Jump To New Level");
    jump_button->setToolTip("Allow the player to instantly jump to any level in the dungeon.");
    connect(jump_button, SIGNAL(clicked()), this, SLOT(wiz_jump()));
    wizard_layout->addWidget(jump_button, row, 2);

    row++;

    // Add the "teleport_to_target" button
    QPushButton *teleport_target_button = new QPushButton("Teleport To Targeted Spot");
    teleport_target_button->setToolTip("Teleports the player to a specified spot on the dungeon level.");
    connect(teleport_target_button, SIGNAL(clicked()), this, SLOT(wiz_teleport_to_target()));
    wizard_layout->addWidget(teleport_target_button, row, 0);

    // Add the "phase door" button
    QPushButton *phase_door = new QPushButton("Phase Door");
    phase_door->setToolTip("Teleports the player to a random spot up to 10 squares away.");
    connect(phase_door, SIGNAL(clicked()), this, SLOT(wiz_phase_door()));
    wizard_layout->addWidget(phase_door, row, 1);

    // Add the "teleport" button
    QPushButton *teleport = new QPushButton("Teleport");
    teleport->setToolTip("Teleports the player to a random spot up to 100 squares away.");
    connect(teleport, SIGNAL(clicked()), this, SLOT(wiz_teleport()));
    wizard_layout->addWidget(teleport, row, 2);

    row++;

        // Add the "edit character" button
    QPushButton *edit_character = new QPushButton("Edit Character");
    edit_character->setToolTip("Edit character statistics, experience, gold, and fame.");
    connect(edit_character, SIGNAL(clicked()), this, SLOT(wiz_edit_character()));
    wizard_layout->addWidget(edit_character, row, 0);

    row++;

    // Add the dungeon commands
    dungeon_section = new QLabel("<b>Dungeon commands</b>");
    dungeon_section->setAlignment(Qt::AlignCenter);
    wizard_layout->addWidget(dungeon_section, row, 1);

    row++;

    // Add the "summon" button
    QPushButton *summon_button = new QPushButton("Summon Monster");
    summon_button->setToolTip("Summon one random monster.");
    connect(summon_button, SIGNAL(clicked()), this, SLOT(wiz_summon()));
    wizard_layout->addWidget(summon_button, row, 0);

    // Add the "banish" button
    QPushButton *banish_button = new QPushButton("Banish Monsters");
    banish_button->setToolTip("Erase all monsters within 30 squares of player, except for uniques and quest monsters.");
    connect(banish_button, SIGNAL(clicked()), this, SLOT(wiz_banish()));
    wizard_layout->addWidget(banish_button, row, 1);

    // Add the "detect all monsters" button
    QPushButton *display_mon_button = new QPushButton("Detect All Monsters");
    display_mon_button->setToolTip("Detect all monsters on the level.");
    connect(display_mon_button, SIGNAL(clicked()), this, SLOT(wiz_detect_all_monsters()));
    wizard_layout->addWidget(display_mon_button, row, 2);

    row++;

    // Add the "detection" button
    QPushButton *detection = new QPushButton("Detection");
    detection->setToolTip("Cast the 'Detection' spell");
    connect(detection, SIGNAL(clicked()), this, SLOT(wiz_detection()));
    wizard_layout->addWidget(detection, row, 0);

    // Add the "magic mapping" button
    QPushButton *magic_mapping = new QPushButton("Magic Mapping");
    magic_mapping->setToolTip("Cast the 'Magic Mapping' spell.");
    connect(magic_mapping, SIGNAL(clicked()), this, SLOT(wiz_magic_mapping()));
    wizard_layout->addWidget(magic_mapping, row, 1);

    // Add the "light dungeon" button
    QPushButton *dungeon_light = new QPushButton("Light Dungeon");
    dungeon_light->setToolTip("Illuminate the entire dungeon level.");
    connect(dungeon_light, SIGNAL(clicked()), this, SLOT(wiz_level_light()));
    wizard_layout->addWidget(dungeon_light, row, 2);

    row++;

    // Add the "redraw dungeon" button
    QPushButton *redraw_dungeon = new QPushButton("Redraw Dungeon");
    redraw_dungeon->setToolTip("Redraw a new dungeon level at the current depth.");
    connect(redraw_dungeon, SIGNAL(clicked()), this, SLOT(wiz_redraw_dungeon()));
    wizard_layout->addWidget(redraw_dungeon, row, 0);

    row++;

    // Add the object commands
    object_section = new QLabel("<b>Object commands</b>");
    object_section->setAlignment(Qt::AlignCenter);
    wizard_layout->addWidget(object_section, row, 1);

    row++;

    // Add the "mass create items" button
    QPushButton *mass_create_items_button = new QPushButton("Create 25 Random Items");
    mass_create_items_button->setToolTip("Drop 25 randomly generated objects around the player.");
    connect(mass_create_items_button , SIGNAL(clicked()), this, SLOT(wiz_mass_create_items()));
    wizard_layout->addWidget(mass_create_items_button, row, 0);

    // Add the "create 1 random good item" button
    QPushButton *create_good_item = new QPushButton("Create 1 Random Good Item");
    create_good_item->setToolTip("Drop one randomly created guaranteed good item by the player.");
    connect(create_good_item , SIGNAL(clicked()), this, SLOT(wiz_create_good_item()));
    wizard_layout->addWidget(create_good_item, row, 1);

    // Add the "create 1 random great item" button
    QPushButton *create_great_item = new QPushButton("Create 1 Random Great Item");
    create_great_item->setToolTip("Drop one randomly created guaranteed great item by the player.");
    connect(create_great_item , SIGNAL(clicked()), this, SLOT(wiz_create_great_item()));
    wizard_layout->addWidget(create_great_item, row, 2);

    row++;

    // Add the "mass identify" button
    QPushButton *mass_identify = new QPushButton("Mass Identify");
    mass_identify->setToolTip("Identify all objects the player has, as well as all objects within 5 squares.");
    connect(mass_identify, SIGNAL(clicked()), this, SLOT(wiz_mass_identify_items()));
    wizard_layout->addWidget(mass_identify, row, 0);

    vlay->addLayout(wizard_layout);

    buttons = new QDialogButtonBox(QDialogButtonBox::Cancel);
    connect(buttons, SIGNAL(rejected()), this, SLOT(close()));
    vlay->addWidget(buttons);

    setLayout(vlay);
    setWindowTitle(tr("Wizard Mode Menu"));

    this->exec();
}


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

void do_cmd_wizard_mode(void)
{
    WizardModeDialog();
}
