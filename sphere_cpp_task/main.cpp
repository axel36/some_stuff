#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <algorithm>

class LinesController{
public:
  void AddLine(const std::string& line){
   std::vector<std::string> new_line;

   auto start = 0U;
   auto end = line.find(delim_);
   while (end != std::string::npos)
   {
     const auto& token =  line.substr(start, end - start);
     new_line.push_back(token);

     start = end + delim_.length();
     end = line.find(delim_, start);
   }
   storage_.push_back(new_line);
  }
  void PrintData(){
    for (const auto& line : storage_){
      for(const auto & elem: line){
        std::cout << elem << "\t";
      }
      std::cout << std::endl;
    }
  }

  void ApplyCommandsStr(const std::vector<std::string>& commands){
    for(const auto& command: commands){
      ApplyCommand(ParseCommand(command));
    }
  }
private:
  std::vector<std::vector<std::string>> storage_;
  std::string delim_ = "\t";

  struct Command{
    enum CommandType {UPPER, LOWER, REPLACE};
    CommandType type_;
    int field;
    char a_;
    char b_;
  };


  Command ParseCommand(const std::string& str_command) {
    int pos = 0;
//    Get field number
    std::string number;
    for (; pos < str_command.size(); pos++) {
      if (!isdigit(str_command[pos])) {
        break;
      }
      number += str_command[pos];
    }
    int field;
    try {
      field = std::stoi(number);
    } catch (std::invalid_argument &e) {
      throw std::runtime_error("invalid command field number - not int: " + str_command);
    }
    catch (std::out_of_range &e) {
      throw std::runtime_error("invalid command field number - too large: " + str_command);
    }

    if (str_command.size() <= pos || str_command.size() <= pos+1 || str_command[pos] != ':') {
      throw std::runtime_error("invalid command: " + str_command);
    }

    //    Get command_type
    pos++; // skip ':' charracter
    Command::CommandType command_type;
    switch (str_command[pos]) {
      case 'u':command_type = Command::LOWER;
        break;
      case 'U':command_type = Command::UPPER;
        break;
      case 'R':command_type = Command::REPLACE;
        break;
      default:
        throw std::runtime_error("invalid type in command " + str_command);
    }
    // get characters
    if (command_type == Command::REPLACE){
      if (str_command.size() - 1 != pos+2 ) { // count of characters for replace
        throw std::runtime_error("invalid command: " + str_command);
      }
      return Command{command_type, field, str_command[pos+1], str_command[pos+2]};
    }
    return Command{command_type, field};
  }


  void ApplyCommand(const Command& command){
    switch (command.type_) {
      case Command::LOWER:
        ApplyLower(command.field);
        break;
      case Command::UPPER:
        ApplyUpper(command.field);
        break;
      case Command::REPLACE:
        ApplyReplace(command.field, command.a_, command.b_);
        break;
      default:
        throw std::runtime_error("invalid command in ApplyCommand");
    }
  }

  void ApplyLower(int field){
    for (auto& line: storage_){

      if (line.size() <= field){
        throw std::runtime_error("can't apply command LOWER to all data, N: " + std::to_string(field));
      }
      std::transform(line[field].begin(), line[field].end(), line[field].begin(), ::tolower);
    }
  }
  void ApplyUpper(int field){
    for (auto& line: storage_){
      if (line.size() <= field){
        throw std::runtime_error("can't apply command UPPER to all data, N: " + std::to_string(field));
      }
      std::transform(line[field].begin(), line[field].end(), line[field].begin(), ::toupper);
    }
  }
  void ApplyReplace(int field, char a, char b){
    for (auto& line: storage_){
      if (line.size() <= field){
        throw std::runtime_error("can't apply command REPLACE to all data, N: " + std::to_string(field));
      }
      std::replace(line[field].begin(), line[field].end(), a, b);
    }
  }
};

// test_input.txt 1:u 0:U 2:RjH - arguments
int main(int argc, char* argv[]) {

  std::string input_file_name = std::string(argv[1]);

  std::vector<std::string> commands;
  commands.reserve(argc - 2);
  for (int i = 2; i < argc; i++){
    commands.emplace_back(argv[i]);
  }


  std::ifstream input_file(input_file_name);
  if (!input_file.is_open()) {
    std::cout << "can't open file " << input_file_name
              << " sry, try again next time" << std::endl;
    return 0;
  }
  LinesController lc;
  std::string line;
  while (std::getline(input_file, line)) {
    lc.AddLine(line);
  }
  std::cout << "input data: " << std::endl;
  lc.PrintData();
  try {
    lc.ApplyCommandsStr(commands);
  } catch (const std::runtime_error& ex) {
    std::cout << "raise error during commands processing: " << ex.what();
    return 0;
  } catch (const std::exception& ex) {
    std::cout << "something went wrong: " << ex.what();
    return 0;
  }
  std::cout << std::endl;

  std::cout << "input commands: " << std::endl;
  for(const auto& com: commands){
    std::cout << com << " ";
  }
  std::cout << std::endl;
  std::cout << std::endl;

  std::cout << "output data: " << std::endl;
  lc.PrintData();


  return 0;
}
