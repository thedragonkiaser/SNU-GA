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

		typedef T GeneType;
		typedef Q QualityType;

		std::vector<T> genotype;
		Q quality;

		bool operator <(const Solution<T, Q>& r) const {
			return this->quality < r.quality;
        }
	};
}

#endif