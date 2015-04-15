#include <cstdlib>
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

}   // end of namespace tw
