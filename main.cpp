#include "HttpDown.h"
#include <ctime>
#include <time.h>
#include <cstring>

int main(int argc,char *argv[])
{
    clock_t start = clock();

    string path;
    string name;

    for(int i = 2;i<argc;++i)
    {
        if(!strcmp(argv[i],"-o"))
            name = argv[i+1];
        if(!strcmp(argv[i],"-p"))
            path = argv[i+1];
    }

    cout << name << " " << path << endl;

    shared_ptr<HttpDown> hd;
    if(path.size() && name.size())
        hd = make_shared<HttpDown>(argv[1],name,path);
    else
        hd = make_shared<HttpDown>(argv[1]);

    clock_t end = clock();

    cout << endl << "耗时: " << (double)(end-start)/CLOCKS_PER_SEC*100 << "秒" << endl;
    
    return 0;
}