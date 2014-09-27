#include "storedialog.h"
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QSpacerItem>
#include <QGroupBox>
#include <QSpinBox>
#include <QCoreApplication>
#include <QScrollArea>
#include "npp.h"
#include "store.h"

void launch_store(int store_idx)
{
    StoreDialog *dlg = new StoreDialog(store_idx);
    dlg->exec();
    delete dlg;
    process_player_energy(BASE_ENERGY_MOVE);
}

static void clear_grid(QGridLayout *lay)
{
    QLayoutItem *item;
    while ((item = lay->takeAt(0)) != 0) {
        QWidget *wid = item->widget();
        if (wid) delete wid;
        delete item;
    }
}

StoreDialog::StoreDialog(int _store, QWidget *parent): NPPDialog(parent)
{
    store_idx = _store;
    home = (store_idx == STORE_HOME);
    guild = (store_idx == STORE_GUILD);

    central = new QWidget;
    QVBoxLayout *lay1 = new QVBoxLayout;
    central->setLayout(lay1);
    lay1->setSpacing(10);
    this->setClient(central);  // IMPORTANT: it must be called AFTER setting the layout

    QWidget *area1 = new QWidget;
    lay1->addWidget(area1);
    QHBoxLayout *lay3 = new QHBoxLayout;
    area1->setLayout(lay3);
    lay3->setContentsMargins(0, 0, 0, 0);

    if (guild)
    {
        lay3->addWidget(new QLabel("<b>The Adventurer's Guild</b>"));
    }
    else if (!home)
    {
        owner_type *ot_ptr = &b_info[(store_idx * z_info->b_max) + store[store_idx].owner];
        int feat = dungeon_info[p_ptr->py][p_ptr->px].feat;
        QString shop_name = f_info[feat].f_name;
        QString msg = QString("<b>%1</b> - %2 (%3)").arg(shop_name).arg(ot_ptr->owner_name)
                .arg(ot_ptr->max_cost);
        QLabel *store_info = new QLabel(msg);
        lay3->addWidget(store_info);
    }
    else
    {
        lay3->addWidget(new QLabel("<b>Your home</b>"));
    }

    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    lay3->addItem(spacer);

    QPushButton *btn_buy = new QPushButton(home ? "Retrieve (F2)": "Buy (F2)");
    lay3->addWidget(btn_buy);
    connect(btn_buy, SIGNAL(clicked()), this, SLOT(buy_click()));

    QPushButton *btn_sell = new QPushButton(home ? "Stash (F3)": "Sell (F3)");
    lay3->addWidget(btn_sell);
    connect(btn_sell, SIGNAL(clicked()), this, SLOT(sell_click()));

    QPushButton *btn_toggle = new QPushButton("Toggle inven/equip (F4)");
    lay3->addWidget(btn_toggle);
    connect(btn_toggle, SIGNAL(clicked()), this, SLOT(toggle_inven()));

    QWidget *area4 = new QWidget;
    QHBoxLayout *lay6 = new QHBoxLayout;
    area4->setLayout(lay6);
    lay6->setContentsMargins(0, 0, 0, 0);
    lay1->addWidget(area4);

    QLabel *gold_label = new QLabel();
    gold_label->setObjectName("gold_label");
    gold_label->setStyleSheet("font-weight: bold;");
    lay6->addWidget(gold_label);
    reset_gold();

    mode_label = new QLabel("");
    lay6->addWidget(mode_label);
    mode_label->setStyleSheet("font-weight: bold;");

    set_mode(SMODE_DEFAULT);

    spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    lay6->addItem(spacer);

    QPushButton *btn_exam = new QPushButton("Examine (F5)");
    lay6->addWidget(btn_exam);
    connect(btn_exam, SIGNAL(clicked()), this, SLOT(exam_click()));

    QPushButton *btn_wield = new QPushButton("Wield (F6)");
    lay6->addWidget(btn_wield);
    connect(btn_wield, SIGNAL(clicked()), this, SLOT(wield_click()));

    QPushButton *btn_takeoff = new QPushButton("Take off (F7)");
    lay6->addWidget(btn_takeoff);
    connect(btn_takeoff, SIGNAL(clicked()), this, SLOT(takeoff_click()));

    QWidget *area2 = new QWidget;
    QHBoxLayout *lay2 = new QHBoxLayout;
    area2->setLayout(lay2);
    lay2->setContentsMargins(0, 0, 0, 0);
    lay1->addWidget(area2);

    QGroupBox *box1 = new QGroupBox(home ? "Home items": "Store items");
    lay2->addWidget(box1);
    QVBoxLayout *lay4 = new QVBoxLayout;
    box1->setLayout(lay4);

    store_area = new QWidget;
    lay4->addWidget(store_area);

    char_tabs = new QTabWidget;
    lay2->addWidget(char_tabs);

    inven_tab = new QWidget;
    inven_tab->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    equip_tab = new QWidget;
    equip_tab->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    char_tabs->addTab(inven_tab, tr("Inventory"));
    char_tabs->addTab(equip_tab, tr("Equipment"));
    char_tabs->setCurrentIndex(0);

    this->reset_store();
    this->reset_inventory();
    this->reset_equip();

    QWidget *area3 = new QWidget;
    QHBoxLayout *lay5 = new QHBoxLayout;
    area3->setLayout(lay5);
    lay5->setContentsMargins(0, 0, 0, 0);
    lay1->addWidget(area3);

    spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    lay5->addItem(spacer);

    QPushButton *btn_close = new QPushButton("Close");
    lay5->addWidget(btn_close);
    connect(btn_close, SIGNAL(clicked()), this, SLOT(reject()));

    this->clientSizeUpdated();
}

void StoreDialog::wield_click()
{
    do_cmd_wield();
    reset_all();
}

void StoreDialog::takeoff_click()
{
    do_cmd_takeoff();
    reset_all();
}

void StoreDialog::reset_gold()
{
    QLabel *label = this->findChild<QLabel *>("gold_label");
    label->setText(QString("Gold: %1").arg(p_ptr->au));
}

void StoreDialog::exam_click()
{
    set_mode(SMODE_EXAMINE);
}

void StoreDialog::buy_click()
{
    set_mode(SMODE_BUY);
}

//  Determine if a store should offer a certain service or not.
bool StoreDialog::should_offer_service(byte service_num)
{
    service_info *service_ptr = &services_info[service_num];
    quest_type *q_ptr = &q_info[GUILD_QUEST_SLOT];

    /* Services are store-specific */
    if (service_ptr->service_store != store_idx) return (FALSE);

    /*
     * The guild only offers certain services
     * depending on the active quest.
     */

    /* Offer this service only if there is a quest to abandon. */
    if (service_num == SERVICE_ABANDON_QUEST)
    {
        /*We finished the quest, why abandon it?*/
        if (guild_quest_complete()) return (FALSE);

        /* No current guild quest */
        if (!q_ptr->q_type) return (FALSE);

        if (!guild_quest_level()) return (FALSE);

        return (TRUE);
    }

    if ((service_num >= QUEST_REWARD_HEAD) &&
             (service_num <= QUEST_REWARD_TAIL))
    {
        /* Not currently offering a reward */
        if (!guild_quest_complete()) return (FALSE);

        // Certain services only if these rewards are offered.
        if (service_num == SERVICE_QUEST_REWARD_INC_HP)
        {
            if (!(q_ptr->q_reward & (REWARD_INC_HP))) return (FALSE);
        }
        else if (service_num == SERVICE_QUEST_REWARD_RANDART)
        {
            if (!(q_ptr->q_reward & (REWARD_RANDART))) return (FALSE);
        }
        else if (service_num == SERVICE_QUEST_REWARD_INC_STAT)
        {
            if (!(q_ptr->q_reward & (REWARD_INC_STAT))) return (FALSE);
        }
        else if (service_num == SERVICE_QUEST_REWARD_AUGMENTATION)
        {
            if (!(q_ptr->q_reward & (REWARD_AUGMENTATION))) return (FALSE);
        }

        return (TRUE);
    }

    /* Filter out quest-specific services when appropriate. */
    if (service_num == SERVICE_PROBE_QUEST_MON)
    {
        if (!guild_quest_level()) return (FALSE);
        if (guild_quest_complete()) return (FALSE);

        if (q_ptr->q_type == QUEST_VAULT) return (FALSE);
        if (q_ptr->q_type == QUEST_GREATER_VAULT) return (FALSE);
        if (quest_type_collection(q_ptr)) return (FALSE);
        if (quest_multiple_r_idx(q_ptr)) return (FALSE);
    }

    return (TRUE);
}


s32b StoreDialog::price_services(int service_idx)
{
    service_info *service_ptr = &services_info[service_idx];

    /* get the service price*/
    u32b price = service_ptr->service_price;

    /*adjust price, but not for the guild*/
    if (store_idx != STORE_GUILD)
    {
        /* Extract the "minimum" price */
        if (game_mode == GAME_NPPMORIA)
        {
            price = ((price * moria_chr_adj()) / 100L);
        }
        else price = ((price * adj_chr_gold[p_ptr->state.stat_ind[A_CHR]]) / 100L);
    }

    /*Guild price factoring*/
    else
    {
        if (p_ptr->q_fame < 1000) price += price * (1000 - p_ptr->q_fame) / 1000;
    }

    return(price);
}


void StoreDialog::reset_store()
{
    QGridLayout *lay = dynamic_cast<QGridLayout *>(store_area->layout());
    if (lay == 0)
    {
        lay = new QGridLayout;
        lay->setColumnStretch(1, 1);
        store_area->setLayout(lay);
    }

    // Remove previous items
    clear_grid(lay);
    int row = 0;
    if (!home) lay->addWidget(new QLabel("Price"), row++, 2);
    store_type *st = &store[store_idx];
    int i;

    // Display the services
    for (i = 0; i < STORE_SERVICE_MAX; i++)
    {

        /* Check if the services option is disabled */
        if (adult_no_store_services) break;

        /* Services are store-specific */
        if (!should_offer_service(i)) continue;

        service_info *service_ptr = &services_info[i];

        int col = 0;

        // Make an id for the item
        QString id = QString("s%1").arg(i);

        QLabel *lb = new QLabel(QString("%1)").arg(number_to_letter(i)));
        lb->setProperty("item_id", QVariant(id));
        lay->addWidget(lb, row, col++);

        QString desc = service_ptr->service_names;
        s32b price = price_services(i);
        QColor service_color = make_color_readable(defined_colors[TERM_GREEN]);
        QString s = QString("color: %1;") .arg(service_color.name());
        QString style = "text-align: left; font-weight: bold;";
        style += s;

        if (price <= p_ptr->au)
        {
            QPushButton *btn = new QPushButton(desc);
            btn->setProperty("item_id", QVariant(id));

            btn->setStyleSheet(style);
            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            connect(btn, SIGNAL(clicked()), this, SLOT(service_click()));
            lay->addWidget(btn, row, col++);
        }
        else
        {
            QLabel *lb2 = new QLabel(desc);
            lb2->setStyleSheet(style);
            lay->addWidget(lb2, row, col++);
        }

        QLabel *l = new QLabel(_num(price));
        lay->addWidget(l, row, col++);

        QPushButton *help_button = new QPushButton;
        help_button->setIcon(QIcon(":/icons/lib/icons/help.png"));
        help_button->setObjectName(id);
        connect(help_button, SIGNAL(clicked()), this, SLOT(help_click()));
        lay->addWidget(help_button, row, col++);

        ++row;
    }

    for (i = 0; i < st->stock_num; i++)
    {
        object_type *o_ptr = &st->stock[i];
        if (o_ptr->k_idx == 0) continue;

        int col = 0;

        // Make an id for the item
        QString id = QString("p%1").arg(i);

        QLabel *lb = new QLabel(QString("%1)").arg(number_to_letter(i)));
        lb->setProperty("item_id", QVariant(id));
        lay->addWidget(lb, row, col++);

        QString desc = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);
        s32b price = price_item(store_idx, o_ptr, false);
        QString s = QString("color: %1;").arg(get_object_color(o_ptr).name());
        QString style = "text-align: left; font-weight: bold;";
        style += s;

        if (home || price <= p_ptr->au)
        {
            QPushButton *btn = new QPushButton(desc);
            btn->setProperty("item_id", QVariant(id));

            btn->setStyleSheet(style);
            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            connect(btn, SIGNAL(clicked()), this, SLOT(item_click()));
            lay->addWidget(btn, row, col++);
        }
        else
        {
            QLabel *lb2 = new QLabel(desc);
            lb2->setStyleSheet(style);
            lay->addWidget(lb2, row, col++);
        }

        if (!home)
        {
            QLabel *l = new QLabel(_num(price));
            lay->addWidget(l, row, col++);
        }

        QPushButton *help_button = new QPushButton;
        help_button->setIcon(QIcon(":/icons/lib/icons/help.png"));
        help_button->setObjectName(id);
        connect(help_button, SIGNAL(clicked()), this, SLOT(help_click()));
        lay->addWidget(help_button, row, col++);

        ++row;
    }
    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    lay->addItem(spacer, row++, 0);
}

void StoreDialog::help_click()
{
    QString id = QObject::sender()->objectName();
    int o_idx = id.mid(1).toInt();
    object_type *o_ptr;

    // TODO handle services
    if (id.at(0) == 'q')
    {

    }
    else if (id.at(0) == 's')
    {

    }
    else if (id.at(0) == 'p')
    {
        o_ptr = &(store[store_idx].stock[o_idx]);
        object_info_screen(o_ptr);
    }
    else
    {
        o_ptr = &(inventory[o_idx]);
        object_info_screen(o_ptr);
    }

}

void StoreDialog::set_mode(int _mode)
{
    mode = _mode;
    QString names[] = {
        QString(""), QString(home ? "Retrieving": "Buying"), QString(home ? "Stashing": "Selling"),
        QString("Examining")
    };
    QString text = names[mode];
    if (!text.isEmpty()) text.append(". Click over an item.");
    mode_label->setText(text);
}

void StoreDialog::reset_inventory()
{
    QWidget *tab = inven_tab;
    QGridLayout *lay = dynamic_cast<QGridLayout *>(tab->layout());
    if (lay == 0) {
        lay = new QGridLayout;
        lay->setColumnStretch(1, 1);
        tab->setLayout(lay);
    }
    // Remove previous items
    clear_grid(lay);
    int row = 0;
    if (!home) lay->addWidget(new QLabel("Price"), row++, 2);
    int i;
    for (i = 0; i < INVEN_WIELD - 1; i++)
    {
        object_type *o_ptr = inventory + i;
        if (o_ptr->k_idx == 0) continue;        

        // Make an id for the item
        QString id = QString("i%1").arg(i);

        QLabel *lb = new QLabel(QString("%1)").arg(number_to_letter(i).toUpper()));
        lb->setProperty("item_id", QVariant(id));
        lay->addWidget(lb, row, 0);

        QString desc = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);
        QString s = QString("color: %1;").arg(get_object_color(o_ptr).name());
        QString style = "text-align: left; font-weight: bold;";
        style += s;

        if (home || store_will_buy(store_idx, o_ptr)) {
            QPushButton *btn = new QPushButton(desc);
            btn->setProperty("item_id", QVariant(id));

            btn->setStyleSheet(style);
            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            connect(btn, SIGNAL(clicked()), this, SLOT(item_click()));
            lay->addWidget(btn, row, 1);
        }
        else
        {
            QLabel *lb2 = new QLabel(desc);
            lb2->setStyleSheet(style);
            lay->addWidget(lb2, row, 1);
        }

        if (!home && store_will_buy(store_idx, o_ptr)) {
            s32b price = price_item(store_idx, o_ptr, true);
            QLabel *l = new QLabel(_num(price));
            lay->addWidget(l, row, 2);
        }

        QPushButton *help_button = new QPushButton;
        help_button->setIcon(QIcon(":/icons/lib/icons/help.png"));
        help_button->setObjectName(id);
        connect(help_button, SIGNAL(clicked()), this, SLOT(help_click()));
        lay->addWidget(help_button, row, 3);

        ++row;
    }
    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    lay->addItem(spacer, row++, 0);
}

void StoreDialog::reset_equip()
{
    QWidget *tab = equip_tab;
    QGridLayout *lay = dynamic_cast<QGridLayout *>(tab->layout());
    if (lay == 0) {
        lay = new QGridLayout;
        lay->setColumnStretch(1, 1);
        tab->setLayout(lay);
    }
    // Remove previous items
    clear_grid(lay);
    int n = 0;
    int i;
    int row = 0;
    if (!home) lay->addWidget(new QLabel("Price"), row++, 2);
    for (i = INVEN_WIELD; i < QUIVER_END; i++)
    {
        object_type *o_ptr = inventory + i;
        if (o_ptr->k_idx == 0) continue;

        QString use;
        if (i < QUIVER_START)
        {
            use = QString("%1: ").arg(mention_use(i));
        }

        // Make an id for the item
        QString id = QString("e%1").arg(i);

        QLabel *lb = new QLabel(QString("%1)").arg(number_to_letter(n++).toUpper()));
        lb->setProperty("item_id", QVariant(id));
        lay->addWidget(lb, row, 0);

        QString desc = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);
        desc.prepend(use);
        QString s = QString("color: %1;").arg(get_object_color(o_ptr).name());
        QString style = "text-align: left; font-weight: bold;";
        style += s;

        if (home || store_will_buy(store_idx, o_ptr)) {
            QPushButton *btn = new QPushButton(desc);
            btn->setProperty("item_id", QVariant(id));

            btn->setStyleSheet(style);
            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            connect(btn, SIGNAL(clicked()), this, SLOT(item_click()));
            lay->addWidget(btn, row, 1);
        }
        else {
            QLabel *lb2 = new QLabel(desc);
            lb2->setStyleSheet(style);
            lay->addWidget(lb2, row, 1);
        }

        if (!home && store_will_buy(store_idx, o_ptr)) {
            s32b price = price_item(store_idx, o_ptr, true);
            QLabel *l = new QLabel(_num(price));
            lay->addWidget(l, row, 2);
        }

        QPushButton *help_button = new QPushButton;
        help_button->setIcon(QIcon(":/icons/lib/icons/help.png"));
        help_button->setObjectName(id);
        connect(help_button, SIGNAL(clicked()), this, SLOT(help_click()));
        lay->addWidget(help_button, row, 3);

        row++;
    }
    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    lay->addItem(spacer, row++, 0);
}

void StoreDialog::toggle_inven()
{
    char_tabs->setCurrentIndex((char_tabs->currentIndex() + 1) % char_tabs->count());
}

void StoreDialog::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_F2:
        this->buy_click();
        break;
    case Qt::Key_F3:
        this->sell_click();
        break;
    case Qt::Key_F4:
        this->toggle_inven();
        break;
    case Qt::Key_F5:
        this->exam_click();
        break;
    case Qt::Key_F6:
        this->wield_click();
        break;
    case Qt::Key_F7:
        this->takeoff_click();
        break;
    default:
        if (event->text().length() > 0
                && event->text().at(0).isLetter())
        {
            QString letter = event->text().at(0);
            letter.append(")");
            QWidget *container = char_tabs->currentWidget();
            if (letter.at(0).isLower())
            {
                container = store_area;
            }
            QList<QLabel *> lst = container->findChildren<QLabel *>();
            for (int i = 0; i < lst.size(); i++)
            {
                QString text = lst.at(i)->text();
                if (text.startsWith(letter))
                {
                    QString item_id = lst.at(i)->property("item_id").toString();
                    process_item(item_id);
                    break;
                }
            }
        return;
        }
        NPPDialog::keyPressEvent(event);
    }
}

void StoreDialog::process_item(QString id)
{
    int aux_mode = mode;
    if (aux_mode != SMODE_EXAMINE)
    {
        if (id.startsWith("e") || id.startsWith("i")) aux_mode = SMODE_SELL;
        else aux_mode = SMODE_BUY;
    }

    set_mode(aux_mode);

    if (aux_mode == SMODE_SELL && id.startsWith("p")) return;
    if (aux_mode == SMODE_BUY && !id.startsWith("p")) return;

    object_type *o_ptr;
    int item = id.mid(1).toInt();  // Get item index

    if (id.startsWith("p"))
    {
        o_ptr = &(store[store_idx].stock[item]);
    }
    else
    {
        o_ptr = inventory + item;
    }

    int price = price_item(store_idx, o_ptr, false);

    switch (aux_mode)
    {
    case SMODE_BUY:
        if (!home && price > p_ptr->au)
        {
            pop_up_message_box("It's too expensive", QMessageBox::Critical);
            return;
        }
        do_buy(o_ptr, item);
        break;
    case SMODE_SELL:
        if (!home && !store_will_buy(store_idx, o_ptr))
        {
            pop_up_message_box("I don't buy that kind of items", QMessageBox::Critical);
            return;
        }
        do_sell(o_ptr, item);
        break;
    case SMODE_EXAMINE:
        object_info_screen(o_ptr);
        break;
    }

    set_mode(SMODE_DEFAULT);
}

void StoreDialog::process_service(QString id)
{
    // Make sure we are buying a service.
    set_mode(SMODE_BUY);
    if (!id.startsWith("s")) return;

    // Get quest index
    int service = id.mid(1).toInt();

    u32b serv_price = price_services(service);

    if (!services_info[service].service_function(service, serv_price)) return;

    reset_all();
}

void StoreDialog::process_quest(QString id)
{

    set_mode(SMODE_BUY);
}

void StoreDialog::item_click()
{
    QObject *obj = QObject::sender();

    QString id = obj->property("item_id").toString();
    process_item(id);
}

void StoreDialog::service_click()
{
    QObject *service = QObject::sender();

    QString id = service->property("item_id").toString();
    process_service(id);
}

void StoreDialog::quest_click()
{
    QObject *quest = QObject::sender();

    QString id = quest->property("item_id").toString();
    process_quest(id);
}

void StoreDialog::sell_click()
{
    set_mode(SMODE_SELL);
}

bool StoreDialog::do_buy(object_type *o_ptr, int item)
{
    int amt = request_amt(o_ptr, true);

    if (amt == 0) return false;

    cmd_arg args;
    args.item = item;
    args.number = amt;

    if (home)   do_cmd_retrieve(store_idx, args);
    else        do_cmd_buy(store_idx, args);

    reset_all();

    return true;
}

void StoreDialog::reset_all()
{
    reset_store();
    reset_inventory();
    reset_equip();

    reset_gold();

    ui_request_size_update(inven_tab);
    ui_request_size_update(equip_tab);
    ui_request_size_update(store_area);
    QCoreApplication::processEvents();   // IMPORTANT: THE SIZE_HINT UPDATE IS ASYNC, SO WAIT FOR IT
    inven_tab->setMinimumSize(inven_tab->sizeHint());
    equip_tab->setMinimumSize(equip_tab->sizeHint());
    store_area->setMinimumSize(store_area->sizeHint());

    ui_request_size_update(central);
    QCoreApplication::processEvents();   // IMPORTANT: THE SIZE_HINT UPDATE IS ASYNC, SO WAIT FOR IT

    this->clientSizeUpdated();
}

bool StoreDialog::do_sell(object_type *o_ptr, int item)
{
    int amt = request_amt(o_ptr, false);

    if (amt == 0) return false;

    cmd_arg args;
    args.item = item;
    args.number = amt;

    if (home)   do_cmd_stash(store_idx, args);
    else        do_cmd_sell(store_idx, args);

    reset_all();

    return true;
}

void QuantityDialog::do_accept()
{
    amt = amt_spin->value();
    accept();
}

QuantityDialog::QuantityDialog(object_type *op, bool buy)
{
    o_ptr = op;

    buying = buy;

    amt = 0;

    int store_idx = f_info[dungeon_info[p_ptr->py][p_ptr->px].feat].f_power;

    price = price_item(store_idx, o_ptr, !buy);

    if (buying) {
        int money = p_ptr->au;
        max = money / MAX(price, 1);
        max = MIN(max, o_ptr->number);
    }
    else {
        max = o_ptr->number;
    }

    QVBoxLayout *lay1 = new QVBoxLayout;
    this->setLayout(lay1);

    QString verb = tr("sell");
    if (buying) verb = tr("buy");

    QString desc = object_desc(o_ptr, ODESC_FULL | ODESC_PREFIX);
    QString msg = tr("How many items do you want to %1 of %2?").arg(verb).arg(desc);
    question = new QLabel(msg);
    lay1->addWidget(question);

    amt_spin = new QSpinBox();
    lay1->addWidget(amt_spin);
    amt_spin->setMinimum(0);
    amt_spin->setMaximum(max);
    if (buying) amt_spin->setValue(1);
    else amt_spin->setValue(max);
    amt_spin->selectAll();

    total_label = new QLabel("");
    lay1->addWidget(total_label);
    update_totals(amt_spin->value());

    connect(amt_spin, SIGNAL(valueChanged(int)), this, SLOT(update_totals(int)));

    QHBoxLayout *lay2 = new QHBoxLayout;
    lay2->setContentsMargins(0, 0, 0, 0);
    lay1->addLayout(lay2);

    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    lay2->addItem(spacer);

    QPushButton *btn1 = new QPushButton("Ok");
    lay2->addWidget(btn1);
    connect(btn1, SIGNAL(clicked()), this, SLOT(do_accept()));

    QPushButton *btn2 = new QPushButton("Cancel");
    lay2->addWidget(btn2);
    connect(btn2, SIGNAL(clicked()), this, SLOT(reject()));
}

void QuantityDialog::update_totals(int value)
{
    int n = value;
    int money = p_ptr->au;
    int sign = 1;
    if (buying) sign = -1;
    QString msg = tr("Max. items: %4 - Total price: %1 - Gold: %2 - After: %3")
            .arg(n * price).arg(money).arg(money + n * price * sign).arg(max);
    total_label->setText(msg);
}

int StoreDialog::request_amt(object_type *o_ptr, bool buying)
{
    if (home) {
        return o_ptr->number;
    }

    int amt = o_ptr->number;
    int price = price_item(store_idx, o_ptr, !buying);
    QString desc = object_desc(o_ptr, ODESC_FULL | ODESC_PREFIX);
    QString verb = tr("sell");
    if (buying) verb = tr("buy");

    if (amt == 1) {
        QString msg = tr("Do you want to %1 %2? Price: %3")
                .arg(verb).arg(desc).arg(price);
        if (!get_check(msg)) return 0;
        return 1;
    }

    QuantityDialog *dlg = new QuantityDialog(o_ptr, buying);
    dlg->exec();
    amt = dlg->amt;
    delete dlg;

    return amt;
}

bool StatDialog::init_stats_table(int service)
{
    int i;
    bool good_stat = FALSE;

    // Clear the table
    for (i = 0; i < A_MAX; i++) stats[i] = FALSE;

    /* Count up the stats that require the service */
    for (i = 0; i < A_MAX; i++)
    {
        /* Add the stat that need to be restored */
        if ((service == SERVICE_RESTORE_STAT) &&
            (p_ptr->stat_cur[i] == p_ptr->stat_max[i])) continue;

        if ((service == SERVICE_INCREASE_STAT) &&
            (p_ptr->stat_max[i] == 18+100))  continue;

        if ((service == SERVICE_QUEST_REWARD_INC_STAT) &&
            (p_ptr->stat_quest_add[i] >= 3)) continue;

        //this stat is a valid choice
        stats[i] = TRUE;
        good_stat = TRUE;
    }

    return (good_stat);
}

// The first character is the number of the stat collected.
void StatDialog::select_str(void){selected_stat = 0; this->accept();}
void StatDialog::select_int(void){selected_stat = 1; this->accept();}
void StatDialog::select_wis(void){selected_stat = 2; this->accept();}
void StatDialog::select_dex(void){selected_stat = 3; this->accept();}
void StatDialog::select_con(void){selected_stat = 4; this->accept();}
void StatDialog::select_chr(void){selected_stat = 5; this->accept();}

/*
 * Stat selected of A_MAX means no eligible stat.
 * A_Max+1 is cancel.
 */
StatDialog::StatDialog(int service, byte *stat_selected)
{
    if (!init_stats_table(service))
    {
        *stat_selected = A_MAX;
        return;
    }

    QString prompt;

    if (service == SERVICE_RESTORE_STAT)
    {
        prompt = QString("<b><big>Please select a stat to restore.</big></b><br>");
    }
    else if (service == SERVICE_INCREASE_STAT)
    {
        prompt = QString("<b><big>Please select a stat to increase.</big></b><br>");
    }
    /* must be SERVICE_QUEST_REWARD_INC_STAT*/
    else
    {
        prompt = QString("<b><big>Please select a stat to permanently increase.</big></b><br>");
    }

    main_prompt = new QLabel(prompt);
    main_prompt->setAlignment(Qt::AlignCenter);

    QVBoxLayout *vlay = new QVBoxLayout;

    vlay->addWidget(main_prompt);

    // Add the stats
    QGridLayout *stat_layout = new QGridLayout;

    // add the headers
    byte row = 0;
    byte col = 0;
    QLabel *stat_header = new QLabel("Stat");
    QLabel *self_header = new QLabel("Self");
    QLabel *race_adj_header = new QLabel("Race Adj.");
    QLabel *class_adj_header = new QLabel("Class Adj");
    QLabel *equip_adj_header = new QLabel("Equip Adj");
    QLabel *total_stat_header = new QLabel("Total Stat");
    stat_header->setAlignment(Qt::AlignLeft);
    self_header->setAlignment(Qt::AlignLeft);
    race_adj_header->setAlignment(Qt::AlignCenter);
    class_adj_header->setAlignment(Qt::AlignCenter);
    equip_adj_header->setAlignment(Qt::AlignCenter);
    total_stat_header->setAlignment(Qt::AlignLeft);
    stat_layout->addWidget(stat_header, row, col++);
    stat_layout->addWidget(self_header, row, col++);
    if (adult_maximize) stat_layout->addWidget(race_adj_header, row, col++);
    if (adult_maximize) stat_layout->addWidget(class_adj_header, row, col++);
    stat_layout->addWidget(equip_adj_header, row, col++);
    stat_layout->addWidget(total_stat_header, row, col++);

    for (int i = 0; i < A_MAX; i++)
    {
        col = 0;
        row++;

        // Do a button if we can select this stat
        if (stats[i])
        {
            QPushButton *stat_name_button = new QPushButton(stat_names[i]);
            stat_layout->addWidget(stat_name_button, row, col++);
            if (i == 0)connect(stat_name_button, SIGNAL(clicked()), this, SLOT(select_str()));
            else if (i == 1)connect(stat_name_button, SIGNAL(clicked()), this, SLOT(select_int()));
            else if (i == 2)connect(stat_name_button, SIGNAL(clicked()), this, SLOT(select_wis()));
            else if (i == 3)connect(stat_name_button, SIGNAL(clicked()), this, SLOT(select_dex()));
            else if (i == 4)connect(stat_name_button, SIGNAL(clicked()), this, SLOT(select_con()));
            else if (i == 5)connect(stat_name_button, SIGNAL(clicked()), this, SLOT(select_chr()));
        }
        // or make a label
        else
        {
            QLabel *self_label = new QLabel(stat_names[stats[i]]);
            stat_layout->addWidget(self_label, row, col++);
        }

        QLabel *stat_player = new QLabel(cnv_stat(p_ptr->stat_max[i]));
        stat_player->setAlignment(Qt::AlignCenter);
        stat_layout->addWidget(stat_player, row, col++);

        if (adult_maximize)
        {
            QLabel *race_adj = new QLabel(QString("%1") .arg(rp_ptr->r_adj[i] + p_ptr->stat_quest_add[i]));
            race_adj->setAlignment(Qt::AlignCenter);
            stat_layout->addWidget(race_adj, row, col++);

            QLabel *class_adj = new QLabel(QString("%1") .arg(cp_ptr->c_adj[i]));
            class_adj->setAlignment(Qt::AlignCenter);
            stat_layout->addWidget(class_adj, row, col++);
        }

        QLabel *equip_adj = new QLabel(QString("%1") .arg(p_ptr->state.stat_add[i]));
        equip_adj->setAlignment(Qt::AlignCenter);
        stat_layout->addWidget(equip_adj, row, col++);

        QLabel *stat_total = new QLabel(cnv_stat(p_ptr->state.stat_top[i]));
        stat_total->setAlignment(Qt::AlignLeft);
        stat_layout->addWidget(stat_total, row, col++);

        //Display reduced stat if necessary
        if (p_ptr->state.stat_use[i] < p_ptr->state.stat_top[i])
        {
            QString lower_stat = cnv_stat(p_ptr->state.stat_use[i]);
            lower_stat = color_string(lower_stat, TERM_YELLOW);
            QLabel *stat_reduce = new QLabel(lower_stat);
            stat_reduce->setAlignment(Qt::AlignLeft);
            stat_layout->addWidget(stat_reduce, row, col++);
        }
    }

    buttons = new QDialogButtonBox(QDialogButtonBox::Cancel);
    connect(buttons, SIGNAL(rejected()), this, SLOT(close()));

    vlay->addLayout(stat_layout);
    vlay->addWidget(buttons);
    setLayout(vlay);
    setWindowTitle(tr("Stat Selection Menu"));

    if (!this->exec())
    {
        *stat_selected = (A_MAX + 1);
    }
    else
    {
        *stat_selected = selected_stat;
    }
}

int launch_stat_dialog(int choice)
{
    byte selection;

    StatDialog(choice, &selection);
    return selection;
}
