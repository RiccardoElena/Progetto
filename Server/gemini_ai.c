/*********************************************************************************
 * ===== FILE: gemini_ai.h/gemini_ai.c =====
 * Google Gemini AI integration for generating responses
 *********************************************************************************/

#include "gemini_ai.h"
#include "utils.h"

/**
 * @brief Structure for collecting HTTP response data
 */
typedef struct
{
  char *memory; // Response data buffer
  size_t size;  // Current size of data
} curl_response_t;

/**
 * @brief Callback function for cURL to write response data
 * This function is called by cURL when data is received from HTTP request
 * @param contents Pointer to received data
 * @param size Size of each data element
 * @param nmemb Number of data elements
 * @param response Pointer to our response structure
 * @return Number of bytes processed
 */
static size_t
my_curl_write_callback(
    void *contents,
    size_t size,
    size_t nmemb,
    curl_response_t *response)
{
  size_t realsize = size * nmemb;

  // Reallocate buffer to fit new data
  char *ptr = realloc(response->memory, response->size + realsize + 1);
  if (!ptr)
  {
#if SHOW_ERROR
    printf("[ERROR] Not enough memory for HTTP response (realloc returned NULL)\n");
#endif
    return (0); // Tell cURL we couldn't handle the data
  }

  // Copy new data to buffer
  response->memory = ptr;
  memcpy(&(response->memory[response->size]), contents, realsize);
  response->size += realsize;
  response->memory[response->size] = 0; // Null terminate

  return (realsize);
}

/**
 * @brief Generate complete Gemini API JSON request with personality and conversation history
 * Creates the full JSON structure needed for Gemini API calls
 * @param personality User's personality profile
 * @param language_code User's preferred language
 * @param conversation Already formatted JSON string for conversation history
 * @param json_output Output buffer for complete JSON request
 * @param output_size Size of output buffer
 * @return 0 on success, -1 on error
 */
static int
generate_gemini_request_json(
    const char *personality,
    const char *language,
    const char *conversation,
    char *json_output,
    size_t output_size)
{
  // Create the premise with language and personality vector
  char premise[1024];
  snprintf(premise, sizeof(premise),
           "You are a robot assistant (Furhat robot) designed to adapt to human personality. "
           "Respond in %s language. Here is your personality profile: %s\\n\\n"

           "Keep responses concise (1-3 sentences) and naturally conversational.",
           language, personality);

  // Escape quotes in the premise for JSON
  char escaped_premise[4096];
  const char *src = premise;
  char *dst = escaped_premise;
  while (*src && (unsigned long)(dst - escaped_premise) < sizeof(escaped_premise) - 2)
  {
    if (*src == '"')
    {
      *dst++ = '\\';
      *dst++ = '"';
    }
    else if (*src == '\\')
    {
      *dst++ = '\\';
      *dst++ = '\\';
    }
    else if (*src == '\n')
    {
      *dst++ = '\\';
      *dst++ = 'n';
    }
    else if (*src == '\r')
    {
      *dst++ = '\\';
      *dst++ = 'r';
    }
    else
    {
      *dst++ = *src;
    }
    src++;
  }
  *dst = '\0';

  // Check if we had to truncate
  if (*src != '\0')
  {
#if SHOW_WARNING
    printf("[WARNING] Premise was truncated during JSON escaping\n");
#endif
  }

  // Build the complete JSON request and keeping the length
  int json_len = snprintf(json_output, output_size,
                          "{"
                          "  \"generationConfig\": {"
                          "    \"temperature\": 0.9,"
                          "    \"maxOutputTokens\": 800,"
                          "    \"topP\": 1,"
                          "    \"topK\": 1"
                          "  },"
                          "  \"safetySettings\": ["
                          "    {"
                          "      \"category\": \"HARM_CATEGORY_HARASSMENT\","
                          "      \"threshold\": \"BLOCK_MEDIUM_AND_ABOVE\""
                          "    },"
                          "    {"
                          "      \"category\": \"HARM_CATEGORY_HATE_SPEECH\","
                          "      \"threshold\": \"BLOCK_MEDIUM_AND_ABOVE\""
                          "    },"
                          "    {"
                          "      \"category\": \"HARM_CATEGORY_SEXUALLY_EXPLICIT\","
                          "      \"threshold\": \"BLOCK_MEDIUM_AND_ABOVE\""
                          "    },"
                          "    {"
                          "      \"category\": \"HARM_CATEGORY_DANGEROUS_CONTENT\","
                          "      \"threshold\": \"BLOCK_MEDIUM_AND_ABOVE\""
                          "    }"
                          "  ],"
                          "  \"contents\": ["
                          "    {"
                          "      \"role\": \"user\","
                          "      \"parts\": ["
                          "        {"
                          "          \"text\": \"%s\""
                          "        }"
                          "      ]"
                          "    },"
                          "    %s"
                          "  ]"
                          "}",
                          escaped_premise, conversation);

  // Check if JSON was truncated
  if (json_len >= (int)output_size)
  {
#if SHOW_ERROR
    printf("[ERROR] JSON output truncated (%d bytes needed, %zu available)\n",
           json_len, output_size);
#endif
    return (-1);
  }

#if SHOW_DEBUG
  printf("[DEBUG] Generated JSON request size: %zu bytes\n", strlen(json_output));
#endif
  return (0);
}

/**
 * @brief Call Google Gemini AI API to generate response
 * Makes HTTP request to Gemini and parses the response
 * @param api_key Google Gemini API key
 * @param personality User's personality profile
 * @param language User's language preference
 * @param conversation Complete conversation including history and current message
 * @param response Output structure for AI response
 * @return 0 on success, -1 on error
 */
static int
call_gemini_api(
    const char *api_key,
    const char *personality,
    const char *language,
    const char *conversation,
    ai_response_t *response)
{
  CURL *curl;
  CURLcode res;
  curl_response_t api_response = {0};
  long http_code = 0; // Add HTTP status code checking

#if SHOW_DEBUG
  printf("[DEBUG] Calling Gemini AI API\n");
#endif
  // Initialize cURL
  curl = curl_easy_init();
  if (!curl)
  {
#if SHOW_ERROR
    printf("[ERROR] Failed to initialize cURL\n");
#endif
    return (-1);
  }

  // Generate the complete JSON request
  char json_request[MAX_MESSAGE_SIZE * 4];
  if (generate_gemini_request_json(personality, language, conversation,
                                   json_request, sizeof(json_request)) < 0)
  {
#if SHOW_ERROR
    printf("[ERROR] Failed to generate Gemini request JSON\n");
#endif
    curl_easy_cleanup(curl);
    return (-1);
  }
#if SHOW_DEBUG
  printf("[DEBUG] Gemini request JSON: %s\n", json_request);
#endif
  // Build request URL with API key
  char url[512];
  snprintf(url, sizeof(url), "%s?key=%s", GEMINI_API_URL, api_key);

  // Set HTTP headers
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");

  // Configure cURL request
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_request);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_curl_write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &api_response);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, AI_TIMEOUT_SECONDS);

  // Make the HTTP request
  res = curl_easy_perform(curl);

  // Get HTTP status code
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

  // Check for network/cURL errors first
  if (res != CURLE_OK)
  {
#if SHOW_ERROR
    printf("[ERROR] Gemini API request failed: %s\n", curl_easy_strerror(res));
#endif
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    if (api_response.memory)
      free(api_response.memory);
    return (-1);
  }

  // Check HTTP status code
  if (http_code != 200)
  {
#if SHOW_ERROR
    printf("[ERROR] Gemini API returned HTTP %ld\n", http_code);
#endif
    if (api_response.memory)
    {
#if SHOW_DEBUG
      printf("[DEBUG] Error response: %s\n", api_response.memory);
#endif
      // Try to parse error details from response
      json_object *error_root = json_tokener_parse(api_response.memory);
      if (error_root)
      {
        json_object *error_obj;
        if (json_object_object_get_ex(error_root, "error", &error_obj))
        {
          json_object *message_obj;
          if (json_object_object_get_ex(error_obj, "message", &message_obj))
          {
            const char *error_msg = json_object_get_string(message_obj);
#if SHOW_ERROR
            printf("[ERROR] API Error: %s\n", error_msg);
#endif

            // Provide specific error message to user
            if (http_code == 403)
            {
              safe_strncpy(response->response,
                           "API quota exceeded. Please check your billing settings.",
                           sizeof(response->response));
            }
            else if (http_code == 429)
            {
              safe_strncpy(response->response,
                           "API rate limit exceeded. Please try again later.",
                           sizeof(response->response));
            }
            else if (http_code == 401)
            {
              safe_strncpy(response->response,
                           "Invalid API key. Please check your configuration.",
                           sizeof(response->response));
            }
            else
            {
              snprintf(response->response, sizeof(response->response),
                       "API Error: %s", error_msg);
            }
          }
        }
        json_object_put(error_root);
      }
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    if (api_response.memory)
    {
      free(api_response.memory);
    }

    return (-1);
  }

  // HTTP 200 - Parse successful response
  if (api_response.memory)
  {
#if SHOW_DEBUG
    printf("[DEBUG] Gemini response received: %s\n", api_response.memory);
#endif
    // Parse JSON response
    json_object *resp_root = json_tokener_parse(api_response.memory);
    if (resp_root)
    {
      json_object *candidates;
      if (json_object_object_get_ex(resp_root, "candidates", &candidates))
      {
        json_object *first_candidate = json_object_array_get_idx(candidates, 0);
        if (first_candidate)
        {
          json_object *content;
          if (json_object_object_get_ex(first_candidate, "content", &content))
          {
            json_object *parts;
            if (json_object_object_get_ex(content, "parts", &parts))
            {
              json_object *first_part = json_object_array_get_idx(parts, 0);
              if (first_part)
              {
                json_object *text;
                if (json_object_object_get_ex(first_part, "text", &text))
                {
                  const char *ai_text = json_object_get_string(text);
                  safe_strncpy(response->response, ai_text, sizeof(response->response));
                  response->success = 1;
#if SHOW_INFO
                  printf("[INFO] Gemini response processed successfully\n");
                  printf("[INFO] Response: '%s'\n", response->response);
#endif
                }
                else
                {
#if SHOW_ERROR
                  printf("[ERROR] No 'text' field in response\n");
#endif
                }
              }
              else
              {
#if SHOW_ERROR
                printf("[ERROR] No parts in response\n");
#endif
              }
            }
            else
            {
#if SHOW_ERROR
              printf("[ERROR] No 'parts' field in response\n");
#endif
            }
          }
          else
          {
#if SHOW_ERROR
            printf("[ERROR] No 'content' field in response\n");
#endif
          }
        }
        else
        {
#if SHOW_ERROR
          printf("[ERROR] No candidates in response\n");
#endif
        }
      }
      else
      {
#if SHOW_ERROR
        printf("[ERROR] No 'candidates' field in response\n");
#endif
        // Could be an error response even with HTTP 200
        json_object *error_obj;
        if (json_object_object_get_ex(resp_root, "error", &error_obj))
        {
#if SHOW_ERROR
          printf("[ERROR] Response contains error field despite HTTP 200\n");
#endif
        }
      }
      json_object_put(resp_root);
    }
    else
    {
#if SHOW_ERROR
      printf("[ERROR] Failed to parse Gemini response JSON\n");
#endif
    }
  }

  // Cleanup
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  if (api_response.memory)
  {
    free(api_response.memory);
  }

  return response->success ? (0) : (-1);
}

/**
 * @brief Generate AI response based on personality, language, and conversation
 * Main function to get AI response with proper personality adaptation
 * @param api_key Gemini API key
 * @param personality User's personality profile
 * @param language User's language preference
 * @param conversation Complete conversation including history and current message
 * @param response Output structure for AI response
 * @return 0 on success, -1 on error
 */
int generate_ai_response(const char *api_key,
                         const char *personality,
                         const char *language,
                         const char *conversation,
                         ai_response_t *response)
{
  // Initialize response structure
  memset(response, 0, sizeof(ai_response_t));

  // Call Gemini API using our generate_gemini_request_json function
  return call_gemini_api(api_key, personality, language, conversation, response);
}
