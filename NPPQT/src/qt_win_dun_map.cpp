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
#include <QFontDialog>
#include <src/emitter.h>
#include <QtCore/qmath.h>


QRect MainWindow::visible_dun_map()
{
    QGraphicsView *view = dun_map_view;
    QRectF rect1 = view->mapToScene(view->viewport()->geometry()).boundingRect();
    QRect rect2(floor(rect1.x() / cell_wid),
                floor(rect1.y() / cell_hgt),
                ceil(rect1.width() / cell_wid),
                ceil(rect1.height() / cell_hgt));
    QRect rect3(0, 0, p_ptr->cur_map_wid, p_ptr->cur_map_hgt);
    rect2 = rect2.intersected(rect3);
    return rect2;
}

void MainWindow::dun_map_center(int y, int x)
{
    dun_map_view->centerOn(x * cell_wid, y * cell_hgt);
}


void MainWindow::dun_map_calc_cell_size()
{
    if (!dun_map_created) return;

    for (int y = 0; y < MAX_DUNGEON_HGT; y++)
    {
        for (int x = 0; x < MAX_DUNGEON_WID; x++)
        {
            dun_map_grids[y][x]->cellSizeChanged();
            dun_map_grids[y][x]->setPos(x * cell_wid, y * cell_hgt);
        }
    }
    win_dun_map_update();
}

void MainWindow::dun_map_multiplier_clicked(QAction *action)
{
    if (action) dun_map_multiplier = action->objectName();
    QList<QString> parts = dun_map_multiplier.split(":");
    if (parts.size() == 2)
    {
        qreal x = parts.at(1).toFloat();
        qreal y = parts.at(0).toFloat();
        dun_map_view->setTransform(QTransform::fromScale(x, y));
    }
    dun_map_calc_cell_size();
}



// For when savefiles close but the game doesn't.
void MainWindow::win_dun_map_wipe()
{
    if (!show_win_dun_map) return;
    if (!character_generated) return;
    clear_layout(main_vlay_dun_map);
    dun_map_created = FALSE;
}

void MainWindow::dun_map_update_one_grid(int y, int x)
{
    if (!dun_map_created) return;
    DungeonGrid *dmg_ptr = main_window->dun_map_grids[y][x];
    dmg_ptr->setVisible(TRUE);
    dmg_ptr->update(dmg_ptr->boundingRect());
}

void MainWindow::win_dun_map_update()
{
    if (!dun_map_created) return;

    // Adjust scrollbars
    dun_map_view->setSceneRect(0, 0, p_ptr->cur_map_wid * cell_wid, p_ptr->cur_map_hgt * cell_hgt);

    for (int y = 0; y < p_ptr->cur_map_hgt; y++)
    {
        for (int x = 0; x < p_ptr->cur_map_wid; x++)
        {
            dun_map_update_one_grid(y, x);
        }
    }
    dun_map_center(p_ptr->py, p_ptr->px);
}


void MainWindow::create_win_dun_map()
{
    if (!character_generated) return;
    if (!show_win_dun_map) return;
    dun_map_scene = new QGraphicsScene;
    dun_map_view = new QGraphicsView(dungeon_scene);
    main_vlay_dun_map->addWidget(dun_map_view);


    QBrush brush(QColor("black"));
    dun_map_scene->setBackgroundBrush(brush);

    for (int y = 0; y < MAX_DUNGEON_HGT; y++)
    {
        for (int x = 0; x < MAX_DUNGEON_WID; x++)
        {
            dun_map_grids[y][x] = new DungeonGrid(x, y, this);
            dun_map_scene->addItem(dun_map_grids[y][x]);
        }
    }

    dun_map_created = TRUE;

    QAction *act = window_dun_map->findChild<QAction *>(dun_map_multiplier);
    if (act)
    {
        dun_map_multiplier_clicked(act);
    }

    dun_map_calc_cell_size();
}

/*
 *  Make the small_map shell
 */
void MainWindow::win_dun_map_create()
{
    window_dun_map = new QWidget();
    main_vlay_dun_map = new QVBoxLayout;
    window_dun_map->setLayout(main_vlay_dun_map);

    win_dun_map_menubar = new QMenuBar;
    main_vlay_dun_map->setMenuBar(win_dun_map_menubar);
    window_dun_map->setWindowTitle("Dungeon Map Window");
    win_dun_map_settings = win_dun_map_menubar->addMenu(tr("&Settings"));
    QMenu *dun_map_submenu = win_dun_map_settings->addMenu(tr("Tile multiplier"));
    dun_map_multipliers = new QActionGroup(this);

    for (int i = 0; !items[i].isEmpty(); i++)
    {
        QAction *act = dun_map_submenu->addAction(items[i]);
        act->setObjectName(items[i]);
        act->setCheckable(true);
        dun_map_multipliers->addAction(act);
    }
    connect(dun_map_multipliers, SIGNAL(triggered(QAction*)), this, SLOT(dun_map_multiplier_clicked(QAction*)));

    QAction *act = window_dun_map->findChild<QAction *>(dun_map_multiplier);
    if (act)
    {
        act->setChecked(true);
    }

    //Disable the x button from closing the widget
    window_dun_map->setWindowFlags(Qt::WindowTitleHint);
}

void MainWindow::win_dun_map_destroy()
{
    if (!show_win_dun_map) return;
    delete window_dun_map;
    dun_map_created = FALSE;
}

void MainWindow::toggle_win_dun_map_frame()
{
    if (!show_win_dun_map)
    {
        win_dun_map_create();
        show_win_dun_map = TRUE;
        create_win_dun_map();
        win_dun_map->setText("Hide Main Map Window");
        window_dun_map->show();
    }
    else

    {
        win_dun_map_destroy();
        show_win_dun_map = FALSE;
        win_dun_map->setText("Show Main Map Window");
    }
}
