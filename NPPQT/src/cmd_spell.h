#ifndef CMD_SPELL_H
#define CMD_SPELL_H


#include <QtWidgets>
#include "src/npp.h"

enum
{
    COL_SPELL_TITLE = 0,
    COL_LEVEL,
    COL_MANA,
    COL_FAIL_PCT,
    COL_INFO,
    COL_HELP
};

// Add the headers


class QTabWidget;



class SpellSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpellSelectDialog(int *spell, QString prompt, int mode, bool *success, bool *cancelled);


protected:
    void keyPressEvent(QKeyEvent* which_key);

private slots:
    // Receives the number of the button pressed.
    void button_press(QString num_string);
    void help_press(QString num_string);
    void on_dialog_buttons_pressed(QAbstractButton *);

private:

    QTabWidget *spell_dialog;
    QDialogButtonBox *buttons;
    QWidget *spell_tab;
    QLabel *main_prompt;

    // Keeps track of which button goes with which object.
    // Is sent by a signal to the button_press function
    QSignalMapper* button_values;
    QSignalMapper* help_values;



    // Functions to build the actual tabs
    void build_spellbook_dialog(int mode);

    QString format_button_name(QChar char_index, object_type *o_ptr, byte which_tab, int slot);

    //Functions to track the list of possible items
    void count_spells(int mode);
    void clear_spells(void);
    bool spell_okay(int spell, bool known);
    QString get_spell_comment(int spell);

    // Calculate the chance of a spell being sucessfully cast
    int spell_chance(int spell);
    int spell_failure_min_moria(int stat);

    //Record an item selection
    int  get_selected_spell(int chosen_button);

    // Variables for keeping track of which item is selected
    int selected_button;
    int num_buttons;
    bool spell_found;

    bool available_spells[BOOKS_PER_REALM_ANGBAND][SPELLS_PER_BOOK];
    bool available_books[BOOKS_PER_REALM_ANGBAND];

    int num_spells;
    int num_available_spellbooks;
    int max_spellbooks;

};



#endif // CMD_SPELL_H
