#ifndef STOREDIALOG_H
#define STOREDIALOG_H

#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include "src/nppdialog.h"
#include "src/npp.h"

enum
{
    SMODE_DEFAULT = 0,
    SMODE_BUY,
    SMODE_SELL,
    SMODE_EXAMINE
};

enum
{
    SERVICE_ENCHANT_ARMOR	= 0,
    SERVICE_ENCHANT_TO_HIT,
    SERVICE_ENCHANT_TO_DAM,
    SERVICE_ELEM_BRAND_WEAP,
    SERVICE_ELEM_BRAND_AMMO,
    SERVICE_RECHARGING,
    SERVICE_IDENTIFY,
    SERVICE_IDENTIFY_FULLY,
    SERVICE_CURE_CRITICAL,
    SERVICE_RESTORE_LIFE_LEVELS,
    SERVICE_REMOVE_CURSE,
    SERVICE_REMOVE_HEAVY_CURSE,
    SERVICE_RESTORE_STAT,
    SERVICE_INCREASE_STAT,
    SERVICE_CREATE_RANDART,
    SERVICE_PROBE_QUEST_MON,
    SERVICE_BUY_HEALING_POTION,
    SERVICE_BUY_LIFE_POTION,
    SERVICE_BUY_SCROLL_BANISHMENT,
    SERVICE_FIREPROOF_BOOK,
    SERVICE_QUEST_DEFER_REWARD,
    SERVICE_ABANDON_QUEST,
    SERVICE_QUEST_REWARD_RANDART,
    SERVICE_QUEST_REWARD_INC_HP,
    SERVICE_QUEST_REWARD_INC_STAT,
    SERVICE_QUEST_REWARD_AUGMENTATION,

    STORE_SERVICE_MAX
};

#define QUEST_REWARD_HEAD	SERVICE_QUEST_DEFER_REWARD
#define QUEST_REWARD_TAIL	SERVICE_QUEST_REWARD_AUGMENTATION

class service_info
{
public:

    byte service_store;
    u32b service_price;
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

    s16b moria_chr_adj(void);
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

public slots:
    void toggle_inven();
    void item_click();
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

extern void launch_store(int store_idx);

#endif // STOREDIALOG_H
