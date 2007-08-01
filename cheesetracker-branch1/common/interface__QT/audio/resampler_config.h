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

#ifndef RESAMPLER_CONFIG_H
#define RESAMPLER_CONFIG_H

#include <qgroupbox.h>
#include <qlistbox.h>
#include "components/data/config_handler.h"
#include "components/audio/resampler_manager.h"
/**
 *
 * Juan Linietsky
 **/
class Resampler_Config : public QGroupBox
{
	Q_OBJECT
        QListBox *resampler_list_box;

	void update_resampler_list();
protected slots:

	void resampler_selected(int p_which);
public:

        void load_settings(ConfigHandler *p_config_handler);
        void save_settings(ConfigHandler *p_config_handler);

	Resampler_Config(QWidget *p_parent);
	~Resampler_Config();
};

#endif
