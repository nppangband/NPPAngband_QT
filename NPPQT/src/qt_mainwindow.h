#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QTextEdit>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QDir>
#include <QPainter>
#include <QImage>
#include <QFontDatabase>
#include <QKeyEvent>
#include <QActionGroup>
#include <QEventLoop>
#include <QLineEdit>
#include <QTextEdit>
#include "defines.h"
#include "structures.h"
#include "nppdialog.h"
#include "src/cmds.h"

#define UI_MODE_DEFAULT 0
#define UI_MODE_INPUT 1

#define MAX_RECENT_SAVEFILES    5

class QAction;
class QMenu;
class QGraphicsView;
class QGraphicsScene;
class QGraphicsItem;
class DungeonGrid;
class DungeonCursor;
class QTextEdit;
class QLineEdit;
class QTableWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // The editable part of the main window.
    QGraphicsView *graphics_view;
    QGraphicsScene *dungeon_scene;

    int ui_mode;
    UserInput input;
    QEventLoop ev_loop;

    int anim_depth;
    QEventLoop anim_loop;

    DungeonGrid *grids[MAX_DUNGEON_HGT][MAX_DUNGEON_WID];

    QFont cur_font;
    int font_hgt, font_wid;
    int tile_hgt, tile_wid;
    int cell_hgt, cell_wid;
    QString current_multiplier;
    bool do_pseudo_ascii;

    // Scaled tiles
    QHash<QString,QPixmap> tiles;

    // For light effects
    QHash<QString,QPixmap> shade_cache;

    QList<QGraphicsItem *> path_items;

    DungeonCursor *cursor;

    QTextEdit *message_area;

    QToolBar *target_toolbar;
    QToolBar *status_bar;

    QWidget *sidebar_widget;
    QVBoxLayout *sidebar;
    QTableWidget *sidebar_mon;

    MainWindow();

    void update_messages(void);
    QPoint get_target(u32b flags);
    void init_scene();
    void set_font(QFont newFont);
    void calculate_cell_size();
    void destroy_tiles();
    void set_graphic_mode(int mode);
    void redraw();
    void update_cursor();
    void force_redraw();
    bool panel_contains(int y, int x);    
    bool running_command();
    QPixmap get_tile(QString tile_id);
    QPixmap apply_shade(QString tile_id, QPixmap tile, QString shade_id);
    void wait_animation(int n_animations = 1);
    void animation_done();
    bool check_disturb();
    void create_sidebar();
    void update_sidebar();
    void close_game_death();
    void update_titlebar();
    void create_titlebar();
    void create_statusbar();
    void update_statusbar();
    void create_targetbar();
    void update_targetbar(int toolbar);
    void hide_statusbar();


protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent* which_key);
    bool eventFilter(QObject *obj, QEvent *event);


private slots:

    // Functions called from signals from the top menu.

    void start_game_nppangband();
    void start_game_nppmoria();
    void open_current_savefile();
    void save_character();
    void save_character_as();
    void save_and_close();
    void open_recent_file();
    void about();
    void command_list();
    void options_dialog();
    void fontselect_dialog();

    void slot_find_player();
    void slot_redraw();
    void toggle_searching() {do_cmd_toggle_search();}
    void click_study() {do_cmd_study(-1);}

    void do_create_package();
    void do_extract_from_package();

    void slot_targetting_button();

    void slot_multiplier_clicked(QAction *);

    // Functions to make sure the available menu commands are appropriate to the situation.
    //  For example, make the save game command unanavailable when no savefile is open.
    void update_file_menu_game_active();
    void update_file_menu_game_inactive();

    // Graphics
    void set_dvg();
    void set_old_tiles();
    void set_ascii();
    void set_pseudo_ascii();
    void display_monster_info();
    void display_object_info();
    void display_ego_item_info();
    void display_artifact_info();
    void display_terrain_info();
    void display_notes();
    void display_messages();
    void display_home();
    void display_scores();
    void display_kill_count();


private:

    void setup_nppangband();
    void setup_nppmoria();
    void launch_birth(bool quick_start = false);

    // Functions that initialize the file menu of the main window.
    void create_actions();
    void create_menus();
    void create_toolbars();
    void select_font();

    // Set up many of the game commands
    void create_signals();


    // Remember the game settings
    void read_settings();
    void write_settings();    

    //Functions and variables that handle opening and saving files, as well as maintain the
    //  5 most recent savefile list.
    void load_file(const QString &file_name);
    void save_file(const QString &file_name);
    void set_current_savefile(const QString &file_name);
    void update_recent_savefiles();
    QString stripped_name(const QString &full_file_name);
    QStringList recent_savefiles;
    QAction *recent_savefile_actions[MAX_RECENT_SAVEFILES];



    //  Holds the actual commands for the file menu and toolbar.
    QMenu *file_menu;
    QMenu *recent_files_menu;
    QMenu *settings;
    QMenu *knowledge;
    QMenu *help_menu;
    QToolBar *file_toolbar;
    QAction *new_game_nppangband;
    QAction *new_game_nppmoria;
    QAction *open_savefile;
    QAction *save_cur_char;
    QAction *save_cur_char_as;
    QAction *close_cur_char;
    QAction *exit_npp;

    //Command for the settings menu
    QAction *options_act;
    QAction *bigtile_act;
    QAction *ascii_mode_act;
    QAction *dvg_mode_act;
    QAction *old_tiles_act;
    QAction *pseudo_ascii_act;
    QAction *fontselect_act;

    //Commmands for the knowledge menu
    QAction *view_monster_knowledge;
    QAction *view_object_knowledge;
    QAction *view_ego_item_knowledge;
    QAction *view_artifact_knowledge;
    QAction *view_terrain_knowledge;
    QAction *view_notes;
    QAction *view_messages;
    QAction *view_home_inven;
    QAction *view_scores;
    QAction *view_kill_count;


    // Holds the actual commands for the help menu.
    QAction *help_about;
    QAction *help_about_Qt;
    QAction *help_command_list;
    QAction *separator_act;


    // information about the main window
    QFontDatabase font_database;
    bool use_bigtile;

    QActionGroup *multipliers;

    // Actions for the statusbar
    // buttons for status bar
    QAction *recall;
    QAction *resting;
    QAction *repeating;
    QAction *searching;
    QAction *status_cut;
    QAction *status_stun;
    QAction *status_hunger;
    QAction *study;

    QAction *blind;
    QAction *paralyzed;
    QAction *confused;
    QAction *afraid;
    QAction *hallucination;
    QAction *poisoned;
    QAction *protect_evil;
    QAction *invulnerability;
    QAction *hero;
    QAction *berzerk;
    QAction *shield;
    QAction *blessed;
    QAction *see_invisible;
    QAction *infravision;

    QAction *resist_acid;
    QAction *resist_cold;
    QAction *resist_fire;
    QAction *resist_lightning;
    QAction *resist_poison;

    QAction *flying;

    QAction *native_lava;

    QAction *native_oil;
    QAction *native_sand;
    QAction *native_tree;
    QAction *native_water;
    QAction *native_mud;

    QAction *status_speed;

    QAction *elemental_weapon;
    QAction *call_hourns;

    QAction *nativity;
    QAction *status_trap_detect;

    // QActions for the targetbar
    QAction *escape;
    QAction *use_current;
    QAction *target_closest;
    QAction *target_interactive;
    QAction *target_manually;
    QAction *target_player;
    QAction *view_grid_contents;
    QAction *target_help;

};

QPoint to_dungeon_coord(QGraphicsItem *item, QPoint p);

extern MainWindow *main_window;

class PackageDialog: public NPPDialog
{
    Q_OBJECT
public:
    QWidget *central;
    QLineEdit *pak_path;
    QLineEdit *folder_path;
    QString mode;

    PackageDialog(QString _mode);

public slots:
    void find_pak();
    void find_folder();
    void do_accept();
};

#endif
