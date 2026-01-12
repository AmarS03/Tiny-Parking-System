idf_component_register(
    SRCS "https.c"
    INCLUDE_DIRS "."
    REQUIRES esp_http_client esp_netif esp-tls)