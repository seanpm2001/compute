//---------------------------------------------------------------------------//
// Copyright (c) 2015 Jakub Szuppe <j.szuppe@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://kylelutz.github.com/compute for more information.
//---------------------------------------------------------------------------//

#include <algorithm>
#include <iostream>
#include <vector>

#include <thrust/find.h>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>

#include "perf.hpp"

// Max integer that can be generated by rand_int() function.
int rand_int_max = 25;

int rand_int()
{
    return static_cast<int>((rand() / double(RAND_MAX)) * rand_int_max);
}

int main(int argc, char *argv[])
{
    perf_parse_args(argc, argv);
    std::cout << "size: " << PERF_N << std::endl;

    // create vector of random numbers on the host
    thrust::host_vector<int> host_vector(PERF_N);
    thrust::generate(host_vector.begin(), host_vector.end(), rand_int);

    thrust::device_vector<int> v = host_vector;

    // trying to find element that isn't in vector (worst-case scenario)
    int wanted = rand_int_max + 1;
    
    // result
    thrust::device_vector<int>::iterator device_result_it;
    
    perf_timer t;
    for(size_t trial = 0; trial < PERF_TRIALS; trial++){
        t.start();
        device_result_it = thrust::find(v.begin(), v.end(), wanted);
        cudaDeviceSynchronize();
        t.stop();
    }
    std::cout << "time: " << t.min_time() / 1e6 << " ms" << std::endl;

    // verify
    if(device_result_it != v.end()){
        std::cout << "ERROR: "
                  << "device_result_iterator != "
                  << "v.end()"
                  << std::endl;
        return -1;
    }
    
    return 0;
}
