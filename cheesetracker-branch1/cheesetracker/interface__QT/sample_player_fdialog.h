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

#include <q3filedialog.h>
#include <qcheckbox.h>
#include <QEvent>
/**
@author Juan Linietsky
*/
class Sample_Player_FDialog : public Q3FileDialog {

	Q_OBJECT

	QCheckBox *audition;
	bool audition_mode;

	struct Vars {
		File_Format_Manager *file_manager;
		Player_Realtime_Keyboard *rt_keyboard;
		Sample_Data *sample;
	} vars;

	struct Preview {



	} preview;

	bool eventFilter(QObject *p_obj,QEvent *p_event);
	void position_audition_checkbox();

protected slots:
	void filename_hilite_slot(const QString& p_file);
	void audition_slot(bool p_on);
public:

	void set_binds(File_Format_Manager *p_file_manager,Player_Realtime_Keyboard *p_rt_keyboard);

	Sample_Player_FDialog();
	~Sample_Player_FDialog();

};

#endif
