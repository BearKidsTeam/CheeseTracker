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
#include "property_bridge_edit.h"
//Added by qt3to4:
#include <Q3Frame>
#include <QLabel>


//////////////////////////////////////////////////////////////////////////////
///////////////////// INT PROPERTY //////////////////////////
//////////////////////////////////////////////////////////////////////////////

void Q_Property_Bridge_Int::value_changed(int p_newval) {

	if (!bridge) return;
	if (updating_bridge_edit)
		return;

	updating_bridge=true;
	if (vertical) 
		p_newval=bridge->get_max()-(p_newval-bridge->get_min());
	
	bridge->set(p_newval);
	
	char buf[40];
	buf[39]=0;
	snprintf(buf,39," %i",p_newval);
	slider_label->setText(buf);
	
	updating_bridge=false;
}

void Q_Property_Bridge_Int::bridge_changed() {

	if (updating_bridge)
		return;
	
	
	if (!bridge) {
		ERROR("huh?");
		return;
	};
	updating_bridge_edit=true;
	update_value(bridge->get());
	updating_bridge_edit=false;
	
};

void Q_Property_Bridge_Int::update_value(int p_newval) {

	
	char buf[40];
	buf[39]=0;
	snprintf(buf,39," %i",p_newval);
	slider_label->setText(buf);

	if (vertical) 
		p_newval=bridge->get_max()-(p_newval-bridge->get_min());
		
	slider->setValue(p_newval);
	
	
};

void Q_Property_Bridge_Int::set_bridge(Int_Property_Bridge *p_bridge) {

	if (bridge && !connection.empty()) {

		connection.disconnect();
	};

	bridge=p_bridge;
	int bridge_val=bridge->get();
	slider->setRange(bridge->get_min(),bridge->get_max());
	slider->setValue(bridge_val);
	label->setText(bridge->get_name().c_str());
	vlabel->setText(bridge->get_name().c_str());
	value_changed(bridge_val);

	connection = bridge->changed_signal.connect(SigC::slot<void>(*this, &Q_Property_Bridge_Int::bridge_changed));
}

Q_Property_Bridge_Int::Q_Property_Bridge_Int(QWidget *p_parent,bool p_vertical) :  Q3VBox(p_parent) {

	updating_bridge=false;
	updating_bridge_edit=false;
	vertical=p_vertical;
	bridge=NULL;
	label = new QLabel("Bridge:",this);
	slider_hbox = new Q3HBox(this);
	vlabel = new VLabel(slider_hbox);
	slider = new QSlider(p_vertical? Qt::Vertical : Qt::Horizontal,slider_hbox);
	slider_label = new QLabel("0",slider_hbox);
	
	if (vertical)
		slider_label->hide();
	
	if (vertical)
		label->hide();
	else 
		vlabel->hide();
	

	QObject::connect( slider, SIGNAL(valueChanged(int)),this, SLOT(value_changed(int)) );
}

//////////////////////////////////////////////////////////////////////////////
///////////////////// FLOAT PROPERTY //////////////////////////
//////////////////////////////////////////////////////////////////////////////

void Q_Property_Bridge_Float::value_changed(int p_newval) {

	if (!bridge || input_source != NONE) return;

	input_source = USER;
	update_value(p_newval);
	input_source = NONE;
}

void Q_Property_Bridge_Float::update_value(int p_newval) {

	double real_range=bridge->get_max()-bridge->get_min();

	double own_range=(double)MAX-(double)MIN;
	double real_val=(double)p_newval*real_range/own_range;
	real_val+=bridge->get_min();

	if (text_edit) {
		char buf[40];
		buf[39]=0;
		snprintf(buf,39,"%.3f",real_val);
		text_edit->setText(buf);
	};

	bridge->set(real_val);
};

void Q_Property_Bridge_Float::update_value(float p_newval) {

	double real_range=bridge->get_max()-bridge->get_min();
	double own_range=(double)MAX-(double)MIN;

	double int_val=(double)(p_newval-bridge->get_min())*own_range/real_range;

	input_source = NETWORK;
	slider->setValue((int)int_val);
	input_source = NONE;
};

void Q_Property_Bridge_Float::bridge_changed() {

	// reverse the value?
	if (input_source != NONE) return;

	input_source = NETWORK;
	update_value(bridge->get());
	input_source = NONE;
};

void Q_Property_Bridge_Float::text_changed() {

	if (!bridge) return;

	float textval=atof(text_edit->text().ascii());

	//validate
	bridge->set(textval);
	textval=bridge->get();

	/*
	double real_range=bridge->get_max()-bridge->get_min();
	double own_range=(double)MAX-(double)MIN;
	double real_val=(double)(textval-bridge->get_min())*own_range/real_range;

	slider->setValue((int)real_val);
	*/
	input_source = USER;
	update_value(textval);
	input_source = NONE;

	char buf[40];
	buf[39]=0;
	snprintf(buf,39,"%.3f",textval);
	text_edit->setText(buf);

//	value_changed(real_val);

}

void Q_Property_Bridge_Float::set_bridge(Float_Property_Bridge *p_bridge) {

	if (bridge) {

		connection.disconnect();
	};

	bridge=p_bridge;
	slider->setRange(MIN,MAX);
	label->setText(bridge->get_name().c_str());
	char buf[40];
	buf[39]=0;
	snprintf(buf,39,"%.3f",bridge->get());

	if (text_edit) {
		text_edit->setText(buf);
		text_changed();
	};
	bridge_changed();

	connection = bridge->changed_signal.connect(SigC::slot<void>(*this, &Q_Property_Bridge_Float::bridge_changed));
}

Q_Property_Bridge_Float::Q_Property_Bridge_Float(QWidget *p_parent, bool p_text_visible) :  Q3VBox(p_parent) {

	bridge=NULL;
	input_source = NONE;
	label = new QLabel("Bridge:",this);
	slider_hbox = new Q3HBox(this);
	slider = new QSlider(Qt::Horizontal,slider_hbox);
	QObject::connect( slider, SIGNAL(valueChanged(int)),this, SLOT(value_changed(int)) );

	if (p_text_visible) {

		text_edit = new QLineEdit(slider_hbox);
		QObject::connect( text_edit, SIGNAL(returnPressed()),this, SLOT(text_changed()) );
	} else {
		text_edit = NULL;
	};
}

//////////////////////////////////////////////////////////////////////////////
///////////////////// FLOAT PROPERTY DIAL//////////////////////////
//////////////////////////////////////////////////////////////////////////////

void Q_Property_Bridge_Float_Dial::value_changed(int p_newval) {

	if (!bridge) return;

	update_value(p_newval);
}

void Q_Property_Bridge_Float_Dial::update_value(int p_newval) {

	double real_range=bridge->get_max()-bridge->get_min();

	double own_range=(double)MAX-(double)MIN;
	double real_val=(double)p_newval*real_range/own_range;
	real_val+=bridge->get_min();

	bridge->set(real_val);
};

void Q_Property_Bridge_Float_Dial::update_value(float p_newval) {

	double real_range=bridge->get_max()-bridge->get_min();
	double own_range=(double)MAX-(double)MIN;

	double int_val=(double)(p_newval-bridge->get_min())*own_range/real_range;

	dial->setValue((int)int_val);
};


void Q_Property_Bridge_Float_Dial::bridge_changed() {

	update_value(bridge->get());
};

void Q_Property_Bridge_Float_Dial::set_bridge(Float_Property_Bridge *p_bridge) {

	if (bridge) {

		connection.disconnect();
	};

	bridge=p_bridge;
	dial->setMaxValue(MAX);
	dial->setMinValue(MIN);
	label->setText(bridge->get_name().c_str());

	bridge_changed();

	connection = bridge->changed_signal.connect(SigC::slot<void>(*this, &Q_Property_Bridge_Float_Dial::bridge_changed));
}

Q_Property_Bridge_Float_Dial::Q_Property_Bridge_Float_Dial(QWidget *p_parent) :  Q3VBox(p_parent) {

	bridge=NULL;
	dial = new QDial(this);
	label = new QLabel("Bridge:",this);
	setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
	dial->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
	dial->setMaximumSize(QSize(48,48));
	QObject::connect( dial, SIGNAL(valueChanged(int)),this, SLOT(value_changed(int)) );
}

//////////////////////////////////////////////////////////////////////////////
///////////////////// INT PROPERTY CSPIN //////////////////////////
//////////////////////////////////////////////////////////////////////////////

void Q_Property_Bridge_Int_CSpinButon::value_changed(int p_newval) {

	if (!bridge) return;
	bridge->set(p_newval);
}

void Q_Property_Bridge_Int_CSpinButon::set_bridge(Int_Property_Bridge *p_bridge) {

	if (bridge) {

		connection.disconnect();
	};

	bridge=p_bridge; //now finally we can set the real one and the value
	int bridge_val=bridge->get();
	spin->setMaxValue(bridge->get_max());
	spin->setMinValue(bridge->get_min());
	spin->setValue(bridge_val);

//	spin->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
	label->setText(bridge->get_name().c_str());
	value_changed(bridge_val);

	connection = bridge->changed_signal.connect(SigC::slot<void>(*this, &Q_Property_Bridge_Int_CSpinButon::bridge_changed));
}

void Q_Property_Bridge_Int_CSpinButon::bridge_changed() {

	spin->setValue(bridge->get());
};

Q_Property_Bridge_Int_CSpinButon::Q_Property_Bridge_Int_CSpinButon(QWidget *p_parent) :  Q3HBox(p_parent) {

	bridge=NULL;
	label = new QLabel("Bridge:",this);
	spin = new CSpinButton(this);

	QObject::connect( spin, SIGNAL(valueChanged(int)),this, SLOT(value_changed(int)) );
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////// BOOL PROPERTY //////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void Q_Property_Bridge_Bool::toggled_cbk(bool p_on) {

	if (!bridge) return;
	bridge->set(p_on);

}

void Q_Property_Bridge_Bool::bridge_changed() {

	check->setChecked(bridge->get());
};

Q_Property_Bridge_Bool::Q_Property_Bridge_Bool(QWidget *p_parent,bool p_vertical) : Q3VBox(p_parent) {

	setSizePolicy(QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum));
	vertical=p_vertical;
	vlabel = new VLabel(this);
	vlabel->setSizePolicy(QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding));
	
	if (!p_vertical)
		vlabel->hide();
	
	Q3HBox *auxhb = new Q3HBox(this);
	
	(new Q3Frame(auxhb))->setSizePolicy(QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum));;
	check = new QCheckBox(auxhb);
	bridge=NULL;
	check->setText("");
	QObject::connect(check, SIGNAL(toggled(bool)),this, SLOT(toggled_cbk(bool)) );
}

void Q_Property_Bridge_Bool::set_bridge(Bool_Property_Bridge *p_bridge) {

	if (bridge) {

		connection.disconnect();
	};

	bridge=p_bridge;
	
	check->setChecked(bridge->get());
	
	if (vertical)
		vlabel->setText(bridge->get_name().c_str());
	else
		check->setText(bridge->get_name().c_str());

	connection = bridge->changed_signal.connect(SigC::slot<void>(*this, &Q_Property_Bridge_Bool::bridge_changed));
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////// OPTIONS PROPERTY //////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void Q_Property_Bridge_Options::value_changed(int p_newval) {

	if (!bridge) return;

	bridge->set(p_newval);
//	//printf("Selected %i\n",p_newval);
}

void Q_Property_Bridge_Options::bridge_changed() {

	combo->setCurrentItem(bridge->get());
};

void Q_Property_Bridge_Options::set_bridge(Options_Property_Bridge *p_bridge) {

	if (bridge) {

		connection.disconnect();
	};

	bridge=p_bridge;
	label->setText(p_bridge->get_name().c_str());
	combo->clear();
	for (int i=0;i<p_bridge->get_option_count();i++) {

		combo->insertItem(bridge->get_option_text(i),i);
	}

	combo->setCurrentItem(bridge->get());

	connection = bridge->changed_signal.connect(SigC::slot<void>(*this, &Q_Property_Bridge_Options::bridge_changed));
}

Q_Property_Bridge_Options::Q_Property_Bridge_Options(QWidget *p_parent) :  Q3VBox(p_parent) {

	bridge=NULL;
	label = new QLabel("Bridge:",this);
	combo = new QComboBox(this);

	QObject::connect( combo , SIGNAL(activated(int)),this, SLOT(value_changed(int)) );
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////// STRING SINGLE LINE//////////////////////////
//////////////////////////////////////////////////////////////////////////////////


void Q_Property_Bridge_String::value_changed(const QString& p_text) {

	if (!bridge || validating) return;

	string new_text=text_edit->text().ascii();
	if ((int)new_text.length()>(int)bridge->get_max_length()) {
		new_text=new_text.substr(0,bridge->get_max_length());
		validating=true;
		bridge->set(new_text);
		//text_edit->setText(new_text.c_str());
		validating=false;
	} else {
		bridge->set(new_text);
	}
}

void Q_Property_Bridge_String::bridge_changed() {

	text_edit->setText(bridge->get().c_str());
};

void Q_Property_Bridge_String::set_bridge(String_Property_Bridge *p_bridge) {

	if (bridge) {

		connection.disconnect();
	};

	bridge=p_bridge;
	label->setText(bridge->get_name().c_str());
	text_edit->setText(p_bridge->get().c_str());

	connection = bridge->changed_signal.connect(SigC::slot<void>(*this, &Q_Property_Bridge_String::bridge_changed));
}

Q_Property_Bridge_String::Q_Property_Bridge_String(QWidget *p_parent) :  Q3VBox(p_parent) {

	bridge=NULL;
	label = new QLabel("Bridge:",this);
	text_edit= new QLineEdit(this);
	text_edit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	validating=false;

	QObject::connect( text_edit, SIGNAL(textChanged(const QString&)),this, SLOT(value_changed(const QString&)) );
}




///////////////////////////////////////////////////////////////////////////////////
///////////////////// STRING MULTILINE//////////////////////////
//////////////////////////////////////////////////////////////////////////////////


void Q_Property_Bridge_String_Multiline::value_changed() {

	if (!bridge || validating) return;

	validating = true;
	bridge->set(text_edit->text().ascii());
	validating = false;
}

void Q_Property_Bridge_String_Multiline::bridge_changed() {

        if (validating) return;
	validating = true;
	text_edit->setText(bridge->get().c_str());
	validating = false;
};

void Q_Property_Bridge_String_Multiline::set_bridge(String_Property_Bridge *p_bridge) {

	if (bridge) connection.disconnect();	
	bridge=p_bridge;
	label->setText(bridge->get_name().c_str());
	text_edit->setText(p_bridge->get().c_str());
	connection = bridge->changed_signal.connect(SigC::slot<void>(*this, &Q_Property_Bridge_String_Multiline::bridge_changed));
}

Q_Property_Bridge_String_Multiline::Q_Property_Bridge_String_Multiline(QWidget *p_parent) :  Q3VBox(p_parent) {


	bridge=NULL;
	validating = false;
	label = new QLabel("Bridge:",this);
	text_edit= new Q3TextEdit(this);
	text_edit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	QObject::connect( text_edit, SIGNAL(textChanged()),this, SLOT(value_changed()) );

}

