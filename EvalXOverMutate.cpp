#pragma region Includes:

    #include "Trip.h"      // Trip class
    #include <omp.h>       // omp directives
    #include <algorithm>   // std::sort
    #include "utility.hpp" // RNG
    #include "string.h"    // memset
    #include <iostream>
    #include <unordered_map> // hash map
	#include <random>

#pragma endregion


#pragma region Prototypes:

    void evaluate(Trip trip[CHROMOSOMES], const Point* coordinates);
    void crossover(Trip parents[TOP_X], Trip offsprings[TOP_X], const Point* coordinates);
    void mutate(Trip offsprings[TOP_X], const int RATE);
    void generateComplement(const char* trip1, char* trip2);

    std::size_t translateToIndex(const char c_city) noexcept;

    char translateToCity(const std::size_t ui_index) noexcept;

	float distanceV2(const Point& p1, const Point& p2);

#pragma endregion


#pragma region Implementations:
	
	/// <summary>
	///          Calculates the euclidean distance between <paramref name="p1"/> and <paramref name="p2"/> as a float.
	/// </summary>
	/// <param name="p1">
	///          First point.
	/// </param>
	/// <param name="p2">
	///          Second point.
	/// </param>
	/// <returns>
	///			The euclidian distance between <paramref name="p1"/> and <paramref name="p2"/> as a float.
	/// </returns>
	/// <exception cref=""></exception>
	float distanceV2(const Point& p1, const Point& p2)
	{
		float result = 0.0;
		// only one thread should modify the data structure at a time
		static std::unordered_map<Point, std::unordered_map<Point, float>> p2p_knownDistances;

		// check if distance calculation has been memoized
		if (p2p_knownDistances.find(p1) != p2p_knownDistances.end() && p2p_knownDistances.find(p1)->second.find(p2) != p2p_knownDistances.find(p1)->second.end())
		{
			result = p2p_knownDistances.find(p1)->second.find(p2)->second;
		} // end if
		else if (p2p_knownDistances.find(p2) != p2p_knownDistances.end() && p2p_knownDistances.find(p2)->second.find(p1) != p2p_knownDistances.find(p2)->second.end())
		{
			result = p2p_knownDistances.find(p2)->second.find(p1)->second;
		} // end if
		else
		{
			// new distance calculation, create memoization record and return value
			#pragma omp critical(lookup_table)
			{
				result = distance(p1.x, p1.y, p2.x, p2.y);
				p2p_knownDistances[p1].insert(std::pair<Point, float>(p2, result));
			} // end critical
		} // end else
		return result;
	} // end method distanceV2


	/// <summary>
	///          Translates an alpha-numeric city name (ABC...012...) to its corresponding index in the coordinates matrix.
	/// </summary>
	/// <param name="c_city">
	///                    The alpha-numeric city name to translate.
	/// </param>
	/// <returns>
	///          The index corresponding to <paramref name="c_city"/> in the coordinates matrix.
	/// </returns>
	inline std::size_t translateToIndex(const char c_city) noexcept
	{
		return (c_city >= 'A' ? static_cast<std::size_t>(c_city) - static_cast<std::size_t>('A') : static_cast<std::size_t>(c_city) - 22);
	} // end method translateToIndex


	/// <summary>
	///         Translates an index in range [0,35] to its corresponding alpha-numeric city name (ABC...012...).
	/// </summary>
	/// <param name="ui_index">
	///			The index to translate.
	/// </param>
	/// <returns>
	///			The alpha-numeric city name corresponding to <paramref name="ui_index"/>.
	/// </returns>
	inline char translateToCity(const std::size_t ui_index) noexcept
	{
		return (ui_index <= 25 ? static_cast<char>(ui_index) + 'A' : static_cast<char>(ui_index - 26) + '0');
	} // end method translateToCity


    /// <summary>
    ///          Evaluates the length of the given <paramref name="trip"/> using the provided <paramref name="coordinates"/> for the cities.
    ///          No value is returned, the value of <paramref name="trip"/>.fitness is set to the length of the trip.
    /// </summary>
    /// <param name="trip">
    ///                    The trip to evaluate.
    /// </param>
    /// <param name="coordinates">
    ///                           The coordinates of the cities to be used for determining distance between cities.
    /// </param>
    /// <exception cref=""></exception>
    void evaluate(Trip trip[CHROMOSOMES], const Point* coordinates)
    {
		static const Point start(0, 0);

		#pragma omp parallel for schedule(guided)
        for (auto i = 0; i < CHROMOSOMES; i++)
        {
            auto& t = trip[i];
            float d_tripLength = distanceV2(start, coordinates[translateToIndex(t.itinerary[0])]);
            
            for (auto j = 0; j < CITIES - 1; j++)
            {
                auto city_1 = translateToIndex(t.itinerary[j]);
                auto city_2 = translateToIndex(t.itinerary[j+1]);

                d_tripLength += distanceV2(coordinates[city_1], coordinates[city_2]);
            } // end for j
            
            t.fitness = d_tripLength;
        } // end for i

        std::sort(trip, trip + CHROMOSOMES);
    } // end method evaluate


    /// <summary>
    ///          Generates <see cref="TOP_X"/> offsprings from the trips in <paramref name="parents"/> and stores them in
    ///          <paramref name="offsprings"/>.
    /// </summary>
    /// <param name="parents">
    ///                       The selected parents that will reproduce into <see cref="TOP_X"/> offspring.
    /// </param>
    /// <param name="offsprings">
    ///                          Output parameter for generates offsprings.
    /// </param>
    /// <param name="coordinates">
    ///                           The coordinates of the cities to be used for determining distance between cities.
    /// </param>
    void crossover(Trip parents[TOP_X], Trip offsprings[TOP_X], const Point* coordinates)
    {       
		char* c_a_visited = nullptr;

		static std::random_device rd{};
		static std::mt19937 engine{ rd() };

		// shuffle parent pool to increase diversity of children
		std::shuffle(parents, parents + TOP_X, engine);

        // iterate over all pairs of parents
		#pragma omp parallel for private(c_a_visited) schedule(guided)
		for (auto i = 0; i < TOP_X ; i+= 2)
        {
			// keep track of visited cities with a binary array
			c_a_visited = new char[CITIES];
            memset(c_a_visited, '0', static_cast<std::size_t>(CITIES));

            // temp pointers for easy access of parents and children
            auto& p1 = parents[i];
            auto& p2 = parents[i+1];
            auto& c1 = offsprings[i];
            auto& c2 = offsprings[i+1];

            // source city of the trip from parent 1
            c1.itinerary[0] = p1.itinerary[0];
            c1.itinerary[CITIES] = static_cast<char>(NULL);

            // mark first city as visited
            c_a_visited[translateToIndex(p1.itinerary[0])] = '1';
            
			// generate child chromosome
            for (auto j = 1; j < CITIES; j++)
            {
                auto ui_index_p1 = translateToIndex(p1.itinerary[j]);
                auto ui_index_p2 = translateToIndex(p2.itinerary[j]);

                // next city for both parents has already been visited
                if (c_a_visited[ui_index_p1] != '0' && c_a_visited[ui_index_p2] != '0')
                {
					// pick random city
                    auto ui_picked = randomIntInRange<std::size_t>(0,CITIES-1);

					// ensure we've never been to this city before
                    while(c_a_visited[ui_picked] != '0')
                    {
                        ui_picked = randomIntInRange<std::size_t>(0,CITIES-1);
                    } // end while

                    c_a_visited[ui_picked] = '1';
                    c1.itinerary[j] = translateToCity(ui_picked);
                } // end if
                else // at least one of the candidate cities has not been visited
                { 
					// get distance to each candidate city
                    auto ui_index_source = translateToIndex(c1.itinerary[j-1]);
                    auto f_source_p1 = distanceV2(coordinates[ui_index_source], coordinates[ui_index_p1]);
                    auto f_source_p2 = distanceV2(coordinates[ui_index_source], coordinates[ui_index_p2]);
                    
                    if (f_source_p1 < f_source_p2) 
                    {
                        if (c_a_visited[ui_index_p1] == '0') // p1 is closer and available
                        {
                            c_a_visited[ui_index_p1] = '1';
                            c1.itinerary[j] = p1.itinerary[j];
                        } // end if
                        else // p1 is closer but unavailable
                        {
                            c_a_visited[ui_index_p2] = '1';
                            c1.itinerary[j] = p2.itinerary[j];
                        } // end else
                    } // end if
                    else
                    {
                        if (c_a_visited[ui_index_p2] == '0') // p2 is closer and available
                        {
                            c_a_visited[ui_index_p2] = '1';
                            c1.itinerary[j] = p2.itinerary[j];
                        } // end if
                        else // p2 is closer but unavailable
                        {
                            c_a_visited[ui_index_p1] = '1';
                            c1.itinerary[j] = p1.itinerary[j];
                        } // end else
                    } // end else
                } // end else
            } // end for j
			delete[] c_a_visited;

            generateComplement(c1.itinerary, c2.itinerary);
        } // end for i
    } // end method crossover


	/// <summary>
	///          Generates the complement of <paramref name="trip1"/> and writes it into <paramref name="trip2"/>.
	/// </summary>
	/// <param name="trip1">
	///          Existing child to use as template for making <paramref name="trip2"/>.
	/// </param>
	/// <param name="trip2">
	///         Child to be created as complement of <paramref name="trip1"/>.
	/// </param>
    void generateComplement(const char* trip1, char* trip2)
    {
        for (auto i = 0; i < CITIES; i++)
        {
			std::size_t temp = translateToIndex(trip1[i]);
			trip2[i] = translateToCity(CITIES - 1 - temp);
        } // end for i
    } // end method generateComplement


    /// <summary>
    ///          Generates a random mutation in all given <paramref name="offsprings"/> by swapping two random cities in a trip. 
    ///          Mutation may or may not occur for each offspring, this is determined by <see cref="MUTATE_RATE"/>.
    /// </summary>
    /// <param name="offsprings">
    ///          The offsprings that should be mutated. Mutations are applied directly to the objects in this array.
    /// </param>
	/// <param name="RATE">
	///          The mutation rate to use for determining whether mutation occurs or not.
	/// </param>
    void mutate(Trip offsprings[TOP_X], const int RATE)
    {
		#pragma omp parallel for schedule(guided)
        for (auto cur = 0; cur < TOP_X; cur++)
        {
            if (randomIntInRange<int>(0,100) <= RATE)
            {
                // pick two random cities
                auto i = randomIntInRange<std::size_t>(0, CITIES-1);
                auto j = randomIntInRange<std::size_t>(0, CITIES-1);

                while ( i == j)
                {
                    j = randomIntInRange<std::size_t>(0, CITIES-1);
                } // end while

				std::swap(offsprings[cur].itinerary[i], offsprings[cur].itinerary[j]);
            } // end if
        } // end for
    } // end method mutate

#pragma endregion




