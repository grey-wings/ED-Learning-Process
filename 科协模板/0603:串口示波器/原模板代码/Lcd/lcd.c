/****************************************************************
LCD相关函数
****************************************************************/

#include "lcd.h"
#include "touch.h"

/*=============================================函数声明=================================================================*/
void Method_LCD_Init(lcd* device);
void Method_LCD_Show_Init(struct lcd_class* device);
void Method_LCD_Run       (struct lcd_class* device);
void Method_LCD_Add_Page(lcd* device, lcd_page* page);
/*==============================================函数定义================================================================*/
void Method_LCD_Init(lcd* device){
/* USER CODE BEGIN */
TFTLCD_Init();
TP_Init();
/* USER CODE END */
}

void Method_LCD_Run(struct lcd_class* device){
/* USER CODE BEGIN */
if(device->now_page->UpdateFlag == 1){
device->now_page->UpdateFlag = 0;
device->now_page->Show_Init(device->now_page);
device->now_page->Button_Show_Init(device->now_page);
}
device->now_page->Refresh(device->now_page);            // 先刷新界面
device->now_page->Touch_Scan(device->now_page);         // 按键扫描
device->now_page->Refresh_Button(device->now_page);     // 刷新按键界面

device->now_page->button_head->LastPress = device->now_page->button_head->NowPress;
if (device->now_page->button_head->ButtonUp){
device->now_page->Touch_Function(device->now_page);         // 执行按键的操作
device->now_page->button_head->ButtonUp = 0;
}
/* USER CODE END */
}

void Method_LCD_Show_Init(struct lcd_class* device){
/* USER CODE BEGIN */
device->now_page->Show_Init(device->now_page);
/* USER CODE END */
}

void Method_LCD_Refresh_Button(struct lcd_class* device){
/* USER CODE BEGIN */
device->now_page->Refresh_Button(device->now_page);
/* USER CODE END */
}
/***********************************************************************************************************************/

void Method_LCD_Add_Page(lcd* device, lcd_page* page){
page_node *new_page = (page_node*)malloc(sizeof(page_node));     //  创建新的button节点
new_page->page = page;
new_page->next = NULL;

if(device->page_head->next != NULL){                     // 如果头指针的next不为空，则遍历button链表
page_node* page_pn = device->page_head->next;
for(int i = 1; i < device->page_head->length; i++){
page_pn = page_pn->next;
}
page_pn->next = new_page;
}
else{
device->page_head->next = new_page;
}
device->page_head->length += 1;
if(device->now_page == NULL){       // 如果目前的page是空的，直接将添加的页数赋值
device->now_page = page;
}
printf("LCD成功添加一个Page!\r\n");
}


/*====================================================================================================================*/
void LCD_Prepare(lcd* device){
device->page_head           = (phead*)malloc(sizeof(phead));
device->page_head->next     = NULL;
device->page_head->length   = 0;

device->Init                = Method_LCD_Init;
device->Add_Page            = Method_LCD_Add_Page;
device->Show_Init           = Method_LCD_Show_Init;
device->Run                 = Method_LCD_Run;
device->Refresh_Button      = Method_LCD_Refresh_Button;
printf("成功创建一个LCD!\r\n");
}


///* 主界面功能 */
//void MainPageProcess(void)
//{
//    MainPage.LastPress = MainPage.NowPress;
//    if (MainPage.ButtonUp)
//    {
//
//        Touch_Function();
//        MainPage.ButtonUp = 0;
//    }
//
//}
//
///*=========================================================================================================================*/
///* 触屏扫描 */
///* 无论是否松开，tp_dev中都记录有最近一次的触摸坐标 */
//static u8 TouchScan(void)
//{
//    tp_dev.scan(0);  //扫描触摸屏
//    if ((tp_dev.sta & 0xC0) == TP_CATH_PRES)     //若触摸屏被按下了一次且已松开
//    {
//        tp_dev.sta &= ~(TP_CATH_PRES);  //将[发生过按下事件]的标志清空
//        return 1;  // 返回该按键已松开
//    }
//    else
//        return 0;  // 返回该键未松开并记录键值
//}
//
///* 按键样式处理 */
//static void ButtonStyleProcess(ButtonTypedef* Button, u16 NowPress, u16 LastPress)
//{
//    if (NowPress != 0)  // 当前按在有效位置
//        LCD_Fill(Button[NowPress].sx + 1, Button[NowPress].sy + 1, Button[NowPress].sx + Button[NowPress].width - 1,
//                 Button[NowPress].sy + Button[NowPress].height - 1, LBBLUE);
//    if (LastPress != 0 && NowPress != LastPress)  // 上次按在有效位置且当前移开
//        LCD_Fill(Button[LastPress].sx + 1, Button[LastPress].sy + 1, Button[LastPress].sx + Button[LastPress].width - 1,
//                 Button[LastPress].sy + Button[LastPress].height - 1, Button[LastPress].color);
//
//    POINT_COLOR = BLACK;
//    //写中文
//    // Show_Str_Mid2(MainPage.button[NowPress].sx,MainPage.button[NowPress].sy,MainPage.button[NowPress].str,24,MainPage.button[NowPress].width,MainPage.button[NowPress].height);
//    Show_Str_Mid2(MainPage.button[LastPress].sx,MainPage.button[LastPress].sy,MainPage.button[LastPress].str,24,MainPage.button[LastPress].width,MainPage.button[LastPress].height);
//}
//
///* 按键颜色修改并填充 */
//static void ButtonFill(ButtonTypedef* Button, u16 pos, u32 color)
//{
//    Button[pos].color = color;
//    LCD_Fill(Button[pos].sx + 1, Button[pos].sy + 1, Button[pos].sx + Button[pos].width - 1,
//             Button[pos].sy + Button[pos].height - 1, Button[pos].color);
//
//    POINT_COLOR = BLACK;
//    //写中文
//    Show_Str_Mid2(MainPage.button[pos].sx,MainPage.button[pos].sy,MainPage.button[pos].str,24,MainPage.button[pos].width,MainPage.button[pos].height);
//}
//
///* 主界面显示 */
//void MainPageShow(void)
//{
//
//    if (MainPage.UpdateFlag)
//    {
//        MainPage.UpdateFlag = 0;
//        LCD_Clear(WHITE);
//        int i;
//        for (i = 1; i <= MainPage.Button_i-1; i++)  // 画按键
//        {
//            LCD_DrawRectangle(MainPage.button[i].sx, MainPage.button[i].sy, MainPage.button[i].sx + MainPage.button[i].width,
//                              MainPage.button[i].sy + MainPage.button[i].height);
//            LCD_Fill(MainPage.button[i].sx + 1, MainPage.button[i].sy + 1, MainPage.button[i].sx + MainPage.button[i].width - 1,
//                     MainPage.button[i].sy + MainPage.button[i].height - 1, MainPage.button[i].color);
//
//            //写中文
//            Show_Str_Mid2(MainPage.button[i].sx,MainPage.button[i].sy,MainPage.button[i].str,24,MainPage.button[i].width,MainPage.button[i].height);
//        }
//        LCD_DrawRectangle(MainPage.spectrumsx, MainPage.spectrumsy, MainPage.spectrumex, MainPage.spectrumey); // 频谱窗
//    }
//    ButtonStyleProcess(MainPage.button, MainPage.NowPress, MainPage.LastPress);
//
//}
//
///* 主界面按键扫描 */
//void MainPageButtonScan(void)
//{
//    MainPage.ButtonUp = TouchScan();
//    for (int i = 1; i <= MainPage.Button_i-1; i++)
//    {
//        if (tp_dev.x[0] >= MainPage.button[i].sx && tp_dev.x[0] <= MainPage.button[i].sx + MainPage.button[i].width)
//        {
//            if (tp_dev.y[0] >= MainPage.button[i].sy && tp_dev.y[0] <= MainPage.button[i].sy + MainPage.button[i].height)
//            {
//                MainPage.NowPress = i;
//                return;
//            }
//        }
//    }
//    MainPage.NowPress = 0;
//}
//
//void Touch_Function()
//{
//    static u8 circuit_x = 0;
//    if(MainPage.OutSta == 0)                                                        //如果有按键按下
//    {
//        switch(MainPage.NowPress)
//        {
//            case circuit1:printf("正常\r\n");	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_1,GPIO_PIN_SET);
//                break;
//
//            case circuit2:printf("顶部\r\n");HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
//                break;
//
//            case circuit3:printf("底部\r\n");	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_6,GPIO_PIN_SET);
//                break;
//
//            case circuit4:printf("双向\r\n");	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_0,GPIO_PIN_SET);
//                break;
//
//            case circuit5:printf("交越\r\n");	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_2,GPIO_PIN_SET);
//                break;
//        }
//        //特殊几个按键进行处理
//        if(MainPage.NowPress&&circuit_x != MainPage.NowPress && MainPage.NowPress <= 5)
//        {
//            ButtonFill(MainPage.button, MainPage.NowPress, RED);
//            MainPage.OutSta = 1;
//            circuit_x = MainPage.NowPress;
//
//        }
//    }
//    else if(circuit_x == MainPage.NowPress)
//    {
//        HAL_GPIO_WritePin(GPIOE,GPIO_PIN_1,GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(GPIOI,GPIO_PIN_6,GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(GPIOI,GPIO_PIN_0,GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(GPIOI,GPIO_PIN_2,GPIO_PIN_RESET);
//        switch(MainPage.NowPress)
//        {
//            case circuit1:printf("正常关闭!\r\n");	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_1,GPIO_PIN_RESET);
//                break;
//
//            case circuit2:printf("顶部关闭！\r\n");HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
//                break;
//
//            case circuit3:printf("底部关闭!\r\n");	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_6,GPIO_PIN_RESET);
//                break;
//
//            case circuit4:printf("双向关闭!\r\n");	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_0,GPIO_PIN_RESET);
//                break;
//
//            case circuit5:printf("交越关闭!\r\n");	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_2,GPIO_PIN_RESET);
//                break;
//        }
//        if(MainPage.NowPress)
//        {
//            ButtonFill(MainPage.button, MainPage.NowPress, LGRAY);
//            MainPage.OutSta = 0;
//            circuit_x = 0;
//        }
//    }
//
//    //处理其他的函数
//    switch(MainPage.NowPress)
//    {
//        case waveform:printf("打开波形图! \r\n");MainPage.Ui_Sta =2;
//            break;
//
//        case spectrum:  printf("打开频谱图! \r\n");MainPage.Ui_Sta =1;
//            break;
//
//        case single_sample:
//            if(MainPage.Single_Sta == 1)
//            {
//                printf("单次采样关闭!\r\n");
//                MainPage.Single_Sta = 0;
//                delay_ms(200);
//                HAL_TIM_Base_Start_IT(&htim3);
//            }
//            else
//            {
//                MainPage.Single_Sta = 1;
//                printf("单次采样!\r\n");
//            }
//            break;
//
//        case seventh_hormoic:
//            if(MainPage.Seventh_Sta == 1)
//            {
//                printf("七次谐波计算结束!\r\n");
//                MainPage.Seventh_Sta = 0;
//            }
//
//            else
//            {
//                printf("七次谐波计算!\r\n");
//                MainPage.Seventh_Sta = 1;
//            }
//
//            break;
//        case sample:
//            if(MainPage.Single_Sta ==1)
//            {
//                printf("采样!\r\n");
//                HAL_TIM_Base_Start_IT(&htim3);
//            }
//            break;
//    }
//
//}

