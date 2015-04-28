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
        dp_ = NULL; lp_ = NULL;
    }

    // copy constructor
    SiftData(const SiftData & obj):
        name_(obj.name_), version_(obj.version_), npoint_(obj.npoint_),
        nLocDim_(obj.nLocDim_), nDesDim_(obj.nDesDim_)
    {
        //copy feature descriptors
        DTYPE *dp_origin = dp_;
        dp_ = new DTYPE [obj.npoint_];
        memcpy(dp_, obj.dp_, sizeof(DTYPE)*obj.npoint_);
        delete [] dp_origin;

        //copy location descriptors
        LTYPE *lp_origin = lp_;
        lp_ = new LTYPE [obj.npoint_];
        memcpy(lp_, obj.lp_, sizeof(LTYPE)*obj.npoint_);
        delete [] lp_origin;
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
        DTYPE *dp_origin = dp_;
        dp_ = new DTYPE [rhs.npoint_];
        memcpy(dp_, rhs.dp_, sizeof(DTYPE)*rhs.npoint_);
        delete [] dp_origin;

        //copy location descriptors
        LTYPE *lp_origin = lp_;
        lp_ = new LTYPE [rhs.npoint_];
        memcpy(lp_, rhs.lp_, sizeof(LTYPE)*rhs.npoint_);
        delete [] lp_origin;

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
            std::cout << "Can't read sift file " << szFileName << '\n';
            return false;
        }
        fread(&name_, sizeof(int), 1, fd);
        fread(&version_, sizeof(int), 1, fd);

        if(name_ == ('S'+ ('I'<<8)+('F'<<16)+('T'<<24)))
        {
            fread(&npoint_, sizeof(int), 1, fd);
            fread(&nLocDim_, sizeof(int), 1, fd);
            fread(&nDesDim_, sizeof(int), 1, fd);

            lp_ = new LTYPE [npoint_ * nLocDim_];          //restoring location data
            dp_ = new DTYPE [npoint_ * nDesDim_];          //restoring descriptor data
            if(npoint_ >= 0 && nLocDim_ > 0 && nDesDim_ == 128)
            {
                fread(lp_, sizeof(LTYPE), nLocDim_ * npoint_, fd);
                fread(dp_, sizeof(DTYPE), nDesDim_ * npoint_, fd);
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