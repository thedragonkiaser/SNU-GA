#if !defined(_REPLACEMENT_H_)
#define _REPLACEMENT_H_

#include "../lib/CmdLine.h"
#include "GA.h"

namespace GA {
	template <typename S>
	class ReplacementOp {
	public:
		enum {
			Worst = 0,
			Parent,
			WorstParent,
			Crowding
		};
	public:
		virtual void replace(typename S::Vector& offsprings, std::vector<typename S::Pair> parents, typename S::Vector& population) {
			int nSize = (int)offsprings.size();
			for (int i=0; i<nSize; ++i) {
				typename S::Ptr pOffspring(offsprings[i]);
				this->_remove(pOffspring, parents[i], population);

				typename S::Vector::iterator pit = population.begin();
				for (; pit != population.end(); ++pit) {
					if (*pOffspring > **pit)
						break;
				}
				population.insert(pit, pOffspring);
			}
		}

		static ReplacementOp<S>* Create(CCmdLine& cmdLine) {
			int mode = MyUtil::strTo<int>( cmdLine.GetArgument("-R", 0) );
			switch (mode) {
			case Worst:			return new ReplaceWorst<S>(cmdLine);
			case Parent:		return new ReplaceParent<S>(cmdLine);
			case WorstParent:	return new ReplaceWorstParent<S>(cmdLine);
			case Crowding:		return NULL;
			}
			return NULL;
		}

	protected:
		virtual void _remove(typename S::Ptr pOffspring, typename S::Pair& parents, typename S::Vector& population) = 0;
	};

	template <typename S>
	class ReplaceWorst : public ReplacementOp<S> {
	public:
		ReplaceWorst(CCmdLine& cmdLine) {}
		virtual ~ReplaceWorst() {}

	protected:
		virtual void _remove(typename S::Ptr pOffspring, typename S::Pair& parents, typename S::Vector& population) {
			population.pop_back();
		}
	};

	template <typename S>
	class ReplaceParent : public ReplacementOp<S> {
	public:
		ReplaceParent(CCmdLine& cmdLine) {}
		virtual ~ReplaceParent() {}

	protected:
		virtual void _remove(typename S::Ptr pOffspring, typename S::Pair& parents, typename S::Vector& population) {
			typename S::Vector::iterator pit = population.begin();
			for (; pit != population.end(); ++pit) {
				if (**pit == *(parents.first)) {
					population.erase(pit);
					break;
				}
			}
		}
	};

	template <typename S>
	class ReplaceWorstParent : public ReplacementOp<S> {
	public:
		ReplaceWorstParent(CCmdLine& cmdLine) {}
		virtual ~ReplaceWorstParent() {}

	protected:
		virtual void _remove(typename S::Ptr pOffspring, typename S::Pair& parents, typename S::Vector& population) {
			typename S::Ptr parent( *(parents.first) < *(parents.second) ? parents.first : parents.second );
			typename S::Vector::iterator pit = population.begin();
			for (; pit != population.end(); ++pit) {
				if (**pit == *parent) {
					population.erase(pit);
					break;
				}
			}
		}
	};
}

#endif