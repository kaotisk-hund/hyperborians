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
#ifndef UniqueName_H
#define UniqueName_H

/**
 * Helpers for building identifiers that are unique per macro expansion.
 * UniqueName_MK(x) expands to x##__COUNTER__, so a macro that needs a
 * private declaration (and references to it) builds the name once:
 *
 *     #define FOO(...) FOO_impl(__VA_ARGS__, UniqueName_MK(foo_tmp))
 *
 * This replaces the historical JavaScript preprocessor which substituted
 * random hex strings.
 */
#define UniqueName_CAT(a,b) a##b
#define UniqueName_MK(name) UniqueName_CAT(name, __COUNTER__)

#endif
