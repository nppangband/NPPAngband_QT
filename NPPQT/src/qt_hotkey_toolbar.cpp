
/*
 *  qt_hotkey_toolbar.cpp
 * Copyright (c) 2016  Jeff Greene, Diego Gonzalez
 *
 * Please see copyright.txt for complete copyright and licensing restrictions.
 *
 */

#include <src/qt_mainwindow.h>
#include <src/npp.h>
#include <QToolBar>
#include <src/hotkeys.h>

/*
 * This assumes the qactions coincide with the list_hotkeys in hotkeys.cpp
 */
void MainWindow::hotkey_toolbar_clicked(QAction *action)
{
    // Paranoia
    if (!action) return;
    if (!character_dungeon) return;
    bool shift = FALSE;

    // Extract the funtion button and shift information
    QString action_name = action->objectName();
    if (action_name.contains("Shift-", Qt::CaseSensitive))
    {
        shift = TRUE;
        action_name.remove("Shift-");
    }
    action_name.remove("F", Qt::CaseSensitive);
    bool ok;
    int num = action_name.toInt(&ok) - 1;

    // Paranoia
    if (!ok) return;

    // Now refer to the list_hotkeys list
    if (shift) num += (NUM_HOTKEYS/2);

    // Paranoia
    if (num >= NUM_HOTKEYS) return;
    hotkey_list *hk_list_ptr = &list_hotkeys[num];

    // Run the hotkey
    check_hotkey_commands(hk_list_ptr->listed_hotkey, hk_list_ptr->shift, FALSE, FALSE, FALSE);
}


void MainWindow::update_hotkey_toolbar()
{
    for (int i = 0; i < list_hotkey_toolbar_qactions.size(); i++)
    {
        QAction *qa_ptr = list_hotkey_toolbar_qactions.at(i);
        hotkey_list *hk_list_ptr = &list_hotkeys[i];
        single_hotkey *plyr_hk_ptr = &player_hotkeys[i];

        if (plyr_hk_ptr->has_commands())
        {
            qa_ptr->setToolTip(plyr_hk_ptr->hotkey_name);
            qa_ptr->setVisible(TRUE);
        }

        // Hide and re-set name
        else
        {
            qa_ptr->setToolTip(hk_list_ptr->hotkey_list_name + QString(" Hotkey"));
            qa_ptr->setVisible(FALSE);
        }
    }

    show_hotkey_toolbar();
}

// Set up the statusbar.  The order of the icons needs
// to be consistent with the enum in qt_statusbar.h
void MainWindow::create_hotkey_toolbar(void)
{
    list_hotkey_toolbar_qactions.clear();

    hotkey_toolbar_actions = new QActionGroup(this);

    hotkey_toolbar = new QToolBar;
    hotkey_toolbar->setObjectName("Hotkey Toolbar");
    addToolBar(Qt::RightToolBarArea, hotkey_toolbar);
    hide_hotkey_toolbar();

    for (int i = 0; i < NUM_HOTKEYS; i++)
    {
        hotkey_list *hk_list_ptr = &list_hotkeys[i];
        QString qaction_name = hk_list_ptr->hotkey_list_name;
        QString qaction_tooltip = hk_list_ptr->hotkey_list_name + QString(" Hotkey");

        QPointer<QAction> this_qaction = new QAction(this);
        this_qaction->setToolTip(qaction_tooltip);
        this_qaction->setObjectName(qaction_name);
        hotkey_toolbar->addAction(this_qaction);
        this_qaction->setVisible(FALSE);
        hotkey_toolbar_actions->addAction(this_qaction);
        list_hotkey_toolbar_qactions.append(this_qaction);

        switch (i)
        {
            case 0: {this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f1.png")); break;}
            case 1: {this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f2.png")); break;}
            case 2: {this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f3.png")); break;}
            case 3: {this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f4.png")); break;}
            case 4: {this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f5.png")); break;}
            case 5: {this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f6.png")); break;}
            case 6: {this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f7.png")); break;}
            case 7: {this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f8.png")); break;}
            case 8: {this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f9.png")); break;}
            case 9:{this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f10.png")); break;}
            case 10:{this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f11.png")); break;}
            case 11:{this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f12.png")); break;}
            case 12:{this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f1_shift.png")); break;}
            case 13:{this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f2_shift.png")); break;}
            case 14:{this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f3_shift.png")); break;}
            case 15:{this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f4_shift.png")); break;}
            case 16:{this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f5_shift.png")); break;}
            case 17:{this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f6_shift.png")); break;}
            case 18:{this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f7_shift.png")); break;}
            case 19:{this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f8_shift.png")); break;}
            case 20:{this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f9_shift.png")); break;}
            case 21:{this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f10_shift.png")); break;}
            case 22:{this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f11_shift.png")); break;}
            case 23:{this_qaction->setIcon(QIcon(":/icons/lib/icons/key_f12_shift.png")); break;}
            default: break;
        }
    }

    connect(hotkey_toolbar_actions, SIGNAL(triggered(QAction*)), this, SLOT(hotkey_toolbar_clicked(QAction*)));

    /*
    hotkey_f2 = new QAction(tr("F2 Hotkey"), this);
    hotkey_f2->setIcon(QIcon(":/icons/lib/icons/key_f2.png"));
    hotkey_f2->setToolTip("F2 Hotkey");
    hotkey_toolbar->addAction(hotkey_f2);
    hotkey_f2->setVisible(FALSE);

    hotkey_f3 = new QAction(tr("F3 Hotkey"), this);
    hotkey_f3->setIcon(QIcon(":/icons/lib/icons/key_f3.png"));
    hotkey_f3->setToolTip("F3 Hotkey");
    hotkey_toolbar->addAction(hotkey_f3);
    hotkey_f3->setVisible(FALSE);

    hotkey_f4 = new QAction(tr("F4 Hotkey"), this);
    hotkey_f4->setIcon(QIcon(":/icons/lib/icons/key_f4.png"));
    hotkey_f4->setToolTip("F4 Hotkey");
    hotkey_toolbar->addAction(hotkey_f4);
    hotkey_f4->setVisible(FALSE);

    hotkey_f5 = new QAction(tr("F5 Hotkey"), this);
    hotkey_f5->setIcon(QIcon(":/icons/lib/icons/key_f5.png"));
    hotkey_f5->setToolTip("F5 Hotkey");
    hotkey_toolbar->addAction(hotkey_f5);
    hotkey_f5->setVisible(FALSE);

    hotkey_f6 = new QAction(tr("F6 Hotkey"), this);
    hotkey_f6->setIcon(QIcon(":/icons/lib/icons/key_f6.png"));
    hotkey_f6->setToolTip("F6 Hotkey");
    hotkey_toolbar->addAction(hotkey_f6);
    hotkey_f6->setVisible(FALSE);

    hotkey_f7 = new QAction(tr("F7 Hotkey"), this);
    hotkey_f7->setIcon(QIcon(":/icons/lib/icons/key_f7.png"));
    hotkey_f7->setToolTip("F7 Hotkey");
    hotkey_toolbar->addAction(hotkey_f7);
    hotkey_f7->setVisible(FALSE);

    hotkey_f8 = new QAction(tr("F8 Hotkey"), this);
    hotkey_f8->setIcon(QIcon(":/icons/lib/icons/key_f8.png"));
    hotkey_f8->setToolTip("F8 Hotkey");
    hotkey_toolbar->addAction(hotkey_f8);
    hotkey_f8->setVisible(FALSE);

    hotkey_f9 = new QAction(tr("F9 Hotkey"), this);
    hotkey_f9->setIcon(QIcon(":/icons/lib/icons/key_F9.png"));
    hotkey_f9->setToolTip("F9 Hotkey");
    hotkey_toolbar->addAction(hotkey_f9);
    hotkey_f9->setVisible(FALSE);

    hotkey_f10 = new QAction(tr("F10 Hotkey"), this);
    hotkey_f10->setIcon(QIcon(":/icons/lib/icons/key_F10.png"));
    hotkey_f10->setToolTip("10 Hotkey");
    hotkey_toolbar->addAction(hotkey_f10);
    hotkey_f10->setVisible(FALSE);

    hotkey_f11 = new QAction(tr("F11 Hotkey"), this);
    hotkey_f11->setIcon(QIcon(":/icons/lib/icons/key_F11.png"));
    hotkey_f11->setToolTip("11 Hotkey");
    hotkey_toolbar->addAction(hotkey_f11);
    hotkey_f11->setVisible(FALSE);

    hotkey_f12 = new QAction(tr("F12 Hotkey"), this);
    hotkey_f12->setIcon(QIcon(":/icons/lib/icons/key_F12.png"));
    hotkey_f12->setToolTip("12 Hotkey");
    hotkey_toolbar->addAction(hotkey_f12);
    hotkey_f12->setVisible(FALSE);*/
}


void MainWindow::hide_hotkey_toolbar(void)
{
    if (hotkey_toolbar == NULL) return;
    hotkey_toolbar->setVisible(FALSE);
}

void MainWindow::show_hotkey_toolbar(void)
{
    hotkey_toolbar->setVisible(TRUE);
}
