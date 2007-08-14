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
// C++ Implementation: cpp
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "order_and_defaults_editor.h"
//Added by qt3to4:
#include <QLabel>
#include <Q3Frame>

Order_And_Defaults_Editor::Defaults::Entry::Entry(QWidget *p_parent) : Q3HBox(p_parent) {

	Q3Frame *auxframe;
	channel = new QLabel(this);

	auxframe=new Q3Frame(this);
	auxframe->setFrameStyle(Panel|Sunken);

	pan = new Q_Property_Bridge_Int(this);
	pan->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));

	surround = new Q_Property_Bridge_Bool(this);

	auxframe=new Q3Frame(this);
	auxframe->setFrameStyle(Panel|Sunken);

	volume = new Q_Property_Bridge_Int(this);
	volume->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));

	mute = new Q_Property_Bridge_Bool(this);
};

void Order_And_Defaults_Editor::set_song(Song *p_song) {


	song=p_song;
	for (int i=0;i<64;i++) {


		defaults.entry_list[i]->pan->set_bridge(&song->initial_variables.channel[i].prop_pan);
		defaults.entry_list[i]->surround->set_bridge(&song->initial_variables.channel[i].prop_surround);
		defaults.entry_list[i]->volume->set_bridge(&song->initial_variables.channel[i].prop_volume);
		defaults.entry_list[i]->mute->set_bridge(&song->initial_variables.channel[i].prop_mute);
	}


}
void Order_And_Defaults_Editor::set_editor(Editor *p_rditor) {

	editor=p_rditor;
	order.orderlist->link_to_editor(editor);
}

void Order_And_Defaults_Editor::timer_callback() {

	timer_counter++;

	if ((timer_counter%50)==0) {

		for (int i=0;i<(int)defaults.entry_list.size();i++) {
			//this will update
			defaults.entry_list[i]->mute->set_bridge(&song->initial_variables.channel[i].prop_mute);

		}
	}

}


Order_And_Defaults_Editor::Order_And_Defaults_Editor(QWidget *p_parent) : Q3HBox(p_parent) {

	string channel="Channel ";

        order.group = new Q3GroupBox(1,Qt::Vertical,"List:",this);
	order.group->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding));
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	order.orderlist = new OrderList_Edit(order.group);
	order.orderlist->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

//	defaults.scroll = new Vertical_Scrolled_Window(this);
	defaults.group = new Q3GroupBox(1,Qt::Vertical,"Initial channel defaults:",this);
	defaults.scroll = new Q3ScrollView(defaults.group);
	defaults.scroll->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	defaults.scroll->setResizePolicy(Q3ScrollView::AutoOneFit);

	defaults.scroll->enableClipper(true);
	defaults.scrolled_box = new Q3VBox(defaults.scroll->viewport());
	defaults.scroll->addChild(defaults.scrolled_box);

//	defaults.scroll->set_child(defaults.scrolled_box);
//	defaults.scrolled_box = new QVBox(this);
	defaults.scrolled_box->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));


	for (int i=0;i<64;i++) { //tracker hardcoded world

		char channelnum[3]={0,0,0};
		channelnum[0]='0'+(i+1)/10;
		channelnum[1]='0'+(i+1)%10;
		string channelname=channel+channelnum;

		Defaults::Entry *new_entry = new Defaults::Entry(defaults.scrolled_box);
		new_entry->channel->setText(channelname.c_str());

		defaults.entry_list.push_back(new_entry);

	}

	timer_counter=0;

}

Order_And_Defaults_Editor::~Order_And_Defaults_Editor()
{
}
