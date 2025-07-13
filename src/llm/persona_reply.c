#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// http
#include <curl/curl.h>
#include <json-c/json.h>

// local
#include "../utils/helper.h"

#include "persona_reply.h"

// Callback for writing response data
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, APIResponse *response)
{
  size_t realsize = size * nmemb;
  char *ptr = realloc(response->data, response->size + realsize + 1);
  if (!ptr)
    return 0;

  response->data = ptr;
  memcpy(&(response->data[response->size]), contents, realsize);
  response->size += realsize;
  response->data[response->size] = 0;
  return realsize;
}

/**
 * !! You must free() the returned pointer !!
 */
static char *generate_system_prompt(const int max_tokens, const char *persona_name, const char *persona_description)
{
  const char *template =
      "You are a simulated persona named \"%s\", created by the user for emotional catharsis and comedic relief. "
      "Your job is to respond to unsent emails written by the user with exaggerated humor, sharp wit, and emotionally validating sarcasm.\n\n"
      "This persona has the following characteristics:\n\n"
      "%s\n\n"
      "Rules of engagement:\n"
      "- Always stay in character. Do not break the fourth wall.\n"
      "- Please be brief and reply only in one paragraph. No more. You have only about %d tokens to respond.\n"
      "- Do not provide real advice or apologies. Your job is to be a theatrical, over-the-top sounding board.\n"
      "- Your tone should match the persona's voice and purpose, blending dark humor, empathy, or satire as needed.\n"
      "- Never refer to yourself as an AI, chatbot, or assistant. You're just \"%s\".\n"
      "- The email you're replying to was never sent — it's a vent, not a conversation. Your response should feel like a reply, but be playfully inappropriate or overblown.\n\n"
      "Remember: This is part of a private local journaling app. Be bold. Be weird. Be cathartic.\n\n"
      "Now, read the user's unsent email below and reply in full character.";

  char *full_prompt = malloc(MAX_PROMPT_SIZE);
  if (!full_prompt)
  {
    fprintf(stderr, "Memory allocation failed.\n");
    return NULL;
  }

  snprintf(full_prompt, MAX_PROMPT_SIZE, template, persona_name, persona_description, max_tokens, persona_name);

  return full_prompt;
}

/**
 * Call LLm
 *
 * !! You must free() the returned pointer !!
 */
char *generate_ai_response(const char *scream_text, const char *persona_name, const char *persona_description)
{
  // check for user's LLM API Keys
  const char *api_key = getenv("LLM_API_KEY");
  const char *api_url = getenv("LLM_COMPLETION_URL");

  if (!api_key || !api_url)
  {
    puts("\n   [**WARNING**]  LLM API or URL environment variable not set. Your persona can not reply.");
    puts("                  Use the `-nr` flag if you explicitly don't want a reply from your persona!\n");
    return NULL;
  }

  CURL *curl;
  CURLcode res;
  APIResponse response = {0};
  struct curl_slist *headers = NULL;
  int maximum_tokens = 200;

  // Prepare JSON payload
  json_object *root = json_object_new_object();
  json_object *model = json_object_new_string("gpt-4");
  json_object *messages = json_object_new_array();
  json_object *message = json_object_new_object();
  json_object *role = json_object_new_string("system");

  // Create prompt
  char *prompt = generate_system_prompt(maximum_tokens, persona_name, persona_description);
  json_object *content = json_object_new_string(prompt);

  // max token and avoid unfinished replies
  json_object *max_tokens = json_object_new_int(maximum_tokens + 200);
  json_object *temperature = json_object_new_double(.7);

  json_object_object_add(message, "role", role);
  json_object_object_add(message, "content", content);
  json_object_array_add(messages, message);

  // add user email
  json_object *user_message = json_object_new_object();
  json_object *user_role = json_object_new_string("user");
  json_object *user_content = json_object_new_string(scream_text);

  json_object_object_add(user_message, "role", user_role);
  json_object_object_add(user_message, "content", user_content);
  json_object_array_add(messages, user_message);

  json_object_object_add(root, "model", model);
  json_object_object_add(root, "messages", messages);
  json_object_object_add(root, "max_tokens", max_tokens);
  json_object_object_add(root, "temperature", temperature);

  const char *json_string = json_object_to_json_string(root);

  free(prompt);

  curl = curl_easy_init();
  if (curl)
  {
    // Auth Key
    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);
    // more headers
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth_header);

    // api url
    char url_buffer[1024];
    snprintf(url_buffer, sizeof(url_buffer), "%s", api_url);
    curl_easy_setopt(curl, CURLOPT_URL, url_buffer);
    // more options
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_string);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
  }
  else
  {
    res = CURLE_FAILED_INIT;
  }

  json_object_put(root); // also free memory for any nested objects

  if (res == CURLE_OK && response.data)
  {
    // Parse response to extract content
    json_object *resp_root = json_tokener_parse(response.data);
    json_object *choices;
    if (json_object_object_get_ex(resp_root, "choices", &choices))
    {
      json_object *first_choice = json_object_array_get_idx(choices, 0);
      json_object *message_obj, *content_obj;
      if (json_object_object_get_ex(first_choice, "message", &message_obj) &&
          json_object_object_get_ex(message_obj, "content", &content_obj))
      {
        char *summary = strdup(json_object_get_string(content_obj));
        json_object_put(resp_root);
        free(response.data);
        return summary;
      }
    }
    json_object_put(resp_root);
  }

  if (response.data)
    free(response.data);

  return NULL;
}
