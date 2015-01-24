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


#include <src/player_birth.h>
#include <src/optionsdialog.h>
#include <QDialogButtonBox>
#include <QRadioButton>
#include <src/help.h>
#include <QCheckBox>
#include <QList>
#include <QPushButton>



//assumes layout is consistent with add_stat_boxes below
void PlayerBirth::update_vlay_stats_info()
{

    for (int i = 0; i < A_MAX; i++)
    {
        int r = rp_ptr->r_adj[i];
        int c = cp_ptr->c_adj[i];
        int rc = r + c;

        QString race_num = format_stat(r);
        race_num.append(" ");
        race_num = color_string(race_num, (r < 0 ? TERM_RED : TERM_BLUE));
        QLabel *r_lbl = this->findChild<QLabel *>(QString("race_%1") .arg(i));
        r_lbl->setText(race_num);

        QString class_num = format_stat(c);
        class_num.append(" ");
        class_num = color_string(class_num, (c < 0 ? TERM_RED : TERM_BLUE));
        QLabel *cl_lbl = this->findChild<QLabel *>(QString("class_%1") .arg(i));
        cl_lbl->setText(class_num);

        QString combined_num = format_stat(rc);
        combined_num.append(" ");
        combined_num = color_string(combined_num, (rc < 0 ? TERM_RED : TERM_BLUE));
        QLabel *co_lbl = this->findChild<QLabel *>(QString("combined_%1") .arg(i));
        co_lbl->setText(combined_num);
    }

    // Update the hit points
    QString this_race = (QString("<b>%1 </b>") .arg(rp_ptr->r_mhp));
    QString this_class = (QString("<b>%1 </b>") .arg(cp_ptr->c_mhp));
    QString this_both = (QString("<b>%1 </b>") .arg(rp_ptr->r_mhp + cp_ptr->c_mhp));

    // Update the race hitpoints
    QLabel *this_lbl = this->findChild<QLabel *>("HD_Race");
    this_lbl->setText(color_string(this_race, TERM_BLUE));

    // Update the class hitpoints
    this_lbl = this->findChild<QLabel *>("HD_Class");
    this_lbl->setText(color_string(this_class, TERM_BLUE));

    // Update the combined hitpoints
    this_lbl = this->findChild<QLabel *>("HD_Both");
    this_lbl->setText(color_string(this_both, TERM_BLUE));

    // Update the % exp
    this_race = (QString("<b>%1%</b>") .arg(rp_ptr->r_exp));
    this_class = (QString("<b>%1%</b>") .arg(cp_ptr->c_exp));
    this_both = (QString("<b>%1%</b>") .arg(rp_ptr->r_exp + cp_ptr->c_exp));

    // Update the race %
    this_lbl = this->findChild<QLabel *>("XP_Race");
    this_lbl->setText(color_string(this_race, TERM_BLUE));

    // Update the class %
    this_lbl = this->findChild<QLabel *>("XP_Class");
    this_lbl->setText(color_string(this_class, TERM_BLUE));

    // Update the combined %
    this_lbl = this->findChild<QLabel *>("XP_Both");
    this_lbl->setText(color_string(this_both, TERM_BLUE));

}

// The layout here needs to be consistent with update_vlay_stats_info above
void PlayerBirth::add_stat_boxes(QVBoxLayout *return_layout)
{
    QLabel *stat_box_label = new QLabel("<h2>Stat Adjustments</h2>");
    stat_box_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    return_layout->addWidget(stat_box_label, Qt::AlignCenter);

    grid_stat_modifiers = new QGridLayout;
    vlay_stats_info_area->addLayout(grid_stat_modifiers);

    int row = 0;
    int col = 0;

    QLabel *stat_header = new QLabel;
    make_standard_label(stat_header, "STAT   ", TERM_DARK);
    grid_stat_modifiers->addWidget(stat_header, row, col++, Qt::AlignLeft);
    QLabel *race_adj_header = new QLabel;
    make_standard_label(race_adj_header, "   RA ", TERM_DARK);
    race_adj_header->setToolTip("Adjustments due to player race");
    grid_stat_modifiers->addWidget(race_adj_header, row, col++, Qt::AlignRight);
    QLabel *class_adj_header = new QLabel;
    make_standard_label(class_adj_header, "   CA ", TERM_DARK);
    class_adj_header->setToolTip("Adjustments due to player class");
    grid_stat_modifiers->addWidget(class_adj_header, row, col++, Qt::AlignRight);
    QLabel *all_adj_header = new QLabel;
    make_standard_label(all_adj_header, "   TA ", TERM_DARK);
    all_adj_header->setToolTip("Total Adjustments");
    grid_stat_modifiers->addWidget(all_adj_header, row, col++, Qt::AlignRight);

    row++;

    for (int i = 0; i < A_MAX; i++)
    {
        col = 0;

        // Stat label
        QLabel *stat_label = new QLabel();
        make_standard_label(stat_label, stat_names[i], TERM_DARK);
        stat_label->setToolTip(stat_entry(i));
        grid_stat_modifiers->addWidget(stat_label, row, col++, Qt::AlignLeft);

        int r = rp_ptr->r_adj[i];
        int c = cp_ptr->c_adj[i];
        int rc = r + c;

        QLabel *race_adj = new QLabel();
        make_standard_label(race_adj, (QString("   %1 ") .arg(format_stat(r))), (r < 0 ? TERM_RED : TERM_BLUE));
        race_adj->setObjectName(QString("race_%1") .arg(i));
        grid_stat_modifiers->addWidget(race_adj, row, col++, Qt::AlignRight);


        QLabel *class_adj = new QLabel();
        make_standard_label(class_adj, (QString("   %1 ") .arg(format_stat(c))), (c < 0 ? TERM_RED : TERM_BLUE));
        class_adj->setObjectName(QString("class_%1") .arg(i));
        grid_stat_modifiers->addWidget(class_adj, row, col++, Qt::AlignRight);

        QLabel *combined_adj = new QLabel();
        make_standard_label(combined_adj, (QString("   %1 ") .arg(format_stat(rc))), (rc < 0 ? TERM_RED : TERM_BLUE));
        combined_adj->setObjectName(QString("combined_%1") .arg(i));
        grid_stat_modifiers->addWidget(combined_adj, row++, col, Qt::AlignRight);
    }


    // Add a little space
    QLabel *dummy_label = new QLabel(" ");
    grid_stat_modifiers->addWidget(dummy_label, row++, 0, Qt::AlignLeft);
    QLabel *dummy_label2 = new QLabel(" ");
    grid_stat_modifiers->addWidget(dummy_label2, row++, 0, Qt::AlignLeft);

    col = 0;

    // Display character hit dice
    QLabel *hit_die_label = new QLabel();
    make_standard_label(hit_die_label, "Hit Die:", TERM_DARK);
    grid_stat_modifiers->addWidget(hit_die_label, row, col++, Qt::AlignLeft);

    QString this_race;
    QString this_class;
    QString this_both;

    this_race.setNum(rp_ptr->r_mhp);
    this_class.setNum(cp_ptr->c_mhp);
    this_both.setNum(rp_ptr->r_mhp + cp_ptr->c_mhp);
    this_race.append(" ");
    this_class.append(" ");
    this_both.append(" ");

    // HD Race
    QLabel *hit_die_race = new QLabel();
    make_standard_label(hit_die_race, this_race, TERM_BLUE);
    hit_die_race->setObjectName("HD_Race");
    grid_stat_modifiers->addWidget(hit_die_race, row, col++, Qt::AlignRight);

    // HD class
    QLabel *hit_die_class = new QLabel;
    make_standard_label(hit_die_class, this_class, TERM_BLUE);
    hit_die_class->setObjectName("HD_Class");
    grid_stat_modifiers->addWidget(hit_die_class, row, col++, Qt::AlignRight);

    // HD combined
    QLabel *hit_die_both = new QLabel;
    make_standard_label(hit_die_both, this_both, TERM_BLUE);
    hit_die_both->setObjectName("HD_Both");
    grid_stat_modifiers->addWidget(hit_die_both, row, col++, Qt::AlignRight);

    col = 0;
    row++;

    // Display experience percent
    QLabel *exp_pct_label = new QLabel();
    make_standard_label(exp_pct_label, "Experience:", TERM_DARK);
    grid_stat_modifiers->addWidget(exp_pct_label, row, col++, Qt::AlignLeft);

    this_race = (QString(" %1%") .arg(rp_ptr->r_exp));
    this_class = (QString(" %1%") .arg(cp_ptr->c_exp));
    this_both = (QString(" %1%") .arg(rp_ptr->r_exp + cp_ptr->c_exp));

    // HD Race
    QLabel *exp_pct_race = new QLabel();
    make_standard_label(exp_pct_race, this_race, TERM_BLUE);
    exp_pct_race->setObjectName("XP_Race");
    grid_stat_modifiers->addWidget(exp_pct_race, row, col++, Qt::AlignRight);

    // HD class
    QLabel *exp_pct_class = new QLabel;
    make_standard_label(exp_pct_class, this_class, TERM_BLUE);
    exp_pct_class->setObjectName("XP_Class");
    grid_stat_modifiers->addWidget(exp_pct_class, row, col++, Qt::AlignRight);

    // HD combined
    QLabel *exp_pct_both = new QLabel;
    make_standard_label(exp_pct_both, this_both, TERM_BLUE);
    exp_pct_both->setObjectName("XP_Both");
    grid_stat_modifiers->addWidget(exp_pct_both, row, col++, Qt::AlignRight);


}


// Find the button changed, and update the option value
void PlayerBirth::option_changed(int index)
{
    QList<QAbstractButton *> ops  = group_options->buttons();
    for (int i = 0; i < ops.size(); i++)
    {
        QAbstractButton *chk = ops.at(i);

        int id = group_options->id(chk);
        if (id != index) continue;
        op_ptr->opt[index] = chk->isChecked();
    }
}

void PlayerBirth::call_options_dialog()
{
    OptionsDialog *dlg = new OptionsDialog;
    dlg->exec();
    delete dlg;

    // now update the birth checkboxes
    QList<QAbstractButton *> ops  = group_options->buttons();
    for (int i = 0; i < ops.size(); i++)
    {
        QAbstractButton *chk = ops.at(i);

        int id = group_options->id(chk);
        chk->setChecked(op_ptr->opt[id]);
    }
}

// Add a box for all birth options
void PlayerBirth::add_option_boxes(QVBoxLayout *return_layout)
{
    group_options = new QButtonGroup;
    group_options->setExclusive(FALSE);

    QLabel *options_label = new QLabel("<h2>Birth Options</h2>");
    options_label->setToolTip("Birth Options must be changed before completing character<br>in order to apply to the current game.");
    options_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    return_layout->addWidget(options_label, Qt::AlignCenter);

    for (int i = 0; i < OPT_PAGE_PER; i++)
    {
        byte idx;
        if (game_mode == GAME_NPPANGBAND)
        {
            idx = option_page_nppangband[OPT_PAGE_BIRTH][i];
        }
        else //GAME_NPPMORIA
        {
            idx = option_page_nppmoria[OPT_PAGE_BIRTH][i];
        }
        if (idx == OPT_NONE) continue;

        option_entry *opt_ptr = &options[idx];
        if (opt_ptr->name.isEmpty()) continue;

        QCheckBox *this_checkbox = new QCheckBox(opt_ptr->description);
        this_checkbox->setChecked(op_ptr->opt[idx]);
        this_checkbox->setToolTip(get_help_topic(QString("option_info"), opt_ptr->name));

        group_options->addButton(this_checkbox, idx);

        return_layout->addWidget(this_checkbox);
    }

    connect(group_options, SIGNAL(buttonClicked(int)), this, SLOT(option_changed(int)));


}

void PlayerBirth::add_info_boxes(QVBoxLayout *return_layout)
{
    // add race description
    race_info = new QLabel();
    QString race_help = get_help_topic(QString("race_class_info"), p_info[cur_race].pr_name);
    race_info->setText(QString("%1<br><br>") .arg(race_help));
    race_info->setWordWrap(TRUE);
    return_layout->addWidget(race_info);

    // add class description
    class_info = new QLabel();
    QString class_help = get_help_topic(QString("race_class_info"), c_info[cur_class].cl_name);
    class_info->setText(QString("%1<br>") .arg(class_help));
    class_info->setWordWrap(TRUE);
    return_layout->addWidget(class_info);
}

void PlayerBirth::class_changed(int new_class)
{
    p_ptr->pclass = cur_class = new_class;
    QString class_help = get_help_topic(QString("race_class_info"), c_info[cur_class].cl_name);
    class_info->setText(QString("%1<br>") .arg(class_help));
    update_character();
}

void PlayerBirth::add_classes(QVBoxLayout *return_layout)
{
    group_class = new QButtonGroup;
    group_class->setExclusive(TRUE);

    QLabel *class_label = new QLabel("<h2>Player Class</h2>");
    class_label->setToolTip(get_help_topic("race_class_info", "Classes"));
    class_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    return_layout->addWidget(class_label, Qt::AlignCenter);

    cur_class = 0;

    for (int i = 0; i < z_info->c_max; i++)
    {
        QRadioButton *this_radiobutton = new QRadioButton(c_info[i].cl_name);
        if (i == cur_class) this_radiobutton->setChecked(TRUE);
        else this_radiobutton->setChecked(FALSE);
        this_radiobutton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        group_class->addButton(this_radiobutton, i);
        return_layout->addWidget(this_radiobutton);
    }

    connect(group_class, SIGNAL(buttonClicked(int)), this, SLOT(class_changed(int)));
}

void PlayerBirth::race_changed(int new_race)
{
    p_ptr->prace = cur_race = new_race;

    QString race_help = get_help_topic(QString("race_class_info"), p_info[cur_race].pr_name);
    race_info->setText(QString("%1<br><br>") .arg(race_help));
    update_character();
}

void PlayerBirth::add_races(QVBoxLayout *return_layout)
{
    group_race = new QButtonGroup;
    group_race->setExclusive(TRUE);

    QLabel *race_label = new QLabel("<h2>Player Race</h2>");
    race_label->setToolTip(get_help_topic("race_class_info", "races"));
    race_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    return_layout->addWidget(race_label, Qt::AlignCenter);

    cur_race = 0;

    for (int i = 0; i < z_info->p_max; i++)
    {
        QRadioButton *this_radiobutton = new QRadioButton(p_info[i].pr_name);
        if (i == cur_race) this_radiobutton->setChecked(TRUE);
        else this_radiobutton->setChecked(FALSE);
        this_radiobutton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        group_race->addButton(this_radiobutton, i);
        return_layout->addWidget(this_radiobutton);
    }

    connect(group_race, SIGNAL(buttonClicked(int)), this, SLOT(race_changed(int)));
}

void PlayerBirth::gender_changed(int new_gender)
{
    p_ptr->psex = cur_gender = new_gender;
    update_character();
}

void PlayerBirth::name_changed(QString new_name)
{
    op_ptr->full_name = cur_name = new_name;
    update_character();
}

// Generate a name at random
void PlayerBirth::random_name(void)
{
    cur_name = make_random_name(4, 12);
    player_name->setText(cur_name);

};
//Select a random gender
void PlayerBirth::random_gender(void)
{
    int x = group_gender->buttons().count();
    group_gender->buttons().at(rand_int(x))->click();
};
//select a random race
void PlayerBirth::random_race(void)
{
    int x = group_race->buttons().count();
    group_race->buttons().at(rand_int(x))->click();

};
// select a random class
void PlayerBirth::random_class(void)
{
    int x = group_class->buttons().count();
    group_class->buttons().at(rand_int(x))->click();
};
// select everything randomly
void PlayerBirth::random_all(void)
{
    hold_update = TRUE;
    random_name();
    random_gender();
    random_race();
    random_class();
    hold_update = FALSE;
    update_character();
};

// Add player names, buttons, and gender
void PlayerBirth::add_genders(QVBoxLayout *return_layout)
{
    QLabel *name_label = new QLabel("<h2>Charater Name</h2>");
    name_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    return_layout->addWidget(name_label, Qt::AlignCenter);

    op_ptr->full_name = cur_name = "Player";
    player_name = new QLineEdit(cur_name);
    player_name->setText(cur_name);
    connect(player_name, SIGNAL(textChanged(QString)), this, SLOT(name_changed(QString)));
    player_name->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    return_layout->addWidget(player_name, Qt::AlignLeft);

    return_layout->addStretch(1);

    group_gender = new QButtonGroup;
    group_gender->setExclusive(TRUE);

    QLabel *gender_label = new QLabel("<h2>Player Gender</h2>");
    gender_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    return_layout->addWidget(gender_label, Qt::AlignCenter);

    cur_gender = 0;

    for (int i = 0; i < MAX_SEXES; i++)
    {
        QRadioButton *this_radiobutton = new QRadioButton(sex_info[i].title);
        this_radiobutton->setChecked(TRUE);
        if (i == cur_gender) this_radiobutton->setChecked(TRUE);
        else this_radiobutton->setChecked(FALSE);
        this_radiobutton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        group_gender->addButton(this_radiobutton, i);
        return_layout->addWidget(this_radiobutton);
    }

    connect(group_gender, SIGNAL(buttonClicked(int)), this, SLOT(gender_changed(int)));

    QLabel *gender_expl = new QLabel(color_string(QString("<b>Gender does not have any significant gameplay effects.</b>"), TERM_BLUE));
    gender_expl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    gender_expl->setWordWrap(TRUE);
    return_layout->addWidget(gender_expl);

    return_layout->addStretch(1);

    QPushButton *rand_name = new QPushButton("Random Name");
    rand_name->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    return_layout->addWidget(rand_name, Qt::AlignCenter);
    connect(rand_name, SIGNAL(clicked()), this, SLOT(random_name()));

    QPushButton *rand_gender = new QPushButton("Random Gender");
    rand_gender->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    return_layout->addWidget(rand_gender, Qt::AlignCenter);
    connect(rand_gender, SIGNAL(clicked()), this, SLOT(random_gender()));

    QPushButton *rand_race = new QPushButton("Random Race");
    rand_race->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    return_layout->addWidget(rand_race, Qt::AlignCenter);
    connect(rand_race, SIGNAL(clicked()), this, SLOT(random_race()));

    QPushButton *rand_class = new QPushButton("Random Class");
    rand_class->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    return_layout->addWidget(rand_class, Qt::AlignCenter);
    connect(rand_class, SIGNAL(clicked()), this, SLOT(random_class()));

    QPushButton *rand_all = new QPushButton("Random Character");
    rand_all->setToolTip("Randomly select the character's name, gender, race, and class.");
    rand_all->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    return_layout->addWidget(rand_all, Qt::AlignCenter);
    connect(rand_all, SIGNAL(clicked()), this, SLOT(random_all()));

    QPushButton *all_options = new QPushButton("All Options");
    all_options->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    return_layout->addWidget(all_options, Qt::AlignCenter);
    connect(all_options, SIGNAL(clicked()), this, SLOT(call_options_dialog()));
}

void PlayerBirth::update_character()
{
    // prevent updating more than once per change, if necessary;
    if (hold_update) return;

    p_ptr->prace = cur_race;
    p_ptr->pclass = cur_class;
    p_ptr->psex = cur_gender;
    generate_player();
    calc_bonuses(inventory, &p_ptr->state, false);
    calc_stealth();
    update_vlay_stats_info();
    update_char_screen();
}

void PlayerBirth::setup_character()
{
    init_birth();

    p_ptr->prace = cur_race;
    p_ptr->pclass = cur_class;
    p_ptr->psex = cur_gender;
    generate_player();

    // UGLY HACK!
    if (game_mode == GAME_NPPANGBAND) adult_maximize = birth_maximize;
    else adult_maximize = birth_maximize = false;

}

// Build the birth dialog
PlayerBirth::PlayerBirth(bool quickstart)
{
    quick_start = quickstart;
    hold_update = FALSE;

    QVBoxLayout *main_layout = new QVBoxLayout;
    QHBoxLayout *choices = new QHBoxLayout;
    main_layout->addLayout(choices);

    // Add a box for options
    vlay_options = new QVBoxLayout;
    choices->addLayout(vlay_options);
    add_option_boxes(vlay_options);
    vlay_options->addStretch(1);
    main_layout->addStretch(1);

    // Add gender column and buttons
    vlay_gender = new QVBoxLayout;
    choices->addLayout(vlay_gender);
    add_genders(vlay_gender);
    vlay_gender->addStretch(1);
    main_layout->addStretch(1);

    // Add race column
    vlay_race  = new QVBoxLayout;
    choices->addLayout(vlay_race);
    add_races(vlay_race);
    vlay_race->addStretch(1);
    main_layout->addStretch(1);

    //Add class column
    vlay_class  = new QVBoxLayout;
    choices->addLayout(vlay_class);
    add_classes(vlay_class);
    vlay_class->addStretch(1);
    main_layout->addStretch(1);

    vlay_help_area = new QVBoxLayout;
    choices->addLayout(vlay_help_area);
    add_info_boxes(vlay_help_area);
    main_layout->addStretch(1);

    // Setup the character
    setup_character();

    vlay_stats_info_area = new QVBoxLayout;
    choices->addLayout(vlay_stats_info_area);
    add_stat_boxes(vlay_stats_info_area);
    vlay_stats_info_area->addStretch(1);
    main_layout->addStretch(1);

    QHBoxLayout *hlay_info = new QHBoxLayout;
    main_layout->addLayout(hlay_info);

    QVBoxLayout *vlay_char_basic = new QVBoxLayout;
    QGridLayout *glay_char_basic = new QGridLayout;
    char_basic_info(glay_char_basic);
    vlay_char_basic->addLayout(glay_char_basic);
    vlay_char_basic->addStretch(1);
    hlay_info->addLayout(vlay_char_basic);
    hlay_info->addStretch(1);

    QVBoxLayout *vlay_char_data = new QVBoxLayout;
    QGridLayout *glay_char_data = new QGridLayout;
    char_basic_data(glay_char_data);
    vlay_char_data->addLayout(glay_char_data);
    vlay_char_data->addStretch(1);
    hlay_info->addLayout(vlay_char_data);
    hlay_info->addStretch(1);

    /*QVBoxLayout *vlay_ability_info = new QVBoxLayout;
    QGridLayout *glay_ability_info = new QGridLayout;
    char_ability_info(glay_ability_info);
    vlay_ability_info->addLayout(glay_ability_info);
    vlay_char_data->addStretch(1);
    hlay_info->addLayout(vlay_ability_info);
    hlay_info->addStretch(1);*/

    //Add a close button on the right side
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    main_layout->addWidget(buttons);

    setLayout(main_layout);
    if (this->exec()) done_birth = TRUE;
    else done_birth = FALSE;


}


