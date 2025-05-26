#ifndef GEMINI_AI_H
#define GEMINI_AI_H

#include "server.h"

int generate_ai_response(const char * api_key,
                         const char * personality, const char * language_code, const char * conversation,
                         ai_response_t * response);

#endif /* GEMINI_H */
