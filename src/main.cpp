#include <iostream>

#include <boost/program_options.hpp>

#include "imageconv.hpp"
#include "option_error.hpp"

namespace po = boost::program_options;

int main(int ac, char **av) {
    using namespace std;
    cerr << "imageconv 1.0\n"
         << "  by: hexian000 Copyright(c) 2020\n"
         << endl;

    // Declare the supported options.
    // clang-format off
	po::options_description desc("Allowed options");
	desc.add_options()
			("help,h", "show this help message")
			("input,i", po::value<string>(), "set input file")
			("output,o", po::value<string>(), "set output file")
			("weight,w", po::value<double>()->default_value(10.0), "set the weight of gaussian kernel")
            ("extend,x", po::value<unsigned>()->default_value(64u), "set max number of extended pixels")
            ("method,m", po::value<string>()->default_value("gaussian"), "specify method to use (default: gaussian)");
    // clang-format on
    options op;

    try {
        po::variables_map vm;
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cerr << desc << endl;
            return EXIT_SUCCESS;
        }

        if (!vm.count("input")) {
            throw option_error("required option is missing: input");
        }
        if (!vm.count("output")) {
            throw option_error("required option is missing: output");
        }

        op.input = vm["input"].as<string>();
        op.output = vm["output"].as<string>();
        op.weight = vm["weight"].as<double>();
        op.extend = vm["extend"].as<unsigned>();
        const auto method = vm["method"].as<string>();
        if (!op.set_method_str(method)) {
            throw option_error("unknown method: " + method);
        }

        op.check();

        imageconv o(op);
        o.run();
    } catch (const option_error &ex) {
        cerr << "argument error: " << ex.what() << endl;
        cerr << desc << endl;
        return EXIT_FAILURE;
    } catch (const exception &ex) {
        cerr << "error: " << ex.what() << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
