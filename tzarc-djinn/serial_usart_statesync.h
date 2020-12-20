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

#ifdef SPLIT_SYNC_TYPE_KB

#    include "sync_state_kb.h"

// Synchronises keyboard shared state between master and slave.
// Any modifications by master should be made invoking the equivalent task.
// Any modifications made by slave will be made available to master after invoking the equivalent task.
void split_sync_kb(void);

// Getter for shared state for keyboard -- datatype needs to be defined in config.h
SPLIT_SYNC_TYPE_KB* get_split_sync_state_kb(void);

// Slave-side callback for when state has been received from master
// Any modifications made by the slave during this callback will propagate back to master.
void split_sync_on_receive_kb(SPLIT_SYNC_TYPE_KB* state);

#endif

#ifdef SPLIT_SYNC_TYPE_USER

#    include "sync_state_user.h"

// Synchronises user/keymap shared state between master and slave.
// Any modifications by master should be made invoking the equivalent task.
// Any modifications made by slave will be made available to master after invoking the equivalent task.
void split_sync_user(void);

// Getter for shared state for user/keymap -- datatype needs to be defined in config.h
SPLIT_SYNC_TYPE_USER* get_split_sync_state_user(void);

// Slave-side callback for when state has been received from master
// Any modifications made by the slave during this callback will propagate back to master.
void split_sync_on_receive_user(SPLIT_SYNC_TYPE_USER* state);

#endif
