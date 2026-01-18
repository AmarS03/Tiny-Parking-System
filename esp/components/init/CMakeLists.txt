idf_component_register(
    SRCS "init.c"
    INCLUDE_DIRS "."
    REQUIRES esp_psram wifi cv ultrasonic weight https
    PRIV_REQUIRES espressif__esp32-camera
)