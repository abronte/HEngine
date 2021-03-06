/**
 * @author VaL Doroshchuk
 * @copyright Alex X. Liu, Ke Shen, Eric Torng, VaL Doroshchuk
 * @license GNU GPL v2
 */

/**
 * Finds duplicates in a list
 */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/resource.h>

#include "HEngine_sn.h"

using namespace hengine;

int main( int argc, char **argv )
{
    float  userTime;
    struct rusage startTime, stopTime;

    getrusage( RUSAGE_SELF, &startTime );

    if ( argc != 3 )
    {
        std::cout << "Usage: " << argv[0] << " <k> <data file>" << std::endl << std::endl;

        return 1;
    }

    unsigned k = atoi( argv[1] );
    std::string line;

    std::cout << "Reading the dataset ........ ";
    fflush( stdout );

    NumTable db;
    std::ifstream dict;
    dict.open( argv[2], std::ifstream::in );
    while ( getline( dict, line ) )
    {
        Number h;

        std::istringstream reader( line );
        reader >> h;
        db.push_back( h );
    }

    std::cout <<  "done. " << db.size() << " hashes" << std::endl;
    std::cout << "Building with " << k << " hamming distance bound ....... ";
    fflush( stdout );

    HEngine_sn e( k );
    e.build( db );

    std::cout << "done." << std::endl;

    getrusage( RUSAGE_SELF, &stopTime );
    userTime =
                ( (float) ( stopTime.ru_utime.tv_sec  - startTime.ru_utime.tv_sec ) ) +
                ( (float) ( stopTime.ru_utime.tv_usec - startTime.ru_utime.tv_usec ) ) * 1e-6;

    std::cout << std::endl;
    std::cout << "Building time: " << userTime << " seconds" << std::endl;
    std::cout << std::endl;

    std::cout << "Searching duplicates ......." << std::endl;

    getrusage( RUSAGE_SELF, &startTime );
    int c = 0;
    int s = db.size();
    int i = 0;
    for ( auto &h: db )
    {
        Matches res = e.query( h );
        c += res.size();
        if ( i % 1000 == 0 )
        {
            std::cout << "                                      ";
            std::cout << "\r" << (i + 0.0)/ s  << " % " << i << "/" << s;
        }
        i++;
    }

    getrusage( RUSAGE_SELF, &stopTime );
    userTime =
                ( (float) ( stopTime.ru_utime.tv_sec  - startTime.ru_utime.tv_sec ) ) +
                ( (float) ( stopTime.ru_utime.tv_usec - startTime.ru_utime.tv_usec ) ) * 1e-6;

    std::cout << std::endl << "found " << c  << " total matches. Query time: " << userTime << " seconds" << std::endl << std::endl;
    std::cout << std::endl;


    getrusage( RUSAGE_SELF, &startTime );
    std::cout << "Searching linear duplicates ......." << std::endl;
    i = 0;
    for ( auto &h1: db )
    {
        int c = 0;
        for ( auto &h2: db )
        {
            unsigned d = HEngine::getHammingDistance( h1, h2 );
            if ( d <= k )
            {
                c++;
            }
        }

        if ( i % 1000 == 0 )
        {
            std::cout << "                                             ";
            std::cout << "\r" << (i + 0.0)/ s  << " % " << i << "/" << s;
        }
        i++;
    }

    getrusage( RUSAGE_SELF, &stopTime );
    userTime =
                ( (float) ( stopTime.ru_utime.tv_sec  - startTime.ru_utime.tv_sec ) ) +
                ( (float) ( stopTime.ru_utime.tv_usec - startTime.ru_utime.tv_usec ) ) * 1e-6;

    std::cout << std::endl << "found " << c  << " total matches. Linear query time: " << userTime << " seconds" << std::endl << std::endl;
    std::cout << std::endl;



    return 0;
}
