
/* File: knowledge_monsters.cpp */

/*
 * Copyright (c) 2014 Jeff Greene, Diego Gonzalez
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 3, or
 *
 * b) the "Angband licence":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 */


#include <src/npp.h>
#include <src/knowledge.h>
#include <src/store.h>
#include <src/utilities.h>
#include <src/object_settings.h>
#include <QVBoxLayout>
#include <QPushButton>

static QString squelch_status[SQUELCH_OPT_MAX] =
{
    "Never Squelch",
    "Never Pickup",
    "Always Pickup",
    "Always Squelch",
};

static const byte squelch_status_color[SQUELCH_OPT_MAX] =
{
    TERM_YELLOW,
    TERM_L_GREEN,
    TERM_L_UMBER,
    TERM_L_RED,
};


#define NUM_OBJECT_GROUPS 34

object_grouper object_text_order[NUM_OBJECT_GROUPS] =
{
    {TV_RING,			"Rings"			},
    {TV_AMULET,			"Amulets"		},
    {TV_POTION,			"Potions"		},
    {TV_SCROLL,			"Scrolls"		},
    {TV_WAND,			"Wands"			},
    {TV_STAFF,			"Staves"		},
    {TV_ROD,			"Rods"			},
    {TV_FOOD,			"Food"			},
    {TV_PRAYER_BOOK,	"Priest Books"	},
    {TV_DRUID_BOOK,		"Druid Books"	},
    {TV_MAGIC_BOOK,		"Mage Books"	},
    {TV_LIGHT,			"Lights"		},
    {TV_FLASK,			"Flasks"		},
    {TV_SWORD,			"Swords"		},
    {TV_POLEARM,		"Polearms"		},
    {TV_HAFTED,			"Hafted Weapons"},
    {TV_BOW,			"Bows"			},
    {TV_ARROW,			"Ammunition"	},
    {TV_BOLT,			"Bolts"			}, // moved to "Ammunition group".  See object_matched_group.
    {TV_SHOT,			"Shots"			}, // moved to "Ammunition group"   See object_matched_group.
    {TV_SHIELD,			"Shields"		},
    {TV_CROWN,			"Crowns"		},
    {TV_HELM,			"Helms"			},
    {TV_GLOVES,			"Gloves"		},
    {TV_BOOTS,			"Boots"			},
    {TV_CLOAK,			"Cloaks"		},
    {TV_DRAG_ARMOR,		"Dragon Scale Mail" },
    {TV_DRAG_SHIELD,	"Dragon Scale Shields" },
    {TV_HARD_ARMOR,		"Hard Armors"	},
    {TV_SOFT_ARMOR,		"Soft Armors"	},
    {TV_CHEST,          "Chests"		},
    {TV_SPIKE,			"Spikes"		},
    {TV_DIGGING,		"Diggers"		},
    {TV_JUNK,			"Junk"			}, // Junk should be last
};

/*
 * Add a pval so the object descriptions don't look strange*
 */
void apply_magic_fake(object_type *o_ptr)
{
    /* Analyze type */
    switch (o_ptr->tval)
    {
        case TV_DIGGING:
        {
            o_ptr->pval = 1;
            break;
        }

        /*many rings need a pval*/
        case TV_RING:
        {
            switch (o_ptr->sval)
            {
                case SV_RING_STR:
                case SV_RING_CON:
                case SV_RING_DEX:
                case SV_RING_INT:
                case SV_RING_SPEED:
                case SV_RING_SEARCHING:
                {
                    o_ptr->pval = 1;
                    break;
                }

                case SV_RING_AGGRAVATION:
                {
                    o_ptr->ident |= (IDENT_CURSED);
                    break;
                }
                case SV_RING_WEAKNESS:
                case SV_RING_STUPIDITY:
                {
                    /* Broken */
                    o_ptr->ident |= (IDENT_BROKEN);

                    /* Cursed */
                    o_ptr->ident |= (IDENT_CURSED);

                    /* Penalize */
                    o_ptr->pval = -1;

                    break;
                }
                /* WOE */
                case SV_RING_WOE:
                {
                    /* Broken */
                    o_ptr->ident |= (IDENT_BROKEN);

                    /* Cursed */
                    o_ptr->ident |= (IDENT_CURSED);

                    /* Penalize */
                    o_ptr->to_a = -1;
                    o_ptr->pval = -1;

                    break;
                }
                /* Ring that increase damage */
                case SV_RING_DAMAGE:
                {
                    /* Bonus to damage */
                    o_ptr->to_d = 1;

                    break;
                }
                /* Ring that increase accuracy */
                case SV_RING_ACCURACY:
                {
                    /* Bonus to hit */
                    o_ptr->to_h = 1;

                    break;
                }
                /* Rings that provide of Protection */
                case SV_RING_PROTECTION:
                case SV_RING_FLAMES:
                case SV_RING_ACID:
                case SV_RING_ICE:
                case SV_RING_LIGHTNING:
                {
                    /* Bonus to armor class */
                    o_ptr->to_a = 1;

                    break;
                }
                /* Rings that provide of Protection */
                case SV_RING_LORD_PROT_ACID:
                case SV_RING_LORD_PROT_FIRE:
                case SV_RING_LORD_PROT_COLD:
                {
                    /* Bonus to armor class */
                    o_ptr->to_a = 5;

                    break;
                }
                /*both to-hit and to-damage*/
                case SV_RING_SLAYING:
                {
                    /* Bonus to damage and to hit */
                    o_ptr->to_d = 1;
                    o_ptr->to_h = 1;

                    break;
                }
                default: break;

            }
            /*break for TVAL-Rings*/
            break;
        }

        case TV_AMULET:
        {
            /* Analyze */
            switch (o_ptr->sval)
            {
                /* Amulet of wisdom/charisma/infravision */
                case SV_AMULET_WISDOM:
                case SV_AMULET_CHARISMA:
                case SV_AMULET_INFRAVISION:
                case SV_AMULET_SEARCHING:
                case SV_AMULET_ESP:
                case SV_AMULET_DEVOTION:
                case SV_AMULET_TRICKERY:
                {
                    /* Stat bonus */
                    o_ptr->pval = 1;

                    break;
                }

                /* Amulet of the Magi -- never cursed */
                case SV_AMULET_THE_MAGI:
                {
                    o_ptr->pval = 1;
                    o_ptr->to_a = 1;

                    break;
                }

                /* Amulet of Weaponmastery -- never cursed */
                case SV_AMULET_WEAPONMASTERY:
                {
                    o_ptr->to_h = 1;
                    o_ptr->to_d = 1;
                    o_ptr->pval = 1;

                    break;
                }

                /* Amulet of Doom -- always cursed */
                case SV_AMULET_DOOM:
                case SV_AMULET_WOE:
                {
                    /* Broken */
                    o_ptr->ident |= (IDENT_BROKEN);

                    /* Cursed */
                    o_ptr->ident |= (IDENT_CURSED);

                    /* Penalize */
                    o_ptr->pval = -1;
                    o_ptr->to_a = -1;

                    break;
                }

                default: break;

            }
            /*break for TVAL-Amulets*/
            break;
        }

        case TV_LIGHT:
        {
            /* Analyze */
            switch (o_ptr->sval)
            {
                case SV_LIGHT_TORCH:
                case SV_LIGHT_LANTERN:
                {
                    o_ptr->timeout = 1;

                    break;
                }

            }
            /*break for TVAL-Lites*/
            break;
        }

        /*give then one charge*/
        case TV_STAFF:
        case TV_WAND:
        {
            o_ptr->pval = 1;

            break;
        }
    }
}

// Make an object just for display purposes.
void make_object_fake(object_type *o_ptr, int k_idx)
{
    o_ptr->object_wipe();

    /* Create the object */
    object_prep(o_ptr, k_idx);

    /* Add minimum bonuses so the descriptions don't look strange. */
    apply_magic_fake(o_ptr);

    /* Hack -- its in the store */
    if (k_info[k_idx].aware) o_ptr->ident |= (IDENT_STORE);

    /* It's fully known */
    if (!k_info[k_idx].flavor)
    {
        /* Mark the item as fully known */
        o_ptr->ident |= (IDENT_MENTAL);
        object_aware(o_ptr);
        object_known(o_ptr);
    }
}

/*
 * Describe fake ego item "lore"
 */
static void desc_ego_fake(int ego_num)
{
    /* Hack: dereference the join */
    QString xtra[10] = { "sustains", "higher resistances", "abilities", "immunities", "stat increases",
                            "slays", "*slays*", "elemental brands", "elemental resists", "native abilities"};

    ego_item_type *e_ptr = &e_info[ego_num];

    object_type dummy;
    object_type *o_ptr = &dummy;
    o_ptr->object_wipe();

    QString output = color_string(QString("%1\n") .arg(e_ptr->e_name), TERM_L_BLUE);


    if (e_ptr->e_text.length())
    {
        output.append(QString("\n%1\n") .arg(e_ptr->e_text));
    }

    /* List ego flags */
    o_ptr->ego_num = ego_num;
    o_ptr->tval = e_ptr->tval[0];
    output.append(object_info_out(o_ptr, FALSE));

    if (e_ptr->xtra)
    {
        output.append(QString("It provides one or more random %1.\n") .arg(xtra[e_ptr->xtra - 1]));
    }

    if (e_ptr->e_flags3 & (TR3_PERMA_CURSE)) output.append("It is permanently cursed.");
    else if (e_ptr->e_flags3 & (TR3_HEAVY_CURSE)) output.append("It is heavily cursed.");
    if (e_ptr->e_flags3 & (TR3_LIGHT_CURSE)) output.append("It is cursed.");

    /* Finally, display it */
    display_info_window(DISPLAY_INFO_OBJECT, o_ptr->k_idx, output, o_ptr);
}

/*
 * Show artifact lore
 */
void desc_art_fake(int a_idx)
{
    object_type *o_ptr;
    object_type object_type_body;
    bool lost = TRUE;
    int i, j;

    /* Get local object */
    o_ptr = &object_type_body;
    o_ptr->object_wipe();

    /* Look for the artifact, either in inventory, store or the object list */
    for (i = 0; i < z_info->o_max; i++)
    {
        if (o_list[i].art_num == a_idx)
        {
            o_ptr = &o_list[i];
            lost = FALSE;
            break;
        }
    }

    if (lost)
    {
        for (i = 0; i < INVEN_TOTAL; i++)
        {
            if (inventory[i].art_num == a_idx)
            {
                o_ptr = &inventory[i];
                lost = FALSE;
                break;
            }
        }
    }

    if (lost)
    {
        for (j = 0; j < MAX_STORES; j++)
        {
            for (i = 0; i < store[j].stock_size; i++)
            {
                if (store[j].stock[i].art_num == a_idx)
                {
                    o_ptr = &store[j].stock[i];
                    lost = FALSE;
                    break;
                }
            }
            if (!lost) break;
        }
    }

    /* If it's been lost, make a fake artifact for it */
    if (lost)
    {
        make_fake_artifact(o_ptr, a_idx);
        object_aware(o_ptr);
        object_known(o_ptr);
        o_ptr->ident |= (IDENT_MENTAL);
    }

    /* Print the artifact information */
    object_info_screen(o_ptr);

}


/*
 * Looks up an artifact idx given an object_kind *that's already known
 * to be an artifact*.  Behaviour is distinctly unfriendly if passed
 * flavours which don't correspond to an artifact.
 */
static int get_artifact_from_kind(object_kind *k_ptr)
{
    int i;

    /* Look for the corresponding artifact */
    for (i = 0; i < z_info->art_max; i++)
    {
        if (k_ptr->tval == a_info[i].tval &&
            k_ptr->sval == a_info[i].sval)
        {
            break;
        }
    }

    return i;
}

/*
 * Check if the given artifact idx is something we should "Know" about
 */
static bool artifact_is_known(int a_idx)
{
    int i;
    store_type *st_ptr = &store[STORE_GUILD];
    artifact_type *a_ptr = &a_info[a_idx];

    /* Artifact doesn't exist at all, we are in wizard mode, or not created yet */
    if ((a_ptr->tval + a_ptr->sval) == 0) return FALSE;
    if (p_ptr->is_wizard) return TRUE;
    if (a_ptr->a_cur_num == 0) return FALSE;

    /* Check all objects to see if it exists but hasn't been IDed */
    for (i = 0; i < z_info->o_max; i++)
    {
        int a = o_list[i].art_num;

        if (!a) continue;
        if (a != a_idx) continue;


        /* If we haven't actually identified the artifact yet */
        object_type *o_ptr = &o_list[i];
        if (o_ptr->is_known()) continue;

        return FALSE;
    }

    /* Check inventory for the same */
    for (i = 0; i < INVEN_TOTAL; i++)
    {
        object_type *o_ptr = &inventory[i];

        /* Ignore non-objects */
        if (!o_ptr->k_idx) continue;

        if ((o_ptr->art_num == a_idx) && !o_ptr->is_known())
        {
            return FALSE;
        }
    }

    /* Check guild to see if it is waiting as a quest reward */
    for (i = 0; i < st_ptr->stock_num; i++)
    {
        object_type *o_ptr = &st_ptr->stock[i];

        if (o_ptr->art_num == a_idx) return (FALSE);
    }

    return TRUE;
}

int DisplayObjectKnowledge::object_matches_group(int k_idx)
{

    int tval = k_info[k_idx].tval;

    // Hack - group ammunition together
    if ((tval == TV_BOLT) || (tval == TV_SHOT)) tval = TV_ARROW;

    for (int i = 0; i < NUM_OBJECT_GROUPS; i++)
    {
        object_grouper *group_ptr = &object_text_order[i];

        if (tval == group_ptr->tval) return (i);
    }

    // Shouldn't happen, but it must be handled
    return (NUM_OBJECT_GROUPS - 1);
}

// Display the object info
void DisplayObjectKnowledge::button_press(int k_idx)
{
    object_kind *k_ptr = &k_info[k_idx];
    object_type object_type_body;
    object_type *o_ptr = &object_type_body;

    /* Check for known artifacts, display them as artifacts */
    if (k_ptr->k_flags3 & (TR3_INSTA_ART))
    {
        // DO the insta_art check first
        if (artifact_is_known(get_artifact_from_kind(k_ptr)))
        {
            desc_art_fake(get_artifact_from_kind(k_ptr));
            return;
        }
    }

    make_object_fake(o_ptr, k_idx);

    object_info_screen(o_ptr);
}

// Display the object kind settings
void DisplayObjectKnowledge::settings_press(int k_idx)
{
    object_kind_settings(k_idx);

    // Find the squelch setting label and update it.
    for (int i = 0; i < object_table->rowCount(); i++)
    {
        QString text_idx = this->object_table->item(i, 5)->text();
        int row_idx = text_idx.toInt();

        if (row_idx != k_idx) continue;

        // Update the label
        object_kind *k_ptr = &k_info[k_idx];
        QString squelch_st = QString(squelch_status[k_ptr->squelch]);
        this->object_table->item(i, 2)->setText(squelch_st);
        this->object_table->item(i, 2)->setTextColor(squelch_status_color[k_ptr->squelch]);

    }
}

void DisplayObjectKnowledge::filter_rows(int row, int col)
{
    int which_group = 0;

    (void)col;
    int i;

    // First find the group we want to filter for
    for (i = 0; i < object_group_info.size(); i++)
    {

        if (!object_group_info[i]) continue;
        if (which_group == row) break;
        which_group++;
    }

    //Remember the group
    which_group = i;

    // Go through and hide all the rows where the object doesn't meet the criteria
    for (i = 0; i < object_table->rowCount(); i++)
    {
        QString text_idx = this->object_table->item(i, 5)->text();
        int k_idx = text_idx.toInt();

        if (object_matches_group(k_idx) == which_group)
        {
            object_table->showRow(i);
        }
        else object_table->hideRow(i);
    }


}


// Set up the object knowledge table

DisplayObjectKnowledge::DisplayObjectKnowledge(void)
{
    object_proxy_model = new QSortFilterProxyModel;
    object_proxy_model->setSortCaseSensitivity(Qt::CaseSensitive);
    QVBoxLayout *main_layout = new QVBoxLayout;
    QHBoxLayout *object_knowledge_hlay = new QHBoxLayout;
    main_layout->addLayout(object_knowledge_hlay);

    // To track the object kind info button
    object_button_group = new QButtonGroup;
    object_button_group->setExclusive(FALSE);

    // To track the object settings info button
    object_settings_group = new QButtonGroup;
    object_button_group->setExclusive(FALSE);

    // Set the table and headers
    object_group_table = new QTableWidget(0, 1, this);
    object_group_table->setAlternatingRowColors(FALSE);

    QTableWidgetItem *object_group_header = new QTableWidgetItem("Object Kinds");
    object_group_header->setTextAlignment(Qt::AlignLeft);
    object_group_table->setHorizontalHeaderItem(0, object_group_header);

    object_table = new QTableWidget(0, 6, this);
    object_table->setAlternatingRowColors(FALSE);

    do_spoiler = FALSE;

    int row = 0;
    int col = 0;

    QTableWidgetItem *obj_header = new QTableWidgetItem("Object");
    obj_header->setTextAlignment(Qt::AlignLeft);
    object_table->setHorizontalHeaderItem(col++, obj_header);
    QTableWidgetItem *symbol_header = new QTableWidgetItem("Symbol");
    symbol_header->setTextAlignment(Qt::AlignCenter);
    object_table->setHorizontalHeaderItem(col++, symbol_header);
    QTableWidgetItem *squelch_header = new QTableWidgetItem("Squelch");
    squelch_header->setTextAlignment(Qt::AlignLeft);
    object_table->setHorizontalHeaderItem(col++, squelch_header);
    QTableWidgetItem *info_header = new QTableWidgetItem("Info");
    info_header->setTextAlignment(Qt::AlignCenter);
    object_table->setHorizontalHeaderItem(col++, info_header);
    QTableWidgetItem *settings_header = new QTableWidgetItem("settings");
    settings_header->setTextAlignment(Qt::AlignCenter);
    object_table->setHorizontalHeaderItem(col++, settings_header);
    //This column will be hidden, but is used in filter_rows
    QTableWidgetItem *k_idx_header = new QTableWidgetItem("k_idx");
    k_idx_header->setTextAlignment(Qt::AlignCenter);
    object_table->setHorizontalHeaderItem(col++, k_idx_header);

    //Gather information to populate the object kind groups
    object_group_info.clear();
    for (int x = 0; x < NUM_OBJECT_GROUPS; x++) object_group_info.append(FALSE);

    //  Populate the table
    for (int i = 1; i < z_info->k_max; i++)
    {
        object_kind *k_ptr = &k_info[i];

        /* Skip "empty" and unknown objects, and gold */
        if (!k_ptr->k_name.length()) continue;
        if (k_ptr->tval == TV_GOLD) continue;
        if (!k_ptr->everseen || !k_ptr->aware) continue;

        /* Skip items with no distribution (including special artifacts) */
        int k = 0;
        /* Scan allocation pairs */
        for (int j = 0; j < 4; j++)
        {
            /*add the rarity, if there is one*/
            k += k_ptr->chance[j];
        }
        /*not in allocation table*/
        if (!k)  continue;

        object_table->insertRow(row);
        col = 0;

        // Object_kind
        QString this_object = capitalize_first(k_ptr->k_name);
        QTableWidgetItem *kind = new QTableWidgetItem(this_object);
        kind->setTextAlignment(Qt::AlignLeft);
        object_table->setItem(row, col++, kind);

        // Symbol (or tile if tiles are used)
        QString obj_symbol = (QString("'%1'") .arg(k_ptr->d_char));
        QTableWidgetItem *kind_ltr = new QTableWidgetItem(obj_symbol);
        if (use_graphics)
        {
            QPixmap pix = ui_get_tile(k_ptr->tile_id);
            pix = pix.scaled(32, 32);
            kind_ltr->setIcon(pix);
        }
        else kind_ltr->setTextColor(k_ptr->d_color);
        kind_ltr->setTextAlignment(Qt::AlignCenter);
        object_table->setItem(row, col++, kind_ltr);

        // Squelch status
        QString squelch_st = QString(squelch_status[k_ptr->squelch]);
        QTableWidgetItem *squelch = new QTableWidgetItem(squelch_st);
        squelch->setTextColor(squelch_status_color[k_ptr->squelch]);
        squelch->setTextAlignment(Qt::AlignLeft);
        object_table->setItem(row, col++, squelch);

        // object info
        QPushButton *info_button = new QPushButton();
        info_button->setIcon(QIcon(":/icons/lib/icons/help.png"));
        info_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        object_table->setCellWidget(row, col++, info_button);
        object_button_group->addButton(info_button, i);

        // object settings
        QPushButton *settings_button = new QPushButton();
        settings_button->setIcon(QIcon(":/icons/lib/icons/settings.png"));
        settings_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        object_table->setCellWidget(row, col++, settings_button);
        object_settings_group->addButton(settings_button, i);

        // k_idx
        QString this_k_idx = (QString("%1") .arg(i));
        QTableWidgetItem *k_idx = new QTableWidgetItem(this_k_idx);
        k_idx->setTextAlignment(Qt::AlignRight);
        object_table->setItem(row, col++, k_idx);

        row++;

        // Now make sure the feature type is added to the table.
        object_group_info[object_matches_group(i)] = TRUE;
    }

    connect(object_button_group, SIGNAL(buttonClicked(int)), this, SLOT(button_press(int)));
    connect(object_settings_group, SIGNAL(buttonClicked(int)), this, SLOT(settings_press(int)));

    row = col = 0;

    //Now populate the object_group table
    for (int i = 0; i < object_group_info.size(); i++)
    {
        if (!object_group_info[i]) continue;
        object_group_table->insertRow(row);

        // Object Group
        QString group_name = QString(object_text_order[i].name);
        QTableWidgetItem *obj_group_label = new QTableWidgetItem(group_name);
        obj_group_label->setTextAlignment(Qt::AlignLeft);
        object_group_table->setItem(row++, 0, obj_group_label);
    }

    object_group_table->resizeColumnsToContents();
    object_group_table->resizeRowsToContents();
    object_group_table->setSortingEnabled(FALSE);
    object_group_table->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    object_group_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(object_group_table, SIGNAL(cellClicked(int,int)), this, SLOT(filter_rows(int, int)));
    object_knowledge_hlay->addWidget(object_group_table);

    object_table->setSortingEnabled(TRUE);
    object_table->resizeColumnsToContents();
    object_table->resizeRowsToContents();
    object_table->sortByColumn(0, Qt::AscendingOrder);
    // Hide the r_idx column
    object_table->setColumnHidden(5, TRUE);
    object_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    object_table->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    object_knowledge_hlay->addWidget(object_table);

    //Add a close button on the right side
    QHBoxLayout *close_across = new QHBoxLayout;
    main_layout->addLayout(close_across);
    close_across->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, SIGNAL(rejected()), this, SLOT(close()));
    close_across->addWidget(buttons);

    //Filter for the first monster group.
    filter_rows(0,0);

    setLayout(main_layout);
    setWindowTitle(tr("Object Knowledge"));

    this->exec();
}

void display_object_knowledge(void)
{
    DisplayObjectKnowledge();
}




void display_ego_item_knowledge(void)
{

}

void display_artifact_knowledge(void)
{

}
