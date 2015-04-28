#ifndef IO_UTILS_HEADER
#define IO_UTILS_HEADER

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
        static void TrimLeft(std::basic_string<charType> & str, const char *chars2remove = NULL);

        template <class charType>
        static void TrimRight(std::basic_string<charType> & str, const char *chars2remove = NULL);

        template <class charType>
        static void Trim(std::basic_string<charType> & str, const char *chars2remove = NULL);

        static bool IsEmptyString(const std::string & str);

        static int ExtractLines(const char *input_file_path, std::vector<std::string> & lines);

        static bool IsFileExist(const char *filename);
    };

}   // end of namespace tw

#endif  // IO_UTILS_HEADER
