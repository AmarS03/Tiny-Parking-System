idf_component_register(
  SRCS "init.c" "fsm.c" "main.c"
  INCLUDE_DIRS "."
  REQUIRES cv https wifi weight ultrasonic esp_psram esp_idf_lib_helpers esp_http_client esp-tls
  )
