/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                          License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009-2010, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef DEBUGMAT_H
#define DEBUGMAT_H

#include <opencv2/opencv.hpp>
#include <QDebug>

namespace cv_for_qt
{



static inline char getCloseBrace(char c)
{
    return c == '[' ? ']' : c == '(' ? ')' : c == '{' ? '}' : '\0';
}


template<typename _Tp> static void writeElems(QDebug& out, const _Tp* data,
                                              int nelems, int cn, char obrace, char cbrace)
{
    typedef typename DataType<_Tp>::work_type _WTp;
    nelems *= cn;
    for(int i = 0; i < nelems; i += cn)
    {
        if(cn == 1)
        {
            out << (_WTp)data[i] << (i+1 < nelems ? ", " : "");
            continue;
        }
        out << obrace;
        for(int j = 0; j < cn; j++)
            out << (_WTp)data[i + j] << (j+1 < cn ? ", " : "");
        out << cbrace << (i+cn < nelems ? ", " : "");
    }
}


static void writeElems(QDebug& out, const void* data, int nelems, int type, char brace)
{
    int depth = CV_MAT_DEPTH(type), cn = CV_MAT_CN(type);
    char cbrace = ' ';
    if(!brace || isspace(brace))
    {
        nelems *= cn;
        cn = 1;
    }
    else
        cbrace = getCloseBrace(brace);
    if(depth == CV_8U)
        writeElems(out, (const uchar*)data, nelems, cn, brace, cbrace);
    else if(depth == CV_8S)
        writeElems(out, (const schar*)data, nelems, cn, brace, cbrace);
    else if(depth == CV_16U)
        writeElems(out, (const ushort*)data, nelems, cn, brace, cbrace);
    else if(depth == CV_16S)
        writeElems(out, (const short*)data, nelems, cn, brace, cbrace);
    else if(depth == CV_32S)
        writeElems(out, (const int*)data, nelems, cn, brace, cbrace);
    else if(depth == CV_32F)
    {
        //std::streamsize pp = out.precision();
        //out.precision(8);
        writeElems(out, (const float*)data, nelems, cn, brace, cbrace);
        //out.precision(pp);
    }
    else if(depth == CV_64F)
    {
        //std::streamsize pp = out.precision();
        //out.precision(16);
        writeElems(out, (const double*)data, nelems, cn, brace, cbrace);
        //out.precision(pp);
    }
    else
        CV_Error(CV_StsUnsupportedFormat, "");
}


static void writeMat(QDebug& out, const Mat& m, char rowsep, char elembrace, bool singleLine)
{
    CV_Assert(m.dims <= 2);
    int type = m.type();

    char crowbrace = getCloseBrace(rowsep);
    char orowbrace = crowbrace ? rowsep : '\0';

    if( orowbrace || isspace(rowsep) )
        rowsep = '\0';

    for( int i = 0; i < m.rows; i++ )
    {
        if(orowbrace)
            out << orowbrace;
        if( m.data )
            writeElems(out, m.ptr(i), m.cols, type, elembrace);
        if(orowbrace)
            out << crowbrace << (i+1 < m.rows ? ", " : "");
        if(i+1 < m.rows)
        {
            if(rowsep)
                out << rowsep << (singleLine ? " " : "");
            if(!singleLine)
                out << "\n  ";
        }
    }
}

static void writeMatToQDebug(const Mat& m) {
    QDebug qd = qDebug();

    qd << "[";
    writeMat(qd, m, ';', ' ', m.cols == 1);
    qd << "]";
}

}

#endif // DEBUGMAT_H
