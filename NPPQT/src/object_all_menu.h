#ifndef OBJECT_ALL_MENU_H
#define OBJECT_ALL_MENU_H

// Class to build an interactive dialog
// for all player info.

#include <src/object_dialog.h> //includes npp.h and player_command.h
#include "src/utilities.h"
#include <QLabel>
#include <QScrollArea>
#include <QTabWidget>




// The tab order
enum
{
    TAB_FLOOR = 0,
    TAB_INVEN,
    TAB_EQUIP,
    TABS_MAX
};

class AllObjectsDialog : public QDialog
{
    Q_OBJECT
private:

    void update_header();
    void update_all();

    // Header area
    QLabel *header_main;
    QLabel *header_weight1;
    QLabel *header_weight2;

    // Message area
    QLabel *message_area;

    QWidget *top_widget;
    QTabWidget *object_tabs;
    QWidget *floor_tab;
    QWidget *inven_tab;
    QWidget *equip_tab;

    QScrollArea *scroll_box;
    QScrollArea *scroll_floor;
    QScrollArea *scroll_inven;
    QScrollArea *scroll_equip;

    bool allow_floor;
    bool allow_inven;
    bool allow_equip;
    bool allow_quiver;

    int floor_tab_idx;
    int inven_tab_idx;
    int equip_tab_idx;

    void confirm_tabs();
    void hide_or_show_tabs();
    void update_active_tabs();
    void link_pushbuttons();

    bool no_objects();
    int start_tab;
    int current_tab;

    // Layouts and labels
    QVBoxLayout *floor_vlay;
    QVBoxLayout *inven_vlay;
    QVBoxLayout *equip_and_quiver_vlay;
    QVBoxLayout *equip_vlay;
    QVBoxLayout *quiver_vlay;

    QGridLayout *floor_list;
    QGridLayout *inven_list;
    QGridLayout *equip_list;
    QGridLayout *quiver_list;

    QLabel *header_floor;
    QLabel *header_inven;
    QLabel *header_equip;
    QLabel *header_quiver;
    QLabel *empty_space;

private slots:

    void move_left(void);
    void move_right(void);
    void button_click(void);

protected:
    void keyPressEvent(QKeyEvent* which_key);


public:
    explicit AllObjectsDialog(bool do_buttons, int start_screen);
    void update_dialog();
    void close_dialog();


};

#endif // OBJECT_ALL_MENU_H
