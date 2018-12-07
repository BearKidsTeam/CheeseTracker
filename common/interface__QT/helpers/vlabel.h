//
// C++ Interface: vlabel
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef VLABEL_H
#define VLABEL_H

#include <qwidget.h>
//Added by qt3to4:
#include <QPaintEvent>
/**
@author Juan Linietsky
*/
class VLabel : public QWidget {

	QString text;
	void paintEvent( QPaintEvent * p_event );
public:
	void setText(QString p_text);
	VLabel(QWidget *p_parent);
	~VLabel();

};

#endif
