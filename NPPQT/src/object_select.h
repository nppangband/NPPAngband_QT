#ifndef OBJECT_SELECT_H
#define OBJECT_SELECT_H

#include "src/object_dialog.h"
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QSignalMapper>
#include <QKeyEvent>


// The numbered order of the tabs
enum
{
    TAB_FLOOR = 0,
    TAB_INVEN,
    TAB_EQUIP,
    TAB_QUIVER
};

class QTabWidget;



class ObjectSelectDialog : public ObjectDialog
{
    Q_OBJECT

public:
    explicit ObjectSelectDialog(int *item, QString prompt, int mode, bool *success, bool *cancelled, int sq_y, int sq_x);


protected:
    void keyPressEvent(QKeyEvent* which_key);

private slots:
    // Receives the number of the button pressed.
    void button_press(QString num_string);
    void on_dialog_buttons_pressed(QAbstractButton *);

private:

    QTabWidget *object_tabs;
    QDialogButtonBox *buttons;
    QWidget *floor_tab;
    QWidget *inven_tab;
    QWidget *equip_tab;
    QWidget *quiver_tab;
    QLabel *main_prompt;

    // Keeps track of which button goes with which object.
    // Is sent by a signal to the button_press function
    QSignalMapper* button_values;

    // Functions to build the actual tabs
    void build_floor_tab();
    void build_inven_tab();
    void build_equip_tab();
    void build_quiver_tab();

    byte find_starting_tab(int mode);

    void track_longest_object_name(object_type *o_ptr, byte which_tab, int slot);
    QString add_equip_use(int slot);
    QString format_button_name(QChar char_index, object_type *o_ptr, byte which_tab, int slot);

    //Functions to track the list of possible items
    void floor_items_count(int mode, int sq_y, int sq_x);
    void inven_items_count(int mode);
    void equip_items_count(int mode);
    void quiver_items_count(int mode);


    //Vectors to store the index numbers of the actual objects
    QVector<int> floor_items;
    QVector<int> inven_items;
    QVector<int> equip_items;
    QVector<int> quiver_items;

    bool allow_floor;
    bool allow_inven;
    bool allow_equip;
    bool allow_quiver;

    //Record an item selection
    int  get_selected_object(int num_tracker);

    // Variables for keeping track of which item is selected
    QVector<byte> tab_order;
    int selected_button;
    int num_buttons;
    bool object_found;

};


#endif // OBJECT_SELECT_H
