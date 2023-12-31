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
    ogs_list_t      smsf_ue_list;
    ogs_hash_t      *supi_hash;
} smsf_context_t;

struct smsf_ue_s {
    ogs_sbi_object_t sbi;
    ogs_fsm_t sm;

    /* where is this used??? */
    OpenAPI_smsf_registration_t *smsf_registration;
    OpenAPI_ue_sms_context_data_t *ue_sms_context_data;
    OpenAPI_sms_management_subscription_data_t *sms_management_subscription_data;

    char *ctx_id;
    char *supi;
    char *amf_instance_id;
    OpenAPI_access_type_e access_type;
    char *gpsi;
};

void smsf_context_init(void);
void smsf_context_final(void);
smsf_context_t *smsf_self(void);

int smsf_context_parse_config(void);

smsf_ue_t *smsf_ue_add(char *supi);
void smsf_ue_remove(smsf_ue_t *smsf_ue);
void smsf_ue_remove_all(void);
smsf_ue_t *smsf_ue_find_by_supi(char *supi);
smsf_ue_t *smsf_ue_find_by_gpsi(char *gpsi);

smsf_ue_t *smsf_ue_cycle(smsf_ue_t *smsf_ue);

int get_ue_load(void);

#ifdef __cplusplus
}
#endif

#endif /* SMSF_CONTEXT_H */
