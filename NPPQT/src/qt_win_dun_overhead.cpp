/*
 * Copyright (c) 2015 Jeff Greene, Diego Gonzalez
 *
 * Please see copyright.txt for complete copyright and licensing restrictions.
 *
 */

#include <src/npp.h>
#include <src/qt_mainwindow.h>
#include <QFontDialog>
#include <src/emitter.h>
#include <QtCore/qmath.h>


// The map width and height is half that of the regular dungeon.
// Note every dungeon cell reference must be careful to reference the right place,
// As the corresponding dungeon grids are (oh_y*2-oh_y*2+1, oh_x*2-oh_x*2+1).
// This fuction makes the dungeon grid, and maps it to x*2, y*2.
DunOverheadGrid::DunOverheadGrid(int _x, int _y)
{
    oh_x = _x*2;
    oh_y = _y*2;
    setZValue(0);
}


// This function must be changed carefully as it has half of the dungeon squares of the other two maps.
// It looks at a block of 2x2 squares and displays the one with the highest priority
void DunOverheadGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;

    if (!character_dungeon) return;

    painter->fillRect(QRectF(0, 0, main_window->overhead_map_cell_wid, main_window->overhead_map_cell_hgt), Qt::black);

    // Now figure out which square has the highest priority
    int dungeon_grid_y = oh_y;
    int dungeon_grid_x = oh_x;

    // Figure out which square of the 2x2 area has the highest priority
    // We are not checking for in bounds here because dungeon_width and hight must be even numbers
    byte max_priority = dungeon_info[oh_y][oh_x].priority;
    if (dungeon_info[oh_y+1][oh_x].priority > max_priority)
    {
        max_priority = dungeon_info[oh_y+1][oh_x].priority;
        dungeon_grid_y = oh_y+1;
    }
    if (dungeon_info[oh_y][oh_x+1].priority > max_priority)
    {
        max_priority = dungeon_info[oh_y][oh_x+1].priority;
        dungeon_grid_x = oh_x+1;
    }
    if (dungeon_info[oh_y+1][oh_x+1].priority > max_priority)
    {
        max_priority = dungeon_info[oh_y+1][oh_x+1].priority;
        dungeon_grid_x = oh_x+1;
        dungeon_grid_y = oh_y+1;
    }

    dungeon_type *d_ptr = &dungeon_info[dungeon_grid_y][dungeon_grid_x];
    QChar square_char = d_ptr->dun_char;
    QColor square_color = d_ptr->dun_color;
    bool empty = true;
    u16b flags = 0;
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

        if (!main_window->do_pseudo_ascii) key2 = d_ptr->monster_tile;
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

    if (empty && !d_ptr->has_visible_terrain()) return;

    bool done_bg = false;
    bool done_fg = false;

    painter->save();

    if (use_graphics && main_window->overhead_map_use_graphics)
    {
        // Draw background tile
        QString key1 = d_ptr->dun_tile;

        if (key1.length() > 0)
        {
            QPixmap pix = main_window->get_tile(key1, main_window->dun_map_cell_hgt, main_window->dun_map_cell_wid);

            if (flags & UI_LIGHT_TORCH)
            {
                QColor color = QColor("yellow").darker(150);
                if (flags & UI_COSMIC_TORCH) color = QColor("cyan").darker(150);
                pix = colorize_pix(pix, color);
            }
            else if (flags & UI_LIGHT_BRIGHT)
            {
                pix = main_window->apply_shade(key1, pix, "bright");
            }
            else if (flags & UI_LIGHT_DIM)
            {
                pix = main_window->apply_shade(key1, pix, "dim");
            }

            painter->drawPixmap(pix.rect(), pix, pix.rect());
            done_bg = true;

            // Draw cloud effects (in graphics mode), if not already drawing that
            if (!is_cloud)
            {
                QString tile = find_cloud_tile(dungeon_grid_y, dungeon_grid_x);
                if (!tile.isEmpty())
                {
                    painter->setOpacity(0.7);
                    QPixmap pix = main_window->get_tile(tile, main_window->overhead_map_cell_hgt, main_window->overhead_map_cell_wid);
                    painter->drawPixmap(0, 0, pix);
                    painter->setOpacity(1);
                    done_bg = true;
                }
            }

            // Draw foreground tile
            if (key2.length() > 0)
            {
               QPixmap pix = main_window->get_tile(key2, main_window->overhead_map_cell_hgt, main_window->overhead_map_cell_wid);
               if (flags & (UI_TRANSPARENT_EFFECT | UI_TRANSPARENT_MONSTER))
               {
                   painter->setOpacity(opacity);
               }
               painter->drawPixmap(pix.rect(), pix, pix.rect());
               painter->setOpacity(1);
               done_fg = true;
            }
            // draw foreground circle for dtrap edge
            else if (d_ptr->dtrap)
            {
                QPixmap sample = main_window->get_tile(key1, main_window->overhead_map_cell_hgt, main_window->overhead_map_cell_wid);
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
                QPixmap pix = pseudo_ascii(square_char, square_color, main_window->font_overhead_map,
                                           QSizeF(main_window->overhead_map_cell_wid, main_window->overhead_map_cell_hgt));
                painter->drawPixmap(pix.rect(), pix, pix.rect());
                done_fg = true;
            }
        }
    }

    // Go ascii?
    if (!done_fg && (!empty || !done_bg))
    {
        painter->setFont(main_window->font_overhead_map);
        painter->setPen(square_color);
        painter->drawText(QRectF(0, 0, main_window->overhead_map_cell_wid, main_window->overhead_map_cell_hgt),
                          Qt::AlignCenter, QString(square_char));
    }

    painter->restore();
}

QPainterPath DunOverheadGrid::shape() const
{
    QPainterPath p;
    p.addRect(boundingRect());
    return p;
}

QRectF DunOverheadGrid::boundingRect() const
{
    return QRectF(0, 0, main_window->overhead_map_cell_wid, main_window->overhead_map_cell_hgt);
}

void DunOverheadGrid::DunMapCellSizeChanged()
{
    prepareGeometryChange();
}


QRect MainWindow::visible_overhead_map()
{
    QGraphicsView *view = overhead_map_view;
    QRectF rect1 = view->mapToScene(view->viewport()->geometry()).boundingRect();
    QRect rect2(floor(rect1.x() / overhead_map_cell_wid),
                floor(rect1.y() / overhead_map_cell_hgt),
                ceil(rect1.width() / overhead_map_cell_wid),
                ceil(rect1.height() / overhead_map_cell_hgt));
    QRect rect3(0, 0, p_ptr->cur_map_wid/2, p_ptr->cur_map_hgt/2);
    rect2 = rect2.intersected(rect3);
    return rect2;
}

void MainWindow::set_overhead_map_font(QFont newFont)
{
    font_overhead_map = newFont;
    overhead_map_calc_cell_size();
}

void MainWindow::win_overhead_map_font()
{
    bool selected;
    QFont font = QFontDialog::getFont( &selected, font_overhead_map, this);

    if (selected)
    {
        set_overhead_map_font(font);
    }
}

void MainWindow::set_overhead_map_graphics()
{
    if (!overhead_map_created) return;
    overhead_map_use_graphics = overhead_map_graphics->isChecked();

    if ((!overhead_map_use_graphics) || (use_graphics == GRAPHICS_NONE))
    {
        overhead_map_tile_hgt = overhead_map_tile_wid = 0;
    }

    else switch (use_graphics)
    {
        case GRAPHICS_RAYMOND_GAUSTADNES:
        {
            overhead_map_tile_hgt = overhead_map_tile_wid = 64;
            break;
        }
        case GRAPHICS_DAVID_GERVAIS:
        {
            overhead_map_tile_hgt = overhead_map_tile_wid = 32;
            break;
        }
        case GRAPHICS_ORIGINAL:
        {
            overhead_map_tile_hgt = overhead_map_tile_wid = 8;
            break;
        }
        default:
        {
            overhead_map_tile_hgt = overhead_map_tile_wid = 0;
            break;
        }
    }

    overhead_map_calc_cell_size();
}

// This function receives the normal dungeon coordinates and converts them to this smaller dungeon.
void MainWindow::overhead_map_center(int y, int x)
{
    if (!overhead_map_created) return;
    y /= 2;
    x /= 2;
    overhead_map_view->centerOn(x * overhead_map_cell_wid, y * overhead_map_cell_hgt);
}


void MainWindow::overhead_map_calc_cell_size()
{
    if (!overhead_map_created) return;

    QFontMetrics metrics(font_overhead_map);

    overhead_map_font_hgt = metrics.height() + FONT_EXTRA;
    overhead_map_font_wid = metrics.width('M') + FONT_EXTRA;

    overhead_map_cell_wid = MAX(overhead_map_tile_wid, overhead_map_font_wid);

    overhead_map_cell_hgt = MAX(overhead_map_tile_hgt, overhead_map_font_hgt);

    // Tis dungeon map has half as many squares of the actual dungeon
    for (int y = 0; y < MAX_DUNGEON_HGT/2; y++)
    {
        for (int x = 0; x < MAX_DUNGEON_WID/2; x++)
        {
            overhead_map_grids[y][x]->DunMapCellSizeChanged();
            overhead_map_grids[y][x]->setPos(x * overhead_map_cell_wid, y * overhead_map_cell_hgt);
        }
    }
    win_overhead_map_update();
}

void MainWindow::overhead_map_multiplier_clicked(QAction *action)
{
    if (action) overhead_map_multiplier = action->objectName();
    QList<QString> parts = overhead_map_multiplier.split(":");
    if (parts.size() == 2)
    {
        qreal x = parts.at(1).toFloat();
        qreal y = parts.at(0).toFloat();
        overhead_map_view->setTransform(QTransform::fromScale(x, y));
    }
    overhead_map_calc_cell_size();
}



// For when savefiles close but the game doesn't.
void MainWindow::win_overhead_map_wipe()
{
    if (!show_win_overhead_map) return;
    if (!character_generated) return;
    clear_layout(main_vlay_overhead_map);
    overhead_map_created = FALSE;
}

// This function assumes the coordinates have already been cut in half
void MainWindow::overhead_map_update_one_grid(int y, int x)
{
    if (!overhead_map_created) return;
    DunOverheadGrid *dmg_ptr = overhead_map_grids[y][x];
    dmg_ptr->setVisible(TRUE);
    dmg_ptr->update(dmg_ptr->boundingRect());
}

void MainWindow::win_overhead_map_update()
{
    if (!overhead_map_created) return;

    // Adjust scrollbars
    overhead_map_view->setSceneRect(0, 0, p_ptr->cur_map_wid/2 * overhead_map_cell_wid, p_ptr->cur_map_hgt/2 * overhead_map_cell_hgt);

    for (int y = 0; y < p_ptr->cur_map_hgt/2; y++)
    {
        for (int x = 0; x < p_ptr->cur_map_wid/2; x++)
        {
            overhead_map_update_one_grid(y, x);
        }
    }
    overhead_map_center(p_ptr->py, p_ptr->px);
}


void MainWindow::create_win_overhead_map()
{
    if (!character_generated) return;
    if (!show_win_overhead_map) return;
    overhead_map_scene = new QGraphicsScene;
    overhead_map_view = new QGraphicsView(overhead_map_scene);
    main_vlay_overhead_map->addWidget(overhead_map_view);


    QBrush brush(QColor("black"));
    overhead_map_scene->setBackgroundBrush(brush);

    for (int y = 0; y < MAX_DUNGEON_HGT/2; y++)
    {
        for (int x = 0; x < MAX_DUNGEON_WID/2; x++)
        {
            overhead_map_grids[y][x] = new DunOverheadGrid(x, y);
            overhead_map_scene->addItem(overhead_map_grids[y][x]);
        }
    }

    overhead_map_created = TRUE;

    QAction *act = window_overhead_map->findChild<QAction *>(overhead_map_multiplier);
    if (act)
    {
        overhead_map_multiplier_clicked(act);
    }
    set_overhead_map_graphics();
    overhead_map_calc_cell_size();
}

void MainWindow::close_win_overhead_map_frame(QObject *this_object)
{
    (void)this_object;
    window_overhead_map = NULL;
    show_win_overhead_map = FALSE;
    win_overhead_map->setText("Show Overhead Window");
    overhead_map_created = FALSE;
}

/*
 *  Make the small_map shell
 */
void MainWindow::win_overhead_map_create()
{
    window_overhead_map = new QWidget();
    main_vlay_overhead_map = new QVBoxLayout;
    window_overhead_map->setLayout(main_vlay_overhead_map);

    win_overhead_map_menubar = new QMenuBar;
    main_vlay_overhead_map->setMenuBar(win_overhead_map_menubar);
    window_overhead_map->setWindowTitle("Overhead Map Window");
    win_overhead_map_settings = win_overhead_map_menubar->addMenu(tr("&Settings"));
    overhead_map_font = new QAction(tr("Set Overhead Map Font"), this);
    overhead_map_font->setStatusTip(tr("Set the font for the Overhead Map Screen."));
    connect(overhead_map_font, SIGNAL(triggered()), this, SLOT(win_overhead_map_font()));
    win_overhead_map_settings->addAction(overhead_map_font);
    overhead_map_graphics = new QAction(tr("Use Graphics"), this);
    overhead_map_graphics->setCheckable(true);
    overhead_map_graphics->setChecked(overhead_map_use_graphics);
    overhead_map_graphics->setStatusTip(tr("If the main window is using graphics, use them in the Overhead Map Window."));
    connect(overhead_map_graphics, SIGNAL(changed()), this, SLOT(set_overhead_map_graphics()));
    win_overhead_map_settings->addAction(overhead_map_graphics);
    QMenu *overhead_map_submenu = win_overhead_map_settings->addMenu(tr("Tile multiplier"));
    overhead_map_multipliers = new QActionGroup(this);

    for (int i = 0; !mult_list[i].isEmpty(); i++)
    {
        QPointer<QAction> act = overhead_map_submenu->addAction(mult_list[i]);
        act->setObjectName(mult_list[i]);
        act->setCheckable(true);
        overhead_map_multipliers->addAction(act);
    }
    connect(overhead_map_multipliers, SIGNAL(triggered(QAction*)), this, SLOT(overhead_map_multiplier_clicked(QAction*)));

    QAction *act = window_overhead_map->findChild<QAction *>(overhead_map_multiplier);
    if (act)
    {
        act->setChecked(true);
    }

    window_overhead_map->setAttribute(Qt::WA_DeleteOnClose);
    connect(window_overhead_map, SIGNAL(destroyed(QObject*)), this, SLOT(close_win_overhead_map_frame(QObject*)));
}

void MainWindow::win_overhead_map_destroy()
{
    if (!show_win_overhead_map) return;
    if (!window_overhead_map) return;
    delete window_overhead_map;
    overhead_map_created = FALSE;
    window_overhead_map = NULL;
}

void MainWindow::toggle_win_overhead_map_frame()
{
    if (!show_win_overhead_map)
    {
        win_overhead_map_create();
        show_win_overhead_map = TRUE;
        create_win_overhead_map();
        win_overhead_map->setText("Hide Overhead Window");
        window_overhead_map->show();
    }
    else

    {
        win_overhead_map_destroy();
        show_win_overhead_map = FALSE;
        win_overhead_map->setText("Show Overhead Window");
    }
}

