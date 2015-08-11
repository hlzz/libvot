#ifndef IO_UTILS_HEADER
#define IO_UTILS_HEADER

#include <vector>

namespace tw 
{
    /**
     *  allocate a continuous 2d array
     */
    template<typename T> T* allocate_2d(T ***arr, int row, int col);

    /**
     *  free a continuous 2d array, both the raw pointer to the data
     *  and the 2d array pointers are needed.
     */
    template<typename T> void free_2d(T ***array, T *data);

    /**
     *  To safely read and write files
     */
    class IO
    {
    private:
        IO() {}
    public:
        template <class charType>
        static void TrimLeft(std::basic_string<charType> & str, const char *chars2remove = NULL);   //!< trim the left of a string
        template <class charType>
        static void TrimRight(std::basic_string<charType> & str, const char *chars2remove = NULL);  //!< trim the right of a string
        template <class charType>
        static void Trim(std::basic_string<charType> & str, const char *chars2remove = NULL);       // it consists TrimLeft and TrimRight
        static bool IsEmptyString(const std::string & str);                                     // judge whether the string is empty
        static int ExtractLines(const char *input_file_path, std::vector<std::string> & lines); //!< extract lines from a text file
        static bool IsFileExist(const char *filename);                          //!< judge whether the file exists
        static std::string JoinPath(std::string folder, std::string filename);  //!< join a filename with a directory name
        static bool Mkdir(const std::string path);      //!< make a directory
        static size_t GetAvailMem();                    //!< return the total availbale memory that can be used 
    };

}   // end of namespace tw

#endif  // IO_UTILS_HEADER
