
// windows marks string functions as insecure
#if defined(_WIN32) || defined(_WIN64) 
	#define _CRT_SECURE_NO_WARNINGS
#endif

#pragma region Includes:

	#include <iostream>      // cout
	#include <fstream>       // ifstream, ofstream
	#include <string.h>      // strncpy
	#include <omp.h>         // OpenMP
	#include <limits>        // float max
	#include "utility.hpp"   // randomIntInRange
	#include <unordered_set> // hash set
	#include "Trip.h"		 // Trip

#pragma endregion


#pragma region Function Prototypes:

	void initialize(Trip trip[CHROMOSOMES], Point coordinates[CITIES]);
	void select(const Trip trip[CHROMOSOMES], Trip parents[TOP_X]);
	void populate(Trip trip[CHROMOSOMES], Trip offsprings[TOP_X]);
	int run(int nThreads, int i_mutationRate, std::ofstream& out_file);

#pragma endregion


#pragma region External Function Prototypes:

	extern void evaluate(Trip trip[CHROMOSOMES], const Point* coordinates);
	extern void crossover(Trip parents[TOP_X], Trip offsprings[TOP_X], const Point* coordinates);
	extern void mutate(Trip offsprings[TOP_X], const int RATE);

#pragma endregion


/*
 * MAIN: usage: Tsp #threads, mutation rate %
 */
int main(int argc, char* argv[]) 
{
	int nThreads = 1;           // number of threads
	int i_mutationRate = 50;			// mutation rate to use
	int i_output = 1;					// main return value

	std::ofstream out_file("program_output.txt", std::ios::out | std::ios::app); // output file
	
	// check that opening output file worked
	if (!out_file.is_open() || out_file.bad())
	{
		std::cout << "Opening output file failed! Exiting ..." << std::endl;
		out_file.close();
		exit(EXIT_FAILURE);
	} // end if

	// verify the arguments
	if (argc == 2)
	{
		nThreads = atoi(argv[1]);
	} // end if
	else if (argc == 3)
	{
		nThreads = atoi(argv[1]);
		i_mutationRate = atoi(argv[2]);
	} // end elif
	else
	{
		std::cout << "usage: Tsp <#threads> <mutation rate %>" << std::endl;
		if (argc != 1)
		{
			out_file.close();
			exit(EXIT_FAILURE); // wrong arguments
		} // end if
	} // end else

	// run GA
	try
	{
		i_output = run(nThreads, i_mutationRate, out_file);
	} // end try
	catch(std::exception e)
	{
		std::cout << "Execution failed!" << std::endl;
		std::cout << "Reason: " << e.what() << std::endl;
		out_file.close();
		exit(EXIT_FAILURE);
	} // end catch

	out_file.close();

	if (i_output)
	{
		std::cout << "Program exited with non-zero code!" << "\n Exit code: " << i_output << std::endl;
	} // end if

	#if defined(_WIN32) || defined(_WIN64) 
		system("pause");
	#endif

	return i_output;
} // end Main


/// <summary>
///			Runs the Genetic algorithm.
/// </summary>
/// <param name="nThreads">
///			Number of threads to use for the algorithm.
/// </param>
/// <param name="i_mutationRate">
///			Starting mutation rate to use.
/// </param>
/// <param name="out_file">
///			File handle to output file.
/// </param>
/// <returns>
///			0 on success
/// </returns>
int run(int nThreads, int i_mutationRate, std::ofstream& out_file)
{
	Trip *trip = new Trip[CHROMOSOMES],	// all 50000 different trips (or chromosomes)
		 *parents = new Trip[TOP_X],	// pool of selected parents
		 *offsprings = new Trip[TOP_X]; // pool of generate children

	Trip shortest;                      // the shortest path so far
	Point coordinates[CITIES];          // (x, y) coordinates of all 36 cities:
	
	
	int bias = 0;						// bias value for mutation rate	

	#if ENABLE_STD_OUT
		std::cout << "# threads = " << nThreads << std::endl;
		std::cout << "current rate " << i_mutationRate << std::endl;
	#endif

	out_file << "# threads = " << nThreads << std::endl;
	out_file << "current rate " << i_mutationRate << std::endl;

	// initialize 5000 trips and 36 cities' coordinates
	initialize(trip, coordinates);

	// start a timer 
	timePoint end = highRes_Clock::now();
	timePoint start = highRes_Clock::now();

	// change # of threads
	omp_set_num_threads(nThreads);

	// find the shortest path in each generation
	for (int generation = 0; generation < MAX_GENERATION; generation++) {

		// evaluate the distance of all 50000 trips
		evaluate(trip, coordinates);

		// just print out the progress
		if (!(generation % 20))
		{
			std::cout << "generation: " << generation << std::endl;

			// update mutation rate to diversify converging population
			i_mutationRate = (bias < 99 ? bias : 99);
			bias += 20;
		} // end if

		// whenever a shorter path was found, update the shortest path
		if (shortest.fitness < 0 || shortest.fitness > trip[0].fitness)
		{
			shortest = trip[0];
			#if ENABLE_STD_OUT
				std::cout << "generation: " << generation << " shortest distance = " << shortest.fitness << "\t itinerary = " << shortest.itinerary << std::endl;
			#endif
			out_file << "generation: " << generation << " shortest distance = " << shortest.fitness	<< "\t itinerary = " << shortest.itinerary << std::endl;
		} // end if

		// choose TOP_X parents from trip
		select(trip, parents);

		// generates TOP_X offsprings from TOP_X parenets
		crossover(parents, offsprings, coordinates);

		// mutate offsprings
		mutate(offsprings, i_mutationRate);

		// populate the next generation.
		populate(trip, offsprings);
	} // end for generation

	// stop the timer
	end = highRes_Clock::now();

	#if ENABLE_STD_OUT
		std::cout << " shortest distance = " << shortest.fitness << "\t itinerary = " << shortest.itinerary << std::endl;
		std::cout << "elapsed time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " ms." << std::endl;
	#endif

	out_file << " shortest distance = " << shortest.fitness << "\t itinerary = " << shortest.itinerary << std::endl;
	out_file << "elapsed time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " ms." << std::endl << std::endl << std::endl;

	// clean up
	delete[] trip;
	delete[] parents;
	delete[] offsprings;

	return EXIT_SUCCESS;
} // end method run


/*
 * Initializes trip[CHROMOSOMES] with chromosome.txt and coordiantes[CITIES][2] with cities.txt
 *
 * @param trip[CHROMOSOMES]:      50000 different trips
 * @param coordinates[CITIES][2]: (x, y) coordinates of 36 different cities: ABCDEFGHIJKLMNOPQRSTUVWXYZ
 */
void initialize(Trip trip[CHROMOSOMES], Point coordinates[CITIES]) {
	// open two files to read chromosomes (i.e., trips)  and cities
	std::ifstream chromosome_file("chromosome.txt");
	std::ifstream cities_file("cities.txt");

	// read data from the files
	// chromosome.txt:                                                                                           
	//   T8JHFKM7BO5XWYSQ29IP04DL6NU3ERVA1CZG                                                                    
	//   FWLXU2DRSAQEVYOBCPNI608194ZHJM73GK5T                                                                    
	//   HU93YL0MWAQFIZGNJCRV12TO75BPE84S6KXD
	for (int i = 0; i < CHROMOSOMES; i++) {
		chromosome_file >> trip[i].itinerary;
		trip[i].fitness = std::numeric_limits<float>::max();
	}

	// cities.txt:                                                                                               
	// name    x       y                                                                                         
	// A       83      99                                                                                        
	// B       77      35                                                                                        
	// C       14      64                                                                                        
	for (int i = 0; i < CITIES; i++) {
		char city;
		cities_file >> city;
		int index = (city >= 'A') ? city - 'A' : city - '0' + 26;
		cities_file >> coordinates[index].x >> coordinates[index].y;
	}

	// close the files.
	chromosome_file.close();
	cities_file.close();
}


/*
 * Select TOP_X parents from trip as parents
 *
 * @param trip[CHROMOSOMES]: all trips
 * @param parents[TOP_X]:    the selected TOP_X parents
 */
void select(const Trip trip[CHROMOSOMES], Trip parents[TOP_X]) 
{
	std::unordered_set<std::size_t> selected;

	// tournament selection
	for (auto i = 0; i < TOP_X; i++)
	{
		std::size_t ui_winnerIndex = randomIntInRange<std::size_t>(0, CHROMOSOMES - 1);

		while (selected.count(ui_winnerIndex) > 0)
		{
			ui_winnerIndex = randomIntInRange<std::size_t>(0, CHROMOSOMES - 1);
		} // end while

		for (auto j = 1; j < TOURNAMENT_SIZE; j++)
		{
			auto ui_candidateIndex = randomIntInRange<std::size_t>(0, CHROMOSOMES - 1);

			while (selected.count(ui_candidateIndex) > 0)
			{
				ui_candidateIndex = randomIntInRange<std::size_t>(0, CHROMOSOMES - 1);
			} // end while

			if (trip[ui_candidateIndex].fitness < trip[ui_winnerIndex].fitness)
			{
				ui_winnerIndex = ui_candidateIndex;
			} // end if
		} // end for j
		
		#pragma omp critical(winner_table)
		{
			selected.insert(ui_winnerIndex);
		} // end critical

		strncpy(parents[i].itinerary, trip[ui_winnerIndex].itinerary, CITIES + 1);
	} // end for i
} // end method select


/*
 * Replace the bottom TOP_X trips with the TOP_X offsprings
 */
void populate(Trip trip[CHROMOSOMES], Trip offsprings[TOP_X]) 
{
	// just copy TOP_X offsprings to the bottom TOP_X trips.
	#pragma omp parallel for 
	for (int i = 0; i < TOP_X; i++)
	{
		strncpy(trip[CHROMOSOMES - TOP_X + i].itinerary, offsprings[i].itinerary, CITIES + 1);
	} // end for
} // end method populate
