/* vim: set expandtab ts=4 sw=4: */
/*
 * You may redistribute this program and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef BufferAllocator_H
#define BufferAllocator_H

#include "memory/Allocator.h"
#include "util/UniqueName.h"
#include "util/Gcc.h"
#include "util/Linker.h"
Linker_require("memory/BufferAllocator.c")

/**
 * Create a new Allocator which allocates from to a user supplied buffer.
 * This allocator will reset the pointer to the beginning of the buffer when
 * free() is called on it, it is up to the user to free their buffer.
 * Allocator_child(allocator) always returns NULL.
 *
 * @param buffer an array to write to.
 * @param length the size of the array. If more is written than this length,
 *               further allocations will fail and return NULL.
 */
struct Allocator* BufferAllocator__new(void* buffer,
                                       unsigned long length,
                                       char* file,
                                       int line);

#define BufferAllocator_new(a,b) BufferAllocator__new((a),(b),Gcc_SHORT_FILE,Gcc_LINE)

// UniqueName_MK has (via __COUNTER__) a single name per macro expansion.
#define BufferAllocator_STACK(name, length) \
    BufferAllocator_STACK_impl(name, length, UniqueName_MK(BufferAllocator_STACK_tmp_))
#define BufferAllocator_STACK_impl(name, length, tmp) \
    uint8_t tmp[length]; \
    name = BufferAllocator_new(tmp, length);

#endif
