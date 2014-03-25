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
    bool home;

    QLabel *mode_label;

    StoreDialog(int _store, QWidget *parent = 0);

    void reset_store();
    void reset_inventory();
    void reset_equip();

    void reset_all();

    void reset_gold();

    void set_mode(int _mode);

    virtual void keyPressEvent(QKeyEvent *event);

    bool do_buy(object_type *o_ptr, int item);
    bool do_sell(object_type *o_ptr, int item);

    int request_amt(object_type *o_ptr, bool buying);

public slots:
    void toggle_inven();
    void item_click();
    void sell_click();
    void buy_click();
    void exam_click();
    void wield_click();
    void takeoff_click();
};

class QSpinBox;

class QuantityDialog: public QDialog
{
    Q_OBJECT
public:
    QLabel *question;
    QSpinBox *amt_spin;
    QLabel *total_label;
    object_type *o_ptr;
    bool buying;
    int amt;
    int max;
    int price;

    QuantityDialog(object_type *op, bool buy);

public slots:
    void update_totals(int value);
    void do_accept();
};

extern void launch_store(int store_idx);

#endif // STOREDIALOG_H
