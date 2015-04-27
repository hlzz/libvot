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

}   // end of namespace tw

#endif  // IO_UTILS_HEADER
