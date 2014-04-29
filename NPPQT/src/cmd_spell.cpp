
/* File: was cmd5.c */

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


#include <src/cmd_spell.h>\

// Receives the number of the button pressed.
void SpellSelectDialog::button_press(QString num_string)
{
    selected_button = num_string.toInt();

    this->accept();
}

// See if the user selected a button bia a keypress.
void SpellSelectDialog::keyPressEvent(QKeyEvent* which_key)
{
    // Handle escape key
    if (which_key->key() == Qt::Key_Escape)
    {
        this->close();
        return;
    }

    QString key_pressed = which_key->text();

    //  Make sure we are dealing with a letter
    if (key_pressed.length() != 1 || !key_pressed.at(0).isLetter()) return;

    // Make it lowercase
    key_pressed = key_pressed.toLower();

    // Make sure we are dealing with the item name buttons and not the info buttons
    // or other future buttons
    key_pressed.append(") ");

    QWidget *tab = this->spell_dialog->currentWidget(); // Search in the current tab
    QList<QPushButton *> buttons = tab->findChildren<QPushButton *>();
    for (int i = 0; i < buttons.size(); i++)
    {
        if (buttons.at(i)->text().startsWith(key_pressed))
        {
            buttons.at(i)->click();
            break;
        }
    }
}


// Receives the number of the button pressed.
void SpellSelectDialog::help_press(QString num_string)
{
    int spell_num = num_string.toInt();

    QString spell_desc = (QString("<b><big>%1</big></b><br><br>")
                        .arg(cast_spell(MODE_SPELL_NAME, cp_ptr->spell_book, spell_num, 0)));

    spell_desc.append(cast_spell(MODE_SPELL_DESC, cp_ptr->spell_book, spell_num, 0));

    /* Display the spell */
    QMessageBox::information(0, "Press OK to continue.", spell_desc, QMessageBox::Ok);
}



void SpellSelectDialog::count_spells(int mode)
{
    for (int i = 0; i < max_spellbooks; i++)
    {
        int idx = lookup_kind(cp_ptr->spell_book, i);
        if (!object_kind_is_available(idx, USE_FLOOR | USE_INVEN)) continue;

        num_available_spellbooks++;

        for (int j = 0; j < SPELLS_PER_BOOK; j++)
        {
            int spell = get_spell_from_list(i, j);

            if (spell == -1) continue;

            if (mode != BOOK_BROWSE)
            {
                if (!spell_okay(spell, (mode == BOOK_CAST))) continue;
            }

            // Note that the book and spell is available for use.
            available_spells[i][j] = TRUE;

            // Make sure we know we are using this book
            available_books[i] = TRUE;

            num_spells++;
        }
    }
}

void SpellSelectDialog::clear_spells(void)
{
    for (int i = 0; i < BOOKS_PER_REALM_ANGBAND; i++)
    {
        available_books[i] = FALSE;
        for (int x = 0; x < SPELLS_PER_BOOK; x++)
        {
            available_spells[i][x] = FALSE;
        }
    }
}

QString SpellSelectDialog::get_spell_comment(int spell)
{
    const magic_type *s_ptr = &mp_ptr->info[spell];
    QString comment;

    // Illegible spell
    if (s_ptr->slevel >= 99) comment = "(illegible)";

    // Is it an ironman spell?
    else if (p_ptr->spell_flags[spell] & PY_SPELL_IRONMAN)      comment = "Ironman Spell";

    // Forgotten spell
    else if (p_ptr->spell_flags[spell] & PY_SPELL_FORGOTTEN)    comment = "forgotten";

    // Not learned yet
    else if (!(p_ptr->spell_flags[spell] & PY_SPELL_LEARNED))
    {
        if (s_ptr->slevel <= p_ptr->lev) comment = "unknown";
        else comment = "difficult";
    }
    else if (!(p_ptr->spell_flags[spell] & PY_SPELL_WORKED)) comment = "untried";

    // Get a spell description
    else comment = cast_spell(MODE_SPELL_DESC_SHORT, cp_ptr->spell_book, spell, 0);

    return (comment);
}

void SpellSelectDialog::on_dialog_buttons_pressed(QAbstractButton *)
{
    this->reject();
}


void SpellSelectDialog::build_spellbook_dialog(int mode)
{
    for (int i = 0; i < max_spellbooks; i++)
    {
        // Track to which line we are adding widgets
        int row_num = 0;

        // Nothing to select in this book.
        if (!available_books[i]) continue;

        int idx = lookup_kind(cp_ptr->spell_book, i);

        // Get the book name and remove the brackets
        QString book_name = k_info[idx].k_name;
        book_name.remove("[");
        book_name.remove("]");

        spell_tab = new QWidget;

        QVBoxLayout *vlay = new QVBoxLayout;

        QWidget *aux = new QWidget;
        vlay->addWidget(aux);

        vlay->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

        QGridLayout *spell_layout = new QGridLayout;
        aux->setLayout(spell_layout);

        spell_layout->setColumnStretch(0, 100);

        // Add a button to select the spellbook
        if (choosing_book)
        {
            QString noun = cast_spell(MODE_SPELL_NOUN, cp_ptr->spell_book, 1, 0);
            QString text_num = QString::number(i);
            QPushButton *button = new QPushButton(text_num);
            button->setText(QString("Study a %1 from %2") .arg(noun) .arg(book_name));
            button->setStyleSheet("Text-align:left");
            spell_layout->addWidget(button, row_num, COL_SPELL_TITLE);
            connect(button, SIGNAL(clicked()), button_values, SLOT(map()));
            button_values->setMapping(button, text_num);
            row_num++;
        }

            // Give each one titles
        QLabel *spell_title_header = new QLabel("Spell Name");
        QLabel *level_header = new QLabel("  Level  ");
        QLabel *mana_header = new QLabel("  Mana  ");
        QLabel *fail_header = new QLabel("  % Fail  ");
        QLabel *info_header = new QLabel(" Info");
        QLabel *help_header = new QLabel("  Help  ");
        spell_title_header->setAlignment(Qt::AlignLeft);
        spell_title_header->setAlignment(Qt::AlignVCenter);
        level_header->setAlignment(Qt::AlignCenter);
        mana_header->setAlignment(Qt::AlignCenter);
        fail_header->setAlignment(Qt::AlignCenter);
        info_header->setAlignment(Qt::AlignLeft);
        info_header->setAlignment(Qt::AlignVCenter);
        help_header->setAlignment(Qt::AlignCenter);

        // Add the headers
        spell_layout->addWidget(spell_title_header, row_num, COL_SPELL_TITLE);
        spell_layout->addWidget(level_header, row_num, COL_LEVEL);
        spell_layout->addWidget(mana_header, row_num, COL_MANA);
        spell_layout->addWidget(fail_header, row_num, COL_FAIL_PCT);
        spell_layout->addWidget(info_header, row_num, COL_INFO);
        spell_layout->addWidget(help_header, row_num, COL_HELP);

        row_num++;

        for (int j = 0; j < SPELLS_PER_BOOK; j++)
        {
            // First decide if thise must be a text button or a push button
            int spell = get_spell_from_list(i, j);

            // Not a spell
            if (spell == -1) continue;

            /* Get the spell info */
            const magic_type *s_ptr = &mp_ptr->info[spell];

            bool do_text = FALSE;
            if (mode == BOOK_BROWSE) do_text = TRUE;
            else if (choosing_book) do_text = TRUE;
            else if (!spell_okay(spell, (mode == BOOK_CAST))) do_text = TRUE;

            QString spell_name = (QString("%1) ") .arg(number_to_letter(j)));
            spell_name.append(cast_spell(MODE_SPELL_NAME, cp_ptr->spell_book, spell, 0));
            QString spell_comment = get_spell_comment(spell);

            QString text_num = QString::number(spell);
            // Either do a pushbutton or a text label
            if (do_text)
            {
                QLabel *spell_label = new QLabel(spell_name);
                spell_label->setAlignment(Qt::AlignLeft);
                spell_label->setAlignment(Qt::AlignVCenter);
                spell_label->setToolTip(cast_spell(MODE_SPELL_DESC, cp_ptr->spell_book, spell, 0));
                spell_layout->addWidget(spell_label, row_num, COL_SPELL_TITLE);
            }
            // Create a push button and link it
            else
            {
                QPushButton *button = new QPushButton(text_num);
                button->setText(spell_name);
                button->setStyleSheet("Text-align:left");
                button->setToolTip(cast_spell(MODE_SPELL_DESC, cp_ptr->spell_book, spell, 0));
                spell_layout->addWidget(button, row_num, COL_SPELL_TITLE);
                connect(button, SIGNAL(clicked()), button_values, SLOT(map()));
                button_values->setMapping(button, text_num);

            }
            // Add level info
            QLabel *level_value = new QLabel(QString("%1") .arg(s_ptr->slevel));
            level_value->setAlignment(Qt::AlignCenter);
            spell_layout->addWidget(level_value, row_num, COL_LEVEL);
            // Add mana info
            QLabel *mana_value = new QLabel(QString("%1") .arg(s_ptr->smana));
            mana_value->setAlignment(Qt::AlignCenter);
            spell_layout->addWidget(mana_value, row_num, COL_MANA);
            // Add spell % info
            QLabel *spell_value = new QLabel(QString("%1%") .arg(spell_chance(spell)));
            spell_value->setAlignment(Qt::AlignCenter);
            spell_layout->addWidget(spell_value, row_num, COL_FAIL_PCT);
            // Add spell_desc info
            QLabel *info_desc = new QLabel(QString("%1") .arg(spell_comment));
            info_desc->setAlignment(Qt::AlignLeft);
            info_desc->setAlignment(Qt::AlignVCenter);
            spell_layout->addWidget(info_desc, row_num, COL_INFO);

            // Add a help button to put up a detailed spell description.
            QPushButton *help_button = new QPushButton(text_num);
            help_button->setIcon(QIcon(":/icons/lib/icons/help.png"));
            help_button->setText(QString(""));
            connect(help_button, SIGNAL(clicked()), help_values, SLOT(map()));
            help_values->setMapping(help_button, text_num);
            spell_layout->addWidget(help_button, row_num, COL_HELP);

            row_num++;

        }

        spell_tab->setLayout(vlay);

        spell_dialog->addTab(spell_tab, book_name);
    }
}


// This assumes the check that the player cna cast has already been done.
SpellSelectDialog::SpellSelectDialog(int *spell, QString prompt, int mode, bool *success, bool *cancelled)
{
    spell_dialog = new QTabWidget;

    main_prompt = new QLabel(QString("<b><big>%1</big></b>") .arg(prompt));
    main_prompt->setAlignment(Qt::AlignCenter);

    button_values = new QSignalMapper(this);
    help_values = new QSignalMapper(this);
    connect(button_values, SIGNAL(mapped(QString)), this, SLOT(button_press(QString)));
    connect(help_values, SIGNAL(mapped(QString)), this, SLOT(help_press(QString)));

    // Start with a clean slate
    num_spells = 0;
    max_spellbooks = (game_mode == GAME_NPPANGBAND ? BOOKS_PER_REALM_ANGBAND : BOOKS_PER_REALM_MORIA);
    num_available_spellbooks = 0;
    choosing_book = FALSE;
    *cancelled = FALSE;
    *success = FALSE;
    clear_spells();

    // First, find the eligible spells
    count_spells(mode);

    // Make sure we can actually do the command asked for
    if ((mode == BOOK_STUDY) && !p_ptr->can_study())
    {
        return;
    }
    else if (!p_ptr->can_cast())
    {
        return;
    }

    // Handle no available objects.
    if ((mode == BOOK_BROWSE) && (!num_available_spellbooks))
    {
        /* Report failure */
         return;
    }
    else if (!num_spells)
    {
        /* Report failure */
        return;
    }

    // We are selecting a book instead of a specific spell.
    if ((mode == BOOK_STUDY) && !p_ptr->chooses_spells()) choosing_book = TRUE;

    build_spellbook_dialog(mode);

    if (mode == BOOK_BROWSE) buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
    else buttons = new QDialogButtonBox(QDialogButtonBox::Cancel);
    connect(buttons, SIGNAL(clicked(QAbstractButton*)), this,
    SLOT(on_dialog_buttons_pressed(QAbstractButton*)));

    QVBoxLayout *main_layout = new QVBoxLayout;

    main_layout->addWidget(main_prompt);
    main_layout->addWidget(spell_dialog);
    main_layout->addWidget(buttons);

    setLayout(main_layout);
    setWindowTitle(tr("Spell Selection Menu"));

    if (!this->exec())
    {
        *cancelled = TRUE;
        *success = FALSE;
    }
    else
    {
        *spell = selected_button;
        if (*spell > -1) *success = TRUE;
        else *success = FALSE;
    }
}


/*
 * Determine if a spell is "okay" for the player to cast or study
 * The spell must be legible, not forgotten, and also, to cast,
 * it must be known, and to study, it must not be known.
 */
bool spell_okay(int spell, bool known)
{
    const magic_type *s_ptr;

    /* Get the spell */
    s_ptr = &mp_ptr->info[spell];

    /* Spell is illegal */
    if (s_ptr->slevel > p_ptr->lev) return (FALSE);

    /* Spell is forgotten */
    if (p_ptr->spell_flags[spell] & PY_SPELL_FORGOTTEN)
    {
        /* Never okay */
        return (FALSE);
    }

    /* Spell is ironman */
    if (p_ptr->spell_flags[spell] & PY_SPELL_IRONMAN)
    {
        /* Never okay */
        return (FALSE);
    }

    /* Spell is learned */
    if (p_ptr->spell_flags[spell] & PY_SPELL_LEARNED)
    {

        /* Okay to cast, not to study */
        return (known);
    }

    /* Okay to study, not to cast */
    return (!known);
}


/*
 * Learn the specified spell.
 */
static void spell_learn(int spell)
{
    int i;
   QString noun = cast_spell(MODE_SPELL_NOUN, cp_ptr->spell_book, 1, 0);

    /* Learn the spell */
    p_ptr->spell_flags[spell] |= PY_SPELL_LEARNED;

    /* Find the next open entry in "spell_order[]" */
    for (i = 0; i < PY_MAX_SPELLS; i++)
    {
        /* Stop at the first empty space */
        if (p_ptr->spell_order[i] == 99) break;
    }

    /* Add the spell to the known list */
    p_ptr->spell_order[i] = spell;

    /* Mention the result */
    message(QString("You have learned the %1 of %2.") .arg(noun) .arg(get_spell_name(cp_ptr->spell_book, spell)));

    /* One less spell available */
    p_ptr->new_spells--;

    /* Message if needed */
    if (p_ptr->new_spells)
    {
        if (p_ptr->new_spells > 1) noun.append("s");

        /* Message */
        message(QString("You can learn %1 more %2.") .arg(p_ptr->new_spells) .arg(noun));
    }

    /* Redraw Study Status */
    p_ptr->redraw |= (PR_STUDY | PR_OBJECT);
}

/*
 * Gain a random spell from the given book (for priests)
 */
static void study_book(int book)
{

    int spell = -1;
    int i, k = 0;

    /* Extract spells */
    for (i = 0; i < SPELLS_PER_BOOK; i++)
    {
        int s = get_spell_from_list(book, i);

        /* Skip non-OK spells */
        if (s == -1) continue;
        if (!spell_okay(s, FALSE)) continue;

        /* Apply the randomizer */
        if ((++k > 1) && (randint0(k) != 0)) continue;

        /* Track it */
        spell = s;
    }

    /* Remember we have used this book */
    object_kind *k_ptr = &k_info[lookup_kind(cp_ptr->spell_book, book)];
    k_ptr->tried = TRUE;

    spell_learn(spell);
}

s16b get_spell_from_list(s16b book, s16b spell)
{
    int realm = get_player_spell_realm();

    if (game_mode == GAME_NPPMORIA)
    {
        /* Check bounds */
        if ((spell < 0) || (spell >= SPELLS_PER_BOOK)) return (-1);
        if ((book < 0) || (book >= BOOKS_PER_REALM_MORIA)) return (-1);

        if (realm == MAGE_REALM) return (spell_list_nppmoria_mage[book][spell]);
        if (realm == PRIEST_REALM) return (spell_list_nppmoria_priest[book][spell]);
    }
    else
    {
        /* Check bounds */
        if ((spell < 0) || (spell >= SPELLS_PER_BOOK)) return (-1);
        if ((book < 0) || (book >= BOOKS_PER_REALM_ANGBAND)) return (-1);

        if (realm == MAGE_REALM) return (spell_list_nppangband_mage[book][spell]);
        if (realm == PRIEST_REALM) return (spell_list_nppangband_priest[book][spell]);
        if (realm == DRUID_REALM) return (spell_list_nppangband_druid[book][spell]);
    }


    /* Whoops! */
    return (-1);
}

/*
 * Helper function to help spells that target traps (disarming, etc...)
 */
static bool is_trap_spell(byte spell_book, int spell)
{
    if (spell_book == TV_MAGIC_BOOK)
    {
        if (spell == SPELL_TRAP_DOOR_DESTRUCTION) return (TRUE);
    }
    else if (spell_book == TV_PRAYER_BOOK)
    {
        if (spell == PRAYER_UNBARRING_WAYS) return (TRUE);
    }
    else if (spell_book == TV_DRUID_BOOK)
    {
        if (spell == DRUID_TRAP_DOOR_DESTRUCTION) return (TRUE);

    }
    return (FALSE);
}

/* Adjustment to minimum failure rates for wisdom/intelligence in moria */
static int spell_failure_min_moria(int stat)
{
    int value = p_ptr->state.stat_use[stat];

    if (value > 117) 		return(0);
    else if (value > 107)	return(1);
    else if (value > 87)	return(2);
    else if (value > 67)	return(3);
    else if (value > 17)	return(4);
    else if (value > 14)	return(7);
    else if (value > 7)		return(10);
    else	return(25);
}

/*
 * Returns chance of failure for a spell
 */
int spell_chance(int spell)
{
    int chance, minfail;

    const magic_type *s_ptr;

    /* Paranoia -- must be literate */
    if (!cp_ptr->spell_book) return (100);

    /* Get the spell */
    s_ptr = &mp_ptr->info[spell];

    /* Extract the base spell failure rate */
    chance = s_ptr->sfail;

    /* Reduce failure rate by "effective" level adjustment */
    chance -= 3 * (p_ptr->lev - s_ptr->slevel);

    /* Reduce failure rate by INT/WIS adjustment */
    /* Extract the minimum failure rate */
    if (game_mode == GAME_NPPMORIA)
    {
        chance -= 3 * (stat_adj_moria(MORIA_SPELL_STAT)-1);
    }
    else chance -= adj_mag_stat[SPELL_STAT_SLOT];

    /* Not enough mana to cast */
    if (s_ptr->smana > p_ptr->csp)
    {
        chance += 5 * (s_ptr->smana - p_ptr->csp);
    }

    /* Extract the minimum failure rate */
    if (game_mode == GAME_NPPMORIA) minfail = spell_failure_min_moria(MORIA_SPELL_STAT);
    else minfail = adj_mag_fail[SPELL_STAT_SLOT];

    /* Non mage/priest characters never get better than 5 percent */
    if (!(cp_ptr->flags & CF_ZERO_FAIL))
    {
        if (minfail < 5) minfail = 5;
    }

    /* Priest prayer penalty for "edged" weapons (before minfail) */
    if (p_ptr->state.icky_wield)
    {
        chance += 25;
    }

    /* Minimum failure rate */
    if (chance < minfail) chance = minfail;

    /* Stunning makes spells harder (after minfail) */
    if (p_ptr->timed[TMD_STUN] > 50) chance += 25;
    else if (p_ptr->timed[TMD_STUN]) chance += 15;

    /* Always a 5 percent chance of working */
    if (chance > 95) chance = 95;

    /* Return the chance */
    return (chance);
}

// Cast a spell
void do_cmd_cast(void)
{
    int spell;
    int dir = DIR_UNKNOWN;
    QString noun = cast_spell(MODE_SPELL_NOUN, cp_ptr->spell_book, 1, 0);
    QString verb = cast_spell(MODE_SPELL_VERB, cp_ptr->spell_book, 1, 0);

    QString prompt = (QString("Please select a %1 to %2.") .arg(noun) .arg(verb));

    int mode = BOOK_CAST;
    bool success;
    bool cancelled;
    SpellSelectDialog(&spell, prompt, mode, &success, &cancelled);

    // Handle not having a spell to learn
    if ((!success) || (cancelled))
    {
        if (!success && !cancelled) message(QString("You have no %1s that you can %3 right now.") .arg(noun) .arg(verb));
        return;
    }

    bool trap_spell = is_trap_spell(cp_ptr->spell_book, spell);

    // TODO solve this
    p_ptr->command_dir = 0;

    if (spell_needs_aim(cp_ptr->spell_book, spell) && !get_aim_dir(&dir, trap_spell)) return;

    /* Get the spell */
    const magic_type *s_ptr;s_ptr = &mp_ptr->info[spell];

    /* Verify insufficient mana */
    if (s_ptr->smana > p_ptr->csp)
    {
        /* Warning */
        message(QString("You do not have enough mana to %1 this %2.") .arg(verb) .arg(noun));

        /* Verify */
        if (!get_check("Attempt it anyway? ")) return;
    }

    /* Spell failure chance */
    int chance = spell_chance(spell);

    /* Failed spell */
    if (rand_int(100) < chance)
    {
        message(QString("You failed to get the spell off!"));
        process_player_energy(BASE_ENERGY_MOVE);
        return;
    }

    //Actually cast the spell.
    sound(MSG_SPELL);

    if (cast_spell(MODE_SPELL_CAST, cp_ptr->spell_book, spell, dir) == NULL) return;

    /* The spell was cast */
    if (!(p_ptr->spell_flags[spell] & PY_SPELL_WORKED))
    {
        int e = s_ptr->sexp;

        /* The spell worked */
        p_ptr->spell_flags[spell] |= PY_SPELL_WORKED;

        /* Gain experience */
        gain_exp(e * s_ptr->slevel);

        /* Redraw object recall */
        p_ptr->redraw |= (PR_OBJECT);
    }

    /* Sufficient mana */
    if (s_ptr->smana <= p_ptr->csp)
    {
        /* Use some mana */
        p_ptr->csp -= s_ptr->smana;
    }

    /* Over-exert the player */
    else
    {
        int oops = s_ptr->smana - p_ptr->csp;

        /* No mana left */
        p_ptr->csp = 0;
        p_ptr->csp_frac = 0;

        /* Message */
        message(QString("You faint from the effort!"));

        /* Hack -- Bypass free action */
        (void)inc_timed(TMD_PARALYZED, randint1(5 * oops + 1), TRUE);

        /* Damage CON (possibly permanently) */
        if (rand_int(100) < 50)
        {
            bool perm = (rand_int(100) < 25);

            /* Message */
            message(QString("You have damaged your health!"));

            /* Reduce constitution */
            (void)dec_stat(A_CON, 15 + randint(10), perm);
        }
    }

    /* Redraw mana */
    p_ptr->redraw |= (PR_MANA);

    process_player_energy(BASE_ENERGY_MOVE);
}

// Learn a spell
void do_cmd_study(void)
{
    int spell;
    QString noun = cast_spell(MODE_SPELL_NOUN, cp_ptr->spell_book, 1, 0);

    if (!p_ptr->chooses_spells()) noun = QString("prayer book");

    QString prompt = (QString("Please select a %1 to study.") .arg(noun));

    int mode = BOOK_STUDY;
    bool success;
    bool cancelled;
    SpellSelectDialog(&spell, prompt, mode, &success, &cancelled);

    // Handle not having a spell to learn
    if ((!success) || (cancelled))
    {
        if (!success && !cancelled) message(QString("You have no %1s that you can study right now.") .arg(noun));
        return;
    }

    //Actually learn the spell.
    if (p_ptr->chooses_spells()) spell_learn(spell);
    else study_book(spell);
    process_player_energy(BASE_ENERGY_MOVE);
}

// Browse the available spellbooks
void do_cmd_browse(void)
{
    int spell;
    QString prompt = QString("Press OK when done browsing.");
    int mode = BOOK_BROWSE;
    bool success;
    bool cancelled;
    SpellSelectDialog(&spell, prompt, mode, &success, &cancelled);

    if (!success) message(QString("You have no books that you can read."));
}
