/** @file c_plus/webrequest.c  HTTP(S)/FTP requests (using CURL).

@authors Copyright (c) 2018 Jaakko Keränen <jaakko.keranen@iki.fi>

@par License

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

<small>THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.</small>
*/

#include "c_plus/webrequest.h"
#include "c_plus/stringarray.h"
#include <curl/curl.h>

struct Impl_WebRequest {
    iObject object;
    CURL *curl;
    iBlock postData;
    iString postContentType;
    iBlock result;
    iString errorMessage;
    iStringArray *headers;
};

static size_t headerCallback_WebRequest_(char *ptr, size_t size, size_t nmemb, void *userdata) {
    iWebRequest *d = (iWebRequest *) userdata;
    iUnused(d);
    const size_t len = size * nmemb;
    iString *hdr = newCStrN_String(ptr, len);
    trim_String(hdr);
    if (!isEmpty_String(hdr)) {
        printf("Header received: `%s`\n", cstr_String(hdr));
        pushBack_StringArray(d->headers, hdr);
    }
    delete_String(hdr);
    return len;
}

static size_t dataCallback_WebRequest_(char *ptr, size_t size, size_t nmemb, void *userdata) {
    iWebRequest *d = (iWebRequest *) userdata;
    const size_t len = size * nmemb;
    appendData_Block(&d->result, ptr, len);
    return len;
}

void init_WebRequest(iWebRequest *d) {
    d->curl = curl_easy_init();
    init_Block(&d->postData, 0);
    init_String(&d->postContentType);
    init_Block(&d->result, 0);
    init_String(&d->errorMessage);
    d->headers = new_StringArray();
    curl_easy_setopt(d->curl, CURLOPT_HEADERFUNCTION, headerCallback_WebRequest_);
    curl_easy_setopt(d->curl, CURLOPT_HEADERDATA, d);
    curl_easy_setopt(d->curl, CURLOPT_WRITEFUNCTION, dataCallback_WebRequest_);
    curl_easy_setopt(d->curl, CURLOPT_WRITEDATA, d);
}

void deinit_WebRequest(iWebRequest *d) {
    iRelease(d->headers);
    deinit_String(&d->errorMessage);
    deinit_Block(&d->result);
    deinit_String(&d->postContentType);
    deinit_Block(&d->postData);
    curl_easy_cleanup(d->curl);
}

iDefineClass(WebRequest)
iDefineObjectConstruction(WebRequest)

void clear_WebRequest(iWebRequest *d) {
    curl_easy_reset(d->curl);
    clear_Block(&d->postData);
    clear_Block(&d->result);
    clear_String(&d->errorMessage);
    clear_StringArray(d->headers);
}

void setUrl_WebRequest(iWebRequest *d, const iString *url) {
    curl_easy_setopt(d->curl, CURLOPT_URL, cstr_String(url));
}

void setUserAgent_WebRequest(iWebRequest *d, const iString *userAgent) {
    curl_easy_setopt(d->curl, CURLOPT_USERAGENT, cstr_String(userAgent));
}

void setPostData_WebRequest(iWebRequest *d, const char *contentType, const iBlock *data) {
    set_Block(&d->postData, data);
    format_String(&d->postContentType, "Content-Type: %s", contentType);
}

static iBool execute_WebRequest_(iWebRequest *d) {
    char errorMsg[CURL_ERROR_SIZE];
    clear_String(&d->errorMessage);
    clear_Block(&d->result);
    clear_StringArray(d->headers);
    curl_easy_setopt(d->curl, CURLOPT_ERRORBUFFER, errorMsg);
    const iBool ok = (curl_easy_perform(d->curl) == CURLE_OK);
    if (!ok) {
        setCStr_String(&d->errorMessage, errorMsg);
    }
    return ok;
}

iBool get_WebRequest(iWebRequest *d) {
    return execute_WebRequest_(d);
}

iBool post_WebRequest(iWebRequest *d) {
    struct curl_slist *headers = curl_slist_append(NULL, cstr_String(&d->postContentType));
    curl_easy_setopt(d->curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(d->curl, CURLOPT_POSTFIELDS, data_Block(&d->postData));
    curl_easy_setopt(d->curl, CURLOPT_POSTFIELDSIZE, size_Block(&d->postData));
    const iBool ok = execute_WebRequest_(d);
    curl_slist_free_all(headers);
    return ok;
}

const iBlock *result_WebRequest(const iWebRequest *d) {
    return &d->result;
}

const iStringArray *headers_WebRequest(const iWebRequest *d) {
    return d->headers;
}

const iString *errorMessage_WebRequest(const iWebRequest *d) {
    return &d->errorMessage;
}
