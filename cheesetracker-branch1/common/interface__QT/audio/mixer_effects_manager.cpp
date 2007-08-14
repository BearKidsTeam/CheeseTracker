//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "components/audio/sound_driver_manager.h"
#include "mixer_effects_manager.h"
#include <q3groupbox.h>
#include <qcursor.h>
//Added by qt3to4:
#include <Q3Frame>
#include <QLabel>
#include <Q3PopupMenu>

void Mixer_Effects_Manager::buffer_name_changed(const QString& p_str) {

	if (updating)
		return;

	if (vars.selected_buffer<vars.mixer->get_channel_count()) {

		vars.mixer->set_channel_name( vars.selected_buffer, p_str.ascii() );
		string str=itostr(vars.selected_buffer)+"- "+p_str.ascii();
                channel_list->changeItem(str.c_str(),vars.selected_buffer);
	} else {
        	int selected=vars.selected_buffer-vars.mixer->get_channel_count();
		if (selected>vars.mixer->get_buffer_count()) {

			ERROR("Selected buffer invalid??");
		}
		vars.mixer->set_buffer_name( selected, p_str.ascii() );
                buffer_list->changeItem(p_str,selected);

	}


}


void Mixer_Effects_Manager::channel_selected_slot(int p_which) {

	vars.selected_buffer=p_which;
	vars.selected_output=0;
	update_selected_buffer();
	update_send_parameters();
}

void Mixer_Effects_Manager::buffer_selected_slot(int p_which) {

	vars.selected_buffer=p_which+vars.mixer->get_channel_count();
	vars.selected_output=0;
	update_selected_buffer();
	update_send_parameters();

}

void Mixer_Effects_Manager::set_mixer(Mixer *p_mixer) {

	vars.mixer=p_mixer;
	update();

}


void Mixer_Effects_Manager::update_selected_buffer() {

	if (vars.selected_buffer<0)
		return;
	updating=true;
	if (vars.selected_buffer<vars.mixer->get_channel_count()) {

		chain_editor->set_effect_chain(
			vars.mixer->get_channel_effect_parameter_chain(
				vars.selected_buffer
			)
		);
		buffer_list->clearSelection();
		selected_name->setText( vars.mixer->get_channel_name( vars.selected_buffer ).c_str());


	} else {
        	int selected=vars.selected_buffer-vars.mixer->get_channel_count();
		if (selected>vars.mixer->get_buffer_count()) {

			ERROR("Selected buffer invalid??");
		}
		chain_editor->set_effect_chain(
			vars.mixer->get_buffer_effect_parameter_chain(
				selected
			)
		);
		channel_list->clearSelection();
		selected_name->setText( vars.mixer->get_buffer_name( selected ).c_str());
	}
	updating=false;
}

void Mixer_Effects_Manager::update() {


	int total_chans=vars.mixer->get_channel_count();


        channel_list->clear();
	buffer_list->clear();
	//clear others

	if (vars.selected_buffer>total_chans)
		vars.selected_buffer=total_chans-1;

	if (vars.selected_buffer<0)
		return;


	for (int i=0;i<vars.mixer->get_channel_count();i++) {

		string cstring=itostr(i)+"- "+vars.mixer->get_channel_name(i);

		channel_list->insertItem(cstring.c_str(),i);
	}

	channel_list->setSelected( vars.selected_buffer , true);

	for (int i=0;i<vars.mixer->get_buffer_count();i++) {

		string cstring=vars.mixer->get_buffer_name(i);

		buffer_list->insertItem(cstring.c_str(),i+vars.mixer->get_channel_count());
	}

	if (vars.selected_buffer<total_chans) {

		channel_list->setSelected( vars.selected_buffer , true);
	} else {

		buffer_list->setSelected( vars.selected_buffer-total_chans , true);

	}

	update_selected_buffer();
	update_send_parameters();


}


void Mixer_Effects_Manager::output_send_level_changed_slot(int p_to_what) {

	int total_chans=vars.mixer->get_channel_count();
	if (vars.selected_buffer<0)
		return;
	if (vars.selected_buffer>=(vars.mixer->get_channel_count()+vars.mixer->get_buffer_count()))
		return;
	if (vars.selected_output<0)
		return;

	if (vars.selected_buffer<total_chans) {


		if (vars.selected_output>=vars.mixer->get_channel_output_count(vars.selected_buffer))
			return;

                vars.mixer->set_channel_output_send_level(vars.selected_buffer,vars.selected_output,p_to_what);
	} else {
		int selected=vars.selected_buffer-total_chans;
		if (vars.selected_output>=vars.mixer->get_buffer_output_count(selected))
			return;
                vars.mixer->set_buffer_output_send_level(selected,vars.selected_output,p_to_what);
	}

}

void Mixer_Effects_Manager::output_send_control_changed_slot(int p_to_what) {

	int total_chans=vars.mixer->get_channel_count();
	if (vars.selected_buffer<0)
		return;
	if (vars.selected_buffer>=(vars.mixer->get_channel_count()+vars.mixer->get_buffer_count()))
		return;
	if (vars.selected_output<0)
		return;

	if (vars.selected_buffer<total_chans) {


		if (vars.selected_output>=vars.mixer->get_channel_output_count(vars.selected_buffer))
			return;

                vars.mixer->set_channel_output_send_control(vars.selected_buffer,vars.selected_output,p_to_what);
	} else {
		int selected=vars.selected_buffer-total_chans;
		if (vars.selected_output>=vars.mixer->get_buffer_output_count(selected))
			return;
                vars.mixer->set_buffer_output_send_control(selected,vars.selected_output,p_to_what);
	}

}
void Mixer_Effects_Manager::output_selected_slot(int p_idx) {

//	int total_chans=vars.mixer->get_channel_count();
	if (vars.selected_buffer<0)
		return;
	if (vars.selected_buffer>=(vars.mixer->get_channel_count()+vars.mixer->get_buffer_count()))
		return;

	vars.selected_output=p_idx;
	update_send_parameters();

}


#define GLOBAL_MIXDOWN_HACK_ID 1024

void Mixer_Effects_Manager::at_popup_show_slot() {

	buffer_select_popup->clear();

	buffer_select_popup->insertItem("Global Mixdown",0);
	buffer_select_popup->insertSeparator();

	int total_chans=vars.mixer->get_channel_count();
	if (vars.selected_buffer<0)
		return;
	if (vars.selected_buffer>=(vars.mixer->get_channel_count()+vars.mixer->get_buffer_count()))
		return;

	int max;

	if (vars.selected_buffer<total_chans) {

                max=vars.mixer->get_buffer_count();

	} else {
                max=vars.selected_buffer-total_chans;
	}

	for (int i=0;i<max;i++) {

		buffer_select_popup->insertItem(vars.mixer->get_buffer_name(i).c_str(),i+1);
	}

}

void Mixer_Effects_Manager::popup_selected_buffer_slot(int p_which) {

	if (p_which==0)
		p_which=Mixer::MIXDOWN_BUFFER_INDEX;
	else
		p_which--;

	//printf("***p_which is %i\n",p_which);

	int total_chans=vars.mixer->get_channel_count();
	if (vars.selected_buffer<0)
		return;
	if (vars.selected_buffer>=(vars.mixer->get_channel_count()+vars.mixer->get_buffer_count()))
		return;

	Mutex_Lock * vl=Sound_Driver_Manager::get_singleton_instance()->get_variables_lock();
	if (vl) vl->grab(__FILE__, __LINE__);

	if (vars.selected_buffer<total_chans) {

		int selected=vars.selected_buffer;
		int outp=vars.mixer->get_channel_output_count(selected);
		vars.mixer->add_channel_output(selected);
		vars.mixer->set_channel_output_buffer_index(selected,outp,p_which);
		vars.selected_output=outp;
	} else {

		int selected=vars.selected_buffer-total_chans;
		int outp=vars.mixer->get_buffer_output_count(selected);
		vars.mixer->add_buffer_output(selected);
		vars.mixer->set_buffer_output_buffer_index(selected,outp,p_which);
		vars.selected_output=outp;

	}
	if (vl) vl->release();

	update_send_parameters();

}

void Mixer_Effects_Manager::output_add_slot() {

	buffer_select_popup->exec(QCursor::pos());
}
void Mixer_Effects_Manager::output_remove_slot() {

	int total_chans=vars.mixer->get_channel_count();
	if (vars.selected_buffer<0)
		return;
	if (vars.selected_buffer>=(vars.mixer->get_channel_count()+vars.mixer->get_buffer_count()))
		return;

	Mutex_Lock * vl=Sound_Driver_Manager::get_singleton_instance()->get_variables_lock();
	if (vl) vl->grab(__FILE__, __LINE__);

	if (vars.selected_buffer<total_chans) {

		vars.mixer->remove_channel_output(vars.selected_buffer,vars.selected_output);
	} else {

		vars.mixer->remove_buffer_output(vars.selected_buffer-total_chans,vars.selected_output);
	}
	if (vl) vl->release();

        if (vars.selected_output>0)
		vars.selected_output--;

	update_send_parameters();
}

void Mixer_Effects_Manager::update_send_parameters() {

	output_buffer->clear();

	int total_chans=vars.mixer->get_channel_count();
	if (vars.selected_buffer<0)
		return;
	if (vars.selected_buffer>=(vars.mixer->get_channel_count()+vars.mixer->get_buffer_count()))
		return;
	if (vars.selected_buffer<total_chans) {

		int selected=vars.selected_buffer;
                for (int i=0;i<vars.mixer->get_channel_output_count( selected ) ; i++ ) {

			int idx=vars.mixer->get_channel_output_buffer_index( selected, i);
			string name;

			if (idx==Mixer::MIXDOWN_BUFFER_INDEX) {
				name="Global Mixdown";
			} else {
				name=vars.mixer->get_buffer_name( idx );
			}

			output_buffer->insertItem(name.c_str(),i);
                }

                if (vars.mixer->get_channel_output_count( selected ) ) {

			output_buffer->setCurrentItem(vars.selected_output);
			initial_send_level->setValue( vars.mixer->get_channel_output_send_level(selected, vars.selected_output) );
			send_level_control->setValue( vars.mixer->get_channel_output_send_control(selected, vars.selected_output) );
			output_buffer->setEnabled(true);
			initial_send_level->setEnabled(true);
			send_level_control->setEnabled(true);
			buffer_out_remove->setEnabled(true);
		} else {
			output_buffer->setEnabled(false);
			initial_send_level->setEnabled(false);
			send_level_control->setEnabled(false);
			buffer_out_remove->setEnabled(false);

		}
	} else {

		int selected=vars.selected_buffer-total_chans;

		//printf("buffer has %i outputs\n",vars.mixer->get_buffer_output_count( selected ));
                for (int i=0;i<vars.mixer->get_buffer_output_count( selected ) ; i++ ) {

			int idx=vars.mixer->get_buffer_output_buffer_index( selected, i);
			string name;
			if (idx==Mixer::MIXDOWN_BUFFER_INDEX) {
				name="Global Mixdown";
			} else {
				name=vars.mixer->get_buffer_name( idx );
			}

			output_buffer->insertItem(name.c_str(),i);
                }

                if (vars.mixer->get_buffer_output_count( selected ) ) {

			output_buffer->setCurrentItem(vars.selected_output);
			initial_send_level->setValue( vars.mixer->get_buffer_output_send_level(selected, vars.selected_output) );
			send_level_control->setValue( vars.mixer->get_buffer_output_send_control(selected, vars.selected_output) );
			output_buffer->setEnabled(true);
			initial_send_level->setEnabled(true);
			send_level_control->setEnabled(true);
			buffer_out_remove->setEnabled(true);
		} else {
			output_buffer->setEnabled(false);
			initial_send_level->setEnabled(false);
			send_level_control->setEnabled(false);
			buffer_out_remove->setEnabled(false);
		}


	}
}

void Mixer_Effects_Manager::create_buffer_slot() {

	Mutex_Lock * vl=Sound_Driver_Manager::get_singleton_instance()->get_variables_lock();
	if (vl) vl->grab(__FILE__, __LINE__);
	vars.mixer->add_buffer();
	if (vl) vl->release();

	update();

}
void Mixer_Effects_Manager::erase_selected_buffer_slot() {


	if (vars.selected_buffer<vars.mixer->get_channel_count())
		return;

	int selected=vars.selected_buffer-vars.mixer->get_channel_count();

	if (selected>vars.mixer->get_buffer_count()) {

		ERROR("Selected buffer invalid??");
		return;
	}

	Mutex_Lock * vl=Sound_Driver_Manager::get_singleton_instance()->get_variables_lock();
	if (vl) vl->grab(__FILE__, __LINE__);
	vars.mixer->remove_buffer(selected);
	if (vl) vl->release();

	vars.selected_buffer--;
	update();
}

Mixer_Effects_Manager::Mixer_Effects_Manager(QWidget *p_parent) : Q3HBox(p_parent) {

	Q3VBox * buffer_splitter = new Q3VBox(this);
	setStretchFactor(buffer_splitter,1);

	Q3GroupBox *name_gb= new Q3GroupBox ( 1, Qt::Horizontal,"Selected:",buffer_splitter);

        selected_name = new QLineEdit(name_gb);
	QObject::connect(selected_name,SIGNAL(textChanged(const QString& )),this,SLOT(buffer_name_changed(const QString& )));

	Q3GroupBox *channel_gb= new Q3GroupBox ( 1, Qt::Horizontal,"Mixer Channels",buffer_splitter);
	channel_list = new Q3ListBox(channel_gb);
	QObject::connect(channel_list,SIGNAL(highlighted(int)) ,this,SLOT(channel_selected_slot(int)));
	Q3GroupBox *buffer_gb= new Q3GroupBox ( 1, Qt::Horizontal,"Global Buffers",buffer_splitter);
	buffer_list = new Q3ListBox(buffer_gb);
	buffer_list_manager = new Clist_Manager(buffer_gb);
	QObject::connect(buffer_list_manager->add,SIGNAL(clicked()),this,SLOT(create_buffer_slot()));
	QObject::connect(buffer_list_manager->remove,SIGNAL(clicked()),this,SLOT(erase_selected_buffer_slot()));
	QObject::connect(buffer_list,SIGNAL(highlighted(int)) ,this,SLOT(buffer_selected_slot(int)));
	buffer_list_manager->movedown->hide();
	buffer_list_manager->moveup->hide();

	Q3VBox * route_splitter = new Q3VBox(this);
	setStretchFactor(route_splitter,2);

	Q3GroupBox *chain_gb= new Q3GroupBox ( 1, Qt::Horizontal,"Effect Chain",route_splitter);
	chain_editor = new Effect_Chain_Editor(chain_gb);

	Q3GroupBox *send_gb= new Q3GroupBox ( 1, Qt::Horizontal,"Output to Buffer",route_splitter);
	send_gb->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));

	Q3VBox * output_splitter = new Q3VBox(send_gb);
	Q3HBox *output_buffer_splitter = new Q3HBox(output_splitter);

	new QLabel("Outputs:",output_buffer_splitter);

	output_buffer = new QComboBox(output_buffer_splitter);
	output_buffer->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));

	buffer_out_add = new QPushButton("Add",output_buffer_splitter);
	buffer_out_remove = new QPushButton("Remove",output_buffer_splitter);

	Q3HBox *output_send_splitter = new Q3HBox(output_splitter);

	new QLabel("Initial Send:",output_send_splitter);
	initial_send_level = new CSpinButton(output_send_splitter);
	initial_send_level->setMaxValue(127);
	initial_send_level->setMinValue(0);
	new QLabel("Send Ctrl#:",output_send_splitter);
	send_level_control = new CSpinButton(output_send_splitter);
	send_level_control->setMaxValue(127);
	send_level_control->setMinValue(-1);
	QObject::connect(initial_send_level,SIGNAL(valueChanged(int)),this,SLOT(output_send_level_changed_slot(int )));
	QObject::connect(send_level_control,SIGNAL(valueChanged(int)),this,SLOT(output_send_control_changed_slot(int )));
	QObject::connect(output_buffer,SIGNAL(activated(int)),this,SLOT(output_selected_slot(int )));
	QObject::connect(buffer_out_add,SIGNAL(clicked()),this,SLOT(output_add_slot()));
	QObject::connect(buffer_out_remove,SIGNAL(clicked()),this,SLOT(output_remove_slot()));


	Q3Frame *auxfrm = new Q3Frame(output_send_splitter);
	auxfrm->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));

	buffer_select_popup = new Q3PopupMenu(this);
	QObject::connect(buffer_select_popup,SIGNAL(aboutToShow()),this,SLOT(at_popup_show_slot()));
	QObject::connect(buffer_select_popup,SIGNAL(activated(int)),this,SLOT(popup_selected_buffer_slot(int)));

	vars.selected_buffer=0;
	vars.selected_effect=0;
	vars.selected_output=0;
	updating=false;

}


Mixer_Effects_Manager::~Mixer_Effects_Manager()
{
}


