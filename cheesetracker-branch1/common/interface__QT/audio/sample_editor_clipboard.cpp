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
#include "ns_autoptr.h"
//Added by qt3to4:
#include <QLabel>

void Sample_Editor_Clipboard::copy_cbk() {

	if (!selection->is_active())
		return;
	if (!sdata || !sdata->get_size())
		return;

	Mutex_Lock_Container *sdata_lock = sdata->lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_sdata_lock;
	ns_sdata_lock.ptr_new(sdata_lock);

	clipboard.set_num_channels(sdata->num_channels());
	clipboard.set_size(selection->get_end() - selection->get_begin());
	clipboard.seek(0);
	sdata->seek(selection->get_begin());

	for (size_t ix=0;ix<clipboard.get_size();ix++) {
		clipboard.put_sample(sdata->get_int_sample());
	}

}

void Sample_Editor_Clipboard::cut_cbk() {

	if (!selection->is_active())
		return;
	if (!sdata || !sdata->get_size())
		return;

	// A clipboard cut is really just a Copy followed by deletion.
	// So we use the copy_cbk() function first...

	copy_cbk();

	// Lock the mutex only after copy_cbk(), because copy_cbk()
	// also locks the mutex.

	Mutex_Lock_Container *sdata_lock = sdata->lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_sdata_lock;
	ns_sdata_lock.ptr_new(sdata_lock);

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

	size_t tmp_size = sdata->get_size() - selection->get_end();

	// Get the part of the sample after the selection and put
	// it in tmp_data...

	sample_int_t *tmp_data=NULL; 
	if(tmp_size) {
		tmp_data = new sample_int_t[tmp_size*sdata->num_channels()];
		sdata->seek(selection->get_end());
		sdata->get_sample_array(tmp_data, tmp_size);
	}

	// Go to the beginning of the selection and overwrite it with
	// the trailing data.

	sdata->seek(selection->get_begin());
	if(tmp_data)
		sdata->put_sample_array(tmp_data, tmp_size);

	// Drop any extra data.
	sdata->truncate();

	delete[] tmp_data;

	selection->clear();
	destructive_operation_end();


}

// Paste the contents of the clipboard after the end of the
// selection.

void Sample_Editor_Clipboard::paste_cbk() {

	if (!selection->is_active())
		return;
	if (!sdata)
		return;

	if (!clipboard.get_size())
		return;

	Mutex_Lock_Container *sdata_lock = sdata->lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_sdata_lock;
	ns_sdata_lock.ptr_new(sdata_lock);
	destructive_operation_begin();

	if(!sdata->get_size()) {
		sdata->set_num_channels(clipboard.num_channels());
	}

	// Back up the data after the selection. It will be reinserted after
	// the pasted clipboard data.

	sdata->seek(selection->get_end());
	size_t backup_size;
	sample_int_t *backup_data;
	ns_autoptr<sample_int_t> ns_backup_data;

	backup_size = sdata->get_size() -
		         sdata->get_current_pos();

	backup_data = new sample_int_t[sdata->num_channels() * backup_size];
	ns_backup_data.arr_new(backup_data);
	sdata->get_sample_array(backup_data, backup_size);

	// Return to the end of the sample.

	sdata->seek(selection->get_end());

	// Extend the sample by the size of the clipboard.

	sdata->set_size(sdata->get_size()+clipboard.get_size());

	// Go to the beginning of the clipboard.
	clipboard.seek(0);


	// Put the clipboard data into the sample buffer.
	//
	// If the clipboard and the sample have the same number of
	// channels, then a direct copy is possible. 
	//
	// However, if the sample has a different number of channels than
	// the clipboard, then extra channels in the sample must be zero-padded,
	// while extra channels in the clipboard must be dropped. This requires
	// the use of an extra buffer, {sdata_buf}, which is allocated first
	// so that it only has to be allocated and deallocated once.

	ns_autoptr<sample_int_t> ns_sdata_buf;
	sample_int_t *sdata_buf = new sample_int_t[sdata->num_channels()];
	ns_sdata_buf.arr_new(sdata_buf);

	for(size_t ix=0; ix<clipboard.get_size(); ix++) {
		if(clipboard.num_channels() == sdata->num_channels()) {
			sdata->put_sample(clipboard.get_int_sample());
		} else {
			const sample_int_t *clipboard_buf = clipboard.get_int_sample();
			for(size_t chan=0; chan<sdata->num_channels(); chan++) {
				if(chan > clipboard.num_channels())
					sdata_buf[chan] = 0;
				else
					sdata_buf[chan] = clipboard_buf[chan];
			}
			sdata->put_sample(sdata_buf);
		}
	}

	// Restore the backup data.

	sdata->put_sample_array(backup_data, backup_size);


	if  (sdata->get_loop_end()>=selection->get_end()) {

		sdata->set_loop_end( sdata->get_loop_end() + clipboard.get_size() );

	}

	if  (sdata->get_loop_begin()>=selection->get_end()) {

		sdata->set_loop_begin( sdata->get_loop_begin() + clipboard.get_size() );

	}

	destructive_operation_end();

}

void Sample_Editor_Clipboard::clear_clipboard() {
	clipboard.set_size(0);
}


Sample_Editor_Clipboard::Sample_Editor_Clipboard(QWidget *p_parent) : Q3HBox(p_parent) {

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
