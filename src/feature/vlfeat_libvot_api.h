#ifndef VLFEAT_LIBVOT_API_H
#define VLFEAT_LIBVOT_API_H

extern "C" {
#include <vl/generic.h>
#include <vl/sift.h>
}
#include "utils/data_types.h"
#include <string>

namespace tw {
struct VlFeatParam {
	VlFeatParam () : edge_thresh(-1), peak_thresh(-1), magnif(-1) {}
	double edge_thresh;
	double peak_thresh;
	double magnif;
};

struct Vlfeature {
	float x;
	float y;
	float scale;
	float orientation;
	float descr[128];
};

int RunVlFeature(unsigned char *data, int image_width, int image_height, int num_channel,
                 SiftData &sift_data, VlFeatParam const & vlfeat_param);
}	// end of namespace tw

#endif
