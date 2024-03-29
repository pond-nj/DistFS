#define FUSE_USE_VERSION 26

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.h"
#include "dist.h"
#include "meta.h"
#include "params.h"
#include "util.h"

#ifdef HAVE_SYS_XATTR_H
#include <sys/xattr.h>
#endif

#include "log.h"

///////////////////////////////////////////////////////////
//
// Prototypes for all these functions, and the C-style comments,
// come from /usr/include/fuse.h
//
/** Get file attributes.
 *
 * Similar to stat().  The 'st_dev' and 'st_blksize' fields are
 * ignored.  The 'st_ino' field is ignored except if the 'use_ino'
 * mount option is given.
 */
int fs_getattr(const char *path, struct stat *statbuf) {
  // TODO(Pond):
  int retstat;
  char fpath[PATH_MAX];

  log_msg("\nfs_getattr(path=\"%s\", statbuf=0x%08x)\n", path, statbuf);
  fs_fullpath(fpath, path);

  retstat = log_syscall("lstat", lstat(fpath, statbuf), 0);

  log_stat(statbuf);

  return retstat;
}

/** Read the target of a symbolic link
 *
 * The buffer should be filled with a null terminated string.  The
 * buffer size argument includes the space for the terminating
 * null character.  If the linkname is too long to fit in the
 * buffer, it should be truncated.  The return value should be 0
 * for success.
 */
// Note the system readlink() will truncate and lose the terminating
// null.  So, the size passed to to the system readlink() must be one
// less than the size passed to fs_readlink()
// fs_readlink() code by Bernardo F Costa (thanks!)
int fs_readlink(const char *path, char *link, size_t size) {
  // int retstat;
  // char fpath[PATH_MAX];

  log_msg("\nfs_readlink(path=\"%s\", link=\"%s\", size=%d)\n", path, link,
          size);
  // fs_fullpath(fpath, path);

  // retstat = log_syscall("readlink", readlink(fpath, link, size - 1), 0);
  // if (retstat >= 0) {
  //   link[retstat] = '\0';
  //   retstat = 0;
  //   log_msg("    link=\"%s\"\n", link);
  // }

  return 0;
}

/** Create a file node
 *
 * There is no create() operation, mknod() will be called for
 * creation of all non-directory, non-symlink nodes.
 */
// shouldn't that comment be "if" there is no.... ?
int fs_mknod(const char *path, mode_t mode, dev_t dev) {
  int retstat;

  log_msg("\nfs_mknod(path=\"%s\", mode=0%3o, dev=%lld)\n", path, mode, dev);

  // On Linux this could just be 'mknod(path, mode, dev)' but this
  // tries to be be more portable by honoring the quote in the Linux
  // mknod man page stating the only portable use of mknod() is to
  // make a fifo, but saying it should never actually be used for
  // that.

  // TODO(Pond): what does this do?

  // TODO(Pond): log call here?
  log_syscall("add_meta", add_meta(path, 0));

  // if (S_ISREG(mode)) {
  //   log_syscall("add_meta", add_meta(path, 0));
  //   // if (retstat >= 0) retstat = log_syscall("close", close(retstat), 0);
  // } else if (S_ISFIFO(mode))
  //   retstat = log_syscall("mkfifo", mkfifo(fpath, mode), 0);
  // else
  //   retstat = log_syscall("mknod", mknod(fpath, mode, dev), 0);

  return 0;
}

/** Create a directory */
int fs_mkdir(const char *path, mode_t mode) {
  // char fpath[PATH_MAX];

  log_msg("\nfs_mkdir(path=\"%s\", mode=0%3o)\n", path, mode);
  // fs_fullpath(fpath, path);

  // return log_syscall("mkdir", mkdir(fpath, mode), 0);
  return 0;
}

/** Remove a file */
int fs_unlink(const char *path) {
  // char fpath[PATH_MAX];

  log_msg("fs_unlink(path=\"%s\")\n", path);
  // fs_fullpath(fpath, path);

  // return log_syscall("unlink", unlink(fpath), 0);
  return 0;
}

/** Remove a directory */
int fs_rmdir(const char *path) {
  // char fpath[PATH_MAX];

  log_msg("fs_rmdir(path=\"%s\")\n", path);
  // fs_fullpath(fpath, path);

  // return log_syscall("rmdir", rmdir(fpath), 0);
  return 0;
}

/** Create a symbolic link */
// The parameters here are a little bit confusing, but do correspond
// to the symlink() system call.  The 'path' is where the link points,
// while the 'link' is the link itself.  So we need to leave the path
// unaltered, but insert the link into the mounted directory.
int fs_symlink(const char *path, const char *link) {
  // char flink[PATH_MAX];

  log_msg("\nfs_symlink(path=\"%s\", link=\"%s\")\n", path, link);
  // fs_fullpath(flink, link);

  // return log_syscall("symlink", symlink(path, flink), 0);
  return 0;
}

/** Rename a file */
// both path and newpath are fs-relative
int fs_rename(const char *path, const char *newpath) {
  // char fpath[PATH_MAX];
  // char fnewpath[PATH_MAX];

  log_msg("\nfs_rename(fpath=\"%s\", newpath=\"%s\")\n", path, newpath);
  // fs_fullpath(fpath, path);
  // fs_fullpath(fnewpath, newpath);

  // return log_syscall("rename", rename(fpath, fnewpath), 0);
  return 0;
}

/** Create a hard link to a file */
int fs_link(const char *path, const char *newpath) {
  // char fpath[PATH_MAX], fnewpath[PATH_MAX];

  log_msg("\nfs_link(path=\"%s\", newpath=\"%s\")\n", path, newpath);
  // fs_fullpath(fpath, path);
  // fs_fullpath(fnewpath, newpath);

  // return log_syscall("link", link(fpath, fnewpath), 0);
  return 0;
}

/** Change the permission bits of a file */
int fs_chmod(const char *path, mode_t mode) {
  // char fpath[PATH_MAX];

  log_msg("\nfs_chmod(fpath=\"%s\", mode=0%03o)\n", path, mode);
  // fs_fullpath(fpath, path);

  // return log_syscall("chmod", chmod(fpath, mode), 0);
  return 0;
}

/** Change the owner and group of a file */
int fs_chown(const char *path, uid_t uid, gid_t gid)

{
  // char fpath[PATH_MAX];

  log_msg("\nfs_chown(path=\"%s\", uid=%d, gid=%d)\n", path, uid, gid);
  // fs_fullpath(fpath, path);

  // return log_syscall("chown", chown(fpath, uid, gid), 0);
  return 0;
}

/** Change the size of a file */
int fs_truncate(const char *path, off_t newsize) {
  // char fpath[PATH_MAX];

  log_msg("\nfs_truncate(path=\"%s\", newsize=%lld)\n", path, newsize);
  // fs_fullpath(fpath, path);

  // return log_syscall("truncate", truncate(fpath, newsize), 0);
  return 0;
}

/** Change the access and/or modification times of a file */
/* note -- I'll want to change this as soon as 2.6 is in debian testing */
int fs_utime(const char *path, struct utimbuf *ubuf) {
  // char fpath[PATH_MAX];

  log_msg("\nfs_utime(path=\"%s\", ubuf=0x%08x)\n", path, ubuf);
  // fs_fullpath(fpath, path);

  // return log_syscall("utime", utime(fpath, ubuf), 0);
  return 0;
}

// TODO(Pond): read comments
/** File open operation
 *
 * No creation, or truncation flags (O_CREAT, O_EXCL, O_TRUNC)
 * will be passed to open().  Open should check if the operation
 * is permitted for the given flags.  Optionally open may also
 * return an arbitrary filehandle in the fuse_file_info structure,
 * which will be passed to all file operations.
 *
 * Changed in version 2.2
 */
int fs_open(const char *path, struct fuse_file_info *fi) {
  log_msg("\nfs_open(path\"%s\", fi=0x%08x)\n", path, fi);
  add_meta(path, 0);

  log_fi(fi);

  return 0;
}

/** Read data from an open file
 *
 * Read should return exactly the number of bytes requested except
 * on EOF or error, otherwise the rest of the data will be
 * substituted with zeroes.  An exception to this is when the
 * 'direct_io' mount option is specified, in which case the return
 * value of the read system call will reflect the return value of
 * this operation.
 *
 * Changed in version 2.2
 */
// I don't fully understand the documentation above -- it doesn't
// match the documentation for the read() system call which says it
// can return with anything up to the amount of data requested. nor
// with the fusexmp code which returns the amount of data also
// returned by read.
int fs_read(const char *path, char *buf, size_t size, off_t offset,
            struct fuse_file_info *fi) {
  log_msg(
      "\nfs_read(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n",
      path, buf, size, offset, fi);
  // no need to get fpath on this one, since I work from fi->fh not the path
  log_fi(fi);

  // TODO(Pond): maybe there is no need to load the whole file into cache?
  if (!has_cache(path)) {
    remote_to_cache(path);
  }

  return log_syscall("read_from_cache",
                     read_from_cache(path, buf, size, offset), 0);
}

/** Write data to an open file
 *
 * Write should return exactly the number of bytes requested
 * except on error.  An exception to this is when the 'direct_io'
 * mount option is specified (see read operation).
 *
 * Changed in version 2.2
 */
// As  with read(), the documentation above is inconsistent with the
// documentation for the write() system call.
int fs_write(const char *path, const char *buf, size_t size, off_t offset,
             struct fuse_file_info *fi) {
  log_msg(
      "\nfs_write(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n",
      path, buf, size, offset, fi);
  log_fi(fi);

  assert(size != 0);  // does not allow file of size 0

  return log_syscall("write_to_cache", write_to_cache(path, buf, size, offset),
                     0);
}

/** Get file system statistics
 *
 * The 'f_frsize', 'f_favail', 'f_fsid' and 'f_flag' fields are ignored
 *
 * Replaced 'struct statfs' parameter with 'struct statvfs' in
 * version 2.5
 */
int fs_statfs(const char *path, struct statvfs *statv) {
  // int retstat = 0;
  // char fpath[PATH_MAX];

  log_msg("\nfs_statfs(path=\"%s\", statv=0x%08x)\n", path, statv);
  // fs_fullpath(fpath, path);

  // // get stats for underlying filesystem
  // retstat = log_syscall("statvfs", statvfs(fpath, statv), 0);

  // log_statvfs(statv);

  // return retstat;
  return 0;
}

/** Possibly flush cached data
 *
 * BIG NOTE: This is not equivalent to fsync().  It's not a
 * request to sync dirty data.
 *
 * Flush is called on each close() of a file descriptor.  So if a
 * filesystem wants to return write errors in close() and the file
 * has cached dirty data, this is a good place to write back data
 * and return any errors.  Since many applications ignore close()
 * errors this is not always useful.
 *
 * NOTE: The flush() method may be called more than once for each
 * open().  This happens if more than one file descriptor refers
 * to an opened file due to dup(), dup2() or fork() calls.  It is
 * not possible to determine if a flush is final, so each flush
 * should be treated equally.  Multiple write-flush sequences are
 * relatively rare, so this shouldn't be a problem.
 *
 * Filesystems shouldn't assume that flush will always be called
 * after some writes, or that if will be called at all.
 *
 * Changed in version 2.2
 */
// this is a no-op in BBFS.  It just logs the call and returns success
int fs_flush(const char *path, struct fuse_file_info *fi) {
  log_msg("\nfs_flush(path=\"%s\", fi=0x%08x)\n", path, fi);
  // no need to get fpath on this one, since I work from fi->fh not the path
  log_fi(fi);

  return 0;
}

/** Release an open file
 *
 * Release is called when there are no more references to an open
 * file: all file descriptors are closed and all memory mappings
 * are unmapped.
 *
 * For every open() call there will be exactly one release() call
 * with the same flags and file descriptor.  It is possible to
 * have a file opened more than once, in which case only the last
 * release will mean, that no more reads/writes will happen on the
 * file.  The return value of release is ignored.
 *
 * Changed in version 2.2
 */
int fs_release(const char *path, struct fuse_file_info *fi) {
  log_msg("\nfs_release(path=\"%s\", fi=0x%08x)\n", path, fi);
  log_fi(fi);

  // We need to close the file.  Had we allocated any resources
  // (buffers etc) we'd need to free them here as well.

  return log_syscall("cache_to_remote", cache_to_remote(path), 0);
}

/** Synchronize file contents
 *
 * If the datasync parameter is non-zero, then only the user data
 * should be flushed, not the meta data.
 *
 * Changed in version 2.2
 */
int fs_fsync(const char *path, int datasync, struct fuse_file_info *fi) {
  log_msg("\nfs_fsync(path=\"%s\", datasync=%d, fi=0x%08x)\n", path, datasync,
          fi);
  log_fi(fi);

  //   // some unix-like systems (notably freebsd) don't have a datasync call
  // #ifdef HAVE_FDATASYNC
  //   if (datasync)
  //     return log_syscall("fdatasync", fdatasync(fi->fh), 0);
  //   else
  // #endif
  // return log_syscall("fsync", fsync(fi->fh), 0);
  return 0;
}

#ifdef HAVE_SYS_XATTR_H
/** Note that my implementations of the various xattr functions use
    the 'l-' versions of the functions (eg fs_setxattr() calls
    lsetxattr() not setxattr(), etc).  This is because it appears any
    symbolic links are resolved before the actual call takes place, so
    I only need to use the system-provided calls that don't follow
    them */

/** Set extended attributes */
int fs_setxattr(const char *path, const char *name, const char *value,
                size_t size, int flags) {
  // char fpath[PATH_MAX];

  log_msg(
      "\nfs_setxattr(path=\"%s\", name=\"%s\", value=\"%s\", size=%d, "
      "flags=0x%08x)\n",
      path, name, value, size, flags);
  // fs_fullpath(fpath, path);

  // return log_syscall("lsetxattr", lsetxattr(fpath, name, value, size, flags),
  //                    0);
  return 0;
}

/** Get extended attributes */
int fs_getxattr(const char *path, const char *name, char *value, size_t size) {
  // int retstat = 0;
  // char fpath[PATH_MAX];

  log_msg(
      "\nfs_getxattr(path = \"%s\", name = \"%s\", value = 0x%08x, size = "
      "%d)\n",
      path, name, value, size);
  // fs_fullpath(fpath, path);

  // retstat = log_syscall("lgetxattr", lgetxattr(fpath, name, value, size), 0);
  // if (retstat >= 0) log_msg("    value = \"%s\"\n", value);

  // return retstat;
  return 0;
}

/** List extended attributes */
int fs_listxattr(const char *path, char *list, size_t size) {
  // int retstat = 0;
  // char fpath[PATH_MAX];
  // char *ptr;

  log_msg("\nfs_listxattr(path=\"%s\", list=0x%08x, size=%d)\n", path, list,
          size);
  // fs_fullpath(fpath, path);

  // retstat = log_syscall("llistxattr", llistxattr(fpath, list, size), 0);
  // if (retstat >= 0) {
  //   log_msg("    returned attributes (length %d):\n", retstat);
  //   if (list != NULL)
  //     for (ptr = list; ptr < list + retstat; ptr += strlen(ptr) + 1)
  //       log_msg("    \"%s\"\n", ptr);
  //   else
  //     log_msg("    (null)\n");
  // }

  // return retstat;
  return 0;
}

/** Remove extended attributes */
int fs_removexattr(const char *path, const char *name) {
  // char fpath[PATH_MAX];

  log_msg("\nfs_removexattr(path=\"%s\", name=\"%s\")\n", path, name);
  // fs_fullpath(fpath, path);

  // return log_syscall("lremovexattr", lremovexattr(fpath, name), 0);
  return 0;
}
#endif

/** Open directory
 *
 * This method should check if the open operation is permitted for
 * this  directory
 *
 * Introduced in version 2.3
 */
int fs_opendir(const char *path, struct fuse_file_info *fi) {
  // DIR *dp;
  // int retstat = 0;
  // char fpath[PATH_MAX];

  log_msg("\nfs_opendir(path=\"%s\", fi=0x%08x)\n", path, fi);
  // fs_fullpath(fpath, path);

  // // since opendir returns a pointer, takes some custom handling of
  // // return status.
  // dp = opendir(fpath);
  // log_msg("    opendir returned 0x%p\n", dp);
  // if (dp == NULL) retstat = log_error("fs_opendir opendir");

  // fi->fh = (intptr_t)dp;

  // log_fi(fi);

  // return retstat;
  return 0;
}

/** Read directory
 *
 * This supersedes the old getdir() interface.  New applications
 * should use this.
 *
 * The filesystem may choose between two modes of operation:
 *
 * 1) The readdir implementation ignores the offset parameter, and
 * passes zero to the filler function's offset.  The filler
 * function will not return '1' (unless an error happens), so the
 * whole directory is read in a single readdir operation.  This
 * works just like the old getdir() method.
 *
 * 2) The readdir implementation keeps track of the offsets of the
 * directory entries.  It uses the offset parameter and always
 * passes non-zero offset to the filler function.  When the buffer
 * is full (or an error happens) the filler function will return
 * '1'.
 *
 * Introduced in version 2.3
 */

int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
               off_t offset, struct fuse_file_info *fi) {
  int retstat = 0;
  DIR *dp;
  struct dirent *de;

  log_msg(
      "\nfs_readdir(path=\"%s\", buf=0x%08x, filler=0x%08x, offset=%lld, "
      "fi=0x%08x)\n",
      path, buf, filler, offset, fi);
  // once again, no need for fullpath -- but note that I need to cast fi->fh
  dp = (DIR *)(uintptr_t)fi->fh;

  // Every directory contains at least two entries: . and ..  If my
  // first call to the system readdir() returns NULL I've got an
  // error; near as I can tell, that's the only condition under
  // which I can get an error from readdir()
  de = readdir(dp);
  log_msg("    readdir returned 0x%p\n", de);
  if (de == 0) {
    retstat = log_error("fs_readdir readdir");
    return retstat;
  }

  // This will copy the entire directory into the buffer.  The loop exits
  // when either the system readdir() returns NULL, or filler()
  // returns something non-zero.  The first case just means I've
  // read the whole directory; the second means the buffer is full.
  do {
    log_msg("calling filler with name %s\n", de->d_name);
    if (filler(buf, de->d_name, NULL, 0) != 0) {
      log_msg("    ERROR fs_readdir filler:  buffer full");
      return -ENOMEM;
    }
  } while ((de = readdir(dp)) != NULL);

  log_fi(fi);

  return retstat;
}

/** Release directory
 *
 * Introduced in version 2.3
 */
int fs_releasedir(const char *path, struct fuse_file_info *fi) {
  // int retstat = 0;

  log_msg("\nfs_releasedir(path=\"%s\", fi=0x%08x)\n", path, fi);
  log_fi(fi);

  // closedir((DIR *)(uintptr_t)fi->fh);

  // return retstat;
  return 0;
}

/** Synchronize directory contents
 *
 * If the datasync parameter is non-zero, then only the user data
 * should be flushed, not the meta data
 *
 * Introduced in version 2.3
 */
// when exactly is this called?  when a user calls fsync and it
// happens to be a directory? ??? >>> I need to implement this...
int fs_fsyncdir(const char *path, int datasync, struct fuse_file_info *fi) {
  // int retstat = 0;

  log_msg("\nfs_fsyncdir(path=\"%s\", datasync=%d, fi=0x%08x)\n", path,
          datasync, fi);
  log_fi(fi);

  // return retstat;
  return 0;
}

/**
 * Initialize filesystem
 *
 * The return value will passed in the private_data field of
 * fuse_context to all file operations and as a parameter to the
 * destroy() method.
 *
 * Introduced in version 2.3
 * Changed in version 2.6
 */
// Undocumented but extraordinarily useful fact:  the fuse_context is
// set up before this function is called, and
// fuse_get_context()->private_data returns the user_data passed to
// fuse_main().  Really seems like either it should be a third
// parameter coming in here, or else the fact should be documented
// (and this might as well return void, as it did in older versions of
// FUSE).
void *fs_init(struct fuse_conn_info *conn) {
  log_msg("\nfs_init()\n");

  log_conn(conn);
  log_fuse_context(fuse_get_context());

  // TODO(Pond): init structure e.g. mem

  return FS_DATA;
}

/**
 * Clean up filesystem
 *
 * Called on filesystem exit.
 *
 * Introduced in version 2.3
 */
void fs_destroy(void *userdata) {
  log_msg("\nfs_destroy(userdata=0x%08x)\n", userdata);
  // TODO(Pond): cleanup?
}

/**
 * Check file access permissions
 *
 * This will be called for the access() system call.  If the
 * 'default_permissions' mount option is given, this method is not
 * called.
 *
 * This method is not called under Linux kernel versions 2.4.x
 *
 * Introduced in version 2.5
 */
int fs_access(const char *path, int mask) {
  // int retstat = 0;
  // char fpath[PATH_MAX];

  log_msg("\nfs_access(path=\"%s\", mask=0%o)\n", path, mask);
  // fs_fullpath(fpath, path);

  // retstat = access(fpath, mask);

  // if (retstat < 0) retstat = log_error("fs_access access");

  // return retstat;
  return 0;
}

/**
 * Create and open a file
 *
 * If the file does not exist, first create it with the specified
 * mode, and then open it.
 *
 * If this method is not implemented or under Linux kernel
 * versions earlier than 2.6.15, the mknod() and open() methods
 * will be called instead.
 *
 * Introduced in version 2.5
 */
// Not implemented.  I had a version that used creat() to create and
// open the file, which it turned out opened the file write-only.

/**
 * Change the size of an open file
 *
 * This method is called instead of the truncate() method if the
 * truncation was invoked from an ftruncate() system call.
 *
 * If this method is not implemented or under Linux kernel
 * versions earlier than 2.6.15, the truncate() method will be
 * called instead.
 *
 * Introduced in version 2.5
 */
int fs_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi) {
  // int retstat = 0;

  log_msg("\nfs_ftruncate(path=\"%s\", offset=%lld, fi=0x%08x)\n", path, offset,
          fi);
  log_fi(fi);

  // retstat = ftruncate(fi->fh, offset);
  // if (retstat < 0) retstat = log_error("fs_ftruncate ftruncate");

  // return retstat;
  return 0;
}

/**
 * Get attributes from an open file
 *
 * This method is called instead of the getattr() method if the
 * file information is available.
 *
 * Currently this is only called after the create() method if that
 * is implemented (see above).  Later it may be called for
 * invocations of fstat() too.
 *
 * Introduced in version 2.5
 */
int fs_fgetattr(const char *path, struct stat *statbuf,
                struct fuse_file_info *fi) {
  // int retstat = 0;

  log_msg("\nfs_fgetattr(path=\"%s\", statbuf=0x%08x, fi=0x%08x)\n", path,
          statbuf, fi);
  log_fi(fi);

  // On FreeBSD, trying to do anything with the mountpoint ends up
  // opening it, and then using the FD for an fgetattr.  So in the
  // special case of a path of "/", I need to do a getattr on the
  // underlying root directory instead of doing the fgetattr().
  // if (!strcmp(path, "/")) return fs_getattr(path, statbuf);

  // retstat = fstat(fi->fh, statbuf);
  // if (retstat < 0) retstat = log_error("fs_fgetattr fstat");

  // log_stat(statbuf);

  // return retstat;
  return 0;
}

struct fuse_operations fs_oper = {
    .getattr = fs_getattr,
    .readlink = fs_readlink,
    // no .getdir -- that's deprecated
    .getdir = NULL,
    .mknod = fs_mknod,
    .mkdir = fs_mkdir,
    .unlink = fs_unlink,
    .rmdir = fs_rmdir,
    .symlink = fs_symlink,
    .rename = fs_rename,
    .link = fs_link,
    .chmod = fs_chmod,
    .chown = fs_chown,
    .truncate = fs_truncate,
    .utime = fs_utime,
    .open = fs_open,
    .read = fs_read,
    .write = fs_write,
    /** Just a placeholder, don't set */  // huh???
    .statfs = fs_statfs,
    .flush = fs_flush,
    .release = fs_release,
    .fsync = fs_fsync,

#ifdef HAVE_SYS_XATTR_H
    .setxattr = fs_setxattr,
    .getxattr = fs_getxattr,
    .listxattr = fs_listxattr,
    .removexattr = fs_removexattr,
#endif
    .opendir = fs_opendir,
    .readdir = fs_readdir,
    .releasedir = fs_releasedir,
    .fsyncdir = fs_fsyncdir,
    .init = fs_init,
    .destroy = fs_destroy,
    .access = fs_access,
    .ftruncate = fs_ftruncate,
    .fgetattr = fs_fgetattr};

void fs_usage() {
  fprintf(stderr, "usage:  bbfs [FUSE and mount options] rootDir mountPoint\n");
  abort();
}

int main(int argc, char *argv[]) {
  int fuse_stat;
  struct fs_state *fs_data;

  // bbfs doesn't do any access checking on its own (the comment
  // blocks in fuse.h mention some of the functions that need
  // accesses checked -- but note there are other functions, like
  // chown(), that also need checking!).  Since running bbfs as root
  // will therefore open Metrodome-sized holes in the system
  // security, we'll check if root is trying to mount the filesystem
  // and refuse if it is.  The somewhat smaller hole of an ordinary
  // user doing it with the allow_other flag is still there because
  // I don't want to parse the options string.
  if ((getuid() == 0) || (geteuid() == 0)) {
    fprintf(stderr,
            "Running BBFS as root opens unnacceptable security holes\n");
    return 1;
  }

  // See which version of fuse we're running
  fprintf(stderr, "Fuse library version %d.%d\n", FUSE_MAJOR_VERSION,
          FUSE_MINOR_VERSION);

  // Perform some sanity checking on the command line:  make sure
  // there are enough arguments, and that neither of the last two
  // start with a hyphen (this will break if you actually have a
  // rootpoint or mountpoint whose name starts with a hyphen, but so
  // will a zillion other programs)
  if ((argc < 3) || (argv[argc - 2][0] == '-') || (argv[argc - 1][0] == '-'))
    fs_usage();

  fs_data = malloc(sizeof(struct fs_state));
  if (fs_data == NULL) {
    perror("main calloc");
    abort();
  }

  // Pull the rootdir out of the argument list and save it in my
  // internal data
  fs_data->rootdir = realpath(argv[argc - 2], NULL);
  argv[argc - 2] = argv[argc - 1];
  argv[argc - 1] = NULL;
  argc--;

  fs_data->logfile = log_open();

  // turn over control to fuse
  fprintf(stderr, "about to call fuse_main\n");
  fuse_stat = fuse_main(argc, argv, &fs_oper, fs_data);
  fprintf(stderr, "fuse_main returned %d\n", fuse_stat);

  return fuse_stat;
}
