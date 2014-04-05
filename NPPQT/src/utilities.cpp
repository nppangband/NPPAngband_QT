#include "src/npp.h"
#include "src/qt_mainwindow.h"
#include <QtWidgets>


QString _num(int n)
{
    return QString::number(n);
}

/*
 * Returns a "rating" of x depending on y, and sets "attr" to the
 * corresponding "attribute".
 */
QString likert(int x, int y, byte *attr)
{
    /* Paranoia */
    if (y <= 0) y = 1;

    /* Negative value */
    if (x < 0)
    {
        *attr = TERM_RED;
        return ("Very Bad");
    }

    /* Analyze the value */
    switch ((x / y))
    {
        case 0:
        case 1:
        {
            *attr = TERM_RED;
            return ("Bad");
        }
        case 2:
        {
            *attr = TERM_RED;
            return ("Poor");
        }
        case 3:
        case 4:
        {
            *attr = TERM_YELLOW;
            return ("Fair");
        }
        case 5:
        {
            *attr = TERM_YELLOW;
            return ("Good");
        }
        case 6:
        {
            *attr = TERM_YELLOW;
            return ("Very Good");
        }
        case 7:
        case 8:
        {
            *attr = TERM_L_GREEN;
            return ("Excellent");
        }
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        {
            *attr = TERM_L_GREEN;
            return ("Superb");
        }
        case 14:
        case 15:
        case 16:
        case 17:
        {
            *attr = TERM_L_GREEN;
            return ("Heroic");
        }
        default:
        {
            *attr = TERM_L_GREEN;
            return ("Legendary");
        }
    }
}

// There is probably a better way with QChar, but I can't find it.
int letter_to_number (QChar let)
{
   // Make sure we are dealing with lowercase letters.
    let = let.toLower();
    if (!let.isLower()) return 0;

    for (int i = 0; i < 26; i++)
    {
        letters_and_numbers *ln_ptr =& lowercase_and_numbers[i];
        if (ln_ptr->let == let) return (ln_ptr->num);
    }

    /* all else - just return zero*/
    return 0;
}

// There is probably a better way with QChar, but I can't find it.
QChar number_to_letter (int num)
{
    for (int i = 0; i < 26; i++)
    {
        letters_and_numbers *ln_ptr =& lowercase_and_numbers[i];
        if (ln_ptr->num == num) return (ln_ptr->let);
    }

    /* all else - just return zero*/
    return 0;

    /* all else - Paranoia*/
    return ('a');
}

bool is_a_vowel(QChar single_letter)
{
    // Make sure we are dealing with lowercase letters.
    single_letter = single_letter.toLower();
    if (!single_letter.isLower()) return FALSE;

    if (single_letter == 'a') return TRUE;
    if (single_letter == 'e') return TRUE;
    if (single_letter == 'i') return TRUE;
    if (single_letter == 'o') return TRUE;
    if (single_letter == 'u') return TRUE;

    return (FALSE);
}

bool begins_with_vowel(QString line)
{
    // Paranoia
    if (line.isEmpty()) return (FALSE);
    QChar first = line[0];
    return (is_a_vowel(first));
}

// Capitilize the first character in a string.
QString capitalize_first(QString line)
{
    // Paranoia
    if (line.isEmpty()) return (line);
    QChar first = line[0];
    first = first.toUpper();
    line[0] = first;
    return (line);

}

void pop_up_message_box(QString message, QMessageBox::Icon the_icon)
{
    QMessageBox msg_box;
    msg_box.setModal(true);
    msg_box.setIcon(the_icon);
    msg_box.setText(message);
    msg_box.exec();
}

void popup1(QString message, int id)
{
    static QList<int> keys;
    if (keys.contains(id)) return;
    keys.append(id);
    pop_up_message_box(message);
}

// Post a question to the player and wait for a yes/no response.
// return TRUE/FALSE;
bool get_check(QString question)
{
    int answer = QMessageBox::question(0, "Please respond yes or no", question, QMessageBox::Yes, QMessageBox::No);
    if (answer == QMessageBox::Yes) return (TRUE);
    return (FALSE);
}

// Post a question to the player and wait for player to enter a string.
// returns the string;
QString get_string(QString question, QString description, QString answer)
{

    bool ok;
    QString text = QInputDialog::getText(0, question, description, QLineEdit::Normal, answer, &ok, Qt::Dialog, 0);

    if (!ok) return NULL;

    return (text);
}

/*
 * Request a "quantity" from the user
 *
 * Allow "p_ptr->command_arg" to specify a quantity
 */
s16b get_quantity(QString prompt, int max, int amt)
{
    /* Use "command_arg" */
    if (p_ptr->command_arg)
    {
        /* Extract a number */
        amt = p_ptr->command_arg;

        /* Clear "command_arg" */
        p_ptr->command_arg = 0;

        /* Enforce the maximum */
        if (amt > max) amt = max;

        /* Enforce the minimum */
        if (amt < 0) amt = 0;

        return (amt);
    }

    if (max > 1)
    {
        bool ok;

        /* Build a prompt if needed */
        if (prompt.isEmpty())
        {
            /* Build a prompt */
            prompt = (QString("Please enter a quantity (0-%1)") .arg(max));
        }
        else prompt.append(QString(" (0-%1)") .arg(max));

        // Input dialog (this, title, prompt, initial value, min, max, step, ok, flags)
        amt = QInputDialog::getInt(0, "Please enter a number", prompt, amt, 1, max, 1, &ok, 0);

        if (!ok) return (0);
    }

    /* Enforce the maximum */
    if (amt > max) amt = max;

    /* Enforce the minimum */
    if (amt < 0) amt = 0;

    /* Return the result */
    return (amt);
}


QColor add_preset_color(int which_color)
{
    QColor color;

    color.setRgb(preset_colors[which_color].red, preset_colors[which_color].green, preset_colors[which_color].blue, 255);

    return (color);
}

/*
 *  Add a message - assume the color of white
 *This should be the only function to add to the message list, to make sure
 *it never gets larger than 250 messages
 */
static void add_message_to_vector(QString msg, QColor which_color)
{
    message_type message_body;
    message_type *msg_ptr = &message_body;

    // First make sure the message list gets no greater than 200
    while (message_list.size() >= 200)
    {
        message_list.removeLast();
    }

    // Default is a while message
    msg_ptr->msg_color = which_color;

    msg_ptr->message = msg;
    msg_ptr->message_turn = turn;

    // Add the message at the beginning of the list
    message_list.prepend(message_body);

    ui_show_message(0);
}

/*
 *  Add a message with a preset color.
 */
void color_message(QString msg, int which_color)
{
    QColor msg_color;

    //Paranoia
    if (which_color > MAX_COLORS) which_color = TERM_WHITE;

    // Default is a while message
    msg_color = defined_colors[which_color];

    add_message_to_vector(msg, msg_color);
}

/*
 *  returns a QString with a preset color
 *  Colors are in 24-bit hex RGB format (#000000 - #FFFFFF)
 *  Keep this current with preset_colors table in tables.c.
 */
QString color_string(QString msg, int which_color)
{
    QColor msg_color;

    //Paranoia
    if (which_color > MAX_COLORS) which_color = TERM_WHITE;

    switch (which_color)
    {
        case TERM_DARK :            return (QString("<font color='#000000'>%1</font>") .arg(msg));
        case TERM_L_DARK :          return (QString("<font color='#606060'>%1</font>") .arg(msg));
        case TERM_SLATE :           return (QString("<font color='#808080'>%1</font>") .arg(msg));
        case TERM_SLATE_GRAY :      return (QString("<font color='#708090'>%1</font>") .arg(msg));
        case TERM_TAUPE :           return (QString("<font color='#8B8589'>%1</font>") .arg(msg));
        case TERM_WHITE :           return (QString("<font color='#FFFFFF'>%1</font>") .arg(msg));
        case TERM_LIGHT_GRAY :      return (QString("<font color='#C0C0C0'>%1</font>") .arg(msg));
        case TERM_SNOW_WHITE :      return (QString("<font color='#FFFAFA'>%1</font>") .arg(msg));
        case TERM_RED :             return (QString("<font color='#C00000'>%1</font>") .arg(msg));
        case TERM_L_RED :           return (QString("<font color='#FF4040'>%1</font>") .arg(msg));
        case TERM_RED_LAVA :        return (QString("<font color='#CF1020'>%1</font>") .arg(msg));
        case TERM_RASPBERRY :       return (QString("<font color='#E30B5C'>%1</font>") .arg(msg));
        case TERM_RED_RUST :        return (QString("<font color='#B7410E'>%1</font>") .arg(msg));
        case TERM_PINK :            return (QString("<font color='#FF1493'>%1</font>") .arg(msg));
        case TERM_ORANGE :          return (QString("<font color='#FF8000'>%1</font>") .arg(msg));
        case TERM_MAHAGONY :        return (QString("<font color='#C04000'>%1</font>") .arg(msg));
        case TERM_GREEN :           return (QString("<font color='#008040'>%1</font>") .arg(msg));
        case TERM_L_GREEN :         return (QString("<font color='#00FF00'>%1</font>") .arg(msg));
        case TERM_JUNGLE_GREEN :    return (QString("<font color='#29AB87'>%1</font>") .arg(msg));
        case TERM_LIME_GREEN :      return (QString("<font color='#BFFF00'>%1</font>") .arg(msg));
        case TERM_BLUE :            return (QString("<font color='#0040FF'>%1</font>") .arg(msg));
        case TERM_L_BLUE :          return (QString("<font color='#00FFFF'>%1</font>") .arg(msg));
        case TERM_NAVY_BLUE :       return (QString("<font color='#4C4CA6'>%1</font>") .arg(msg));
        case TERM_SKY_BLUE :        return (QString("<font color='#00BFFF'>%1</font>") .arg(msg));
        case TERM_UMBER :           return (QString("<font color='#804000'>%1</font>") .arg(msg));
        case TERM_L_UMBER :         return (QString("<font color='#C08040'>%1</font>") .arg(msg));
        case TERM_AUBURN :          return (QString("<font color='#6D351A'>%1</font>") .arg(msg));
        case TERM_L_BROWN :         return (QString("<font color='#C19A6B'>%1</font>") .arg(msg));
        case TERM_YELLOW :          return (QString("<font color='#FFFF00'>%1</font>") .arg(msg));
        case TERM_EARTH_YELLOW :    return (QString("<font color='#FFFF00'>%1</font>") .arg(msg));
        case TERM_MAIZE :           return (QString("<font color='#FBEC5D'>%1</font>") .arg(msg));
        case TERM_VIOLET :          return (QString("<font color='#FF00FF'>%1</font>") .arg(msg));
        case TERM_PURPLE :          return (QString("<font color='#A500FF'>%1</font>") .arg(msg));
        case TERM_GOLD :            return (QString("<font color='#FFD700'>%1</font>") .arg(msg));
        case TERM_SILVER :          return (QString("<font color='#C0C0C0'>%1</font>") .arg(msg));
        case TERM_COPPER :          return (QString("<font color='#B87333'>%1</font>") .arg(msg));
        default: break;
    }

    // handle invalid colors
    return (QString("<font color='#FFFFFF'> %1</font>") .arg(msg));

}

//  Add a message - assume the color of white
void message(QString msg)
{
    add_message_to_vector(msg, add_preset_color(TERM_WHITE));
}

//  Add a message with any 24 bit color
void custom_color_message(QString msg, byte red, byte green, byte blue)
{
    QColor msg_color;

    msg_color.setRgb(red, green, blue, 255);

    add_message_to_vector(msg, msg_color);
}

static bool repeat_prev_allowed;

void cmd_enable_repeat(void)
{
    repeat_prev_allowed = TRUE;
}

void cmd_disable_repeat(void)
{
    repeat_prev_allowed = FALSE;
}

/*
 * Return object weight in a "XX.X" format.
 * The output is automatically formatted to be 6 characters long.
 */
QString format_object_weight(object_type *o_ptr)
{
    int object_weight = o_ptr->weight * o_ptr->number;

    QString formatted_weight = (QString("%1.%2") .arg(object_weight/10) .arg(object_weight % 10));

    while (formatted_weight.length() < 6) formatted_weight.prepend(" ");
    if (object_weight == 10) formatted_weight.append(" lb ");
    else formatted_weight.append(" lbs");

    return (formatted_weight);
}

/*
 * Angband sorting algorithm -- quick sort in place
 *
 * Note that the details of the data we are sorting is hidden,
 * and we rely on the "ang_sort_comp()" and "ang_sort_swap()"
 * function hooks to interact with the data, which is given as
 * two pointers, and which may have any user-defined form.
 */
static void ang_sort_aux(void *u, void *v, int p, int q)
{
    int z, a, b;

    /* Done sort */
    if (p >= q) return;

    /* Pivot */
    z = p;

    /* Begin */
    a = p;
    b = q;

    /* Partition */
    while (TRUE)
    {
        /* Slide i2 */
        while (!(*ang_sort_comp)(u, v, b, z)) b--;

        /* Slide i1 */
        while (!(*ang_sort_comp)(u, v, z, a)) a++;

        /* Done partition */
        if (a >= b) break;

        /* Swap */
        (*ang_sort_swap)(u, v, a, b);

        /* Advance */
        a++, b--;
    }

    /* Recurse left side */
    ang_sort_aux(u, v, p, b);

    /* Recurse right side */
    ang_sort_aux(u, v, b+1, q);
}


/*
 * Angband sorting algorithm -- quick sort in place
 *
 * Note that the details of the data we are sorting is hidden,
 * and we rely on the "ang_sort_comp()" and "ang_sort_swap()"
 * function hooks to interact with the data, which is given as
 * two pointers, and which may have any user-defined form.
 */
void ang_sort(void *u, void *v, int n)
{
    /* Sort the array */
    ang_sort_aux(u, v, 0, n-1);
}


// Write a note to the notes file
void write_note(QString note, s16b depth)
{
    (void)note;
    (void)depth;
    // TODO write note
}

QString get_player_title(void)
{
    if (game_mode == GAME_NPPMORIA)
    {
        return (cp_ptr->cl_title[p_ptr->lev-1]);
    }

    return (cp_ptr->cl_title[(p_ptr->lev - 1) / 5]);
}

void debug_rarities()
{
    if (!character_dungeon) return;

    // TODO PLAYTESTING
    int n = 0;
    for (int y = 0; y < p_ptr->cur_map_hgt; y++) {
        for (int x = 0; x < p_ptr->cur_map_wid; x++) {
            if (dungeon_info[y][x].feat == 0) {
                ++n;
                color_message(QString("NONE terrain %1x%2").arg(y).arg(x), TERM_RED);
            }
        }
    }

    /*
    n = 0;
    for (int i = 1; i < z_info->f_max; i++) {
        feature_type *f_ptr = f_info + i;
        if ((f_ptr->f_flags1 & FF1_DOOR) && (f_ptr->f_flags3 & FF3_DOOR_LOCKED)) {
            ++n;
        }
    }
    if (n == 0) color_message("There is not locked doors", TERM_RED);

    for (int i = 0; i < z_info->art_max; i++) {
        artifact_type *a_ptr = a_info + i;
        if (a_ptr->tval == 0 and a_ptr->sval) {
            popup1(QString("Invalid artifact: %1 %2").arg(a_ptr->a_name).arg(a_ptr->sval), 53);
            break;
        }
    }

    n = 0;
    for (int i = 0; i < mon_max; i++) {
        monster_type *m_ptr = mon_list + i;
        if (m_ptr->r_idx == 0) continue;
        monster_race *r_ptr = r_info + m_ptr->r_idx;
        if (r_ptr->level == 0) ++n;
    }
    if (n > 0) message(QString("monsters: %1").arg(n));

    if (monster_level == 0 && p_ptr->depth > 0) message("?????");
    */
}

// Make a color darker if it's too bright
QColor make_color_readable(QColor clr)
{
    // Gray
    if (clr.red() == clr.green() && clr.green() == clr.blue()) {
        return QColor("black");
    }
    clr = clr.toHsv();
    int value = MIN(clr.value(), 150);
    int saturation = 255;
    clr.setHsv(clr.hue(), saturation, value, 255);
    return clr.toRgb();
}

// For object listings
QColor get_object_color(object_type *o_ptr)
{
    int idx = tval_to_attr[o_ptr->tval];
    QColor clr = defined_colors[idx];
    return make_color_readable(clr);
}


/*
 * Add a pval so the object descriptions don't look strange*
 */
void apply_magic_fake(object_type *o_ptr)
{
    /* Analyze type */
    switch (o_ptr->tval)
    {
        case TV_DIGGING:
        {
            o_ptr->pval = 1;
            break;
        }

        /*many rings need a pval*/
        case TV_RING:
        {
            switch (o_ptr->sval)
            {
                case SV_RING_STR:
                case SV_RING_CON:
                case SV_RING_DEX:
                case SV_RING_INT:
                case SV_RING_SPEED:
                case SV_RING_SEARCHING:
                {
                    o_ptr->pval = 1;
                    break;
                }

                case SV_RING_AGGRAVATION:
                {
                    o_ptr->ident |= (IDENT_CURSED);
                    break;
                }
                case SV_RING_WEAKNESS:
                case SV_RING_STUPIDITY:
                {
                    /* Broken */
                    o_ptr->ident |= (IDENT_BROKEN);

                    /* Cursed */
                    o_ptr->ident |= (IDENT_CURSED);

                    /* Penalize */
                    o_ptr->pval = -1;

                    break;
                }
                /* WOE */
                case SV_RING_WOE:
                {
                    /* Broken */
                    o_ptr->ident |= (IDENT_BROKEN);

                    /* Cursed */
                    o_ptr->ident |= (IDENT_CURSED);

                    /* Penalize */
                    o_ptr->to_a = -1;
                    o_ptr->pval = -1;

                    break;
                }
                /* Ring that increase damage */
                case SV_RING_DAMAGE:
                {
                    /* Bonus to damage */
                    o_ptr->to_d = 1;

                    break;
                }
                /* Ring that increase accuracy */
                case SV_RING_ACCURACY:
                {
                    /* Bonus to hit */
                    o_ptr->to_h = 1;

                    break;
                }
                /* Rings that provide of Protection */
                case SV_RING_PROTECTION:
                case SV_RING_FLAMES:
                case SV_RING_ACID:
                case SV_RING_ICE:
                case SV_RING_LIGHTNING:
                {
                    /* Bonus to armor class */
                    o_ptr->to_a = 1;

                    break;
                }
                /* Rings that provide of Protection */
                case SV_RING_LORD_PROT_ACID:
                case SV_RING_LORD_PROT_FIRE:
                case SV_RING_LORD_PROT_COLD:
                {
                    /* Bonus to armor class */
                    o_ptr->to_a = 5;

                    break;
                }
                /*both to-hit and to-damage*/
                case SV_RING_SLAYING:
                {
                    /* Bonus to damage and to hit */
                    o_ptr->to_d = 1;
                    o_ptr->to_h = 1;

                    break;
                }
                default: break;

            }
            /*break for TVAL-Rings*/
            break;
        }

        case TV_AMULET:
        {
            /* Analyze */
            switch (o_ptr->sval)
            {
                /* Amulet of wisdom/charisma/infravision */
                case SV_AMULET_WISDOM:
                case SV_AMULET_CHARISMA:
                case SV_AMULET_INFRAVISION:
                case SV_AMULET_SEARCHING:
                case SV_AMULET_ESP:
                case SV_AMULET_DEVOTION:
                case SV_AMULET_TRICKERY:
                {
                    /* Stat bonus */
                    o_ptr->pval = 1;

                    break;
                }

                /* Amulet of the Magi -- never cursed */
                case SV_AMULET_THE_MAGI:
                {
                    o_ptr->pval = 1;
                    o_ptr->to_a = 1;

                    break;
                }

                /* Amulet of Weaponmastery -- never cursed */
                case SV_AMULET_WEAPONMASTERY:
                {
                    o_ptr->to_h = 1;
                    o_ptr->to_d = 1;
                    o_ptr->pval = 1;

                    break;
                }

                /* Amulet of Doom -- always cursed */
                case SV_AMULET_DOOM:
                case SV_AMULET_WOE:
                {
                    /* Broken */
                    o_ptr->ident |= (IDENT_BROKEN);

                    /* Cursed */
                    o_ptr->ident |= (IDENT_CURSED);

                    /* Penalize */
                    o_ptr->pval = -1;
                    o_ptr->to_a = -1;

                    break;
                }

                default: break;

            }
            /*break for TVAL-Amulets*/
            break;
        }

        case TV_LIGHT:
        {
            /* Analyze */
            switch (o_ptr->sval)
            {
                case SV_LIGHT_TORCH:
                case SV_LIGHT_LANTERN:
                {
                    o_ptr->timeout = 1;

                    break;
                }

            }
            /*break for TVAL-Lites*/
            break;
        }

        /*give then one charge*/
        case TV_STAFF:
        case TV_WAND:
        {
            o_ptr->pval = 1;

            break;
        }
    }
}


static QString tile_obj_name_convert(QString orig_name)
{
    // first, make it standard ASCII, then lowercase
    orig_name = to_ascii(orig_name);
    orig_name = orig_name.toLower();

    //now delete all commas, and then replace spaces with '_', then start with "mon_".
    orig_name.remove(QChar(','));
    orig_name.remove(QString("'"));
    orig_name.remove(QChar('['));
    orig_name.remove(QChar(']'));
    orig_name.replace(QChar(' '), QChar('_'));
    orig_name.prepend("obj_");
    return (orig_name);
}


static QString tile_mon_name_convert(QString orig_name)
{
    // first, make it standard ASCII, then lowercase
    orig_name = to_ascii(orig_name);
    orig_name = orig_name.toLower();

    //now delete all commas, and then replace spaces with '_', then start with "mon_".
    orig_name.remove(QChar(','));
    orig_name.remove(QString("'"));
    orig_name.replace(QChar(' '), QChar('_'));
    orig_name.prepend("mon_");
    return (orig_name);
}

static QString tile_feat_name_convert(QString orig_name)
{
    // first, make it standard ASCII, then lowercase
    orig_name = to_ascii(orig_name);
    orig_name = orig_name.toLower();

    //now delete all commas, and then replace spaces with '_', then start with "mon_".
    orig_name.remove(QChar(','));
    orig_name.remove(QChar('('));
    orig_name.remove(QChar(')'));
    orig_name.remove(QString("'"));
    orig_name.replace(QChar(' '), QChar('_'));
    orig_name.prepend("feat_");
    return (orig_name);
}

static QString tile_flav_name_convert(QString orig_name, int tval)
{
    // first, make it standard ASCII, then lowercase
    orig_name = to_ascii(orig_name);
    orig_name = orig_name.toLower();

    //now delete all commas, and then replace spaces with '_', then start with "mon_".
    orig_name.remove(QChar(','));
    orig_name.remove(QChar('('));
    orig_name.remove(QChar(')'));
    orig_name.remove(QString("'"));
    orig_name.replace(QChar(' '), QChar('_'));
    if (tval == TV_RING) orig_name.prepend("ring_");
    else if (tval == TV_AMULET) orig_name.prepend("amulet_");
    else if (tval == TV_STAFF) orig_name.prepend("staff_");
    else if (tval == TV_WAND) orig_name.prepend("wand_");
    else if (tval == TV_ROD) orig_name.prepend("rod_");
    else if (tval == TV_FOOD) orig_name.prepend("mushroom_");
    else if (tval == TV_POTION) orig_name.prepend("potion_");
    else if (tval == TV_SCROLL) orig_name.prepend("scroll_");
    orig_name.prepend("flav_");
    return (orig_name);
}



void extract_tiles(void)
{
    int i;
    QString tile_id;
    QString file_name;

    for (i = 0; i < z_info->r_max; i++)\
    {
        monster_race *r_ptr = &r_info[i];
        if (r_ptr->r_name_full.isEmpty()) continue;
        tile_id = r_ptr->tile_id;
        QString race_name = monster_desc_race(i);
        if (r_ptr->flags1 & (RF1_FRIEND | RF1_FRIENDS)) race_name = plural_aux(race_name);
        race_name = tile_mon_name_convert(race_name);
        QPixmap pix = ui_get_tile(tile_id);
        QFile tile_file;
        tile_file.setFileName(QString("%1%2.png" ) .arg(NPP_DIR_GRAF) .arg(race_name));
        tile_file.open(QIODevice::WriteOnly);
        pix.save(&tile_file, "PNG");
    }
    for (i = 0; i < z_info->k_max; i++)\
    {
        object_kind *k_ptr = &k_info[i];
        if (k_ptr->k_name.isEmpty()) continue;
        tile_id = k_ptr->tile_id;
        object_type object_type_body;
        object_type *o_ptr = &object_type_body;
        /* Check for known artifacts, display them as artifacts */
        if (k_ptr->k_flags3 & (TR3_INSTA_ART))
        {
            make_fake_artifact(o_ptr, i);
            object_aware(o_ptr);
            object_known(o_ptr);
            o_ptr->ident |= (IDENT_MENTAL);
        }
        else
        {
            o_ptr->object_wipe();
            object_prep(o_ptr, i);
            apply_magic_fake(o_ptr);
            o_ptr->ident |= (IDENT_STORE);
            if (!k_info[i].flavor)
            {
                /* Mark the item as fully known */
                o_ptr->ident |= (IDENT_MENTAL | IDENT_STORE);
                object_aware(o_ptr);
                object_known(o_ptr);
            }
        }

        QString object_name;
        if (o_ptr->tval == TV_GOLD) object_name = strip_name(i);
        else object_name = object_desc(o_ptr, ODESC_BASE);
        object_name = tile_obj_name_convert(object_name);
        QPixmap pix = ui_get_tile(tile_id);
        QFile tile_file;
        tile_file.setFileName(QString("%1%2.png" ) .arg(NPP_DIR_GRAF) .arg(object_name));
        tile_file.open(QIODevice::WriteOnly);
        pix.save(&tile_file, "PNG");
    }
    for (i = 0; i < z_info->f_max; i++)
    {
        feature_type *f_ptr = &f_info[i];
        if (f_ptr->f_name.isEmpty()) continue;
        tile_id = f_ptr->tile_id;
        QString feat_name = feature_desc(i, FALSE, FALSE);
        feat_name = tile_feat_name_convert(feat_name);
        QPixmap pix = ui_get_tile(tile_id);
        QFile tile_file;
        tile_file.setFileName(QString("%1%2.png" ) .arg(NPP_DIR_GRAF) .arg(feat_name));
        tile_file.open(QIODevice::WriteOnly);
        pix.save(&tile_file, "PNG");
    }
    for (i = 0; i < z_info->flavor_max; i++)
    {
        flavor_type *flavor_ptr = &flavor_info[i];
        if (flavor_ptr->text.isEmpty() && flavor_ptr->tval != TV_SCROLL) continue;
        tile_id = flavor_ptr->tile_id;
        QString flavor_name = flavor_ptr->text;
        flavor_name = tile_flav_name_convert(flavor_name, flavor_ptr->tval);
        QPixmap pix = ui_get_tile(tile_id);
        QFile tile_file;
        tile_file.setFileName(QString("%1%2.png" ) .arg(NPP_DIR_GRAF) .arg(flavor_name));
        tile_file.open(QIODevice::WriteOnly);
        pix.save(&tile_file, "PNG");
    }
    for (i = 0; i < z_info->p_max; i++)
    {
        p_ptr->prace = i;
        QString race_name = to_ascii(p_info[p_ptr->prace].pr_name);
        race_name = race_name.toLower();
        for (int j = 0; j < z_info->c_max; j++)
        {
            p_ptr->pclass = j;
            QString class_name = to_ascii(c_info[p_ptr->pclass].cl_name);
            class_name = class_name.toLower();
            init_graphics();
            tile_id = p_ptr->tile_id;
            QPixmap pix = ui_get_tile(tile_id);
            QFile tile_file;
            tile_file.setFileName(QString("%1player_%2_%3.png" ) .arg(NPP_DIR_GRAF) .arg(race_name) .arg(class_name));
            tile_file.open(QIODevice::WriteOnly);
            pix.save(&tile_file, "PNG");
        }
    }
    if (use_graphics == GRAPHICS_DAVID_GERVAIS)
    {
        tile_id = QString("%1x%2").arg(0x87 & 0x7F).arg(0xB7 & 0x7F);
        QPixmap pix = ui_get_tile(tile_id);
        QFile tile_file;
        tile_file.setFileName(QString("%1obj_pile.png") .arg(NPP_DIR_GRAF));
        tile_file.open(QIODevice::WriteOnly);
        pix.save(&tile_file, "PNG");
    }
}

// Display an actual window with the information, sybmol, and tile
void display_info_window(byte mode, int index, QString info, object_type *o_ptr)
{
    QString tile_id;
    QMessageBox message_box;
    message_box.setText(info);
    message_box.setStandardButtons(QMessageBox::Ok);
    message_box.setDefaultButton(QMessageBox::Ok);
    message_box.setInformativeText("Press 'OK' to continue.");

    //Get the pixmap, depending on if we are displaying an object, terrain, or monster.
    if (mode == DISPLAY_INFO_FEATURE)
    {
        index = f_info[index].f_mimic;
        feature_type *f_ptr = &f_info[index];
        tile_id = f_ptr->tile_id;
    }
    else if (mode == DISPLAY_INFO_MONSTER)
    {
        monster_race *r_ptr = &r_info[index];
        tile_id = r_ptr->tile_id;
    }
    else if (mode == DISPLAY_INFO_OBJECT)
    {
        object_kind *k_ptr = &k_info[index];
        if (use_flavor_glyph(o_ptr)) {
            tile_id = flavor_info[k_ptr->flavor].tile_id;
        }
        else {
            tile_id = k_ptr->tile_id;
        }
    }
    // Whoops!
    else return;
    message_box.setIconPixmap(ui_get_tile(tile_id));

    message_box.show();
    message_box.exec();

}

class Repl {
public:
    QString what;
    QString with;
    Repl(QString a, QString b);
};

Repl::Repl(QString a, QString b)
{
    what = a;
    with = b;
}

QString to_ascii(QString src)
{
    Repl items[] = {
        Repl("áäâà", "a"),
        Repl("éëêè", "e"),
        Repl("íîïì", "i"),
        Repl("óöôò", "o"),
        Repl("úûüù", "u"),
        Repl("ñ", "n"),
        Repl("", "")
    };

    for (int i = 0; !items[i].what.isEmpty(); i++) {
        QString what = items[i].what;
        QString with = items[i].with;
        for (int j = 0; j < what.size(); j++) {
            src = src.replace(what[j], with[0]);
            src = src.replace(what[j].toUpper(), with[0].toUpper()); // Handle uppercase letters
        }
    }

    return src;
}
