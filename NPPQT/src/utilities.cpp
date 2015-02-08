#include "src/npp.h"
#include "src/qt_mainwindow.h"
#include "src/utilities.h"
#include "src/knowledge.h"
#include "tilebag.h"
#include "src/init.h"
#include <QInputDialog>
#include <QLineEdit>


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
    if (num >=26) return ('a');

    for (int i = 0; i < 26; i++)
    {
        letters_and_numbers *ln_ptr =& lowercase_and_numbers[i];
        if (ln_ptr->num == num) return (ln_ptr->let);
    }

    /* all else - Paranoia*/
    return ('a');
}

//Convert a long number to string number formatted with commas
QString number_to_formatted_string(s32b number)
{
    bool is_negative = TRUE;
    if (number >= 0) is_negative = FALSE;

    QString formatted_num;
    formatted_num.clear();

    //preserve the original number
    s32b working_num = number;
    if (working_num < 0) working_num = working_num * -1;

    while (working_num >=1000)
    {
        s32b remainder = (working_num % 1000);
        QString temp_string = (QString("%1") .arg(remainder));
        if (remainder < 100) temp_string.prepend('0');
        if (remainder < 10)  temp_string.prepend('0');
        temp_string.prepend(",");
        working_num = working_num / 1000;

        formatted_num.prepend(temp_string);
    }

    formatted_num.prepend(QString("%1") .arg(working_num));
    if (is_negative) formatted_num.prepend('-');

    return(formatted_num);
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
 */
s16b get_quantity(QString prompt, int max, int amt, bool allow_zero)
{
    if (!amt) amt = 1;
    int min = 1;
    if (allow_zero) min = 0;
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
        amt = QInputDialog::getInt(0, "Please enter a number", prompt, amt, min, max, 1, &ok, 0);

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
 * Add a message
 * This should be the only function to add to the message list, to make sure
 * it never gets larger than 200 messages.
 */
static void add_message_to_vector(QString msg, QColor which_color)
{
    message_type message_body;
    message_type *msg_ptr = &message_body;

    bool add_message = TRUE;

    // First make sure the message list gets no greater than 200
    while (message_list.size() >= 400)
    {
        message_list.removeLast();
    }

    // Without this check, the game will crash when adding the first message
    if (!message_list.empty())
    {
        // Point to the last message
        message_type *msg_one = &message_list[0];

        if (operator==(msg_one->message, msg) && (msg_one->msg_color == which_color) &&
            (msg_one->message_turn == p_ptr->game_turn) && !p_ptr->message_append)
        {
            msg_one->repeats++;
            add_message = FALSE;
        }
    }

    if (add_message)
    {
        // Default is a while message
        msg_ptr->msg_color = which_color;

        msg_ptr->message = msg;
        msg_ptr->message_turn = p_ptr->game_turn;
        msg_ptr->repeats = 1;
        if (!p_ptr->message_append) msg_ptr->append = FALSE;
        else if (p_ptr->message_first_append)
        {
            msg_ptr->append = FALSE;
            p_ptr->message_first_append = FALSE;
        }
        else msg_ptr->append = TRUE;

        // Add the message at the beginning of the list
        message_list.prepend(message_body);
    }
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


// Returns a QString in any 16 bit color, in HTML format
QString color_string_16bit(QString msg, QColor which_color)
{
    return (QString("<font color='%1'>%2</font>").arg(which_color.name()).arg(msg));
}

/*
 *  returns a QString with a preset color
 *  Colors are in 24-bit hex RGB format (#000000 - #FFFFFF)
 *  Keep this current with preset_colors table in tables.c.
 */
QString color_string(QString msg, byte color_num)
{
    // Paranoia
    if (color_num >= MAX_COLORS) color_num = TERM_WHITE;

    QColor msg_color = defined_colors[color_num];

    return (color_string_16bit(msg, msg_color));
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

QString formatted_weight_string(s32b weight)
{
    return (QString("%1.%2") .arg(weight / 10) .arg(weight % 10));
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



QString get_player_title(void)
{
    if(p_ptr->is_wizard) return ("--WIZARD--");

    if (p_ptr->total_winner) return(sp_ptr->winner);

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





static QString tile_obj_name_convert(QString orig_name)
{
    // first, make it standard ASCII, then lowercase
    orig_name = to_ascii(orig_name);
    orig_name = orig_name.toLower();

    //now delete all commas, and then replace spaces with '_', then start with "obj_".
    orig_name.remove(QChar(','));
    orig_name.remove(QString("'"));
    orig_name.remove(QChar('['));
    orig_name.remove(QChar(']'));
    orig_name.remove(QChar('('));
    orig_name.remove(QChar(')'));
    orig_name.remove(QChar('/'));
    orig_name.replace(QChar(' '), QChar('_'));
    orig_name.replace("*", "_star_");
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

static QPixmap old_get_tile(int y32, int x32, int y8, int x8)
{
    if (!use_graphics) return ui_make_blank();

    static int cur_mode = 0;
    static QPixmap *map = 0;

    if (use_graphics != cur_mode) {
        if (map) delete map;
        map = 0;

        cur_mode = use_graphics;

        if (cur_mode == GRAPHICS_DAVID_GERVAIS)
        {
            map = new QPixmap(npp_dir_graf.absoluteFilePath("32x32.png"));
        }
        else
        {
            map = new QPixmap(npp_dir_graf.absoluteFilePath("8x8.png"));
        }
    }

    int w = 8;
    int h = 8;
    int x = x8;
    int y = y8;

    if (cur_mode == GRAPHICS_DAVID_GERVAIS) {
        w = 32;
        h = 32;
        x = x32;
        y = y32;
    }

    return map->copy(x * w, y * h, w, h);
}

void extract_tiles(bool save)
{
    int i;
    if (save)
    {
        npp_dir_graf.mkdir("tiles");
        npp_dir_graf.cd("tiles");
    }

    for (i = 0; i < z_info->r_max; i++)\
    {
        monster_race *r_ptr = &r_info[i];
        if (r_ptr->r_name_full.isEmpty()) continue;
        QString race_name = monster_desc_race(i);
        if (r_ptr->flags1 & (RF1_FRIEND | RF1_FRIENDS)) race_name = plural_aux(race_name);
        race_name = tile_mon_name_convert(race_name);
        if (save) {
            QPixmap pix = old_get_tile(r_ptr->tile_32x32_y, r_ptr->tile_32x32_x,
                                       r_ptr->tile_8x8_y, r_ptr->tile_8x8_x);
            QFile tile_file(npp_dir_graf.absoluteFilePath(race_name + ".png"));
            tile_file.open(QIODevice::WriteOnly);
            pix.save(&tile_file, "PNG");
        }
        else {
            r_ptr->tile_id = race_name;
        }
    }
    for (i = 0; i < z_info->k_max; i++)\
    {
        object_kind *k_ptr = &k_info[i];
        if (k_ptr->k_name.isEmpty()) continue;
        object_type object_type_body;
        object_type *o_ptr = &object_type_body;
        /* Check for known artifacts, display them as artifacts */
        if (k_ptr->k_flags3 & (TR3_INSTA_ART))
        {
            int art_num = i;
            if (game_mode == GAME_NPPANGBAND) {
                if (k_ptr->tval == TV_HAFTED && k_ptr->sval == SV_GROND) art_num = ART_GROND;
                if (k_ptr->tval == TV_CROWN && k_ptr->sval == SV_MORGOTH) art_num = ART_MORGOTH;
            }
            make_fake_artifact(o_ptr, art_num);
            object_aware(o_ptr);
            object_known(o_ptr);
            o_ptr->ident |= (IDENT_MENTAL);
            o_ptr->update_object_flags();
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
                o_ptr->update_object_flags();
            }
        }

        QString object_name;
        if (o_ptr->tval == TV_GOLD) object_name = strip_name(i);
        else object_name = object_desc(o_ptr, ODESC_BASE);
        object_name = tile_obj_name_convert(object_name);
        if (save) {
            QPixmap pix = old_get_tile(k_ptr->tile_32x32_y, k_ptr->tile_32x32_x,
                                       k_ptr->tile_8x8_y, k_ptr->tile_8x8_x);
            QFile tile_file(npp_dir_graf.absoluteFilePath(object_name + ".png"));
            //if (object_name == "obj_nothing") qDebug("nothing: %d", i);
            tile_file.open(QIODevice::WriteOnly);
            pix.save(&tile_file, "PNG");
        }
        else {
            k_ptr->tile_id = object_name;
        }
    }
    for (i = 0; i < z_info->f_max; i++)
    {
        feature_type *f_ptr = &f_info[i];
        if (f_ptr->f_name.isEmpty()) continue;
        QString feat_name = feature_desc(i, FALSE, FALSE);
        feat_name = tile_feat_name_convert(feat_name);
        if (save) {
            QPixmap pix = old_get_tile(f_ptr->tile_32x32_y, f_ptr->tile_32x32_x,
                                       f_ptr->tile_8x8_y, f_ptr->tile_8x8_x);
            QFile tile_file(npp_dir_graf.absoluteFilePath(feat_name + ".png"));
            tile_file.open(QIODevice::WriteOnly);
            pix.save(&tile_file, "PNG");
        }
        else {
            f_ptr->tile_id = feat_name;
        }
    }
    for (i = 0; i < z_info->flavor_max; i++)
    {
        flavor_type *flavor_ptr = &flavor_info[i];
        if (flavor_ptr->text.isEmpty() && flavor_ptr->tval != TV_SCROLL) continue;
        QString flavor_name = flavor_ptr->text;
        flavor_name = tile_flav_name_convert(flavor_name, flavor_ptr->tval);
        if (save) {
            QPixmap pix = old_get_tile(flavor_ptr->tile_32x32_y, flavor_ptr->tile_32x32_x,
                                       flavor_ptr->tile_8x8_y, flavor_ptr->tile_8x8_x);
            QFile tile_file(npp_dir_graf.absoluteFilePath(flavor_name + ".png"));
            tile_file.open(QIODevice::WriteOnly);
            pix.save(&tile_file, "PNG");
        }
        else {
            flavor_ptr->tile_id = flavor_name;
        }
    }
    if (save) {
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
                QPixmap pix = old_get_tile(p_ptr->tile_32x32_y, p_ptr->tile_32x32_x,
                                           p_ptr->tile_8x8_y, p_ptr->tile_8x8_x);
                QFile tile_file(npp_dir_graf.absoluteFilePath("player_" + race_name + "_" + class_name + ".png"));
                tile_file.open(QIODevice::WriteOnly);
                pix.save(&tile_file, "PNG");
            }
        }
    }
    else {
        QString race_name = to_ascii(p_info[p_ptr->prace].pr_name).toLower();
        QString class_name = to_ascii(c_info[p_ptr->pclass].cl_name).toLower();
        p_ptr->tile_id = QString("player_%1_%2").arg(race_name).arg(class_name);
    }
    if (save && use_graphics == GRAPHICS_DAVID_GERVAIS)
    {
        QPixmap pix = old_get_tile(0x87 & 0x7F, 0xB7 & 0x7F, 0, 0);
        QFile tile_file(npp_dir_graf.absoluteFilePath("obj_pile.png"));
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

/*
 * Converts stat num into a six-char (right justified) string
 */
QString cnv_stat(int val)
{
    QString str;

    /* Above 18 */
    if (val > 18)
    {
        int bonus = (val - 18);

        if (game_mode == GAME_NPPMORIA)
        {
            if (bonus > 99) bonus = 100;
        }

        if (bonus >= 220)
            str = "18/***";
        else if (bonus >= 100)
            str = QString("18/%1").arg(_num(bonus), 3, '0');
        else
            str = QString(" 18/%1").arg(_num(bonus), 2, '0');
    }

    /* From 3 to 18 */
    else
    {
        str = QString("%1").arg(_num(val), 6, ' ');
    }

    return str;
}


// Write a note to the notes file
void write_note(QString note, s16b depth)
{
    notes_type note_body;
    notes_type *notes_ptr = &note_body;

    notes_ptr->game_turn = p_ptr->game_turn;
    notes_ptr->dun_depth = depth;
    notes_ptr->player_level = p_ptr->lev;
    notes_ptr->recorded_note = note;

    notes_log.append(note_body);
}

static bool is_roman_numeral_char(QChar letter)
{
    QString numerals = "IVXLCDM";

    if (numerals.contains(letter, Qt::CaseSensitive)) return (TRUE);
    return (FALSE);
}

// Return the roman number segment of a string
QString find_roman_numeral(QString full_name)
{
    QString roman_numeral = NULL;
    QString name = full_name;
    QString space = QString(" ");

    // Require a space
    if (!name.contains(space)) return NULL;

    //Start analyzing after the first space
    name.remove(0, name.indexOf(' '));

    while (name.length())
    {
        QChar first = name[0];

        // If we find a roman numeral char, start recording
        if (is_roman_numeral_char(first))
        {
            roman_numeral.append(first);
        }
        // If the roman numeral stops, quit.
        else if (roman_numeral.length()) break;

        // Break when a space if found.
        if (operator==(first, space)) break;

        // Cut off the first letter
        name.remove(0, 1);
    }
    return (roman_numeral);
}

/*----- Roman numeral functions  ------*/

/*
 * Converts an arabic numeral (int) to a roman numeral (char *).
 *
 * An arabic numeral is accepted in parameter `n`, and the corresponding
 * upper-case roman numeral is placed in the parameter `roman`.  The
 * length of the buffer must be passed in the `bufsize` parameter.  When
 * there is insufficient room in the buffer, or a roman numeral does not
 * exist (e.g. non-positive integers) a value of 0 is returned and the
 * `roman` buffer will be the empty string.  On success, a value of 1 is
 * returned and the zero-terminated roman numeral is placed in the
 * parameter `roman`.
 */
QString int_to_roman(int n)
{
    QString roman;
    /* Roman symbols */
    QString roman_symbol_labels[13] =
        {"M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX",
         "V", "IV", "I"};
    int  roman_symbol_values[13] =
        {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};

    /* Clear the roman numeral buffer */
    roman.clear();

    /* Roman numerals have no zero or negative numbers */
    if (n < 1)
        return NULL;

    /* Build the roman numeral in the buffer */
    while (n > 0)
    {
        int i = 0;

        /* Find the largest possible roman symbol */
        while (n < roman_symbol_values[i]) i++;

        roman.append(roman_symbol_labels[i]);

        /* Decrease the value of the arabic numeral */
        n -= roman_symbol_values[i];
    }

    return (roman);
}


/*
 * Converts a roman numeral to an arabic numeral (int).
 *
 * The null-terminated roman numeral is accepted in the `roman`
 * parameter and the corresponding integer arabic numeral is returned.
 * Only upper-case values are considered. When the `roman` parameter
 * is empty or does not resemble a roman numeral, a value of -1 is
 * returned.
 *
 * XXX This function will parse certain non-sense strings as roman
 *     numerals, such as IVXCCCVIII
 */
int roman_to_int(QString roman)
{

    int n = 0;

    QString numerals = "IVXLCDM";
    QString roman_symbol_labels[13] =
        {"M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX",
         "V", "IV", "I"};
    int  roman_symbol_values[13] =
        {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};

    if (!roman.length()) return -1;

    while (roman.length())
    {
        bool found = FALSE;
        for (int i = 0; i < 13; i++)
        {
            if (roman.indexOf(roman_symbol_labels[i]) != 0) continue;

            // We have a match
            n += roman_symbol_values[i];
            roman.remove(0, roman_symbol_labels[i].length());
            found = TRUE;
            break;
        }
        if (!found) roman.remove(0,1);
    }

    return n;
}


