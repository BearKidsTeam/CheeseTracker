//
// C++ Implementation: loader_ct
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "loader_ct.h"
#include "zlib.h"
#include "components/data/dds_packer.h"

#define FILE_FORMAT_MAJOR 1
#define FILE_FORMAT_MINOR 2



void Loader_CT::set_dds_to_sample(DDS *p_dds,Sample *p_sample) {

	p_sample->prop_name.set( basename(p_dds->get_str_var("name").c_str()) );
	p_sample->prop_def_volume.set( p_dds->get_int_var("def_volume") );
	p_sample->prop_glb_volume.set( p_dds->get_int_var("glb_volume") );
	p_sample->prop_def_panning_on .set( p_dds->get_int_var("def_panning_active") );
	p_sample->prop_def_panning.set( p_dds->get_int_var("def_panning") );
	p_sample->prop_vibrato_type.set( p_dds->get_int_var("vibrato_type") );
	p_sample->prop_vibrato_speed.set( p_dds->get_int_var("vibrato_speed") );
	p_sample->prop_vibrato_depth.set( p_dds->get_int_var("vibrato_depth") );
	p_sample->prop_vibrato_rate.set( p_dds->get_int_var("vibrato_rate") );

	p_sample->in_use = ( p_dds->get_int_var("in_use") );

	if (p_sample->in_use) {
		DDS &data=p_dds->get_dds_var("data");
		DDS_Helpers::set_sample_data(&data,&p_sample->data);
	}
}

void Loader_CT::set_dds_to_instrument(DDS *p_dds,Instrument *p_instr) {

	p_instr->prop_name.set( p_dds->get_str_var("name") );
	p_instr->prop_buffer.set( p_dds->get_int_var("channel_output") );
        int layer_count=p_dds->get_dds_array_var("layers").size();
	if (layer_count>Instrument::MAX_LAYERS) {

		ERROR("This version doesnt support more than " << Instrument::MAX_LAYERS << " layers.");
		layer_count=Instrument::MAX_LAYERS;
	}

	bool includes_samples=p_dds->is_var("samples");

	printf("includes samples.. %i\n",includes_samples);
	int sample_count=0;
	vector<int> sample_trans_table;

	if (includes_samples) {

		sample_count=p_dds->get_dds_array_var("samples").size();
		printf("%i samples\n",sample_count);

		vector_dds &samples=p_dds->get_dds_array_var("samples");

		for (int k=0;k<(int)samples.size();k++) {

			int smp_idx=song->find_unused_sample_slot();

			Sample* sample=song->get_sample( smp_idx );
			if (!sample)
				continue;
			set_dds_to_sample(&samples[k],sample);
			sample_trans_table.push_back(smp_idx);
		}
	}

	vector_dds layers;
	for (int i=0;i<layer_count;i++) {

		Instrument::Layer *lay=p_instr->get_layer(i);
		DDS &layer =p_dds->get_dds_array_var("layers")[i];

		lay->override_instrument_output=layer.get_int_var("override_instrument_output");
		lay->output_index=layer.get_int_var("channel_output");
		lay->NNA_type=layer.get_int_var("nna_type");
		lay->duplicate_check_type=layer.get_int_var("dc_type");
		lay->duplicate_check_action=layer.get_int_var("dc_action");


		DDS_Helpers::set_envelope(&layer.get_dds_var("volume_envelope"),&lay->volume.envelope);
		DDS_Helpers::set_envelope(&layer.get_dds_var("panning_envelope"),&lay->panning.envelope);
		DDS_Helpers::set_envelope(&layer.get_dds_var("pitch_envelope"),&lay->pitch.envelope);


		lay->prop_volume_global_amount.set( layer.get_int_var("volume_amount") );
		lay->prop_volume_fadeout.set( layer.get_int_var("volume_fadeuout") );
		lay->prop_volume_random.set( layer.get_int_var("volume_random") );

		lay->prop_panning_use_default.set( layer.get_int_var("pan_use_default") );
		lay->prop_panning_default_amount.set( layer.get_int_var("pan_default") );
		lay->prop_panning_pitch_separation.set( layer.get_int_var("pitch_separation") );
		lay->prop_panning_pitch_center.set( layer.get_int_var("pitch_center") );
		lay->prop_panning_pitch_center.set( layer.get_int_var("pitch_center") );
		lay->prop_panning_random_variation.set( layer.get_int_var("pan_random") );

		lay->pitch.use_as_filter = layer.get_int_var("pitch_use_as_filter");
		lay->pitch_use_default_cutoff.set( layer.get_int_var("pitch_use_default_cutoff") );
		lay->pitch_use_default_Q.set( layer.get_int_var("pitch_use_default_Q") );
		lay->pitch_default_cutoff.set( layer.get_int_var("pitch_default_cutoff") );
		lay->pitch_default_Q.set( layer.get_int_var("pitch_default_Q") );

		const vector_int &notes=layer.get_int_array_var("note_table");

		for (int j=0;j<(int)notes.size();j++) {

			lay->note_number[j]=notes[j];
		}


		const vector_int &samples = layer.get_int_array_var("sample_table");

		if (includes_samples) {
			for (int j=0;j<Note::NOTES;j++) {

				int samp_idx=samples[j];

				if ((samp_idx<0) || (samp_idx>=(int)sample_trans_table.size()))
					lay->sample_number[j]=EMPTY_FIELD;
				else
					lay->sample_number[j]=sample_trans_table[samp_idx];
			}
		} else {
			for (int j=0;j<(int)samples.size();j++)
				lay->sample_number[j]=samples[j];
		}

	}


}

void Loader_CT::set_dds_to_pattern(DDS *p_dds,Pattern *p_pattern) {


	Uint16 pat_length;

	int row=0,flag,channel,j;
	Uint8 aux_byte;
//	Uint32 reserved;
	Uint8 chan_mask[64];
	Note last_value[64];

	for (j=0;j<64;j++) {

		chan_mask[j]=0;
		last_value[j].clear();
	}
	pat_length=p_dds->get_int_var("rows");
	printf("this has %i rows\n",pat_length);
	p_pattern->set_length( pat_length );
	const vector_int &comp_block=p_dds->get_int_array_var("compressed_block");
	int read_idx=0;


	do {

		aux_byte=comp_block[read_idx++];
		flag=aux_byte;

		if ( flag==0 ) {

			row++;
		} else {

			channel=(flag-1) & 63;

			if ( flag & 128 ) {

				aux_byte=comp_block[read_idx++];
				chan_mask[channel]=aux_byte;
			}


			if ( chan_mask[channel]&1 ) { // read note

				aux_byte=comp_block[read_idx++];

				if ( aux_byte<120 ) p_pattern->get_note_ref(channel,row).note=aux_byte;
				else if ( aux_byte==255 ) p_pattern->get_note_ref(channel,row).note=Note::OFF;
				else if ( aux_byte==254 ) p_pattern->get_note_ref(channel,row).note=Note::CUT;
				else p_pattern->get_note_ref(channel,row).note=EMPTY_FIELD;

				last_value[channel].note=p_pattern->get_note(channel,row).note;
			}


			if ( chan_mask[channel]&2 ) {

				aux_byte=comp_block[read_idx++];
				if ( aux_byte<100 ) p_pattern->get_note_ref(channel,row).instrument=aux_byte-1;
				else p_pattern->get_note_ref(channel,row).instrument=EMPTY_FIELD;

				last_value[channel].instrument=p_pattern->get_note(channel,row).instrument;
			}
			if ( chan_mask[channel]&4 ) {

				aux_byte=comp_block[read_idx++];
				if ( aux_byte<213 ) p_pattern->get_note_ref(channel,row).volume=aux_byte;
				else p_pattern->get_note_ref(channel,row).volume=EMPTY_FIELD;

				last_value[channel].volume=p_pattern->get_note(channel,row).volume;
			}
			if ( chan_mask[channel]&8 ) {

				aux_byte=comp_block[read_idx++];
				if ( aux_byte<213 ) p_pattern->get_note_ref(channel,row).command=aux_byte-1;
				else p_pattern->get_note_ref(channel,row).command=EMPTY_FIELD;
				last_value[channel].command=p_pattern->get_note(channel,row).command;

				aux_byte=comp_block[read_idx++];
				p_pattern->get_note_ref(channel,row).parameter=aux_byte;
				last_value[channel].parameter=p_pattern->get_note(channel,row).parameter;
			}

			if ( chan_mask[channel]&16 ) {

				p_pattern->get_note_ref(channel,row).note=last_value[channel].note;
			}

			if ( chan_mask[channel]&32 ) {

				p_pattern->get_note_ref(channel,row).instrument=last_value[channel].instrument;
			}
			if ( chan_mask[channel]&64 ) {

				p_pattern->get_note_ref(channel,row).volume=last_value[channel].volume;
			}
			if ( chan_mask[channel]&128 ) {

				p_pattern->get_note_ref(channel,row).command=last_value[channel].command;
				p_pattern->get_note_ref(channel,row).parameter=last_value[channel].parameter;
			}

		}
	} while(row<pat_length);

}



Loader::Error Loader_CT::load(const char *p_filename,bool p_load_patterns) {

	if (file_read.open(p_filename)) return CANNOT_OPEN_FILE;


	printf("loader going\n");

	char type[5];
	type[4]=0;

	file_read.get_byte_array((Uint8*)type,4);
	if ((string)type!="CHTM") {
		file_read.close();
		return FILE_FORMAT_NOT_RECOGNIZED;
	}

	Uint8 format_major=file_read.get_byte();
	Uint8 format_minor=file_read.get_byte();

	if (format_major>FILE_FORMAT_MAJOR) {

		ERROR("File format is too new!! I cant read this");
		file_read.close();
		return FILE_FORMAT_NOT_RECOGNIZED;
	} else if (format_minor>FILE_FORMAT_MINOR) {

		ERROR("This file was created with a sightly newer version of this program, consider upgrading");
	}


	Uint32 length=file_read.get_dword();
	Uint32 compressed_length=file_read.get_dword();
	if (compressed_length>length) {
		file_read.close();
                ERROR("File compressed cant be greater than uncompressed");
		return FILE_FORMAT_NOT_RECOGNIZED;
	}


        vector<Uint8> file_compressed;
	file_compressed.resize(compressed_length);
        vector<Uint8> file;
	file.resize(length);
	printf("reading file...\n");
	file_read.get_byte_array(&file_compressed[0],compressed_length);
	printf("decompressing file...\n");

	uLongf blah=length;
	int res=uncompress(&file[0],&blah,&file_compressed[0],compressed_length);
	if (res!=Z_OK) {

		file_read.close();
                ERROR("Couldnt decompress buffer!");
		return FILE_FORMAT_NOT_RECOGNIZED;
	}

	printf("unpacking file...\n");
	DDSPacker::unpack(&data,&file[0],length);

        file_read.close();

	return SUCCESS;
}

bool Loader_CT::test(const char *p_filename) {

	if (file_read.open(p_filename)) return false;

	char type[5];
	type[4]=0;

	file_read.get_byte_array((Uint8*)type,4);
	if ((string)type!="CHTM") {
		file_read.close();
		return false;
	}

	Uint8 format_major=file_read.get_byte();
	Uint8 format_minor=file_read.get_byte();

	if (format_major>FILE_FORMAT_MAJOR) {

		ERROR("File format is too new!! I cant read this");
		file_read.close();
		return false;
	} else if (format_minor>FILE_FORMAT_MINOR) {

		ERROR("This file was created with a sightly newer version of this program, consider upgrading");
	}

	file_read.close();

	return true;

}



/* Compute Sample Info */
int Loader_CT::get_amount_of_samples() {

	return 0;
}

Sample_Data *Loader_CT::get_sample_data(int p_sample_index) {

	return NULL;
}

string Loader_CT::get_sample_name(int p_sample_index) {

	return "";
}

void Loader_CT::add_sample_to_song(int p_sample_index,int p_dest_index,bool create_instrument) {


}


Loader::Error Loader_CT::load_sample(const char *p_filename,int p_dest_index) {

	if (file_read.open(p_filename)) return CANNOT_OPEN_FILE;

	printf("loader going\n");

	char type[5];
	type[4]=0;

	file_read.get_byte_array((Uint8*)type,4);
	if ((string)type!="CHTS") {
		file_read.close();
		return FILE_FORMAT_NOT_RECOGNIZED;
	}

	Uint8 format_major=file_read.get_byte();
	Uint8 format_minor=file_read.get_byte();

	if (format_major>FILE_FORMAT_MAJOR) {

		ERROR("File format is too new!! I cant read this");
		file_read.close();
		return FILE_FORMAT_NOT_RECOGNIZED;
	} else if (format_minor>FILE_FORMAT_MINOR) {

		ERROR("This file was created with a sightly newer version of this program, consider upgrading");
	}


	Uint32 length=file_read.get_dword();
	Uint32 compressed_length=file_read.get_dword();
	if (compressed_length>length) {
		file_read.close();
                ERROR("File compressed cant be greater than uncompressed");
		return FILE_FORMAT_NOT_RECOGNIZED;
	}


        vector<Uint8> file_compressed;
	file_compressed.resize(compressed_length);
        vector<Uint8> file;
	file.resize(length);
	printf("reading file...\n");
	file_read.get_byte_array(&file_compressed[0],compressed_length);
	printf("decompressing file...\n");

	uLongf blah=length;
	int res=uncompress(&file[0],&blah,&file_compressed[0],compressed_length);
	if (res!=Z_OK) {

		file_read.close();
                ERROR("Couldnt decompress buffer!");
		return FILE_FORMAT_NOT_RECOGNIZED;
	}
	file_read.close();

	DDS base;
	printf("unpacking file...\n");
	DDSPacker::unpack(&base,&file[0],length);

	song->get_sample(p_dest_index)->reset();
	set_dds_to_sample(&base,song->get_sample(p_dest_index));
	
        
	return SUCCESS;
}

Sample_Data *Loader_CT::load_sample(const char *p_filename) {

	if (file_read.open(p_filename)) return NULL;

	printf("loader going\n");

	char type[5];
	type[4]=0;

	file_read.get_byte_array((Uint8*)type,4);
	if ((string)type!="CHTS") {
		file_read.close();
		return NULL;
	}

	Uint8 format_major=file_read.get_byte();
	Uint8 format_minor=file_read.get_byte();

	if (format_major>FILE_FORMAT_MAJOR) {

		ERROR("File format is too new!! I cant read this");
		file_read.close();
		return NULL;
	} else if (format_minor>FILE_FORMAT_MINOR) {

		ERROR("This file was created with a sightly newer version of this program, consider upgrading");
	}


	Uint32 length=file_read.get_dword();
	Uint32 compressed_length=file_read.get_dword();
	if (compressed_length>length) {
		file_read.close();
                ERROR("File compressed cant be greater than uncompressed");
		return NULL;
	}


        vector<Uint8> file_compressed;
	file_compressed.resize(compressed_length);
        vector<Uint8> file;
	file.resize(length);
	printf("reading file...\n");
	file_read.get_byte_array(&file_compressed[0],compressed_length);
	printf("decompressing file...\n");

	uLongf blah=length;
	int res=uncompress(&file[0],&blah,&file_compressed[0],compressed_length);
	if (res!=Z_OK) {

		file_read.close();
                ERROR("Couldnt decompress buffer!");
		return NULL;
	}
	file_read.close();

	DDS base;
	printf("unpacking file...\n");
	DDSPacker::unpack(&base,&file[0],length);

	if (!base.get_int_var("in_use"))
		return NULL;
	Sample_Data * sdata= new Sample_Data;
	DDS_Helpers::set_sample_data(&base.get_dds_var("data"),sdata);
	return sdata;
}

Loader::Error Loader_CT::load_samples_from_instrument(const char *p_filename) {

	return FILE_ERROR;
}


/* Compute Instrument Info */
Loader::Error Loader_CT::load_instrument(const char *p_filename,int p_dest_index) {

	if (file_read.open(p_filename)) return CANNOT_OPEN_FILE;

	printf("loader going\n");

	char type[5];
	type[4]=0;

	file_read.get_byte_array((Uint8*)type,4);
	if ((string)type!="CHTI") {
		file_read.close();
		return FILE_FORMAT_NOT_RECOGNIZED;
	}

	Uint8 format_major=file_read.get_byte();
	Uint8 format_minor=file_read.get_byte();

	if (format_major>FILE_FORMAT_MAJOR) {

		ERROR("File format is too new!! I cant read this");
		file_read.close();
		return FILE_FORMAT_NOT_RECOGNIZED;
	} else if (format_minor>FILE_FORMAT_MINOR) {

		ERROR("This file was created with a sightly newer version of this program, consider upgrading");
	}


	Uint32 length=file_read.get_dword();
	Uint32 compressed_length=file_read.get_dword();
	if (compressed_length>length) {
		file_read.close();
                ERROR("File compressed cant be greater than uncompressed");
		return FILE_FORMAT_NOT_RECOGNIZED;
	}


        vector<Uint8> file_compressed;
	file_compressed.resize(compressed_length);
        vector<Uint8> file;
	file.resize(length);
	printf("reading file...\n");
	file_read.get_byte_array(&file_compressed[0],compressed_length);
	printf("decompressing file...\n");

	uLongf blah=length;
	int res=uncompress(&file[0],&blah,&file_compressed[0],compressed_length);
	if (res!=Z_OK) {

		file_read.close();
                ERROR("Couldnt decompress buffer!");
		return FILE_FORMAT_NOT_RECOGNIZED;
	}
	file_read.close();

	DDS base;
	printf("unpacking file...\n");
	DDSPacker::unpack(&base,&file[0],length);

	set_dds_to_instrument(&base,song->get_instrument(p_dest_index));

	return FILE_ERROR;
}


void Loader_CT::transfer_data_to_song() {

	if (!data.is_var("variables")) {

		ERROR("Broken file on purpose?!!!");
		return;
	}
        DDS &variables=data.get_dds_var("variables");
	vector_dds aux_vec;

	song->variables.prop_name.set( variables.get_str_var("title") );
	song->variables.prop_message.set( variables.get_str_var("message") );
	song->variables.prop_hilight_major.set( variables.get_int_var("row_hilight_major") );
	song->variables.prop_hilight_minor.set( variables.get_int_var("row_hilight_minor") );
	song->variables.prop_mixing_volume.set( variables.get_int_var("mixing_volume") );
	song->variables.prop_stereo_separation.set( variables.get_int_var("stereo_separation") );
	song->variables.prop_use_stereo.set( variables.get_int_var("use_stereo") );
	song->variables.prop_use_linear_slides.set( variables.get_int_var("use_linear_slides") );
	song->variables.prop_old_effects.set( variables.get_int_var("old_effects") );
	song->variables.prop_compatible_gxx.set( variables.get_int_var("compatible_gxx") );


	if (!data.is_var("initial_variables")) {

		ERROR("Broken file on purpose, initial variables?!!!");
		return;
	}

        DDS &ivariables=data.get_dds_var("initial_variables");

	song->initial_variables.prop_global_volume.set( ivariables.get_int_var("global_volume") );
	song->initial_variables.prop_speed.set( ivariables.get_int_var("speed")  );
	song->initial_variables.prop_tempo.set( ivariables.get_int_var("tempo")  );


	if (!ivariables.is_var("channel")) {

		ERROR("Broken file on purpose, initial channel vals?!!!");
		return;
	}

	vector_dds &channel_props=ivariables.get_dds_array_var("channel");

	for (int i=0;i<(int)channel_props.size();i++) {
        	DDS &dds = channel_props[i];
		song->initial_variables.channel[i].prop_pan.set( dds.get_int_var("pan") );
		song->initial_variables.channel[i].prop_volume.set( dds.get_int_var("volume") );
		song->initial_variables.channel[i].prop_surround.set( dds.get_int_var("surround") );
		song->initial_variables.channel[i].prop_mute.set( dds.get_int_var("mute") );
	}

	if (!data.is_var("order_list")) {

		ERROR("Broken file on purpose, order_list?!!!");
		return;
	}

	const vector_int &order=data.get_int_array_var("order_list");
	for (int i=0;i<(int)order.size();i++) {
		song->set_order(i, order[i]);
	}

	if (!data.is_var("samples")) {

		ERROR("Broken file on purpose, samples?!!!");
		return;
	}

	vector_dds &samples=data.get_dds_array_var("samples");

	for (int i=0;i<(int)samples.size();i++) {

		DDS &dds=samples[i];
		if (!dds.is_var("index")) {
			ERROR("No index in sample?");
			continue;
		}
		int index=dds.get_int_var("index");
		set_dds_to_sample(&dds,song->get_sample(index));
	}
	if (!data.is_var("instruments")) {

		ERROR("Broken file on purpose, instruments?!!!");
		return;
	}

	vector_dds &instruments=data.get_dds_array_var("instruments");
//	instruments.reserve(MAX_INSTRUMENTS);
	for (int i=0;i<(int)instruments.size();i++) {

		DDS &dds=instruments[i];
		if (!dds.is_var("index")) {
			ERROR("No index in instrument?");
			continue;
		}
		int index=dds.get_int_var("index");

		set_dds_to_instrument(&dds,song->get_instrument(index));
	}

	if (!data.is_var("patterns")) {

		ERROR("Broken file on purpose, patterns?!!!");
		return;
	}

	vector_dds &patterns=data.get_dds_array_var("patterns");

	for (int i=0;i<(int)patterns.size();i++) {

		DDS &dds=patterns[i];
		if (!dds.is_var("index")) {
			ERROR("No index in pattern?");
			continue;
		}
		int index=dds.get_int_var("index");

		set_dds_to_pattern(&dds,song->get_pattern(index));
	}

	if (!data.is_var("mixer")) {

		ERROR("Broken file on purpose, mixer?!!!");
		return;
	}

        DDS &mixerdds=data.get_dds_var("mixer");
	DDS_Helpers::set_mixer_data(&mixerdds,mixer);

	printf("loader finished\n");
	return;
}


void Loader_CT::free_info(bool free_sampledata) {

	data.clear();

}


Loader_CT::Loader_CT() {

	format_name="Cheese Tracker";
}



Loader_CT::~Loader_CT()
{
}


