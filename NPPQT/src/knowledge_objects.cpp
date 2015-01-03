
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
#define NUM_ARTIFACT_GROUPS (NUM_OBJECT_GROUPS + 1)

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
    {TV_BOLT,			"Bolts"			}, // for objects, moved to "Ammunition group".  See object_matched_group.
    {TV_SHOT,			"Shots"			}, // for objects, moved to "Ammunition group"   See object_matched_group.
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

    if (o_ptr->ego_num)
    {
        ego_item_type *ego_ptr = &e_info[o_ptr->ego_num];
        if (ego_ptr->max_to_a)
        {
            if (ego_ptr->max_to_a > 0) o_ptr->to_a = 1;
            else o_ptr->to_a = -1;
        }
        if (ego_ptr->max_to_h)
        {
            if (ego_ptr->max_to_h > 0) o_ptr->to_h = 1;
            else o_ptr->to_h = -1;
        }
        if (ego_ptr->max_to_d)
        {
            if (ego_ptr->max_to_d > 0) o_ptr->to_d = 1;
            else o_ptr->to_d = -1;
        }
        if (ego_ptr->max_pval)
        {
            if (ego_ptr->max_pval > 0) o_ptr->pval = 1;
            else o_ptr->pval = -1;
        }

        if (ego_ptr->e_flags3 & (TR3_LIGHT_CURSE)) o_ptr->ident |= (IDENT_CURSED);
        if (ego_ptr->e_flags3 & (TR3_HEAVY_CURSE)) o_ptr->ident |= (IDENT_CURSED);
        if (ego_ptr->e_flags3 & (TR3_PERMA_CURSE)) o_ptr->ident |= (IDENT_CURSED);
    }

    o_ptr->update_object_flags();
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

static int find_first_ego_match(int e_idx)
{
    /* Get the actual kind */
    ego_item_type *e_ptr = &e_info[e_idx];

    for (int i = 1; i < z_info->k_max; i++)
    {
        object_kind *k_ptr = &k_info[i];

        if (!k_ptr->k_name.length()) continue;

        /* Test if this is a legal ego-item type for this object */
        for (int j = 0; j < EGO_TVALS_MAX; j++)
        {
            /* Require identical base type */
            if (k_ptr->tval != e_ptr->tval[j]) continue;

            /* Require sval in bounds, lower */
            if (k_ptr->sval < e_ptr->min_sval[j]) continue;

            /* Require sval in bounds, upper */
            if (k_ptr->sval > e_ptr->max_sval[j]) continue;

            return (i);
        }
    }

    // Shouldn't every happpen
    return (0);
}

/*
 * Describe fake ego item "lore"
 */
static void desc_ego_fake(int ego_num, QString object_string)
{
    /* Hack: dereference the join */
    QString xtra[10] = { "sustains", "higher resistances", "abilities", "immunities", "stat increases",
                            "slays", "*slays*", "elemental brands", "elemental resists", "native abilities"};

    ego_item_type *e_ptr = &e_info[ego_num];

    object_type dummy;
    object_type *o_ptr = &dummy;

        /* List ego flags */
    int k_idx = find_first_ego_match(ego_num);
    if (!k_idx) return;

    make_object_fake(o_ptr, k_idx);
    o_ptr->ego_num = ego_num;
    apply_magic_fake(o_ptr);
    o_ptr->xtra2 = 0;
    o_ptr->update_object_flags();

    QString output = color_string(QString("%1 %2<br>") .arg(object_string) .arg(e_ptr->e_name), TERM_BLUE);

    if (e_ptr->e_text.length())
    {
        output.append(QString("<br>%1<br>") .arg(e_ptr->e_text));
    }

    output.append(object_info_out(o_ptr, FALSE));

    if (e_ptr->xtra)
    {
        output.append(QString("<br>It provides one or more random %1.<br>") .arg(xtra[e_ptr->xtra - 1]));
    }

    if (e_ptr->e_flags3 & (TR3_PERMA_CURSE)) output.append("It is permanently cursed.");
    else if (e_ptr->e_flags3 & (TR3_HEAVY_CURSE)) output.append("It is heavily cursed.");
    if (e_ptr->e_flags3 & (TR3_LIGHT_CURSE)) output.append("It is cursed.");

    /* Finally, display it */
    display_info_window(DISPLAY_INFO_OBJECT, o_ptr->k_idx, output, o_ptr);
}

static QString get_artifact_display_name(int a_idx)
{
    object_type object_type_body;
    object_type *o_ptr = &object_type_body;

    /* Make fake artifact */
    o_ptr = &object_type_body;
    make_fake_artifact(o_ptr, a_idx);
    object_aware(o_ptr);
    object_known(o_ptr);
    o_ptr->ident |= (IDENT_MENTAL);

    /* Get its name */
    return (object_desc(o_ptr, ODESC_PREFIX | ODESC_BASE | ODESC_SPOIL));
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
    object_type object_type_body;
    object_type *o_ptr = &object_type_body;

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


// Set up the object knowledge table.
// Note special artifacts are handled under DisplayArtifactKnowledge.
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
        // Handled in artifact knowledge
        if (k_ptr->k_flags3 & (TR3_INSTA_ART)) continue;

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

        // Now make sure the object type is added to the table.
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
    QDialogButtonBox buttons;
    buttons.setStandardButtons(QDialogButtonBox::Close);
    connect(&buttons, SIGNAL(rejected()), this, SLOT(close()));
    main_layout->addWidget(&buttons);

    //Filter for the first monster group.
    filter_rows(0,0);

    setLayout(main_layout);
    setWindowTitle(tr("Object Knowledge"));

    this->exec();
}

void display_object_knowledge(void)
{
    // Paranoia
    if (!p_ptr->playing) return;

    DisplayObjectKnowledge();
}

// Assumes "everseen" has been handled
bool DisplayEgoItemKnowledge::ego_item_matches_group(int e_idx, int group)
{
    ego_item_type *ego_ptr = &e_info[e_idx];

    object_grouper *group_ptr = &object_text_order[group];

    for (int x = 0; x < EGO_TVALS_MAX; x++)
    {
        if (ego_ptr->tval[x] == group_ptr->tval) return (TRUE);
    }

    // No match found
    return (FALSE);
}

// Display the object info
void DisplayEgoItemKnowledge::button_press(int e_idx)
{
    int row = ego_item_group_table->currentRow();
    QString group_text = this->ego_item_group_table->item(row, 0)->text();

    desc_ego_fake(e_idx, group_text);
}

// Display the object kind settings
void DisplayEgoItemKnowledge::settings_press(int e_idx)
{
    ego_item_type *ego_ptr = &e_info[e_idx];

    // Find the squelch setting label and update it.
    for (int i = 0; i < ego_item_table->rowCount(); i++)
    {
        QString text_idx = this->ego_item_table->item(i, 4)->text();
        int row_idx = text_idx.toInt();

        if (row_idx != e_idx) continue;

        // Toggle it
        ego_ptr->squelch = !ego_ptr->squelch;

        // Update the label
        QString squelch_st = QString("TRUE");
        if (!ego_ptr->squelch) squelch_st = QString("FALSE");
        this->ego_item_table->item(i, 1)->setText(squelch_st);
    }
}

void DisplayEgoItemKnowledge::filter_rows(int row, int col)
{
    int which_group = 0;

    (void)col;
    int i;

    // First find the group we want to filter for
    for (i = 0; i < ego_item_group_info.size(); i++)
    {

        if (!ego_item_group_info[i]) continue;
        if (which_group == row) break;
        which_group++;
    }

    //Remember the group
    which_group = i;

    // Go through and hide all the rows where the object doesn't meet the criteria
    for (i = 0; i < ego_item_table->rowCount(); i++)
    {
        QString text_idx = this->ego_item_table->item(i, 4)->text();
        int e_idx = text_idx.toInt();

        if (ego_item_matches_group(e_idx, which_group))
        {
            ego_item_table->showRow(i);
        }
        else ego_item_table->hideRow(i);
    }
}


// Set up the object knowledge table

DisplayEgoItemKnowledge::DisplayEgoItemKnowledge(void)
{
    ego_item_proxy_model = new QSortFilterProxyModel;
    ego_item_proxy_model->setSortCaseSensitivity(Qt::CaseSensitive);
    QVBoxLayout *main_layout = new QVBoxLayout;
    QHBoxLayout *ego_item_knowledge_hlay = new QHBoxLayout;
    main_layout->addLayout(ego_item_knowledge_hlay);

    // To track the ego_item info button
    ego_item_button_group = new QButtonGroup;
    ego_item_button_group->setExclusive(FALSE);

    // To track the ego_item squelch toggle button
    ego_item_squelch_toggle = new QButtonGroup;
    ego_item_squelch_toggle->setExclusive(FALSE);

    // Set the table and headers
    ego_item_group_table = new QTableWidget(0, 1, this);
    ego_item_group_table->setAlternatingRowColors(FALSE);

    QTableWidgetItem *ego_item_group_header = new QTableWidgetItem("Object Kinds");
    ego_item_group_header->setTextAlignment(Qt::AlignLeft);
    ego_item_group_table->setHorizontalHeaderItem(0, ego_item_group_header);

    ego_item_table = new QTableWidget(0, 5, this);
    ego_item_table->setAlternatingRowColors(FALSE);

    do_spoiler = FALSE;

    int row = 0;
    int col = 0;

    QTableWidgetItem *obj_header = new QTableWidgetItem("Ego Item");
    obj_header->setTextAlignment(Qt::AlignLeft);
    ego_item_table->setHorizontalHeaderItem(col++, obj_header);
    QTableWidgetItem *squelch_header = new QTableWidgetItem("Squelch Setting");
    squelch_header->setTextAlignment(Qt::AlignLeft);
    ego_item_table->setHorizontalHeaderItem(col++, squelch_header);
    QTableWidgetItem *info_header = new QTableWidgetItem("Info");
    info_header->setTextAlignment(Qt::AlignCenter);
    ego_item_table->setHorizontalHeaderItem(col++, info_header);
    QTableWidgetItem *toggle_squech_header = new QTableWidgetItem("Toggle Squelch");
    toggle_squech_header->setTextAlignment(Qt::AlignCenter);
    ego_item_table->setHorizontalHeaderItem(col++, toggle_squech_header);
    //This column will be hidden, but is used in filter_rows
    QTableWidgetItem *e_idx_header = new QTableWidgetItem("e_idx");
    e_idx_header->setTextAlignment(Qt::AlignCenter);
    ego_item_table->setHorizontalHeaderItem(col++, e_idx_header);

    //Gather information to populate the object kind groups
    ego_item_group_info.clear();
    for (int x = 0; x < NUM_OBJECT_GROUPS; x++) ego_item_group_info.append(FALSE);

    //  Populate the table
    for (int i = 1; i < z_info->e_max; i++)
    {
        ego_item_type *e_ptr = &e_info[i];

        /* Skip "empty" and unknown objects, and gold */
        if (!e_ptr->e_name.length()) continue;
        if (!e_ptr->everseen) continue;

        ego_item_table->insertRow(row);
        col = 0;

        // Object_kind
        QString this_object = capitalize_first(e_ptr->e_name);
        QTableWidgetItem *kind = new QTableWidgetItem(this_object);
        kind->setTextAlignment(Qt::AlignLeft);
        ego_item_table->setItem(row, col++, kind);

        // Squelch status
        QString squelch_st = QString("FALSE");
        if (e_ptr->squelch) squelch_st = QString("TRUE");
        QTableWidgetItem *squelch = new QTableWidgetItem(squelch_st);
        squelch->setTextAlignment(Qt::AlignLeft);
        ego_item_table->setItem(row, col++, squelch);

        // object info
        QPushButton *info_button = new QPushButton();
        info_button->setIcon(QIcon(":/icons/lib/icons/help.png"));
        info_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        ego_item_table->setCellWidget(row, col++, info_button);
        ego_item_button_group->addButton(info_button, i);

        // object settings
        QPushButton *settings_button = new QPushButton();
        settings_button->setIcon(QIcon(":/icons/lib/icons/settings.png"));
        settings_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        settings_button->setStatusTip("Toggle Squelch Status");
        ego_item_table->setCellWidget(row, col++, settings_button);
        ego_item_squelch_toggle->addButton(settings_button, i);

        // e_idx
        QString this_e_idx = (QString("%1") .arg(i));
        QTableWidgetItem *e_idx = new QTableWidgetItem(this_e_idx);
        e_idx->setTextAlignment(Qt::AlignRight);
        ego_item_table->setItem(row, col++, e_idx);

        row++;

        // Now make sure the ego_item group is added to the table.
        for (int x = 0; x < ego_item_group_info.size(); x++)
        {
            if (ego_item_matches_group(i, x)) ego_item_group_info[x] = TRUE;
        }
    }

    if (!ego_item_table->rowCount())
    {
        pop_up_message_box("You are not currently aware of any ego items");
        return;
    }

    connect(ego_item_button_group, SIGNAL(buttonClicked(int)), this, SLOT(button_press(int)));
    connect(ego_item_squelch_toggle, SIGNAL(buttonClicked(int)), this, SLOT(settings_press(int)));

    row = col = 0;

    //Now populate the ego_item_group table
    for (int i = 0; i < ego_item_group_info.size(); i++)
    {
        if (!ego_item_group_info[i]) continue;
        ego_item_group_table->insertRow(row);

        // Object Group
        QString group_name = QString(object_text_order[i].name);
        // Hack - work in "Arrow"
        if (object_text_order[i].tval == TV_ARROW) group_name = QString("Arrows");
        QTableWidgetItem *ego_group_label = new QTableWidgetItem(group_name);
        ego_group_label->setTextAlignment(Qt::AlignLeft);
        ego_item_group_table->setItem(row++, 0, ego_group_label);
    }

    ego_item_group_table->resizeColumnsToContents();
    ego_item_group_table->resizeRowsToContents();
    ego_item_group_table->setSortingEnabled(FALSE);
    ego_item_group_table->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    ego_item_group_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ego_item_group_table, SIGNAL(cellClicked(int,int)), this, SLOT(filter_rows(int, int)));
    ego_item_knowledge_hlay->addWidget(ego_item_group_table);

    ego_item_table->setSortingEnabled(TRUE);
    ego_item_table->resizeColumnsToContents();
    ego_item_table->resizeRowsToContents();
    ego_item_table->sortByColumn(0, Qt::AscendingOrder);
    // Hide the e_idx column
    ego_item_table->setColumnHidden(4, TRUE);
    ego_item_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ego_item_table->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    ego_item_knowledge_hlay->addWidget(ego_item_table);

    //Add a close button on the right side
    QDialogButtonBox buttons;
    buttons.setStandardButtons(QDialogButtonBox::Close);
    connect(&buttons, SIGNAL(rejected()), this, SLOT(close()));
    main_layout->addWidget(&buttons);

    //Filter for the first monster group.
    filter_rows(0,0);

    setLayout(main_layout);
    setWindowTitle(tr("Ego Item Knowledge"));

    this->exec();
}


void display_ego_item_knowledge(void)
{
    // Paranoia
    if (!p_ptr->playing) return;

    DisplayEgoItemKnowledge();
}



int DisplayArtifactKnowledge::artifact_matches_group(int a_idx)
{

    int tval = a_info[a_idx].tval;

    // Hack - Insta-art group is first.  Note i+1 is returned
    if (a_info[a_idx].is_special_artifact()) return (0);

    for (int i = 0; i < NUM_OBJECT_GROUPS; i++)
    {
        object_grouper *group_ptr = &object_text_order[i];

        if (tval == group_ptr->tval) return (i + 1);
    }

    // Shouldn't happen, but it must be handled
    return (NUM_OBJECT_GROUPS);
}

// Display the object info
void DisplayArtifactKnowledge::button_press(int a_idx)
{

    desc_art_fake(a_idx);
}

// Display the object kind settings
void DisplayArtifactKnowledge::settings_press(int a_idx)
{
    object_artifact_settings(a_idx);
}


void DisplayArtifactKnowledge::filter_rows(int row, int col)
{
    int which_group = 0;

    (void)col;
    int i;

    // First find the group we want to filter for
    for (i = 0; i < artifact_group_info.size(); i++)
    {
        if (!artifact_group_info[i]) continue;
        if (which_group == row) break;
        which_group++;
    }

    //Remember the group
    which_group = i;

    // Go through and hide all the rows where the object doesn't meet the criteria
    for (i = 0; i < artifact_table->rowCount(); i++)
    {
        QString text_idx = this->artifact_table->item(i, 3)->text();
        int a_idx = text_idx.toInt();

        if (artifact_matches_group(a_idx) == which_group)
        {
            artifact_table->showRow(i);
        }
        else artifact_table->hideRow(i);
    }
}


// Set up the artifact knowledge table
// Note the code handling creating a group for special artifacts
DisplayArtifactKnowledge::DisplayArtifactKnowledge(void)
{
    artifact_proxy_model = new QSortFilterProxyModel;
    artifact_proxy_model->setSortCaseSensitivity(Qt::CaseSensitive);
    QVBoxLayout *main_layout = new QVBoxLayout;
    QHBoxLayout *artifact_knowledge_hlay = new QHBoxLayout;
    main_layout->addLayout(artifact_knowledge_hlay);

    // To track the artifact info button
    artifact_button_group = new QButtonGroup;
    artifact_button_group->setExclusive(FALSE);

    // To track the artifact settings button
    artifact_settings_group = new QButtonGroup;
    artifact_settings_group->setExclusive(FALSE);

    // Set the table and headers
    artifact_group_table = new QTableWidget(0, 1, this);
    artifact_group_table->setAlternatingRowColors(FALSE);

    QTableWidgetItem *artifact_group_header = new QTableWidgetItem("Object Kinds");
    artifact_group_header->setTextAlignment(Qt::AlignLeft);
    artifact_group_table->setHorizontalHeaderItem(0, artifact_group_header);

    artifact_table = new QTableWidget(0, 4, this);
    artifact_table->setAlternatingRowColors(FALSE);

    do_spoiler = FALSE;

    int row = 0;
    int col = 0;

    QTableWidgetItem *art_header = new QTableWidgetItem("Object");
    art_header->setTextAlignment(Qt::AlignLeft);
    artifact_table->setHorizontalHeaderItem(col++, art_header);
    QTableWidgetItem *info_header = new QTableWidgetItem("Info");
    info_header->setTextAlignment(Qt::AlignCenter);
    artifact_table->setHorizontalHeaderItem(col++, info_header);
    QTableWidgetItem *settings_header = new QTableWidgetItem("settings");
    settings_header->setTextAlignment(Qt::AlignCenter);
    artifact_table->setHorizontalHeaderItem(col++, settings_header);
    //This column will be hidden, but is used in filter_rows
    QTableWidgetItem *a_idx_header = new QTableWidgetItem("s_idx");
    a_idx_header->setTextAlignment(Qt::AlignCenter);
    artifact_table->setHorizontalHeaderItem(col++, a_idx_header);

    //Gather information to populate the object kind groups
    // First, add one for the special artifacts
    artifact_group_info.clear();
    artifact_group_info.append(FALSE);
    for (int x = 0; x < NUM_OBJECT_GROUPS; x++) artifact_group_info.append(FALSE);

    //  Populate the table
    for (int i = 0; i < z_info->art_rand_max; i++)
    {
        artifact_type *a_ptr = &a_info[i];

        /* This slot is already being used */
        if ((a_ptr->tval + a_ptr->sval) == 0) continue;
        if (!artifact_is_known(i)) continue;

        /*  Don't do quest artifacts*/
        if (i == QUEST_ART_SLOT) continue;

        artifact_table->insertRow(row);
        col = 0;

        // Object_kind
        QString this_art = get_artifact_display_name(i);
        QTableWidgetItem *art_kind = new QTableWidgetItem(this_art);
        art_kind->setTextAlignment(Qt::AlignLeft);
        artifact_table->setItem(row, col++, art_kind);

        // object info
        QPushButton *info_button = new QPushButton();
        info_button->setIcon(QIcon(":/icons/lib/icons/help.png"));
        info_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        artifact_table->setCellWidget(row, col++, info_button);
        artifact_button_group->addButton(info_button, i);

        // object settings
        QPushButton *settings_button = new QPushButton();
        settings_button->setIcon(QIcon(":/icons/lib/icons/settings.png"));
        settings_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        artifact_table->setCellWidget(row, col++, settings_button);
        artifact_settings_group->addButton(settings_button, i);

        // a_idx
        QString this_a_idx = (QString("%1") .arg(i));
        QTableWidgetItem *a_idx = new QTableWidgetItem(this_a_idx);
        a_idx->setTextAlignment(Qt::AlignRight);
        artifact_table->setItem(row, col++, a_idx);

        row++;

        // Now make sure the object type is added to the table.
        artifact_group_info[artifact_matches_group(i)] = TRUE;
    }

    if (!artifact_table->rowCount())
    {
        pop_up_message_box("You are not currently aware of any artifacts");
        return;
    }

    connect(artifact_button_group, SIGNAL(buttonClicked(int)), this, SLOT(button_press(int)));
    connect(artifact_settings_group, SIGNAL(buttonClicked(int)), this, SLOT(settings_press(int)));

    row = col = 0;

    //Now populate the object_group table
    for (int i = 0; i < artifact_group_info.size(); i++)
    {
        if (!artifact_group_info[i]) continue;
        artifact_group_table->insertRow(row);

        // Object Group
        QString group_name = QString("Special");
        if (i) group_name = QString(object_text_order[i-1].name);
        QTableWidgetItem *artifact_group_label = new QTableWidgetItem(group_name);
        artifact_group_label->setTextAlignment(Qt::AlignLeft);
        artifact_group_table->setItem(row++, 0, artifact_group_label);
    }

    artifact_group_table->resizeColumnsToContents();
    artifact_group_table->resizeRowsToContents();
    artifact_group_table->setSortingEnabled(FALSE);
    artifact_group_table->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    artifact_group_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(artifact_group_table, SIGNAL(cellClicked(int,int)), this, SLOT(filter_rows(int, int)));
    artifact_knowledge_hlay->addWidget(artifact_group_table);

    artifact_table->setSortingEnabled(TRUE);
    artifact_table->resizeColumnsToContents();
    artifact_table->resizeRowsToContents();
    artifact_table->sortByColumn(0, Qt::AscendingOrder);
    // Hide the r_idx column
    artifact_table->setColumnHidden(3, TRUE);
    artifact_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    artifact_table->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    artifact_knowledge_hlay->addWidget(artifact_table);

    //Add a close button on the right side
    QDialogButtonBox buttons;
    buttons.setStandardButtons(QDialogButtonBox::Close);
    connect(&buttons, SIGNAL(rejected()), this, SLOT(close()));
    main_layout->addWidget(&buttons);

    //Filter for the first monster group.
    filter_rows(0,0);

    setLayout(main_layout);
    setWindowTitle(tr("Object Knowledge"));

    this->exec();
}

void display_artifact_knowledge(void)
{
    // Paranoia
    if (!p_ptr->playing) return;

    DisplayArtifactKnowledge();
}
