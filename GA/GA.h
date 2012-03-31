#if !defined(_GA_H_)
#define _GA_H_

#include <memory>
#include <utility>
#include <vector>

namespace GA {
	template <typename T, typename Q>
	struct Solution {
		typedef std::shared_ptr< Solution<T, Q> > Ptr;
		typedef std::pair<Ptr, Ptr> Pair;
		typedef std::vector<Ptr> Vector;

		std::vector<T> genotype;
		Q quality;
	};
}

#endif