extern char* DryerMachine_state;
extern float DryerMachine_consumption;
extern float DryerMachine_on_Consumption;
extern float DryerMachine_off_Consumption;
float get_DryerMachine();
char * get_DryerMachine_state();
float get_DryerMachine_consumption();
void change_DryerMachine_state(int new_state);