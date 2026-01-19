idf_component_register(
    SRCS "cv.c"
    INCLUDE_DIRS "."
    REQUIRES esp_http_client esp-tls esp_netif cjson
    EMBED_FILES "mock_plate.jpg"
    PRIV_REQUIRES espressif__esp32-camera
)
