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
#ifndef Defined_H
#define Defined_H

/**
 * Defined(macro) is replaced at build time by 1 if the macro given is defined,
 * and 0 if it is not.
 *
 * This is a substitute for the historical JavaScript-based preprocessor that
 * performed the same job.  The macro must be one of the names mirrored below,
 * each of which is kept in sync with the corresponding -D flag emitted by
 * configure.ac.  If a new condition is needed, add a Defined_<name> entry here
 * and the corresponding definition in configure.ac.
 */
#define Defined(macro) (Defined_##macro)

#define Defined_Log_DEBUG 1
#define Defined_Log_KEYS 0
#define Defined_Log_INFO 1

#define Defined_win32 0
#define Defined_linux 1
#define Defined_darwin 0
#define Defined_sunos 0
#define Defined_freebsd 0
#define Defined_android 0

#define Defined_PARANOIA 1
#define Defined_Allocator_PARANOIA 0

#define Defined_SUBNODE 0

#define Defined_NSA_APPROVED 0

#define Defined_Iface_OPTIMIZE 0

#define Defined_si_syscall 0

#define Defined_HAS_ETH_INTERFACE 1

#define Defined_Address_ROT64 1

#define Defined_NodeStore_whichIsWorse_PATHCOUNTS 0

#endif