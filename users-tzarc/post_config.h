/* Copyright 2018-2020 Nick Brassel (@tzarc)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Disable old macros
#ifndef NO_ACTION_MACRO
#define NO_ACTION_MACRO
#endif  // NO_ACTION_MACRO

// Disable old functions
#ifndef NO_ACTION_FUNCTION
#define NO_ACTION_FUNCTION
#endif  // NO_ACTION_FUNCTION

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#ifdef LOCKING_SUPPORT_ENABLE
#undef LOCKING_SUPPORT_ENABLE
#endif  // LOCKING_SUPPORT_ENABLE

/* Locking resynchronize hack */
#ifdef LOCKING_RESYNC_ENABLE
#undef LOCKING_RESYNC_ENABLE
#endif  // LOCKING_RESYNC_ENABLE
