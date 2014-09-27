#ifndef STOREDIALOG_H
#define STOREDIALOG_H

#include <QtWidgets>
#include "src/nppdialog.h"
#include "src/npp.h"
#include "store.h"

enum
{
    SMODE_DEFAULT = 0,
    SMODE_BUY,
    SMODE_SELL,
    SMODE_EXAMINE
};



#define QUEST_REWARD_HEAD	SERVICE_QUEST_DEFER_REWARD
#define QUEST_REWARD_TAIL	SERVICE_QUEST_REWARD_AUGMENTATION

class service_info
{
public:

    byte service_store;
    u32b service_price;
    bool (*service_function)(byte choice, u32b price);
    QString service_names;
};

extern service_info services_info[STORE_SERVICE_MAX];

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
    bool guild;

    QLabel *mode_label;

    StoreDialog(int _store, QWidget *parent = 0);

    s32b price_services(int service_idx);
    bool should_offer_service(byte service_num);


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

    void process_item(QString item_id);
    void process_service(QString item_id);
    void process_quest(QString item_id);

public slots:
    void toggle_inven();
    void item_click();
    void service_click();
    void quest_click();
    void sell_click();
    void buy_click();
    void exam_click();
    void wield_click();
    void takeoff_click();
    void help_click();
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

class StatDialog : public QDialog
{
    Q_OBJECT
public:
    explicit StatDialog(int service, byte *stat_selected);

private:
    QLabel *main_prompt;
    QDialogButtonBox *buttons;

    byte selected_stat;

public slots:
    void select_str(void);
    void select_int(void);
    void select_wis(void);
    void select_dex(void);
    void select_con(void);
    void select_chr(void);

public:
     bool stats[A_MAX];
     bool init_stats_table(int service);
};

extern void launch_store(int store_idx);
extern int launch_stat_dialog(int choice);


#endif // STOREDIALOG_H
