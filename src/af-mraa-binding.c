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
#include <mraa.h>

static void version (struct afb_req request)
{
    json_object *jsrep = json_object_new_string (mraa_get_version());
    json_object *query = afb_req_json(request);
    afb_req_success_f(request, jsrep, 
                      "libmraa Version is %s, runnng on %s",
                      mraa_get_version(), mraa_get_platform_name(),
                      json_object_to_json_string(query));
}

/*
 * Verb Array
 */
static const struct afb_verb_v2 verbs[] = {
    { "version", version, NULL, AFB_SESSION_NONE },
    { "i2c", version, NULL, AFB_SESSION_NONE },
    { NULL }
};

static int preinit()
{
    AFB_NOTICE("mraa binding preinit (was register)");
    return 0;
}

static int init()
{
    AFB_NOTICE("mraa binding init");
    return 0;
}

static void onevent(const char *event, struct json_object *object)
{
    AFB_NOTICE("onevent %s", event);
}

/*
 * description of the binding for afb-daemon
 */
const struct afb_binding_v2 afbBindingV2 = {
    /* description conforms to VERSION 2 because VERSION 1 is OLD! */
    .api = "mraa",
    .specification = NULL,
    .verbs = verbs,
    .preinit = preinit,
    .init = init
};
