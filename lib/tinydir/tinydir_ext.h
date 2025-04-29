#pragma once
#ifndef _TINYDIR_EXT_H
#define _TINYDIR_EXT_H

#include <tinydir.h>

// checks whether or not a directory is a special navigation(. and ..)
int tinydir_is_nav(const tinydir_file file);

#endif // _TINYDIR_EXT_H
