#ifndef QT_MAINWINDOW_H
#define QT_MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDir>
#include <QPainter>
#include <QImage>
#include <QFontDatabase>
#include <QKeyEvent>
#include <QTableWidget>
#include <QActionGroup>
#include <QEventLoop>
#include <QLineEdit>
#include <QTextEdit>
#include <QMenuBar>
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

    QFont font_main_window;
    QFont font_message_window;
    QFont font_sidebar_window;
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

    QDockWidget *message_dock;
    QDockWidget *sidebar_dock;
    QTextEdit *message_area;

    QToolBar *target_toolbar;
    QToolBar *status_bar;

    QWidget *sidebar_widget;
    QScrollArea *sidebar_scroll;
    QVBoxLayout *sidebar_vlay;
    QVBoxLayout *player_info_vlay;
    QVBoxLayout *mon_health_vlay;

    MainWindow();

    void update_messages(void);
    QPoint get_target(u32b flags);
    void init_scene();
    void set_font_main_window(QFont newFont);
    void set_font_message_window(QFont newFont);
    void set_font_sidebar_window(QFont newFont);
    void calculate_cell_size();
    void destroy_tiles();
    void set_graphic_mode(int mode);
    void set_keymap_mode(int mode);
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
    void update_sidebar_font();
    void update_sidebar_player();
    void update_sidebar_mon();
    void sidebar_display_mon(int m_idx);
    void update_sidebar_all() {update_sidebar_mon(); update_sidebar_player();}
    void hide_sidebar();
    void show_sidebar();
    void close_game_death();
    void update_titlebar();
    void create_titlebar();
    void create_statusbar();
    void update_statusbar();
    void hide_statusbar();
    void show_statusbar();
    void create_targetbar();
    void update_targetbar(int toolbar);



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
    void font_dialog_main_window();
    void font_dialog_message_window();
    void font_dialog_sidebar_window();

    void slot_find_player();
    void slot_redraw();
    void toggle_searching() {do_cmd_toggle_search();}
    void click_study() {do_cmd_study(-1);}

    void do_create_package();
    void do_extract_from_package();

    void slot_targetting_button();

    void slot_simplified_keyset() {set_keymap_mode(KEYSET_NEW);}
    void slot_angband_keyset() {set_keymap_mode(KEYSET_ANGBAND);}
    void slot_rogue_keyset() {set_keymap_mode(KEYSET_ROGUE);}
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
    void launch_birth(bool quick_start);

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
    QMenu *win_menu;
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
    QActionGroup *tiles_choice;
    QAction *ascii_mode_act;
    QAction *dvg_mode_act;
    QAction *old_tiles_act;
    QAction *pseudo_ascii_act;
    QAction *font_main_select_act;
    QAction *font_messages_select_act;
    QAction *font_sidebar_select_act;
    QAction *keymap_new;
    QAction *keymap_angband;
    QAction *keymap_rogue;

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

    // Commands for the additional windows
    QAction *win_mon_list;
    QAction *win_obj_list;
    QAction *win_mon_recall;
    QAction *win_obj_recall;
    QAction *win_feat_recall;
    QAction *win_messages;
    QAction *win_char_basic;

    // Holds the actual commands for the help menu.
    QAction *help_about;
    QAction *help_about_Qt;
    QAction *help_command_list;
    QAction *separator_act;


    // information about the main window
    QFontDatabase font_database;

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

// Monster list window
private:
    bool show_mon_list;
    QWidget *window_mon_list;
    QVBoxLayout *mon_list_vlay;
    QTableWidget *mon_list_area;
    QMenuBar *mon_list_menubar;
    QAction *mon_list_set_font;
    QFont font_win_mon_list;
    QMenu *mon_win_settings;
    void win_mon_list_create();
    void win_mon_list_destroy();
    void win_mon_list_wipe();
    void set_font_win_mon_list(QFont newFont);

public:
    void win_mon_list_update();

private slots:
    void win_mon_list_font();
    void toggle_win_mon_list();

// Object list window
private:
    bool show_obj_list;
    QWidget *window_obj_list;
    QVBoxLayout *obj_list_vlay;
    QTableWidget *obj_list_area;
    QMenuBar *obj_list_menubar;
    QAction *obj_list_set_font;
    QFont font_win_obj_list;
    QMenu *obj_win_settings;
    void win_obj_list_create();
    void win_obj_list_destroy();
    void win_obj_list_wipe();
    void set_font_win_obj_list(QFont newFont);

public:
    void win_obj_list_update();

private slots:
    void win_obj_list_font();
    void toggle_win_obj_list();

// Monster Recall window
private:
    bool show_mon_recall;
    QWidget *window_mon_recall;
    QVBoxLayout *mon_recall_vlay;
    QTextEdit *mon_recall_area;
    QMenuBar *mon_recall_menubar;
    QAction *mon_recall_set_font;
    QFont font_win_mon_recall;
    QMenu *mon_recall_win_settings;
    void win_mon_recall_create();
    void win_mon_recall_destroy();
    void win_mon_recall_wipe();
    void set_font_win_mon_recall(QFont newFont);

public:
    void win_mon_recall_update();

private slots:
    void win_mon_recall_font();
    void toggle_win_mon_recall();

// Object Recall window
private:
    bool show_obj_recall;
    QWidget *window_obj_recall;
    QVBoxLayout *obj_recall_vlay;
    QTextEdit *obj_recall_area;
    QMenuBar *obj_recall_menubar;
    QAction *obj_recall_set_font;
    QFont font_win_obj_recall;
    QMenu *obj_recall_win_settings;
    void win_obj_recall_create();
    void win_obj_recall_destroy();
    void win_obj_recall_wipe();
    void set_font_win_obj_recall(QFont newFont);

public:
    void win_obj_recall_update();

private slots:
    void win_obj_recall_font();
    void toggle_win_obj_recall();

// Feature Recall window
private:
    bool show_feat_recall;
    QWidget *window_feat_recall;
    QVBoxLayout *feat_recall_vlay;
    QTextEdit *feat_recall_area;
    QMenuBar *feat_recall_menubar;
    QAction *feat_recall_set_font;
    QFont font_win_feat_recall;
    QMenu *feat_recall_win_settings;
    void win_feat_recall_create();
    void win_feat_recall_destroy();
    void win_feat_recall_wipe();
    void set_font_win_feat_recall(QFont newFont);

public:
    void win_feat_recall_update();

private slots:
    void win_feat_recall_font();
    void toggle_win_feat_recall();

// Messages window
private:
    bool show_messages_win;
    QWidget *window_messages;
    QVBoxLayout *win_messages_vlay;
    QTextEdit *win_messages_area;
    QMenuBar *win_messages_menubar;
    QAction *win_messages_set_font;
    QFont font_win_messages;
    QMenu *messages_win_settings;
    void win_messages_create();
    void win_messages_destroy();
    void win_messages_wipe();
    void set_font_win_messages(QFont newFont);

public:
    void win_messages_update();

private slots:
    void win_messages_font();
    void toggle_win_messages();


// Character Information window
private:
    bool show_char_info_basic;
    QWidget *window_char_info_basic;
    QVBoxLayout *main_vlay;
    QMenuBar *char_info_basic_menubar;
    QAction *char_info_basic_font;
    QFont font_char_basic_info;
    QMenu *char_info_basic_settings;
    void win_char_info_basic_create();
    void win_char_info_basic_destroy();
    void win_char_info_basic_wipe();
    void set_font_char_info_basic(QFont newFont);
    void name_change_pushbutton(QGridLayout *return_layout);
    void create_win_char_info();

public:
    void win_char_info_basic_update();
    void win_char_info_score();
    void win_char_info_turncount();

private slots:
    void win_char_info_basic_font();
    void toggle_win_char_info_frame();
    void name_change(void);
};

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



extern QVector<s16b> sidebar_monsters;

#endif
