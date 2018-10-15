
#ifndef __UTILITY_H__
#define __UTILITY_H__

#pragma region Includes:

    #include <random>        // random device, mersenne twister, uniform distributions
    #include <unordered_map> // hash map
	#include <chrono>		 // timer

#pragma endregion


#pragma region Typedefs

typedef std::chrono::high_resolution_clock::time_point	timePoint;
typedef std::chrono::high_resolution_clock				highRes_Clock;

#pragma endregion


#pragma region Defines:

    #ifndef NULL
        #define NULL (void*)0
    #endif
	#ifndef EXIT_SUCCESS
		#define EXIT_SUCCESS 0
	#endif
	#ifndef EXIT_FAILURE
		#define EXIT_FAILURE 1
	#endif
	#ifndef ENABLE_STD_OUT
		#define ENABLE_STD_OUT 1
	#endif
	#ifndef TOURNAMENT_SIZE
		#define TOURNAMENT_SIZE    20     // number of participants in a tournament
	#endif

#pragma endregion


#pragma region Structs

    struct Point
    {
        int x;
        int y;
        Point(int xval, int yval) {x = xval; y = yval;}
		Point() { x = 0; y = 0; }

        inline bool operator==(const Point& OTHER) const
        {
            return x == OTHER.x && y == OTHER.y;
        } // end operator ==
    }; // end struct Point

    namespace std
    {
        template <>
        struct hash<Point>
        {
            /// <summary>
            ///          Use of the unorded_map requires a hashing function. This utilizes Szudzik's function for hashing
            ///             (a, b) -> H, where H is the hash associated with the tuple (a, b).
            /// </summary>
            /// <param name="p">
            ///                 The point to hash.
            /// </param>
            /// <returns>
            ///          An unsigned hash value for the given point.
            /// </returns>
            /// <remarks>
            ///          This hashing function is directly added to the std namespace to allow the unordered map to see it
            ///          without needing to provide additional parameters to the constructor.
            /// </remarks>
            std::size_t operator()(const Point& p) const
            {
                long long A = p.x >= 0 ? 2 * p.x : -2 * p.x - 1;
                long long B = p.y >= 0 ? 2 * p.y : -2 * p.y - 1;
                return A >= B ? static_cast<std::size_t>(abs(A * A + A + B)) : static_cast<std::size_t>(abs(A + B * B));
            } // end operator ()
        }; // end struct hash<Point>
    } // end std

#pragma endregion


#pragma region Templates:

    /// <summary>
    ///          Finds the euclidean distance between a pair of two-dimensional points.
    /// </summary>
    /// <typeparam name="T">
    ///                     Some <see cref="std::NumericType"/>.
    /// </typeparam>
    /// <param name="x1">
    ///                    x-coordinate of the first point.
    /// </param>
    /// <param name="y1">
    ///                    y-coordinate of the first point.
    /// </param>
    /// <param name="x2">
    ///                    x-coordinate of the second point.
    /// </param>
    /// <param name="y2">
    ///                    y-coordinate of the second point.
    /// </param>
    /// <returns>
    ///          The distance between the given points.
    /// </returns>
    /// <exception cref=""></exception>
    template<typename T>
    inline float distance(T x1, T y1, T x2, T y2)
    {
        return sqrt(pow((static_cast<double>(x2)-static_cast<double>(x1)),2.0) + pow((static_cast<double>(y2)-static_cast<double>(y1)), 2.0));
    } // end template distance


    /// <summary>
    ///          Generates a random <see cref="std::IntType"/> value in the range [<paramref name="t_MIN"/>,<paramref name="t_MAX"/>].
    /// </summary>
    /// <typeparam name="T">
    ///                     Some <see cref="std::IntType"/> to initialize the random engine to.
    /// </typeparam>
    /// <param name="t_MIN">
    ///                    Minimum value of the integer distribution (inclusive).
    /// </param>
    /// <param name="t_MAX">
    ///                    Maximum value of the integer distribution (inclusive).
    /// </param>
    /// <returns>
    ///          A pseudo-random value in the given range.
    /// </returns>
    /// <exception cref=""></exception>
    template<typename T>
    inline T randomIntInRange(T t_MIN, T t_MAX)
    {
        static std::random_device rd{};
        static std::mt19937 engine{rd()};
        std::uniform_int_distribution<T> dist{t_MIN, t_MAX};

        return dist(engine);
    } // end template randomIntInRange

#pragma endregion

#endif
