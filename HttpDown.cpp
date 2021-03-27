#include "HttpDown.h"

HttpDown::HttpDown(string _url,string _name,string _path) :
    filename(_name),filepath(_path),hp(_url),
    threadpool(make_shared<ThreadPool>(16))
    {}

HttpDown::HttpDown(string _url) :
    HttpDown(_url,".","1")
    {}

long HttpDown::Position(int k)
{
    //获取文件大小
    struct stat info;
    stat(filename.c_str(),&info);
    long size = info.st_size;

    fstream in(filename,ios::in);
    if(!in.is_open() || size < filesize)
    {
        //创建文件
        in.open(filename,ios::out);
        in.close();
        return 0;
    }

    //判断文件是否存在且已下载完
    if(size == filesize)
    {
        for(int i=0;i<k;++i)
        {
            *(current+i) = 1;
        }
        return 0;
    }

    //检查文件末尾校验和
    char ch;
    in.seekg(filesize+1,ios::beg);

    int written = 0;
    while(in.read(&ch,1))
    {
        *(current+written) = ch;
        written++;
    }

    in.close();
    return written;
}

void ThreadDownLoad(long start,long end,HttpOperation ho,HttpDown *hd)
{
    long endsize = 0;
    if(end > hd->filesize)
    {
        endsize = end-start;
        end = hd->filesize;
    }
    //需要下载的字节数
    long size = end-start;

    ho.Connect();
    ho.SendHttpHead(start,end);
    ho.ReadHttpHead();

    char buf[size];　//缓冲区
    long s = 0;

    while(s < size)
    {
        s += ho.HttpRead(buf+s,size-s);
    }

    ho.Close();

    fstream out(hd->filename);
    //将缓冲区数据写入文件
    out.seekp(start,ios::beg);
    out.write(buf,size);
    //计算校验码写入位置
    long check = 0;

    if(endsize > 0)
        size = endsize;

    check = (hd->filesize)+(start+1)/size+1;
    //写入校验码
    char ch = 1;
    out.seekp(check,ios::beg);
    out.write(&ch,1);

    //更新已下载大小
    *(hd->current+(start+1)/size) = 1;

    out.close();
}

int HttpDown::DownLoad()
{
    //获取文件信息数据
    hp.Parse();
    hp.GetIpAddr();
    hp.Connect();
    hp.SendHttpHead(0,-1);
    hp.ReadHttpHead();
    hp.ParseHead();
    hp.Close();

    //显示文件信息数据
    cout << "文件 : " + hp.filename << endl;
    cout << "来自 : " + hp.host < endl;
    cout << "大小 : " + ((hp.size>1024)?(to_string(hp.size/1024)+"k"):(to_string(hp.size))) << endl;
    cout << "类型 : " + hp.type << endl;
    cout << "路径 : " + filepath << endl;

    filename += "." + hp.type;
    hp.filename = filename;
    filename = filepath + filename;

    filesize = hp.size;
    int content = 4096;

    long part = ((filesize + content-1)/content);

    current = new char[part];
    bzero(current,part);

    //获取文件信息
    Position(part);

    if(part > 100)
    {
        //根据文件大小初始化线程池
        threadpool = make_shared<ThreadPool>)(part/3);
    }

    //开始多线程下载文件，每个线程下载content字节
    for(long i=0;i<part;++i)
    {
        if(*(current+i) == i)
            continue;

        if(part > 100)
        {
            threadpool->Append(ThreadDownLoad,i*content,(i+1)*content,hp,this);
        }
        else
        {
            thread t(ThreadDownLoad,i*content,(i+1)*content,hp,this);
            t.detach();
        }
    }

    while(true)
    {
        int f = 0;
        int s = 1;

        for(int i=0;i<part;++i)
        {
            if(*(current+i) == 0)
            {
                s = 0;
                f++;
            }
        }
        Print(part-f,part);
        
        if(s)
            break;
    }

    //下载完毕去除文件校验码
    truncate(hp.filename.c_str(),filesize);

    return 1;
}

void HttpDown::Print(int current,int final)
{
    current = 100*((double)Current/final);
    final = 100;

    print("[");
    for(int i=0;i<current;++i)
        printf("#");
    for(int i=0;i<final-current;++i)
        printf("-");
    printf("]");

    fflush(stdout);
    sleep(1);

    for(int i=0;i<final+2;++i)
        printf("\b");
    
}

HttpDown::~HttpDown()
{
    delete current;
}