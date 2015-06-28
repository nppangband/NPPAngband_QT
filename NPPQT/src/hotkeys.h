#ifndef HOTKEYS_H
#define HOTKEYS_H

#include <src/npp.h>
#include <QVector>
#include <QObject>
#include <QScrollArea>

enum
{
    HK_TYPE_MOVE = 0,
    HK_TYPE_JUMP,
    HK_TYPE_RUN,
    HK_TYPE_END,
};

typedef struct hotkey_list hotkey_list;
struct hotkey_list
{
    QString hotkey_name;
    int hotkey;
};

typedef struct hotkey_step hotkey_step;
struct hotkey_step
{
    QString hotkey_name;
    byte hotkey_type;
    int hotkey;
    byte spell;
    u16b object_idx;
    cmd_arg hk_args;
};

typedef struct hotkey_type hotkey_type;
struct hotkey_type
{
    void (*hotkey_function)(cmd_arg args);
    bool needs_direction;
    bool needs_target;
    bool needs_spell;
    bool needs_object;
    QString name;

};

class HotKeyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HotKeyDialog(void);

private:
    QWidget *top_widget;
    QScrollArea *scroll_box;
};

extern void do_hotkey_manage();
extern void do_hotkey_export();
extern void do_hotkey_import();

extern QVector<hotkey_step> single_hotkey;

#endif // HOTKEYS_H
