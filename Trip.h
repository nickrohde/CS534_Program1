#ifndef _TRIP_H_
#define _TRIP_H_

#define CHROMOSOMES        50000  // 50000 different trips                              (DO NOT CHANGE)
#define CITIES             36     // 36 cities = ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789   (DO NOT CHANGE)
#define MAX_GENERATION     150    //                                                    (DO NOT CHANGE)
#define TOP_X              25000  // top 50%                                            (DO NOT CHANGE)
#define MUTATE_RATE        90     // 50% for time being                                 (YOU MAY CHANGE IT)
#define N_THREADS          1      // number of threads
                                                                    
#define DEBUG          false // for debugging   

// Each trip (or chromosome) informatioin
class Trip {
public:
  char itinerary[CITIES + 1];  // a route through all 36 cities from (0, 0) 
  float fitness;               // the distance of this entire route

  // Relational operator friend definitions:
  friend bool operator<(const Trip& LHS,  const Trip& RHS);
  friend bool operator==(const Trip& LHS, const Trip& RHS);
  friend bool operator>(const Trip& LHS,  const Trip& RHS);
  friend bool operator<=(const Trip& LHS, const Trip& RHS);
  friend bool operator>=(const Trip& LHS, const Trip& RHS);
  friend bool operator!=(const Trip& LHS, const Trip& RHS);
};

  // Implementation of operator friend functions:
  inline bool operator<(const Trip& LHS,  const Trip& RHS) { return LHS.fitness <  RHS.fitness; }
  inline bool operator==(const Trip& LHS, const Trip& RHS) { return LHS.fitness == RHS.fitness; }
  inline bool operator>(const Trip& LHS,  const Trip& RHS) { return  (RHS <  LHS); }
  inline bool operator<=(const Trip& LHS, const Trip& RHS) { return !(LHS >  RHS); }
  inline bool operator>=(const Trip& LHS, const Trip& RHS) { return !(LHS <  RHS); }
  inline bool operator!=(const Trip& LHS, const Trip& RHS) { return !(LHS == RHS); }

#endif
