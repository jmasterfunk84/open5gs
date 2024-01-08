/*
 * Copyright (C) 2019 by Sukchan Lee <acetcom@gmail.com>
 *
 * This file is part of Open5GS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SMSF_SMS_H
#define SMSF_SMS_H

#ifdef __cplusplus
extern "C" {
#endif

#define SMSF_SERVICE_MESSAGE_TYPE_CP_ACK            4

#define SMSF_PROTOCOL_DISCRIMINATOR_SMS             0x9

ogs_pkbuf_t *smsf_sms_encode_cp_ack(bool ti_flag, int ti_o);

#ifdef __cplusplus
}
#endif

#endif /* SMSF_SMS_H */