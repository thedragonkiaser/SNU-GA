#if !defined(_CROSSOVER_H_)
#define _CROSSOVER_H_

#include "../lib/CmdLine.h"
#include "GA.h"
#include <set>
#include <map>
#include <vector>
#include <cstdlib>

namespace GA {
	template <typename S>
	class BaseCrossover {
	public:
		virtual typename S::Ptr crossover(typename S::Pair& parents) {
			typename S::Ptr p(new S);
			p->genotype.reserve(parents.first->genotype.size());
			p->genotype.resize(parents.first->genotype.size());

			this->_crossover(parents, p);

			return p;
		};

	protected:
		virtual void _crossover(typename S::Pair& parents, typename S::Ptr p) = 0;
	};

	template <typename S>
	class CycleCrossover : public BaseCrossover<S> {
	public:
		CycleCrossover(CCmdLine& cmdLine) : BaseCrossover<S>() {}
		virtual ~CycleCrossover() {}

	protected:
		void _crossover(typename S::Pair& parents, typename S::Ptr p) {
			typename S::ChromosomeType* _parents[2] = {&parents.first->genotype, &parents.second->genotype};

			int nToggle = 0;
			int nLen = parents.first->genotype.size();

			std::vector<int> indice(nLen, 0);
			for (int i=0; i<nLen; ++i) {
				if (indice[i] == 1)
					continue;

				int nIdx = i;
				do {
					p->genotype[nIdx] = (_parents[nToggle % 2])->at(nIdx);
					indice[nIdx] = 1;
					
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
	class OrderCrossover : public BaseCrossover<S> {
	public:
		OrderCrossover(CCmdLine& cmdLine) : BaseCrossover<S>() {}
		virtual ~OrderCrossover() {}

	protected:
		void _crossover(typename S::Pair& parents, typename S::Ptr p) {
			typename S::ChromosomeType& p1 = parents.first->genotype;
			typename S::ChromosomeType& p2 = parents.second->genotype;
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

				typename std::set<typename S::GeneType>::iterator it = values.find(p2[i]);
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
	class PMXCrossover : public BaseCrossover<S> {
	public:
		PMXCrossover(CCmdLine& cmdLine) : BaseCrossover<S>() {}
		virtual ~PMXCrossover() {}

	protected:
		void _crossover(typename S::Pair& parents, typename S::Ptr p) {
			typename S::ChromosomeType& p1 = parents.first->genotype;
			typename S::ChromosomeType& p2 = parents.second->genotype;
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

				typename std::set<typename S::GeneType>::iterator it = values.find(p2[i]);
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

	template <typename S>
	class EdgeRecombinationCrossover : public BaseCrossover<S> {
	public:
		EdgeRecombinationCrossover(CCmdLine& cmdLine) : BaseCrossover<S>() {}
		virtual ~EdgeRecombinationCrossover() {}

	protected:
		void _crossover(typename S::Pair& parents, typename S::Ptr p) {
			typename S::ChromosomeType* _parents[2] = {&parents.first->genotype, &parents.second->genotype};
			std::map< typename S::GeneType, std::set<typename S::GeneType> > adjsTable;
			std::set<typename S::GeneType> keys;

			for (int i=0; i<2; ++i) {
				typename S::ChromosomeType& px = *_parents[i];
				
				int nLen = px.size();
				for (int k=0; k<nLen; ++k) {
					typename S::GeneType val = px[k];
					std::set<typename S::GeneType>& s = adjsTable[val];
					keys.insert(val);
			
					s.insert( (k == 0) ? px[nLen-1] : px[k-1] );
					s.insert( (k == nLen-1) ? px[0] : px[k+1] );
				}
			}

			typename S::ChromosomeType& px = *_parents[rand() % 2];
			int nLen = px.size();
			typename S::GeneType val = px[0];
			for (int i=0; i<nLen; ++i) {
				p->genotype[i] = val;

				if (i == nLen-1)
					break;

				std::set<typename S::GeneType>::iterator kit = keys.begin();
				for (; kit != keys.end(); ++kit)
					adjsTable[ *kit ].erase(val);
				
				std::set<typename S::GeneType>& s = adjsTable[val];
				std::set<typename S::GeneType>::iterator it = s.begin();

				typename S::GeneType min = *it;
				unsigned int minSize = adjsTable[min].size();
				
				for (; it != s.end(); ++it) {
					if (adjsTable[ *it ].size() <= minSize) {
						min = *it;
						minSize = adjsTable[min].size();
					}
				}

				val = min;
			}
		}
	};

	template <typename S>
	class CrossoverOp {
	public:
		enum {
			Cycle = 0,
			Order,
			PMX,
			EdgeRecombination,
		};
	public:
		static BaseCrossover<S>* Create(CCmdLine& cmdLine) {
			int mode = MyUtil::strTo<int>( cmdLine.GetArgument("-X", 0) );
			switch (mode) {
				case Cycle: return new CycleCrossover<S>(cmdLine);
				case Order: return new OrderCrossover<S>(cmdLine);
				case PMX:	return new PMXCrossover<S>(cmdLine);
				case EdgeRecombination:	return new EdgeRecombinationCrossover<S>(cmdLine);
			}
			return NULL;
		}
	};
}

#endif
