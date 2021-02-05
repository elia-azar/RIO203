extern char* Fan_state;
extern float Fan_consumption;
extern float Fan_on_Consumption;
extern float Fan_off_Consumption;
float get_Fan();
char * get_Fan_state();
float get_Fan_consumption();
void change_Fan_state(int new_state);