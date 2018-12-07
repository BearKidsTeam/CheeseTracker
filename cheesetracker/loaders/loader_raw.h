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



/***************************************************************************
                      loader_wav.h  -  simple wav loader
                             -------------------
    begin                : Tue Apr 24 2001
    copyright            : (C) 2001 by Calvin French
    email                : frenchc@home.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LOADER_RAW_H
#define LOADER_RAW_H

#include "loader.h"




struct Loader_RAW : public Loader  {

   int load_sample_func(const char *p_filename, Sample_Data *SD);
   virtual Error load_sample(const char *p_filename, int p_dest_index);
   virtual Sample_Data* load_sample(const char *p_filename);

   // test always returns false
   virtual bool test(const char *p_filename);

   // these shouldn't be called (according to reduz) but we implement them anyway
   virtual Error load(const char *p_filename,bool p_load_patterns);
   virtual int get_amount_of_samples();
   virtual Sample_Data *get_sample_data(int p_sample_index);
   virtual string get_sample_name(int p_sample_index);
   virtual void add_sample_to_song(int p_sample_index,int p_dest_index,bool create_instrument);
   virtual Error load_samples_from_instrument(const char *p_filename);
   virtual Error load_instrument(const char *p_filename,int p_dest_index);
   virtual void transfer_data_to_song();
   virtual void free_info(bool free_sampledata=false);

   Loader_RAW();
   ~Loader_RAW();
};



#endif
