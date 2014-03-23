#ifndef STOREDIALOG_H
#define STOREDIALOG_H

#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include "nppdialog.h"

enum {
    SMODE_DEFAULT = 0,
    SMODE_BUY,
    SMODE_SELL,
    SMODE_INFO,
    SMODE_EXAMINE
};

class object_type;

class StoreDialog : public NPPDialog
{
    Q_OBJECT
public:
    int store_idx;
    int mode;
    QWidget *central;
    QTabWidget *char_tabs;
    QWidget *inven_tab;
    QWidget *equip_tab;
    QWidget *store_area;

    QLabel *mode_label;

    StoreDialog(int _store, QWidget *parent = 0);

    void reset_store();
    void reset_inventory();
    void reset_equip();

    void set_mode(int _mode);

    virtual void keyPressEvent(QKeyEvent *event);

    bool do_buy(object_type *o_ptr);
    bool do_sell(object_type *o_ptr);

    int request_amt(object_type *o_ptr, bool buying);

public slots:
    void toggle_inven();
    void item_click();
    void sell_click();
    void buy_click();
    void info_click();
    void exam_click();
};

extern void launch_store(int store_idx);

#endif // STOREDIALOG_H
