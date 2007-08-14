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
//
//
// C++ Interface: property_bridge_edit_list
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

//
//
// C++ Implementation: property_bridge_edit_list
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//


#ifndef PROPERTY_BRIDGE_EDIT_LIST_H_
#define PROPERTY_BRIDGE_EDIT_LIST_H_

#include "components/data/property_bridges.h"
#include "interface__QT/helpers/property_bridge_edit.h"


#include <qtabwidget.h>
#include <q3scrollview.h>

#include <vector>
	using std::vector;
#include <list>
	using std::list;


class Property_Bridge_Edit_List : public Q3VBox {


	struct Section {

                Q3ScrollView *scroll;
		Q3VBox *vbox;
	};

	QTabWidget * tabs;

	vector<Section> sections;


	void create_section(string p_name, bool p_in_tabwidget=true);
	bool get_bridge(Property_Bridge *p_bridge,QWidget *p_parent);

public:

	void set_property_bridge_list(list<Property_Bridge*> p_list);
	void clear();
	Property_Bridge_Edit_List(QWidget *p_parent);
};

#endif

