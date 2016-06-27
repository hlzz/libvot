#include <boost/python.hpp>

char const* hello()
{
    return "Hello world from pyvot!\n";
}

// python-c++ internal module is _libvot, will be further wrapped in python-style
BOOST_PYTHON_MODULE(_libvot)
{
    using namespace boost::python; 
    def("hello", hello);
}
