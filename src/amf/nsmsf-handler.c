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

#include "nsmsf-handler.h"

#include "sbi-path.h"
#include "nas-path.h"

int amf_nsmsf_sm_service_handle_activate(
        amf_ue_t *amf_ue, int state, ogs_sbi_message_t *recvmsg)
{
    int r;

    r = amf_ue_sbi_discover_and_send(
            OGS_SBI_SERVICE_TYPE_NPCF_AM_POLICY_CONTROL, NULL,
            amf_npcf_am_policy_control_build_create, amf_ue, state, NULL);
    ogs_expect(r == OGS_OK);
    ogs_assert(r != OGS_ERROR);
    return OGS_OK;
}

int amf_nsmsf_sm_service_handle_deactivate(
        amf_ue_t *amf_ue, ogs_sbi_message_t *recvmsg)
{
    return OGS_OK;
}

int amf_nsmsf_sm_service_handle_uplink_sms(
        amf_ue_t *amf_ue, ogs_sbi_message_t *recvmsg)
{
    return OGS_OK;
}
