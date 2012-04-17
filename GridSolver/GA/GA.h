#pragma once

#include <memory>
#include <utility>
#include <vector>
#include <algorithm>

#include <assert.h>

//using namespace rel_ops;

namespace GA {
	struct Solution {
		// Type Definitions
		typedef std::vector<int> ChromosomeType;
		typedef std::shared_ptr<Solution> Ptr;
		typedef std::pair<Ptr, Ptr> Pair;
		typedef std::vector<Ptr> Vector;

		// Constructors
		Solution() : width(0), height(0), cost(0) {};
		Solution(int w, int h) : width(w), height(h), cost(0) {
			genotype.reserve( width * height );
		};

		// Fields
		ChromosomeType genotype;
		int width, height;
		int cost;

		// Methods
		int get(int x, int y) {
			assert(y * width + x < (int)this->genotype.size());
			return this->genotype[y * width + x];
		}

		int getDistance(const Solution& sol) {
			// Hamming Distance
			int dist=0;

			int size = this->genotype.size();
			for (int i=0; i<size; ++i) {
				if (this->genotype[i] != sol.genotype[i])
					++dist;
			}

			return dist;
		}

		int getDistance(Ptr pSol) {
			return this->getDistance(*pSol);
		}

		// Operators
		bool operator ==(const Solution& other) const {
			return equal(this->genotype.begin(), this->genotype.end(), other.genotype.begin());
        }
		bool operator ==(Ptr p) const {
			return *this == *p;
        }
		bool operator <(const Solution& r) const {
			return this->cost < r.cost;
        }
		bool operator <(Ptr p) const {
			return *this < *p;
        }
	};
}