idf_component_register(
  SRCS "fsm.c" "main.c"
  INCLUDE_DIRS "."
  REQUIRES cv https wifi weight init ultrasonic_sensor servo_motor
  )
