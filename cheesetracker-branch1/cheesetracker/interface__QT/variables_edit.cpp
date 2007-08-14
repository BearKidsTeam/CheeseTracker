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
#include "variables_edit.h"
#include <qmessagebox.h>


void Variables_Edit::set_song(Song *p_song) {

	song=p_song;

	variables.vbox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding));

	variables.prop_tempo->set_bridge(&song->initial_variables.prop_tempo);
	variables.prop_speed->set_bridge(&song->initial_variables.prop_speed);

        variables.prop_hilight_minor->set_bridge(&song->variables.prop_hilight_minor);
        variables.prop_hilight_major->set_bridge(&song->variables.prop_hilight_major);

	variables.prop_global_volume->set_bridge(&song->initial_variables.prop_global_volume);

	variables.prop_mixing_volume->set_bridge(&song->variables.prop_mixing_volume);
	variables.prop_separation->set_bridge(&song->variables.prop_stereo_separation);

	variables.prop_old_effects->set_bridge(&song->variables.prop_old_effects);
	variables.prop_compatible_gxx->set_bridge(&song->variables.prop_compatible_gxx);

	variables.prop_stereo->set_bridge(&song->variables.prop_use_stereo);
//	variables.use_instruments = new QCheckBox(variables.compatibility_group);
	variables.prop_linear_slides->set_bridge(&song->variables.prop_use_linear_slides);

	variables.prop_name->set_bridge(&song->variables.prop_name);
	variables.prop_message->set_bridge(&song->variables.prop_message);

}


Variables_Edit::Variables_Edit(QWidget *p_parent) :  Q3HBox(p_parent)
{

	variables.vbox = new Q3VBox(this);
	variables.vbox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding));

	variables.speed_group = new Q3GroupBox(1, Qt::Horizontal,"Speed/Tempo:", variables.vbox);
	variables.prop_tempo = new Q_Property_Bridge_Int_CSpinButon(variables.speed_group);
	variables.prop_tempo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	variables.prop_speed = new Q_Property_Bridge_Int_CSpinButon(variables.speed_group);

	variables.hilight_group = new Q3GroupBox(1, Qt::Horizontal,"Row Hilight:", variables.vbox);

	variables.prop_hilight_minor = new Q_Property_Bridge_Int_CSpinButon(variables.hilight_group);
	variables.prop_hilight_major = new Q_Property_Bridge_Int_CSpinButon(variables.hilight_group);

	variables.modifiers_group = new Q3GroupBox(1, Qt::Horizontal,"Mixing:", variables.vbox);
	variables.modifiers_group->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	variables.prop_global_volume = new Q_Property_Bridge_Int_CSpinButon(variables.modifiers_group);
	variables.prop_mixing_volume = new Q_Property_Bridge_Int_CSpinButon(variables.modifiers_group);
	variables.prop_separation = new Q_Property_Bridge_Int_CSpinButon(variables.modifiers_group);

	variables.compatibility_group = new Q3GroupBox(1, Qt::Horizontal,"Compatibility:", variables.vbox);
	variables.compatibility_group->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	variables.prop_old_effects = new Q_Property_Bridge_Bool(variables.compatibility_group);
	variables.prop_compatible_gxx = new Q_Property_Bridge_Bool(variables.compatibility_group);

	variables.prop_stereo = new Q_Property_Bridge_Bool(variables.compatibility_group);

	variables.prop_linear_slides = new Q_Property_Bridge_Bool(variables.compatibility_group);

	variables.message_group = new Q3GroupBox(1, Qt::Horizontal,"Song Name & Comments:", this);
	variables.message_group->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	variables.prop_name = new Q_Property_Bridge_String(variables.message_group);
	variables.prop_name->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	variables.prop_message = new Q_Property_Bridge_String_Multiline(variables.message_group);
	variables.prop_message->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	song=NULL;

}

Variables_Edit::~Variables_Edit()
{
}
