
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

#include "src/npp.h"
#include <QFileDialog>
#include <QTextStream>
#include <src/init.h>

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
        QString simple_1 = label_string;
        QString simple_2 = output_string;
        simple_1.remove(QRegExp("<[^>]*>"));
        simple_2.remove(QRegExp("<[^>]*>"));

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
