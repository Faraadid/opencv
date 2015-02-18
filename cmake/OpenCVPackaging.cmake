# Use patched version of CPACK to build accurate set of Debian packages
# https://github.com/asmorkalov/CMake/tree/deb_generator_improvement

if(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
set(CPACK_set_DESTDIR "on")

if(NOT OPENCV_CUSTOM_PACKAGE_INFO)
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Open Computer Vision Library")
  set(CPACK_PACKAGE_DESCRIPTION
"OpenCV (Open Source Computer Vision Library) is an open source computer vision
and machine learning software library. OpenCV was built to provide a common
infrastructure for computer vision applications and to accelerate the use of
machine perception in the commercial products. Being a BSD-licensed product,
OpenCV makes it easy for businesses to utilize and modify the code.")
  set(CPACK_PACKAGE_VENDOR "OpenCV Foundation")
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
  set(CPACK_PACKAGE_CONTACT "OpenCV Developers <admin@opencv.org>")
  set(CPACK_PACKAGE_VERSION_MAJOR "${OPENCV_VERSION_MAJOR}")
  set(CPACK_PACKAGE_VERSION_MINOR "${OPENCV_VERSION_MINOR}")
  set(CPACK_PACKAGE_VERSION_PATCH "${OPENCV_VERSION_PATCH}")
  set(CPACK_PACKAGE_VERSION "${OPENCV_VCSVERSION}")
endif(NOT OPENCV_CUSTOM_PACKAGE_INFO)

set(CPACK_STRIP_FILES 1)

#arch
if(X86)
  set(CPACK_DEBIAN_ARCHITECTURE "i386")
  set(CPACK_RPM_PACKAGE_ARCHITECTURE "i686")
elseif(X86_64)
  set(CPACK_DEBIAN_ARCHITECTURE "amd64")
  set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
elseif(ARM)
  set(CPACK_DEBIAN_ARCHITECTURE "armhf")
  set(CPACK_RPM_PACKAGE_ARCHITECTURE "armhf")
else()
  set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
  set(CPACK_RPM_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
endif()

if(CPACK_GENERATOR STREQUAL "DEB")
  set(OPENCV_PACKAGE_ARCH_SUFFIX ${CPACK_DEBIAN_ARCHITECTURE})
elseif(CPACK_GENERATOR STREQUAL "RPM")
  set(OPENCV_PACKAGE_ARCH_SUFFIX ${CPACK_RPM_PACKAGE_ARCHITECTURE})
else()
  set(OPENCV_PACKAGE_ARCH_SUFFIX ${CMAKE_SYSTEM_PROCESSOR})
endif()

set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${OPENCV_VCSVERSION}-${OPENCV_PACKAGE_ARCH_SUFFIX}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${OPENCV_VCSVERSION}-${OPENCV_PACKAGE_ARCH_SUFFIX}")

#rpm options
set(CPACK_RPM_COMPONENT_INSTALL TRUE)
set(CPACK_RPM_PACKAGE_SUMMARY ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_RPM_PACKAGE_URL "http://opencv.org")
set(CPACK_RPM_PACKAGE_LICENSE "BSD")

#deb options
set(CPACK_DEB_COMPONENT_INSTALL TRUE)
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SECTION "libs")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://opencv.org")

#depencencies
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS TRUE)
set(CPACK_COMPONENT_samples_DEPENDS libs dev)
set(CPACK_COMPONENT_dev_DEPENDS libs)
set(CPACK_COMPONENT_docs_DEPENDS libs)
set(CPACK_COMPONENT_java_DEPENDS libs)
set(CPACK_COMPONENT_python_DEPENDS libs)
set(CPACK_DEB_python_PACKAGE_DEPENDS "python-numpy (>=${PYTHON_NUMPY_VERSION}), python${PYTHON_VERSION_MAJOR_MINOR}")
set(CPACK_COMPONENT_tests_DEPENDS libs)
if (HAVE_opencv_python)
  set(CPACK_DEB_tests_PACKAGE_DEPENDS "python-numpy (>=${PYTHON_NUMPY_VERSION}), python${PYTHON_VERSION_MAJOR_MINOR}, python-py | python-pytest")
endif()

if(HAVE_CUDA)
  string(REPLACE "." "-" cuda_version_suffix ${CUDA_VERSION})
  if(CUDA_VERSION VERSION_LESS "6.5")
    set(CPACK_DEB_libs_PACKAGE_DEPENDS "cuda-core-libs-${cuda_version_suffix}, cuda-extra-libs-${cuda_version_suffix}")
    set(CPACK_DEB_dev_PACKAGE_DEPENDS "cuda-headers-${cuda_version_suffix}")
  else()
    set(CPACK_DEB_libs_PACKAGE_DEPENDS "cuda-cudart-${cuda_version_suffix}, cuda-npp-${cuda_version_suffix}")
    set(CPACK_DEB_dev_PACKAGE_DEPENDS "cuda-cudart-dev-${cuda_version_suffix}, cuda-npp-dev-${cuda_version_suffix}")
    if(HAVE_CUFFT)
      set(CPACK_DEB_libs_PACKAGE_DEPENDS "${CPACK_DEB_libs_PACKAGE_DEPENDS}, cuda-cufft-${cuda_version_suffix}")
      set(CPACK_DEB_dev_PACKAGE_DEPENDS "${CPACK_DEB_dev_PACKAGE_DEPENDS}, cuda-cufft-dev-${cuda_version_suffix}")
    endif()
    if(HAVE_HAVE_CUBLAS)
      set(CPACK_DEB_libs_PACKAGE_DEPENDS "${CPACK_DEB_libs_PACKAGE_DEPENDS}, cuda-cublas-${cuda_version_suffix}")
      set(CPACK_DEB_dev_PACKAGE_DEPENDS "${CPACK_DEB_dev_PACKAGE_DEPENDS}, cuda-cublas-dev-${cuda_version_suffix}")
    endif()
  endif()

  set(CPACK_COMPONENT_dev_DEPENDS libs)
endif()

if(NOT OPENCV_CUSTOM_PACKAGE_INFO)
  set(CPACK_COMPONENT_libs_DISPLAY_NAME "libopencv")
  set(CPACK_COMPONENT_libs_DESCRIPTION "Open Computer Vision Library")
  set(CPACK_COMPONENT_libs_SECTION "libs")

  set(CPACK_COMPONENT_python_DISPLAY_NAME "libopencv-python")
  set(CPACK_COMPONENT_python_DESCRIPTION "Python bindings for Open Source Computer Vision Library")
  set(CPACK_COMPONENT_python_SECTION "python")

  set(CPACK_COMPONENT_java_DISPLAY_NAME "libopencv-java")
  set(CPACK_COMPONENT_java_DESCRIPTION "Java bindings for Open Source Computer Vision Library")
  set(CPACK_COMPONENT_java_SECTION "java")

  set(CPACK_COMPONENT_dev_DISPLAY_NAME "libopencv-dev")
  set(CPACK_COMPONENT_dev_DESCRIPTION "Development files for Open Source Computer Vision Library")
  set(CPACK_COMPONENT_dev_SECTION "libdevel")

  set(CPACK_COMPONENT_docs_DISPLAY_NAME "libopencv-docs")
  set(CPACK_COMPONENT_docs_DESCRIPTION "Documentation for Open Source Computer Vision Library")
  set(CPACK_COMPONENT_docs_SECTION "doc")

  set(CPACK_COMPONENT_samples_DISPLAY_NAME "libopencv-samples")
  set(CPACK_COMPONENT_samples_DESCRIPTION "Samples for Open Source Computer Vision Library")
  set(CPACK_COMPONENT_samples_SECTION "devel")

  set(CPACK_COMPONENT_tests_DISPLAY_NAME "libopencv-tests")
  set(CPACK_COMPONENT_tests_DESCRIPTION "Accuracy and performance tests for Open Source Computer Vision Library")
  set(CPACK_COMPONENT_tests_SECTION "misc")
endif(NOT OPENCV_CUSTOM_PACKAGE_INFO)

if(CPACK_GENERATOR STREQUAL "DEB")
  find_program(GZIP_TOOL NAMES "gzip" PATHS "/bin" "/usr/bin" "/usr/local/bin")
  if(NOT GZIP_TOOL)
    message(FATAL_ERROR "Unable to find 'gzip' program")
  endif()

  execute_process(COMMAND "date" "-R"
                  OUTPUT_VARIABLE CHANGELOG_PACKAGE_DATE
                  OUTPUT_STRIP_TRAILING_WHITESPACE)

  set(CHANGELOG_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}")
  set(ALL_COMPONENTS "libs" "dev" "docs" "python" "java" "samples" "tests")
  foreach (comp ${ALL_COMPONENTS})
    set(DEBIAN_CHANGELOG_OUT_FILE    "${CMAKE_BINARY_DIR}/deb-packages-gen/${comp}/changelog.Debian")
    set(DEBIAN_CHANGELOG_OUT_FILE_GZ "${CMAKE_BINARY_DIR}/deb-packages-gen/${comp}/changelog.Debian.gz")
    set(CHANGELOG_PACKAGE_NAME "${CPACK_COMPONENT_${comp}_DISPLAY_NAME}")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/changelog.Debian.in" "${DEBIAN_CHANGELOG_OUT_FILE}" @ONLY)

    execute_process(COMMAND "${GZIP_TOOL}" "-cf9" "${DEBIAN_CHANGELOG_OUT_FILE}"
                    OUTPUT_FILE "${DEBIAN_CHANGELOG_OUT_FILE_GZ}"
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

    install(FILES "${DEBIAN_CHANGELOG_OUT_FILE_GZ}"
            DESTINATION "share/doc/${CPACK_COMPONENT_${comp}_DISPLAY_NAME}"
            COMPONENT "${comp}")
  endforeach()
endif()

include(CPack)

ENDif(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
