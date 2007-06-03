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
// C++ Implementation: sample_editor_clipboard
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <algorithm>
#include "sample_editor_clipboard.h"

void Sample_Editor_Clipboard::copy_cbk() {

	if (!selection->is_active())
		return;
	if (!sdata || !sdata->get_size(0))
		return;

	clipboard.alloc_channels(sdata->num_channels());

	for(size_t chan=0; chan<sdata->num_channels(); chan++) {
		clipboard.set_size(chan,
		                   selection->get_end() - selection->get_begin());
		clipboard.seek(chan, 0);
		sdata->seek(chan, selection->get_begin());
		for (size_t ix=0;ix<clipboard.get_size();ix++) {
			clipboard.put_sample(chan,
			                         sdata->get_int_sample(chan));
		}
	}

}

void Sample_Editor_Clipboard::cut_cbk() {

	if (!selection->is_active())
		return;
	if (!sdata || !sdata->get_size(0))
		return;

	copy_cbk();

	// Correct the loop-begin and loop-end indicators, as the
	// change in sample size may invalidate them.

	if ( (sdata->get_loop_begin()>=selection->get_begin()) &&
	     (sdata->get_loop_begin()<selection->get_end()) ) {

		sdata->set_loop_begin(selection->get_begin());
	} else if (sdata->get_loop_begin()>=selection->get_end()) {

		sdata->set_loop_begin( sdata->get_loop_begin() -
					  ( selection->get_end() -
					    selection->get_begin()
					  )
				     );

	}

	if ( (sdata->get_loop_end() >= selection->get_begin()) &&
	     (sdata->get_loop_end()<selection->get_end())
	   ) {

		sdata->set_loop_end(selection->get_begin());
	} else if (sdata->get_loop_end()>=selection->get_end()) {

		sdata->set_loop_end( sdata->get_loop_end() -
		                     ( selection->get_end() -
		                       selection->get_begin()
		                     )
		                   );

	}

	// We begin the cut...

	destructive_operation_begin();

	for(size_t chan=0; chan<sdata->num_channels(); chan++) {
		// Get the part of the sample after the selection and put
		// it in sdata...

		size_t tmp_size = sdata->get_size(chan) - selection->get_end();
		sample_int_t *tmp_data=NULL; 
		if(tmp_size) {
			tmp_data = new sample_int_t[tmp_size];
			sdata->seek(chan, selection->get_end());
			sdata->get_sample_array(chan, tmp_data, tmp_size);
		}

		// Go to the beginning of the selection and overwrite it with
		// the trailing data.

		sdata->seek(chan, selection->get_begin());
		if(tmp_data)
			sdata->put_sample_array(chan, tmp_data, tmp_size);

		// Drop any extra data.
		sdata->truncate(chan);

		delete[] tmp_data;
	}

	selection->clear();
	destructive_operation_end();


}

void Sample_Editor_Clipboard::paste_cbk() {

	if (!selection->is_active())
		return;
	if (!sdata)
		return;

	if (!clipboard.get_size(0))
		return;

	destructive_operation_begin();

	if(!sdata->get_size(0)) {
		sdata->alloc_channels(clipboard.num_channels());
	}

	size_t max_channels =
		std::min<size_t>(sdata->num_channels(),
		                 clipboard.num_channels());

	// Paste the contents of the clipboard after the end of the
	// selection.

	for(size_t chan=0; chan<max_channels; chan++) {

		// tmp_data holds the data from the end of the selection
		// to the end of the sample.

		sdata->seek(chan, selection->get_end());
		size_t tmp_size;
		tmp_size = sdata->get_size(chan) -
		           sdata->get_current_pos(chan);
		sample_int_t *tmp_data = NULL;

		if(tmp_size) {
			tmp_data = new sample_int_t[tmp_size];
		}

		// Go to the end of the selection.

		sdata->seek(chan, selection->get_end());

		// Back up the data after the selection, unless the selection
		// ends at the end of the sample.

		if(tmp_data) {
			sdata->get_sample_array(chan, tmp_data, tmp_size);
			sdata->seek(chan, selection->get_end());
		}

		// Extend the sample by the size of the clipboard.

		sdata->set_size(chan, sdata->get_size(chan)+
		                      clipboard.get_size(chan));

		// Go to the beginning of the clipboard.

		clipboard.seek(chan,0);
		for(size_t ix=0; ix<clipboard.get_size(chan); ix++) {
			// Store the data.
			sdata->put_sample(chan,
			                      clipboard.get_int_sample(chan));
		}
		if(tmp_data) {
			sdata->put_sample_array(chan, tmp_data, tmp_size); 
			delete[] tmp_data;
		}
	}


	if  (sdata->get_loop_end()>=selection->get_end()) {

		sdata->set_loop_end( sdata->get_loop_end() + clipboard.get_size(0) );

	}

	if  (sdata->get_loop_begin()>=selection->get_end()) {

		sdata->set_loop_begin( sdata->get_loop_begin() + clipboard.get_size(0) );

	}

	destructive_operation_end();

}

void Sample_Editor_Clipboard::clear_clipboard() {

	for(size_t chan=0; chan<clipboard.num_channels(); chan++) {
		clipboard.seek(chan, 0);
		clipboard.truncate(chan);
	}

	clipboard.alloc_channels(0);
}


Sample_Editor_Clipboard::Sample_Editor_Clipboard(QWidget *p_parent) : QHBox(p_parent) {

	new QLabel("Edit:",this);

	copy = new QPushButton("Copy",this);
	cut = new QPushButton("Cut",this);
	paste = new QPushButton("Paste",this);

	sdata=NULL;
	selection=NULL;

	QObject::connect(copy,SIGNAL(clicked()),this,SLOT(copy_cbk()));
	QObject::connect(cut,SIGNAL(clicked()),this,SLOT(cut_cbk()));
	QObject::connect(paste,SIGNAL(clicked()),this,SLOT(paste_cbk()));

}

Sample_Editor_Clipboard::~Sample_Editor_Clipboard() {


}
