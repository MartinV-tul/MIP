#include "q4.h"

#include "vv_lcd.h"
#include "vv_ui.h"

#include "q4_data.h"
#include "q4_config.h"

#include "mbed.h"
#include "EthernetInterface.h"
#include <cstdio>
#include <iterator>
#include <string>
#include "vv_time.h"
#include "SynchronizedTask.h"
#include "ThreadManager.h"


extern int LCD_SCREEN_MAX_X;
extern int LCD_SCREEN_MAX_Y;

DigitalOut  led(LED1);    
//DigitalIn   button(PI_11); // bez interuptu
InterruptIn button(PI_11); 
EthernetInterface eth;
TCPSocket socket;
SocketAddress a;
ThreadManager threadManager;

char ID1[] = "M20000177";
char ID2[] = "M20000178";


// DEBUG
#ifdef APP_SW_LCDCTRL
RectBtn lcd_btn[1];
Thread eth_thread;
#endif

#ifdef APP_SW_LCDCTRL
static void ShowReadyScreen(char* title, bool show_btn) {
#else
static void ShowReadyScreen(char* title) {
#endif
    LCD_Clear();
    LCD_DisplayString(0, LCD_SCREEN_MAX_Y / 2, LCD_COLOR_GREEN, CENTER_MODE, title);

#ifdef APP_SW_LCDCTRL
    if(show_btn)
        LCD_DrawButton(lcd_btn[0]);
#endif
}

static void ShowSentSubScreen() {

    static int id = 0;
    LCD_DisplayString(0, LCD_SCREEN_MAX_Y - 50, LCD_COLOR_GREEN, RIGHT_MODE, "by: t.id %d", id++);
}

void SendData(char field1[], u32 field2) {
    char message[50];
    snprintf(message, 50, "Sending data:%s,%d",field1,field2);

    #ifdef APP_SW_LCDCTRL
        ShowReadyScreen(message, true);
    #else
        ShowSentSubScreen(message);
    #endif
    char request[200];
    int size = snprintf(request, 200, "GET /update?api_key=WI0ZE8A9EL8N50TR&field1=%s&field2=%d HTTP/1.1\r\nHost: api.thingspeak.com\r\n\r\n",field1,field2);
    printf("%s", request);
    socket.send(request, size);
    char buffer[500];
    int ret = socket.recv(buffer, sizeof(buffer)-1);
    buffer[ret] = '\0';
    printf("Received %d chars from server:\n%s\n", ret, buffer);
}

void Send() {
    socket.open(&eth);
    socket.connect(a);
    static bool turn = false;
    turn = !turn;
    if (turn) {
        SendData(ID1, TIME_GetNow());
    }else {
        SendData(ID2, TIME_GetNow());
    }
    ThisThread::sleep_for(15100ms);
    ShowReadyScreen((char*)"Data send", true);
    ShowSentSubScreen();
    socket.close();
}

void ShowMessage() {
    printf("showing\n");
}

void BtnCallback() {
    SynchronizedTask* task = new SynchronizedTask(Send);
    threadManager.StartThread(task);
    static bool state = false;
    state = !state;
    led.write(state);
}


void Q4_Init() {
    eth.connect();
    eth.get_ip_address(&a);
    eth.gethostbyname("api.thingspeak.com", &a);
    a.set_port(80);
    LCD_Init(LCD_COLOR_BLACK);
#ifdef APP_SW_LCDCTRL
    // registrace tlacitka (LCD)
    lcd_btn[0] = UI_CreateRectBtn(10, 10, 80, 50, (char*)"nic", LCD_COLOR_WHITE, LCD_COLOR_RED);
#endif

#ifdef APP_SW_LCDCTRL
    ShowReadyScreen((char*)"Press BTN to send data", true);
#else
    ShowReadyScreen((char*)"Press BTN to send data");
#endif

}


void Q4_Exec() {

#ifdef APP_SW_LCDCTRL
    // check jestli neni stistknuto tlacitko na LCD
    if(LCD_GetButtonTouched(lcd_btn, 1) == 0) {

        BtnCallback(); // vola se i v interuptu
    }
#endif

    // debug na tlacitko
    //led.write(button.read()); 
}
