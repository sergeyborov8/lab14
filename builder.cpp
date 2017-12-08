#include <iostream>
#include <chrono>
#include <boost/program_options.hpp>
#include <boost/process.hpp>

namespace bp = boost::process;
namespace po = boost::program_options;

bp::child Make(int time = 0, std::string build="Debug")
{
  std::string cmd("cmake -H. -B_builds -DCMAKE_INSTALL_PREFIX=");
  cmd += "_install";
  cmd += " -DCMAKE_BUILD_TYPE=";
  cmd += build;
  std::cout << "command = " << cmd << '\n';
  bp::child c(cmd, bp::std_out > stdout);
  if (time) {
    if (!c.wait_for(std::chrono::seconds(time)));
      c.terminate();
  }
  else
    c.wait();
  return c;
}

bp::child Build(int time = 0)
{
  std::string cmd("cmake --build _builds");

  std::cout << "command = " << cmd << '\n';
  bp::child c(cmd, bp::std_out > stdout);
  if (time){
    if (!c.wait_for(std::chrono::seconds(time)));
      c.terminate();
  }
  else
    c.wait();
  return c;
}

bp::child Target(std::string inst)
{
  std::string cmd("cmake --build _builds");
  cmd += " --target ";
  cmd +=inst;

  std::cout << "command = " << cmd << '\n';
  bp::child c(cmd, bp::std_out > stdout);
  c.wait();
  return c;
}

int main(int argc, char *argv[])
{
    po::options_description params("Allowed options");
    params.add_options()
      ("help", "выводим вспомогательное сообщение")
      ("config", po::value<std::string>(), "указываем конфигурацию сборки (по умолчанию Debug)")
      ("install", "добавляем этап установки\n(в директорию _install)")
      ("pack", "добавляем этап упакови\n(в архив формата tar.gz)")
      ("timeout", po::value<int>(), "указываем время ожидания\n(в секундах)")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, params), vm);
    po::notify(vm);

    int res;
    if(vm.count("help"))
    {
      std::cout << params << '\n';
      return 1;
    }
    else if (vm.count("config"))
    {
      std::string par(vm["config"].as<std::string>());
      res = Make(0, par).exit_code();
      if (!res)
        res = Build().exit_code();
      std::cout << "code == " << res << '\n';
    }
    else if (vm.count("install") && vm.count("pack"))
    {
      res = Make().exit_code();
      if (!res)
        res = Build().exit_code();
      if (!res)
        res = Target("install").exit_code();
        if (!res)
          res = Target("pack").exit_code();
      std::cout << "code == " << res << '\n';
    }
    else if (vm.count("install"))
    {
      res = Make().exit_code();
      if (!res)
        res = Build().exit_code();
      if (!res)
        res = Target("install").exit_code();
      std::cout << "code == " << res << '\n';
    }
    else if (vm.count("pack"))
    {
      res = Make().exit_code();
      if (!res)
        res = Build().exit_code();
      if (!res)
        res = Target("package").exit_code();
      std::cout << "code == " << res << '\n';
    }
    else if (vm.count("timeout"))
    {
      int tim = vm["timeout"].as<int>();
      res = Make(tim).exit_code();
      if (!res)
        res = Build(tim).exit_code();
      std::cout << "code == " << res << '\n';
    }
    else {
      res = Make().exit_code();
      if (!res)
        res = Build().exit_code();
      std::cout << "code == " << res << '\n';
    }
    return 0;
}
