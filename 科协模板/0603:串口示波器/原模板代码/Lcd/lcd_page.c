#include "lcd_page.h"

static uint8_t  Touch_Scan              ();

/* 触屏扫描 */
/* 无论是否松开，tp_dev中都记录有最近一次的触摸坐标 */
static uint8_t Touch_Scan(){
    tp_dev.scan(0);  //扫描触摸屏
    if ((tp_dev.sta & 0xC0) == TP_CATH_PRES)     //若触摸屏被按下了一次且已松开
    {
        tp_dev.sta &= ~(TP_CATH_PRES);  //将[发生过按下事件]的标志清空
        return 1;  // 返回该按键已松开
    }
    elsefew
    return 0;  // 返回该键未松开并记录键值
}

///* 按键颜色修改并填充 */
//static void ButtonFill(ButtonTypedef* Button, u16 pos, u32 color)
//{
//    Button[pos].color = color;
//    LCD_Fill(Button[pos].sx + 1, Button[pos].sy + 1, Button[pos].sx + Button[pos].width - 1,
//             Button[pos].sy + Button[pos].height - 1, Button[pos].color);
//
//    POINT_COLOR = BLACK;
//    //写中文
////    Show_Str_Mid2(MainPage.button[pos].sx,MainPage.button[pos].sy,MainPage.button[pos].str,24,MainPage.button[pos].width,MainPage.button[pos].height);
//}
/*==========================================================================================================================*/
void            Method_LCD_Page_Change_Point_Color              (struct lcd_page_class* device, uint32_t color);
void            Method_LCD_Page_Add_Button                      (struct lcd_page_class* device, lcd_button * button, void(*function)());
void            Method_LCD_Page_Button_Touch_Scan               (struct lcd_page_class* device);
void            Method_LCD_Page_Add_Line                        (struct lcd_page_class* device,uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void            Method_LCD_Page_Add_Rectangle                   (struct lcd_page_class* device);
void            Method_LCD_Page_Change_Display_direction        (struct lcd_page_class* device, uint32_t dir);
/*==========================================================================================================================*/
/*******************************************显示部分*********************************************************************/

void Method_LCD_Page_Add_Line(struct lcd_page_class* device,uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
    /* USER CODE BEGIN */
    LCD_DrawLine(x1, y1, x2, y2);
    /* USER CODE END */
}

void Method_LCD_Page_Add_Rectangle(struct lcd_page_class* device){
    /* USER CODE BEGIN */

    /* USER CODE END */
}

/**********************************************************************************************************************/
void    Method_LCD_Page_Button_Show_Init        (struct lcd_page_class* device);
void    Method_LCD_Page_Refresh_Button          (struct lcd_page_class* device);

void Method_LCD_Page_Button_Show_Init(struct lcd_page_class* device){
    button_node* button_pn;
    if(device->button_head->next == NULL){
        return;
    }
    button_pn = device->button_head->next;
    for (int i = 1; i <= device->button_head->length; i++){
        LCD_DrawRectangle(button_pn->button->sx, button_pn->button->sy, button_pn->button->sx + button_pn->button->width,
                          button_pn->button->sy + button_pn->button->height);
        LCD_Fill(button_pn->button->sx + 1, button_pn->button->sy + 1, button_pn->button->sx + button_pn->button->width - 1,
                 button_pn->button->sy + button_pn->button->height - 1, button_pn->button->color);
        button_pn = button_pn->next;
//            Show_Str_Mid(MainPage.button[i].sx,MainPage.button[i].sy,MainPage.button[i].str,24,MainPage.button[i].width,MainPage.button[i].height);
    }
}

void Method_LCD_Page_Refresh_Button(struct lcd_page_class* device){
    button_node* button_pn;
    if(device->button_head->next == NULL){
        return;
    }
    button_pn = device->button_head->next;
    if(device->button_head->NowPress != 0){             // 当前按在有效位置
        for(int i = 1; i < device->button_head->NowPress; i++){
            button_pn = button_pn->next;
        }
        LCD_Fill(button_pn->button->sx + 1, button_pn->button->sy + 1, button_pn->button->sx + button_pn->button->width - 1,
                 button_pn->button->sy + button_pn->button->height - 1, LBBLUE);
    };
    if (device->button_head->LastPress != 0 && device->button_head->NowPress != device->button_head->LastPress){     // 上次按在有效位置且当前移开
        for(int i = 1; i < device->button_head->LastPress; i++){
            button_pn = button_pn->next;
        }
        LCD_Fill(button_pn->button->sx + 1, button_pn->button->sy + 1, button_pn->button->sx + button_pn->button->width - 1,
                 button_pn->button->sy + button_pn->button->height - 1, button_pn->button->color);
    }
    POINT_COLOR = BLACK;
}

void Method_LCD_Page_Touch_Function(struct lcd_page_class* device){
    button_node* button_pn;
    if(device->button_head->next == NULL || device->button_head->NowPress == 0){
        return;
    }
    button_pn = device->button_head->next;
    for(int i = 1; i < device->button_head->NowPress; i++){
        button_pn = button_pn->next;
    }
    button_pn->button->Function();
}

void Method_LCD_Page_Change_Point_Color(struct lcd_page_class* device, uint32_t color){
    /* USER CODE BEGIN */
    device->point_color = color;
    POINT_COLOR = device->point_color;
    /* USER CODE END */
}

void Method_LCD_Page_Change_Display_direction(struct lcd_page_class* device, uint32_t dir){
    /* USER CODE BEGIN */
    device->direction = dir;
    LCD_Display_Dir(device->direction);
    tp_dev.touchtype|=lcddev.dir&0X01;//横屏还是竖屏
    /* USER CODE END */
}

/**********************************************************************************************************************/
void Method_LCD_Page_Add_Button(struct lcd_page_class* device, lcd_button* button, void(*function)()){
    button_node* new_button = (button_node*)malloc(sizeof(button_node));     //  创建新的button节点
    button->Function   = function;
    new_button->button = button;
    new_button->next = NULL;

    if(device->button_head->next != NULL){                     // 如果头指针的next不为空，则遍历button链表
        button_node* button_pn = device->button_head->next;
        for(int i = 1; i < device->button_head->length; i++){
            button_pn = button_pn->next;
        }
        button_pn->next = new_button;
    }
    else{
        device->button_head->next = new_button;
    }
    device->button_head->length += 1;
    printf("Page成功添加一个Button!\r\n");
}

void Method_LCD_Page_Delete_Button(struct lcd_page_class* device, lcd_button* button){
    if(device->button_head->next == NULL){
        return;
    }
    button_node* button_pn = device->button_head->next;
    for(int i = 1; i <= device->button_head->length; i++){
        if(button_pn->next->button == button){
            LCD_Fill(button->sx, button->sy, button->sx+button->width, button->sy+button->height, device->back_color);
            if(button_pn->next != NULL) {
                button_pn->next = button_pn->next->next;
            }
        }
    }
}
/* page button scan */
void Method_LCD_Page_Button_Touch_Scan(struct lcd_page_class* device)
{
    device->button_head->ButtonUp = Touch_Scan();
    if(device->button_head->next == NULL){
        return;
    }
    button_node* button_pn = device->button_head->next;
    for (int i = 1; i <= device->button_head->length; i++){
        if (tp_dev.x[0] >= button_pn->button->sx && tp_dev.x[0] <= button_pn->button->sx + button_pn->button->width)
        {
            if (tp_dev.y[0] >= button_pn->button->sy && tp_dev.y[0] <= button_pn->button->sy + button_pn->button->height)
            {
                device->button_head->NowPress = i;
                return;
            }
        }
        button_pn = button_pn->next;
    }
    device->button_head->NowPress = 0;
}

/*===============================================================================================================================*/
void LCD_Page_Prepare(lcd_page* device, void(*refresh)(struct lcd_page_class* device)
        , void(show)(struct lcd_page_class* device)){
    device->button_head                 = (bhead*)malloc(sizeof(bhead));
    device->button_head->next           = NULL;
    device->button_head->NowPress       = 0;
    device->button_head->length         = 0;
    device->button_head->LastPress      = 0;

    device->point_color                 = BLACK;
    device->back_color                  = WHITE;
    device->direction                   = 0;
    device->UpdateFlag                  = 1;

    device->Add_Button                  = Method_LCD_Page_Add_Button;
    device->Add_Line                    = Method_LCD_Page_Add_Line;
    device->Change_Display_direction    = Method_LCD_Page_Change_Display_direction;

    device->Button_Show_Init            = Method_LCD_Page_Button_Show_Init;
    device->Refresh_Button              = Method_LCD_Page_Refresh_Button;
    device->Touch_Function              = Method_LCD_Page_Touch_Function;
    device->Touch_Scan                  = Method_LCD_Page_Button_Touch_Scan;
    device->Refresh                     = refresh;
    device->Show_Init                   = show;
    printf("成功创建一个Page!\r\n");
}