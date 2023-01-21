/*
 * Copyright (C) 2023 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define FAILURE_DEBUG_PREFIX "jpeg"

#include <inttypes.h>
#include <setjmp.h>
#include <algorithm>

#include <vector>
extern "C" {
#include <jpeglib.h>
}
#include <libyuv.h>

#include <hardware/camera3.h>

#include "jpeg.h"
#include "debug.h"
#include "exif.h"

namespace android {
namespace hardware {
namespace camera {
namespace provider {
namespace implementation {
namespace jpeg {
namespace {

bool compressYUVImplPixels(const android_ycbcr& image, jpeg_compress_struct* cinfo) {
    constexpr int kJpegMCUSize = 16;  // we have to feed `jpeg_write_raw_data` in multiples of this

    const uint8_t* y[16];
    const uint8_t* cb[8];
    const uint8_t* cr[8];
    const uint8_t** planes[] = { y, cb, cr };
    const int height = cinfo->image_height;
    const int height1 = height - 1;
    const int ystride = image.ystride;
    const int cstride = image.cstride;

    while (true) {
        const int nscl = cinfo->next_scanline;
        if (nscl >= height) {
            break;
        }

        for (int i = 0; i < kJpegMCUSize; ++i) {
            const int nscli = std::min(nscl + i, height1);
            y[i] = static_cast<const uint8_t*>(image.y) + nscli * ystride;
            if ((i & 1) == 0) {
                const int offset = (nscli / 2) * cstride;
                cb[i / 2] = static_cast<const uint8_t*>(image.cb) + offset;
                cr[i / 2] = static_cast<const uint8_t*>(image.cr) + offset;
            }
        }

        if (!jpeg_write_raw_data(cinfo, const_cast<JSAMPIMAGE>(planes), kJpegMCUSize)) {
            return FAILURE(false);
        }
    }

    return true;
}

struct JpegErrorMgr : public jpeg_error_mgr {
    JpegErrorMgr() {
        error_exit = &onJpegErrorS;
    }

    void onJpegError(j_common_ptr cinfo) {
        {
            char errorMessage[JMSG_LENGTH_MAX];
            memset(errorMessage, 0, sizeof(errorMessage));
            (*format_message)(cinfo, errorMessage);
            ALOGE("%s:%d: JPEG compression failed with '%s'",
                  __func__, __LINE__, errorMessage);
        }

        longjmp(jumpBuffer, 1);
    }

    static void onJpegErrorS(j_common_ptr cinfo) {
        static_cast<JpegErrorMgr*>(cinfo->err)->onJpegError(cinfo);
    }

    jmp_buf jumpBuffer;
};

bool compressYUVImpl(const android_ycbcr& image, const Rect<uint16_t> imageSize,
                     unsigned char* const rawExif, const unsigned rawExifSize,
                     const int quality,
                     jpeg_destination_mgr* sink) {
    jpeg_compress_struct cinfo;
    JpegErrorMgr err;
    bool result;

    cinfo.err = jpeg_std_error(&err);
    jpeg_create_compress(&cinfo);
    cinfo.image_width = imageSize.width;
    cinfo.image_height = imageSize.height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_YCbCr;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    jpeg_default_colorspace(&cinfo);
    cinfo.raw_data_in = TRUE;
    cinfo.dct_method = JDCT_IFAST;
    cinfo.comp_info[0].h_samp_factor = 2;
    cinfo.comp_info[0].v_samp_factor = 2;
    cinfo.comp_info[1].h_samp_factor = 1;
    cinfo.comp_info[1].v_samp_factor = 1;
    cinfo.comp_info[2].h_samp_factor = 1;
    cinfo.comp_info[2].v_samp_factor = 1;
    cinfo.dest = sink;

    if (setjmp(err.jumpBuffer)) {
        jpeg_destroy_compress(&cinfo);
        return FAILURE(false);
    }

    jpeg_start_compress(&cinfo, TRUE);

    if (rawExif) {
        jpeg_write_marker(&cinfo, JPEG_APP0 + 1, rawExif, rawExifSize);
    }

    result = compressYUVImplPixels(image, &cinfo);

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    return result;
}

android_ycbcr resizeYUV(const android_ycbcr& srcYCbCr,
                        const Rect<uint16_t> srcSize,
                        const Rect<uint16_t> dstSize,
                        std::vector<uint8_t>* pDstData) {
    if (srcYCbCr.chroma_step != 1) {
        return FAILURE(android_ycbcr());
    }

    const size_t dstWidth = dstSize.width;
    const size_t dstHeight = dstSize.height;
    if ((dstWidth & 1) || (dstHeight & 1)) {
        return FAILURE(android_ycbcr());
    }

    const size_t wxh = dstWidth * dstHeight;
    std::vector<uint8_t> dstData(wxh * 3 / 2);

    android_ycbcr dstYCbCr = android_ycbcr();
    dstYCbCr.y = &dstData[0];
    dstYCbCr.cb = &dstData[wxh];
    dstYCbCr.cr = &dstData[wxh + (wxh >> 2)];
    dstYCbCr.ystride = dstWidth;
    dstYCbCr.cstride = dstHeight / 2;
    dstYCbCr.chroma_step = 1;

    const int result = libyuv::I420Scale(
        static_cast<const uint8_t*>(srcYCbCr.y), srcYCbCr.ystride,
        static_cast<const uint8_t*>(srcYCbCr.cb), srcYCbCr.cstride,
        static_cast<const uint8_t*>(srcYCbCr.cr), srcYCbCr.cstride,
        srcSize.width, srcSize.height,
        static_cast<uint8_t*>(dstYCbCr.y), dstYCbCr.ystride,
        static_cast<uint8_t*>(dstYCbCr.cb), dstYCbCr.cstride,
        static_cast<uint8_t*>(dstYCbCr.cr), dstYCbCr.cstride,
        dstWidth, dstHeight,
        libyuv::kFilterBilinear);

    if (result) {
        return FAILURE_V(android_ycbcr(), "libyuv::I420Scale failed with %d", result);
    } else {
        *pDstData = std::move(dstData);
        return dstYCbCr;
    }
}

struct StaticBufferSink : public jpeg_destination_mgr {
    StaticBufferSink(void* dst, const size_t dstCapacity) {
        next_output_byte = static_cast<JOCTET*>(dst);
        free_in_buffer = dstCapacity;
        init_destination = &initDestinationS;
        empty_output_buffer = &emptyOutputBufferS;
        term_destination = &termDestinationS;
    }

    static void initDestinationS(j_compress_ptr) {}
    static boolean emptyOutputBufferS(j_compress_ptr) { return 0; }
    static void termDestinationS(j_compress_ptr) {}
};

}  // namespace

bool compressYUV(const android_ycbcr& image,
                 const Rect<uint16_t> imageSize,
                 const CameraMetadata& metadata,
                 void* const jpegData,
                 const size_t jpegDataCapacity) {
    if (image.chroma_step != 1) {
        return FAILURE(false);
    }

    auto exifData = exif::createExifData(metadata, imageSize);
    if (!exifData) {
        return FAILURE(false);
    }

    const camera_metadata_t* const rawMetadata =
        reinterpret_cast<const camera_metadata_t*>(metadata.metadata.data());
    camera_metadata_ro_entry_t metadataEntry;

    do {
        Rect<uint16_t> thumbnailSize = {0, 0};
        int thumbnailQuality = 0;

        if (find_camera_metadata_ro_entry(rawMetadata, ANDROID_JPEG_THUMBNAIL_SIZE,
                                          &metadataEntry)) {
            break;
        } else {
            thumbnailSize.width = metadataEntry.data.i32[0];
            thumbnailSize.height = metadataEntry.data.i32[1];
            if ((thumbnailSize.width <= 0) || (thumbnailSize.height <= 0)) {
                break;
            }
        }

        if (find_camera_metadata_ro_entry(rawMetadata, ANDROID_JPEG_THUMBNAIL_QUALITY,
                                          &metadataEntry)) {
            break;
        } else {
            thumbnailQuality = metadataEntry.data.i32[0];
            if (thumbnailQuality <= 0) {
                break;
            }
        }

        std::vector<uint8_t> thumbnailData;
        const android_ycbcr thumbmnail = resizeYUV(image, imageSize,
                                                   thumbnailSize, &thumbnailData);
        if (!thumbmnail.y) {
            return FAILURE(false);
        }

        StaticBufferSink sink(jpegData, jpegDataCapacity);
        if (!compressYUVImpl(thumbmnail, thumbnailSize, nullptr, 0,
                             thumbnailQuality, &sink)) {
            return FAILURE(false);
        }

        const size_t thumbnailJpegSize = jpegDataCapacity - sink.free_in_buffer;
        void* exifThumbnailJpegDataPtr = exif::exifDataAllocThumbnail(
            exifData.get(), thumbnailJpegSize);
        if (!exifThumbnailJpegDataPtr) {
            return FAILURE(false);
        }

        memcpy(exifThumbnailJpegDataPtr, jpegData, thumbnailJpegSize);
    } while (false);

    const int quality = (find_camera_metadata_ro_entry(rawMetadata,
                                                       ANDROID_JPEG_QUALITY,
                                                       &metadataEntry))
        ? 85 : metadataEntry.data.i32[0];

    unsigned char* rawExif = nullptr;
    unsigned rawExifSize = 0;
    exif_data_save_data(const_cast<ExifData*>(exifData.get()),
                        &rawExif, &rawExifSize);
    if (!rawExif) {
        return FAILURE(false);
    }

    const size_t jpegImageDataCapacity = jpegDataCapacity - sizeof(struct camera3_jpeg_blob);
    StaticBufferSink sink(jpegData, jpegImageDataCapacity);
    const bool result = compressYUVImpl(image, imageSize, rawExif, rawExifSize,
                                        quality, &sink);
    if (result) {
        struct camera3_jpeg_blob blob;
        blob.jpeg_blob_id = CAMERA3_JPEG_BLOB_ID;
        blob.jpeg_size = jpegImageDataCapacity - sink.free_in_buffer;
        memcpy(static_cast<uint8_t*>(jpegData) + jpegImageDataCapacity,
               &blob, sizeof(blob));
    }

    free(rawExif);

    return result;
}

}  // namespace jpeg
}  // namespace implementation
}  // namespace provider
}  // namespace camera
}  // namespace hardware
}  // namespace android
