/*
 * Copyright (C) 2019-2022 by Sukchan Lee <acetcom@gmail.com>
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

#include "sbi-path.h"
#include "nnrf-handler.h"

void ausf_nnrf_handle_nf_discover(
        ogs_sbi_xact_t *xact, ogs_sbi_message_t *recvmsg)
{
    ogs_sbi_object_t *sbi_object = NULL;
    OpenAPI_nf_type_e target_nf_type = 0;
    ogs_sbi_discovery_option_t *discovery_option = NULL;

    OpenAPI_search_result_t *SearchResult = NULL;

    ogs_assert(recvmsg);
    ogs_assert(xact);
    sbi_object = xact->sbi_object;
    ogs_assert(sbi_object);
    target_nf_type = xact->target_nf_type;
    ogs_assert(target_nf_type);

    discovery_option = xact->discovery_option;

    SearchResult = recvmsg->SearchResult;
    if (!SearchResult) {
        ogs_error("No SearchResult");
        return;
    }

    ogs_nnrf_handle_nf_discover_search_result(
            sbi_object, target_nf_type, discovery_option, SearchResult);

    ogs_sbi_select_nf(sbi_object, target_nf_type, discovery_option);

    ogs_expect(true == ausf_sbi_send_request(sbi_object, target_nf_type, xact));
}
