
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>

#include "easylogging++.h"
#include "meta_tester.h"


INITIALIZE_EASYLOGGINGPP

void init_log() {
  el::Configurations conf;
  conf.setToDefault();
  char log_path[NAME_MAX] = "./test.log";
  conf.set(el::Level::Global, el::ConfigurationType::Filename, log_path);
  el::Loggers::reconfigureAllLoggers(conf);
}

void help() {
  std::cout << "  usage: " << "./meta_tester  so_path data_dir" <<std::endl;
  std::cout << "  eg: " << "./meta_tester ./libmeta_mgr.so ./data"<< std::endl;
  exit(-1);
}

int main(int argc, char *argv[]) {
  START_EASYLOGGINGPP(argc, argv);
  init_log();
  if (argc != 3) {
    help();
    std::cout << "parameter error, expect 3, but get " << argc << std::endl;
    return -1;
  }
  const char *so_path = argv[1];
  const char *data_path = argv[2];

  LOG(INFO) << "Begin test, it is just a demo!";
  LOG(INFO) << "  >> libmeta_mgr path : " << so_path;
  LOG(INFO) << "  >> data path : " << data_path;
  MetaTester tester;
  tester.Init(so_path, data_path);
  int error = 0;
  double time = tester.Run(error);
  LOG(INFO) << "time: " << time << " seconds, error number: " << error;
  tester.Deinit();
  return 0;
}
