/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *    Copyright 2014-2017 (c) Fraunhofer IOSB (Author: Julius Pfrommer)
 *    Copyright 2018 (c) Fraunhofer IOSB (Author: Lukas Meling)
 */

#ifndef UA_TYPES_ENCODING_JSON_H_
#define UA_TYPES_ENCODING_JSON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ua_util_internal.h"
#include "ua_types_encoding_binary.h"
#include "ua_types_encoding_json.h"
#include "ua_types.h"
#include "../deps/jsmn/jsmn.h"
#include "assert.h"
 
#define TOKENCOUNT 1000
    
size_t
UA_calcSizeJson(const void *src, const UA_DataType *type,
                UA_String *namespaces, size_t namespaceSize,
                UA_String *serverUris, size_t serverUriSize,
                UA_Boolean useReversible) UA_FUNC_ATTR_WARN_UNUSED_RESULT;


UA_StatusCode
UA_encodeJson(const void *src, const UA_DataType *type,
              uint8_t **bufPos, const uint8_t **bufEnd,
              UA_String *namespaces, size_t namespaceSize,
              UA_String *serverUris, size_t serverUriSize,
              UA_Boolean useReversible) UA_FUNC_ATTR_WARN_UNUSED_RESULT;

UA_StatusCode
UA_decodeJson(const UA_ByteString *src, void *dst,
              const UA_DataType *type) UA_FUNC_ATTR_WARN_UNUSED_RESULT;

/* Interal Definitions
 *
 * For future by the PubSub encoding */

#define UA_JSON_ENCODING_MAX_RECURSION 100
typedef struct {
    uint8_t *pos;
    const uint8_t *end;

    size_t namespacesSize;
    UA_String *namespaces;
    
    size_t serverUrisSize;
    UA_String *serverUris;
    
    uint16_t depth; /* How often did we en-/decoding recurse? */
    UA_Boolean useReversible;
    UA_Boolean commaNeeded[UA_JSON_ENCODING_MAX_RECURSION];
} CtxJson;

UA_StatusCode writeJsonObjStart(CtxJson *ctx);
UA_StatusCode writeJsonObjElm(CtxJson *ctx, UA_String *key,
                              const void *value, const UA_DataType *type);
UA_StatusCode writeJsonObjEnd(CtxJson *ctx);

UA_StatusCode writeJsonArrStart(CtxJson *ctx);
UA_StatusCode writeJsonArrElm(CtxJson *ctx, const void *value,
                              const UA_DataType *type);
UA_StatusCode writeJsonArrEnd(CtxJson *ctx);

UA_StatusCode writeJsonKey(CtxJson *ctx, const char* key);
UA_StatusCode writeJsonCommaIfNeeded(CtxJson *ctx);
UA_StatusCode writeJsonNull(CtxJson *ctx);

/* The encoding length is returned in ctx->pos */
UA_StatusCode calcJsonObjStart(CtxJson *ctx);
UA_StatusCode calcJsonObjElm(CtxJson *ctx, UA_String *key,
                             const void *value, const UA_DataType *type);
UA_StatusCode calcJsonObjEnd(CtxJson *ctx);

UA_StatusCode calcJsonArrStart(CtxJson *ctx);
UA_StatusCode calcJsonArrElm(CtxJson *ctx, const void *value,
                             const UA_DataType *type);
UA_StatusCode calcJsonArrEnd(CtxJson *ctx);

UA_StatusCode calcJsonKey(CtxJson *ctx, const char* key);
UA_StatusCode calcJsonCommaIfNeeded(CtxJson *ctx);
UA_StatusCode calcJsonNull(CtxJson *ctx);

typedef struct {
    jsmntok_t *tokenArray;
    UA_Int32 tokenCount;
    UA_UInt16 *index;
} ParseCtx;

typedef UA_StatusCode
(*encodeJsonSignature)(const void *src, const UA_DataType *type, CtxJson *ctx);

typedef UA_StatusCode
(*calcSizeJsonSignature)(const void *src, const UA_DataType *type, CtxJson *ctx);

typedef UA_StatusCode
(*decodeJsonSignature)(void *dst, const UA_DataType *type,
                       CtxJson *ctx, ParseCtx *parseCtx,
                       UA_Boolean moveToken);

/* Map for decoding a Json Object. An array of this is passed to the
 * decodeFields function. If the key "fieldName" is found in the json object
 * (mark as found and) decode the value with the "function" and write result
 * into "fieldPointer" (destination). */
typedef struct {
    const char * fieldName;
    void * fieldPointer;
    decodeJsonSignature function;
    UA_Boolean found;
} DecodeEntry;

UA_StatusCode
decodeFields(CtxJson *ctx, ParseCtx *parseCtx,
             DecodeEntry *entries, size_t entryCount,
             const UA_DataType *type);

UA_StatusCode
decodeJsonInternal(void *dst, const UA_DataType *type,
                   CtxJson *ctx, ParseCtx *parseCtx, UA_Boolean moveToken);

/* workaround: TODO generate functions for UA_xxx_decodeJson */
decodeJsonSignature getDecodeSignature(u8 index);
UA_StatusCode lookAheadForKey(const char* search, CtxJson *ctx, ParseCtx *parseCtx,
                              size_t *resultIndex);
jsmntype_t getJsmnType(const ParseCtx *parseCtx);
UA_StatusCode tokenize(ParseCtx *parseCtx, CtxJson *ctx,
                       const UA_ByteString *src, UA_UInt16 *tokenIndex);
UA_Boolean isJsonNull(const CtxJson *ctx, const ParseCtx *parseCtx);

#ifdef __cplusplus
}
#endif

#endif /* UA_TYPES_ENCODING_JSON_H_ */