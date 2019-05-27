/* Ziplib */
#include <zlib.h>
#include <zconf.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define CHUNK 16384

#include "ZipLibUtil.h"
#include <vector>


int ExtracMemory2Memory(unsigned char *source_begin, size_t source_size, unsigned char **dest, size_t *dest_size){
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char *in = Z_NULL;
    unsigned char out[CHUNK];
    unsigned char *source_iterator = source_begin;
    unsigned char *source_end = source_begin + source_size;
    std::vector<unsigned char> dest_vector;
    *dest = NULL;
    *dest_size = 0;

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        if (source_end >= source_iterator)
        {
            if (source_iterator + CHUNK<source_end){
                strm.avail_in = CHUNK;
                in=source_iterator;
                source_iterator += CHUNK;
            }
            else{
                strm.avail_in = source_end - source_iterator;
                in=source_iterator;
                source_iterator = source_end;
            }
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }
            have = CHUNK - strm.avail_out;
            dest_vector.insert(dest_vector.end(), &out[0], &out[have]);
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);

    *dest_size = dest_vector.size();
    *dest = new unsigned char[dest_vector.size()];
    std::copy(dest_vector.begin(), dest_vector.end(), *dest);

    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

int ComprssMemory2Memory(unsigned char *source_begin, size_t source_size, unsigned char **dest, size_t *dest_size){
    int level = Z_DEFAULT_COMPRESSION;

    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char *in = Z_NULL;
    unsigned char out[CHUNK];
    unsigned char *source_iterator = source_begin;
    unsigned char *source_end = source_begin + source_size;
    std::vector<unsigned char> dest_vector;
    *dest = NULL;
    *dest_size = 0;

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    if (ret != Z_OK)
        return ret;

    /* compress until end of file */
    do
    {
        if (source_end >= source_iterator)
        {
            if (source_iterator + CHUNK<source_end){
                strm.avail_in = CHUNK;
                in = source_iterator;
                source_iterator += CHUNK;
            }
            else{
                strm.avail_in = source_end - source_iterator;
                in = source_iterator;
                source_iterator = source_end;
            }
        }
        flush = (strm.avail_in == 0) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        /* run deflate() on input until output buffer not full, finish
        compression if all of source has been read in */
        do
        {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            have = CHUNK - strm.avail_out;
            dest_vector.insert(dest_vector.end(), &out[0], &out[have]);
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    (void)deflateEnd(&strm);

    *dest_size = dest_vector.size();
    *dest = new unsigned char[dest_vector.size()];
    std::copy(dest_vector.begin(), dest_vector.end(), *dest);

    return Z_OK;
}

int ExtracFile2File(FILE *source, FILE *dest){
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

int ComprssFile2File(FILE *source, FILE *dest){
    int level = Z_DEFAULT_COMPRESSION;

    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    if (ret != Z_OK)
        return ret;

    /* compress until end of file */
    do
    {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source))
        {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        /* run deflate() on input until output buffer not full, finish
        compression if all of source has been read in */
        do
        {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest))
            {
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    (void)deflateEnd(&strm);
    return Z_OK;
}


int ExtracMemory2File(unsigned char *source_begin, size_t source_size, FILE *dest){
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char *in = Z_NULL;
    unsigned char out[CHUNK];
    unsigned char *source_iterator = source_begin;
    unsigned char *source_end = source_begin + source_size;

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        if (source_end >= source_iterator)
        {
            if (source_iterator + CHUNK<source_end){
                strm.avail_in = CHUNK;
                in = source_iterator;
                source_iterator += CHUNK;
            }
            else{
                strm.avail_in = source_end-source_iterator;
                in = source_iterator;
                source_iterator = source_end;
            }
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

int ComprssFile2Memory(FILE *source, unsigned char **dest, size_t *dest_size){
    int level = Z_DEFAULT_COMPRESSION;

    if (!source)
        return Z_ERRNO;

    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];
    std::vector<unsigned char> dest_vector;
    *dest = NULL;
    *dest_size = 0;

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    if (ret != Z_OK)
        return ret;

    /* compress until end of file */
    do
    {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source))
        {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        /* run deflate() on input until output buffer not full, finish
        compression if all of source has been read in */
        do
        {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            have = CHUNK - strm.avail_out;
            dest_vector.insert(dest_vector.end(), &out[0], &out[have]);
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    (void)deflateEnd(&strm);
    
    *dest_size = dest_vector.size();
    *dest = new unsigned char[dest_vector.size()];
    std::copy(dest_vector.begin(), dest_vector.end(), *dest);

    return Z_OK;
}
