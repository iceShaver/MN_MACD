//
// Created by kamil on 13.03.18.
//

#include "macd.hpp"
#include <fstream>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <functional>
#include <Python.h>
#include <boost/format.hpp>
#include <future>

namespace mn_macd {




    std::shared_ptr<std::vector<double>>
    parse_file(path_t const &file_path, std::locale locale) {
        std::ifstream file(file_path);
        file.imbue(locale);
        auto result = std::make_shared<std::vector<double>>(std::istream_iterator<double>(file),
                                                            std::istream_iterator<double>());
        if (result->size() <=1 ) {
            cerr << "\nERROR: input file empty/corrupted/bad locale" << endl;
            exit(1);
        }
        return result;
    }

    std::shared_ptr<std::vector<double>>
    ema(std::vector<double> const &input, size_t period) {
        if (period > input.size()){
            cerr << "\nERROR: period is bigger than data" << endl;
            exit(2);
        }
        auto result = std::make_shared<std::vector<double>>(period);
        auto data_iterator = input.begin() + period;
        double alpha = 2 / (period - 1.0);
        while (data_iterator != input.end()) {
            double nominator = 0, denominator = 0;
            for (int i = 0; i <= period; ++i) {
                nominator += *(data_iterator - i) * std::pow(1 - alpha, i);
                denominator += std::pow(1 - alpha, i);
            }
            result->push_back(nominator / denominator);
            data_iterator++;
        }
        return result;
    }


    void write_to_file(path_t const &file_path, std::vector<double> const &data) {
        std::ofstream file;
        file.exceptions(std::ofstream::badbit | std::ofstream::failbit);
        try {
            file.open(file_path);
            for (auto const x : data) file << x << '\n';
        } catch (std::ios_base::failure const &e) {
            cerr << "An error occured while writing to file: " << file_path << '\n' << e.what()
                 << endl;
            exit(3);
        }

    }

    void
    py_gen_plots(path_t const &py_script_path,
                 std::string title,
                 path_t const &rate_in,
                 path_t const &signal_in,
                 path_t const &macd_in,
                 path_t const &rate_out,
                 path_t const &macd_signal_out) {

        if (!std::experimental::filesystem::exists(py_script_path)) {
            cerr << "Python program " << py_script_path
                 << " does not exists in current location. It is needed for a plot generation." << endl;
            exit(1);
        };
        Py_SetProgramName(const_cast<wchar_t *>(L"MACD plot generator"));
        Py_Initialize();
        auto file = std::ifstream(py_script_path);
        auto python_code_template = std::move(
                std::string(
                        std::istreambuf_iterator<char>(file),
                        std::istreambuf_iterator<char>()));
        file.close();
        auto python_code = boost::format(python_code_template) %
                           title %
                           signal_in %
                           macd_in %
                           rate_in %
                           macd_signal_out %
                           rate_out;
        PyRun_SimpleString(python_code.str().c_str());
        Py_Finalize();
    }

    std::tuple<
            std::shared_ptr<std::vector<double>>, // macd
            std::shared_ptr<std::vector<double>>, // signal
            std::shared_ptr<std::vector<double>>> // rate
    compute_macd(std::shared_ptr<std::vector<double>> input) {
        auto ema12_thread = std::async(std::launch::async, mn_macd::ema, *input, 12);
        auto ema26_thread = std::async(std::launch::async, mn_macd::ema, *input, 26);
        auto macd = std::make_shared<std::vector<double>>();
        macd->reserve(input->size());
        auto ema12 = ema12_thread.get();
        auto ema26 = ema26_thread.get();

        std::transform(ema12->begin(), ema12->end(), ema26->begin(), std::back_inserter(*macd),
                       std::minus<>()); //MACD = EMA12 - EMA26
        auto signal = mn_macd::ema(*macd, 9);

        // Trim data at beginning
        auto trim_data = [](auto x, size_t offset) { x->erase(x->begin(), x->begin() + offset); };
        trim_data(macd, 35);
        trim_data(signal, 35);
        trim_data(input, 35);
        return std::make_tuple(macd, signal, input);
    }

}
