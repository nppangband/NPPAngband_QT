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

#include <src/npp.h>
#include <src/qt_mainwindow.h>
#include <src/knowledge.h>
#include <QHeaderView>
#include <QFontDialog>
#include <QtCore/qmath.h>

/*
 * Object data for the visible object list
 */
class object_vis
{
public:
    object_type *obj_type;
    u16b obj_y;
    u16b obj_x;
    qreal distance;
};


static bool compare_types(const object_type *o_ptr, const object_type *j_ptr)
{
    if (o_ptr->tval < j_ptr->tval) return (FALSE);
    if (o_ptr->tval > j_ptr->tval) return (TRUE);
    if (o_ptr->sval < j_ptr->sval) return (FALSE);
    return (TRUE);
}

/* some handy macros for sorting */
#define object_is_worthless(o) (k_info[o->k_idx].cost == 0)

/*
 * Sort comparator for objects
 *  TRUE if o1 should be first
 *  FALSE if o2 should be first, or it doesn't matter
 */
static bool compare_items(const object_vis ov1, const object_vis ov2)
{
    object_type *o1 = ov1.obj_type;
    object_type *o2 = ov2.obj_type;

    /* known artifacts will sort first */
    if (o1->is_known_artifact() && o2->is_known_artifact())
        return compare_types(o1, o2);
    if (o1->is_known_artifact()) return FALSE;
    if (o2->is_known_artifact()) return TRUE;

    /* unknown objects will sort next */
    if (!o1->is_flavor_known() && !o2->is_flavor_known())
        return compare_types(o1, o2);
    if (!o1->is_flavor_known()) return FALSE;
    if (!o2->is_flavor_known()) return TRUE;

    /* if only one of them is worthless, the other comes first */
    if (object_is_worthless(o1) && !object_is_worthless(o2)) return TRUE;
    if (!object_is_worthless(o1) && object_is_worthless(o2)) return FALSE;

    /* otherwise, just compare tvals and svals */
    /* NOTE: arguably there could be a better order than this */
    return compare_types(o1, o2);
}


void MainWindow::set_font_win_obj_list(QFont newFont)
{
    font_win_obj_list = newFont;
    win_obj_list_update();
}

void MainWindow::win_obj_list_font()
{
    bool selected;
    QFont font = QFontDialog::getFont( &selected, font_message_window, this );

    if (selected)
    {
        set_font_win_obj_list(font);
    }
}

// For when savefiles close but the game doesn't.
void MainWindow::win_obj_list_wipe()
{
    if (!show_obj_list) return;
    if (!character_generated) return;
    while (obj_list_area->rowCount()) obj_list_area->removeRow(obj_list_area->rowCount()-1);
}

void MainWindow::win_obj_list_update()
{
    if (!character_generated) return;
    if (!show_obj_list) return;


    win_obj_list_wipe();

    /* Hallucination is weird */
    if (p_ptr->timed[TMD_IMAGE])
    {
        obj_list_area->insertRow(0);
        obj_list_area->insertRow(1);
        obj_list_area->insertRow(2);

        QTableWidgetItem *header1 = new QTableWidgetItem("You can't believe");
        header1->setTextAlignment(Qt::AlignLeft);
        header1->setFont(font_win_obj_list);
        header1->setTextColor(defined_colors[TERM_ORANGE]);
        obj_list_area->setItem(0, 0, header1);

        QTableWidgetItem *header2 = new QTableWidgetItem("what you are seeing!");
        header2->setTextColor(defined_colors[TERM_YELLOW]);
        header2->setFont(font_win_obj_list);
        header2->setTextAlignment(Qt::AlignLeft);
        obj_list_area->setItem(1, 0, header2);

        QTableWidgetItem *header3 = new QTableWidgetItem("It's like a dream!");
        header3->setTextColor(defined_colors[TERM_ORANGE_PEEL]);
        header3->setFont(font_win_obj_list);
        header3->setTextAlignment(Qt::AlignLeft);
        obj_list_area->setItem(2, 0, header3);

        obj_list_area->resizeColumnsToContents();

        return;
    }

    int py = p_ptr->py;
    int px = p_ptr->px;

    int dungeon_hgt = p_ptr->cur_map_hgt;
    int dungeon_wid = p_ptr->cur_map_wid;

    QVector<object_vis> vis_obj_list;

    object_kind *k_ptr;

    /* Player gets special treatment */
    int floor_list_player[MAX_FLOOR_STACK];
    int num_player = scan_floor(floor_list_player, MAX_FLOOR_STACK, py, px, 0x02);

    /* Scan the list of objects on the level */
    /* Look at each square of the dungeon for items */
    for (int my = 0; my < dungeon_hgt; my++)
    {
        for (int mx = 0; mx < dungeon_wid; mx++)
        {
            int floor_list_stack[MAX_FLOOR_STACK];

            /* No objects here, or it is the player square */
            if (!dungeon_info[my][mx].has_object()) continue;
            if ((my == py) && (mx == px)) continue;

            int num_square = scan_floor(floor_list_stack, MAX_FLOOR_STACK, my, mx, 0x02);

            /* Iterate over all the items found on this square */
            for (int i = 0; i < num_square; i++)
            {
                int j;

                object_type *o_ptr = &o_list[floor_list_stack[i]];

                /* Skip gold/squelched */
                if ((o_ptr->tval == TV_GOLD) ||
                    ((k_info[o_ptr->k_idx].squelch == SQUELCH_ALWAYS) && (k_info[o_ptr->k_idx].aware)))
                    continue;

                bool found_match = FALSE;

                /*
                 * See if we've already seen a similar item;
                 * if so, just add to its count.
                 */
                for (j = 0; j < vis_obj_list.size(); j++)
                {
                    object_vis *obj_vis_ptr = &vis_obj_list[j];

                    if (object_similar(o_ptr, obj_vis_ptr->obj_type))
                    {
                        obj_vis_ptr->obj_type->number += o_ptr->number;
                        // If this one is closer, use the new distance (using pythagorean's theorum to calc distance)
                        qreal new_distance = qSqrt(qPow((my - p_ptr->py), 2) + qPow((mx - p_ptr->px), 2));
                        if (new_distance < obj_vis_ptr->distance)
                        {
                            obj_vis_ptr->distance = new_distance;
                            obj_vis_ptr->obj_y = my;
                            obj_vis_ptr->obj_x = mx;
                        }
                        found_match = TRUE;
                        break;
                    }
                }

                if (found_match) continue;

                /*
                 * We saw a new item. So insert it at the end of the list and
                 * then sort it forward using compare_items(). The types list
                 * is always kept sorted.
                 */

                object_vis new_entry;
                new_entry.obj_type = o_ptr;
                new_entry.distance = qSqrt(qPow((my - p_ptr->py), 2) + qPow((mx - p_ptr->px), 2));
                new_entry.obj_y = my;
                new_entry.obj_x = mx;

                vis_obj_list.append(new_entry);
            }
        }
    }

    /* Note no visible objects */
    if (!vis_obj_list.size())
    {
        obj_list_area->insertRow(0);

        if(p_ptr->timed[TMD_BLIND])
        {
            QTableWidgetItem *header1 = new QTableWidgetItem("You are blind!");
            header1->setTextColor(defined_colors[TERM_RED]);
            header1->setTextAlignment(Qt::AlignLeft);
            header1->setFont(font_win_obj_list);
            obj_list_area->setItem(0, 0, header1);
        }
        else
        {
            QTableWidgetItem *header1 = new QTableWidgetItem("You see no objects.");
            header1->setTextColor(defined_colors[TERM_WHITE]);
            header1->setTextAlignment(Qt::AlignLeft);
            header1->setFont(font_win_obj_list);
            obj_list_area->setItem(0, 0, header1);
        }

        obj_list_area->resizeColumnsToContents();

        return;
    }

    // sort the list by compare_types
    qSort(vis_obj_list.begin(), vis_obj_list.end(), compare_items);

    int row = 0;

    if (num_player)
    {
        obj_list_area->insertRow(row++);

        QString player_message = ("YOU ARE STANDING ON:");
        QTableWidgetItem *in_los = new QTableWidgetItem(player_message);
        in_los->setFont(font_win_obj_list);
        in_los->setTextColor(defined_colors[TERM_L_BLUE]);
        in_los->setTextAlignment(Qt::AlignLeft);
        obj_list_area->setItem(0, 1, in_los);
    }

    /* List objects player is standing on */
    for (int i = 0; i < num_player; i++)
    {

        object_type *o_ptr = &o_list[floor_list_player[i]];

        int col = 0;

        obj_list_area->insertRow(row);

        k_ptr = &k_info[o_ptr->k_idx];

        // Symbol (or tile if tiles are used)
        QString obj_symbol = (QString("'%1'") .arg(k_ptr->d_char));
        QTableWidgetItem *obj_ltr = new QTableWidgetItem(obj_symbol);
        if (use_graphics)
        {
            QPixmap pix = ui_get_tile(k_ptr->tile_id);
            pix = pix.scaled(32, 32);
            obj_ltr->setIcon(pix);
        }

        obj_ltr->setData(Qt::ForegroundRole, k_ptr->d_color);
        obj_ltr->setTextAlignment(Qt::AlignCenter);
        obj_ltr->setFont(font_win_obj_list);
        obj_list_area->setItem(row, col++, obj_ltr);

        QColor this_color;

        // Different colors depending on object_type.
        if (o_ptr->is_artifact() && o_ptr->is_known()) this_color = defined_colors[TERM_VIOLET];
        else if (!o_ptr->is_aware()) this_color = defined_colors[TERM_RED];
        else if (object_is_worthless(o_ptr)) this_color = defined_colors[TERM_SLATE];
        else if (!o_ptr->is_known()) this_color = defined_colors[TERM_L_UMBER];
        else this_color = defined_colors[TERM_WHITE];

        // Object Name
        QString obj_name = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);
        if (o_ptr->number > 1) obj_name.append(QString("(x%1)") .arg(o_ptr->number));
        QTableWidgetItem *object_label = new QTableWidgetItem(obj_name);
        object_label->setFont(font_win_obj_list);
        object_label->setTextColor(this_color);
        object_label->setTextAlignment(Qt::AlignLeft);
        obj_list_area->setItem(row, col++, object_label);

        row ++;
    }

    if (vis_obj_list.size())
    {
        obj_list_area->insertRow(row);
        QTableWidgetItem *label_see = new QTableWidgetItem("YOU CAN SEE:");
        label_see->setFont(font_win_obj_list);
        label_see->setTextColor(defined_colors[TERM_L_BLUE]);
        label_see->setTextAlignment(Qt::AlignLeft);
        obj_list_area->setItem(row++, 1, label_see);
    }

    /* List all other objects */
    for (int i = 0; i < vis_obj_list.size(); i++)
    {
        object_vis *obj_vis_ptr = &vis_obj_list[i];
        object_type *o_ptr = obj_vis_ptr->obj_type;

        int col = 0;

        obj_list_area->insertRow(row);

        k_ptr = &k_info[o_ptr->k_idx];

        // Symbol (or tile if tiles are used)
        QString obj_symbol = (QString("'%1'") .arg(k_ptr->d_char));
        QTableWidgetItem *obj_ltr = new QTableWidgetItem(obj_symbol);
        if (use_graphics)
        {
            QPixmap pix = ui_get_tile(k_ptr->tile_id);
            pix = pix.scaled(32, 32);
            obj_ltr->setIcon(pix);
        }

        obj_ltr->setData(Qt::ForegroundRole, o_ptr->get_color());
        obj_ltr->setTextAlignment(Qt::AlignCenter);
        obj_ltr->setFont(font_win_obj_list);
        obj_list_area->setItem(row, col++, obj_ltr);

        QColor this_color;

        // Different colors depending on object_type.
        if (o_ptr->is_artifact() && o_ptr->is_known()) this_color = defined_colors[TERM_VIOLET];
        else if (!o_ptr->is_aware()) this_color = defined_colors[TERM_RED];
        else if (object_is_worthless(o_ptr)) this_color = defined_colors[TERM_SLATE];
        else if (!o_ptr->is_known()) this_color = defined_colors[TERM_L_UMBER];
        else this_color = defined_colors[TERM_WHITE];

        // Object Name
        QString obj_name = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);
        if (o_ptr->number > 1) obj_name.append(QString("(x%1)") .arg(o_ptr->number));
        QTableWidgetItem *object_label = new QTableWidgetItem(obj_name);
        object_label->setFont(font_win_obj_list);
        object_label->setTextColor(this_color);
        object_label->setTextAlignment(Qt::AlignLeft);
        obj_list_area->setItem(row, col++, object_label);

        int dir_y = py - obj_vis_ptr->obj_y;
        int dir_x = px - obj_vis_ptr->obj_x;
        QString direction;
        direction.clear();
        if (dir_y)
        {
            if (dir_y < 0)direction.append("S");
            else direction.append("N");
            direction.append(QString("%1 ") .arg(ABS(dir_y)));
        }
        if (dir_x)
        {
            if (dir_x > 0)direction.append("E");
            else direction.append("W");
            direction.append(QString("%1 ") .arg(ABS(dir_x)));
        }


        // Direction
        QTableWidgetItem *dir_label = new QTableWidgetItem(direction);
        // Give it a directional arrow icon
        switch (ui_get_dir_from_slope(py, px, obj_vis_ptr->obj_y, obj_vis_ptr->obj_x))
        {
            case 6:
            {
                dir_label->setIcon(QIcon(":/icons/lib/icons/arrow-east.png"));
                break;
            }
            case 9:
            {
                dir_label->setIcon(QIcon(":/icons/lib/icons/arrow-northeast.png"));
                break;
            }
            case 8:
            {
                dir_label->setIcon(QIcon(":/icons/lib/icons/arrow-north.png"));
                break;
            }
            case 7:
            {
                dir_label->setIcon(QIcon(":/icons/lib/icons/arrow-northwest.png"));
                break;
            }
            case 4:
            {
                dir_label->setIcon(QIcon(":/icons/lib/icons/arrow-west.png"));
                break;
            }
            case 1:
            {
                dir_label->setIcon(QIcon(":/icons/lib/icons/arrow-southwest.png"));
                break;
            }
            case 2:
            {
                dir_label->setIcon(QIcon(":/icons/lib/icons/arrow-south.png"));
                break;
            }
            case 3:
            {
                dir_label->setIcon(QIcon(":/icons/lib/icons/arrow-southeast.png"));
                break;
            }
            default:
            {
                dir_label->setIcon(QIcon(":/icons/lib/icons/arrow-east.png"));
                break;
            }
        }
        dir_label->setFont(font_win_obj_list);
        dir_label->setTextColor(this_color);
        dir_label->setTextAlignment(Qt::AlignLeft);
        obj_list_area->setItem(row, col++, dir_label);

        row ++;
    }

    obj_list_area->resizeColumnsToContents();

}

/*
 *  Show widget is called after this to allow
 * the settings to restore the save geometry.
 */
void MainWindow::win_obj_list_create()
{
    window_obj_list = new QWidget();
    obj_list_vlay = new QVBoxLayout;
    window_obj_list->setLayout(obj_list_vlay);
    obj_list_area = new QTableWidget(0, 3);
    obj_list_area->setAlternatingRowColors(FALSE);
    obj_list_area->verticalHeader()->setVisible(FALSE);
    obj_list_area->horizontalHeader()->setVisible(FALSE);
    obj_list_area->setEditTriggers(QAbstractItemView::NoEditTriggers);
    obj_list_area->setSortingEnabled(FALSE);
    qtablewidget_add_palette(obj_list_area);
    obj_list_vlay->addWidget(obj_list_area);
    obj_list_menubar = new QMenuBar;
    obj_list_vlay->setMenuBar(obj_list_menubar);
    window_obj_list->setWindowTitle("Viewable Object List");
    obj_win_settings = obj_list_menubar->addMenu(tr("&Settings"));
    obj_list_set_font = new QAction(tr("Set Object List Font"), this);
    obj_list_set_font->setStatusTip(tr("Set the font for the Object List."));
    connect(obj_list_set_font, SIGNAL(triggered()), this, SLOT(win_obj_list_font()));
    obj_win_settings->addAction(obj_list_set_font);

    //Disable the x button from closing the widget
    window_obj_list->setWindowFlags(Qt::WindowTitleHint);
}

void MainWindow::win_obj_list_destroy()
{
    if (!show_obj_list) return;
    delete window_obj_list;
}

void MainWindow::toggle_win_obj_list()
{
    if (!show_obj_list)
    {
        win_obj_list_create();
        show_obj_list = TRUE;
        win_obj_list->setText("Hide Object List Window");
        window_obj_list->show();
        win_obj_list_update();
    }
    else
    {
        win_obj_list_destroy();
        show_obj_list = FALSE;
        win_obj_list->setText("Show Object List Window");
    }
}
