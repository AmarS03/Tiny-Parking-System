idf_component_register(
    SRCS "https_task.c" "https.c"
    INCLUDE_DIRS "."
    REQUIRES esp_http_client esp-tls esp_netif cjson
)
