#include <cstdlib>
#include <sys/stat.h>
#include <fstream>
#include <vector>
#include "io_utils.h"
#include "image_graph/image_graph.h"

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

    template vot::LinkNode* allocate_2d<vot::LinkNode>(vot::LinkNode ***array, int row, int col);
    template void free_2d<vot::LinkNode>(vot::LinkNode ***array, vot::LinkNode *data);

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
}   // end of namespace tw
