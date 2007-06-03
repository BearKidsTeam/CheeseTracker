//
// C++ Implementation: saver_ct
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "saver_ct.h"
#include "components/data/dds_packer.h"
#include "zlib.h"

#define FILE_FORMAT_MAJOR 1
#define FILE_FORMAT_MINOR 1

void Saver_CT::get_dds_from_sample(Sample *p_sample,DDS *p_dds) {

	p_dds->set_str_var("name",p_sample->name);
	p_dds->set_int_var("def_volume",p_sample->def_volume);
	p_dds->set_int_var("glb_volume",p_sample->glb_volume);
	p_dds->set_int_var("def_panning_active",p_sample->def_panning_on);
	p_dds->set_int_var("def_panning",p_sample->def_panning);
	p_dds->set_int_var("vibrato_type",p_sample->vibrato_type);
	p_dds->set_int_var("vibrato_speed",p_sample->vibrato_speed);
	p_dds->set_int_var("vibrato_depth",p_sample->vibrato_depth);
	p_dds->set_int_var("vibrato_rate",p_sample->vibrato_rate);

	p_dds->set_int_var("in_use",p_sample->in_use);

	if (!p_sample->in_use)
		return; //no need to save data
	DDS data;
	DDS_Helpers::get_sample_data(&p_sample->data,&data);

	p_dds->set_dds_var("data",data);

}

void Saver_CT::get_dds_from_pattern(Pattern *p_pattern,DDS *p_dds) {

	// Just using the IT Algorithm...

	Note note_cache[PATTERN_WIDTH];
	bool first_parameter[PATTERN_WIDTH];
	Uint8 mask_cache[PATTERN_WIDTH];
	vector_int data;

	Note empty_note;
        int i,j;
//	int length;
//	Uint32 begin_pos;
//	Uint32 end_pos;

	enum {

		WRITE_NOTE=(1),
		WRITE_INSTRUMENT=(1<<1),
		WRITE_VOLUME=(1<<2),
		WRITE_COMMAND=(1<<3),
		WRITE_NOTE_MASK=(1<<4),
		WRITE_INSTRUMENT_MASK=(1<<5),
		WRITE_VOLUME_MASK=(1<<6),
		WRITE_COMMAND_MASK=(1<<7)
	};

	for(j=0;j<PATTERN_WIDTH;j++) {

		first_parameter[j]=true;
		mask_cache[j]=0;
	}

	p_dds->set_int_var("rows",p_pattern->get_length());

	for (i=0;i<p_pattern->get_length();i++) {

		for(j=0;j<PATTERN_WIDTH;j++) {

			if (!(empty_note==p_pattern->get_note(j,i))) {
	                        // we have some data, boss

				int flags=0;
				Note src_note=p_pattern->get_note(j,i);

				//Check for what changed, and fill cache!

				if (src_note.note!=EMPTY_FIELD) {

					if (src_note.note!=note_cache[j].note) {

						flags|=WRITE_NOTE;
						note_cache[j].note=src_note.note;

					} else {

						flags|=WRITE_NOTE_MASK;
					}
				}

				if (src_note.instrument!=EMPTY_FIELD) {

					if (src_note.instrument!=note_cache[j].instrument) {

						flags|=WRITE_INSTRUMENT;
						note_cache[j].instrument=src_note.instrument;

					} else {

						flags|=WRITE_INSTRUMENT_MASK;
					}
				}


				if (src_note.volume!=EMPTY_FIELD) {

					if (src_note.volume!=note_cache[j].volume) {

						flags|=WRITE_VOLUME;
						note_cache[j].volume=src_note.volume;

					} else {

						flags|=WRITE_VOLUME_MASK;
					}
				}

				if ((src_note.command!=EMPTY_FIELD) || (src_note.parameter!=0)){

					if ((src_note.command!=note_cache[j].command) || (src_note.parameter!=note_cache[j].parameter)) {

						flags|=WRITE_COMMAND;
						note_cache[j].command=src_note.command;
						note_cache[j].parameter=src_note.parameter;

					} else {

						flags|=WRITE_COMMAND_MASK;
					}
				}

				//Now let's write the note

				//first the flags
				if (flags!=mask_cache[j]) {

					int tmp_chanmark;
					tmp_chanmark=(j+1)|128;
					data.push_back(tmp_chanmark);
					data.push_back(flags);
					mask_cache[j]=flags;
				} else {
                                        int tmp_chanmark;
					tmp_chanmark=(j+1);
					data.push_back(tmp_chanmark);
				}

				if (flags&WRITE_NOTE) {

					Uint8 dest_note;

					if (src_note.note==EMPTY_FIELD) {

                                         	dest_note=0;
					} else if (src_note.note==Note::OFF) {

                                         	dest_note=255;
					} else if (src_note.note==Note::CUT) {

                                         	dest_note=254;
					} else {

                                         	dest_note=src_note.note;
                                        }

					data.push_back(dest_note);

				}

				if (flags&WRITE_INSTRUMENT) {

					data.push_back(src_note.instrument+1);


				}

				if (flags&WRITE_VOLUME) {

					data.push_back(src_note.volume);

				}

				if (flags&WRITE_COMMAND) {

					data.push_back(src_note.command+1);
					data.push_back(src_note.parameter);

				}

			}
		}
		data.push_back(0);
	}


	p_dds->set_int_array_var("compressed_block",data);
}

void Saver_CT::get_dds_from_instrument(Instrument *p_instr,DDS *p_dds,bool p_include_samples) {



	p_dds->set_str_var("name",p_instr->name);
	p_dds->set_int_var("channel_output",p_instr->buffer.output_index);

	bool got_it[MAX_SAMPLES];
	memset(got_it,0,sizeof(bool)*MAX_SAMPLES);
	vector<int> sample_trans_table;

	if (p_include_samples) {
		for (int i=0;i<Instrument::MAX_LAYERS;i++) {

			for (int j=0;j<Note::NOTES;j++) {

				int samp_idx=p_instr->get_layer(i)->sample_number[j];
				if ((samp_idx<0) || (samp_idx>=MAX_SAMPLES))
					continue;
				if (got_it[samp_idx])
					continue;

				sample_trans_table.push_back(samp_idx);

				got_it[samp_idx]=true;
			}
		}
	}

	vector_dds layers;
	for (int i=0;i<Instrument::MAX_LAYERS;i++) {

		Instrument::Layer *lay=p_instr->get_layer(i);
		DDS layer;
		layer.set_int_var("override_instrument_output",lay->override_instrument_output);
		layer.set_int_var("channel_output",lay->output_index);
		layer.set_int_var("nna_type",lay->NNA_type);
		layer.set_int_var("dc_type",lay->duplicate_check_type);
		layer.set_int_var("dc_action",lay->duplicate_check_action);

		DDS envelope;
		DDS_Helpers::get_envelope(&lay->volume.envelope,&envelope);
		layer.set_dds_var("volume_envelope",envelope);
		envelope.clear();
		DDS_Helpers::get_envelope(&lay->panning.envelope,&envelope);
		layer.set_dds_var("panning_envelope",envelope);
		envelope.clear();
		DDS_Helpers::get_envelope(&lay->pitch.envelope,&envelope);
		layer.set_dds_var("pitch_envelope",envelope);
		envelope.clear();

		layer.set_int_var("volume_amount",lay->volume.global_amount);
		layer.set_int_var("volume_fadeuout",lay->volume.fadeout);
		layer.set_int_var("volume_random",lay->volume.random_variation);

		layer.set_int_var("pan_use_default",lay->panning.use_default);
		layer.set_int_var("pan_default",lay->panning.default_amount);
		layer.set_int_var("pitch_separation",lay->panning.pitch_separation);
		layer.set_int_var("pitch_center",lay->panning.pitch_center);
		layer.set_int_var("pitch_center",lay->panning.pitch_center);
		layer.set_int_var("pan_random",lay->panning.random_variation);

		layer.set_int_var("pitch_use_as_filter",lay->pitch.use_as_filter);
		layer.set_int_var("pitch_use_default_cutoff",lay->pitch.use_default_cutoff);
		layer.set_int_var("pitch_use_default_Q",lay->pitch.use_default_Q);
		layer.set_int_var("pitch_default_cutoff",lay->pitch.default_cutoff);
		layer.set_int_var("pitch_default_Q",lay->pitch.default_Q);
		layer.set_int_var("pitch_filter_type",0); //reserved for now

		vector_int notes;
		for (int j=0;j<Note::NOTES;j++) {

			notes.push_back(lay->note_number[j]);
		}

		layer.set_int_array_var("note_table",notes);

		vector_int samples;

		if (p_include_samples) {

			for (int j=0;j<Note::NOTES;j++) {

				int samp_idx=lay->sample_number[j];
				if ((samp_idx<0) || (samp_idx>=MAX_SAMPLES)) {

					samples.push_back(samp_idx);
				} else {

					bool found=false;
					for (int k=0;k<(int)sample_trans_table.size();k++) {

						if (sample_trans_table[k]==samp_idx) {

							found=true;
							samples.push_back(k);
							break;
						}
					}

					if (!found)
						ERROR("something stinks!");
				}
			}
		} else {
			for (int j=0;j<Note::NOTES;j++)
				samples.push_back(lay->sample_number[j]);
		}

		layer.set_int_array_var("sample_table",samples);
		layers.push_back(layer);
	}

	p_dds->set_dds_array_var("layers",layers);

	if (p_include_samples) {

		vector_dds samples;

		for (int k=0;k<(int)sample_trans_table.size();k++) {

			Sample* sample=song->get_sample( sample_trans_table[k] );
			if (!sample)
				continue;

			DDS samp;
			get_dds_from_sample(sample,&samp);
			samples.push_back(samp);
		}

		p_dds->set_dds_array_var("samples",samples);
	}


}

int Saver_CT::save(const char *p_filename) {

 	string fname=p_filename,ext;
	ext=fname.substr(fname.length()-3,3);
	printf("file is %s\n",p_filename);
	if ((fname.length()<5) || ((ext!=".ct") && (ext!=".CT")) ) return UNKNOWN_TYPE;
	printf("type recognized\n");

	if (writer.open(p_filename)) return SAVE_ERROR;
	printf("writer going\n");

	DDS base;

        DDS variables;
	vector_dds aux_vec;

	variables.set_str_var("title",song->variables.name);
	variables.set_str_var("message",song->variables.message);
	variables.set_int_var("row_hilight_major",song->variables.row_highlight_major);
	variables.set_int_var("row_hilight_minor",song->variables.row_highlight_minor);
	variables.set_int_var("mixing_volume",song->variables.mixing_volume);
	variables.set_int_var("stereo_separation",song->variables.stereo_separation);
	variables.set_int_var("use_stereo",song->variables.use_stereo);
	variables.set_int_var("use_linear_slides",song->variables.use_linear_slides);
	variables.set_int_var("old_effects",song->variables.old_effects);
	variables.set_int_var("compatible_gxx",song->variables.compatible_gxx);

	base.set_dds_var("variables",variables);

	variables.clear();

	variables.set_int_var("global_volume",song->initial_variables.global_volume);
	variables.set_int_var("speed",song->initial_variables.speed);
	variables.set_int_var("tempo",song->initial_variables.tempo);

	variables.set_dds_array_var("channel",aux_vec);
	vector_dds &channel_props=variables.get_dds_array_var("channel");

	for (int i=0;i<PATTERN_WIDTH;i++) {
        	DDS dds;
		dds.set_int_var("pan",song->initial_variables.channel[i].pan);
		dds.set_int_var("volume",song->initial_variables.channel[i].volume);
		dds.set_int_var("surround",song->initial_variables.channel[i].surround);
		dds.set_int_var("mute",song->initial_variables.channel[i].mute);
                channel_props.push_back(dds);
	}

	base.set_dds_var("initial_variables",variables);
	variables.clear();

	vector_int order;
	order.reserve(MAX_ORDERS);
	for (int i=0;i<MAX_ORDERS;i++) {

		order.push_back( song->get_order(i) );
	}

	base.set_int_array_var("order_list",order);
	order.clear();

	base.set_dds_array_var("samples",aux_vec);
	vector_dds &samples=base.get_dds_array_var("samples");

	for (int i=0;i<MAX_SAMPLES;i++) {

		DDS dds;
		if (!song->get_sample(i)->in_use)
			continue;
		get_dds_from_sample(song->get_sample(i),&dds);
		dds.set_int_var("index",i);
		samples.push_back(dds);
	}

	base.set_dds_array_var("instruments",aux_vec);
	vector_dds &instruments=base.get_dds_array_var("instruments");
//	instruments.reserve(MAX_INSTRUMENTS);
	for (int i=0;i<MAX_INSTRUMENTS;i++) {

		DDS dds;

		get_dds_from_instrument(song->get_instrument(i),&dds);
		dds.set_int_var("index",i);
		instruments.push_back(dds);
	}

	base.set_dds_array_var("patterns",aux_vec);
	vector_dds &patterns=base.get_dds_array_var("patterns");

	for (int i=0;i<MAX_PATTERNS;i++) {

		DDS dds;
		if (!song->get_pattern(i)->in_use())
			continue;

		get_dds_from_pattern(song->get_pattern(i),&dds);
		dds.set_int_var("index",i);
		patterns.push_back(dds);
	}

        DDS mixerdds;
	DDS_Helpers::get_mixer_data(mixer,&mixerdds);
	base.set_dds_var("mixer",mixerdds);


	writer.store_byte('C');
	writer.store_byte('H');
	writer.store_byte('T');
	writer.store_byte('M'); //cheese tracker module!
	writer.store_byte(FILE_FORMAT_MAJOR);
	writer.store_byte(FILE_FORMAT_MINOR);


        vector<Uint8> file;

	printf("creating file...\n");
	DDSPacker::pack(&base, file, 0);

        vector<Uint8> file_compressed;
        file_compressed.resize(file.size());
	unsigned long dst_size=file.size();
	int res=compress(&file_compressed[0],&dst_size,&file[0],file.size());
	if (res!=Z_OK) {
	 	writer.close();
		ERROR("cant zlib compress this!");
		switch (res) {
			case Z_MEM_ERROR: ERROR("Not enough memory!"); break;
			case Z_BUF_ERROR: ERROR("DST Buffer is to small?"); break;
		}
		return SAVE_ERROR;
	}

	writer.store_dword(file.size());
	writer.store_dword(dst_size);
	writer.store_byte_array(&file_compressed[0],dst_size);

	if (writer.error_saving()) {

	 	writer.close();
		return SAVE_ERROR;
	}
	printf("writer no error\n");

 	writer.close();

	return FUNCTION_SUCCESS;

}

int Saver_CT::save_sample(const char *p_filename,int p_sample_index) {

 	string fname=p_filename,ext;
	ext=fname.substr(fname.length()-3,3);
	if ((fname.length()<5) || ((ext!=".cs") && (ext!=".CS")) ) return UNKNOWN_TYPE;

	if (writer.open(p_filename)) return SAVE_ERROR;

	writer.store_byte('C');
	writer.store_byte('H');
	writer.store_byte('T');
	writer.store_byte('S'); //cheese tracker sample!
	writer.store_byte(FILE_FORMAT_MAJOR);
	writer.store_byte(FILE_FORMAT_MINOR);

        DDS base;

	get_dds_from_sample(song->get_sample(p_sample_index),&base);

        vector<Uint8> file;

	printf("creating file...\n");
	DDSPacker::pack(&base, file, 0);

        vector<Uint8> file_compressed;
        file_compressed.resize(file.size());
	unsigned long dst_size=file.size();
	int res=compress(&file_compressed[0],&dst_size,&file[0],file.size());
	if (res!=Z_OK) {
	 	writer.close();
		ERROR("cant zlib compress this!");
		switch (res) {
			case Z_MEM_ERROR: ERROR("Not enough memory!"); break;
			case Z_BUF_ERROR: ERROR("DST Buffer is to small?"); break;
		}
		return SAVE_ERROR;
	}

	writer.store_dword(file.size());
	writer.store_dword(dst_size);
	writer.store_byte_array(&file_compressed[0],dst_size);

	if (writer.error_saving()) {

	 	writer.close();
		return SAVE_ERROR;
	}
	printf("writer no error\n");

 	writer.close();

	return FUNCTION_SUCCESS;
}

int Saver_CT::save_instrument(const char *p_filename,int p_instrument_index) {
 	string fname=p_filename,ext;
	ext=fname.substr(fname.length()-3,3);

	if ((fname.length()<5) || ((ext!=".ci") && (ext!=".CI")) ) return UNKNOWN_TYPE;

	if (writer.open(p_filename)) return SAVE_ERROR;

	writer.store_byte('C');
	writer.store_byte('H');
	writer.store_byte('T');
	writer.store_byte('I'); //cheese tracker sample!
	writer.store_byte(FILE_FORMAT_MAJOR);
	writer.store_byte(FILE_FORMAT_MINOR);

	DDS base;

	get_dds_from_instrument(song->get_instrument(p_instrument_index),&base,true);

	vector<Uint8> file;

	printf("creating file...\n");
	DDSPacker::pack(&base, file, 0);

	vector<Uint8> file_compressed;
	file_compressed.resize(file.size());
	unsigned long dst_size=file.size();
	int res=compress(&file_compressed[0],&dst_size,&file[0],file.size());

	if (res!=Z_OK) {
		writer.close();
		ERROR("cant zlib compress this!");
		switch (res) {
			case Z_MEM_ERROR: ERROR("Not enough memory!"); break;
			case Z_BUF_ERROR: ERROR("DST Buffer is to small?"); break;
		}
		return SAVE_ERROR;
	}

	writer.store_dword(file.size());
	writer.store_dword(dst_size);
	writer.store_byte_array(&file_compressed[0],dst_size);

	if (writer.error_saving()) {

		writer.close();
		return SAVE_ERROR;
	}

	printf("writer no error\n");

	writer.close();

	return FUNCTION_SUCCESS;

}


Saver_CT::~Saver_CT()
{
}


