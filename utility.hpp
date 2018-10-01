
#pragma region Includes:

    #include <random>      // random device, mersenne twister, uniform distributions

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
    inline T distance(T x1, T y1, T x2, T y2)
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


    /// <summary>
    ///          Generates a random <see cref="std::RealType"/> value in the range [<paramref name="t_MIN"/>,<paramref name="t_MAX"/>).
    /// </summary>
    /// <typeparam name="T">
    ///                     Some <see cref="std::RealType"/> to initialize the random engine to.
    /// </typeparam>
    /// <param name="t_MIN">
    ///                    Minimum value of the real distribution (inclusive).
    /// </param>
    /// <param name="t_MAX">
    ///                    Maximum value of the real distribution (exclusive).
    /// </param>
    /// <returns>
    ///          A pseudo-random value in the given range.
    /// </returns>
    /// <exception cref=""></exception>
    template<typename T>
    inline T randomRealInRange(const T t_MIN, const T t_MAX)
    {
        static std::random_device rd{};
        static std::mt19937 engine{rd()};
        std::uniform_real_distribution<T> dist{t_MIN, t_MAX};

        return dist(engine);
    } // end template randomRealInRange


#pragma endregion


