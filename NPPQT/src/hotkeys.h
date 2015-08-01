#ifndef HOTKEYS_H
#define HOTKEYS_H

#include <src/npp.h>
#include <QLineEdit>
#include <QVector>
#include <QObject>
#include <QScrollArea>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QButtonGroup>


#define NUM_HOTKEYS 12

enum
{
    HK_TYPE_EMPTY = 0,
    HK_TYPE_MOVE,
    HK_TYPE_JUMP,
    HK_TYPE_RUN,
    HK_TYPE_END,
};

typedef struct hotkey_list hotkey_list;
struct hotkey_list
{
    QString hotkey_list_name;
    int listed_hotkey;
};

enum
{
    HK_NEEDS_DIRECTION = 0,
    HK_NEEDS_TARGET,
    HK_NEEDS_SPELL,
    HK_NEEDS_OBJECT,
};

#define HK_VERIFY_YES   true
#define HK_VERIFY_NO   false

typedef struct hotkey_type hotkey_type;
struct hotkey_type
{
    void (*hotkey_function)(cmd_arg args);
    byte hotkey_needs;
    bool use_verify;
    QString name;
};

typedef struct hotkey_step hotkey_step;
struct hotkey_step
{
    byte step_commmand;
    cmd_arg step_args;
};


class single_hotkey
{
public:
    QString hotkey_name;
    QString hotkey_button_name;
    int hotkey_button;
    QVector<hotkey_step> hotkey_steps;

    void copy_hotkey(single_hotkey *other_hotkey);
    void clear_hotkey(void);
    bool has_commands(void);
};

class HotKeyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HotKeyDialog(void);
    single_hotkey dialog_hotkey;
    int active_step;

private:
    QWidget *top_widget;
    QVBoxLayout *top_layout;
    QVBoxLayout *main_layout;
    QVBoxLayout *vlay_hotkey_steps;
    QScrollArea *scroll_box;
    QLineEdit *hotkey_name;

    void add_hotkeys_header();
    QComboBox *current_hotkey_name;
    QLineEdit *current_name;

    void create_one_hotkey_step(QHBoxLayout *this_layout, int step);
    void display_hotkey_steps();
    void create_direction_pad(QHBoxLayout *this_layout, int step);
    QButtonGroup *group_directions;

    int current_hotkey_int;

    void save_current_hotkey();
    void load_new_hotkey(int this_choice);

private slots:

    void active_hotkey_changed(int new_hotkey);
    void active_hotkey_name_changed(QString new_name);
    void active_hotkey_command_changed(int this_choice);
    void active_hotkey_direction_changed(int new_dir);
};

extern void do_hotkey_manage();
extern void wipe_hotkeys();
extern void do_hotkey_export();
extern void do_hotkey_import();
extern void run_hotkey_step();

extern single_hotkey running_hotkey;
extern single_hotkey player_hotkeys[NUM_HOTKEYS];

#endif // HOTKEYS_H
