#ifndef RESAMPLER_KERNEL
#define RESAMPLER_KERNEL
#include "common/components/audio/resampler.h"

// sample_getter - A function object that fills its output buffer.
//
// arguments     - output: A buffer large enough to hold one sample
//                 frame in floating-point format.
//
// notes         - The number of channels in a sample frame is
//                 determined in the function mix_sample from
//                 resampler_kernel.cpp.

class sample_getter {
	public:
		virtual void operator()(float *output)=0;
		virtual ~sample_getter() { } ;
};

#define DEFINE_GETTER(name, CODE_BLOCK)				\
class name : public sample_getter {				\
	Sample_Data *data_ptr;					\
	public:							\
		name(Sample_Data *data) { 			\
			data_ptr = data;			\
		}						\
		virtual void operator()(float *output) 		\
			CODE_BLOCK				\
		virtual ~name() { };				\
}

void mix_sample(Resampler::Mix_Data *mixdata, sample_getter *get_sample, bool perform_filtering);

#endif
