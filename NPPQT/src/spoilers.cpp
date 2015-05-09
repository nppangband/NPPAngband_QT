/*
 * Copyright (c) 2015 Jeff Greene, Diego Gonzalez
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

#include <src/init.h>
#include <src/npp.h>
#include <QTextStream>
#include <QTextDocument>



void print_monster_spoiler_file(void)
{
    QString spoil_mon_filename = "mon_spoiler.rtf";
    QFile spoil_mon_file;

    if (game_mode == GAME_NPPANGBAND) spoil_mon_filename.prepend("nppangband_");
    else spoil_mon_filename.prepend("nppmoria_"); /* (game_mode == GAME_NPPMORIA) */

    /* Build the filename */
    spoil_mon_file.setFileName(QString("%1/%2" ) .arg(npp_dir_user.path()) .arg(spoil_mon_filename));

    if (!spoil_mon_file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QTextStream out(&spoil_mon_file);

    QTextDocument mon_txt_doc;

    QString output = QString("<big># Monster Spoiler File </big>");

    for (int i = 1; i < z_info->r_max; i++)
    {
        monster_race *r_ptr = &r_info[i];

        /* Unused slot */
        if (r_ptr->r_name_full.isEmpty()) continue;
        if (r_ptr->is_player_ghost()) continue;

        QString race_name_desc = QString("<br><br><big><b>%1</b></big><br>") .arg(monster_desc_race(i));
        race_name_desc.append(get_monster_description(i, TRUE, NULL, FALSE)); 

        output.append(race_name_desc);
    }

    mon_txt_doc.setHtml(output);

    out << mon_txt_doc.toHtml();

    /* Close and save the ghost_template file */
    spoil_mon_file.close();
}


