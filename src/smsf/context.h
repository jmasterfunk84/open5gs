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

#ifndef SMSF_CONTEXT_H
#define SMSF_CONTEXT_H

#include "ogs-app.h"
#include "ogs-sbi.h"

#include "smsf-sm.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int __smsf_log_domain;

#undef OGS_LOG_DOMAIN
#define OGS_LOG_DOMAIN __smsf_log_domain

typedef struct smsf_context_s {
    ogs_list_t      nsi_list; /* NSI List */
} smsf_context_t;

void smsf_context_init(void);
void smsf_context_final(void);
smsf_context_t *smsf_self(void);

int smsf_context_parse_config(void);

typedef struct smsf_nsi_s {
    ogs_lnode_t     lnode;

    char *nrf_id;
    char *nsi_id;

    ogs_s_nssai_t s_nssai;
} smsf_nsi_t;

smsf_nsi_t *smsf_nsi_add(char *nrf_id, uint8_t sst, ogs_uint24_t sd);
void smsf_nsi_remove(smsf_nsi_t *nsi);
void smsf_nsi_remove_all(void);
smsf_nsi_t *smsf_nsi_find_by_s_nssai(ogs_s_nssai_t *s_nssai);

int get_nsi_load(void);

#ifdef __cplusplus
}
#endif

#endif /* SMSF_CONTEXT_H */
