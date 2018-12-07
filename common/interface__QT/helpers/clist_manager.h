//
// C++ Interface: clist_manager
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CLIST_MANAGER_H
#define CLIST_MANAGER_H



#include <q3hbox.h>
#include <qpushbutton.h>

/**
@author Juan Linietsky
*/
class Clist_Manager : public Q3HBox {

public:

	QPushButton *add;
	QPushButton *remove;
	QPushButton *moveup;
	QPushButton *movedown;


	Clist_Manager(QWidget *p_parent);
	~Clist_Manager();

};

#endif
