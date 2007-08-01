/***************************************************************************
    This file is part of the CheeseTronic Music Tools
    url                  : http://reduz.com.ar/cheesetronic
    copyright            : (C) 2003 by Juan Linietsky
    email                : coding@reduz.com.ar
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ORDER_AND_DEFAULTS_EDITOR_H
#define ORDER_AND_DEFAULTS_EDITOR_H


#include "interface__QT/helpers/vertical_scrolled_window.h"
#include "interface__QT/helpers/property_bridge_edit.h"
#include <qhbox.h>
#include <qvbox.h>
#include <qgroupbox.h>
#include "interface__QT/orderlist_edit.h"
#include "trackercore/song.h"


/**
 *
 * Juan Linietsky
 **/
class Order_And_Defaults_Editor : public QHBox {

	Q_OBJECT


        struct Order {

		QGroupBox *group;
		OrderList_Edit *orderlist;

	} order;

	struct Defaults {

//		Vertical_Scrolled_Window * scroll;
		QGroupBox *group;
		QScrollView * scroll;
		QVBox *scrolled_box;

		struct Entry : public QHBox {
                        QLabel *channel;
			Q_Property_Bridge_Int * pan;
			Q_Property_Bridge_Bool * surround;
			Q_Property_Bridge_Int * volume;
			Q_Property_Bridge_Bool * mute;

			Entry(QWidget *p_parent);
		};

		vector<Entry*> entry_list;

	} defaults;


	Song *song;
	Editor *editor;
	int timer_counter;
public:
	OrderList_Edit *get_orderlist() { return order.orderlist; }


	void timer_callback();
	void set_song(Song *p_song);
	void set_editor(Editor *p_rditor);


	Order_And_Defaults_Editor(QWidget *p_parent);
	~Order_And_Defaults_Editor();
};

#endif
