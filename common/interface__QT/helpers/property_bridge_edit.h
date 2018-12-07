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

#ifndef PROPERTY_BRIDGE_EDIT_H
#define PROPERTY_BRIDGE_EDIT_H

#include "common/components/data/property_bridges.h"
#include <QVBoxLayout>
#include <qlabel.h>
#include <qslider.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qtextedit.h>
#include <qlineedit.h>
#include <qdial.h>
#include "interface__QT/helpers/cspinbutton.h"
#include "interface__QT/helpers/vlabel.h"



//#include "common/components/data
/**
 *
 * Juan Linietsky
 **/

enum WidgetInputSource {

	NONE,
	USER,
	NETWORK,
};


class Q_Property_Bridge_Int : public QVBoxLayout, public sigc::trackable {

 Q_OBJECT

	bool updating_bridge;
 	bool updating_bridge_edit;
	bool vertical;
	QLabel *label;
	QHBoxLayout * slider_hbox;
	QSlider * slider;
	QLabel * slider_label;
	VLabel *vlabel;
	Int_Property_Bridge *bridge;
	Q_Property_Bridge_Int();

	sigc::connection connection;
	void update_value(int p_newval);
	void bridge_changed();

private Q_SLOTS:
	void value_changed(int p_newval);

public:
	void set_bridge(Int_Property_Bridge *p_bridge);

	Q_Property_Bridge_Int(QWidget *p_parent, bool p_vertical=false);
};

class Q_Property_Bridge_Int_CSpinButon : public QHBoxLayout, public sigc::trackable {

 Q_OBJECT

	QLabel *label;
	CSpinButton * spin;
	Int_Property_Bridge *bridge;
	Q_Property_Bridge_Int_CSpinButon();

	sigc::connection connection;
	void bridge_changed();

private Q_SLOTS:
	void value_changed(int p_newval);


public:
	void set_bridge(Int_Property_Bridge *p_bridge);

	Q_Property_Bridge_Int_CSpinButon(QWidget *p_parent);
};

class Q_Property_Bridge_Float : public QVBoxLayout, public sigc::trackable {

 Q_OBJECT

 	enum InternalRange {

		MIN=0,
		MAX=999999
	};

	QLabel *label;
	QHBoxLayout * slider_hbox;
	QSlider * slider;
	Float_Property_Bridge *bridge;
	Q_Property_Bridge_Float();
	QLineEdit *text_edit;

	WidgetInputSource input_source;

	sigc::connection connection;
	void update_value(int p_newval);
	void update_value(float p_newval);
	void bridge_changed();
	bool changing_bridge;

private Q_SLOTS:
	void value_changed(int p_newval);
	void text_changed();

public:

	void set_bridge(Float_Property_Bridge *p_bridge);

	Q_Property_Bridge_Float(QWidget *p_parent, bool textbox_visible = false);
 };

class Q_Property_Bridge_Float_Dial : public QVBoxLayout, public sigc::trackable {

	 Q_OBJECT

 	enum InternalRange {

		MIN=0,
		MAX=999999
	};

	QLabel *label;
	QDial *dial;
	Float_Property_Bridge *bridge;
	Q_Property_Bridge_Float_Dial();

	sigc::connection connection;
	void update_value(int p_newval);
	void update_value(float p_newval);
	void bridge_changed();

private Q_SLOTS:
	void value_changed(int p_newval);

public:
	void set_bridge(Float_Property_Bridge *p_bridge);

	Q_Property_Bridge_Float_Dial(QWidget *p_parent);
 };


class Q_Property_Bridge_Bool : public QVBoxLayout, public sigc::trackable {

 Q_OBJECT
	Q_Property_Bridge_Bool();

 	QCheckBox *check;
	VLabel *vlabel;
	bool vertical;
	Bool_Property_Bridge *bridge;

	sigc::connection connection;
	void bridge_changed();

private Q_SLOTS:
	void toggled_cbk(bool p_on);
public:
	void set_bridge(Bool_Property_Bridge *p_bridge);

	Q_Property_Bridge_Bool(QWidget *p_parent, bool p_vertical=false);
};


class Q_Property_Bridge_Options: public QVBoxLayout, public sigc::trackable {

 Q_OBJECT

	QLabel *label;
	QComboBox* combo;

	Q_Property_Bridge_Options();
	Options_Property_Bridge *bridge;

	sigc::connection connection;
	void update_value(int p_newval);
	void bridge_changed();

private Q_SLOTS:
	void value_changed(int p_newval);

public:

	void set_bridge(Options_Property_Bridge *p_bridge);
	Q_Property_Bridge_Options(QWidget *p_parent);
 };


 class Q_Property_Bridge_String: public QVBoxLayout, public sigc::trackable {

 Q_OBJECT

	QLabel *label;
	QLineEdit *text_edit;

	Q_Property_Bridge_String();
	String_Property_Bridge *bridge;
	bool validating;

	sigc::connection connection;
	void bridge_changed();

private Q_SLOTS:
	void value_changed(const QString& p_text);

public:

	void set_bridge(String_Property_Bridge *p_bridge);
	Q_Property_Bridge_String(QWidget *p_parent);
 };

class Q_Property_Bridge_String_Multiline: public QVBoxLayout, public sigc::trackable {

Q_OBJECT

	QLabel *label;
	QTextEdit *text_edit;

	Q_Property_Bridge_String_Multiline();
	String_Property_Bridge *bridge;
        bool validating;

	sigc::connection connection;
	void bridge_changed();

private Q_SLOTS:
	void value_changed();

public:

	void set_bridge(String_Property_Bridge *p_bridge);
	Q_Property_Bridge_String_Multiline(QWidget *p_parent);
 };

#endif
