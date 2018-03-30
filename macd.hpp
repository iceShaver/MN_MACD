//
// Created by kamil on 13.03.18.
//

#ifndef MN_MACD_MACD_HPP
#define MN_MACD_MACD_HPP

#include <bits/unique_ptr.h>
#include <vector>
#include <memory>
#include <experimental/filesystem>
#include <iostream>


namespace mn_macd {
    using namespace std::string_literals;
    using path_t = std::experimental::filesystem::path;
    using std::cerr;
    using std::endl;

    std::shared_ptr<std::vector<double>>
    parse_file(path_t const &file_path, std::locale locale = std::locale("en_US.utf8"));

    std::tuple<
            std::shared_ptr<std::vector<double>>, // macd
            std::shared_ptr<std::vector<double>>, // signal
            std::shared_ptr<std::vector<double>>> // rate
    compute_macd(std::shared_ptr<std::vector<double>> input);

    std::shared_ptr<std::vector<double>>
    ema(std::vector<double> const &input, size_t period);


    void write_to_file(path_t const &file_path, std::vector<double> const &data);


    void py_gen_plots(path_t const &py_script_path,
                      std::string title,
                      path_t const &rate_in,
                      path_t const &signal_in,
                      path_t const &macd_in,
                      path_t const &rate_out,
                      path_t const &macd_signal_out);
}
#endif //MN_MACD_MACD_HPP
