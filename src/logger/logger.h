#ifndef __STUDY_LOGGER_H__
#define __STUDY_LOGGER_H__

#include <string>
#include "namespace.h"

namespace STUDY_NAMESPACE 
{
    /** Logger: */
    class Logger
    {
    private:
        /* data */
    public:
        Logger() = default;
        Logger(const std::string& name = "default");
        ~Logger();
    };
    
    Logger::Logger(/* args */)
    {
    }
    
    Logger::~Logger()
    {
    }
    
    


}


#endif  /** __STUDY_LOGGER_H__ */