#include <QtWidgets>
#include <QHash>
#include <QTextStream>
#include <QGraphicsItem>
#include <QGraphicsRectItem>

#include "src/npp.h"
#include "src/qt_mainwindow.h"
#include "src/init.h"
#include "src/optionsdialog.h"
#include "src/birthdialog.h"
#include "emitter.h"
#include "griddialog.h"
#include "package.h"
#include "tilebag.h"

MainWindow *main_window = 0;

void ui_request_size_update(QWidget *widget)
{
    QObjectList lst = widget->children();
    for (int i = 0; i < lst.size(); i++) {
        QObject *obj = lst.at(i);
        if (obj->isWidgetType()) {
            ui_request_size_update((QWidget *)obj);
        }
    }

    widget->updateGeometry();

    if (widget->layout()) {
        widget->layout()->invalidate();
    }
}

QSize ui_estimate_table_size(QTableWidget *table, bool horiz, bool vert, int padding)
{
    QSize final(padding, padding);
    int w = 0, h = 0;

    if (horiz) {
        w += table->verticalHeader()->width();
        for (int i = 0; i < table->columnCount(); i++) {
            if (table->isColumnHidden(i)) continue;
            w += table->columnWidth(i);
        }
    }

    if (vert) {
        h += table->horizontalHeader()->height();
        for (int i = 0; i < table->rowCount(); i++) {
            if (table->isRowHidden(i)) continue;
            h += table->rowHeight(i);
        }
    }

    final += QSize(w, h);
    return final;
}

void ui_resize_to_contents(QWidget *widget)
{
    ui_request_size_update(widget);
    QCoreApplication::processEvents();
    widget->resize(widget->sizeHint());
}

QPoint to_dungeon_coord(QGraphicsItem *item, QPoint p)
{
    p += item->pos().toPoint();
    QPoint p2(p.x() / main_window->cell_wid, p.y() / main_window->cell_hgt);
    return p2;
}

bool ui_draw_path(u16b path_n, u16b *path_g, int y1, int x1, int cur_tar_y, int cur_tar_x)
{
    if (path_n < 1) return false;

    QPen pen(QColor("yellow"));

    for (int i = 0; i < path_n; i++) {
        int y = GRID_Y(path_g[i]);
        int x = GRID_X(path_g[i]);        

        // Don't touch the cursor
        if (y == cur_tar_y && x == cur_tar_x) continue;

        QGraphicsRectItem *item = main_window->dungeon_scene->addRect(
                    x * main_window->cell_wid, y * main_window->cell_hgt,
                    main_window->cell_wid - 1, main_window->cell_hgt - 1, pen, Qt::NoBrush);

        item->setOpacity(1);
        item->setZValue(90);

        main_window->path_items.append(item);
    }

    return true;
}

void ui_destroy_path()
{
    for (int i = 0; i < main_window->path_items.size(); i++) {
        QGraphicsItem *item = main_window->path_items.at(i);
        main_window->dungeon_scene->removeItem(item);
        delete item;
    }
    main_window->path_items.clear();
    main_window->force_redraw();
}

QString rect_to_string(QRect rect)
{
    return QString("%1:%2 (%3x%4)").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
}

QRect visible_dungeon()
{
    QGraphicsView *view = main_window->graphics_view;
    QRectF rect1 = view->mapToScene(view->viewport()->geometry()).boundingRect();
    QRect rect2((int)rect1.x() / main_window->cell_wid,
                (int)rect1.y() / main_window->cell_hgt,
                (int)rect1.width() / main_window->cell_wid,
                (int)rect1.height() / main_window->cell_hgt);
    QRect rect3(0, 0, p_ptr->cur_map_wid, p_ptr->cur_map_hgt);
    rect2 = rect2.intersected(rect3);
    return rect2;
}

class DungeonGrid: public QGraphicsItem
{
public:
    DungeonGrid(int _x, int _y, MainWindow *_parent);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    QPainterPath shape() const;

    void cellSizeChanged();

    MainWindow *parent;
    int c_x, c_y;
};

class DungeonCursor: public QGraphicsItem
{    
public:
    MainWindow *parent;
    int c_x, c_y;

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    QPainterPath shape() const;

    DungeonCursor(MainWindow *_parent);
    void moveTo(int y, int x);

    void cellSizeChanged();
};

UserInput ui_get_input()
{
    // Avoid reentrant calls
    if (main_window->ev_loop.isRunning()) {
        UserInput temp;
        temp.mode = INPUT_MODE_NONE;
        temp.key = 0;
        temp.x = temp.y = -1;
        temp.text.clear();
        return temp;
    }

    main_window->ui_mode = UI_MODE_INPUT;

    main_window->input.mode = INPUT_MODE_NONE;

    main_window->cursor->update();

    main_window->ev_loop.exec();

    main_window->ui_mode = UI_MODE_DEFAULT;

    main_window->cursor->update();

    if (main_window->input.mode == INPUT_MODE_KEY) {
        main_window->input.x = main_window->input.y = -1;
    }
    else {
        main_window->input.key = 0;
        main_window->input.text.clear();
    }

    return main_window->input;
}

void ui_player_moved()
{
    if (!character_dungeon) return;
    main_window->update_cursor();

    int py = p_ptr->py;
    int px = p_ptr->px;

    if (center_player && !p_ptr->running) {
        ui_center(py, px);
        return;
    }

    QRect vis = visible_dungeon();
    if (py < vis.y() + panel_change_offset_y
            || py >= vis.y() + vis.height() - panel_change_offset_y
            || px < vis.x() + panel_change_offset_x
            || px >= vis.x() + vis.width() - panel_change_offset_x) {
        ui_center(py, px);
    }
}

QSize ui_grid_size()
{
    return QSize(main_window->cell_wid, main_window->cell_hgt);
}


QPixmap colorize_pix3(QPixmap src, QColor color)
{
    QImage img = src.toImage();
    QPainter p(&img);
    p.setCompositionMode(QPainter::CompositionMode_Overlay);
    p.fillRect(img.rect(), color);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.drawPixmap(0, 0, src);
    return QPixmap::fromImage(img);
}

QPixmap rotate_pix(QPixmap src, qreal angle)
{
    QImage img(src.width(), src.height(), QImage::Format_ARGB32);
    for (int x = 0; x < src.width(); x++) {
        for (int y = 0; y < src.height(); y++) {
            img.setPixel(x, y, QColor(0, 0, 0, 0).rgba());
        }
    }
    QPainter p(&img);
    QTransform tra;
    tra.translate(src.width() / 2, src.height() / 2);
    tra.rotate(-angle);
    tra.translate(-src.width() / 2, -src.height() / 2);
    p.setTransform(tra);
    p.drawPixmap(QPointF(0, 0), src);
    return QPixmap::fromImage(img);
}

QPixmap ui_get_tile(QString tile_id)
{
    // Build a transparent 1x1 pixmap
    if (tile_id.isEmpty() || !use_graphics) {
        return ui_make_blank();
    }

    main_window->rebuild_tile(tile_id);
    return main_window->tiles.value(tile_id);
}

void MainWindow::slot_something()
{
    int dir;
    p_ptr->command_dir = 0;
    graphics_view->setFocus();
    if (!get_aim_dir(&dir, false) || dir == 0) return;
    int k = rand_int(8);

    k = 3;

    if (k == 0) fire_arc(GF_DISENCHANT, dir, 300, 0, 45);
    else if (k == 1) fire_bolt(GF_DISENCHANT, dir, 300);
    else if (k == 2) fire_beam(GF_DISENCHANT, dir, 300, 0);
    else if (k == 3) fire_ball(GF_DISENCHANT, dir, 300, 2);
    else if (k == 4) fire_star(GF_DISENCHANT, 300, 4, 0);
    else if (k == 5) fire_bolt_beam_special(GF_ARROW, dir, 300, 0, PROJECT_AMMO);
    else if (k == 6) fire_bolt_beam_special(GF_ARROW, dir, 300, 0, PROJECT_SHOT);
    else if (k == 7) fire_bolt_beam_special(GF_ARROW, dir, 300, 0, PROJECT_ROCK);
}

void ui_animate_ball(int y, int x, int radius, int type, u32b flg)
{
    BallAnimation *ball = new BallAnimation(QPointF(x, y), radius, type, flg);
    main_window->dungeon_scene->addItem(ball);
    ball->start();
    main_window->wait_animation();
}

void ui_animate_arc(int y0, int x0, int y1, int x1, int type, int radius, int degrees, u32b flg)
{
    ArcAnimation *arc = new ArcAnimation(QPointF(x0, y0), QPointF(x1, y1), degrees, type, radius, flg);
    main_window->dungeon_scene->addItem(arc);
    arc->start();
    main_window->wait_animation();
}

void ui_animate_beam(int y0, int x0, int y1, int x1, int type)
{
    BeamAnimation *beam = new BeamAnimation(QPointF(x0, y0), QPointF(x1, y1), type);
    main_window->dungeon_scene->addItem(beam);
    beam->start();
    main_window->wait_animation();
}

void ui_animate_bolt(int y0, int x0, int y1, int x1, int type, u32b flg)
{
    BoltAnimation *bolt = new BoltAnimation(QPointF(x0, y0), QPointF(x1, y1), type, flg);
    main_window->dungeon_scene->addItem(bolt);
    bolt->start();
    main_window->wait_animation();
}

void ui_animate_throw(int y0, int x0, int y1, int x1, object_type *o_ptr)
{
    BoltAnimation *bolt = new BoltAnimation(QPointF(x0, y0), QPointF(x1, y1), 0, 0, o_ptr);
    main_window->dungeon_scene->addItem(bolt);
    bolt->start();
    main_window->wait_animation();
}

void ui_animate_star(int y, int x, int radius, int type, int gy[], int gx[], int grids)
{
    StarAnimation *star = new StarAnimation(QPointF(x, y), radius, type, gy, gx, grids);
    main_window->dungeon_scene->addItem(star);
    star->start();
    main_window->wait_animation();
}

void MainWindow::slot_find_player()
{
    if (!character_dungeon) return;

    ui_center(p_ptr->py, p_ptr->px);
    update_cursor();
}

QPixmap ui_make_blank()
{
    QImage img(1, 1, QImage::Format_ARGB32);
    img.setPixel(0, 0, qRgba(0, 0, 0, 0));
    return QPixmap::fromImage(img);
}

void ui_animate_victory(int y, int x)
{
    u32b flg = PROJECT_PASS;

    BallAnimation *b1 = new BallAnimation(QPointF(x, y), 3, GF_TIME, flg);
    main_window->dungeon_scene->addItem(b1);
    b1->setZValue(1000);

    HaloAnimation *h1 = new HaloAnimation(y, x);
    main_window->dungeon_scene->addItem(h1);
    h1->setZValue(900);

    b1->start();
    h1->start();

    main_window->wait_animation(2);
}

void MainWindow::slot_redraw()
{
    //redraw();

    ui_animate_victory(p_ptr->py, p_ptr->px);
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
        //qDebug("Quitting %d", anim_depth);
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

QFont ui_current_font()
{
    return main_window->cur_font;
}

void DungeonGrid::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!character_dungeon) return;

    if (parent->anim_depth > 0) return;

    int old_x = parent->cursor->c_x;
    int old_y = parent->cursor->c_y;
    parent->grids[old_y][old_x]->update();
    if (parent->ui_mode == UI_MODE_INPUT) {
        parent->input.x = c_x;
        parent->input.y = c_y;
        parent->input.mode = INPUT_MODE_MOUSE;
        parent->ui_mode = UI_MODE_DEFAULT;
        parent->ev_loop.quit();
    }
    else if (!parent->ev_loop.isRunning()) {
        parent->cursor->setVisible(true);
        parent->cursor->moveTo(c_y, c_x);
        //ui_center(c_y, c_x);

        GridDialog dlg(c_y, c_x);
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

    if (use_graphics) {
        // Draw background tile
        QString key1 = d_ptr->dun_tile;

        if (key1.length() > 0) {
            parent->rebuild_tile(key1); // Grab tile from the cache
            QPixmap pix = parent->tiles[key1];

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
                    parent->rebuild_tile(tile);
                    QPixmap pix = parent->tiles.value(tile);
                    painter->drawPixmap(0, 0, pix);
                    painter->setOpacity(1);
                    done_bg = true;
                }
            }

            // Draw foreground tile
            if (key2.length() > 0) {
               parent->rebuild_tile(key2); // Grab tile from the cache
               QPixmap pix = parent->tiles.value(key2);
               if (flags & (UI_TRANSPARENT_EFFECT | UI_TRANSPARENT_MONSTER)) {
                   painter->setOpacity(opacity);
               }               
               painter->drawPixmap(pix.rect(), pix, pix.rect());
               painter->setOpacity(1);
               done_fg = true;
            }

            if (do_shadow) {
                QPixmap pix = pseudo_ascii(square_char, square_color, parent->cur_font,
                                           QSizeF(parent->cell_wid, parent->cell_hgt));
                painter->drawPixmap(pix.rect(), pix, pix.rect());
                done_fg = true;
            }
        }
    }

    // Go ascii?
    if (!done_fg && (!empty || !done_bg)) {
        painter->setFont(parent->cur_font);
        painter->setPen(square_color);
        painter->drawText(QRectF(0, 0, parent->cell_wid, parent->cell_hgt),
                          Qt::AlignCenter, QString(square_char));
    }

    // Show a red line over a monster with its remaining hp
    if (d_ptr->has_visible_monster() && d_ptr->monster_idx > 0) {
        monster_type *m_ptr = mon_list + d_ptr->monster_idx;
        if (m_ptr->maxhp > 0 && m_ptr->hp < m_ptr->maxhp) {
            int w = parent->cell_wid * m_ptr->hp / m_ptr->maxhp;
            w = MAX(w, 1);
            int h = 1;
            if (parent->cell_hgt > 16) h = 2;
            painter->fillRect(0, 0, w, h, QColor("red"));
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

    QPixmap pix;

    if (shade_cache.find(tile_id, &pix)) return pix;

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

QPixmap gray_pix(QPixmap src)
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

QPixmap colorize_pix(QPixmap src, QColor color)
{
    QImage img = src.toImage();
    QPainter p(&img);
    p.setCompositionMode(QPainter::CompositionMode_HardLight);
    p.fillRect(img.rect(), color);
    QPixmap pix = QPixmap::fromImage(img);
    return pix;
}

QPixmap colorize_pix2(QPixmap src, QColor color)
{
    QImage img(src.width(), src.height(), QImage::Format_ARGB32);
    QPainter p(&img);
    p.fillRect(img.rect(), color);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.drawPixmap(QPoint(0, 0), src);
    QPixmap pix = QPixmap::fromImage(img);
    return pix;
}

QPixmap darken_pix(QPixmap src)
{
    QImage img = src.toImage();
    QPainter p(&img);
    p.setCompositionMode(QPainter::CompositionMode_HardLight);
    p.fillRect(img.rect(), QColor("#444"));
    QPixmap pix = QPixmap::fromImage(img);
    return pix;    
}

QPixmap lighten_pix(QPixmap src)
{
    QImage img = src.toImage();
    QPainter p(&img);
    p.setCompositionMode(QPainter::CompositionMode_HardLight);
    p.fillRect(img.rect(), QColor("#999"));
    QPixmap pix = QPixmap::fromImage(img);
    return pix;
}

void MainWindow::destroy_tiles()
{
    tiles.clear();
    shade_cache.clear();
}

void MainWindow::calculate_cell_size()
{
    cell_hgt = font_hgt;
    if (tile_hgt > cell_hgt) cell_hgt = tile_hgt;

    cell_wid = font_wid;
    if (tile_wid > cell_wid) cell_wid = tile_wid;

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
    int hgt, wid;
    QString fname;
    int cy = -1;
    int cx = -1;

    // Remember the center of the view
    if (character_dungeon) {
        QRect vis = visible_dungeon();
        cy = vis.y() + vis.height() / 2;
        cx = vis.x() + vis.width() / 2;
    }

    switch (mode) {
    case GRAPHICS_DAVID_GERVAIS:
        hgt = 32;
        wid = 32;
        fname.append("32x32.png");
        break;    
    case GRAPHICS_ORIGINAL:
        hgt = 8;
        wid = 8;
        fname.append("8x8.png");
        break;
    default:
        hgt = 0;
        wid = 0;
        break;
    }

    if (fname.length() > 0) {
        fname.prepend(NPP_DIR_GRAF);
        QPixmap pix = QPixmap(fname);
        if (pix.isNull()) {
            pop_up_message_box(QString("Can't load tiles"));
            return;
        }
        use_graphics = mode;
        tile_hgt = hgt;
        tile_wid = wid;
        if (mode == GRAPHICS_ORIGINAL) {
            /*
            font.setPointSize(8);
            QFontMetrics metrics(font);
            font_hgt = metrics.height();
            font_wid = metrics.width('M');
            */
        }
        calculate_cell_size();
        destroy_tiles();
        tile_map = pix;
        init_graphics();
    }
    // Go to text mode
    else {
        use_graphics = mode;
        tile_hgt = hgt;
        tile_wid = wid;
        calculate_cell_size();
        destroy_tiles();
        tile_map = blank_pix;
        clear_graphics();           
    }    

    // Recenter the view
    if (cy != -1 && cx != -1) {
        ui_redraw_all();
        ui_center(cy, cx);
    }
}

// Tile creation on demmand
void MainWindow::rebuild_tile(QString key)
{
    // Already created
    if (tiles.contains(key)) return;

    QList<QString> coords = key.split("x");
    if (coords.size() != 2) return;
    int x = coords.at(1).toInt();
    int y = coords.at(0).toInt();
    // Grab a chunk from the tile map
    QPixmap pix = tile_map.copy(x * tile_wid, y * tile_hgt, tile_wid, tile_hgt);
    // Scale if necessary
    if ((cell_wid != tile_wid) || (cell_hgt != tile_hgt)) {
        pix = pix.scaled(QSize(cell_wid, cell_hgt));
    }
    tiles.insert(key, pix);
}

void MainWindow::set_font(QFont newFont)
{
    cur_font = newFont;
    QFontMetrics metrics(cur_font);
    font_hgt = metrics.height();
    font_wid = metrics.width('M');

    calculate_cell_size();

    destroy_tiles();
}

void MainWindow::init_scene()
{
    QFontMetrics metrics(cur_font);

    font_hgt = metrics.height();
    font_wid = metrics.width('M');
    tile_hgt = tile_wid = 0;
    cell_hgt = cell_wid = 0;

    QBrush brush(QColor("black"));
    dungeon_scene->setBackgroundBrush(brush);

    blank_pix = ui_make_blank();

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
    wiz_light();

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

    setAttribute(Qt::WA_DeleteOnClose);

    ui_mode = UI_MODE_DEFAULT;

    cursor = new DungeonCursor(this);
    do_pseudo_ascii = false;

    current_multiplier = "1:1";

    dungeon_scene = new QGraphicsScene;
    graphics_view = new QGraphicsView(dungeon_scene);
    graphics_view->installEventFilter(this);

    QWidget *central = new QWidget;
    setCentralWidget(central);

    QVBoxLayout *lay1 = new QVBoxLayout;
    central->setLayout(lay1);

    message_area = new QTextEdit;
    message_area->setReadOnly(true);
    message_area->setMaximumHeight(80);
    message_area->setStyleSheet("background-color: black;");
    lay1->addWidget(message_area);

    lay1->addWidget(graphics_view);

    QHBoxLayout *lay2 = new QHBoxLayout;
    lay1->addLayout(lay2);

    QPushButton *b1 = new QPushButton("Find player");
    lay2->addWidget(b1);
    connect(b1, SIGNAL(clicked()), this, SLOT(slot_find_player()));

    QPushButton *b2 = new QPushButton("Redraw");
    lay2->addWidget(b2);
    connect(b2, SIGNAL(clicked()), this, SLOT(slot_redraw()));

    /*
    QPushButton *b3 = new QPushButton("Zoom out");
    lay2->addWidget(b3);
    connect(b3, SIGNAL(clicked()), this, SLOT(slot_zoom_out()));

    QPushButton *b4 = new QPushButton("Zoom in");
    lay2->addWidget(b4);
    connect(b4, SIGNAL(clicked()), this, SLOT(slot_zoom_in()));
    */

    QPushButton *b5 = new QPushButton("Test something");
    lay2->addWidget(b5);
    connect(b5, SIGNAL(clicked()), this, SLOT(slot_something()));

    create_actions();
    update_file_menu_game_inactive();
    create_menus();
    create_toolbars();
    select_font();
    create_directories();
    init_tile_bags();
    create_signals();
    (void)statusBar();

    read_settings();
    init_scene();
    set_graphic_mode(use_graphics);

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

    launch_birth();
}

// Prepare to play a game of NPPMoria.
void MainWindow::start_game_nppmoria()
{
    setup_nppmoria();

    launch_birth();
}

void MainWindow::open_current_savefile()
{
    // Let the user select the savefile
    QString file_name = QFileDialog::getOpenFileName(this, tr("Select a savefile"), NPP_DIR_SAVE, tr("NPP (*.npp)"));
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
    QString fileName = QFileDialog::getSaveFileName(this, tr("Select a savefile"), NPP_DIR_SAVE, tr("NPP (*.npp)"));
    if (fileName.isEmpty())
        return;

    save_file(fileName);
}

void MainWindow::load_messages()
{
    message_area->clear();
    for (int i = message_list.size() - 1; i >= 0; i--) {
        message_area->setTextColor(message_list[i].msg_color);
        message_area->insertPlainText(QString("%1: %2\n")
                                      .arg(message_list[i].message_turn)
                                      .arg(message_list[i].message));
    }
    message_area->moveCursor(QTextCursor::End);
}

void ui_show_message(int idx)
{
    // Clear contents if too many lines
    if (main_window->message_area->document()->blockCount() > 1000) {
        main_window->load_messages();
    }
    else if (idx >= 0 && idx < message_list.size()) {
        main_window->message_area->moveCursor(QTextCursor::End);
        main_window->message_area->setTextColor(message_list[idx].msg_color);
        main_window->message_area->insertPlainText(QString("%1: %2\n")
                                      .arg(message_list[idx].message_turn)
                                      .arg(message_list[idx].message));
        main_window->message_area->moveCursor(QTextCursor::End);
    }
}

void MainWindow::save_and_close()
{
    if (running_command()) return;

    save_character();

    set_current_savefile("");

    character_loaded = character_dungeon = character_generated = character_icky = false;

    update_file_menu_game_inactive();

    // close game
    cleanup_npp_games();

    message_area->clear();

    cursor->setVisible(false);
    destroy_tiles();
    redraw();
}

void ui_show_cursor(int y, int x)
{
    if (y < 0 || x < 0) {
        main_window->update_cursor();
    }
    else {
        main_window->cursor->moveTo(y, x);
        main_window->cursor->setVisible(true);
    }
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

static void process_mov_key(QKeyEvent *event, int dir)
{
    int mask = event->modifiers() & (Qt::ShiftModifier | Qt::AltModifier | Qt::ControlModifier);

    if (mask == Qt::ControlModifier) {
        ui_change_panel(dir);
    }
    else {
        move_player(dir, false);
    }
}

void MainWindow::keyPressEvent(QKeyEvent* which_key)
{
    if (anim_depth > 0) return;

    // TODO PLAYTESTING
    debug_rarities();

    QString keystring = which_key->text();

    // Go to special key handling
    if (ui_mode == UI_MODE_INPUT) {
        input.key = which_key->key();
        input.text = keystring;
        input.mode = INPUT_MODE_KEY;
        ui_mode = UI_MODE_DEFAULT;
        ev_loop.quit();
        return;
    }

    // Normal mode
    switch (which_key->key())
    {
        // ESCAPE
        case Qt::Key_Escape:
        {
            ui_center(p_ptr->py, p_ptr->px);
            break;
        }
        // TODO PLAYTESTING
        case Qt::Key_Asterisk:
        {
            // Stone to mud
            if (which_key->modifiers() & Qt::ControlModifier) {
                int dir;
                p_ptr->command_dir = 0;
                if (get_aim_dir(&dir, false)) {
                    wall_to_mud(dir, 100);
                }
            }
            // Shot everything
            else {
                slot_something();
            }
            // Take a turn
            process_player_energy(BASE_ENERGY_MOVE);
            break;
        }
        // TODO PLAYTESTING
        case Qt::Key_J:
        {
            int l;
            bool ok;
            l = QInputDialog::getInt(0, "Please enter a number",
                                     "Jump to level", p_ptr->depth, 0, 101, 1, &ok, 0);
            if (ok) {
                dungeon_change_level(l);
                process_player_energy(BASE_ENERGY_MOVE);
            }
            break;
        }
        // Move down
        case Qt::Key_2:
        case Qt::Key_Down:
        {
            process_mov_key(which_key, 2);
            return;
        }
        // Move up
        case Qt::Key_8:
        case Qt::Key_Up:
        {
            process_mov_key(which_key, 8);
            return;
        }
        // Move left
        case Qt::Key_4:
        case Qt::Key_Left:
        {
            process_mov_key(which_key, 4);;
            return;
        }
        // Move right
        case Qt::Key_6:
        case Qt::Key_Right:
        {
            process_mov_key(which_key, 6);
            return;
        }
        // Move diagonally left and up
        case Qt::Key_7:
        {
            process_mov_key(which_key, 7);
            return;
        }
        // Move diagonally right and up
        case Qt::Key_9:
        {
            process_mov_key(which_key, 9);
            return;
        }
        // Move diagonally left and down
        case Qt::Key_1:
        {
            process_mov_key(which_key, 1);
            return;
        }
        // Move diagonally right and down
        case Qt::Key_3:
        {
            process_mov_key(which_key, 3);
            return;
        }
        case Qt::Key_V:
        {
            if (keystring == "v")   do_cmd_throw();
            return;
        }
        case Qt::Key_F:
        {
            if (keystring == "f")   do_cmd_fire();
            else                    do_cmd_refuel();
            return;
        }
        case Qt::Key_I:
        {
            do_cmd_observe();
            return;
        }
        case Qt::Key_G:
        {
            if (keystring == "g")   do_cmd_pickup();
            else                    do_cmd_study();
            return;
        }
        case Qt::Key_D:
        {            
            if (keystring == "d")   do_cmd_drop();
            else                    do_cmd_destroy();
            return;
        }
        case Qt::Key_BraceLeft:
        {
            do_cmd_inscribe();
            return;
        }
        case Qt::Key_BraceRight:
        {
            do_cmd_uninscribe();
            return;
        }
        case Qt::Key_Greater:
        {
            do_cmd_go_down();
            return;
        }
        case Qt::Key_Less:
        {
            do_cmd_go_up();
            return;
        }

        default:
        {
            // handle lowercase keystrokes        
            if (keystring == "a") do_cmd_activate();
            else if (keystring == "p") do_cmd_cast();
            else if (keystring == "b") do_cmd_browse();
            else if (keystring == "e") do_cmd_use_item();
            else if (keystring == "t") do_cmd_takeoff();
            else if (keystring == "w") do_cmd_wield();
            else if (keystring == "x") do_cmd_swap_weapon();
            else if (keystring == "o") do_cmd_open();
            else if (keystring == "z") describe_monster(644,TRUE,NULL);
            else
            {
                //  TODO something useful with unused keypresses
                //QMessageBox* box = new QMessageBox();
                //box->setWindowTitle(QString("Unused Key %1"));
                //box->setText(QString("You Pressed: ")+ which_key->text());
                //box->show();
            }
        }
    }
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
    if (running_command()) {
        event->ignore();
        return;
    }

    if (!current_savefile.isEmpty() && character_dungeon)
    {
        save_character();
        pop_up_message_box("Game saved");
    }
    write_settings();
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
    OptionsDialog *dlg = new OptionsDialog(this);
    dlg->exec();
    ui_redraw_all();
    delete dlg;
}

void MainWindow::fontselect_dialog()
{
    bool selected;
    QFont font = QFontDialog::getFont( &selected, cur_font, this );

    if (selected)
    {
        set_font(font);
        redraw();
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
    options_act->setIcon(QIcon(":/icons/lib/icons/options.png"));
    connect(options_act, SIGNAL(triggered()), this, SLOT(options_dialog()));

    ascii_mode_act = new QAction(tr("Ascii graphics"), this);
    ascii_mode_act->setStatusTip(tr("Set the graphics to ascii mode."));
    connect(ascii_mode_act, SIGNAL(triggered()), this, SLOT(set_ascii()));

    dvg_mode_act = new QAction(tr("David Gervais tiles"), this);
    dvg_mode_act->setStatusTip(tr("Set the graphics to David Gervais tiles mode."));
    connect(dvg_mode_act, SIGNAL(triggered()), this, SLOT(set_dvg()));

    old_tiles_act = new QAction(tr("Old tiles"), this);
    old_tiles_act->setStatusTip(tr("Set the graphics to old tiles mode."));
    connect(old_tiles_act, SIGNAL(triggered()), this, SLOT(set_old_tiles()));

    pseudo_ascii_act = new QAction(tr("Pseudo-Ascii monsters"), this);
    pseudo_ascii_act->setCheckable(true);
    pseudo_ascii_act->setChecked(false);
    pseudo_ascii_act->setStatusTip(tr("Set the monsters graphics to pseudo-ascii."));
    connect(pseudo_ascii_act, SIGNAL(changed()), this, SLOT(set_pseudo_ascii()));

    bigtile_act = new QAction(tr("Use Bigtile"), this);
    bigtile_act->setCheckable(true);
    bigtile_act->setChecked(use_bigtile);
    bigtile_act->setStatusTip(tr("Doubles the width of each dungeon square."));

    fontselect_act = new QAction(tr("Fonts"), this);
    fontselect_act->setStatusTip(tr("Change the window font or font size."));
    connect(fontselect_act, SIGNAL(triggered()), this, SLOT(fontselect_dialog()));

    about_act = new QAction(tr("&About"), this);
    about_act->setStatusTip(tr("Show the application's About box"));
    connect(about_act, SIGNAL(triggered()), this, SLOT(about()));

    about_Qt_act = new QAction(tr("About &Qt"), this);
    about_Qt_act->setStatusTip(tr("Show the Qt library's About box"));
    connect(about_Qt_act, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
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
    set_graphic_mode(0);
    ui_redraw_all();
}

void MainWindow::set_pseudo_ascii()
{
    do_pseudo_ascii = pseudo_ascii_act->isChecked();
    ui_redraw_all();
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
    if (parts.size() == 2) {
        int x = parts.at(1).toInt();
        int y = parts.at(0).toInt();
        graphics_view->setTransform(QTransform::fromScale(x, y));
    }
}

qreal ui_get_angle(int y1, int x1, int y2, int x2)
{
    QLineF line(ui_get_center(y1, x1), ui_get_center(y2, x2));
    if (line.length() == 0.0) return 0.0;
    return line.angle();
}

QPoint ui_get_center(int y, int x)
{
    x *= main_window->cell_wid;
    y *= main_window->cell_hgt;
    x += main_window->cell_wid / 2;
    y += main_window->cell_hgt / 2;
    return QPoint(x, y);
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
    settings->addAction(fontselect_act);
    settings->addAction(ascii_mode_act);
    settings->addAction(dvg_mode_act);
    settings->addAction(old_tiles_act);
    settings->addAction(pseudo_ascii_act);

    QMenu *submenu = settings->addMenu(tr("Tile multiplier"));
    multipliers = new QActionGroup(this);
    QString items[] = {
      QString("1:1"),
      QString("2:1"),
      QString("2:2"),
      QString("3:1"),
      QString("3:3"),
      QString("4:2"),
      QString("4:4"),
      QString("6:3"),
      QString("6:6"),
      QString("8:4"),
      QString("8:8"),
      QString("16:8"),
      QString("16:16"),
      QString("")
    };
    for (int i = 0; !items[i].isEmpty(); i++) {
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

    // Help section of top menu.
    help_menu = menuBar()->addMenu(tr("&Help"));
    help_menu->addAction(about_act);
    help_menu->addAction(about_Qt_act);
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

    toolbar1 = new QToolBar;
    toolbar1->setObjectName("toolbar1");
    addToolBar(Qt::BottomToolBarArea, toolbar1);
    toolbar1->setVisible(false);

    struct ButtonData {
        QString command;
        int key;
        QString tooltip;
    };

    struct ButtonData buttons[] =
    {
        {"ESCAPE", Qt::Key_Escape, tr("Cancel current targeting mode")},
        {"5", Qt::Key_5, tr("Accept current target")},
        {"c", Qt::Key_C, tr("Target closest")},
        {"*", Qt::Key_Asterisk, tr("Interactive targeting mode")},
        {"o", Qt::Key_O, tr("Manual targeting")},
        {"p", Qt::Key_P, tr("Target player location")},
        {"f", Qt::Key_F, tr("Toggle terrain description")},
        {"", 0, ""}
    };

    for (int i = 0; buttons[i].command.length() > 0; i++) {
        QAction *act = toolbar1->addAction(buttons[i].command);
        act->setObjectName(buttons[i].command);
        act->setProperty("key", QVariant(buttons[i].key));
        act->setStatusTip(buttons[i].tooltip);
        act->setToolTip(buttons[i].tooltip);
        connect(act, SIGNAL(triggered()), this, SLOT(slot_targetting_button()));
    }
}

void ui_toolbar_show(int toolbar)
{
    QToolBar *tb = main_window->toolbar1;
    switch (toolbar)
    {
    case TOOLBAR_TARGETTING:
        tb->findChild<QAction *>("c")->setVisible(true);
        tb->findChild<QAction *>("*")->setVisible(true);
        tb->findChild<QAction *>("o")->setVisible(false);
        tb->findChild<QAction *>("p")->setVisible(false);
        tb->findChild<QAction *>("f")->setVisible(false);
        tb->show();
        break;
    case TOOLBAR_TARGETTING_INTERACTIVE:
        tb->findChild<QAction *>("c")->setVisible(false);
        tb->findChild<QAction *>("*")->setVisible(false);
        tb->findChild<QAction *>("o")->setVisible(true);
        tb->findChild<QAction *>("p")->setVisible(true);
        tb->findChild<QAction *>("f")->setVisible(true);
        tb->show();
        break;
    }
}

void ui_toolbar_hide(int toolbar)
{
    switch (toolbar)
    {
    case TOOLBAR_TARGETTING:
    case TOOLBAR_TARGETTING_INTERACTIVE:
        main_window->toolbar1->hide();
        break;
    }
}

void MainWindow::slot_targetting_button()
{
    if (ui_mode != UI_MODE_INPUT || !ev_loop.isRunning()) return;

    QObject *snd = QObject::sender();
    input.text = snd->objectName();
    input.key = snd->property("key").toInt();
    input.mode = INPUT_MODE_KEY;
    ui_mode = UI_MODE_DEFAULT;
    ev_loop.quit();
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
            cur_font = QFont(family);
            have_font = TRUE;
        }
    }

    cur_font.setPointSize(12);
    //  Figure out - this sets the font for everything setFont(cur_font);
}



// Read and write the game settings.
// Every entry in write-settings should ahve a corresponding entry in read_settings.
void MainWindow::read_settings()
{
    QSettings settings("NPPGames", "NPPQT");

    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
    recent_savefiles = settings.value("recentFiles").toStringList();
    bool bigtile_setting = settings.value("set_bigtile", TRUE).toBool();
    bigtile_act->setChecked((bigtile_setting));
    do_pseudo_ascii = settings.value("pseudo_ascii", false).toBool();
    pseudo_ascii_act->setChecked(do_pseudo_ascii);
    use_graphics = settings.value("use_graphics", 0).toInt();
    current_multiplier = settings.value("tile_multiplier", "1:1").toString();
    QAction *act = this->findChild<QAction *>(current_multiplier);
    if (act) {
        act->setChecked(true);
        slot_multiplier_clicked(act);
    }

    QString load_font = settings.value("current_font", cur_font ).toString();
    cur_font.fromString(load_font);    
    restoreState(settings.value("window_state").toByteArray());

    update_recent_savefiles();
}

void MainWindow::write_settings()
{
    QSettings settings("NPPGames", "NPPQT");

    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("recentFiles", recent_savefiles);
    settings.setValue("set_bigtile", bigtile_act->isChecked());
    settings.setValue("current_font", cur_font.toString());
    settings.setValue("window_state", saveState());
    settings.setValue("pseudo_ascii", do_pseudo_ascii);
    settings.setValue("use_graphics", use_graphics);
    settings.setValue("tile_multiplier", current_multiplier);
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

            load_messages();

            if (!character_loaded) {
                save_prev_character();
                launch_birth(true);
            }
            else {
                update_file_menu_game_active();
                launch_game();
                ui_player_moved();
                graphics_view->setFocus();
                redraw();
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
    BirthDialog *dlg = new BirthDialog(this);    

    dlg->set_quick_start(quick_start);
    if (dlg->run()) {                
        update_file_menu_game_active();
        launch_game();
        save_character();
        ui_player_moved();
        graphics_view->setFocus();
        redraw();
    } else {
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

// determine of a dungeon square is onscreen at present
bool panel_contains(int y, int x)
{
    return main_window->panel_contains(y, x);
}

void ui_ensure(int y, int x)
{
    main_window->graphics_view->ensureVisible(QRectF(x * main_window->cell_wid,
                                                     y * main_window->cell_hgt,
                                                     main_window->cell_wid, main_window->cell_hgt));
}

bool ui_modify_panel(int y, int x)
{
    QRect vis = visible_dungeon();

    if (y < 0) y = 0;
    if (y >= p_ptr->cur_map_hgt) y = p_ptr->cur_map_hgt - 1;

    if (x < 0) x = 0;
    if (x >= p_ptr->cur_map_wid) x = p_ptr->cur_map_wid - 1;

    if (y == vis.y() && x == vis.x()) return false;

    main_window->graphics_view->verticalScrollBar()->setValue(y * main_window->cell_hgt);    
    main_window->graphics_view->horizontalScrollBar()->setValue(x * main_window->cell_wid);

    return true;
}

bool ui_adjust_panel(int y, int x)
{
    QRect vis = visible_dungeon();

    int y2 = vis.y();
    int x2 = vis.x();

    while (y < y2) y2 -= vis.height() / 2;
    while (y >= y2 + vis.height()) y2 += vis.height() / 2;

    while (x < x2) x2 -= vis.width() / 2;
    while (x >= x2 + vis.width()) x2 += vis.width() / 2;

    return ui_modify_panel(y2, x2);
}

bool ui_change_panel(int dir)
{
    QRect vis = visible_dungeon();

    int y = vis.y() + ddy[dir] * vis.height() / 2;
    int x = vis.x() + ddx[dir] * vis.width() / 2;

    return ui_modify_panel(y, x);
}

void ui_center(int y, int x)
{
    main_window->graphics_view->centerOn(x * main_window->cell_wid, y * main_window->cell_hgt);
}

void ui_redraw_grid(int y, int x)
{
    DungeonGrid *g_ptr = main_window->grids[y][x];
    g_ptr->setVisible(true);
    g_ptr->update(g_ptr->boundingRect());
}

void ui_redraw_all()
{
    main_window->redraw();
}
