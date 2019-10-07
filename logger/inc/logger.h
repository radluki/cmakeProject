#include <iostream>
#include <string.h>

class Logger
{
public:
	Logger(std::ostream& ostr) : ostr{ostr} {}
	~Logger() {
		ostr << std::endl;
	}
	std::ostream& getStream(const std::string& filename, const std::string& function, unsigned line) {
		return ostr << filename << ":" << function << "():"<< line << " ";
	}
private:
	std::ostream& ostr;
};

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG_BASE(stream) Logger{stream}.getStream(__FILENAME__, __FUNCTION__, __LINE__)
#define LOG LOG_BASE(std::cout)


