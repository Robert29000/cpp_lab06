// Copyright 2020 Your Name <your_email>

#include <picosha2.h>
#include <nlohmann/json.hpp>
#include <boost/log/trivial.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <thread>
#include <future>
#include <cstdlib>
#include <chrono>
#include <vector>
#include <string>
#include <mutex>
#include <iostream>
#include <fstream>
#include <csignal>
#include <hash_checker.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;

using nlohmann::json;

void init_logger();
void calculate_hash(int n);
void add_data_to_json(const std::string& input, const std::string& hash);
void signal_callback_handler(int signum);


std::mutex mut;
json array;
std::chrono::time_point<std::chrono::system_clock> begin;
std::string file_name;

int main(int argc, char* argv[]){
  unsigned int n, m;
  if (argc == 4){
    n = std::stoi(argv[1]);
    m = std::stoi(argv[2]);
    file_name = argv[3];
  }else if (argc == 3){
    n = std::stoi(argv[1]);
    m = std::thread::hardware_concurrency();
    file_name = argv[2];
  } else {
    throw std::runtime_error("Incorrect number of parameters");
  }
  init_logger();
  array = json::array();
  signal(SIGINT, signal_callback_handler);
  std::srand(std::time(nullptr));
  std::vector<std::future<void>> futures;
  begin = std::chrono::system_clock::now();
  for (unsigned int i = 0; i < m; i++){
    futures.push_back(std::async(calculate_hash, n));
  }
  return 0;
}


void calculate_hash(int n) {
  for (;;) {
    auto value = std::rand();
    std::string svalue = std::to_string(value);
    std::string hash;
    picosha2::hash256_hex_string(svalue, hash);
    BOOST_LOG_TRIVIAL(trace) << "Input: " << svalue << " Hash: " << hash;
    if (check_hash(n, hash)){
      add_data_to_json(svalue, hash);
      BOOST_LOG_TRIVIAL(info) << "Input: " << svalue << " Hash: " << hash;
    }
  }
}

void add_data_to_json(const std::string& input, const std::string& hash){
  std::lock_guard<std::mutex> guard(mut);
  json temp;
  auto time = std::chrono::system_clock::now();
  temp["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(time - begin).count();
  temp["hash"] = hash;
  temp["data"] = input;
  array.push_back(temp);
}


void init_logger(){
  boost::log::add_common_attributes();
  boost::shared_ptr<logging::core> core = logging::core::get();
  core->add_global_attribute("ThreadID", attrs::current_thread_id());
  boost::shared_ptr<sinks::text_file_backend > backend =
      boost::make_shared<sinks::text_file_backend>(
          keywords::file_name = "Logs/file_%5N.log",
          keywords::rotation_size = 5 * 1024 * 1024
  );

  typedef sinks::synchronous_sink<sinks::text_file_backend> sink_f; // file sink
  typedef sinks::synchronous_sink<sinks::text_ostream_backend> sink_c; // console sink
  boost::shared_ptr<sink_c> console_sink(new sink_c());
  boost::shared_ptr<sink_f> file_sink(new sink_f(backend));
  boost::shared_ptr<std::ostream> stream(&std::cout, boost::null_deleter());
  console_sink->locked_backend()->add_stream(stream);
  file_sink->set_formatter(expr::stream
                               << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
                               << ": <" << logging::trivial::severity
                               << "> <"  << expr::attr<attrs::current_thread_id::value_type>("ThreadID")
                               << "> " << expr::smessage);
  console_sink->set_formatter(expr::stream
                                  << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
                                  << ": <" << logging::trivial::severity
                                  << "> ["  << expr::attr<attrs::current_thread_id::value_type>("ThreadID")
                                  << "] " << expr::smessage);
  core->add_sink(file_sink);
  core->add_sink(console_sink);
}

void signal_callback_handler(int signum){
  std::ofstream file(file_name + ".json");
  file << std::setw(4) << array << std::endl;
  file.close();
  std::cout.flush();
  exit(signum);
}