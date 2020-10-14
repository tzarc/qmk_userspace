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

// Must only be use from the master side, initiates a request/response transaction from the slave
size_t serial_userxfer_transaction(const void* sendData, size_t sendLen, void* recvData, size_t recvLen);

// Must be implemented by the slave side (weak function) -- handles receiving arbitrary data from the master
__attribute__((weak)) bool serial_userxfer_receive(const void* data, size_t len);

// Must only be used from the slave side -- transmits responses back to the master
void serial_userxfer_respond(const void* data, size_t len);
