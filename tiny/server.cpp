#include "../include/httplib.h"
#include "../include/json.hpp"

#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

using json = nlohmann::json;

std::string exec(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

int main()
{
    httplib::Server svr;

    svr.Post("/ast", [](const httplib::Request &req, httplib::Response &res) {
        std::string code = req.body;
        std::cout << "Code Input: " << code << std::endl;

        // Write the code to input.tny file
        std::ofstream input_file;
        input_file.open("input.tny");
        input_file << code;
        input_file.close();

        try
        {
            std::string result = exec("./tiny input.tny");
            std::cout << result << std::endl;

            json j;
            j["ast"] = result;

            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(j.dump(), "application/json");
        }
        catch (std::exception &e)
        {
            std::cout << "Exception: " << e.what() << std::endl;
        }
    });

    std::cout << "Starting api server on http://localhost:8080..." << std::endl;
    std::cout << "Waiting for requests..." << std::endl;
    svr.listen("0.0.0.0", 8080);
}