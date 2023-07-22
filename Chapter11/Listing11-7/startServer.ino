void startServer()
{
  httpd_handle_t esp32_httpd = NULL;    // define esp32_httpd and
  httpd_handle_t stream_httpd = NULL;   // stream_httpd handlers
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  httpd_uri_t page_uri =        // link / URL  to page_handler
{.uri="/", .method = HTTP_GET, .handler = page_handler, .user_ctx = NULL};
  httpd_uri_t stream_uri =        // link /stream to stream_handler
{.uri="/stream", .method=HTTP_GET, .handler=stream_handler,.user_ctx=NULL};
  if (httpd_start(&esp32_httpd, &config) == ESP_OK)
      httpd_register_uri_handler(esp32_httpd, &page_uri);
  config.server_port = 81;
  config.ctrl_port = config.ctrl_port + 1;    // required for streaming
  if (httpd_start(&stream_httpd, &config) == ESP_OK)
    httpd_register_uri_handler(stream_httpd, &stream_uri);
}
