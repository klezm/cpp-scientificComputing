#include <iostream>
#include <chrono>
#include <vector>
#include <map>
#include <list>
#include <sstream>
#include <memory>

const int SIZE_ARRAY = 100000000;

// define our containers
// the last one is the STL implementation of a c-array
// as we cannot define it at runtime, we chose to always go with the maximum size
typedef std::vector<std::string> Vector;
typedef std::map<int,std::string> Map;
typedef std::list<std::string> List;
typedef std::array<std::string,SIZE_ARRAY> Array;


struct Measure
{
    /**
     * \brief Function to measure the time a function took to execute
     * \param func  The function to measure
     * \param args  The function's arguments
     * \return      Time the function took in seconds
     */
    template <typename F, typename... Args>
    static double duration ( F& func, Args&... args )
    {
        auto start = std::chrono::steady_clock::now();
        func( args... );
        auto end = std::chrono::steady_clock::now();
        return std::chrono::duration<double>( end - start ).count();
    }
};

/**
 * \brief Helper function to fill a container with hexadecimal numbers
 * \param func Function to insert one element into the container
 * \param args The container
 * \param N    Max Number
 */
template <typename F, typename... Args>
void fill ( F& func, Args&... args, int N )
{
    for ( int i = 0; i < N; ++i )
    {
        std::stringstream ss;
        ss << std::hex << i;
        func ( args..., i, ss.str() );
    }
};

/**
 * The next functions are used to insert one element into the different containers.
 */
void insert_vector ( std::shared_ptr<Vector> v, int i, std::string s )
{
    (*v)[i] = s;
}

void insert_map ( std::shared_ptr<Map> m, int i, std::string s )
{
    m->insert( std::make_pair( i, s ) );
}

void insert_list ( std::shared_ptr<List> l, int i, std::string s )
{
    l->push_back( s );
}

void insert_array ( std::shared_ptr<Array> array, int i, std::string s )
{
    (*array)[i];
}

int main ()
{
    // the limits
    std::vector<unsigned> limitsN { 100000, 1000000, 10000000, 100000000 };

    for ( auto&& n : limitsN )
    {
        // our containers, we need to allocate the array on the heap (stack is too small)
        // so we allocate all of them on the heap
        // and use smart pointer to delete their contents once they are not needed anymore
        // on my system this code needed at least 10gb of RAM for the last value of n!
        std::shared_ptr<Vector> v ( new Vector ( n ) );
        std::shared_ptr<Map> m ( new Map );
        std::shared_ptr<List> l ( new List );
        std::shared_ptr<Array> a ( new Array );

        printf( "Measuring now for N = %d\n", n );
        printf( "Vector: %lf\n", Measure::duration( fill<decltype(insert_vector), decltype(v)>, insert_vector, v, n ) );
        v.reset(); // deleting v here reduces memory usage
        printf( "Map:    %lf\n", Measure::duration( fill<decltype(insert_map),    decltype(m)>, insert_map,    m, n ) );
        m.reset();
        printf( "List:   %lf\n", Measure::duration( fill<decltype(insert_list),   decltype(l)>, insert_list,   l, n ) );
        m.reset();
        printf( "Array:  %lf\n", Measure::duration( fill<decltype(insert_array),  decltype(a)>, insert_array,  a, n ) );
    }

    return 0;
}