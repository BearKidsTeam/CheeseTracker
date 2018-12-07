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

#ifndef CCOLOR_LIST_H
#define CCOLOR_LIST_H

#include "components/data/config_handler.h"
#include <q3scrollview.h>
#include <q3vbox.h>
#include "interface__QT/helpers/ccolor_bridge.h"
#include <vector>
#include <map>
	using std::vector;
	using std::map;
#include <string>
	using std::string;

#include <qcombobox.h>
/**
 *
 * Juan Linietsky
 **/
class CColor_List : public Q3VBox
{
	Q_OBJECT
public:

	class Preset {

		struct Data {

			QColor color;
		};
		map<string,Data> data_list;
        public:
		string name;
		void add_item(string p_section,string p_name,const QColor &p_color);
		const QColor *get_item(string p_section,string p_name);
	};


private:
        Q3ScrollView *scroll;

        struct Color_Data {
		CColor_Bridge *bridge;
		QString name;
		QString section;
	};

	vector<Color_Data> color_list;

	Q3VBox *vbox;
	QString current_section;

	QComboBox *preset_combo;
	vector<Preset*> preset_list;
public slots:

	void preset_selected_cbk(int p_which);

public:

        void load_settings(ConfigHandler *p_config_handler);
        void save_settings(ConfigHandler *p_config_handler);


	void add_preset(Preset* p_preset);
	void set_section(QString p_section_name,QString p_section_label);

	void register_color(QColor *p_color,const QString &p_name,const QString &p_label);

	CColor_List(QWidget *p_parent);
	~CColor_List();
};

#endif
