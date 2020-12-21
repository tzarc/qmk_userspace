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

#pragma once

//----------------------------------------------------------
// Runtime data sync -- keyboard

// Synchronises keyboard shared state between master and slave.
// Any modifications by master should be made invoking the equivalent task.
// Any modifications made by slave will be made available to master after invoking the equivalent task.
void split_sync_kb(bool force);

//---
// To be implemented by keyboard code:

// Get the keyboard shared state block of data
// The parameter should be filled in with the size of the data block.
// Return value is a pointer to the data block.
void* get_split_sync_state_kb(size_t* state_size);

// Keyboard level callback to update the data block.
// This is called for both master and slave, and both can fill in data accordingly.
// Return value is whether a forced update should occur -- true if so
bool split_sync_update_task_kb(void);

// Keyboard level callback to act upon the synchronised data block.
// The view of master and slave will match at this point.
void split_sync_action_task_kb(void);

//----------------------------------------------------------
// Runtime data sync -- user/keymap

// Synchronises user/keymap shared state between master and slave.
// Any modifications by master should be made invoking the equivalent task.
// Any modifications made by slave will be made available to master after invoking the equivalent task.
void split_sync_user(bool force);

//---
// To be implemented by user/keymap code:

// Get the user/keymap shared state block of data
// The parameter should be filled in with the size of the data block.
// Return value is a pointer to the data block.
void* get_split_sync_state_user(size_t* state_size);

// user/keymap level callback to update the data block.
// This is called for both master and slave, and both can fill in data accordingly.
// Return value is whether a forced update should occur -- true if so
bool split_sync_update_task_user(void);

// User/keymap level callback to act upon the synchronised data block.
// The view of master and slave will match at this point.
void split_sync_action_task_user(void);
