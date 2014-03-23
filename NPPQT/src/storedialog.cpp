#include "storedialog.h"
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QSpacerItem>
#include <QGroupBox>
#include "npp.h"
#include "store.h"

StoreDialog::StoreDialog(int _store, QWidget *parent): NPPDialog(parent)
{
    store_idx = _store;

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

    QPushButton *btn_buy = new QPushButton("Buy (F2)");
    lay3->addWidget(btn_buy);
    connect(btn_buy, SIGNAL(clicked()), this, SLOT(buy_click()));

    QPushButton *btn_sell = new QPushButton("Sell (F3)");
    lay3->addWidget(btn_sell);
    connect(btn_sell, SIGNAL(clicked()), this, SLOT(sell_click()));

    QPushButton *btn_toggle = new QPushButton("Toggle inven/equip (F4)");
    lay3->addWidget(btn_toggle);
    connect(btn_toggle, SIGNAL(clicked()), this, SLOT(toggle_inven()));

    mode_label = new QLabel("");
    lay3->addWidget(mode_label);
    mode_label->setStyleSheet("font-weight: bold;");

    set_mode(SMODE_DEFAULT);

    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    lay3->addItem(spacer);

    QWidget *area2 = new QWidget;
    QHBoxLayout *lay2 = new QHBoxLayout;
    area2->setLayout(lay2);
    lay2->setContentsMargins(0, 0, 0, 0);
    lay1->addWidget(area2);

    QGroupBox *box1 = new QGroupBox("Store items");
    lay2->addWidget(box1);
    QVBoxLayout *lay4 = new QVBoxLayout;
    box1->setLayout(lay4);

    store_area = new QWidget;
    lay4->addWidget(store_area);

    char_tabs = new QTabWidget;
    lay2->addWidget(char_tabs);

    inven_tab = new QWidget;
    equip_tab = new QWidget;

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

void StoreDialog::buy_click()
{
    set_mode(SMODE_BUY);
}

void StoreDialog::reset_store()
{
    QLayout *lay = store_area->layout();
    if (lay == 0) {
        lay = new QVBoxLayout;
        store_area->setLayout(lay);
    }
    // Remove previous items
    QLayoutItem *item;
    while ((item = lay->takeAt(0)) != 0) {
        delete item;
    }
    store_type *st = &store[store_idx];
    for (int i = 0; i < st->stock_num; i++) {
        object_type *o_ptr = &st->stock[i];
        if (o_ptr->k_idx == 0) continue;
        QString desc = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);
        QPushButton *btn = new QPushButton(QString("%1) %2")
                                           .arg(number_to_letter(i))
                                           .arg(desc));
        QString id = QString("s%1").arg(i);
        btn->setProperty("item_id", QVariant(id));
        btn->setStyleSheet("text-align: left;");
        connect(btn, SIGNAL(clicked()), this, SLOT(item_click()));
        lay->addWidget(btn);
    }
    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    lay->addItem(spacer);
}

void StoreDialog::set_mode(int _mode)
{
    mode = _mode;
    QString names[] = {
        QString(""), QString("Buying"), QString("Selling"),
        QString("Querying info"), QString("Examining")
    };
    mode_label->setText(names[mode]);
}

void StoreDialog::reset_inventory()
{
    QWidget *tab = inven_tab;
    QLayout *lay = tab->layout();
    if (lay == 0) {
        lay = new QVBoxLayout;
        tab->setLayout(lay);
    }
    // Remove previous items
    QLayoutItem *item;
    while ((item = lay->takeAt(0)) != 0) {
        delete item;
    }
    for (int i = 0; i < INVEN_WIELD - 1; i++) {
        object_type *o_ptr = inventory + i;
        if (o_ptr->k_idx == 0) continue;
        QString desc = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);
        QPushButton *btn = new QPushButton(QString("%1) %2")
                                           .arg(number_to_letter(i))
                                           .arg(desc));
        QString id = QString("i%1").arg(i);
        btn->setProperty("item_id", QVariant(id));
        btn->setStyleSheet("text-align: left;");
        connect(btn, SIGNAL(clicked()), this, SLOT(item_click()));
        lay->addWidget(btn);
    }
    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    lay->addItem(spacer);
}

void StoreDialog::reset_equip()
{
    QWidget *tab = equip_tab;
    QLayout *lay = tab->layout();
    if (lay == 0) {
        lay = new QVBoxLayout;
        tab->setLayout(lay);
    }
    // Remove previous items
    QLayoutItem *item;
    while ((item = lay->takeAt(0)) != 0) {
        delete item;
    }
    int n = 0;
    for (int i = INVEN_WIELD; i < QUIVER_END; i++) {
        object_type *o_ptr = inventory + i;
        if (o_ptr->k_idx == 0) continue;
        QString use;
        if (i < QUIVER_START) {
            use = QString("%1: ").arg(mention_use(i));
        }
        QString desc = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);
        QPushButton *btn = new QPushButton(QString("%1) %2%3")
                                           .arg(number_to_letter(n++))
                                           .arg(use)
                                           .arg(desc));
        QString id = QString("e%1").arg(i);
        btn->setProperty("item_id", QVariant(id));
        btn->setStyleSheet("text-align: left;");
        connect(btn, SIGNAL(clicked()), this, SLOT(item_click()));
        lay->addWidget(btn);
    }
    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    lay->addItem(spacer);
}

void StoreDialog::toggle_inven()
{
    char_tabs->setCurrentIndex((char_tabs->currentIndex() + 1) % char_tabs->count());
}

void StoreDialog::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_F4:
        this->toggle_inven();
        break;
    case Qt::Key_F2:
        this->buy_click();
        break;
    case Qt::Key_F3:
        this->sell_click();
        break;
    default:
        if (event->text().length() > 0
                && event->text().at(0).isLetter()) {
            if (mode == SMODE_DEFAULT) return;
            QString letter = event->text().at(0);
            letter.append(") ");
            QWidget *container = char_tabs->currentWidget();
            if (mode == SMODE_BUY || mode == SMODE_EXAMINE) {
                container = store_area;
            }
            QList<QPushButton *> lst = container->findChildren<QPushButton *>();
            for (int i = 0; i < lst.size(); i++) {
                if (lst.at(i)->text().startsWith(letter)) {
                    lst.at(i)->click();
                    break;
                }
            }
            return;
        }
        NPPDialog::keyPressEvent(event);
    }
}

void StoreDialog::item_click()
{
    QObject *obj = QObject::sender();
    QString id = obj->property("item_id").toString();
    int aux_mode = mode;
    if (aux_mode == SMODE_DEFAULT) {
        if (id.startsWith("e") || id.startsWith("i")) aux_mode = SMODE_SELL;
        else aux_mode = SMODE_BUY;
    }

    set_mode(aux_mode);

    if (aux_mode == SMODE_SELL && id.startsWith("s")) return;
    if (aux_mode == SMODE_BUY && !id.startsWith("s")) return;

    QString desc;
    object_type *o_ptr;
    int item = id.mid(1).toInt();  // Get item index
    if (id.startsWith("s")) {
        o_ptr = &(store[store_idx].stock[item]);
    }
    else {
        o_ptr = inventory + item;
    }
    desc = object_desc(o_ptr, ODESC_FULL | ODESC_PREFIX);

    switch (aux_mode) {
    case SMODE_BUY:
        get_check(QString("Do you want to buy this item (%1)?").arg(desc));
        break;
    case SMODE_SELL:
        get_check(QString("Do you want to sell this item (%1)?").arg(desc));
        break;
    }

    set_mode(SMODE_DEFAULT);
}

void StoreDialog::sell_click()
{
    set_mode(SMODE_SELL);
}
