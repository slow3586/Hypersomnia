#pragma once
#include "math/vec2.h"

namespace augs {
	namespace graphics {
		namespace gui {
			struct dragger {
				float vel[2], vel_mult;
				dragger();
				void stop();
				void move(augs::vec2<>&);
				void drag(const augs::vec2<int>& mouse, const rects::ltrb<float>&);
			};
		}
	}
}