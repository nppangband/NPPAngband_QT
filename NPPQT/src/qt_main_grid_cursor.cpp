/*
 * File: qt_grid_cursor.cpp
 *
 * Copyright (c) 2014  Jeff Greene, Diego Gonzalez
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

#include <src/qt_mainwindow.h>
#include <src/npp.h>
#include <src/emitter.h>
#include <src/griddialog.h>
#include <src/player_screen.h>
#include <src/object_all_menu.h>
#include <QGraphicsSceneMouseEvent>

DungeonCursor::DungeonCursor(MainWindow *_parent)
{
    parent = _parent;
    c_x = c_y = 0;
    setZValue(100);
    setVisible(false);
}

QRectF DungeonCursor::boundingRect() const
{
    return QRectF(0, 0, parent->main_cell_wid, parent->main_cell_hgt);
}

void DungeonCursor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;

    if (!character_dungeon) return;

    if (!in_bounds(c_y, c_x)) return;

    painter->save();

    if (parent->ui_mode == UI_MODE_INPUT)
    {
        painter->setOpacity(0.5);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor("red"));
        painter->drawRect(this->boundingRect());
    }

    painter->setOpacity(1);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QColor("yellow"));
    painter->drawRect(0, 0, parent->main_cell_wid - 1, parent->main_cell_hgt - 1);
    if ((parent->main_cell_wid > 16) && (parent->main_cell_hgt > 16)) {
        int z = 3;
        painter->drawRect(0, 0, z, z);
        painter->drawRect(parent->main_cell_wid - z - 1, 0, z, z);
        painter->drawRect(0, parent->main_cell_hgt - z - 1, z, z);
        painter->drawRect(parent->main_cell_wid - z - 1, parent->main_cell_hgt - z - 1, z, z);
    }

    painter->restore();
}

void DungeonCursor::moveTo(int _y, int _x)
{
    c_x = _x;
    c_y = _y;
    setPos(c_x * parent->main_cell_wid, c_y * parent->main_cell_hgt);
}

void DungeonCursor::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->ignore(); // Pass event to the grid
}

void DungeonGrid::cellSizeChanged()
{
    prepareGeometryChange();
}

void DungeonCursor::cellSizeChanged()
{
    prepareGeometryChange();
}

QPainterPath DungeonCursor::shape() const
{
    QPainterPath p;
    p.addRect(boundingRect());
    return p;
}

DungeonGrid::DungeonGrid(int _x, int _y, MainWindow *_parent)
{
    c_x = _x;
    c_y = _y;
    parent = _parent;
    setZValue(0);
}

QRectF DungeonGrid::boundingRect() const
{
    return QRectF(0, 0, parent->main_cell_wid, parent->main_cell_hgt);
}

QString find_cloud_tile(int y, int x)
{
    QString tile;

    if (!use_graphics) return tile;

    if (!(dungeon_info[y][x].cave_info & (CAVE_MARK | CAVE_SEEN))) return tile;

    int x_idx = dungeon_info[y][x].effect_idx;
    while (x_idx)
    {
        effect_type *x_ptr = x_list + x_idx;
        x_idx = x_ptr->next_x_idx;

        if (x_ptr->x_flags & EF1_HIDDEN) continue;

        if (x_ptr->x_type == EFFECT_PERMANENT_CLOUD || x_ptr->x_type == EFFECT_LINGERING_CLOUD
                || x_ptr->x_type == EFFECT_SHIMMERING_CLOUD)
        {
            int feat = x_ptr->x_f_idx;
            feat = f_info[feat].f_mimic;
            return f_info[feat].tile_id;
        }
    }

    return tile;
}


// Determine if a tile should be drawn at double-height
static bool is_double_height_tile(int y, int x)
{
    dungeon_type *d_ptr = &dungeon_info[y][x];

    if (use_graphics != GRAPHICS_RAYMOND_GAUSTADNES) return (FALSE);
    if (!d_ptr->double_height_monster) return (FALSE);
    if (main_window->ui_mode == UI_MODE_INPUT) return (FALSE);
    if (in_bounds(y-1, x))
    {
        dungeon_type *dun2_ptr = &dungeon_info[y-1][x];
        if (dun2_ptr->has_visible_monster()) return (FALSE);
    }
    return (TRUE);
}

// This function redraws the actual dungeon square onscreen
void DungeonGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;

    if (!character_dungeon) return;

    if (!in_bounds(c_y, c_x)) return;

    painter->fillRect(QRectF(0, 0, parent->main_cell_wid, parent->main_cell_hgt), Qt::black);

    dungeon_type *d_ptr = &dungeon_info[c_y][c_x];
    QChar square_char = d_ptr->dun_char;
    QColor square_color = d_ptr->dun_color;
    bool empty = true;
    u16b flags = 0;
    QString key2;
    qreal opacity = 1;
    bool do_shadow = false;

    key2.clear();

    bool double_height_mon = FALSE;
    bool double_height_mon_below = FALSE;

    flags = (d_ptr->ui_flags & (UI_LIGHT_BRIGHT | UI_LIGHT_DIM | UI_LIGHT_TORCH | UI_COSMIC_TORCH));

    bool is_cloud = false;

    // Draw visible monsters
    if (d_ptr->has_visible_monster())
    {
        square_char = d_ptr->monster_char;
        square_color = d_ptr->monster_color;

        empty = false;

        if (!parent->do_pseudo_ascii) key2 = d_ptr->monster_tile;
        else do_shadow = true;

        flags |= (d_ptr->ui_flags & UI_TRANSPARENT_MONSTER);
        opacity = 0.5;
    }
    // Draw effects
    else if (d_ptr->has_visible_effect())
    {
        square_char = d_ptr->effect_char;
        square_color = d_ptr->effect_color;

        empty = false;

        key2 = d_ptr->effect_tile;

        flags |= (d_ptr->ui_flags & UI_TRANSPARENT_EFFECT);
        opacity = 0.7;

        is_cloud = (flags & UI_TRANSPARENT_EFFECT);
    }
    // Draw objects
    else if (d_ptr->has_visible_object())
    {
        square_char = d_ptr->object_char;
        square_color = d_ptr->object_color;

        empty = false;

        key2 = d_ptr->object_tile;
    }

    bool done_bg = false;
    bool done_fg = false;

    painter->save();

    // Are we drawing the top half of a monster below?
    if (in_bounds_fully(c_y+1, c_x))
    {
        if (is_double_height_tile(c_y+1, c_x)) double_height_mon_below = TRUE;
    }

    //Double check that there isn't something drawn above.  If double height tile and nothing above, use the space.
    if (is_double_height_tile(c_y, c_x)) double_height_mon = TRUE;

    if (use_graphics)
    {
        // Draw background tile
        QString key1 = d_ptr->dun_tile;

        if (key1.length() > 0)
        {
            QPixmap pix = parent->get_tile(key1, parent->main_cell_hgt, parent->main_cell_wid);

            if (flags & UI_LIGHT_TORCH)
            {
                QColor color = QColor("yellow").darker(150);
                if (flags & UI_COSMIC_TORCH) color = QColor("cyan").darker(150);
                pix = colorize_pix(pix, color);
            }
            else if (flags & UI_LIGHT_BRIGHT)
            {
                pix = parent->apply_shade(key1, pix, "bright");
            }
            else if (flags & UI_LIGHT_DIM)
            {
                pix = parent->apply_shade(key1, pix, "dim");
            }

            painter->drawPixmap(pix.rect(), pix, pix.rect());
            done_bg = true;

            // Draw cloud effects (in graphics mode), if not already drawing that
            if (!is_cloud)
            {
                QString tile = find_cloud_tile(c_y, c_x);
                if (!tile.isEmpty())
                {
                    painter->setOpacity(0.7);
                    QPixmap pix = parent->get_tile(tile, parent->main_cell_hgt, parent->main_cell_wid);
                    painter->drawPixmap(0, 0, pix);
                    painter->setOpacity(1);
                    done_bg = true;
                }
            }

            // Draw foreground tile
            if (key2.length() > 0)
            {
                QPixmap pix;
                if (double_height_mon)
                {
                    // Use only the bottom half of the tile
                    pix = parent->get_tile(key2, parent->main_cell_hgt*2, parent->main_cell_wid);
                    QRect this_rect(0,pix.height()/2+1, pix.width(), pix.height());
                    pix = pix.copy(this_rect);
                }

                else pix = parent->get_tile(key2, parent->main_cell_hgt, parent->main_cell_wid);
                if (flags & (UI_TRANSPARENT_EFFECT | UI_TRANSPARENT_MONSTER))
                {
                   painter->setOpacity(opacity);
                }
                painter->drawPixmap(pix.rect(), pix, pix.rect());
                painter->setOpacity(1);
                done_fg = true;
            }

            // draw foreground circle for dtrap edge
            else if (d_ptr->dtrap && !double_height_mon_below)
            {
                QPixmap sample = parent->get_tile(key1, parent->main_cell_hgt, parent->main_cell_wid);
                int height = sample.height();
                int width = sample.width();
                QBrush brush(Qt::green);
                painter->setPen(Qt::green);
                painter->setBrush(brush);
                painter->setOpacity(0.7);
                painter->drawEllipse(width/3, height/3, width/3, height/3);
                painter->setOpacity(1);
                painter->setBrush(Qt::NoBrush);
                done_fg = true;
            }
            if (double_height_mon_below)
            {
                dungeon_type *d2_ptr = &dungeon_info[c_y+1][c_x];
                // Use only the top half of the tile
                QPixmap pix = parent->get_tile(d2_ptr->monster_tile, parent->main_cell_hgt*2, parent->main_cell_wid);
                QRect this_rect(0, 0, pix.width(), pix.height()/2);
                pix = pix.copy(this_rect);

                painter->setOpacity(opacity - .3);
                painter->drawPixmap(pix.rect(), pix, pix.rect());
                painter->setOpacity(1);
                done_fg = true;
            }

            if (do_shadow)
            {
                QPixmap pix = pseudo_ascii(square_char, square_color, parent->font_main_window,
                                           QSizeF(parent->main_cell_wid, parent->main_cell_hgt));
                painter->drawPixmap(pix.rect(), pix, pix.rect());
                done_fg = true;
            }
        }
    }

    // Go ascii?
    if (!done_fg && (!empty || !done_bg))
    {
        painter->setFont(parent->font_main_window);
        painter->setPen(square_color);
        painter->drawText(QRectF(0, 0, parent->main_cell_wid, parent->main_cell_hgt),
                          Qt::AlignCenter, QString(square_char));
    }

    // Show a red line over a monster with its remaining hp
    if (d_ptr->has_visible_monster())
    {
        int cur = p_ptr->chp;
        int max = p_ptr->mhp;
        if (d_ptr->monster_idx > 0)
        {
            monster_type *m_ptr = mon_list + d_ptr->monster_idx;
            cur = m_ptr->hp;
            max = m_ptr->maxhp;
        }
        if (max > 0 && cur < max)
        {
            int w = parent->main_cell_wid * cur / max;
            w = MAX(w, 1);
            int h = 1;
            if (parent->main_cell_hgt > 16) h = 2;
            QColor color("red");
            if (cur * 100 / max > 50) color = QColor("yellow");
            painter->fillRect(0, 0, w, h, color);
        }
    }

    // Draw a mark for visible artifacts
    if (d_ptr->has_visible_artifact())
    {
        int s = 6;
        QPointF points[] =
        {
            QPointF(parent->main_cell_wid - s, parent->main_cell_hgt),
            QPointF(parent->main_cell_wid, parent->main_cell_hgt),
            QPointF(parent->main_cell_wid, parent->main_cell_hgt - s)
        };
        painter->setBrush(QColor("violet"));
        painter->setPen(Qt::NoPen);
        painter->drawPolygon(points, 3);
    }

    painter->restore();
}

QPainterPath DungeonGrid::shape() const
{
    QPainterPath p;
    p.addRect(boundingRect());
    return p;
}

void DungeonGrid::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!character_dungeon) return;

    if (parent->anim_depth > 0) return;

    if (parent->check_disturb()) return;

    bool left_button = (event->button() & Qt::LeftButton);
    bool right_button = (event->button() & Qt::RightButton);
    bool middle_button = (event->button() & Qt::MiddleButton);
    bool extra1 = (event->button() & Qt::XButton1);
    bool extra2 = (event->button() & Qt::XButton2);

    int old_x = parent->cursor->c_x;
    int old_y = parent->cursor->c_y;
    parent->grids[old_y][old_x]->update();
    if (parent->ui_mode == UI_MODE_INPUT)
    {
        parent->input.x = c_x;
        parent->input.y = c_y;
        parent->input.mode = INPUT_MODE_MOUSE;
        parent->ui_mode = UI_MODE_DEFAULT;
        parent->ev_loop.quit();
    }
    else if (!parent->ev_loop.isRunning())
    {
        if (right_button)
        {
            parent->cursor->setVisible(true);
            parent->cursor->moveTo(c_y, c_x);
            GridDialog dlg(c_y, c_x);
        }
        else if (left_button) do_cmd_findpath(c_y, c_x);
        else if (middle_button)
        {
            do_cmd_walk(ui_get_dir_from_slope(p_ptr->py, p_ptr->px, c_y, c_x), FALSE);
        }
        else if (extra1)
        {
            do_cmd_all_objects(TAB_INVEN);
        }
        else if (extra2)
        {
            do_cmd_character_screen();
        }
    }

    handle_stuff();


    QGraphicsItem::mousePressEvent(event);
}
