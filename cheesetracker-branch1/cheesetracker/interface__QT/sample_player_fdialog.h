//
// C++ Interface: sample_player_fdialog
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SAMPLE_PLAYER_FDIALOG_H
#define SAMPLE_PLAYER_FDIALOG_H


#include "trackercore/file_format_manager.h"
#include "trackercore/player_data.h"
#include "interface_binds/player_rt_keyboard.h"
#include "interface_binds/editor.h"

#include <qfiledialog.h>
/**
@author Juan Linietsky
*/
class Sample_Player_FDialog : public QFileDialog {

	Q_OBJECT

	struct Vars {
		File_Format_Manager *file_manager;
		Player_Realtime_Keyboard *rt_keyboard;
		Sample_Data *sample;
	} vars;

	struct Preview {



	} preview;

	bool eventFilter(QObject *p_obj,QEvent *p_event);

protected slots:
	void filename_hilite_slot(const QString& p_file);
public:

	void set_binds(File_Format_Manager *p_file_manager,Player_Realtime_Keyboard *p_rt_keyboard);

	Sample_Player_FDialog();
	~Sample_Player_FDialog();

};

#endif