#pragma region Includes:

    #include "Trip.h"      // Trip class
    #include <omp.h>       // omp directives
    #include <algorithm>   // std::sort
    #include "utility.hpp" // RNG

#pragma endregion


#pragma region Prototypes:

    void evaluate(Trip trip[CHROMOSOMES], const int coordinates[CITIES][2]);
    std::size_t translateToIndex(const char c_city) noexcept;
    void crossover(const Trip parents[TOP_X], Trip offsprings[TOP_X], const int coordinates[CITIES][2]);
    void mutate(Trip offsprings[TOP_X]);

#pragma endregion


#pragma region Implementations:

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
    void evaluate(Trip trip[CHROMOSOMES], const int coordinates[CITIES][2])
    {
        for (auto i = 0; i < CHROMOSOMES; i++)
        {
            auto& t = trip[i];
            auto d_tripLength = 0.0;

            // itinerary has CITIES + 1 indices
            for (auto j = 0; j < CITIES; j++)
            {
                auto city_1 = translateToIndex(t.itinerary[j]);
                auto city_2 = translateToIndex(t.itinerary[j+1]);

                d_tripLength += distance(coordinates[city_1][0], coordinates[city_1][1], coordinates[city_2][0], coordinates[city_2][1]);
            } // end for j
            
            t.fitness = d_tripLength;
        } // end for i
    } // end method evaluate

    
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
    /// <exception cref=""></exception>
    void crossover(const Trip parents[TOP_X], Trip offsprings[TOP_X], const int coordinates[CITIES][2])
    {
        
    } // end method crossover


    /// <summary>
    ///          Generates a random mutation in all given <paramref name="offsprings"/> by swapping two random cities in a trip. 
    ///          Mutation may or may not occur for each offspring, this is determined by <see cref="MUTATE_RATE"/>.
    /// </summary>
    /// <param name="offsprings">
    ///                          The offsprings that should be mutated. Mutations are applied directly to the objects in this array.
    /// </param>
    /// <exception cref=""></exception>
    void mutate(Trip offsprings[TOP_X] )
    {

    } // end method mutate

#pragma endregion




