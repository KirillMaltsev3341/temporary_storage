#include "ImagePNG.h"

#include <png.h>


bool epng::checkFileValidity(FILE *fin)
{
    png_byte signature[SIG_BYTES];
    fread(signature, sizeof(png_byte), SIG_BYTES, fin);
    return (png_sig_cmp(signature, 0, SIG_BYTES) == 0);
}

void epng::ImagePNG::readInfoFields()
{
    width_               = png_get_image_width(png_ptr_, info_ptr_);
    height_              = png_get_image_height(png_ptr_, info_ptr_);
    bit_depth_           = png_get_bit_depth(png_ptr_, info_ptr_);
    color_type_          = png_get_color_type(png_ptr_, info_ptr_);
    interlace_type_      = png_get_interlace_type(png_ptr_, info_ptr_);
    compression_type_    = png_get_compression_type(png_ptr_, info_ptr_);
    filter_type_         = png_get_filter_type(png_ptr_, info_ptr_);
}

void epng::ImagePNG::transformInput()
{
    if (color_type_ == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr_);
    }
    
    if (color_type_ == PNG_COLOR_TYPE_GRAY && bit_depth_ < 8) {
        png_set_expand_gray_1_2_4_to_8(png_ptr_);
    }

    if (png_get_valid(png_ptr_, info_ptr_, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr_);
    }

    if (bit_depth_ == 16) {
        png_set_strip_16(png_ptr_);
    }

    if (bit_depth_ < 8) {
        png_set_packing(png_ptr_);
    }

    if (color_type_ == PNG_COLOR_TYPE_RGB || 
        color_type_ == PNG_COLOR_TYPE_GRAY || 
        color_type_ == PNG_COLOR_TYPE_PALETTE) {
        png_set_filler(png_ptr_, 0xFF, PNG_FILLER_AFTER);
        png_set_add_alpha(png_ptr_, 0xFF, PNG_FILLER_AFTER);
    }

    if (color_type_ == PNG_COLOR_TYPE_GRAY || 
        color_type_ == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png_ptr_);
    }
}

void epng::ImagePNG::readImageFromFile(const char *input_file_name)
{
    FILE* fin = fopen(input_file_name, "rb");
    
    if (!fin) {
        throwError("Error: file could not be opened.", FILE_ERROR);
    }
    
    if (!checkFileValidity(fin)) {
        fclose(fin);
        throwError("Error: wrong file format.", FILE_ERROR);
    }
    
    png_ptr_ = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr_) {
        fclose(fin);
        throwError("Error: png_create_read_struct failed.", PNG_PROCESSING_ERROR);
    }

    info_ptr_ = png_create_info_struct(png_ptr_);
    if (!info_ptr_) {
        png_destroy_read_struct(&png_ptr_, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fin);
        throwError("Error: (info_ptr) png_create_info_struct failed.", PNG_PROCESSING_ERROR);
    }
    
    end_info_ptr_ = png_create_info_struct(png_ptr_);
    if (!end_info_ptr_) {
        png_destroy_read_struct(&png_ptr_, &info_ptr_, (png_infopp)NULL);
        fclose(fin);
        throwError("Error: (end_info_ptr) png_create_info_struct failed.", PNG_PROCESSING_ERROR);
    }

    if (setjmp(png_jmpbuf(png_ptr_))) {
        png_destroy_read_struct(&png_ptr_, &info_ptr_, &end_info_ptr_);
        fclose(fin);
        throwError("Error: png_init_io failed.", PNG_PROCESSING_ERROR);
    }
    png_init_io(png_ptr_, fin);

    png_set_sig_bytes(png_ptr_, SIG_BYTES);
    png_read_info(png_ptr_, info_ptr_);
    readInfoFields();
    transformInput();
    number_of_passes_ = png_set_interlace_handling(png_ptr_);
    png_read_update_info(png_ptr_, info_ptr_);
    readInfoFields();

    if (setjmp(png_jmpbuf(png_ptr_))) {
        png_destroy_read_struct(&png_ptr_, &info_ptr_, &end_info_ptr_);
        fclose(fin);
        throwError("Error: png_read_image failed.", PNG_PROCESSING_ERROR);
    }

    allocateMemmoryForRowPointers();
    allocateMemmoryForColoredPixels();
    clear();

    png_read_image(png_ptr_, row_pointers_);

    if (setjmp(png_jmpbuf(png_ptr_))) {
        png_destroy_read_struct(&png_ptr_, &info_ptr_, &end_info_ptr_);
        fclose(fin);
        throwError("Error: png_read_end failed.", PNG_PROCESSING_ERROR);
    }

    png_read_end(png_ptr_, end_info_ptr_);
    png_destroy_read_struct(&png_ptr_, &info_ptr_, &end_info_ptr_);
    fclose(fin);
}

void epng::ImagePNG::writeImageToFile(const char *output_file_name)
{   
    FILE *fout = fopen(output_file_name, "wb");
    if (!fout) {
        fclose(fout);
        throwError("Error: file could not be opened.", FILE_ERROR);
    }

    png_structp png_ptr_ = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr_) {
        fclose(fout);
        throwError("Error: png_create_write_struct failed.", PNG_PROCESSING_ERROR);
    }
    
    png_infop info_ptr_ = png_create_info_struct(png_ptr_);

    if (!info_ptr_) {
        png_destroy_info_struct(png_ptr_, (png_infopp)NULL);
        fclose(fout);
        throwError("Error: (info_ptr) png_create_info_struct failed.", PNG_PROCESSING_ERROR);
    }

    if (setjmp(png_jmpbuf(png_ptr_))) {
        png_destroy_info_struct(png_ptr_, &info_ptr_);
        fclose(fout);
        throwError("Error: png_init_io failed.", PNG_PROCESSING_ERROR);
    }
    
    png_init_io(png_ptr_, fout);


    if (setjmp(png_jmpbuf(png_ptr_))) {
        png_destroy_info_struct(png_ptr_, &info_ptr_);
        fclose(fout);
        throwError("Error: png_set_IHDR failed.", PNG_PROCESSING_ERROR);
    }

    png_set_IHDR(
        png_ptr_, 
        info_ptr_, 
        width_, 
        height_, 
        bit_depth_, 
        color_type_,
        interlace_type_, 
        compression_type_, 
        filter_type_
    );
    
    png_write_info(png_ptr_, info_ptr_);

    if (setjmp(png_jmpbuf(png_ptr_))) {
        png_destroy_info_struct(png_ptr_, &info_ptr_);
        fclose(fout);
        throwError("Error: png_write_image failed.", PNG_PROCESSING_ERROR);
    }
    
    png_write_image(png_ptr_, row_pointers_);

    if (setjmp(png_jmpbuf(png_ptr_))) {
        png_destroy_info_struct(png_ptr_, &info_ptr_);
        fclose(fout);
        throwError("Error: png_write_end failed.", PNG_PROCESSING_ERROR);
    }

    png_write_end(png_ptr_, NULL);
    
    png_destroy_write_struct(&png_ptr_, &info_ptr_);

    fclose(fout);
}


void epng::showImageInfo(const char *input_file_name)
{
    FILE* fin = fopen(input_file_name, "rb");
    
    if (!fin) {
        throwError("Error: file could not be opened.", FILE_ERROR);
    }
    
    if (!checkFileValidity(fin)) {
        fclose(fin);
        throwError("Error: wrong file format.", FILE_ERROR);
    }
    
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fin);
        throwError("Error: png_create_read_struct failed.", PNG_PROCESSING_ERROR);
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fin);
        throwError("Error: (info_ptr) png_create_info_struct failed.", PNG_PROCESSING_ERROR);
    }
    
    png_infop end_info_ptr = png_create_info_struct(png_ptr);
    if (!end_info_ptr) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        fclose(fin);
        throwError("Error: (end_info_ptr) png_create_info_struct failed.", PNG_PROCESSING_ERROR);
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
        fclose(fin);
        throwError("Error: png_init_io failed.", PNG_PROCESSING_ERROR);
    }


    png_init_io(png_ptr, fin);

    png_set_sig_bytes(png_ptr, SIG_BYTES);
    png_read_info(png_ptr, info_ptr);

    
    png_uint_32    width;
    png_uint_32    height;
    int            bit_depth;
    int            color_type;
    int            interlace_type;
    int            compression_type;
    int            filter_type;
    
    png_get_IHDR(png_ptr, info_ptr, &width, &height, 
         &bit_depth, &color_type, &interlace_type, 
         &compression_type, &filter_type);
    
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    fclose(fin);

    printf("%s: %d x %d, %d-bit/color, color type - %d\n", input_file_name, width, height, bit_depth, color_type);
}