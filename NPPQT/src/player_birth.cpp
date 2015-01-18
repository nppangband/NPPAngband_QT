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
    cur_class = new_class;
    QString class_help = get_help_topic(QString("race_class_info"), c_info[cur_class].cl_name);
    class_info->setText(QString("%1<br>") .arg(class_help));
}

void PlayerBirth::add_classes(QVBoxLayout *return_layout)
{
    group_class = new QButtonGroup;
    group_class->setExclusive(TRUE);

    QLabel *class_label = new QLabel("<h2>Player Class</h2>");
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
    cur_race = new_race;
    QString race_help = get_help_topic(QString("race_class_info"), p_info[cur_race].pr_name);
    race_info->setText(QString("%1<br><br>") .arg(race_help));

}

void PlayerBirth::add_races(QVBoxLayout *return_layout)
{
    group_race = new QButtonGroup;
    group_race->setExclusive(TRUE);

    QLabel *race_label = new QLabel("<h2>Player Race</h2>");
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
    cur_gender = new_gender;
}

void PlayerBirth::name_changed(QString new_name)
{
    cur_name = new_name;
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

// Add player names, buttons, and gender
void PlayerBirth::add_genders(QVBoxLayout *return_layout)
{
    QLabel *name_label = new QLabel("<h2>Charater Name</h2>");
    name_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    return_layout->addWidget(name_label, Qt::AlignCenter);

    cur_name = "Player";
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
    connect(rand_all, SIGNAL(clicked()), this, SLOT(random_name()));
    connect(rand_all, SIGNAL(clicked()), this, SLOT(random_gender()));
    connect(rand_all, SIGNAL(clicked()), this, SLOT(random_race()));
    connect(rand_all, SIGNAL(clicked()), this, SLOT(random_class()));

    QPushButton *all_options = new QPushButton("All Options");
    all_options->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    return_layout->addWidget(all_options, Qt::AlignCenter);
    connect(all_options, SIGNAL(clicked()), this, SLOT(call_options_dialog()));

}

// Build the birth dialog
PlayerBirth::PlayerBirth(bool quickstart)
{
    quick_start = quickstart;

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

    //Add a close button on the right side
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    main_layout->addWidget(buttons);

    setLayout(main_layout);
    if (this->exec()) done_birth = TRUE;
    else done_birth = FALSE;


}


