#include <cstdlib>
#include <fstream>
#include <vector>
#include "io_utils.h"
#include "../image_graph/image_graph.h"

#if defined(__WIN32__) || defined(_MSC_VER)
#include <io.h>
#include <windows.h>
#include <sys/stat.h>
#if defined(_MSC_VER)
#include <direct.h>
#endif
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#if defined(__MACH__)
#include <unistd.h>
#endif
#endif

namespace tw
{
    template<typename T> T* allocate_2d(T ***array, int row, int col)
    {
        *array = (T**) malloc(row * sizeof(T*));
        T *data = (T*) malloc(row * col * sizeof(T));
        for(int i = 0; i < row; i++)
        {
            (*array)[i] = data + i * col;
        }
        return data;
    }

    template<typename T> void free_2d(T ***array, T *data)
    {
        free(data);
        free(*array);
    }

    template int* allocate_2d<int>(int ***array, int row, int col);
    template void free_2d<int>(int ***array, int *data);

    template float* allocate_2d<float>(float***array, int row, int col);
    template void free_2d<float>(float ***array, float *data);

    template double* allocate_2d<double>(double ***array, int row, int col);
    template void free_2d<double>(double ***array, double *data);

    template vot::LinkEdge* allocate_2d<vot::LinkEdge>(vot::LinkEdge ***array, int row, int col);
    template void free_2d<vot::LinkEdge>(vot::LinkEdge ***array, vot::LinkEdge *data);

    template <class charType>
    void IO::TrimLeft(std::basic_string<charType> & str, const char* chars2remove)
    {
        if (!str.empty())   //trim the characters in chars2remove from the left
        {
            std::string::size_type pos = 0;
            if (chars2remove != NULL)
            {
                pos = str.find_first_not_of(chars2remove);

                if (pos != std::string::npos)
                    str.erase(0,pos);
                else
                    str.erase( str.begin() , str.end() ); // make empty
            }
            else        //trim space
            {
                pos = std::string::npos;        //pos = -1
                for (size_t i = 0; i < str.size(); ++i)
                {
                    if (!isspace(str[i]))
                    {
                        pos = i;
                        break;
                    }
                }
                if (pos != std::string::npos)
                {
                    if (pos > 0)
                    {
                        size_t length = str.size() - pos;
                        for (size_t i = 0; i < length; ++i) str[i] = str[i+pos];
                        str.resize(length);
                    }
                }
                else
                {
                    str.clear();
                }
            }
        }
    }

    template <class charType>
    void IO::TrimRight(std::basic_string<charType> & str, const char* chars2remove)
    {
        if (!str.empty())   //trim the characters in chars2remove from the right
        {
            std::string::size_type pos = 0;
            if (chars2remove != NULL)
            {
                pos = str.find_last_not_of(chars2remove);

                if (pos != std::string::npos)
                    str.erase(pos+1);
                else
                    str.erase( str.begin() , str.end() ); // make empty
            }
            else        //trim space
            {
                pos = std::string::npos;
                for (int i = str.size()-1; i >= 0; --i)
                {
                    if (!isspace(str[i]))
                    {
                        pos = i;
                        break;
                    }
                }
                if (pos != std::string::npos)
                {
                    if (pos+1 != str.size())
                        str.resize(pos+1);
                }
                else
                {
                    str.clear();
                }
            }
        }
    }

    template <class charType>
    void IO::Trim(std::basic_string<charType> & str, const char* chars2remove)
    {
        TrimLeft(str, chars2remove);
        TrimRight(str, chars2remove);
    }

    bool IO::IsEmptyString(const std::string & str)
    {
        if (str.empty())        //this is true when the length of str is 0
        {
            return true;
        }
        for (size_t i = 0; i < str.length(); ++i)
        {
            if (!isspace(str[i]))
                return false;
        }
        return true;
    }

    int IO::ExtractLines(const char *input_file_path, std::vector<std::string> &lines)
    {
        int lineNum = -1;
        std::ifstream fin(input_file_path);
        if(!fin.is_open())
        {
            std::cerr << "Can't open the file " << input_file_path << "\n";
            return lineNum;
        }
        while(!fin.eof())
        {
            std::string line;
            getline(fin, line);
            Trim(line);
            if(!IsEmptyString(line))
            {
                lines.push_back(line);
                lineNum++;
            }
        }

        return lineNum;
    }

    bool IO::IsFileExist(const char *filename)      //POSIX
    {
        struct stat buffer;
        return (stat(filename, &buffer) == 0);
    }

    std::string IO::JoinPath(std::string folder, std::string filename)
    {
        #ifdef WIN32
        std::string sep = "\\";
        #else
        std::string sep = "/";
        #endif
        if(folder.length() > 0)
        {
            if(folder[folder.size()-1] != sep[0])
                return folder+sep+filename;
            else
                return folder+filename;
        }
        else
            return filename;
    }

    bool IO::Mkdir(const std::string path)
    {
        #ifdef _MSC_VER
        int mkDirRes = _mkdir(path.c_str());
        #elif __WIN32__
        int mkDirRes = mkdir(path.c_str());
        #else
        int mkDirRes = mkdir(path.c_str(), S_IRWXU | S_IRGRP | S_IROTH);
        #endif

        if (0 == mkDirRes || (mkDirRes !=0 && EEXIST == errno)  )
        {
            std::cout << "Create folder \"" << path<< "\"" << std::endl;
        }
        else
        {
            std::cout << "The folder may exist \"" << path<< "\"" << std::endl;
        };

        return mkDirRes == 0;
    }
}   // end of namespace tw
