#if !defined(_GA_H_)
#define _GA_H_

#include <memory>
#include <utility>
#include <vector>
#include <algorithm>

namespace GA {
	template <typename T, typename C>
	struct Solution {
		typedef std::shared_ptr< Solution<T, C> > Ptr;
		typedef std::pair<Ptr, Ptr> Pair;
		typedef std::vector<Ptr> Vector;

		typedef T GeneType;
		typedef C CostType;

		std::vector<T> genotype;
		typename Solution<T, C>::Pair parents;
		C cost;

		bool operator <(const Solution<T, C>& r) const {
			return this->cost > r.cost;
        }
		bool operator ==(const Solution<T, C>& other) const {
			return equal(this->genotype.begin(), this->genotype.end(), other.genotype.begin());
        }
		bool operator !=(const Solution<T, C>& other) const {
			return !(*this == other);
        }
	};
}

#endif