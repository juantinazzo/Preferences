/**
 * @file prefs_impl_custom.h
 * @brief Custom Preferences implementation using the application's LittleFS instance
 * @date 14/11/2025
 */

#ifndef PREFS_IMPL_CUSTOM_H
#define PREFS_IMPL_CUSTOM_H

#include "storage.h"
#include "FS.h"

// Helper function to get file from our custom FS
inline File _prefs_open(const char *path, const char *mode)
{
    FS *fs = storage_get_fs();
    if (!fs)
        return File();
    return fs->open(path, mode);
}

// Custom implementations that use our storage instance
static bool _fs_init()
{
    // Already initialized by storage_init()
    return true;
}

static bool _fs_mkdir(const char *path)
{
    FS *fs = storage_get_fs();
    if (!fs)
        return false;

    // Create the directory
    bool result = fs->mkdir(path);
    return result;
}

static bool verifyContent(File &f, const void *buf, int bufsize)
{
    if (int(f.size()) == bufsize && bufsize <= 1024)
    {
        uint8_t tmp[bufsize];
        if (f.read((uint8_t *)tmp, bufsize) == bufsize)
        {
            if (!memcmp(buf, tmp, bufsize))
            {
                return true;
            }
        }
    }
    return false;
}

static bool _fs_verify(const char *path, const void *buf, int bufsize)
{
    if (File f = _prefs_open(path, "r"))
    {
        return verifyContent(f, buf, bufsize);
    }
    return false;
}

static int _fs_create(const char *path, const void *buf, int bufsize)
{
    if (File f = _prefs_open(path, "w"))
    {
        return f.write((const uint8_t *)buf, bufsize);
    }
    return -1;
}

static int _fs_update(const char *path, const void *buf, int bufsize)
{
    if (File f = _prefs_open(path, "r+"))
    {
        if (verifyContent(f, buf, bufsize))
        {
            return bufsize;
        }
        if (int(f.size()) <= bufsize)
        {
            f.seek(0, SeekSet);
            return f.write((const uint8_t *)buf, bufsize);
        }
    }
    return _fs_create(path, buf, bufsize);
}

static int _fs_read(const char *path, void *buf, int bufsize)
{
    if (File f = _prefs_open(path, "r"))
    {
        return f.read((uint8_t *)buf, bufsize);
    }
    return -1;
}

static int _fs_get_size(const char *path)
{
    if (File f = _prefs_open(path, "r"))
    {
        return f.size();
    }
    return -1;
}

static bool _fs_exists(const char *path)
{
    FS *fs = storage_get_fs();
    if (!fs)
        return false;
    return fs->exists(path);
}

static bool _fs_rename(const char *from, const char *to)
{
    FS *fs = storage_get_fs();
    if (!fs)
        return false;
    return fs->rename(from, to);
}

static bool _fs_unlink(const char *path)
{
    FS *fs = storage_get_fs();
    if (!fs)
        return false;
    return fs->remove(path);
}

static bool _fs_clean_dir(const char *path)
{
    FS *fs = storage_get_fs();
    if (!fs)
        return false;

    // Remove the directory (recursively removing files is handled by the FS)
    fs->remove(path);
    return true;
}

#endif
