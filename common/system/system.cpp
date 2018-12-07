//
// C++ Implementation: component_bundle
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "system.h"

#include "plugins/effects/custom/effect_amp.h"
#include "plugins/effects/custom/freeverb_effect.h"
#include "plugins/effects/custom/chorus_effect.h"
#include "plugins/effects/custom/effect_echo.h"
#include "plugins/effects/custom/effect_equalizer.h"
#include "plugins/effects/custom/effect_stereo_fx.h"
#include "plugins/effects/custom/effect_distortion.h"
#include "plugins/effects/custom/effect_pshift.h"


System *System::singleton_instance=NULL;


System* System::get_singleton_instance() {


	return singleton_instance;
}



System::System(int p_init_flags) {

	if (p_init_flags&INITIALIZE_AUDIO_OUT) {
	
		audio.driver_manager.set_variables_lock( Mutex_Lock::create_mutex() );
		audio.driver_manager.set_driver_lock( Mutex_Lock::create_mutex() );
		
		audio.driver_manager.add_mixer( &audio.mixer );
	}
	
	if (p_init_flags&INITIALIZE_AUDIO_IN) {

		/* ? */ 	
	}
	
	if (p_init_flags&INITIALIZE_MIDI_IN) {

		midi.control.set_mixer(&audio.mixer);
		midi.control.set_engine_manager(&midi.engine_manager);

	}
	
	init_flags=p_init_flags;

	effect_source_manager.register_effect_source(&effect_source_internal);

	effect_source_internal.register_effect_constructor("Amplifier","INTERNAL: amplifier",&Effect_Amp::create_this,&Effect_Amp::create_parameters );
	effect_source_internal.register_effect_constructor("Reverb","INTERNAL: freeverb",&Freeverb_Effect::create_this,&Freeverb_Effect::create_parameters );
	effect_source_internal.register_effect_constructor("Stereo Chorus","INTERNAL: stereo_chorus",&Chorus_Effect::create_this,&Chorus_Effect::create_parameters );
	effect_source_internal.register_effect_constructor("Echo","INTERNAL: echo",&Effect_Echo::create_this,&Effect_Echo::create_parameters );
	effect_source_internal.register_effect_constructor("Stereo FX","INTERNAL: stereo_fx",&Effect_Stereo_FX::create_this,&Effect_Stereo_FX::create_parameters );
        effect_source_internal.register_effect_constructor("6 Bands EQ","INTERNAL: eq_6_bands",&Effect_Equalizer::create_this,&Effect_Equalizer::create_parameters_6bands);
        effect_source_internal.register_effect_constructor("10 Bands EQ","INTERNAL: eq_10_bands",&Effect_Equalizer::create_this,&Effect_Equalizer::create_parameters_10bands);
        effect_source_internal.register_effect_constructor("21 Bands EQ","INTERNAL: eq_21_bands",&Effect_Equalizer::create_this,&Effect_Equalizer::create_parameters_21bands);
        effect_source_internal.register_effect_constructor("31 Bands EQ","INTERNAL: eq_31_bands",&Effect_Equalizer::create_this,&Effect_Equalizer::create_parameters_31bands);
	effect_source_internal.register_effect_constructor("Distortion","INTERNAL: distortion",&Effect_Distortion::create_this,&Effect_Distortion::create_parameters );
	effect_source_internal.register_effect_constructor("Pitch Shift","INTERNAL: pshift",&Effect_PShift::create_this,&Effect_PShift::create_parameters );

	singleton_instance=this;
}


System::~System() {

}

