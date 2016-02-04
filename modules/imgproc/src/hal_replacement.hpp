#ifndef OPENCV_IMGPROC_HAL_REPLACEMENT_HPP
#define OPENCV_IMGPROC_HAL_REPLACEMENT_HPP

#include "opencv2/core/hal/interface.h"

inline int hal_ni_filterInit(void **, uchar *, size_t, int, int, int, int, int, int, int, int, double, int, int, bool, bool) { return CV_HAL_ERROR_NOT_IMPLEMENTED; }
inline int hal_ni_filter(void *, uchar *, size_t, uchar *, size_t, int, int, int, int, int, int) { return CV_HAL_ERROR_NOT_IMPLEMENTED; }
inline int hal_ni_filterFree(void *) { return CV_HAL_ERROR_NOT_IMPLEMENTED; }

#define cv_hal_filterInit hal_ni_filterInit
#define cv_hal_filter hal_ni_filter
#define cv_hal_filterFree hal_ni_filterFree

inline int hal_ni_sepFilterInit(void **, int, int, int, uchar *, size_t, int, int, uchar *, size_t, int, int, int, int, double, int) { return CV_HAL_ERROR_NOT_IMPLEMENTED; }
inline int hal_ni_sepFilter(void *, uchar *, size_t, uchar*, size_t, int, int, int, int, int, int) { return CV_HAL_ERROR_NOT_IMPLEMENTED; }
inline int hal_ni_sepFilterFree(void *) { return CV_HAL_ERROR_NOT_IMPLEMENTED; }

#define cv_hal_sepFilterInit hal_ni_sepFilterInit
#define cv_hal_sepFilter hal_ni_sepFilter
#define cv_hal_sepFilterFree hal_ni_sepFilterFree

#include "custom_hal.hpp"

#endif // OPENCV_IMGPROC_HAL_REPLACEMENT_HPP
