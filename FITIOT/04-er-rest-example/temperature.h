#define DEBUG 1
#define min(X,Y) (((X) < (Y)) ? (X) : (Y))
#define max(X,Y) (((X) > (Y)) ? (X) : (Y))
extern float t_outside;
extern float t_inside;
extern float t_heater;
extern int heater_state;
extern int daytime_minutes;
void initialize();
void update_temperature();
float get_temperature();
char * get_state();
float get_consumption();
void change_state(int new_state);
void run();