extern char* stereo_state;
extern float stereo_consumption;
extern float stereo_on_Consumption;
extern float stereo_off_Consumption;
float get_stereo();
char * get_stereo_state();
float get_stereo_consumption();
void change_stereo_state(int new_state);