#include "cheesetracker/trackercore/range_defs.h"
#include "resampler_kernel.h"
#include "ns_autoptr.h"

#define CURRENT_CHANNEL (chan % channels)
#define HARD_CODED_MIXER_CHANNELS 2

void mix_sample(Resampler::Mix_Data *mixdata, sample_getter *get_sample, bool perform_filtering)
{

	// Copy certain values from {mixdata} onto the stack,
	// where access will be faster.

	// dest_buffer is a buffer into which we can copy samples_to_mix
	// sample frames (defined below), which will eventually get
	// copied to the sound card.

	sample_t *dest_buffer 	= mixdata->dst_buff;

	float pan_normalized	= (float)mixdata->panning/(float)PAN_RIGHT;
	bool right_of_centre	= pan_normalized > 0.5;
	float from_centre 	= (right_of_centre ? (pan_normalized - 0.5) : (0.5 - pan_normalized)) * 2.0;
	size_t samples_to_mix 	= mixdata->samples_to_mix;
	// Prevent a division by zero
	if(samples_to_mix == 0) return;
	float send_volume 	= mixdata->l_volume;
	float prev_volume	= mixdata->l_volume_prev;
	float ramp_increment	= (float)(send_volume-prev_volume)/(float)samples_to_mix;
	size_t channels 	= mixdata->sample->num_channels();

	send_volume = prev_volume;

	// A local temporary buffer.
	sample_t final_float[HARD_CODED_MIXER_CHANNELS];
	
	// Fill mixdata->dst_buff with {samples_to_mix} samples.
	while(samples_to_mix--) {
		// Zero-initialize the reusable buffer final_float.
		for(size_t chan=0; chan<HARD_CODED_MIXER_CHANNELS; chan++)
			final_float[chan] = 0.0;

		// Let temp_float be another temporary buffer.
		// final_float has as many channels as are supported
		// for the hardware, but temp_float has the number of
		// channels contained in the sample being processed.
		ns_autoptr<float> ns_temp_float;
		float *temp_float = new float[channels];
		ns_temp_float.arr_new(temp_float);

		// Call the externally-supplied resampling function. This
		// will fill temp_float with one sample frame.
		(*get_sample)(temp_float);

		// Iterate over all channels. The % operator will prevent
		// buffer overflows in the event that HARD_CODED_MIXER_CHANNELS != channels.
		//
		// This loop implements the panning effect.
		//
		// FIXME: The panning algorithm here only works for a left-to-right,
		//        two-speaker hardware setup. A more robust algorithm would also
		//        require different data structures, which would require
		//        changes to the user interface. 
		//
		//        If the sample being processed has more than two channels,
		//        each even channel will go to hardware channel 0, and
		//        each odd channel will go to hardware channel 1.
		//

		for(size_t chan=0; chan<std::max<size_t>(HARD_CODED_MIXER_CHANNELS, channels); chan++) {
			bool this_side_of_centre = (chan % HARD_CODED_MIXER_CHANNELS) ? right_of_centre : !right_of_centre;
			// Copy temp_float[CURRENT_CHANNEL] because the following operations
			// are destructive, and temp_float[CURRENT_CHANNEL] may be processed
			// more than once.
			float current_channel = temp_float[CURRENT_CHANNEL];

			/* Panning towards one side makes the other side quieter. */
			/* But the side being panned toward does not get louder. */
			if(!this_side_of_centre) {
				current_channel -= current_channel * from_centre;
			}
			final_float[chan % HARD_CODED_MIXER_CHANNELS] += current_channel;
		}
		/* Renormalize if any channel in final_float took more than
		 * one channel from temp_float.
		 */
		if(channels > HARD_CODED_MIXER_CHANNELS) {
			for(size_t chan=0; chan<HARD_CODED_MIXER_CHANNELS; chan++)
				final_float[chan] = final_float[chan] / ((float)channels/(float)HARD_CODED_MIXER_CHANNELS);
		}
		mixdata->sample->fixedpoint_move_cursor();   

		send_volume += ramp_increment;
		/*** FILTERING ***/
		if (perform_filtering) {
			float pre_filter = final_float[0];
			// FIXME: mixdata->filter should really be an array of
			// length HARD_CODED_MIXER_CHANNELS.
			for(size_t chan=0; chan<HARD_CODED_MIXER_CHANNELS; chan++) {
				final_float[chan] = final_float[chan] * mixdata->filter.coeffs.b0
					+ mixdata->filter.hist_b1 * mixdata->filter.coeffs.b1
					+ mixdata->filter.hist_b2 * mixdata->filter.coeffs.b2
					+ mixdata->filter.hist_a1 * mixdata->filter.coeffs.a1
					+ mixdata->filter.hist_a2 * mixdata->filter.coeffs.a2;
			}
			mixdata->filter.hist_a2 = mixdata->filter.hist_a1;
			mixdata->filter.hist_b2 = mixdata->filter.hist_b1;
			mixdata->filter.hist_b1 = pre_filter;
			mixdata->filter.hist_a1 = final_float[0];
		}
		/*** MIXDOWN ***/
		for(size_t chan = 0; chan < HARD_CODED_MIXER_CHANNELS; chan++) {
			*dest_buffer++ += send_volume * final_float[chan];
		}
	}
}
