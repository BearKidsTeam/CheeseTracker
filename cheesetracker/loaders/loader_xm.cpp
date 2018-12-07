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
                          loader_xm.cpp  -  description
                             -------------------
    begin                : Tue Apr 9 2002
    copyright            : (C) 2002 by red
    email                : red@server
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "loader_xm.h"

#define ABORT_LOAD  { reader.close(); \
	for (size_t x=0;x<samples.size();x++) {\
		samples[x].data.set_size(0);				\
	}								\
 	samples.clear(); \
  	instruments.clear(); \
    	return FILE_FORMAT_NOT_RECOGNIZED; }




Loader::Error Loader_XM::load(const char *p_filename,bool p_load_patterns) {


        if (reader.open(p_filename)) {

          	return FILE_ERROR;
        };


 	samples.clear();
  	instruments.clear();
   	for (int i=0;i<256;i++) {

      		patterns[i].clear();
        }

        /**************************************
        LOAD HEADER
        ***************************************/

  	reader.get_byte_array(header.idtext,17);
   	header.idtext[17]=0;

  	reader.get_byte_array(header.songname,20);


   	header.songname[20]=0;
  	header.hex1a=reader.get_byte();
   	if (header.hex1a!=0x1A) {

      		ABORT_LOAD;
        }



  	reader.get_byte_array(header.trackername,20);
  	header.trackername[20]=0;


  	header.version=reader.get_word();

  	header.headersize=reader.get_dword();

	header.songlength=reader.get_word();

  	header.restart_pos=reader.get_word();

  	header.channels_used=reader.get_word();

  	header.patterns_used=reader.get_word();

  	header.instruments_used=reader.get_word();

  	header.use_linear_freq=reader.get_word();

  	header.tempo=reader.get_word();

  	header.speed=reader.get_word();

  	reader.get_byte_array(header.orderlist,256);

        /**************************************
        LOAD PATTERNS
        ***************************************/

        for (int i=0;i<header.patterns_used;i++) {

          	Uint32 aux,rows;

		aux=reader.get_dword(); //length
           	aux=reader.get_byte(); //packing type
           	rows=aux=reader.get_word(); //rows!

            	patterns[i].set_length(aux);
           	aux=reader.get_word(); //packed size
            	if (aux==0)
             		continue;
                //unpaaack!
		for(Uint32 j=0;j<rows;j++)
			for(int k=0;k<header.channels_used;k++) {

     				Note aux_note;
         			Uint8 aux_byte;
	                    	aux_byte=reader.get_byte(); //packing type
                      		if (!(aux_byte&0x80)) {

                          		aux_note.note=aux_byte;
                        		aux_byte=0xFE; //if bit 7 not set, read all of them except the note
                          	}

                       		if (aux_byte&1) aux_note.note=reader.get_byte();
                         	if (aux_byte&2) aux_note.instrument=reader.get_byte();
                         	if (aux_byte&4) aux_note.volume=reader.get_byte();
                         	if (aux_byte&8) aux_note.command=reader.get_byte();
                         	if (aux_byte&16) aux_note.parameter=reader.get_byte();

                          	if (aux_note.note!=EMPTY_FIELD) {

                                 	if (aux_note.note==97) aux_note.note=Note::OFF;
                                  	else {
                                   	}
                             	}
	                       	if (aux_note.instrument!=EMPTY_FIELD) {

                                 	if ((aux_note.instrument>0) && (aux_note.instrument<100))
                                  		 aux_note.instrument--;
                                     	else
                                  		 aux_note.instrument=EMPTY_FIELD;
                             	}
	                       	if (aux_note.volume!=EMPTY_FIELD) {

                                 	if (aux_note.volume<0x10) {}
                                  	else if (aux_note.volume<0x50) {

						aux_note.volume-=0x10;

                                     	} else if (aux_note.volume<0x60) {
                                                //
						aux_note.volume=EMPTY_FIELD;

                                     	} else if (aux_note.volume<0x70) {
                                                //60 -- volume slide down
						aux_note.volume-=0x60;
      						if (aux_note.volume>9) aux_note.volume=9;
						aux_note.volume+=95;
						
                                     	} else if (aux_note.volume<0x80) {
                                                //70 -- volume slide up
						aux_note.volume-=0x70;
      						if (aux_note.volume>9) aux_note.volume=9;
						aux_note.volume+=85;


                                     	} else if (aux_note.volume<0x90) {
                                                //80 -- fine volume slide down
						aux_note.volume-=0x80;
      						if (aux_note.volume>9) aux_note.volume=9;
						aux_note.volume+=75;


                                     	} else if (aux_note.volume<0xA0) {
                                                //9 -- fine volume slide up

						aux_note.volume-=0x90;
      						if (aux_note.volume>9) aux_note.volume=9;

						aux_note.volume+=65;



                                     	} else if (aux_note.volume<0xB0) {
                                                //A -- set vibrato speed
						aux_note.volume=EMPTY_FIELD;

                                     	} else if (aux_note.volume<0xC0) {
                                                //B -- vibrato
						aux_note.volume-=0xB0;
      						if (aux_note.volume>9) aux_note.volume=9;
						aux_note.volume+=203;


                                     	} else if (aux_note.volume<0xD0) {
                                                //C -- set panning
                                                int aux=aux_note.volume-=0xC0;
						aux=aux*65/0xF;
      						aux_note.volume=128+aux;

                                     	} else if (aux_note.volume<0xE0) {
						aux_note.volume=EMPTY_FIELD;


                                     	} else if (aux_note.volume<0xF0) {
						aux_note.volume=EMPTY_FIELD;


                                     	} else {
                                                //F -- tone porta
						aux_note.volume-=0xF0;
      						aux_note.volume*=9;
      						aux_note.volume/=0xF;
            					aux_note.volume+=193;
      					}
                             	}
	                       	if (aux_note.command!=EMPTY_FIELD) {

                                   	switch(aux_note.command) {

                                                case 0x0:
        						aux_note.command='J'-'A';
              						break;		
                                                case 0x1:
        						aux_note.command='F'-'A';
              						break;
                                                case 0x2:
        						aux_note.command='E'-'A';
              						break;
                                                case 0x3:
        						aux_note.command='G'-'A';
              						break;
                                                case 0x4:
        						aux_note.command='H'-'A';
                                                        break;
                                                case 0x5:
        						aux_note.command='L'-'A';
                                                        break;
                                                case 0x6:
        						aux_note.command='K'-'A';
                                                        break;
                                                case 0x7:
        						aux_note.command='R'-'A';
                                                        break;
                                                case 0x8:
        						aux_note.command='X'-'A';
                                                        break;
                                                case 0x9:
        						aux_note.command='O'-'A';
                                                        break;
                                                case 0xa:
        						aux_note.command='D'-'A';
                                                        break;
                                                case 0xb:
        						aux_note.command='B'-'A';
                                                        break;
                                                case 0xc:
        						//printf("XM Import: Warning! effect C (set volume) not implemented!\n");
                                                        break;
                                                case 0xd:
        						aux_note.command='C'-'A';
                                                        break;

                                                case 0xe: /* Extended effects */

                                                	aux_note.command='S'-'A';
                                                        switch(aux_note.parameter>>4) {
                                                                case 0x1: /* XM fine porta up */
                                                                	if (!(aux_note.parameter&0xF)) { aux_note.command=EMPTY_FIELD; aux_note.parameter=0; break; }
                                                                	aux_note.command='F'-'A';
                                                            	        aux_note.parameter=0xF0|(aux_note.parameter&0xF);
                                                                        break;
                                                                case 0x2: /* XM fine porta down */
                                                                	if (!(aux_note.parameter&0xF)) { aux_note.command=EMPTY_FIELD; aux_note.parameter=0; break; }
                                                                	aux_note.command='E'-'A';
                                                            	        aux_note.parameter=0xF0|(aux_note.parameter&0xF);
                                                                        break;
                                                                case 0xa: /* XM fine volume up */
                                                                	if (!(aux_note.parameter&0xF)) { aux_note.command=EMPTY_FIELD; aux_note.parameter=0; break; }
                                                                	aux_note.command='D'-'A';
                                                            	        aux_note.parameter=0x0F|((aux_note.parameter&0xF)<<4);

                                                                        break;
                                                                case 0xb: /* XM fine volume down */
                                                                	if (!(aux_note.parameter&0xF)) { aux_note.command=EMPTY_FIELD; aux_note.parameter=0; break; }
                                                                	aux_note.command='D'-'A';
                                                            	        aux_note.parameter=0xF0|(aux_note.parameter&0xF);

                                                                        break;
                                                                case 0x9: /* XM fine volume down */
                                                                	if (!(aux_note.parameter&0xF)) { aux_note.command=EMPTY_FIELD; aux_note.parameter=0; break; }
                                                                	aux_note.command='Q'-'A';
                                                            	        aux_note.parameter=0x00|(aux_note.parameter&0xF);
                                                                        break;

                                                                case 0xc: //notecut
                                                                	
                                                            	        aux_note.parameter=0xC0|(aux_note.parameter&0xF);
                                                                        break;

                                                                case 0xd: //notedelay
                                                                	
                                                            	        aux_note.parameter=0xD0|(aux_note.parameter&0xF);
                                                                        break;

                                                                case 0xe: //patterndelay
                                                                	
                                                            	        aux_note.parameter=0xB0|(aux_note.parameter&0xF);
                                                                        break;
                                                        }

                                                        break;
                                                case 0xf:
                                                	if (aux_note.parameter<32) {
	        						aux_note.command='A'-'A';
               						} else {
	        						aux_note.command='T'-'A';
                       					}
                                                        break;
                                                case 'G'-55:
        						aux_note.command='V'-'A';
                                                        break;
                                                case 'H'-55:
        						aux_note.command='W'-'A';
                                                        break;
                                                case 'K'-55:
                                                	if (aux_note.note!=EMPTY_FIELD) break;
        						aux_note.note=Note::OFF;
                                                        break;
                                                case 'P'-55:
        						aux_note.command='P'-'A';
                                                        break;
                                                case 'R'-55:
        						aux_note.command='Q'-'A';
                                                        break;
              					case 'T'-55:
        						aux_note.command='I'-'A';
                                                        break;
              					default: {

                     					aux_note.command=EMPTY_FIELD;
                     				}
              				}


				}

                                	patterns[i].get_note_ref(k,j)=aux_note;
		    }
	}

        /**************************************
        LOAD INSTRUMENTS!
        ***************************************/

        for (int i=0;i<header.instruments_used;i++) {


		//printf("Instrument %i\n",i);
          	Uint32 aux,hsize,cpos;
           	int sampnum;
           	Instrument instrument;
		char instrname[23];

  		instrname[22]=0;
                cpos=reader.get_file_pos();
		//printf("pos is %i\n",cpos);


/* +4 */	hsize=reader.get_dword(); //length

  		reader.get_byte_array((Uint8*)instrname,22);
		//printf("name is %s\n",instrname);

/* +27 */	aux=reader.get_byte(); //headersize
		//printf("header size is %i\n",aux);

/* +29 */	sampnum=reader.get_word(); //headersize

		//printf("samples %i\n",sampnum);


		instrument.name=instrname;
		if (sampnum==0) {
			aux=reader.get_dword(); //Why is this for?
			instruments.push_back(instrument);
			continue;
		}

/* +33 */      	aux=reader.get_dword(); //header size

		if (Error result=load_instrument_internal(&instrument,false,cpos,hsize,sampnum)) {

			return result;
		}

	}

	reader.close();
	return SUCCESS;
}

Loader::Error Loader_XM::load_instrument_internal(Instrument *p_instr,bool p_xi,int p_cpos, int p_hsize, int p_sampnum) {

		int sampnum;
		Uint32 aux;
		Uint8 notenumb[96];
		Uint16 panenv[24],volenv[24];
		int volpoints,panpoints;
		int vol_loop_begin,vol_loop_end,vol_sustain_loop;
		int pan_loop_begin,pan_loop_end,pan_sustain_loop;
		char instrname[23];
		int sample_index[16]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}; //-1 means no index!

		instrname[22]=0;


/* +129 */	reader.get_byte_array((Uint8*)notenumb,96);
		for (int j=0;j<24;j++) {
			volenv[j]=reader.get_word();
		}
		for (int j=0;j<24;j++) {
			panenv[j]=reader.get_word();
		}

/* +177 */
/* +225 */
/* +226 */	volpoints=reader.get_byte();
/* +227 */	panpoints=reader.get_byte();
/* +230 */	vol_sustain_loop=reader.get_byte();
/* +228 */	vol_loop_begin=reader.get_byte();
/* +229 */	vol_loop_end=reader.get_byte();

		pan_sustain_loop=reader.get_byte();
/* +231 */	pan_loop_begin=reader.get_byte();
/* +232 */	pan_loop_end=reader.get_byte();



/* +234 */	aux=reader.get_byte();
		p_instr->get_layer(0)->volume.envelope.reset();
		p_instr->get_layer(0)->volume.envelope.set_enabled(aux&1);
		p_instr->get_layer(0)->volume.envelope.set_sustain_loop_enabled((aux&2)?true:false);
		p_instr->get_layer(0)->volume.envelope.set_loop_enabled((aux&4)?true:false);
/* +235 */	aux=reader.get_byte();
		p_instr->get_layer(0)->panning.envelope.reset();
		p_instr->get_layer(0)->panning.envelope.set_enabled(aux&1);
		p_instr->get_layer(0)->panning.envelope.set_sustain_loop_enabled((aux&2)?true:false);
		p_instr->get_layer(0)->panning.envelope.set_loop_enabled((aux&4)?true:false);

/* +239 */	aux=reader.get_dword(); // sadly, cant use those
/* +241 */     	p_instr->get_layer(0)->volume.fadeout=reader.get_word() >> 4;
/* +243 */	aux=reader.get_word(); // reserved!



		for (int j=0;j<volpoints;j++) {
			int ofs=volenv[j*2];
			int val=volenv[j*2+1];
			p_instr->get_layer(0)->volume.envelope.add_node_at_offset(ofs,val);

		}

		for (int j=0;j<panpoints;j++) {
			int ofs=panenv[j*2];
			int val=panenv[j*2+1];
			p_instr->get_layer(0)->panning.envelope.add_node_at_offset(ofs,val-32);
		}
		p_instr->get_layer(0)->volume.envelope.set_loop_begin(vol_loop_begin);
		p_instr->get_layer(0)->volume.envelope.set_loop_end(vol_loop_end);
		p_instr->get_layer(0)->volume.envelope.set_sustain_loop_end(vol_sustain_loop);
		p_instr->get_layer(0)->volume.envelope.set_sustain_loop_begin(vol_sustain_loop);
		p_instr->get_layer(0)->panning.envelope.set_loop_begin(pan_loop_begin);
		p_instr->get_layer(0)->panning.envelope.set_loop_end(pan_loop_end);
		p_instr->get_layer(0)->panning.envelope.set_sustain_loop_end(pan_sustain_loop);
		p_instr->get_layer(0)->panning.envelope.set_sustain_loop_begin(pan_sustain_loop);


		if (!p_xi) {

			if ((reader.get_file_pos()-p_cpos)<p_hsize) {

				// Allocate a buffer big enough for the
				// data being read

				Uint8 *junkbuster = new Uint8 [p_hsize-(reader.get_file_pos()-p_cpos)];

				// return in error if it can't be
				// allocated
				if(!junkbuster)
					return FILE_ERROR;

				//printf("extra junk XM instrument in header! hsize is %i, extra junk: %i\n",p_hsize,(reader.get_file_pos()-p_cpos));

				reader.get_byte_array((Uint8*)junkbuster,p_hsize-(reader.get_file_pos()-p_cpos));

				// Fix Debian memory leak.

				delete[] junkbuster;
			}
			sampnum=p_sampnum;

		} else {

			Uint8 junkbuster[500];
			reader.get_byte_array((Uint8*)junkbuster,20);	//14 bytes?



			sampnum=reader.get_word();




		}



		/*SAMPLE!!*/
		size_t sample_size;
		bool sample_is_16bits;
		Sample sample;

		for (int j=0;j<sampnum;j++) {

			if (j>16) ABORT_LOAD;

			char auxb;
			sample.data.set_num_channels(1);
			sample_size=reader.get_dword();
			sample.data.set_loop_begin( reader.get_dword() );

			sample.data.set_loop_end( reader.get_dword() );
			sample.def_volume=reader.get_byte();
			auxb=reader.get_byte();
			int finetune=auxb;
			aux=reader.get_byte();
			sample.data.set_loop_enabled( aux&3 );
			sample.data.set_loop_ping_pong( aux&2 );
			sample_is_16bits=aux&16;


			if (sample_is_16bits){

				sample_size/=2; // evil fasttracker gives samples sizes in bytes
				sample.data.set_loop_begin( sample.data.get_loop_begin()/2 );
				sample.data.set_loop_end( sample.data.get_loop_end()/2 );
			}
			
			sample.data.set_size(sample_size);

			sample.data.set_loop_end( sample.data.get_loop_end() + sample.data.get_loop_begin() );

			sample.def_panning_on=true;
			sample.def_panning=(int)reader.get_byte()*64/255;
			auxb=reader.get_byte();
			int note_offset=(int)(auxb)-1;
			note_offset+=60;
			sample.data.set_c5_freq( Tables::get_linear_frequency(Tables::get_linear_period(note_offset<<1,finetune)) );
			//printf("NOTE %i,fine %i\n",note_offset,finetune);


			auxb=reader.get_byte(); //reserved?
			reader.get_byte_array((Uint8*)instrname,22);
			sample.name=instrname;


			if (sample.data.is_empty()) continue; //screw you d00d
			sample.in_use=true;
			samples.push_back(sample);
			sample_index[j]=samples.size()-1;
		}

		/*SAMPLE __DATA__!!*/

		for (int j=0;j<sampnum;j++) {

			if (sample_index[j]==-1) continue;

			Sample *sample=&samples[sample_index[j]];


			sample_int_t sampleval;
			sample_int_t old=0;
			sample_int_t newsample;

			sample->data.seek(0);
			for(size_t ix=0; ix<sample_size; ix++) {
				if(sample_is_16bits)
					sampleval=CONVERT_FROM_TYPE(Sint16, reader.get_word());
				else
					sampleval=CONVERT_FROM_TYPE(Sint8, reader.get_byte());
				
				newsample=sampleval+old;
				old=newsample;
				
				sample->data.put_sample(&newsample);
			}

		}

		for (int j=0;j<96;j++) {

			int val=notenumb[j];
			if ((val<0) || (val>15)) continue;
			else val=sample_index[val];
			if (val==-1) continue;
			p_instr->get_layer(0)->sample_number[12+j]=val;
		}


		instruments.push_back(*p_instr);


	return SUCCESS;

}

bool Loader_XM::test(const char *p_filename) {

	if (reader.open(p_filename)) {

		return false;
	};
	char somebuff[50];
	char hex1a;
	reader.get_byte_array((Uint8*)somebuff,17);
	reader.get_byte_array((Uint8*)somebuff,20);
	hex1a=reader.get_byte();
	reader.close();

	if (hex1a!=0x1A) {

		return false;

	}





	return true;
}


/* Compute Sample Info */
int Loader_XM::get_amount_of_samples() {

	return samples.size();
}
Sample_Data *Loader_XM::get_sample_data(int p_sample_index) {

	if ((p_sample_index<0) || (p_sample_index>=(int)samples.size())) return NULL;

	return &samples[p_sample_index].data;

}
string Loader_XM::get_sample_name(int p_sample_index) {

  	if ((p_sample_index<0) || (p_sample_index>=(int)samples.size())) return "";

   	return samples[p_sample_index].name;
}
void Loader_XM::add_sample_to_song(int p_sample_index,int p_dest_index,bool create_instrument) {


	*song->get_sample(p_dest_index)=samples[p_sample_index];

  	if (create_instrument) {

		int aux_inst=song->find_unused_instrument();

		if (aux_inst!=-1) {

			song->get_instrument(aux_inst)->reset();
			song->get_instrument(aux_inst)->name=song->get_sample(p_dest_index)->name;

			for(int i=0;i<Note::NOTES;i++) {

				song->get_instrument(aux_inst)->get_layer(0)->sample_number[i]=p_dest_index;
			}
		}
	}

}

Loader::Error Loader_XM::load_sample(const char *p_filename,int p_dest_index) {

	return FILE_ERROR;
}
Loader::Error Loader_XM::load_samples_from_instrument(const char *p_filename) {

	return FILE_ERROR;
}

/* Compute Instrument Info */
Loader::Error Loader_XM::load_instrument(const char *p_filename,int p_dest_index) {

	if ( reader.is_open() ) return LOADER_IN_USE;
	if ( reader.open(p_filename) ) return CANNOT_OPEN_FILE;	
        int i;
 	Instrument instr;
  	int aux;
   	vector<int> slot_replace;

        char buffer[500];
        reader.get_byte_array((Uint8*)buffer,0x15);
        buffer[8]=0;
        if ((string)buffer!="Extended") {

		reader.close();
  		return FILE_FORMAT_NOT_RECOGNIZED;
        }

        reader.get_byte_array((Uint8*)buffer,0x16);
        buffer[0x16]=0;
        instr.name=buffer;
        aux=reader.get_byte();
        if(aux!=0x1a) {

		reader.close();
  		return FILE_FORMAT_NOT_RECOGNIZED;
	}

        reader.get_byte_array((Uint8*)buffer,0x14); //somethingaboutthename
        aux=reader.get_word(); //version or blahblah
 	

  	if (load_instrument_internal(&instr,true,0,0)) {

		reader.close();
  		return FILE_FORMAT_NOT_RECOGNIZED;
	}

	reader.close(); //ook, we got it..

	for (i=0;i<(int)samples.size();i++) {

		int new_slot=song->find_unused_sample_slot();

		if (new_slot!=-1) {
			*song->get_sample(new_slot)=samples[i];
   			slot_replace.push_back(new_slot);

		} else if (!samples[i].data.is_empty()) {

			//there's no room aviable for our sample (IT sux, eh? ;)
			samples[i].data.set_size(0);
   			slot_replace.push_back(EMPTY_FIELD);

		}
	}

	for (i=0;i<Note::NOTES;i++) {

		int index=instr.get_layer(0)->sample_number[i];

  		if ((index<0) || (index>=(int)slot_replace.size())) {

      			index=EMPTY_FIELD;
         	} else {

			index = slot_replace[index];
            	}

		instr.get_layer(0)->sample_number[i]=index;
	}


	*song->get_instrument(p_dest_index)=instr;

	samples.clear();
	instruments.clear();



	return SUCCESS;

}

void Loader_XM::transfer_data_to_song() {

	song->reset_data();
	//song->set_instrument_mode();
	song->variables.name=(const char*)header.songname;
	song->variables.message="XM Imported file using\n";
	song->variables.message=song->variables.message+"-CheeseTracker-\n";
	song->variables.message=song->variables.message+"http://reduz.com.ar/cheesetracker\n";
	song->variables.message=song->variables.message+"Original file written with " + (string)(const char*)header.trackername;
	song->variables.use_linear_slides=header.use_linear_freq;
	song->initial_variables.tempo=header.speed;
	song->initial_variables.speed=header.tempo;

	for (int i=0;i<header.songlength;i++) {

		if (i>=199)
			break;
		song->set_order(i,header.orderlist[i]);
	}

        for (int i=0;i<header.patterns_used;i++) {

		if (i>=199)
			break;

		*song->get_pattern(i)=patterns[i];
	}

	for (int i=0;i<(int)samples.size();i++) {

		if (i>=98)
			break;
		*song->get_sample(i)=samples[i];
	}

	for (int i=0;i<(int)samples.size();i++) {

		if (i>=98)
			break;
		*song->get_sample(i)=samples[i];
	}
	list<Instrument>::iterator I=instruments.begin();
	for (int i=0;I!=instruments.end();I++,i++) {

		if (i>=98)
			break;
		*song->get_instrument(i)=*I;
	}

}

void Loader_XM::free_info(bool free_sampledata) {
	samples.clear();
	instruments.clear();
}


Loader_XM::Loader_XM(){
}
Loader_XM::~Loader_XM(){
}

