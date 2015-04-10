/* File: qt_mainwindow.cpp */

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


#include <QTextStream>
#include <QGraphicsRectItem>
#include <QHeaderView>
#include <QGraphicsScale>
#include <QFileDialog>
#include <QToolButton>
#include <QPushButton>
#include <QGraphicsSceneMouseEvent>
#include <QSplitter>
#include <QApplication>
#include <QFontDialog>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QScrollBar>
#include <QDockWidget>

#include "src/npp.h"
#include "src/qt_mainwindow.h"
#include "src/init.h"
#include "src/optionsdialog.h"
#include <src/command_list.h>
#include <src/player_command.h>
#include <src/object_all_menu.h>
#include "src/player_birth.h"
#include "src/utilities.h"
#include "src/knowledge.h"
#include "src/help.h"
#include "src/emitter.h"
#include "griddialog.h"
#include "package.h"
#include "tilebag.h"
#include <src/messages.h>


MainWindow *main_window = 0;

#define FONT_EXTRA 4

static QPixmap gray_pix(QPixmap src)
{
    QImage img = src.toImage();
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            QColor col = QColor(img.pixel(x, y)).darker();
            int gray = qGray(col.rgb());
            img.setPixel(x, y, qRgb(gray, gray, gray));
        }
    }
    return QPixmap::fromImage(img);
}


static QPixmap darken_pix(QPixmap src)
{
    QImage img = src.toImage();
    QPainter p(&img);
    p.setCompositionMode(QPainter::CompositionMode_HardLight);
    p.fillRect(img.rect(), QColor("#444"));
    QPixmap pix = QPixmap::fromImage(img);
    return pix;
}


void MainWindow::slot_redraw()
{
    //redraw();

    //ui_animate_victory(p_ptr->py, p_ptr->px);

    QString txt = get_string("Enter text to convert", "With accents please...", "");
    txt = to_ascii(txt);
    message(txt);
}

void MainWindow::wait_animation(int n_animations)
{
    anim_depth += n_animations;

    if (anim_depth == n_animations) {
        if (anim_loop.isRunning()) qDebug("Already running animation");
        //qDebug("Animation loop %x", (int)&anim_loop);
        if (anim_loop.exec() == -1) {
            qDebug("Exec failed");
        }
    }
}

void MainWindow::animation_done()
{
    if (--anim_depth < 1) {
        if (!anim_loop.isRunning()) qDebug("Anim loop isn't running");
        //qDebug("Quitting %1", anim_depth);
        anim_loop.quit();
        anim_depth = 0;
        // MEGA HACK - Process some pending events (not user input) to (hopefully) avoid strange behavior
        anim_loop.processEvents(QEventLoop::ExcludeUserInputEvents, 100);
    }
}

void MainWindow::do_create_package()
{
    PackageDialog dlg("create");
}

void MainWindow::do_extract_from_package()
{
    PackageDialog dlg("extract");
}

PackageDialog::PackageDialog(QString _mode)
{
    mode = _mode;

    central = new QWidget;
    QVBoxLayout *lay1 = new QVBoxLayout;
    central->setLayout(lay1);
    this->setClient(central);

    QWidget *area2 = new QWidget;
    lay1->addWidget(area2);
    QGridLayout *lay2 = new QGridLayout;
    lay2->setContentsMargins(0, 0, 0, 0);
    lay2->setColumnStretch(1, 1);
    area2->setLayout(lay2);

    int row = 0;

    QLabel *lb = new QLabel;
    if (mode == "create") {
        lb->setText("Create a tile package");
    }
    else {
        lb->setText("Extract tiles from a package");
    }
    lb->setStyleSheet("font-size: 1.5em; font-weight: bold;");
    lay2->addWidget(lb, row, 0, 1, 3);

    ++row;

    lay2->addWidget(new QLabel(tr("Package")), row, 0);

    pak_path = new QLineEdit;
    pak_path->setReadOnly(true);
    lay2->addWidget(pak_path, row, 1);

    QToolButton *btn2 = new QToolButton();
    btn2->setText("...");
    //btn2->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    lay2->addWidget(btn2, row, 2);
    connect(btn2, SIGNAL(clicked()), this, SLOT(find_pak()));

    ++row;

    lay2->addWidget(new QLabel(tr("Tiles folder")), row, 0);

    folder_path = new QLineEdit;
    folder_path->setReadOnly(true);
    lay2->addWidget(folder_path, row, 1);

    QToolButton *btn3 = new QToolButton();
    btn3->setText("...");
    //btn3->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    lay2->addWidget(btn3, row, 2);
    connect(btn3, SIGNAL(clicked()), this, SLOT(find_folder()));

    ++row;

    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
    lay2->addItem(spacer, row, 0);

    QWidget *area3 = new QWidget;
    lay1->addWidget(area3);
    QHBoxLayout *lay3 = new QHBoxLayout;
    area3->setLayout(lay3);
    lay3->setContentsMargins(0, 0, 0, 0);

    spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    lay3->addItem(spacer);

    QPushButton *btn1 = new QPushButton(tr("Go!"));
    lay3->addWidget(btn1);
    connect(btn1, SIGNAL(clicked()), this, SLOT(do_accept()));

    QPushButton *btn4 = new QPushButton(tr("Cancel"));
    lay3->addWidget(btn4);
    connect(btn4, SIGNAL(clicked()), this, SLOT(reject()));

    this->clientSizeUpdated();
    this->exec();
}

void PackageDialog::do_accept()
{
    if (pak_path->text().isEmpty() || folder_path->text().isEmpty()) {
        pop_up_message_box(tr("Complete both fields"), QMessageBox::Critical);
        return;
    }

    if (mode == "create") {
        int n = create_package(pak_path->text(), folder_path->text());
        pop_up_message_box(tr("Imported tiles: %1").arg(n));
    }
    else {
        Package pak(pak_path->text());
        if (!pak.is_open()) {
            pop_up_message_box(tr("Couldn't load the package"), QMessageBox::Critical);
        }
        else {
            int n = pak.extract_to(folder_path->text());
            pop_up_message_box(tr("Extracted tiles: %1").arg(n));
        }
    }
}

void PackageDialog::find_pak()
{
    QString path;

    if (mode == "extract") {
        path = QFileDialog::getOpenFileName(this, tr("Select a package"), "",
                                            tr("Packages (*.pak)"));
    }
    else {
        path = QFileDialog::getSaveFileName(this, tr("Select a package"), "",
                                            tr("Packages (*.pak)"));
    }

    if (path != "") pak_path->setText(path);
}

void PackageDialog::find_folder()
{
    QString path;

    path = QFileDialog::getExistingDirectory(this, tr("Select a folder"));

    if (path != "") folder_path->setText(path);
}

QPainterPath DungeonGrid::shape() const
{
    QPainterPath p;
    p.addRect(boundingRect());
    return p;
}

QPainterPath DungeonCursor::shape() const
{
    QPainterPath p;
    p.addRect(boundingRect());
    return p;
}

void MainWindow::force_redraw()
{
    graphics_view->viewport()->update();
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


    QGraphicsItem::mousePressEvent(event);
}

void MainWindow::update_cursor()
{
    cursor->moveTo(p_ptr->py, p_ptr->px);
    cursor->setVisible(hilight_player);
}

DungeonCursor::DungeonCursor(MainWindow *_parent)
{
    parent = _parent;
    c_x = c_y = 0;
    setZValue(100);
    setVisible(false);
}

QRectF DungeonCursor::boundingRect() const
{
    return QRectF(0, 0, parent->cell_wid, parent->cell_hgt);
}

void DungeonCursor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;

    if (!character_dungeon) return;

    if (!in_bounds(c_y, c_x)) return;

    painter->save();

    if (parent->ui_mode == UI_MODE_INPUT) {
        painter->setOpacity(0.5);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor("red"));
        painter->drawRect(this->boundingRect());
    }

    painter->setOpacity(1);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QColor("yellow"));
    painter->drawRect(0, 0, parent->cell_wid - 1, parent->cell_hgt - 1);
    if ((parent->cell_wid > 16) && (parent->cell_hgt > 16)) {
        int z = 3;
        painter->drawRect(0, 0, z, z);
        painter->drawRect(parent->cell_wid - z - 1, 0, z, z);
        painter->drawRect(0, parent->cell_hgt - z - 1, z, z);
        painter->drawRect(parent->cell_wid - z - 1, parent->cell_hgt - z - 1, z, z);
    }

    painter->restore();
}

void DungeonCursor::moveTo(int _y, int _x)
{
    c_x = _x;
    c_y = _y;
    setPos(c_x * parent->cell_wid, c_y * parent->cell_hgt);
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
    return QRectF(0, 0, parent->cell_wid, parent->cell_hgt);
}

QString find_cloud_tile(int y, int x)
{
    QString tile;

    if (!use_graphics) return tile;

    if (!(dungeon_info[y][x].cave_info & (CAVE_MARK | CAVE_SEEN))) return tile;

    int x_idx = dungeon_info[y][x].effect_idx;
    while (x_idx) {
        effect_type *x_ptr = x_list + x_idx;
        x_idx = x_ptr->next_x_idx;

        if (x_ptr->x_flags & EF1_HIDDEN) continue;

        if (x_ptr->x_type == EFFECT_PERMANENT_CLOUD || x_ptr->x_type == EFFECT_LINGERING_CLOUD
                || x_ptr->x_type == EFFECT_SHIMMERING_CLOUD) {
            int feat = x_ptr->x_f_idx;
            feat = f_info[feat].f_mimic;
            return f_info[feat].tile_id;
        }
    }

    return tile;
}

void DungeonGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;

    if (!character_dungeon) return;

    if (!in_bounds(c_y, c_x)) return;

    painter->fillRect(QRectF(0, 0, parent->cell_wid, parent->cell_hgt), Qt::black);

    dungeon_type *d_ptr = &dungeon_info[c_y][c_x];
    QChar square_char = d_ptr->dun_char;
    QColor square_color = d_ptr->dun_color;
    bool empty = true;
    u32b flags = 0;
    QString key2;
    qreal opacity = 1;
    bool do_shadow = false;

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

    if (use_graphics)
    {
        // Draw background tile
        QString key1 = d_ptr->dun_tile;

        if (key1.length() > 0) {            
            QPixmap pix = parent->get_tile(key1);

            if (flags & UI_LIGHT_TORCH) {
                QColor color = QColor("yellow").darker(150);
                if (flags & UI_COSMIC_TORCH) color = QColor("cyan").darker(150);
                pix = colorize_pix(pix, color);
            }
            else if (flags & UI_LIGHT_BRIGHT) {
                pix = parent->apply_shade(key1, pix, "bright");
            }
            else if (flags & UI_LIGHT_DIM) {
                pix = parent->apply_shade(key1, pix, "dim");
            }

            painter->drawPixmap(pix.rect(), pix, pix.rect());
            done_bg = true;

            // Draw cloud effects (in graphics mode), if not already drawing that
            if (!is_cloud) {
                QString tile = find_cloud_tile(c_y, c_x);
                if (!tile.isEmpty()) {
                    painter->setOpacity(0.7);                    
                    QPixmap pix = parent->get_tile(tile);
                    painter->drawPixmap(0, 0, pix);
                    painter->setOpacity(1);
                    done_bg = true;
                }
            }

            // Draw foreground tile
            if (key2.length() > 0) {               
               QPixmap pix = parent->get_tile(key2);
               if (flags & (UI_TRANSPARENT_EFFECT | UI_TRANSPARENT_MONSTER)) {
                   painter->setOpacity(opacity);
               }               
               painter->drawPixmap(pix.rect(), pix, pix.rect());
               painter->setOpacity(1);
               done_fg = true;
            }
            // draw foreground circle for dtrap edge
            else if (d_ptr->dtrap)
            {
                QPixmap sample = parent->get_tile(key1);
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

            if (do_shadow) {
                QPixmap pix = pseudo_ascii(square_char, square_color, parent->font_main_window,
                                           QSizeF(parent->cell_wid, parent->cell_hgt));
                painter->drawPixmap(pix.rect(), pix, pix.rect());
                done_fg = true;
            }
        }
    }

    // Go ascii?
    if (!done_fg && (!empty || !done_bg)) {
        painter->setFont(parent->font_main_window);
        painter->setPen(square_color);
        painter->drawText(QRectF(0, 0, parent->cell_wid, parent->cell_hgt),
                          Qt::AlignCenter, QString(square_char));
    }

    // Show a red line over a monster with its remaining hp
    if (d_ptr->has_visible_monster()) {
        int cur = p_ptr->chp;
        int max = p_ptr->mhp;
        if (d_ptr->monster_idx > 0) {
            monster_type *m_ptr = mon_list + d_ptr->monster_idx;
            cur = m_ptr->hp;
            max = m_ptr->maxhp;
        }
        if (max > 0 && cur < max) {
            int w = parent->cell_wid * cur / max;
            w = MAX(w, 1);
            int h = 1;
            if (parent->cell_hgt > 16) h = 2;
            QColor color("red");
            if (cur * 100 / max > 50) color = QColor("yellow");
            painter->fillRect(0, 0, w, h, color);
        }
    }

    // Draw a mark for visible artifacts
    if (d_ptr->has_visible_artifact()) {
        int s = 6;
        QPointF points[] = {
            QPointF(parent->cell_wid - s, parent->cell_hgt),
            QPointF(parent->cell_wid, parent->cell_hgt),
            QPointF(parent->cell_wid, parent->cell_hgt - s)
        };
        painter->setBrush(QColor("violet"));
        painter->setPen(Qt::NoPen);
        painter->drawPolygon(points, 3);
    }

    painter->restore();
}

QPixmap MainWindow::apply_shade(QString tile_id, QPixmap tile, QString shade_id)
{
    tile_id += ":";
    tile_id += shade_id;    

    if (shade_cache.contains(tile_id)) return shade_cache.value(tile_id);

    QPixmap pix;

    if (shade_id == "dim") {
        pix = gray_pix(tile);
    }
    else if (shade_id == "bright") {
        pix = darken_pix(tile);
    }
    else {  // It should never happen
        pix = tile;
    }

    shade_cache.insert(tile_id, pix);

    return pix;
}



void MainWindow::destroy_tiles()
{    
    tiles.clear();
    shade_cache.clear();
}

QPixmap MainWindow::get_tile(QString tile_id)
{
    if (tiles.contains(tile_id)) return tiles.value(tile_id);

    if (!current_tiles) return ui_make_blank();

    QPixmap pix = current_tiles->get_tile(tile_id);

    if (pix.width() == 1) return pix;

    if (cell_wid != pix.width() || cell_hgt != pix.height()) {
        pix = pix.scaled(cell_wid, cell_hgt);
    }

    tiles.insert(tile_id, pix);

    return pix;
}

void MainWindow::calculate_cell_size()
{
    cell_wid = MAX(tile_wid, font_wid);

    cell_hgt = MAX(tile_hgt, font_hgt);

    for (int y = 0; y < MAX_DUNGEON_HGT; y++) {
        for (int x = 0; x < MAX_DUNGEON_WID; x++) {
            grids[y][x]->cellSizeChanged();
            grids[y][x]->setPos(x * cell_wid, y * cell_hgt);
        }
    }

    cursor->cellSizeChanged();
}

void MainWindow::set_graphic_mode(int mode)
{    
    int cy = -1;
    int cx = -1;

    // Remember the center of the view
    if (character_dungeon)
    {
        QRect vis = visible_dungeon();
        cy = vis.y() + vis.height() / 2;
        cx = vis.x() + vis.width() / 2;
    }

    switch (mode)
    {
        case GRAPHICS_DAVID_GERVAIS:
        {
            tile_hgt = 32;
            tile_wid = 32;
            current_tiles = tiles_32x32;
            ascii_mode_act->setChecked(FALSE);
            dvg_mode_act->setChecked(TRUE);
            old_tiles_act->setChecked(FALSE);
            break;
        }
        case GRAPHICS_ORIGINAL:
        {
            tile_hgt = 8;
            tile_wid = 8;
            current_tiles = tiles_8x8;
            ascii_mode_act->setChecked(FALSE);
            dvg_mode_act->setChecked(FALSE);
            old_tiles_act->setChecked(TRUE);
            break;
        }
        default: //GRAPHICS_NONE:
        {
            tile_hgt = 0;
            tile_wid = 0;
            current_tiles = 0;
            ascii_mode_act->setChecked(TRUE);
            dvg_mode_act->setChecked(FALSE);
            old_tiles_act->setChecked(FALSE);
            break;
        }
    }

    use_graphics = mode;
    calculate_cell_size();
    destroy_tiles();
    if (character_dungeon) extract_tiles(false);
    update_sidebar_all();

    // Recenter the view
    if (cy != -1 && cx != -1) {
        ui_redraw_all();
        ui_center(cy, cx);
    }
}

void MainWindow::set_keymap_mode(int mode)
{
    which_keyset = mode;

    if (mode == KEYSET_ANGBAND)
    {
        keymap_new->setChecked(FALSE);
        keymap_angband->setChecked(TRUE);
        keymap_rogue->setChecked(FALSE);
    }
    else if (mode == KEYSET_ROGUE)
    {
        keymap_new->setChecked(FALSE);
        keymap_angband->setChecked(FALSE);
        keymap_rogue->setChecked(TRUE);
    }
    else // (mode == KEYSET_NEW)
    {
        keymap_new->setChecked(TRUE);
        keymap_angband->setChecked(FALSE);
        keymap_rogue->setChecked(FALSE);
    }
}

void MainWindow::set_font_main_window(QFont newFont)
{
    font_main_window = newFont;
    QFontMetrics metrics(font_main_window);
    font_hgt = metrics.height() + FONT_EXTRA;
    font_wid = metrics.width('M') + FONT_EXTRA;

    calculate_cell_size();

    destroy_tiles();
}

void MainWindow::set_font_message_window(QFont newFont)
{
    font_message_window = newFont;
    ui_update_messages();
}

void MainWindow::set_font_sidebar_window(QFont newFont)
{
    font_sidebar_window = newFont;
    update_sidebar_font();
}

void MainWindow::init_scene()
{
    QFontMetrics metrics(font_main_window);

    font_hgt = metrics.height() + FONT_EXTRA;
    font_wid = metrics.width('M') + FONT_EXTRA;
    tile_hgt = tile_wid = 0;
    cell_hgt = cell_wid = 0;

    QBrush brush(QColor("black"));
    dungeon_scene->setBackgroundBrush(brush);    

    for (int y = 0; y < MAX_DUNGEON_HGT; y++) {
        for (int x = 0; x < MAX_DUNGEON_WID; x++) {
            grids[y][x] = new DungeonGrid(x, y, this);
            dungeon_scene->addItem(grids[y][x]);
        }
    }

    dungeon_scene->addItem(cursor);
}

void MainWindow::redraw()
{    
    // Important. No dungeon yet
    if (!character_dungeon) {
        if (graphics_view) force_redraw();
        return;
    }

    // TODO PLAYTESTING. DONT REMOVE YET
    //wiz_light();

    // Adjust scrollbars
    graphics_view->setSceneRect(0, 0, p_ptr->cur_map_wid * cell_wid, p_ptr->cur_map_hgt * cell_hgt);

    for (int y = 0; y < p_ptr->cur_map_hgt; y++) {
        for (int x = 0; x < p_ptr->cur_map_wid; x++) {
            light_spot(y, x);
        }
    }

    //ui_center(p_ptr->py, p_ptr->px);
    update_cursor();
    force_redraw(); // Hack -- Force full redraw
    update_messages();
    update_sidebar_all();
    win_mon_list_update();
    win_obj_list_update();
}

bool MainWindow::panel_contains(int y, int x)
{
    QPolygonF pol = graphics_view->mapToScene(graphics_view->viewport()->geometry());
    // We test top-left and bottom-right corners of the cell
    QPointF point1(x * cell_wid, y * cell_hgt);
    QPointF point2(x * cell_wid + cell_wid, y * cell_hgt + cell_hgt);
    return pol.containsPoint(point1, Qt::OddEvenFill) && pol.containsPoint(point2, Qt::OddEvenFill);
}

QPixmap pseudo_ascii(QChar chr, QColor color, QFont font, QSizeF size)
{
    QImage img(size.width(), size.height(), QImage::Format_ARGB32);
    // Fill with transparent color
    for (int x = 0; x < size.width(); x++) {
        for (int y = 0; y < size.height(); y++) {
            img.setPixel(x, y, QColor(0, 0, 0, 0).rgba());
        }
    }

    QPainter p(&img);
    p.setPen(color);
    p.setFont(font);
    // Draw the text once to get the shape of the letter plus antialiasing
    p.drawText(img.rect(), Qt::AlignCenter, QString(chr));

    // Mark colored grids
    bool marks[img.width()][img.height()];
    for (int x = 0; x < size.width(); x++) {
        for (int y = 0; y < size.height(); y++) {
            QRgb pixel = img.pixel(x, y);
            if (qAlpha(pixel) > 0) {
                marks[x][y] = true;
            }
            else {
                marks[x][y] = false;
            }
        }
    }

    // Surround with black. Note that all concerning grids are burned, even marked ones
    for (int x = 0; x < size.width(); x++) {
        for (int y = 0; y < size.height(); y++) {
            if (!marks[x][y]) continue;
            for (int y1 = y - 1; y1 <= y + 1; y1++) {
                for (int x1 = x - 1; x1 <= x + 1; x1++) {
                    if (!img.rect().contains(x1, y1, false)) continue;
                    img.setPixel(x1, y1, qRgba(0, 0, 0, 255));
                }
            }
        }
    }

    // Draw the text again so the antialiasing pixels blend with black properly
    p.drawText(img.rect(), Qt::AlignCenter, QString(chr));

    return QPixmap::fromImage(img);
}



// The main function - intitalize the main window and set the menus.
MainWindow::MainWindow()
{
    // Store a reference for public functions (panel_contains and others)
    if (!main_window) main_window = this;

    anim_depth = 0;
    which_keyset = KEYSET_NEW;
    show_obj_list = show_mon_list = FALSE;
    show_obj_recall = show_mon_recall = show_feat_recall = FALSE;
    character_dungeon = character_generated = character_loaded = FALSE;

    setAttribute(Qt::WA_DeleteOnClose);

    ui_mode = UI_MODE_DEFAULT;

    cursor = new DungeonCursor(this);
    do_pseudo_ascii = false;

    current_multiplier = "1:1";

    // Set the main area
    dungeon_scene = new QGraphicsScene;
    graphics_view = new QGraphicsView(dungeon_scene);
    graphics_view->installEventFilter(this);
    setCentralWidget(graphics_view);

    // Set up the message area
    message_dock = new QDockWidget;
    message_area = new QTextEdit;
    message_area->setReadOnly(true);
    message_area->setStyleSheet("background-color: black;");
    message_dock->setWidget(message_area);
    message_dock->setAllowedAreas(Qt::TopDockWidgetArea);
    message_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::TopDockWidgetArea, message_dock);


    // Set up the sidebar area, make it scrollable in case there are alot of monsters
    sidebar_widget = new QWidget;
    sidebar_dock = new QDockWidget;
    sidebar_dock->setAllowedAreas(Qt::LeftDockWidgetArea);
    sidebar_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    sidebar_scroll = new QScrollArea;
    QPalette this_pal;
    this_pal.setColor(QPalette::Background, Qt::black);
    sidebar_widget->setAutoFillBackground(TRUE);
    sidebar_widget->setPalette(this_pal);
    sidebar_scroll->setPalette(this_pal);
    sidebar_dock->setWidget(sidebar_scroll);
    sidebar_scroll->setWidget(sidebar_widget);
    sidebar_scroll->setWidgetResizable(TRUE);
    sidebar_scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sidebar_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    sidebar_vlay = new QVBoxLayout;
    sidebar_widget->setLayout(sidebar_vlay);

    create_sidebar();

    addDockWidget(Qt::LeftDockWidgetArea, sidebar_dock);

    // Set up all the folder directories
    create_directories();

    create_actions();

    create_menus();
    create_toolbars();
    select_font();
    create_signals();
    (void)statusBar();
    read_settings();
    init_scene();
    set_graphic_mode(use_graphics);
    set_keymap_mode(which_keyset);

    update_file_menu_game_inactive();

    setWindowFilePath(QString());
}

void MainWindow::setup_nppangband()
{
    game_mode = GAME_NPPANGBAND;

    setWindowTitle(tr("NPPAngband"));

    init_npp_games();
}

void MainWindow::setup_nppmoria()
{
    game_mode = GAME_NPPMORIA;

    setWindowTitle(tr("NPPMoria"));

    init_npp_games();
}


// Prepare to play a game of NPPAngband.
void MainWindow::start_game_nppangband()
{
    setup_nppangband();

    launch_birth(FALSE);
}

// Prepare to play a game of NPPMoria.
void MainWindow::start_game_nppmoria()
{
    setup_nppmoria();

    launch_birth(FALSE);
}

void MainWindow::open_current_savefile()
{
    // Let the user select the savefile
    QString file_name = QFileDialog::getOpenFileName(this, tr("Select a savefile"), npp_dir_save.path(), tr("NPP (*.npp)"));
    if (file_name.isEmpty()) return;

    load_file(file_name);
}

void MainWindow::save_character()
{
    if (current_savefile.isEmpty())
        save_character_as();
    else
        save_file(current_savefile);
}

void MainWindow::save_character_as()
{
    // Start with the current player name
    QString default_name = "player";
    if (!op_ptr->full_name.isEmpty())default_name = op_ptr->full_name;
    QString default_file = npp_dir_save.path();
    default_file.append("/");
    default_file.append(default_name);

    QString fileName = QFileDialog::getSaveFileName(this, tr("Select a savefile"), default_file, tr("NPP (*.npp)"));

    if (fileName.isEmpty())
        return;

    save_file(fileName);
}

void MainWindow::update_messages()
{
    update_message_area(message_area, 3);

    message_area->moveCursor(QTextCursor::End);
}


void MainWindow::close_game_death()
{
    save_and_close();
}

void MainWindow::save_and_close()
{
    if (running_command()) return;

    save_character();

    set_current_savefile("");

    // Wipe the extra windows
    win_mon_list_wipe();
    win_obj_list_wipe();
    win_mon_recall_wipe();
    win_obj_recall_wipe();
    win_feat_recall_wipe();

    character_loaded = character_dungeon = character_generated = false;

    update_file_menu_game_inactive();

    // close game
    cleanup_npp_games();

    message_area->clear();

    cursor->setVisible(false);
    destroy_tiles();
    redraw();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        this->keyPressEvent(dynamic_cast<QKeyEvent *>(event));

        return true;
    }

    return QObject::eventFilter(obj, event);
}

bool MainWindow::check_disturb()
{
    if (p_ptr->is_resting() || p_ptr->is_running() || p_ptr->command_current)
    {
        disturb(0, 0);
        message("Cancelled.");
        return true;
    }
    return false;
}

void MainWindow::keyPressEvent(QKeyEvent* which_key)
{
    if (!character_dungeon) return;
    if (p_ptr->in_store) return;
    if (anim_depth > 0) return;

    if (check_disturb()) return;

    QString keystring = which_key->text();

    // Go to special key handling
    if (ui_mode == UI_MODE_INPUT)
    {
        input.key = which_key->key();
        input.text = keystring;
        input.mode = INPUT_MODE_KEY;
        ui_mode = UI_MODE_DEFAULT;
        ev_loop.quit();
        return;
    }

    // Just a modifier was pressed, wait for a keypress
    if (!keystring.length()) return;

    Qt::KeyboardModifiers modifiers = which_key->modifiers();

    bool shift_key = modifiers.testFlag(Qt::ShiftModifier);
    bool ctrl_key = modifiers.testFlag(Qt::ControlModifier);
    bool alt_key = modifiers.testFlag(Qt::AltModifier);
    bool meta_key = modifiers.testFlag(Qt::MetaModifier);

    int key_press = which_key->key();

    if (which_keyset == KEYSET_NEW)
    {
        commands_new_keyset(key_press, shift_key, alt_key, ctrl_key, meta_key);
    }
    else if (which_keyset == KEYSET_ANGBAND)
    {
        commands_angband_keyset(key_press, shift_key, alt_key, ctrl_key, meta_key);
    }
    else if (which_keyset == KEYSET_ROGUE)
    {
        commands_roguelike_keyset(key_press, shift_key, alt_key, ctrl_key, meta_key);
    }
    else pop_up_message_box("invalid keyset");

    handle_stuff();
}


bool MainWindow::running_command()
{
    if (ev_loop.isRunning()) {
        pop_up_message_box("You must finish or cancel the current command first to do this");
        return true;
    }
    return false;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (running_command())
    {
        event->ignore();
        return;
    }

    if (!current_savefile.isEmpty() && character_dungeon)
    {
        save_character();
        pop_up_message_box("Game saved");
    }

    // We don't need to set settings for the docks
    delete message_dock;
    delete sidebar_dock;

    write_settings();

    // Take out the additional windows
    win_mon_list_destroy();
    win_obj_list_destroy();
    win_mon_recall_destroy();
    win_obj_recall_destroy();
    win_feat_recall_destroy();

    event->accept();
}

void MainWindow::open_recent_file()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        load_file(action->data().toString());
    }
}

void MainWindow::options_dialog()
{
    OptionsDialog *dlg = new OptionsDialog;
    dlg->exec();
    delete dlg;
    p_ptr->redraw |= (PR_MAP);
    handle_stuff();
}

void MainWindow::font_dialog_main_window()
{
    bool selected;
    QFont font = QFontDialog::getFont( &selected, font_main_window, this );

    if (selected)
    {
        set_font_main_window(font);
        redraw();
    }
}

void MainWindow::font_dialog_message_window()
{
    bool selected;
    QFont font = QFontDialog::getFont( &selected, font_message_window, this );

    if (selected)
    {
        set_font_message_window(font);
    }
}

void MainWindow::font_dialog_sidebar_window()
{
    bool selected;
    QFont font = QFontDialog::getFont( &selected, font_sidebar_window, this );

    if (selected)
    {
        set_font_sidebar_window(font);
    }
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About NPPAngband and NPPMoria"),
            tr("<h2>NPPAngband and NPPMoria"
               "<p>Copyright (c) 2003-2015 Jeff Greene and Diego González.</h2>"

               "<p>For resources and links to places you can talk about the game, please see:"
               "<p>http://forum.nppangband.org/ -- the NPPAngband Forums"

               "<p>Based on Moria: (c) 1985 Robert Alan Koeneke and Umoria (c) 1989 James E. Wilson, David Grabiner,"
               "<p>Angband 2.6.2:   Alex Cutler, Andy Astrand, Sean Marsh, Geoff Hill, Charles Teague, Charles Swiger, "
               "Angband 2.7.0 - 2.8.5:   Ben Harrison 2.9.0 - 3.0.6: Robert Ruehlmann, "
               "Angband 3.0.7 - 3.4.0:  Andrew Sidwell"
               "<p>Oangband 0.6.0 Copyright 1998-2003 Leon Marrick, Bahman Rabii"
               "<p>EYAngband 0.5.2 By Eytan Zweig, UNAngband by Andrew Doull"
               "<p>Sangband 0.9.9 and Demoband by Leon Marrick"
               "<p>FAangband 1.6 by Nick McConnell"
               "<p>Please see copyright.txt for complete copyright and licensing restrictions."));
}

void MainWindow::command_list()
{
    do_cmd_command_list();
}

// Activates and de-activates certain file_menu commands when a game is started.
// Assumes create_actions has already been called.
void MainWindow::update_file_menu_game_active()
{
    new_game_nppangband->setEnabled(FALSE);
    new_game_nppmoria->setEnabled(FALSE);
    open_savefile->setEnabled(FALSE);
    save_cur_char->setEnabled(TRUE);
    save_cur_char_as->setEnabled(TRUE);
    close_cur_char->setEnabled(TRUE);

    for (int i = 0; i < MAX_RECENT_SAVEFILES; ++i)
    {
        recent_savefile_actions[i]->setEnabled(FALSE);
    }

    options_act->setEnabled(TRUE);
    view_monster_knowledge->setEnabled(TRUE);
    view_object_knowledge->setEnabled(TRUE);
    view_ego_item_knowledge->setEnabled(TRUE);
    view_artifact_knowledge->setEnabled(TRUE);
    view_terrain_knowledge->setEnabled(TRUE);
    view_notes->setEnabled(TRUE);
    view_messages->setEnabled(TRUE);
    view_home_inven->setEnabled(TRUE);
    view_scores->setEnabled(TRUE);
    view_kill_count->setEnabled(TRUE);

    show_sidebar();
    show_statusbar();

}

// Activates and de-activates certain file_menu commands when a game is ended.
// Assumes create_actions has already been called.
void MainWindow::update_file_menu_game_inactive()
{
    new_game_nppangband->setEnabled(TRUE);
    new_game_nppmoria->setEnabled(TRUE);
    open_savefile->setEnabled(TRUE);
    save_cur_char->setEnabled(FALSE);
    save_cur_char_as->setEnabled(FALSE);
    close_cur_char->setEnabled(FALSE);

    for (int i = 0; i < MAX_RECENT_SAVEFILES; ++i)
    {
        recent_savefile_actions[i]->setEnabled(TRUE);
    }

    options_act->setEnabled(FALSE);
    view_monster_knowledge->setEnabled(FALSE);
    view_object_knowledge->setEnabled(FALSE);
    view_ego_item_knowledge->setEnabled(FALSE);
    view_artifact_knowledge->setEnabled(FALSE);
    view_terrain_knowledge->setEnabled(FALSE);
    view_notes->setEnabled(FALSE);
    view_messages->setEnabled(FALSE);
    view_home_inven->setEnabled(FALSE);
    view_scores->setEnabled(FALSE);
    view_kill_count->setEnabled(FALSE);

    hide_sidebar();
    hide_statusbar();

}


//  Set's up all the QActions that will be added to the menu bar.  These are later added by create_menus.
void MainWindow::create_actions()
{
    new_game_nppangband = new QAction(tr("New Game - NPPAngband"), this);
    new_game_nppangband->setStatusTip(tr("Start a new game of NPPAngband."));
    new_game_nppangband->setIcon(QIcon(":/icons/lib/icons/New_game_NPPAngband.png"));
    new_game_nppangband->setShortcut(tr("Ctrl+A"));
    connect(new_game_nppangband, SIGNAL(triggered()), this, SLOT(start_game_nppangband()));

    new_game_nppmoria = new QAction(tr("New Game - NPPMoria"), this);
    new_game_nppmoria->setStatusTip(tr("Start a new game of NPPMoria."));
    new_game_nppmoria->setIcon(QIcon(":/icons/lib/icons/New_Game_NPPMoria.png"));
    new_game_nppmoria->setShortcut(tr("Ctrl+R"));
    connect(new_game_nppmoria, SIGNAL(triggered()), this, SLOT(start_game_nppmoria()));

    open_savefile = new QAction(tr("Open Savefile"), this);
    open_savefile->setShortcut(tr("Ctrl+F"));
    open_savefile->setIcon(QIcon(":/icons/lib/icons/open_savefile.png"));
    open_savefile->setStatusTip(tr("Open an existing savefile."));
    connect(open_savefile, SIGNAL(triggered()), this, SLOT(open_current_savefile()));

    save_cur_char = new QAction(tr("Save Character"), this);
    save_cur_char->setShortcut(tr("Ctrl+S"));
    save_cur_char->setIcon(QIcon(":/icons/lib/icons/save.png"));
    save_cur_char->setStatusTip(tr("Save current character."));
    connect(save_cur_char, SIGNAL(triggered()), this, SLOT(save_character()));

    save_cur_char_as = new QAction(tr("Save Character As"), this);
    save_cur_char_as->setShortcut(tr("Ctrl+W"));
    save_cur_char_as->setIcon(QIcon(":/icons/lib/icons/save_as.png"));
    save_cur_char_as->setStatusTip(tr("Save current character to new file."));
    connect(save_cur_char_as, SIGNAL(triggered()), this, SLOT(save_character_as()));

    close_cur_char = new QAction(tr("Save And Close"), this);
    close_cur_char->setShortcut(tr("Ctrl+X"));
    close_cur_char->setIcon(QIcon(":/icons/lib/icons/close_game.png"));
    close_cur_char->setStatusTip(tr("Save and close current character."));
    connect(close_cur_char, SIGNAL(triggered()), this, SLOT(save_and_close()));

    exit_npp = new QAction(tr("Exit Game"), this);
    exit_npp->setShortcut(tr("Ctrl+Q"));
    exit_npp->setIcon(QIcon(":/icons/lib/icons/Exit.png"));
    exit_npp->setStatusTip(tr("Exit the application.  Save any open character."));
    connect(exit_npp, SIGNAL(triggered()), this, SLOT(close()));


    for (int i = 0; i < MAX_RECENT_SAVEFILES; ++i)
    {
        recent_savefile_actions[i] = new QAction(this);
        recent_savefile_actions[i]->setVisible(false);
        connect(recent_savefile_actions[i], SIGNAL(triggered()),
                this, SLOT(open_recent_file()));
    }

    options_act = new QAction(tr("Options"), this);
    options_act->setStatusTip(tr("Change the game options."));
    options_act->setShortcut(Qt::Key_Equal);
    options_act->setIcon(QIcon(":/icons/lib/icons/options.png"));
    connect(options_act, SIGNAL(triggered()), this, SLOT(options_dialog()));

    keymap_new = new QAction(tr("Simplified Command Set"), this);
    keymap_new->setStatusTip(tr("Use simplified keyset to enter commands (recommended for players new to Angband and variants"));
    connect(keymap_new, SIGNAL(triggered()), this, SLOT(slot_simplified_keyset()));

    keymap_angband = new QAction(tr("Angband Command Set"), this);
    keymap_angband->setStatusTip(tr("Use the classic Angband keyset to enter commands"));
    connect(keymap_angband, SIGNAL(triggered()), this, SLOT(slot_angband_keyset()));

    keymap_rogue = new QAction(tr("Roguelike Command Set"), this);
    keymap_rogue->setStatusTip(tr("Use the roguelike keyset to enter commands"));
    connect(keymap_rogue, SIGNAL(triggered()), this, SLOT(slot_rogue_keyset()));

    ascii_mode_act = new QAction(tr("Ascii graphics"), this);
    ascii_mode_act->setStatusTip(tr("Set the graphics to ascii mode."));
    connect(ascii_mode_act, SIGNAL(triggered()), this, SLOT(set_ascii()));

    dvg_mode_act = new QAction(tr("David Gervais tiles"), this);
    dvg_mode_act->setStatusTip(tr("Set the graphics to David Gervais tiles mode."));
    connect(dvg_mode_act, SIGNAL(triggered()), this, SLOT(set_dvg()));

    old_tiles_act = new QAction(tr("8x8 tiles"), this);
    old_tiles_act->setStatusTip(tr("Set the graphics to 8x8 tiles mode."));
    connect(old_tiles_act, SIGNAL(triggered()), this, SLOT(set_old_tiles()));

    pseudo_ascii_act = new QAction(tr("Pseudo-Ascii monsters"), this);
    pseudo_ascii_act->setCheckable(true);
    pseudo_ascii_act->setChecked(false);
    pseudo_ascii_act->setStatusTip(tr("Set the monsters graphics to pseudo-ascii."));
    connect(pseudo_ascii_act, SIGNAL(changed()), this, SLOT(set_pseudo_ascii()));

    font_main_select_act = new QAction(tr("Main Window Font"), this);
    font_main_select_act->setStatusTip(tr("Change the font or font size for the main window."));
    connect(font_main_select_act, SIGNAL(triggered()), this, SLOT(font_dialog_main_window()));

    font_messages_select_act = new QAction(tr("Message Window Font"), this);
    font_messages_select_act->setStatusTip(tr("Change the font or font size for the message window."));
    connect(font_messages_select_act, SIGNAL(triggered()), this, SLOT(font_dialog_message_window()));

    font_sidebar_select_act = new QAction(tr("Sidebar Window Font"), this);
    font_sidebar_select_act->setStatusTip(tr("Change the font or font size for the sidebar window."));
    connect(font_sidebar_select_act, SIGNAL(triggered()), this, SLOT(font_dialog_sidebar_window()));

    view_monster_knowledge = new QAction(tr("View Monster Knowledge"), this);
    view_monster_knowledge->setStatusTip(tr("View all information the character knows about the monsters."));
    connect(view_monster_knowledge, SIGNAL(triggered()), this, SLOT(display_monster_info()));

    view_object_knowledge = new QAction(tr("View Object Knowledge"), this);
    view_object_knowledge->setStatusTip(tr("View all information the character knows about game objects."));
    connect(view_object_knowledge, SIGNAL(triggered()), this, SLOT(display_object_info()));

    view_ego_item_knowledge = new QAction(tr("View Ego Item Knowledge"), this);
    view_ego_item_knowledge->setStatusTip(tr("View all information the character knows about ego item objects."));
    connect(view_ego_item_knowledge, SIGNAL(triggered()), this, SLOT(display_ego_item_info()));

    view_artifact_knowledge = new QAction(tr("View Artifact Knowledge"), this);
    view_artifact_knowledge->setStatusTip(tr("View all information the character knows about artifacts."));
    connect(view_artifact_knowledge, SIGNAL(triggered()), this, SLOT(display_artifact_info()));

    view_terrain_knowledge = new QAction(tr("View Terrain Knowledge"), this);
    view_terrain_knowledge->setStatusTip(tr("View all information the character knows about terrains and features."));
    connect(view_terrain_knowledge, SIGNAL(triggered()), this, SLOT(display_terrain_info()));

    view_notes = new QAction(tr("View Game Notes"), this);
    view_notes->setStatusTip(tr("View the notes file listing the character's game highlights."));
    connect(view_notes, SIGNAL(triggered()), this, SLOT(display_notes()));

    view_messages = new QAction(tr("View Messages"), this);
    view_messages->setStatusTip(tr("View the message log."));
    view_messages->setShortcut(tr("Ctrl+L"));
    connect(view_messages, SIGNAL(triggered()), this, SLOT(display_messages()));

    view_home_inven = new QAction(tr("View Home Inventory"), this);
    view_home_inven->setStatusTip(tr("View the inventory stored in the character's home."));
    connect(view_home_inven, SIGNAL(triggered()), this, SLOT(display_home()));

    view_scores = new QAction(tr("View Player Scores"), this);
    view_scores->setStatusTip(tr("View the scores for all characters."));
    connect(view_scores, SIGNAL(triggered()), this, SLOT(display_scores()));

    view_kill_count = new QAction(tr("View Monster Kill Count"), this);
    view_kill_count->setStatusTip(tr("View the number of kills sorted by monster race."));
    connect(view_kill_count, SIGNAL(triggered()), this, SLOT(display_kill_count()));

    win_mon_list = new QAction(tr("Show Monster List Window"), this);
    win_mon_list->setStatusTip(tr("Displays a list of all the visible monsters on the level."));
    connect(win_mon_list, SIGNAL(triggered()), this, SLOT(toggle_win_mon_list()));

    win_obj_list = new QAction(tr("Show Object List Window"), this);
    win_obj_list->setStatusTip(tr("Displays a list of all visible objects on the level."));
    connect(win_obj_list, SIGNAL(triggered()), this, SLOT(toggle_win_obj_list()));

    win_mon_recall = new QAction(tr("Show Monster Recall Window"), this);
    win_mon_recall->setStatusTip(tr("Displays all known information about a given monster race."));
    connect(win_mon_recall, SIGNAL(triggered()), this, SLOT(toggle_win_mon_recall()));

    win_obj_recall = new QAction(tr("Show Object Recall Window"), this);
    win_obj_recall->setStatusTip(tr("Displays all known information about a given object."));
    connect(win_obj_recall, SIGNAL(triggered()), this, SLOT(toggle_win_obj_recall()));

    win_feat_recall = new QAction(tr("Show Feature Recall Window"), this);
    win_feat_recall->setStatusTip(tr("Displays all known information about a given feature."));
    connect(win_feat_recall, SIGNAL(triggered()), this, SLOT(toggle_win_feat_recall()));

    help_about = new QAction(tr("&About"), this);
    help_about->setStatusTip(tr("Show the application's About box"));
    connect(help_about, SIGNAL(triggered()), this, SLOT(about()));

    help_about_Qt = new QAction(tr("About &Qt"), this);
    help_about_Qt->setStatusTip(tr("Show the Qt library's About box"));
    connect(help_about_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    help_command_list = new QAction(tr("&Show Command List"), this);
    help_command_list->setShortcut(Qt::Key_Question);
    help_command_list->setStatusTip(tr("Show a list of all keybord commands"));
    connect(help_command_list, SIGNAL(triggered()), this, SLOT(command_list()));
}

void MainWindow::set_dvg()
{
    set_graphic_mode(GRAPHICS_DAVID_GERVAIS);
    ui_redraw_all();

}

void MainWindow::set_old_tiles()
{
    set_graphic_mode(GRAPHICS_ORIGINAL);
    ui_redraw_all();
}

void MainWindow::set_ascii()
{
    set_graphic_mode(GRAPHICS_NONE);
    ui_redraw_all();
}

void MainWindow::set_pseudo_ascii()
{
    do_pseudo_ascii = pseudo_ascii_act->isChecked();
    ui_redraw_all();
    update_sidebar_all();
}

void MainWindow::display_monster_info()
{
    display_monster_knowledge();
}

void MainWindow::display_object_info()
{
    display_object_knowledge();
}

void MainWindow::display_ego_item_info()
{
    display_ego_item_knowledge();
}

void MainWindow::display_artifact_info()
{
    display_artifact_knowledge();
}

void MainWindow::display_terrain_info()
{
    display_terrain_knowledge();
}

void MainWindow::display_notes()
{
    display_notes_file();
}

void MainWindow::display_messages()
{
    display_message_log();
}

void MainWindow::display_home()
{
    display_home_inventory();
}

void MainWindow::display_scores()
{
    display_player_scores();
}

void MainWindow::display_kill_count()
{
    display_mon_kill_count();
}


//  Set's up many of the keystrokes and commands used during the game.
void MainWindow::create_signals()
{
    // currently empty
}

void MainWindow::slot_multiplier_clicked(QAction *action)
{
    if (action) current_multiplier = action->objectName();
    QList<QString> parts = current_multiplier.split(":");
    if (parts.size() == 2)
    {
        qreal x = parts.at(1).toFloat();
        qreal y = parts.at(0).toFloat();
        graphics_view->setTransform(QTransform::fromScale(x, y));
    }
}


//Actually add the QActions intialized in create_actions to the menu
void MainWindow::create_menus()
{
    //File section of top menu.
    file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction(new_game_nppangband);
    file_menu->addAction(new_game_nppmoria);
    file_menu->addAction(open_savefile);
    separator_act = file_menu->addSeparator();
    file_menu->addAction(save_cur_char);
    file_menu->addAction(save_cur_char_as);
    file_menu->addAction(close_cur_char);
    separator_act = file_menu->addSeparator();
    file_menu->addAction(exit_npp);
    separator_act = file_menu->addSeparator();    
    for (int i = 0; i < MAX_RECENT_SAVEFILES; ++i)
        file_menu->addAction(recent_savefile_actions[i]);
    separator_act = file_menu->addSeparator();

    update_recent_savefiles();

    menuBar()->addSeparator();

    settings = menuBar()->addMenu(tr("&Settings"));
    settings->addAction(options_act);

    QMenu *choose_fonts = settings->addMenu("Choose Fonts");
    choose_fonts->addAction(font_main_select_act);
    choose_fonts->addAction(font_messages_select_act);
    choose_fonts->addAction(font_sidebar_select_act);

    QMenu *choose_keymap = settings->addMenu("Choose Keyset");
    choose_keymap->addAction(keymap_new);
    choose_keymap->addAction(keymap_angband);
    choose_keymap->addAction(keymap_rogue);
    keymap_new->setCheckable(TRUE);
    keymap_angband->setCheckable(TRUE);
    keymap_rogue->setCheckable(TRUE);
    keymap_new->setChecked(TRUE);

    menuBar()->addSeparator();

    //Tileset options
    QMenu *choose_tile_set = settings->addMenu("Choose Tile Set");
    choose_tile_set->addAction(ascii_mode_act);
    choose_tile_set->addAction(dvg_mode_act);
    choose_tile_set->addAction(old_tiles_act);
    settings->addAction(pseudo_ascii_act);
    tiles_choice = new QActionGroup(this);
    tiles_choice->addAction(ascii_mode_act);
    tiles_choice->addAction(dvg_mode_act);
    tiles_choice->addAction(old_tiles_act);
    ascii_mode_act->setCheckable(TRUE);
    ascii_mode_act->setChecked(TRUE);
    dvg_mode_act->setCheckable(TRUE);
    old_tiles_act->setCheckable(TRUE);

    QMenu *submenu = settings->addMenu(tr("Tile multiplier"));
    multipliers = new QActionGroup(this);
    QString items[] = {
      QString("1:1"),
      QString("1.25:1.25"),
      QString("1.5:1.5"),
      QString("1.75:1.75"),
      QString("2:1"),
      QString("2:2"),
      QString("2.5:2.5"),
      QString("3:3"),
      QString("4:2"),
      QString("4:4"),
      QString("5:5"),
      QString("6:3"),
      QString("6:6"),
      QString("7:7"),
      QString("8:4"),
      QString("8:8"),
      QString("10:5"),
      QString("10:10"),
      QString("")
    };
    for (int i = 0; !items[i].isEmpty(); i++)
    {
        QAction *act = submenu->addAction(items[i]);
        act->setObjectName(items[i]);
        act->setCheckable(true);
        multipliers->addAction(act);
        if (i == 0) act->setChecked(true);
    }
    connect(multipliers, SIGNAL(triggered(QAction*)), this, SLOT(slot_multiplier_clicked(QAction*)));


    QAction *act = settings->addAction(tr("Create tile package"));
    connect(act, SIGNAL(triggered()), this, SLOT(do_create_package()));

    act = settings->addAction(tr("Extract tiles from package"));
    connect(act, SIGNAL(triggered()), this, SLOT(do_extract_from_package()));

    // Knowledge section of top menu.
    knowledge = menuBar()->addMenu(tr("&Knowledge"));
    knowledge->addAction(view_monster_knowledge);
    knowledge->addAction(view_object_knowledge);
    knowledge->addAction(view_ego_item_knowledge);
    knowledge->addAction(view_artifact_knowledge);
    knowledge->addAction(view_terrain_knowledge);
    knowledge->addAction(view_notes);
    knowledge->addAction(view_messages);
    knowledge->addAction(view_home_inven);
    knowledge->addAction(view_scores);
    knowledge->addAction(view_kill_count);


    win_menu = menuBar()->addMenu(tr("&Windows"));
    win_menu->addAction(win_mon_list);
    win_menu->addAction(win_obj_list);
    win_menu->addAction(win_mon_recall);
    win_menu->addAction(win_obj_recall);
    win_menu->addAction(win_feat_recall);

    // Help section of top menu.
    help_menu = menuBar()->addMenu(tr("&Help"));
    help_menu->addAction(help_about);
    help_menu->addAction(help_about_Qt);
    help_menu->addAction(help_command_list);
}

// Create the toolbars
void MainWindow::create_toolbars()
{
    file_toolbar = addToolBar(tr("&File"));
    file_toolbar->setObjectName(QString("file_toolbar"));

    file_toolbar->addAction(new_game_nppangband);
    file_toolbar->addAction(new_game_nppmoria);
    file_toolbar->addAction(open_savefile);    
    file_toolbar->addSeparator();
    file_toolbar->addAction(save_cur_char);
    file_toolbar->addAction(save_cur_char_as);
    file_toolbar->addAction(close_cur_char);
    file_toolbar->addAction(options_act);
    file_toolbar->addSeparator();
    file_toolbar->addAction(exit_npp);

    create_targetbar();
    create_statusbar();
}

// Just find an initial font to start the game
// User preferences will be saved with the game.
void MainWindow::select_font()
{
    bool have_font = FALSE;

    foreach (QString family, font_database.families())
    {
        if (font_database.isFixedPitch(family))
        {
            font_database.addApplicationFont(family);
            if (have_font) continue;
            font_main_window = QFont(family);
            font_message_window = QFont(family);
            font_sidebar_window = QFont(family);
            font_win_mon_list = QFont(family);
            font_win_obj_list = QFont(family);
            font_win_mon_recall = QFont(family);
            font_win_obj_recall = QFont(family);
            font_win_feat_recall = QFont(family);
            have_font = TRUE;
        }
    }

    font_main_window.setPointSize(12);
    font_message_window.setPointSize(12);
    font_sidebar_window.setPointSize(12);
    font_win_mon_list.setPointSize(12);
    font_win_obj_list.setPointSize(12);
    font_win_mon_recall.setPointSize(12);
    font_win_obj_recall.setPointSize(12);
    font_win_feat_recall.setPointSize(12);
}



// Read and write the game settings.
// Every entry in write-settings should ahve a corresponding entry in read_settings.
void MainWindow::read_settings()
{
    QSettings settings("NPPGames", "NPPQT");

    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
    recent_savefiles = settings.value("recentFiles").toStringList();
    do_pseudo_ascii = settings.value("pseudo_ascii", false).toBool();
    pseudo_ascii_act->setChecked(do_pseudo_ascii);
    use_graphics = settings.value("use_graphics", 0).toInt();
    which_keyset = settings.value("which_keyset", 0).toInt();
    current_multiplier = settings.value("tile_multiplier", "1:1").toString();
    QAction *act = this->findChild<QAction *>(current_multiplier);
    if (act)
    {
        act->setChecked(true);
        slot_multiplier_clicked(act);
    }

    QString load_font = settings.value("font_window_main", font_main_window ).toString();
    font_main_window.fromString(load_font);
    load_font = settings.value("font_window_messages", font_message_window ).toString();
    font_message_window.fromString(load_font);
    load_font = settings.value("font_window_sidebar", font_sidebar_window ).toString();
    font_sidebar_window.fromString(load_font);
    load_font = settings.value("font_window_mon_list", font_win_mon_list ).toString();
    font_win_mon_list.fromString(load_font);
    load_font = settings.value("font_window_obj_list", font_win_obj_list ).toString();
    font_win_obj_list.fromString(load_font);
    load_font = settings.value("font_window_mon_recall", font_win_mon_recall ).toString();
    font_win_mon_recall.fromString(load_font);
    load_font = settings.value("font_window_obj_recall", font_win_obj_recall ).toString();
    font_win_obj_recall.fromString(load_font);
    load_font = settings.value("font_window_feat_recall", font_win_feat_recall ).toString();
    font_win_feat_recall.fromString(load_font);
    restoreState(settings.value("window_state").toByteArray());

    show_mon_list = settings.value("show_mon_list_window", false).toBool();
    if (show_mon_list)
    {
        show_mon_list = FALSE; //hack - so it gets toggled to true
        toggle_win_mon_list();
        window_mon_list->restoreGeometry(settings.value("winMonListGeometry").toByteArray());
        window_mon_list->show();
    }

    show_obj_list = settings.value("show_obj_list_window", false).toBool();
    if (show_obj_list)
    {
        show_obj_list = FALSE; //hack - so it gets toggled to true
        toggle_win_obj_list();
        window_obj_list->restoreGeometry(settings.value("winObjListGeometry").toByteArray());
        window_obj_list->show();
    }
    show_mon_recall = settings.value("show_mon_recall_window", false).toBool();
    if (show_mon_recall)
    {
        show_mon_recall = FALSE; //hack - so it gets toggled to true
        toggle_win_mon_recall();
        window_mon_recall->restoreGeometry(settings.value("winMonRecallGeometry").toByteArray());
        window_mon_recall->show();
    }
    show_obj_recall = settings.value("show_obj_recall_window", false).toBool();
    if (show_obj_recall)
    {
        show_obj_recall = FALSE; //hack - so it gets toggled to true
        toggle_win_obj_recall();
        window_obj_recall->restoreGeometry(settings.value("winObjRecallGeometry").toByteArray());
        window_obj_recall->show();
    }
    show_feat_recall = settings.value("show_feat_recall_window", false).toBool();
    if (show_feat_recall)
    {
        show_feat_recall = FALSE; //hack - so it gets toggled to true
        toggle_win_feat_recall();
        window_feat_recall->restoreGeometry(settings.value("winFeatRecallGeometry").toByteArray());
        window_feat_recall->show();
    }

    update_recent_savefiles();
}

void MainWindow::write_settings()
{
    QSettings settings("NPPGames", "NPPQT");
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("recentFiles", recent_savefiles);
    settings.setValue("font_window_main", font_main_window.toString());
    settings.setValue("font_window_messages", font_message_window.toString());
    settings.setValue("font_window_sidebar", font_sidebar_window.toString());
    settings.setValue("font_window_mon_list", font_win_mon_list.toString());
    settings.setValue("font_window_obj_list", font_win_obj_list.toString());
    settings.setValue("font_window_mon_recall", font_win_mon_recall.toString());
    settings.setValue("font_window_obj_recall", font_win_obj_recall.toString());
    settings.setValue("font_window_feat_recall", font_win_feat_recall.toString());
    settings.setValue("window_state", saveState());
    settings.setValue("pseudo_ascii", do_pseudo_ascii);
    settings.setValue("use_graphics", use_graphics);
    settings.setValue("which_keyset", which_keyset);
    settings.setValue("tile_multiplier", current_multiplier);
    settings.setValue("show_mon_list_window", show_mon_list);
    if (show_mon_list)
    {
        settings.setValue("winMonListGeometry", window_mon_list->saveGeometry());
    }
    settings.setValue("show_obj_list_window", show_obj_list);
    if (show_obj_list)
    {
        settings.setValue("winObjListGeometry", window_obj_list->saveGeometry());
    }
    settings.setValue("show_mon_recall_window", show_mon_recall);
    if (show_mon_recall)
    {
        settings.setValue("winMonRecallGeometry", window_mon_recall->saveGeometry());
    }
    settings.setValue("show_obj_recall_window", show_obj_recall);
    if (show_obj_recall)
    {
        settings.setValue("winObjRecallGeometry", window_obj_recall->saveGeometry());
    }
    settings.setValue("show_feat_recall_window", show_feat_recall);
    if (show_feat_recall)
    {
        settings.setValue("winFeatRecallGeometry", window_feat_recall->saveGeometry());
    }
}


void MainWindow::load_file(const QString &file_name)
{    
    if (!file_name.isEmpty())
    {
        set_current_savefile(file_name);

        //make sure we have a valid game_mode
        game_mode = GAME_MODE_UNDEFINED;
        load_gamemode();
        if (game_mode == GAME_MODE_UNDEFINED) return;

        // Initialize game then load savefile
        if (game_mode == GAME_NPPANGBAND) setup_nppangband();
        else if (game_mode == GAME_NPPMORIA) setup_nppmoria();

        if (load_player())
        {
            //update_file_menu_game_active();
            statusBar()->showMessage(tr("File loaded"), 2000);

            if (!character_loaded) message_list.clear();
            update_messages();

            if (!character_loaded)
            {
                save_prev_character();
                launch_birth(TRUE);
            }
            else
            {
                update_file_menu_game_active();
                launch_game();
                ui_player_moved();
                graphics_view->setFocus();
                redraw();
                update_sidebar_font();
            }
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Recent Files"), tr("Cannot read file %1").arg(file_name));
        return;
    }
}

void MainWindow::launch_birth(bool quick_start)
{
    PlayerBirth *dlg = new PlayerBirth(quick_start);

    if (p_ptr->game_turn)
    {
        update_file_menu_game_active();
        launch_game();
        save_character();
        ui_player_moved();
        graphics_view->setFocus();
        redraw();
        update_sidebar_font();

        // The main purpose of this greeting is to avoid crashes
        // due to the message vector being empty.
        message(QString("Welcome %1") .arg(op_ptr->full_name));
    }
    else
    {
        cleanup_npp_games();
        character_loaded = false;
        current_savefile.clear();
    }
    delete dlg;


}

void MainWindow::save_file(const QString &file_name)
{
    set_current_savefile(file_name);

    if (!save_player())
    {
        QMessageBox::warning(this, tr("Recent Files"), tr("Cannot write file %1").arg(file_name));
        return;
    }

    statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::set_current_savefile(const QString &file_name)
{
    current_savefile = file_name;
    setWindowModified(false);

    QString shownName = "Untitled";
    if (!current_savefile.isEmpty()) {
        shownName = stripped_name(current_savefile);
        recent_savefiles.removeAll(current_savefile);
        recent_savefiles.prepend(current_savefile);
        update_recent_savefiles();
    }
}

// Update the 5 most recently played savefile list.
void MainWindow::update_recent_savefiles()
{
    QMutableStringListIterator i(recent_savefiles);
    while (i.hasNext()) {
        if (!QFile::exists(i.next()))
            i.remove();
    }

    for (int j = 0; j < MAX_RECENT_SAVEFILES; ++j)
    {
        if (j < recent_savefiles.count())
        {
            QString text = tr("&%1 %2")
                           .arg(j + 1)
                           .arg(stripped_name(recent_savefiles[j]));
            recent_savefile_actions[j]->setText(text);
            recent_savefile_actions[j]->setData(recent_savefiles[j]);
            recent_savefile_actions[j]->setVisible(true);
        }
        else
        {
            recent_savefile_actions[j]->setVisible(false);
        }
    }
    separator_act->setVisible(!recent_savefiles.isEmpty());
}

QString MainWindow::stripped_name(const QString &full_file_name)
{
    return QFileInfo(full_file_name).fileName();
}

