//
// C++ Implementation: vlabel
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "vlabel.h"
 
#include <qpainter.h>
//Added by qt3to4:
#include <QPaintEvent>

void VLabel::setText(QString p_text) {

	text=p_text;
	repaint();
}

 void VLabel::paintEvent( QPaintEvent * p_event ) {
 
        QPainter painter( this );
        painter.rotate( 270 );
        painter.drawText(-height()+2, width()-3 , text);
}

VLabel::VLabel(QWidget *p_parent) : QWidget(p_parent) {
	
        setMinimumSize(20,10);	
}


VLabel::~VLabel()
{
}


