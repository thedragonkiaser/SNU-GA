#if !defined(_MUTATION_H_)
#define _MUTATION_H_

#include "../lib/CmdLine.h"
#include "GA.h"
#include <algorithm>
#include <cstdlib>

namespace GA {
	template <typename S>
	class BaseMutator {
	public:
		BaseMutator(CCmdLine& cmdLine) {
			_threshold = MyUtil::strTo<float>( cmdLine.GetArgument("-M", 1) ) * 1000;
		}
		virtual ~BaseMutator() {}

		virtual bool mutate(typename S::Ptr pSolution) {
			int r = rand() % 1000;
			if (r < this->_getThreshold()) {
				this->_mutate(pSolution);
				return true;
			}
			return false;
		}

	protected:
		virtual void _mutate(typename S::Ptr pSolution) = 0;
		virtual float _getThreshold() { return _threshold; }
		float _threshold;
	};
	
	template <typename S>
	class SwapMutation : public BaseMutator<S> {
	public:
		SwapMutation(CCmdLine& cmdLine) : BaseMutator<S>(cmdLine) {}
		virtual ~SwapMutation() {}

	protected:
		void _mutate(typename S::Ptr pSolution) { 
			int nLen = pSolution->genotype.size();
			int s1 = rand() % nLen;
			int s2 = rand() % nLen;
			while (s1 == s2)
				s2 = rand() % nLen;
			swap(pSolution->genotype[s1], pSolution->genotype[s2]);
		}
	};

	template <typename S>
	class InsertMutation : public BaseMutator<S> {
	public:
		InsertMutation(CCmdLine& cmdLine) : BaseMutator<S>(cmdLine) {}
		virtual ~InsertMutation() {}

	protected:
		void _mutate(typename S::Ptr pSolution) { 
			int nLen = pSolution->genotype.size();
			int s1 = rand() % nLen;
			int s2 = rand() % nLen;
			while (s1 == s2)
				s2 = rand() % nLen;
			if (s1 > s2)
				swap(s1, s2);

			swap(pSolution->genotype[s1 + 1], pSolution->genotype[s2]);
		}
	};

	template <typename S>
	class InversionMutation : public BaseMutator<S> {
	public:
		InversionMutation(CCmdLine& cmdLine) : BaseMutator<S>(cmdLine) {}
		virtual ~InversionMutation() {}

	protected:
		void _mutate(typename S::Ptr pSolution) { 
			int nLen = pSolution->genotype.size();
			int s1 = rand() % nLen;
			int s2 = rand() % nLen;
			while (s1 == s2)
				s2 = rand() % nLen;
			if (s1 > s2)
				swap(s1, s2);

			reverse(pSolution->genotype.begin() + s1, pSolution->genotype.begin() + s2);
		}
	};

	template <typename S>
	class ScrambleMutation : public BaseMutator<S> {
	public:
		ScrambleMutation(CCmdLine& cmdLine) : BaseMutator<S>(cmdLine) {}
		virtual ~ScrambleMutation() {}

	protected:
		void _mutate(typename S::Ptr pSolution) { 
			int nLen = pSolution->genotype.size();
			int s1 = rand() % (nLen + 1);
			int s2 = rand() % (nLen + 1);
			while (s1 == s2)
				s2 = rand() % (nLen + 1);
			if (s1 > s2)
				swap(s1, s2);

			random_shuffle(pSolution->genotype.begin() + s1, pSolution->genotype.begin() + s2);
		}
	};

	template <typename S>
	class MutationOp {
	public:
		enum {
			Swap,
			Insert,
			Inversion,
			Scramble,
		};
	public:
		static BaseMutator<S>* Create(CCmdLine& cmdLine) {
			int mode = MyUtil::strTo<int>( cmdLine.GetArgument("-M", 0) );
			switch (mode) {
			case Swap:		return new SwapMutation<S>(cmdLine);
			case Insert:	return new InsertMutation<S>(cmdLine);
			case Inversion:	return new InversionMutation<S>(cmdLine);
			case Scramble:	return new ScrambleMutation<S>(cmdLine);
			}
			return NULL;
		}
	};
}

#endif
