#pragma region Includes:

    #include "Trip.h"      // Trip class
    #include <omp.h>       // omp directives
    #include <algorithm>   // std::sort
    #include "utility.hpp" // RNG
    #include "string.h"    // memset
    #include <iostream>
    #include <unordered_map> // hash map

#pragma endregion


#pragma region Prototypes:

    void evaluate(Trip trip[CHROMOSOMES], const int coordinates[CITIES][2]);
    void crossover(const Trip parents[TOP_X], Trip offsprings[TOP_X], const int coordinates[CITIES][2]);
    void mutate(Trip offsprings[TOP_X], const int RATE);

    void generateComplement(const char* trip1, char* trip2);

    std::size_t translateToIndex(const char c_city) noexcept;
    char translateToCity(const std::size_t ui_index) noexcept;
    char getComplement(const char c_city);
    void makeLookupTable(std::unordered_map<char,char>& c2c_complements);

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
            float d_tripLength = distance<float>(0, 0, coordinates[translateToIndex(t.itinerary[0])][0], coordinates[translateToIndex(t.itinerary[0])][1]);
            
            for (auto j = 0; j < CITIES - 1; j++)
            {
                auto city_1 = translateToIndex(t.itinerary[j]);
                auto city_2 = translateToIndex(t.itinerary[j+1]);

                d_tripLength += distance<float>(coordinates[city_1][0], coordinates[city_1][1], coordinates[city_2][0], coordinates[city_2][1]);
            } // end for j
            
            t.fitness = d_tripLength;
        } // end for i

        std::sort(trip, trip + CHROMOSOMES);
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


    inline char translateToCity(const std::size_t ui_index) noexcept
    {
        return (ui_index <= 25 ? static_cast<char>(ui_index) + 'A' : static_cast<char>(ui_index - 26) + '0');
    } // end method translateToCity


    inline char getComplement(const char c_city)
    {
        static bool first = true;
        static std::unordered_map<char, char> c2c_complements;

        // we do not want multiple threads to construct the map below
        // so we need to make sure only 1 thread checks 'first' at a time
        #pragma omp critical
        {
            if (first)
            {
                makeLookupTable(c2c_complements);
                first = false;
            } // end if
        } // end critical

        return c2c_complements.find(c_city)->second;
    } // end method getComplement

    void makeLookupTable(std::unordered_map<char,char>& c2c_complements)
    {
        // ABCDEFGHIJKLMNOPQR STUVWXYZ0123456789
        // 9876543210ZYXWVUTS RQPONMLKJIHGFEDCBA
        c2c_complements['A'] = '9';
        c2c_complements['B'] = '8';
        c2c_complements['C'] = '7';
        c2c_complements['D'] = '6';
        c2c_complements['E'] = '5';
        c2c_complements['F'] = '4';
        c2c_complements['G'] = '3';
        c2c_complements['H'] = '2';
        c2c_complements['I'] = '1';
        c2c_complements['J'] = '0';
        c2c_complements['K'] = 'Z';
        c2c_complements['L'] = 'Y';
        c2c_complements['M'] = 'X';
        c2c_complements['N'] = 'W';
        c2c_complements['O'] = 'V';
        c2c_complements['P'] = 'U';
        c2c_complements['Q'] = 'T';
        c2c_complements['R'] = 'S';
        c2c_complements['S'] = 'R';
        c2c_complements['T'] = 'Q';
        c2c_complements['U'] = 'P';
        c2c_complements['V'] = 'O';
        c2c_complements['W'] = 'N';
        c2c_complements['X'] = 'M';
        c2c_complements['Y'] = 'L';
        c2c_complements['Z'] = 'K';
        c2c_complements['0'] = 'J';
        c2c_complements['1'] = 'I';
        c2c_complements['2'] = 'H';
        c2c_complements['3'] = 'G';
        c2c_complements['4'] = 'F';
        c2c_complements['5'] = 'E';
        c2c_complements['6'] = 'D';
        c2c_complements['7'] = 'C';
        c2c_complements['8'] = 'B';
        c2c_complements['9'] = 'A';
    }

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
        // keep track of visited cities with a binary array
        char* c_a_visited = new char[CITIES]; 
        static double miss_count = 0, hit_count = 0;
        
        // iterate over all pairs of parents
        for (auto i = 0; i < TOP_X ; i+= 2)
        {
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
            
            for (auto j = 1; j < CITIES; j++)
            {
                auto ui_index_p1 = translateToIndex(p1.itinerary[j]);
                auto ui_index_p2 = translateToIndex(p2.itinerary[j]);

                // next city for both parents has already been visited
                if (c_a_visited[ui_index_p1] != '0' && c_a_visited[ui_index_p2] != '0')
                {
                    miss_count += 1;
                    auto ui_picked = randomIntInRange<std::size_t>(0,CITIES-1);

                    while(c_a_visited[ui_picked] != '0')
                    {
                        ui_picked = randomIntInRange<std::size_t>(0,CITIES-1);
                    }

                    c_a_visited[ui_picked] = '1';
                    c1.itinerary[j] = translateToCity(ui_picked);
                } // end if
                else
                {
                    hit_count += 1;   
                    auto ui_index_source = translateToIndex(c1.itinerary[j-1]);
                    auto f_source_p1 = distance<float>(coordinates[ui_index_source][0], coordinates[ui_index_source][1], coordinates[ui_index_p1][0], coordinates[ui_index_p1][1]);
                    auto f_source_p2 = distance<float>(coordinates[ui_index_source][0], coordinates[ui_index_source][1], coordinates[ui_index_p2][0], coordinates[ui_index_p2][1]);
                    
                    if (f_source_p1 < f_source_p2)
                    {
                        if (c_a_visited[ui_index_p1] == '0')
                        {
                            c_a_visited[ui_index_p1] = '1';
                            c1.itinerary[j] = p1.itinerary[j];
                        } // end if
                        else
                        {
                            c_a_visited[ui_index_p2] = '1';
                            c1.itinerary[j] = p2.itinerary[j];
                        } // end else
                    } // end elif
                    else
                    {
                        if (c_a_visited[ui_index_p2] == '0')
                        {
                            c_a_visited[ui_index_p2] = '1';
                            c1.itinerary[j] = p2.itinerary[j];
                        } // end if
                        else
                        {
                            c_a_visited[ui_index_p1] = '1';
                            c1.itinerary[j] = p1.itinerary[j];
                        } // end else
                    } // end else
                }
            } // end for j

            generateComplement(c1.itinerary, c2.itinerary);
        } // end for i


        delete[] c_a_visited;
    } // end method crossover


    void generateComplement(const char* trip1, char* trip2)
    {
        for (auto i = 0; i < CITIES; i++)
        {
            trip2[i] = getComplement(trip1[i]);
        } // end for i
    } // end method generateComplement



    /// <summary>
    ///          Generates a random mutation in all given <paramref name="offsprings"/> by swapping two random cities in a trip. 
    ///          Mutation may or may not occur for each offspring, this is determined by <see cref="MUTATE_RATE"/>.
    /// </summary>
    /// <param name="offsprings">
    ///                          The offsprings that should be mutated. Mutations are applied directly to the objects in this array.
    /// </param>
    /// <exception cref=""></exception>
    void mutate(Trip offsprings[TOP_X], const int RATE)
    {
        for (int cur = 0; cur < TOP_X; cur++)
        {
            //*
            if (randomIntInRange2<int>(0,100) <= RATE)
            {
                // pick two random cities
                auto i = randomIntInRange<std::size_t>(0, (CITIES-1)/2);
                auto j = randomIntInRange<std::size_t>((CITIES-1)/2, CITIES-1);
                //auto i = randomIntInRange<std::size_t>(0, CITIES-1);
                //auto j = randomIntInRange<std::size_t>(0, CITIES-1);

                while ( i == j)
                {
                    j = randomIntInRange<std::size_t>(0, CITIES-1);
                }

                char temp = offsprings[cur].itinerary[i];
                offsprings[cur].itinerary[i] = offsprings[cur].itinerary[j];
                offsprings[cur].itinerary[j] = temp;
            } // end if
            //*/
            /*
            if (randomIntInRange<int>(0,100) <= MUTATE_RATE)
            {
                std::random_shuffle(offsprings[cur].itinerary, offsprings[cur].itinerary + CITIES);
            } // end if
            //*/
        } // end for
    } // end method mutate

#pragma endregion




