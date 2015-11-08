
/*
 * File: file_output.cpp
 * Purpose: Various file-related activities
 *
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke,
 * Jeff Greene, Diego Gonzalez and Angband developers
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

#include <QFileDialog>
#include <QTextStream>
#include <src/init.h>
#include <src/player_screen.h>

// Helper function to check if the color is white
static bool is_white(QColor this_color)
{
    if (this_color.operator ==(defined_colors[TERM_WHITE])) return (TRUE);
    if (this_color.operator ==(defined_colors[TERM_LIGHT_GRAY])) return (TRUE);
    if (this_color.operator ==(defined_colors[TERM_SNOW_WHITE])) return (TRUE);
    if (this_color.operator ==(defined_colors[TERM_IVORY])) return (TRUE);
    if (this_color.operator ==(Qt::white)) return (TRUE);

    return (FALSE);
}

static QString get_sidebar_string(int which_line)
{
    QString label_string;
    QString output_string;

    // Paranoia
    if (which_line < 0) return ("");
    else if (which_line == 0) label_string = op_ptr->full_name;
    else if (which_line == 1) label_string = p_info[p_ptr->prace].pr_name;
    else if (which_line == 2) label_string = c_info[p_ptr->pclass].cl_name;
    else if (which_line == 3) label_string = get_player_title();

    // Handle names longer than 20 characters, or anyting else unusual
    if (label_string.length() > 20) label_string.truncate(20);

    // These will not be longer than 20 characters
    if (which_line >= 4)
    {
        // Get the label and the text
        which_line -= 4;
        label_string = ui_return_sidebar_text(TRUE, which_line);
        output_string = ui_return_sidebar_text(FALSE, which_line);
    }

    // Need to get the true length without the html tags
    while (TRUE)
    {
        QString simple_1 = html_string_to_plain_text(label_string);
        QString simple_2 = html_string_to_plain_text(output_string);

        if ((simple_1.length() + simple_2.length()) >= 20) break;

        label_string.append(" ");
    }
    label_string.append(output_string);

    label_string.append("  ");

    return (label_string);
}

/* Take an html screenshot */
void save_screenshot(byte do_png)
{
    if (do_png)
    {
        ui_png_screenshot();
        return;
    }

    // Start with the current player name
    QString default_name = "player";
    if (!op_ptr->full_name.isEmpty())default_name = op_ptr->full_name;
    QString default_file = npp_dir_user.path();
    default_file.append("/");
    default_file.append(default_name);
    default_file.append("_npp_scr");

    QString file_name = QFileDialog::getSaveFileName(0, "Select a savefile", default_file, "HTML (*.html)");

    if (file_name.isEmpty())
        return;

    QFile screenshot_file(file_name);

    if (!screenshot_file.open(QIODevice::WriteOnly)) return;

    QString text = (QString("Version: %1 %2") .arg(VERSION_MODE_NAME) .arg(VERSION_STRING));

    QTextStream out(&screenshot_file);


    out << QString("<!DOCTYPE html><html><head><br>");
    out << (QString("  <meta='generator' content='%1'><br>") .arg(text));
    out << (QString("  <title>%1</title><br>") .arg(npp_dir_user.path()));
    out << QString("</head><br><br>");
    out << QString("<body style='color: #fff; background: #000;'><br>");
    out << QString("<pre><br>");

    int sidebar_row = 0;

    /* Dump the screen */
    for (int y = 0; y < p_ptr->cur_map_hgt; y++)
    {
        bool first_x = TRUE;

        for (int x = 0; x < p_ptr->cur_map_wid; x++)
        {
            if (!panel_contains(y, x)) continue;

            //Hack - print out the sidebar info
            if (first_x)
            {
                first_x = FALSE;

                out << get_sidebar_string(sidebar_row++);
            }

            dungeon_type *d_ptr = &dungeon_info[y][x];

            QChar square_char = d_ptr->dun_char;
            QColor square_color = d_ptr->dun_color;

            if (d_ptr->has_visible_monster())
            {
                square_char = d_ptr->monster_char;
                square_color = d_ptr->monster_color;
            }

            else if (d_ptr->has_visible_effect())
            {
                square_char = d_ptr->effect_char;
                square_color = d_ptr->effect_color;
            }

            else if (d_ptr->has_visible_object())
            {
                square_char = d_ptr->object_char;
                square_color = d_ptr->object_color;
            }

            // Can't print white
            if (is_white(square_color)) square_color = defined_colors[TERM_L_DARK];

            out << color_string(square_char, square_color);
        }

        // end the row
        out << QString("<br>");
    }

    // If there are more rows in the sidebar than in the dungeon
    while (sidebar_row < (4+SIDEBAR_LABEL_SIZE))
    {
        out << get_sidebar_string(sidebar_row++);
        out << QString("<br>");
    }

    out << QString("</pre><br>");
    out << QString("</body><br>");
    out << QString("</html><br>");

    screenshot_file.close();
}

#define BASIC_CHAR_NUM 9

QString basic_char_labels[BASIC_CHAR_NUM] =
{
    "<b>NAME:<b/>",  // Hack - the dialog uses a pushbutton
    "GENDER_LABEL",
    "RACE_LABEL",
    "CLASS_LABEL",
    "TITLE_LABEL",
    "HP_LABEL",
    "SP_LABEL",
    "FAME_LABEL",
    "GOLD_LABEL",
};

QString basic_char_data[BASIC_CHAR_NUM] =
{
    "PLYR_Name",
    "PLYR_Sex",
    "PLYR_Race",
    "PLYR_Class",
    "PLYR_Title",
    "PLYR_HP",
    "PLYR_SP",
    "PLYR_Fame",
    "PLYR_Gold",
};

#define BASIC_DATA_NUM 9

QString basic_data_labels[BASIC_DATA_NUM] =
{
    "AGE_LABEL",
    "HEIGHT_LABEL",
    "WEIGHT_LABEL",
    "SC_LABEL",
    "GAME_TURN_LABEL",
    "PLAYER_TURN_LABEL",
    "DEPTH_CUR_LABEL",
    "DEPTH_MAX_LABEL",
    "INFRA_LABEL",
};

QString basic_data_data[BASIC_DATA_NUM] =
{
    "PLYR_Age",
    "PLYR_Height",
    "PLYR_Weight",
    "PLYR_SC",
    "TURN_Game",
    "TURN_Player",
    "DEPTH_Cur",
    "DEPTH_Max",
    "PLYR_Infra",
};

#define GAME_INFO_NUM 8

QString game_info_labels[GAME_INFO_NUM] =
{
    "LEVEL_LABEL",
    "CUR_EXP_LABEL",
    "MAX_EXP_LABEL",
    "ADVANCE_LABEL",
    "SCORE_LABEL",
    "BURDEN_CUR_LABEL",
    "BURDEN_MAX_LABEL",
    "BURDEN_PERCENT_LABEL",
};

QString game_info_data[GAME_INFO_NUM] =
{
    "PLYR_Level",
    "PLYR_Cur_Exp",
    "PLYR_Max_Exp",
    "PLYR_Advance",
    "PLYR_Score",
    "BURDEN_Cur",
    "BURDEN_Max",
    "BURDEN_Percent",
};

#define COMBAT_INFO_NUM 8

QString combat_info_labels[COMBAT_INFO_NUM] =
{
    "SPEED_LABEL",
    "AC_LABEL",
    "MELEE_LABEL",
    "CRIT_HIT_LABEL",
    "SHOOTING_LABEL",
    "SRCH_FREQ_LABEL",
    "SRCH_CHANCE_LABEL",
    "TUNNEL_LABEL",
};

QString combat_info_data[COMBAT_INFO_NUM] =
{
    "BASE_Speed",
    "PLYR_AC",
    "PLYR_MELEE",
    "HIT_Critical",
    "SHOOTING_Stats",
    "SEARCH_Freq",
    "SEARCH_Chance",
    "PLYR_Tunnel",
};

static QString combine_strings(QString string_1, QString string_2, int max_length)
{
    // Need to get the true length without the html tags
    while (TRUE)
    {
        QString simple_1 = html_string_to_plain_text(string_1);
        QString simple_2 = html_string_to_plain_text(string_2);

        if ((simple_1.length() + simple_2.length()) >= max_length) break;

        string_1.append(" ");
    }

    string_1.append(string_2);
    string_1.append("     ");
    return (string_1);
}

// Create the stat line for the character info file
// The truncate statements are because the combine_strings file adds 5 spaces on the end
static QString make_stat_string(QString stat_label, QString stat_base, QString stat_race, QString stat_class,
                                QString stat_equip, QString stat_quest, QString stat_total, QString stat_reduced)
{
    QString return_string = set_html_string_length(stat_label, 8, FALSE);

    // get rid of spaces
    stat_base.remove(" ");
    stat_race.remove(" ");
    stat_class.remove(" ");
    stat_equip.remove(" ");
    stat_quest.remove(" ");
    stat_total.remove(" ");
    stat_reduced.remove(" ");

    stat_base = set_html_string_length(stat_base, 6, TRUE);
    return_string.append(stat_base);



    if (birth_maximize)
    {
        stat_race = set_html_string_length(stat_race, 5, TRUE);
        return_string.append(stat_race);
        stat_class = set_html_string_length(stat_class, 5, TRUE);
        return_string.append(stat_class);
    }

    stat_equip = set_html_string_length(stat_equip, 5, TRUE);
    return_string.append(stat_equip);

    if (!birth_no_quests)
    {
        stat_quest = set_html_string_length(stat_quest, 5, TRUE);
        return_string.append(stat_quest);
    }

    stat_total = set_html_string_length(stat_total, 12, TRUE);
    return_string.append(stat_total);

    if (stat_reduced.length())
    {
        stat_reduced = set_html_string_length(stat_total, 12, TRUE);
        return_string.append(stat_reduced);
    }

    return (return_string);
}

/* Save a character file
 *
 * For this function to work, it is important that the tables above
 * are consistent with the Qlabel object names in player_screen.
 *
 * This functon creates a dummy characte screen dialog and reads the label values
 */
void save_character_file(void)
{
    // Start with the current player name
    QString default_name = "player";
    if (!op_ptr->full_name.isEmpty())default_name = op_ptr->full_name;
    QString default_file = npp_dir_user.path();
    default_file.append("/");
    default_file.append(default_name);
    default_file.append("_npp_char");

    QString file_name = QFileDialog::getSaveFileName(0, "Select a savefile", default_file, "HTML (*.html)");

    if (file_name.isEmpty())
        return;

    QFile char_info_file(file_name);

    if (!char_info_file.open(QIODevice::WriteOnly)) return;

    QTextStream out(&char_info_file);

    QString text = (QString("<b><h1>[%1 %2 Character Dump]</h1></b><br><br>") .arg(VERSION_MODE_NAME) .arg(VERSION_STRING));

    //Hack - create the player screen from which all the data will be read
    PlayerScreenDialog dlg;

    QList<QLabel *> lbl_list = dlg.findChildren<QLabel *>();

    out << QString("<!DOCTYPE html><html><head>");
    out << (QString("  <meta='generator' content='%1'>") .arg(text));
    out << (QString("  <title>%1</title>") .arg(npp_dir_user.path()));
    out << QString("</head>");
    out << QString("<body style='color: #000; background: #fff;'>");
    out << QString("<pre>");

    out << text;

    int end_loop = MAX(BASIC_DATA_NUM, BASIC_DATA_NUM);
    if (end_loop < A_MAX) end_loop = A_MAX;

    for (int i = 0; i < end_loop; i++)
    {
        QString basic_char_label;
        QString basic_char_info;
        QString basic_data_label;
        QString basic_data_info;
        QString stat_label = "st_label_";
        QString stat_base = "st_base_";
        QString stat_race = "st_race_";
        QString stat_class = "st_class_";
        QString stat_equip = "st_equip_";
        QString stat_quest = "st_quest_";
        QString stat_total = "st_total_";
        QString stat_reduced = "st_reduce_";

        if (i < BASIC_CHAR_NUM)
        {
            basic_char_label = basic_char_labels[i];
            basic_char_info = basic_char_data[i];

            if (basic_char_label.contains("SP_LABEL") && !cp_ptr->spell_book)
            {
                basic_char_label.clear();
                basic_char_info.clear();
            }
            if (basic_char_label.contains("FAME_LABEL") && birth_no_quests)
            {
                basic_char_label.clear();
                basic_char_info.clear();
            }

        }
        else
        {
            basic_char_label.clear();
            basic_char_info.clear();
        }
        if (i < BASIC_DATA_NUM)
        {
            basic_data_label = basic_data_labels[i];
            basic_data_info = basic_data_data[i];
        }
        else
        {
            basic_data_label.clear();
            basic_data_info.clear();
        }

        if (i < (A_MAX+1))
        {
            // Do the labels
            if (!i)
            {
                stat_label.append("X");
                stat_base.append("X");
                stat_race.append("X");
                stat_class.append("X");
                stat_equip.append("X");
                stat_quest.append("X");
                stat_total.append("X");
                stat_reduced.clear();
            }
            // Or fill in the stats
            else
            {
                int k = i - 1;

                QString append;
                append.setNum(k);
                stat_label.append(append);
                stat_base.append(append);
                stat_race.append(append);
                stat_class.append(append);
                stat_equip.append(append);
                stat_quest.append(append);
                stat_total.append(append);
                stat_reduced.append(append);

                // No need to display reduced stat
                if (p_ptr->state.stat_loaded_cur[k] <
                    p_ptr->state.stat_loaded_max[k]) stat_reduced.clear();
            }

            if (!birth_maximize)
            {
                stat_race.clear();
                stat_class.clear();
            }

            if (birth_no_quests)  stat_quest.clear();
        }
        else
        {
            stat_label.clear();
            stat_base.clear();
            stat_race.clear();
            stat_class.clear();
            stat_equip.clear();
            stat_quest.clear();
            stat_total.clear();
            stat_reduced.clear();
        }


        //Find the values
        for (int x = 0; x < lbl_list.size(); x++)
        {
            QLabel *this_lbl = lbl_list.at(x);

            QString this_name = this_lbl->objectName();

            if (strings_match(this_name, basic_char_label)) basic_char_label = this_lbl->text();
            if (strings_match(this_name, basic_char_info))  basic_char_info  = this_lbl->text();
            if (strings_match(this_name, basic_data_label)) basic_data_label = this_lbl->text();
            if (strings_match(this_name, basic_data_info))  basic_data_info  = this_lbl->text();
            if (strings_match(this_name, stat_label))       stat_label       = this_lbl->text();
            if (strings_match(this_name, stat_base))        stat_base        = this_lbl->text();
            if (strings_match(this_name, stat_race))        stat_race        = this_lbl->text();
            if (strings_match(this_name, stat_class))       stat_class       = this_lbl->text();
            if (strings_match(this_name, stat_equip))       stat_equip       = this_lbl->text();
            if (strings_match(this_name, stat_quest))       stat_quest       = this_lbl->text();
            if (strings_match(this_name, stat_total))       stat_total       = this_lbl->text();
            if (strings_match(this_name, stat_reduced))     stat_reduced     = this_lbl->text();
        }

        out << combine_strings(basic_char_label, basic_char_info, 25);
        out << combine_strings(basic_data_label, basic_data_info, 25);
        if (i < (A_MAX+1)) out << make_stat_string(stat_label, stat_base, stat_race, stat_class, stat_equip, stat_quest, stat_total, stat_reduced);
        out << QString("<br>");

    }

    out << QString("<br><br>");

    end_loop = MAX(COMBAT_INFO_NUM, GAME_INFO_NUM);

    for (int i = 0; i < end_loop; i++)
    {
        QString game_info_label;
        QString game_info_info;
        QString combat_info_label;
        QString combat_info_info;

        if (i < GAME_INFO_NUM)
        {
            game_info_label = game_info_labels[i];
            game_info_info = game_info_data[i];
        }
        else
        {
            game_info_label.clear();
            game_info_info.clear();
        }
        if (i < COMBAT_INFO_NUM)
        {
            combat_info_label = combat_info_labels[i];
            combat_info_info = combat_info_data[i];
        }
        else
        {
            combat_info_label.clear();
            combat_info_info.clear();
        }

        //Find the values
        for (int x = 0; x < lbl_list.size(); x++)
        {
            QLabel *this_lbl = lbl_list.at(x);

            QString this_name = this_lbl->objectName();

            if (strings_match(this_name, game_info_label))  game_info_label  = this_lbl->text();
            if (strings_match(this_name, game_info_info))   game_info_info   = this_lbl->text();
            if (strings_match(this_name, combat_info_label))combat_info_label = this_lbl->text();
            if (strings_match(this_name, combat_info_info)) combat_info_info = this_lbl->text();
        }

        out << combine_strings(game_info_label, game_info_info, 22);
        out << combine_strings(combat_info_label, combat_info_info, 40);
        out << QString("<br>");

    }


    out << QString("</pre>");
    out << QString("</body>");
    out << QString("</html>");

    char_info_file.close();
}

#ifdef USEFUL_UNUSED_CODE
void MainWindow::save_screenshot(void)
{
    QRect dungeon_frame = graphics_view->geometry();

    QRect screen_grab(sidebar_dock->pos(), dungeon_frame.bottomRight());

    QPixmap screenshot = main_window->grab(screen_grab);

    // Start with the current player name
    QString default_name = "player";
    if (!op_ptr->full_name.isEmpty())default_name = op_ptr->full_name;
    QString default_file = npp_dir_user.path();
    default_file.append("/");
    default_file.append(default_name);
    default_file.append("_npp_scr");

    QString file_name = QFileDialog::getSaveFileName(this, tr("Select a savefile"), default_file, tr("PNG (*.png)"));

    if (file_name.isEmpty())
        return;



    QString text = (QString("Version: %1 %2") .arg(VERSION_MODE_NAME) .arg(VERSION_STRING));

    QLabel new_label(text);
    new_label.setPixmap(screenshot);

    /*QPrinter this_printer;
    this_printer.setOutputFileName(file_name);
    this_printer.setOutputFormat(QPrinter::PdfFormat);*/

    screenshot.save(file_name, "PNG", 100);




    /*text.append(QString("Name: %1<br>") .arg(op_ptr->full_name));
    text.append(QString("Race: %1<br>") .arg(p_ptr->prace));
    text.append(QString("Class: %1<br>") .arg(p_ptr->pclass));
    text.append(QString("Level: %1<br>") .arg(p_ptr->lev));*/


}
#endif //USEFUL_UNUSED_CODE
