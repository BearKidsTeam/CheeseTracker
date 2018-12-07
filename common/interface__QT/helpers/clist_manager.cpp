//
// C++ Implementation: clist_manager
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "clist_manager.h"



Clist_Manager::Clist_Manager(QWidget *p_parent) : Q3HBox(p_parent) {

	add = new QPushButton("Add",this);
	add->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
	remove = new QPushButton("Remove",this);
	remove->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
	moveup = new QPushButton("Up",this);
	moveup->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
	movedown = new QPushButton("Down",this);
	movedown->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
}

Clist_Manager::~Clist_Manager()
{
}


