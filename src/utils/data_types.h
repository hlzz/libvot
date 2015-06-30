#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>

#include "global_params.h"

namespace tw
{

class SiftData
{
private:
    int name_;
    int version_;
    int npoint_;
    int nLocDim_;
    int nDesDim_;
    DTYPE *dp_;
    LTYPE *lp_;

public:
    SiftData()
    {
        name_ = 'S' + ('I' << 8) + ('F' << 16) + ('T' << 24);
        version_ = 'V' + ('5' << 8) + ('.' << 16) + ('0' << 24);
        npoint_ = 0;
        nLocDim_ = 5;
        nDesDim_ = 128;
        dp_ = NULL; lp_ = NULL;
    }

    // copy constructor
    SiftData(const SiftData & obj):
        name_(obj.name_), version_(obj.version_), npoint_(obj.npoint_),
        nLocDim_(obj.nLocDim_), nDesDim_(obj.nDesDim_)
    {
        //copy feature descriptors
        dp_ = new DTYPE [obj.npoint_];
        memcpy(dp_, obj.dp_, sizeof(DTYPE)*obj.npoint_);

        //copy location descriptors
        lp_ = new LTYPE [obj.npoint_];
        memcpy(lp_, obj.lp_, sizeof(LTYPE)*obj.npoint_);
    }

    // copy assignment operator
    SiftData & operator=(const SiftData &rhs)
    {
        name_ = rhs.name_;
        version_ = rhs.version_;
        npoint_ = rhs.npoint_;
        nLocDim_ = rhs.nLocDim_;
        nDesDim_ = rhs.nDesDim_;

        //copy feature descriptors
        dp_ = new DTYPE [rhs.npoint_];
        memcpy(dp_, rhs.dp_, sizeof(DTYPE)*rhs.npoint_);

        //copy location descriptors
        lp_ = new LTYPE [rhs.npoint_];
        memcpy(lp_, rhs.lp_, sizeof(LTYPE)*rhs.npoint_);

        return *this;
    }

    // destructor
    ~SiftData()
    {
        if (NULL != dp_) delete [] dp_;
        if (NULL != lp_) delete [] lp_;
    }

    ///
    /// \brief ReadSiftFile: read sift file, this function is compatible with the sfm version of sift file
    /// \param szFileName: the name of the sift file
    /// \return
    ///
    bool ReadSiftFile(std::string const &szFileName)
    {
        name_ = 'S' + ('I' << 8) + ('F' << 16) + ('T' << 24);
        version_ = 'V' + ('5' << 8) + ('.' << 16) + ('0' << 24);
        FILE *fd;
        if((fd = fopen(szFileName.c_str(), "rb")) == NULL)
        {
            std::cout << "[ReadSiftFile] Can't read sift file " << szFileName << '\n';
            return false;
        }
        int f = fread(&name_, sizeof(int), 1, fd);
        int g = fread(&version_, sizeof(int), 1, fd);
        if(f != 1 || g != 1)
        {
        	std::cout << "[ReadSiftFile] Reading error\n";
        	return false;
        }

        if(name_ == ('S'+ ('I'<<8)+('F'<<16)+('T'<<24)))
        {
        	int a, b, c;
            a = fread(&npoint_, sizeof(int), 1, fd);
            b = fread(&nLocDim_, sizeof(int), 1, fd);
            c = fread(&nDesDim_, sizeof(int), 1, fd);
            if(a != 1 || b != 1 || c!= 1)
            {
            	std::cout << "[ReadSiftFile] Reading error\n";
            	return false;
            }

            lp_ = new LTYPE [npoint_ * nLocDim_];          //restoring location data
            dp_ = new DTYPE [npoint_ * nDesDim_];          //restoring descriptor data
            if(npoint_ >= 0 && nLocDim_ > 0 && nDesDim_ == 128)
            {
            	int d, e;
                d = fread(lp_, sizeof(LTYPE), nLocDim_ * npoint_, fd);
                e = fread(dp_, sizeof(DTYPE), nDesDim_ * npoint_, fd);
                if(d != nLocDim_ * npoint_ || e != nDesDim_ * npoint_)
                {
	            	std::cout << "[ReadSiftFile] Reading error\n";
	            	return false;
                }
                fclose(fd);
            }
            else
            {
                fclose(fd);
                return false;
            }

        }
        else
        {
            fclose(fd);
            return false;
        }
        return true;
    }

    int getFeatureNum() {return npoint_;}
    int getLocDim() {return nLocDim_;}
    int getDesDim() {return nDesDim_;}
    LTYPE * getLocPointer() {return lp_;}
    DTYPE * getDesPointer() {return dp_;}
};

}   // end of namespace tw

#endif