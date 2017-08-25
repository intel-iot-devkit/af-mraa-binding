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

#include "wrapjson/wrap-json.h"

//#include "uthash/utarray.h"
//struct _i2c afb_dd2c_contexts[10];
//UT_icd afb_i2c_contexts_icd = {sizeof(mraa_i2c_context), NULL, NULL, NULL};
//UT_array *afb_i2c_contexts;

mraa_i2c_context i2c_dev[10];

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
    mraa_result_t mraa_ret;
    json_object *mraa_resp = json_object_new_object();

    queryJ = afb_req_json(request);
    if (!queryJ) {
        afb_req_fail_f (request, "query-notjson","query=%s not a valid json entry", afb_req_value(request,""));
        return;
    }

    char* command;
    char* data;
    char* dataout;
    int intarg, intarg2, index, jwrap_ret;

    jwrap_ret = wrap_json_unpack(queryJ, "[si!]", &command, &index);
    if (jwrap_ret == 0) {
		if (strcmp(command, "mraa_i2c_read_byte") == 0) {
			int mraaread = mraa_i2c_read_byte(i2c_dev[index]);
			wrap_json_pack(&mraa_resp, "{s:i}", "mraachar", mraaread);
		}
		afb_req_success(request, mraa_resp, NULL);
		return;		
    }
 
    jwrap_ret = wrap_json_unpack(queryJ, "[sii!]", &command, &index, &intarg);
    if (jwrap_ret == 0) {
		if (strcmp(command, "mraa_i2c_address") == 0) {
			mraa_ret = mraa_i2c_address(i2c_dev[index], intarg);
			wrap_json_pack(&mraa_resp, "{s:i}", "mraa_result_t", mraa_ret);
		}
		else if (strcmp(command, "mraa_i2c_frequency") == 0) {
			mraa_ret = mraa_i2c_frequency(i2c_dev[index], (mraa_i2c_mode_t) intarg);
			wrap_json_pack(&mraa_resp, "{s:i}", "mraa_result_t", mraa_ret);
		}
		else if (strcmp(command, "mraa_i2c_read_byte_data") == 0) {
			int mraaread = mraa_i2c_read_byte_data(i2c_dev[index], intarg);
			wrap_json_pack(&mraa_resp, "{s:i, s:s}", "length", mraaread, "data", data);
		}
		else if (strcmp(command, "mraa_i2c_read_word_data") == 0) {
			int mraaread = mraa_i2c_read_word_data(i2c_dev[index], intarg);
			wrap_json_pack(&mraa_resp, "{s:i, s:s}", "length", mraaread, "data", data);
		}
		else if (strcmp(command, "mraa_i2c_write_byte") == 0) {
			mraa_ret = mraa_i2c_write_byte(i2c_dev[index], intarg);
			wrap_json_pack(&mraa_resp, "{s:i}", "mraa_result_t", mraa_ret);
		}
		afb_req_success(request, mraa_resp, NULL);
		return;		
    }
    
    jwrap_ret = wrap_json_unpack(queryJ, "[siii!]", &command, &index, &intarg, &intarg2);
	if (jwrap_ret == 0) {
		if (strcmp(command, "mraa_i2c_write_byte_data") == 0) {
			mraa_ret = mraa_i2c_write_byte_data(i2c_dev[index], intarg, intarg2);
			wrap_json_pack(&mraa_resp, "{s:i}", "mraa_result_t", mraa_ret);
		}
		else if (strcmp(command, "mraa_i2c_write_word_data") == 0) {
			mraa_ret = mraa_i2c_write_word_data(i2c_dev[index], intarg, intarg2);
			wrap_json_pack(&mraa_resp, "{s:i}", "mraa_result_t", mraa_ret);
		}
		afb_req_success(request, mraa_resp, NULL);
		return;		
	}
    
    jwrap_ret = wrap_json_unpack(queryJ, "[sisi!]", &command, &index, &data, &intarg);
    if (jwrap_ret == 0) {
		if (strcmp(command, "mraa_i2c_read") == 0) {
			dataout = (char*) calloc(intarg, sizeof(char));
			int mraaread = mraa_i2c_read(i2c_dev[index], dataout, intarg);
			wrap_json_pack(&mraa_resp, "{s:i, s:s}", "length", mraaread, "data", dataout);
		}
		else if (strcmp(command, "mraa_i2c_write") == 0) {
			dataout = (char*) calloc(intarg, sizeof(char));
			mraa_ret = mraa_i2c_write(i2c_dev[index], dataout, intarg);
			wrap_json_pack(&mraa_resp, "{s:i, s:s}", "mraa_result_t", mraa_ret, "data", dataout);
		}
		afb_req_success(request, mraa_resp, NULL);
		return;		
	}
}

static void
dev_init(struct afb_req request)
{
    struct json_object *queryJ;
    mraa_result_t res;
    json_object *mraa_resp = json_object_new_object();
    const char* initfunc;
    int index;

    queryJ = afb_req_json(request);
    if (!queryJ) {
        afb_req_fail_f (request, "query-notjson","query=%s not a valid json entry", afb_req_value(request,""));
        return;
    }

    printf("%s\n---\n", json_object_to_json_string_ext(queryJ, JSON_C_TO_STRING_PRETTY));
    int ret = wrap_json_unpack(queryJ, "[si]", &initfunc, &index);
    if (ret == 0) {
        if (strcmp(initfunc, "mraa_i2c_init") == 0) {
            i2c_dev[index] = mraa_i2c_init(index);
            if (i2c_dev == NULL) {
                afb_req_fail_f(request, "query-notvalid","mraa_i2c_init failed");
                return;
            }
            wrap_json_pack(&mraa_resp, "[I]", &i2c_dev);
            afb_req_success(request, mraa_resp, NULL);
            return;
        }
    }
    afb_req_fail_f(request, "query-notarray","query=%s not valid JSON mraa.io command", afb_req_value(request,""));
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

//    utarray_new(afb_i2c_contexts, &afb_i2c_contexts_icd);

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
