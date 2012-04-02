#if !defined(_CROSSOVER_H_)
#define _CROSSOVER_H_

#include "../lib/CmdLine.h"
#include "GA.h"
#include <set>
#include <vector>

namespace GA {
	template <typename S>
	class CrossoverOp {
	public:
		enum {
			Cycle = 0,
			Order,
			PMX,
		};
	public:
		virtual typename S::Ptr crossover(typename S::Pair& parents) {
			S::Ptr p(new S);
			p->genotype.reserve(parents.first->genotype.size());
			p->genotype.resize(parents.first->genotype.size());
			p->parents = parents;

			this->_crossover(parents, p);

			return p;
		};

		static CrossoverOp<S>* Create(CCmdLine& cmdLine) {
			int mode = MyUtil::strTo<int>( cmdLine.GetArgument("-X", 0) );
			switch (mode) {
			case Cycle: return new CycleCrossover<S>(cmdLine);
			case Order: return new OrderCrossover<S>(cmdLine);
			case PMX:	return new PMXCrossover<S>(cmdLine);
			}
			return NULL;
		}

	protected:
		virtual void _crossover(typename S::Pair& parents, typename S::Ptr p) = 0;
	};

	template <typename S>
	class CycleCrossover : public CrossoverOp<S> {
	public:
		CycleCrossover(CCmdLine& cmdLine) {}
		virtual ~CycleCrossover() {}

	protected:
		void _crossover(typename S::Pair& parents, typename S::Ptr p) {
			std::vector<typename S::GeneType>* _parents[2] = {&parents.first->genotype, &parents.second->genotype};

			int nToggle = 0;
			int nLen = parents.first->genotype.size();

			std::vector<int> indice(nLen, FALSE);
			for (int i=0; i<nLen; ++i) {
				if (indice[i] == TRUE)
					continue;

				int nIdx = i;
				do {
					p->genotype[nIdx] = (_parents[nToggle % 2])->at(nIdx);
					indice[nIdx] = TRUE;
					
					typename S::GeneType val = (_parents[(nToggle + 1)% 2])->at(nIdx);
					for (int k=0; k<nLen; ++k) {
						if (val == (_parents[nToggle % 2])->at(k)) {
							nIdx = k;
							break;
						}
					}
				} while (nIdx != i);
				++nToggle;
			}
		}
	};

	template <typename S>
	class OrderCrossover : public CrossoverOp<S> {
	public:
		OrderCrossover(CCmdLine& cmdLine) {}
		virtual ~OrderCrossover() {}

	protected:
		void _crossover(typename S::Pair& parents, typename S::Ptr p) {
			std::vector<typename S::GeneType>& p1 = parents.first->genotype;
			std::vector<typename S::GeneType>& p2 = parents.second->genotype;
			std::set<typename S::GeneType> values;

			int nLen = p1.size();
			int s1 = rand() % (nLen + 1);
			int s2 = rand() % (nLen + 1);
			while (s1 == s2)
				s2 = rand() % (nLen + 1);

			if (s1 > s2)
				swap(s1, s2);

			for (int i=s1; i<s2; ++i) {
				p->genotype[i] = p1[i];
				values.insert(p->genotype[i]);
			}

			int k=0;
			for (int i=s2; i!= s1; ++i) {
				if (i == nLen) {
					i = -1;
					continue;
				}

				std::set<typename S::GeneType>::iterator it = values.find(p2[i]);
				if (it != values.end()) {
					it = values.find(p2[k]);
					while (it != values.end()) {
						++k;
						it = values.find(p2[k]);
					}
					p->genotype[i] = p2[k];
				}
				else
					p->genotype[i] = p2[i];
				values.insert(p->genotype[i]);
			}
		}
	};

	template <typename S>
	class PMXCrossover : public CrossoverOp<S> {
	public:
		PMXCrossover(CCmdLine& cmdLine) {}
		virtual ~PMXCrossover() {}

protected:
		void _crossover(typename S::Pair& parents, typename S::Ptr p) {
			std::vector<typename S::GeneType>& p1 = parents.first->genotype;
			std::vector<typename S::GeneType>& p2 = parents.second->genotype;
			std::set<typename S::GeneType> values;

			int nLen = p1.size();
			int s1 = rand() % (nLen + 1);
			int s2 = rand() % (nLen + 1);
			while (s1 == s2)
				s2 = rand() % (nLen + 1);

			if (s1 > s2)
				swap(s1, s2);

			for (int i=s1; i<s2; ++i) {
				p->genotype[i] = parents.first->genotype[i];
				values.insert(p->genotype[i]);
			}

			for (int i=0; i<nLen; ++i) {
				if (i >=s1 && i<s2)
					continue;

				std::set<typename S::GeneType>::iterator it = values.find(p2[i]);
				if (it != values.end()) {
					typename S::GeneType val = p2[i];
					do {
						int k = 0;
						for (k=0; k<nLen; ++k) {
							if (val == p1[k])
								break;
						}

						val = p2[k];
						it = values.find(val);
					} while (it != values.end() );

					p->genotype[i] = val;
				}
				else
					p->genotype[i] = p2[i];
				values.insert(p->genotype[i]);
			}
		}
	};
}

#endif
