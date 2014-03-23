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

public slots:
    void toggle_inven();
    void item_click();
    void sell_click();
    void buy_click();
};

#endif // STOREDIALOG_H
