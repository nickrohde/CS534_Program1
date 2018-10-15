#ifndef _TRIP_H_
#define _TRIP_H_

#define CHROMOSOMES        50000  // 50000 different trips                              (DO NOT CHANGE)
#define CITIES             36     // 36 cities = ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789   (DO NOT CHANGE)
#define MAX_GENERATION     150    //                                                    (DO NOT CHANGE)
#define TOP_X              25000  // top 50%                                            (DO NOT CHANGE)
#define MUTATE_RATE        50     // 50% for time being                                 (YOU MAY CHANGE IT)
#define N_THREADS          4      // number of threads

#include <iterator>			      // std::begin/end

// Each trip (or chromosome) information
class Trip
{
public:
	char itinerary[CITIES + 1];  // a route through all 36 cities from (0, 0) 
	float fitness;               // the distance of this entire route

	// Relational operator friend definitions:
	friend bool operator<(const Trip& LHS, const Trip& RHS);
	friend bool operator==(const Trip& LHS, const Trip& RHS);
	friend bool operator>(const Trip& LHS, const Trip& RHS);
	friend bool operator<=(const Trip& LHS, const Trip& RHS);
	friend bool operator>=(const Trip& LHS, const Trip& RHS);
	friend bool operator!=(const Trip& LHS, const Trip& RHS);

	Trip& operator=(const Trip& OTHER)
	{
		std::copy(std::begin(OTHER.itinerary), std::end(OTHER.itinerary), std::begin(itinerary));
		fitness = OTHER.fitness;

		return *this;
	} // end copy assignment

	Trip& operator=(Trip&& other)
	{
		std::move(std::begin(other.itinerary), std::end(other.itinerary), std::begin(itinerary));
		fitness = other.fitness;

		return *this;
	} // end move assignment

	Trip(const Trip& OTHER)
	{
		*this = OTHER;
	} // end copy constructor

	Trip(void)
	{
		itinerary[CITIES] = 0;
		fitness = -1;
	} // end default constructor
}; // end class Trip


// Implementation of relational operators:
inline bool operator<(const Trip& LHS,  const Trip& RHS) { return LHS.fitness < RHS.fitness; }
inline bool operator==(const Trip& LHS, const Trip& RHS) { return LHS.fitness == RHS.fitness; }
inline bool operator>(const Trip& LHS,  const Trip& RHS) { return  (RHS < LHS); }
inline bool operator<=(const Trip& LHS, const Trip& RHS) { return !(LHS > RHS); }
inline bool operator>=(const Trip& LHS, const Trip& RHS) { return !(LHS < RHS); }
inline bool operator!=(const Trip& LHS, const Trip& RHS) { return !(LHS == RHS); }

#endif
