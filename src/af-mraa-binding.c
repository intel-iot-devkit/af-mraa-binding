/*
 * Copyright (C) Intel Corp.
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define _GNU_SOURCE

#define AFB_BINDING_PRAGMA_NO_VERBOSE_UNPREFIX
#define AFB_BINDING_VERSION 2

#include <afb/afb-binding.h>

#include <json-c/json.h>
#include <string.h>

#include <mraa.h>

static void version(struct afb_req request)
{
    json_object *jsrep = json_object_new_string (mraa_get_version());
    json_object *query = afb_req_json(request);
    afb_req_success_f(request, jsrep,
                      "libmraa Version is %s, runnng on %s",
                      mraa_get_version(), mraa_get_platform_name(),
                      json_object_to_json_string(query));
}

static void
command(struct afb_req request)
{
    struct json_object *queryJ;
    mraa_result_t res;
    json_object *mraa_resp = json_object_new_object();

    queryJ = afb_req_json(request);
    if (!queryJ) {
        afb_req_fail_f (request, "query-notjson","query=%s not a valid json entry", afb_req_value(request,""));
        return;
    }

    enum json_type jtype= json_object_get_type(queryJ);
    switch (jtype) {
        case json_type_array:
            if (strcmp(json_object_get_string(json_object_array_get_idx(queryJ, 0)), "mraa_i2c_address")) {
                res = mraa_i2c_address((mraa_i2c_context) json_object_get_string(json_object_array_get_idx(queryJ, 1)), json_object_get_int(json_object_array_get_idx(queryJ, 2)));
                json_object_object_add(mraa_resp, "mraa_result_t", json_object_new_int(res));
            }
            break;
        default:
            afb_req_fail_f(request, "query-notarray","query=%s not valid JSON mraa.io command", afb_req_value(request,""));
            return;
    }

    afb_req_success(request, mraa_resp, NULL);
    return;
}

static void
dev_init(struct afb_req request)
{
    struct json_object *queryJ;
    mraa_result_t res;
    json_object *mraa_resp = json_object_new_object();
    const char* initfunc;

    queryJ = afb_req_json(request);
    if (!queryJ) {
        afb_req_fail_f (request, "query-notjson","query=%s not a valid json entry", afb_req_value(request,""));
        return;
    }

    printf("%s\n---\n", json_object_to_json_string_ext(queryJ, JSON_C_TO_STRING_PRETTY));

    enum json_type jtype= json_object_get_type(queryJ);
    switch (jtype) {
        case json_type_array:
            //for (int idx=0; idx < json_object_array_length(queryJ); idx ++) {
            // TODO: Make dynamic
            initfunc = json_object_get_string(json_object_array_get_idx(queryJ, 0));
            if (strcmp(initfunc, "mraa_i2c_init") == 0) {
                // TODO: Check arg is int and valid
                void* dev = mraa_i2c_init(json_object_get_int(json_object_array_get_idx(queryJ, 1)));
                if (dev == NULL) {
                    afb_req_fail_f(request, "query-notvalid","mraa_i2c_init failed");
                    return;
                }
                json_object_object_add(mraa_resp, "dev", json_object_new_string(dev));

                printf("%s\n---\n", json_object_to_json_string_ext(mraa_resp, JSON_C_TO_STRING_PRETTY));

            }
            break;
        default:
            afb_req_fail_f(request, "query-notarray","query=%s not valid JSON mraa.io command", afb_req_value(request,""));
            return;
    }

    afb_req_success(request, mraa_resp, NULL);
    return;
}

static void
dev_init_raw(struct afb_req request)
{
    return;
}

/*
 * Verb Array
 */
static const struct
afb_verb_v2 verbs[] = {
    { "version", version, NULL, AFB_SESSION_NONE },
    { "dev-init", dev_init, NULL, AFB_SESSION_NONE },
    { "dev-init-raw", dev_init_raw, NULL, AFB_SESSION_NONE },
    { "command", command, NULL, AFB_SESSION_NONE },
    { NULL }
};

static int
preinit()
{
    AFB_NOTICE("mraa binding preinit (was register)");
    return 0;
}

static int
init()
{
    AFB_NOTICE("mraa binding init");
    // TODO: Check return value and do something intelligent here
    mraa_init();
    return 0;
}

static void
onevent(const char *event, struct json_object *object)
{
    AFB_NOTICE("onevent %s", event);
}

/*
 * description of the binding for afb-daemon
 */
const struct
afb_binding_v2 afbBindingV2 = {
    /* description conforms to VERSION 2 because VERSION 1 is OLD! */
    .api = "mraa",
    .specification = NULL,
    .verbs = verbs,
    .preinit = preinit,
    .init = init
};
