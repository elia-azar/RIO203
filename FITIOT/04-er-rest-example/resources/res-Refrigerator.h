extern char* Refrigerator_state ;
extern float Refrigerator_consumption;
extern float Refrigerator_on_Consumption;
extern float Refrigerator_off_Consumption;

char * get_Refrigerator_state();
float get_Refrigerator_consumption();
void change_Refrigerator_state(int new_state);
float get_Refrigerator();
void initialize_refrigerator();
void *refrigeratorThread(void *vargp);