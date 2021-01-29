extern float t_outside;
extern float t_inside;
extern float t_heater;
extern const float MAX_HEATER_TEMP;
extern const float START_HEATING_AT;
extern const float STOP_HEATING_AT;
extern const float HEATER_ON_CONSUMPTION;
extern const float HEATER_OFF_CONSUMPTION;
extern int heater_state;
extern int daytime_minutes;
void initialize_heater();
void update_temperature();
float get_temperature();
char * get_heater_state();
float get_heater_consumption();
void change_heater_state(int new_state);
void run_heater();