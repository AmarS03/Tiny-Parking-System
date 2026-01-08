idf_component_register(
  SRCS "init.c" "fsm.c" "main.c"
  INCLUDE_DIRS "."
  REQUIRES wifi weight ultrasonic esp_psram esp_idf_lib_helpers
  EMBED_FILES "mock_plate.jpg")
