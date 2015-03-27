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

#include <boost/compute/system.hpp>
#include <boost/compute/algorithm/find.hpp>
#include <boost/compute/container/vector.hpp>

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

    // setup context and queue for the default device
    boost::compute::device device = boost::compute::system::default_device();
    boost::compute::context context(device);
    boost::compute::command_queue queue(context, device);
    std::cout << "device: " << device.name() << std::endl;

    // create vector of random numbers on the host
    std::vector<int> host_vector(PERF_N);
    std::generate(host_vector.begin(), host_vector.end(), rand_int);

    // create vector on the device and copy the data
    boost::compute::vector<int> device_vector(PERF_N, context);
    boost::compute::copy(
        host_vector.begin(),
        host_vector.end(),
        device_vector.begin(),
        queue
    );

    // trying to find element that isn't in vector (worst-case scenario)
    int wanted = rand_int_max + 1;

    // device iterator
    boost::compute::vector<int>::iterator device_result_it;

    perf_timer t;
    for(size_t trial = 0; trial < PERF_TRIALS; trial++){
        t.start();
        device_result_it = boost::compute::find(device_vector.begin(),
                                                device_vector.end(),
                                                wanted,
                                                queue);
        queue.finish();
        t.stop();
    }
    std::cout << "time: " << t.min_time() / 1e6 << " ms" << std::endl;

    // verify if found index is correct by comparing it with std::find() result
    size_t host_result_index = std::distance(host_vector.begin(),
                                             std::find(host_vector.begin(),
                                                       host_vector.end(),
                                                       wanted));
    size_t device_result_index = device_result_it.get_index();

    if(device_result_index != host_result_index){
        std::cout << "ERROR: "
                  << "device_result_index (" << device_result_index << ") "
                  << "!= "
                  << "host_result_index (" << host_result_index << ")"
                  << std::endl;
        return -1;
    }

    return 0;
}
