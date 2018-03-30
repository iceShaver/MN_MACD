
//
// Created by kamil on 13.03.18.
//

#include <iostream>
#include <algorithm>
#include <Python.h>
#include <fstream>
#include <future>
#include <experimental/filesystem>
#include "macd.hpp"

using namespace std::string_literals;
using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using path_t = std::experimental::filesystem::path;

void printUsage() {
    cout << "Usage\n";
    cout << "mn_macd"
            " [in input_data]"
            " [out rate_plot]"
            " [out signal_macd_plot]"
            " [out macd_data]"
            " [out signal_data]"
            " [out rate_data]\n";
    cout << "If args not specified, default values are applied:\n";
    cout << "[input_data] = data.txt\n";
    cout << "[rate_plot] = rate.png\n";
    cout << "[signal_macd_plot] = macd-signal.png\n";
    cout << "[macd_data] = macd.txt\n";
    cout << "[signal_data] = signal.txt\n";
    cout << "[rate_data] = rate.txt" << endl;
}

int main(int argc, char *argv[]) {

    // print help
    if (argc == 2 && argv[1] == "--help"s) {
        printUsage();
        exit(1);
    }

    // Handle program args or set default values
    path_t input_data_path =
            argc == 2 ? argv[1] : "data.txt"s;
    path_t output_rate_plot_path =
            argc == 3 ? argv[2] : "rate.png"s;
    path_t output_signal_macd_plot_path =
            argc == 4 ? argv[3] : "macd-signal.png"s;
    path_t output_macd_data_path =
            argc == 5 ? argv[4] : "macd.txt"s;
    path_t output_signal_data_path =
            argc == 6 ? argv[5] : "signal.txt"s;
    path_t output_rate_data_path =
            argc == 7 ? argv[6] : "rate.txt"s;


    // Read data
    if (!std::experimental::filesystem::exists(input_data_path)) {
        cerr << "Given data file does not exist: " << input_data_path.filename() << endl;
        printUsage();
        exit(1);
    };
    cout << "Reading data from " << input_data_path << "... " << flush;
    // set locale pl_pl.utf8 if comma decimal separator
    auto input_data = mn_macd::parse_file(input_data_path, std::locale("pl_PL.utf8"));
    cout << "OK\n";


    // Compute data
    cout << "Computing macd and signal... " << flush;
    auto[macd, signal, rate] = mn_macd::compute_macd(input_data);
    cout << "OK\n";


    // Save data
    cout << "Saving results to " <<
         output_macd_data_path << ", " <<
         output_signal_data_path << ", " <<
         output_rate_data_path
         << "... " << flush;
    mn_macd::write_to_file(output_macd_data_path, *macd);
    mn_macd::write_to_file(output_signal_data_path, *signal);
    mn_macd::write_to_file(output_rate_data_path, *rate);
    cout << "OK\n";



    // Generate plots with external python script
    cout << "Generating plots... " << flush;
    mn_macd::py_gen_plots(
            "make_plots.py"s,
            input_data_path.stem(),
            output_rate_data_path,
            output_signal_data_path,
            output_macd_data_path,
            output_rate_plot_path,
            output_signal_macd_plot_path);
    std::cout << "OK" << std::endl;

    return 0;
}
