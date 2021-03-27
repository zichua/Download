#include "HttpOperation.h"

void HttpOperation::Parse()
{
    size_t ret = url.find("https",0);
    if(ret != string::npos)
        protocol = 1;
    else
        protocol = 0;

    string::size_type i = 0;
    string::size_type start;
    for(i=0;i<url.length();++i)
    {
        if(url.at(i) == '/' && url.at(i+1) == '/')
            break;
    }

    start = i+2;
    for(int j=start;url.at(j)!='/';++j)
    {
        host.push_back(url.at(j));
    }

    

}