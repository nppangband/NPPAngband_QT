
/*
 * File: squelch.c
 * Purpose: Item destruction
 *
 * Copyright (c) 2007 David T. Blackston, Iain McFall, DarkGod, Jeff Greene,
 * Diego Gonzalez, David Vestal, Pete Mack, Andrew Sidwell.
 *
 * Please see copyright.txt for complete copyright and licensing restrictions.
 *
 */
#include "src/npp.h"
#include "src/squelch.h"
#include "src/squelch_dialogs.h"
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>


// Set all applicable settings to teh new squelch quality
void QualitySquelchDialog::set_all_squelch_quality(int new_quality)
{
    // now update all the buttons manually
    QList<QRadioButton *> button_group_list = this->findChildren<QRadioButton *>();


    // Now go through and set the quality squelch_settings, and update the radio buttons.
    for (int x = 0; x < button_group_list.size(); x++)
    {
        QRadioButton *this_radiobutton = button_group_list.at(x);

        QString this_name = this_radiobutton->objectName();

        bool dummy;

        int this_id = this_name.toInt(&dummy);

        // This would only happen if make_quality_squelch_buttons was not set up with a proper object name;
        if (!dummy) continue;

        int this_group = this_id / 100;
        int this_setting = this_id % 100;

        // Continue for the non-applicable settings.
        if ((this_group == PS_TYPE_AMULET) || (this_group == PS_TYPE_RING))
        {
            if (new_quality > SQUELCH_CURSED) continue;
        }
        else if (this_group == PS_TYPE_LIGHT)
        {
            if (new_quality == SQUELCH_ALL) continue;
        }

        // Not the right quality_setting
        if (this_setting != new_quality)
        {
            this_radiobutton->setChecked(FALSE);
            continue;
        }

        // Change the squelch setting, set the radiobox to true;
        squelch_level[this_group] = new_quality;
        this_radiobutton->setChecked(TRUE);
    }
}

// Make a series of pushbuttons to change settings for all subgroups
void QualitySquelchDialog::make_quality_squelch_pushbuttons(QHBoxLayout *return_layout)
{
    QPointer<QPushButton> squelch_all_none = new QPushButton("Set all to Squelch None");
    connect(squelch_all_none, SIGNAL(pressed()), this, SLOT(set_all_quality_none()));
    return_layout->addWidget(squelch_all_none);

    return_layout->addStretch(1);

    QPointer<QPushButton> squelch_all_cursed = new QPushButton("Set all to Squelch Cursed");
    connect(squelch_all_cursed, SIGNAL(pressed()), this, SLOT(set_all_quality_cursed()));
    return_layout->addWidget(squelch_all_cursed);

    return_layout->addStretch(1);

    QPointer<QPushButton> squelch_all_average = new QPushButton("Set all to Squelch Average");
    connect(squelch_all_average, SIGNAL(pressed()), this, SLOT(set_all_quality_average()));
    return_layout->addWidget(squelch_all_average);

    return_layout->addStretch(1);

    QPointer<QPushButton> squelch_all_good_strong = new QPushButton("Set all to Squelch Good");
    connect(squelch_all_good_strong, SIGNAL(pressed()), this, SLOT(set_all_quality_good_strong()));
    return_layout->addWidget(squelch_all_good_strong);

    return_layout->addStretch(1);

    if (!cp_ptr->pseudo_id_heavy())
    {
        QPointer<QPushButton> squelch_all_good_weak = new QPushButton("Set all to Squelch Good (+Good Pseudo ID)");
        connect(squelch_all_good_weak, SIGNAL(pressed()), this, SLOT(set_all_quality_good_weak()));
        return_layout->addWidget(squelch_all_good_weak);

        return_layout->addStretch(1);
    }

    QPointer<QPushButton> squelch_all_but_artifact = new QPushButton("Set all to Squelch All But Artifact");
    connect(squelch_all_but_artifact, SIGNAL(pressed()), this, SLOT(set_all_quality_all_but_artifact()));
    return_layout->addWidget(squelch_all_but_artifact);
}

void QualitySquelchDialog::update_quality_squelch_settings(void)
{
    QString item_id = QObject::sender()->objectName();
    bool dummy;
    int sender_id = item_id.toInt(&dummy);

    // This would only happen if make_quality_squelch_buttons was not set up with a proper object name;
    if (!dummy) return;

    int quality_group = sender_id / 100;
    int quality_setting = sender_id % 100;
    squelch_level[quality_group] = quality_setting;

    // now update all the buttons manually
    QList<QRadioButton *> button_group_list = this->findChildren<QRadioButton *>();

    // Now go through and set all the group buttons to false, except
    // the one just checked.  Ignore the other groups.
    for (int x = 0; x < button_group_list.size(); x++)
    {
        QRadioButton *this_radiobutton = button_group_list.at(x);

        QString this_name = this_radiobutton->objectName();

        int this_id = this_name.toInt(&dummy);

        // This would only happen if make_quality_squelch_buttons was not set up with a proper object name;
        if (!dummy) continue;

        int this_group = this_id / 100;
        int this_setting = this_id % 100;

        if (this_group != quality_group) continue;
        if (this_setting == quality_setting) this_radiobutton->setChecked(TRUE);
        else this_radiobutton->setChecked(FALSE);
    }
}

// Add the quality buttons depending on the player class and the game type
// The function above parses the buttons as the quality_choice is multiplied by 100, then the quality type added.
void QualitySquelchDialog::make_quality_squelch_radiobuttons(QGridLayout *return_layout)
{
    for (int i = 0; i < PS_TYPE_MAX; i++)
    {
        int column = 0;
        bool limited_types = FALSE;
        if (i == PS_TYPE_AMULET) limited_types = TRUE;
        else if (i == PS_TYPE_RING) limited_types = TRUE;

        QPointer<QLabel> quality_name = new QLabel(QString("<b>%1</b>") .arg(quality_choices[i].name));
        quality_name->setAlignment(Qt::AlignCenter);
        return_layout->addWidget(quality_name, i, column++, Qt::AlignLeft);

        QPointer<QRadioButton> quality_none = new QRadioButton(quality_values[SQUELCH_NONE].name);
        quality_none->setToolTip("Do not automatically destroy items of this type based on the quality of that item.");
        quality_none->setObjectName(QString("%1") .arg((i*100) + SQUELCH_NONE));
        return_layout->addWidget(quality_none, i, column++, Qt::AlignLeft);
        quality_none->setAutoExclusive(FALSE);
        if (squelch_level[i] == SQUELCH_NONE)
        {
            quality_none->setChecked(TRUE);
        }
        else quality_none->setChecked(FALSE);
        connect(quality_none, SIGNAL(pressed()), this, SLOT(update_quality_squelch_settings()));

        QPointer<QRadioButton> quality_cursed = new QRadioButton(quality_values[SQUELCH_CURSED].name);
        quality_cursed->setToolTip("Automatically destroy cursed or broken items upon identification or pseudo-id.");
        quality_cursed->setObjectName(QString("%1") .arg((i*100) + SQUELCH_CURSED));
        return_layout->addWidget(quality_cursed, i, column++, Qt::AlignLeft);
        quality_cursed->setAutoExclusive(FALSE);
        if (squelch_level[i] == SQUELCH_CURSED) quality_cursed->setChecked(TRUE);
        else quality_cursed->setChecked(FALSE);
        connect(quality_cursed, SIGNAL(pressed()), this, SLOT(update_quality_squelch_settings()));

        // Jewlery only needs a couple settings
        if (limited_types) continue;


        QPointer<QRadioButton> quality_average = new QRadioButton(quality_values[SQUELCH_AVERAGE].name);
        quality_average->setToolTip("Automatically destroy cursed or average items of this type  upon identification or pseudo-id.");
        quality_average->setObjectName(QString("%1") .arg((i*100) + SQUELCH_AVERAGE));
        return_layout->addWidget(quality_average, i, column++, Qt::AlignLeft);
        quality_average->setAutoExclusive(FALSE);
        if (squelch_level[i] == SQUELCH_AVERAGE) quality_average->setChecked(TRUE);
        else quality_average->setChecked(FALSE);
        connect(quality_average, SIGNAL(pressed()), this, SLOT(update_quality_squelch_settings()));

        QPointer<QRadioButton> quality_good_strong = new QRadioButton(quality_values[SQUELCH_GOOD_STRONG].name);
        if (cp_ptr->pseudo_id_heavy()) quality_good_strong->setToolTip("Automatically destroy cursed, average, or good items of this type upon identification or pseudo-id.");
        else quality_good_strong->setToolTip("Automatically destroy cursed or average item of this type upon identification or pseudo-id. Destroy good items upon identification, but do not destroy items that psuedo-id as good (to prevent an excellent item that pseudo-ids as good from being destroyed).");
        quality_good_strong->setObjectName(QString("%1") .arg((i*100) + SQUELCH_GOOD_STRONG));
        return_layout->addWidget(quality_good_strong, i, column++, Qt::AlignLeft);
        quality_good_strong->setAutoExclusive(FALSE);
        if (squelch_level[i] == SQUELCH_GOOD_STRONG) quality_good_strong->setChecked(TRUE);
        quality_good_strong->setChecked(FALSE);
        connect(quality_good_strong, SIGNAL(pressed()), this, SLOT(update_quality_squelch_settings()));

        if (!cp_ptr->pseudo_id_heavy())
        {
            QPointer<QRadioButton> quality_good_weak = new QRadioButton(quality_values[SQUELCH_GOOD_WEAK].name);
            quality_good_weak->setToolTip("Automatically destroy cursed, average, or good items of this type upon identification or pseudo-id.  Excellent items that pseudo-id as a good item will be destroyed.");
            quality_good_weak->setObjectName(QString("%1") .arg((i*100) + SQUELCH_GOOD_WEAK));
            return_layout->addWidget(quality_good_weak, i, column++, Qt::AlignLeft);
            quality_good_weak->setAutoExclusive(FALSE);
            if (squelch_level[i] == SQUELCH_GOOD_WEAK) quality_good_weak->setChecked(TRUE);
            quality_good_weak->setChecked(FALSE);
            connect(quality_good_weak, SIGNAL(pressed()), this, SLOT(update_quality_squelch_settings()));
        }

        if (i == PS_TYPE_LIGHT) continue;

        if (game_mode != GAME_NPPMORIA)
        {
            QPointer<QRadioButton> quality_all_but_artifact = new QRadioButton(quality_values[SQUELCH_ALL].name);
            quality_all_but_artifact->setToolTip("Automatically destroy all items of this type, except artifacts, upon identification or pseudo-id.");
            quality_all_but_artifact->setObjectName(QString("%1") .arg((i*100) + SQUELCH_ALL));
            return_layout->addWidget(quality_all_but_artifact, i, column++, Qt::AlignLeft);
            quality_all_but_artifact->setAutoExclusive(FALSE);
            if (squelch_level[i] == SQUELCH_ALL) quality_all_but_artifact->setChecked(TRUE);
            connect(quality_all_but_artifact, SIGNAL(pressed()), this, SLOT(update_quality_squelch_settings()));
        }
    }
}

QualitySquelchDialog::QualitySquelchDialog(void): NPPDialog()
{
    central = new QWidget;
    QPointer<QVBoxLayout> main_layout = new QVBoxLayout;
    central->setLayout(main_layout);
    main_layout->setSpacing(10);
    // IMPORTANT: it must be called AFTER setting the layout
    this->setClient(central);

    QPointer<QLabel> quality_header = new QLabel("<b><h2>Quality Squelch Menu</h2></b>");
    quality_header->setToolTip("The settings below allow the player to automatically destroy an item on identification, or pseudo-id, based on the quality of that item.");
    main_layout->addWidget(quality_header, Qt::AlignCenter);

    QPointer<QHBoxLayout> squelch_hlay = new QHBoxLayout;
    main_layout->addLayout(squelch_hlay);
    make_quality_squelch_pushbuttons(squelch_hlay);

    QPointer<QGridLayout> squelch_glay = new QGridLayout;
    main_layout->addLayout(squelch_glay);


    make_quality_squelch_radiobuttons(squelch_glay);

    main_layout->addStretch(1);

    QPointer<QDialogButtonBox> buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, SIGNAL(rejected()), this, SLOT(close()));
    main_layout->addWidget(buttons);

    setWindowTitle(tr("Quality Squelch Menu"));

    this->clientSizeUpdated();

    this->exec();
}

void do_quality_squelch_menu()
{
    QualitySquelchDialog();
}
