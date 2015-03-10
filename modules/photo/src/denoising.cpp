/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective icvers.
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
//   * The name of Intel Corporation may not be used to endorse or promote products
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

#include "precomp.hpp"

#include "fast_nlmeans_denoising_invoker.hpp"
#include "fast_nlmeans_multi_denoising_invoker.hpp"
#include "fast_nlmeans_denoising_opencl.hpp"

void cv::fastNlMeansDenoising( InputArray _src, OutputArray _dst, float h,
                               int templateWindowSize, int searchWindowSize)
{
    fastNlMeansDenoising(_src, _dst, std::vector<float>(1, h),
                         templateWindowSize, searchWindowSize);
}

void cv::fastNlMeansDenoising( InputArray _src, OutputArray _dst, const std::vector<float>& h,
                               int templateWindowSize, int searchWindowSize)
{
    int hn = (int)h.size();
    CV_Assert(hn == 1 || hn == CV_MAT_CN(_src.type()));

    Size src_size = _src.size();
    CV_OCL_RUN(_src.dims() <= 2 && (_src.isUMat() || _dst.isUMat()) &&
               src_size.width > 5 && src_size.height > 5, // low accuracy on small sizes
               ocl_fastNlMeansDenoising(_src, _dst, &h[0], hn,
                                        templateWindowSize, searchWindowSize, false))

    Mat src = _src.getMat();
    _dst.create(src_size, src.type());
    Mat dst = _dst.getMat();

#ifdef HAVE_TEGRA_OPTIMIZATION
    if(hn == 1 && tegra::useTegra() &&
       tegra::fastNlMeansDenoising(src, dst, h[0], templateWindowSize, searchWindowSize))
        return;
#endif

    switch (src.type()) {
        case CV_8U:
            parallel_for_(cv::Range(0, src.rows),
                          FastNlMeansDenoisingInvoker<uchar, int, unsigned, DistSquared, int>(
                              src, dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_8UC2:
            if (hn == 1)
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec2b, int, unsigned, DistSquared, int>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec2b, int, unsigned, DistSquared, Vec2i>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_8UC3:
            if (hn == 1)
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec3b, int, unsigned, DistSquared, int>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec3b, int, unsigned, DistSquared, Vec3i>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_8UC4:
            if (hn == 1)
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec4b, int, unsigned, DistSquared, int>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec4b, int, unsigned, DistSquared, Vec4i>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        default:
            CV_Error(Error::StsBadArg,
                "Unsupported image format! Only CV_8U, CV_8UC2, CV_8UC3 and CV_8UC4 are supported");
    }
}

void cv::fastNlMeansDenoisingAbs( InputArray _src, OutputArray _dst, float h,
                                  int  templateWindowSize, int searchWindowSize)
{
    fastNlMeansDenoisingAbs(_src, _dst, std::vector<float>(1, h),
                            templateWindowSize, searchWindowSize);
}

void cv::fastNlMeansDenoisingAbs( InputArray _src, OutputArray _dst, const std::vector<float>& h,
                                  int  templateWindowSize, int searchWindowSize)
{
    int hn = (int)h.size();
    CV_Assert(hn == 1 || hn == CV_MAT_CN(_src.type()));

    Size src_size = _src.size();
    CV_OCL_RUN(_src.dims() <= 2 && (_src.isUMat() || _dst.isUMat()) &&
               src_size.width > 5 && src_size.height > 5, // low accuracy on small sizes
               ocl_fastNlMeansDenoising(_src, _dst, &h[0], hn,
                                        templateWindowSize, searchWindowSize, true))

    Mat src = _src.getMat();
    _dst.create(src_size, src.type());
    Mat dst = _dst.getMat();

    switch (src.type()) {
        case CV_8U:
            parallel_for_(cv::Range(0, src.rows),
                          FastNlMeansDenoisingInvoker<uchar, int, unsigned, DistAbs, int>(
                              src, dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_8UC2:
            if (hn == 1)
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec2b, int, unsigned, DistAbs, int>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec2b, int, unsigned, DistAbs, Vec2i>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_8UC3:
            if (hn == 1)
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec3b, int, unsigned, DistAbs, int>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec3b, int, unsigned, DistAbs, Vec3i>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_8UC4:
            if (hn == 1)
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec4b, int, unsigned, DistAbs, int>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec4b, int, unsigned, DistAbs, Vec4i>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_16U:
            parallel_for_(cv::Range(0, src.rows),
                    FastNlMeansDenoisingInvoker<ushort, int64, uint64, DistAbs, int>(
                    src, dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_16UC2:
            if (hn == 1)
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec<ushort, 2>, int64, uint64, DistAbs, int>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec<ushort, 2>, int64, uint64, DistAbs, Vec2i>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_16UC3:
            if (hn == 1)
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec<ushort, 3>, int64, uint64, DistAbs, int>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec<ushort, 3>, int64, uint64, DistAbs, Vec3i>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_16UC4:
            if (hn == 1)
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec<ushort, 4>, int64, uint64, DistAbs, int>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, src.rows),
                              FastNlMeansDenoisingInvoker<Vec<ushort, 4>, int64, uint64, DistAbs, Vec4i>(
                                  src, dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        default:
            CV_Error(Error::StsBadArg,
                "Unsupported image format! Only CV_8U, CV_8UC2, CV_8UC3, CV_8UC4, CV_16U, CV_16UC2, CV_16UC3 and CV_16UC4 are supported");
    }
}

void cv::fastNlMeansDenoisingColored( InputArray _src, OutputArray _dst,
                                      float h, float hForColorComponents,
                                      int templateWindowSize, int searchWindowSize)
{
    int type = _src.type(), depth = CV_MAT_DEPTH(type), cn = CV_MAT_CN(type);
    Size src_size = _src.size();
    if (type != CV_8UC3 && type != CV_8UC4)
    {
        CV_Error(Error::StsBadArg, "Type of input image should be CV_8UC3 or CV_8UC4!");
        return;
    }

    CV_OCL_RUN(_src.dims() <= 2 && (_dst.isUMat() || _src.isUMat()) &&
                src_size.width > 5 && src_size.height > 5, // low accuracy on small sizes
                ocl_fastNlMeansDenoisingColored(_src, _dst, h, hForColorComponents,
                                                templateWindowSize, searchWindowSize))

    Mat src = _src.getMat();
    _dst.create(src_size, type);
    Mat dst = _dst.getMat();

    Mat src_lab;
    cvtColor(src, src_lab, COLOR_LBGR2Lab);

    Mat l(src_size, CV_MAKE_TYPE(depth, 1));
    Mat ab(src_size, CV_MAKE_TYPE(depth, 2));
    Mat l_ab[] = { l, ab };
    int from_to[] = { 0,0, 1,1, 2,2 };
    mixChannels(&src_lab, 1, l_ab, 2, from_to, 3);

    fastNlMeansDenoising(l, l, h, templateWindowSize, searchWindowSize);
    fastNlMeansDenoising(ab, ab, hForColorComponents, templateWindowSize, searchWindowSize);

    Mat l_ab_denoised[] = { l, ab };
    Mat dst_lab(src_size, CV_MAKE_TYPE(depth, 3));
    mixChannels(l_ab_denoised, 2, &dst_lab, 1, from_to, 3);

    cvtColor(dst_lab, dst, COLOR_Lab2LBGR, cn);
}

static void fastNlMeansDenoisingMultiCheckPreconditions(
                               const std::vector<Mat>& srcImgs,
                               int imgToDenoiseIndex, int temporalWindowSize,
                               int templateWindowSize, int searchWindowSize)
{
    int src_imgs_size = static_cast<int>(srcImgs.size());
    if (src_imgs_size == 0)
    {
        CV_Error(Error::StsBadArg, "Input images vector should not be empty!");
    }

    if (temporalWindowSize % 2 == 0 ||
        searchWindowSize % 2 == 0 ||
        templateWindowSize % 2 == 0) {
        CV_Error(Error::StsBadArg, "All windows sizes should be odd!");
    }

    int temporalWindowHalfSize = temporalWindowSize / 2;
    if (imgToDenoiseIndex - temporalWindowHalfSize < 0 ||
        imgToDenoiseIndex + temporalWindowHalfSize >= src_imgs_size)
    {
        CV_Error(Error::StsBadArg,
            "imgToDenoiseIndex and temporalWindowSize "
            "should be chosen corresponding srcImgs size!");
    }

    for (int i = 1; i < src_imgs_size; i++)
        if (srcImgs[0].size() != srcImgs[i].size() || srcImgs[0].type() != srcImgs[i].type())
        {
            CV_Error(Error::StsBadArg, "Input images should have the same size and type!");
        }
}

void cv::fastNlMeansDenoisingMulti( InputArrayOfArrays _srcImgs, OutputArray _dst,
                                    int imgToDenoiseIndex, int temporalWindowSize,
                                    float h, int templateWindowSize, int searchWindowSize)
{
    fastNlMeansDenoisingMulti(_srcImgs, _dst, imgToDenoiseIndex, temporalWindowSize,
                              std::vector<float>(1, h), templateWindowSize, searchWindowSize);
}

void cv::fastNlMeansDenoisingMulti( InputArrayOfArrays _srcImgs, OutputArray _dst,
                                    int imgToDenoiseIndex, int temporalWindowSize,
                                    const std::vector<float>& h,
                                    int templateWindowSize, int searchWindowSize)
{
    std::vector<Mat> srcImgs;
    _srcImgs.getMatVector(srcImgs);

    fastNlMeansDenoisingMultiCheckPreconditions(
        srcImgs, imgToDenoiseIndex,
        temporalWindowSize, templateWindowSize, searchWindowSize);

    int hn = (int)h.size();
    CV_Assert(hn == 1 || hn == CV_MAT_CN(srcImgs[0].type()));

    _dst.create(srcImgs[0].size(), srcImgs[0].type());
    Mat dst = _dst.getMat();

    switch (srcImgs[0].type())
    {
        case CV_8U:
            parallel_for_(cv::Range(0, srcImgs[0].rows),
                          FastNlMeansMultiDenoisingInvoker<uchar, int, unsigned, DistSquared, int>(
                              srcImgs, imgToDenoiseIndex, temporalWindowSize,
                              dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_8UC2:
            if (hn == 1)
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec2b, int, unsigned, DistSquared, int>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec2b, int, unsigned, DistSquared, Vec2i>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_8UC3:
            if (hn == 1)
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec3b, int, unsigned, DistSquared, int>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec3b, int, unsigned, DistSquared, Vec3i>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_8UC4:
            if (hn == 1)
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec4b, int, unsigned, DistSquared, int>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec4b, int, unsigned, DistSquared, Vec4i>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        default:
            CV_Error(Error::StsBadArg,
                "Unsupported image format! Only CV_8U, CV_8UC2, CV_8UC3 and CV_8UC4 are supported");
    }
}

void cv::fastNlMeansDenoisingMultiAbs( InputArrayOfArrays _srcImgs, OutputArray _dst,
                                       int imgToDenoiseIndex, int temporalWindowSize,
                                       float h, int templateWindowSize, int searchWindowSize)
{
    fastNlMeansDenoisingMulti(_srcImgs, _dst, imgToDenoiseIndex, temporalWindowSize,
                              std::vector<float>(1, h), templateWindowSize, searchWindowSize);
}

void cv::fastNlMeansDenoisingMultiAbs( InputArrayOfArrays _srcImgs, OutputArray _dst,
                                       int imgToDenoiseIndex, int temporalWindowSize,
                                       const std::vector<float>& h,
                                       int templateWindowSize, int searchWindowSize)
{
    std::vector<Mat> srcImgs;
    _srcImgs.getMatVector(srcImgs);

    fastNlMeansDenoisingMultiCheckPreconditions(
        srcImgs, imgToDenoiseIndex,
        temporalWindowSize, templateWindowSize, searchWindowSize);

    int hn = (int)h.size();
    CV_Assert(hn == 1 || hn == CV_MAT_CN(srcImgs[0].type()));

    _dst.create(srcImgs[0].size(), srcImgs[0].type());
    Mat dst = _dst.getMat();

    switch (srcImgs[0].type())
    {
        case CV_8U:
            parallel_for_(cv::Range(0, srcImgs[0].rows),
                          FastNlMeansMultiDenoisingInvoker<uchar, int, unsigned, DistAbs, int>(
                              srcImgs, imgToDenoiseIndex, temporalWindowSize,
                              dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_8UC2:
            if (hn == 1)
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec2b, int, unsigned, DistAbs, int>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec2b, int, unsigned, DistAbs, Vec2i>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_8UC3:
            if (hn == 1)
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec3b, int, unsigned, DistAbs, int>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec3b, int, unsigned, DistAbs, Vec3i>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_8UC4:
            if (hn == 1)
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec4b, int, unsigned, DistAbs, int>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec4b, int, unsigned, DistAbs, Vec4i>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_16U:
            parallel_for_(cv::Range(0, srcImgs[0].rows),
                FastNlMeansMultiDenoisingInvoker<ushort, int64, uint64, DistAbs, int>(
                    srcImgs, imgToDenoiseIndex, temporalWindowSize,
                    dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_16UC2:
            if (hn == 1)
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec<ushort, 2>, int64, uint64, DistAbs, int>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec<ushort, 2>, int64, uint64, DistAbs, Vec2i>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_16UC3:
            if (hn == 1)
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec<ushort, 3>, int64, uint64, DistAbs, int>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec<ushort, 3>, int64, uint64, DistAbs, Vec3i>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        case CV_16UC4:
            if (hn == 1)
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec<ushort, 4>, int64, uint64, DistAbs, int>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            else
                parallel_for_(cv::Range(0, srcImgs[0].rows),
                              FastNlMeansMultiDenoisingInvoker<Vec<ushort, 4>, int64, uint64, DistAbs, Vec4i>(
                                  srcImgs, imgToDenoiseIndex, temporalWindowSize,
                                  dst, templateWindowSize, searchWindowSize, &h[0]));
            break;
        default:
            CV_Error(Error::StsBadArg,
                "Unsupported image format! Only CV_8U, CV_8UC2, CV_8UC3, CV_8UC4, CV_16U, CV_16UC2, CV_16UC3 and CV_16UC4 are supported");
    }
}

void cv::fastNlMeansDenoisingColoredMulti( InputArrayOfArrays _srcImgs, OutputArray _dst,
                                           int imgToDenoiseIndex, int temporalWindowSize,
                                           float h, float hForColorComponents,
                                           int templateWindowSize, int searchWindowSize)
{
    std::vector<Mat> srcImgs;
    _srcImgs.getMatVector(srcImgs);

    fastNlMeansDenoisingMultiCheckPreconditions(
        srcImgs, imgToDenoiseIndex,
        temporalWindowSize, templateWindowSize, searchWindowSize);

    _dst.create(srcImgs[0].size(), srcImgs[0].type());
    Mat dst = _dst.getMat();

    int type = srcImgs[0].type(), depth = CV_MAT_DEPTH(type);
    int src_imgs_size = static_cast<int>(srcImgs.size());

    if (type != CV_8UC3)
    {
        CV_Error(Error::StsBadArg, "Type of input images should be CV_8UC3!");
        return;
    }

    int from_to[] = { 0,0, 1,1, 2,2 };

    // TODO convert only required images
    std::vector<Mat> src_lab(src_imgs_size);
    std::vector<Mat> l(src_imgs_size);
    std::vector<Mat> ab(src_imgs_size);
    for (int i = 0; i < src_imgs_size; i++)
    {
        src_lab[i] = Mat::zeros(srcImgs[0].size(), type);
        l[i] = Mat::zeros(srcImgs[0].size(), CV_MAKE_TYPE(depth, 1));
        ab[i] = Mat::zeros(srcImgs[0].size(), CV_MAKE_TYPE(depth, 2));
        cvtColor(srcImgs[i], src_lab[i], COLOR_LBGR2Lab);

        Mat l_ab[] = { l[i], ab[i] };
        mixChannels(&src_lab[i], 1, l_ab, 2, from_to, 3);
    }

    Mat dst_l;
    Mat dst_ab;

    fastNlMeansDenoisingMulti(
        l, dst_l, imgToDenoiseIndex, temporalWindowSize,
        h, templateWindowSize, searchWindowSize);

    fastNlMeansDenoisingMulti(
        ab, dst_ab, imgToDenoiseIndex, temporalWindowSize,
        hForColorComponents, templateWindowSize, searchWindowSize);

    Mat l_ab_denoised[] = { dst_l, dst_ab };
    Mat dst_lab(srcImgs[0].size(), srcImgs[0].type());
    mixChannels(l_ab_denoised, 2, &dst_lab, 1, from_to, 3);

    cvtColor(dst_lab, dst, COLOR_Lab2LBGR);
}
