/*
Copyright (c) 2015, Tianwei Shen
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of libvot nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

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
