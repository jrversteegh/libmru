#include <string> 
#include <sstream> 

struct Error { 
  Error(const std::string message, int error = 0): message_(message), error_(error) {}; 
  std::string get_message() const { 
    std::ostringstream os(message_);
    os << " Error: " << error_; 
    return os.str();
  }
private: 
  int error_;
  std::string message_; 
};
