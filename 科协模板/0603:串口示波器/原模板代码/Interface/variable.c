
#include "variable.h"
#include "spi.h"

/**********************************************************************************************************************/
// 申请一个led
lcd base_lcd;

// 申请page
lcd_page page1;

// 申请button
lcd_button button1 = {200, 200, 200, 200, GREEN, "test",};
lcd_button button2 = {500, 300, 100, 100, RED, "test2"};
lcd_button button3 = {0,0, 100, 100, BLUE, 'test3'};

// 申请一个led
led led1;

// 申请一个adf4351
adf4351 adf4351_1;
/*==========================================函数定义====================================================================*/
/*******************************************LCD Page*******************************************************************/
void Page1_Init_Show(struct lcd_page_class* device);
void Page1_Refresh(struct lcd_page_class* device);
void Page1_Init_Show(struct lcd_page_class* device){
    // 填入界面初始的场景
    /* USER CODE BEGIN */
    POINT_COLOR = device->point_color;
    BACK_COLOR  = device->back_color;
    device->Change_Display_direction(device, 1);
    device->Add_Line(device, 1, 1, 200, 200);
    /* USER CODE END */
}

void Page1_Refresh(struct lcd_page_class* device) {
    // 填入你要运行的界面刷新程序
    /* USER CODE BEGIN */

    /* USER CODE END */
}

/*************************************************Button***************************************************************/
void Button1_Function();
void Button1_Function(){
    // 填入该界面按键的功能
    /* USER CODE BEGIN */
    printf("按下了第一个按键\r\n");
    /* USER CODE END */
}

void Button2_Function();
void Button2_Function(){
    /* USER CODE BEGIN */
    printf("按下了第二个按键\r\n");
    /* USER CODE END */
}

void Button3_Function(){
    printf("按下了第三个按键\r\n");
}
/*************************************************初始化函数*************************************************************/
void Variable_Init(){
    /********************LED***************************/
    New_LED(&led1, LED1_GPIO_Port, LED1_Pin);
    /**************************************************/

    /*******************LCD***************************/
    LCD_Prepare(&base_lcd);
    base_lcd.Init(&base_lcd);

    LCD_Page_Prepare(&page1, Page1_Refresh, Page1_Init_Show);  // 初始化新的Page
    page1.Add_Button(&page1, &button1, Button1_Function);
    page1.Add_Button(&page1, &button2, Button2_Function);
    page1.Add_Button(&page1, &button3, Button3_Function);
    base_lcd.Add_Page(&base_lcd, &page1);
    base_lcd.Show_Init(&base_lcd);
    /**************************************************/

    /*****************ADF4351**************************/
//    ADF4351_Prepare(&adf4351_1, HARDWARE, 0, 0, 0, &hspi3,0,0,0,0);
//    adf4351_1.Set_Output_Frequency(&adf4351_1, 150);
    /*************************************************/
    printf("初始化变量完成!\r\n");
}


