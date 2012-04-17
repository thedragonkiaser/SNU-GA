#if !defined(_GA_H_)
#define _GA_H_

#include <memory>
#include <utility>
#include <vector>
#include <algorithm>
#include <functional>

namespace GA {
	template <typename T, typename C>
	struct Solution {
		typedef T GeneType;
		typedef C CostType;
		typedef std::vector<T> ChromosomeType;
		typedef std::shared_ptr< Solution<T, C> > Ptr;
		typedef std::pair<Ptr, Ptr> Pair;
		typedef std::vector<Ptr> Vector;

		ChromosomeType genotype;
		C cost;

		bool operator <(const Solution<T, C>& r) const {
			return this->cost > r.cost;
        }
		bool operator >(const Solution<T, C>& r) const {
			return r < *this;
        }
		bool operator ==(const Solution<T, C>& other) const {
			return equal(this->genotype.begin(), this->genotype.end(), other.genotype.begin());
        }
		bool operator !=(const Solution<T, C>& other) const {
			return !(*this == other);
        }
	};

	template <typename S>
	struct SolutionPtrComp : std::unary_function<typename S::Ptr, bool> {
		bool operator()(const typename S::Ptr& lhs, const typename S::Ptr& rhs) const {
			return *rhs < *lhs;
		}
	};
}

#endif