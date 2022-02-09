// 2022-02-09_Quick first design and color mock draft of the menu for M5Core2.
// Need to clean up and organize code better!!

#include <M5Core2.h>
int menu_id = 0;  // by standard, the MagicFocusEF is selected
int lens_calib_point = 0;

// Set delay after plotting the sprite
#define DELAY 1000

// Width and height of sprite
#define menu_top_WIDTH  320
#define menu_top_HEIGHT 50
#define menu_body_WIDTH 300
#define menu_upper_body_HEIGHT 40
#define menu_lower_body_HEIGHT 120

TFT_eSprite menu_top = TFT_eSprite(&M5.Lcd);
TFT_eSprite menu_upper_body = TFT_eSprite(&M5.Lcd);
TFT_eSprite menu_lower_body = TFT_eSprite(&M5.Lcd);

HotZone_t menu_mf(0,0,160,50); // Menu Autofocus
HotZone_t menu_calib(160,0,320,50); //Menu Calibration
HotZone_t FocusPointBtn(10,110,120,220);  // Focus Point Button
HotZone_t LensSelectBtn(10,60,310,100); // Lens Selction Menu
HotZone_t leftBtn(230,241,310,280); 
HotZone_t rightBtn(230,241,310,280);
    
void setup() {
  Serial.begin(115200);
      M5.begin();

      // Create a sprite of defined size
      menu_top.createSprite(menu_top_WIDTH, menu_top_HEIGHT);
      menu_upper_body.createSprite(menu_body_WIDTH, menu_upper_body_HEIGHT);
      menu_lower_body.createSprite(menu_body_WIDTH, menu_lower_body_HEIGHT);
      
      M5.Lcd.fillRect(0, 0, 160, 50, 0xEF7D);  // Menu Item "MagicFocusEF"
      M5.Lcd.fillRect(160, 0, 160, 50, 0x0000);  // Menu Item "Lens Calibration"
      M5.Lcd.fillRect(0, 50, 320, 190, 0x18E3);  // Menu Background
      M5.Lcd.setCursor(22,32);
      M5.Lcd.setFont(FSS9);  // FSS8, FSS9, FSS12, FSS18
      M5.Lcd.setTextColor(BLACK);
      M5.Lcd.print("MagicFocusEF");
      M5.Lcd.setCursor(198,32);
      M5.Lcd.setTextColor(WHITE);
      M5.Lcd.print("Calibration");
      //http://rinkydinkelectronics.com/calc_rgb565.php
}
    
void loop() {
      M5.update();

         if(M5.BtnC.pressedFor(3000))
          { 
            Serial.println("Shutdown");
          M5.shutdown();  // Shutdown the Core2. Startup when power button is pressed.
          }
       
        if( M5.Touch.ispressed())
        {
            TouchPoint_t point = M5.Touch.getPressPoint();
            if( menu_mf.inHotZone(point))
            {

            // check, if the menu is already selected and only draw menu, if selection changed.
            
            
              Serial.println("Menu_MF activated!");
              menu_top.fillRect(0, 0, 160, 50, WHITE);  // Menu Item "MagicFocusEF"
              menu_top.fillRect(160, 0, 160, 50, 0x0000);  // Menu Item "Lens Calibration"
              menu_top.setCursor(22,32);
              menu_top.setFont(FSS9);  // FSS8, FSS9, FSS12, FSS18
              menu_top.setTextColor(BLACK);
              menu_top.print("MagicFocusEF");
              menu_top.setCursor(198,32);
              menu_top.setTextColor(WHITE);
              menu_top.print("Calibration");
                            
              /* Populating the Menu Body*/
              menu_upper_body.fillRect(0, 0, 300, 40, 0x4229);  // Menu Item Lens List
              menu_upper_body.fillRect(0, 40, 300, 170, 0x18E3);  // space to next item

              menu_upper_body.setCursor(40,25);
              menu_upper_body.setFont(FSSB9); 
              menu_upper_body.setTextColor(WHITE);
              //menu_upper_body.printf("Canon EF 24-70 2.8 IS L");
              menu_upper_body.printf("Canon EF-S 17-55 f2.8 IS..");

              int x_3 = 20;
              int y_3 = 15;
              menu_upper_body.fillTriangle(0+x_3, 0+y_3, -5+x_3, 5+y_3, 0+x_3, 10+y_3, WHITE); 
              x_3 = 280;
              y_3 = 15;
              menu_upper_body.fillTriangle(0+x_3, 0+y_3, 5+x_3, 5+y_3, 0+x_3, 10+y_3, WHITE); 

              menu_lower_body.fillRect(0, 0, 300, 120, 0x18E3);  // Background
              menu_lower_body.setCursor(85,20);
              menu_lower_body.setFont(FSS9); 
              menu_lower_body.setTextColor(WHITE);
              menu_lower_body.println("Lidar:  0.532 m");
              menu_lower_body.setCursor(85,50);
              menu_lower_body.println("Lens:   0.532 m");
              
              menu_lower_body.fillRect(50, 70, 200, 50, 0x31C7); // Reset Focus Item
              menu_lower_body.setCursor(95,100);
              menu_lower_body.setFont(FSSB9); 
              menu_lower_body.setTextColor(0x7C31);
              menu_lower_body.println("Focus Reset");;

              menu_top.pushSprite(0, 0);
              menu_upper_body.pushSprite(10, 60);
              menu_lower_body.pushSprite(10, 110);
           
             
            }
            else if( menu_calib.inHotZone(point))
            {
              Serial.println("Menu_Calib activated!");
              menu_top.fillRect(0, 0, 160, 50, 0x0000);  // Menu Item "MagicFocusEF"
              menu_top.fillRect(160, 0, 160, 50, WHITE);  // Menu Item "Lens Calibration"
              menu_top.setCursor(22,32);
              menu_top.setFont(FSS9);  // FSS8, FSS9, FSS12, FSS18
              menu_top.setTextColor(WHITE);
              menu_top.print("MagicFocusEF");
              menu_top.setCursor(198,32);
              menu_top.setTextColor(BLACK);
              menu_top.print("Calibration");

              /* Populating the Menu Body*/

              menu_upper_body.fillRect(0, 0, 300, 40, 0x4229);  // Lens Selection Item
              menu_upper_body.setCursor(40,25);
              menu_upper_body.setFont(FSSB9); 
              menu_upper_body.setTextColor(WHITE);
              //menu_upper_body.printf("Canon EF 24-70 2.8 IS L");
              menu_upper_body.printf("Canon EF-S 17-55 f2.8 IS..");

              int x_3 = 20;
              int y_3 = 15;
              menu_upper_body.fillTriangle(0+x_3, 0+y_3, -5+x_3, 5+y_3, 0+x_3, 10+y_3, WHITE); 
              x_3 = 280;
              y_3 = 15;
              menu_upper_body.fillTriangle(0+x_3, 0+y_3, 5+x_3, 5+y_3, 0+x_3, 10+y_3, WHITE); 

              menu_lower_body.fillRect(0, 0, 300, 120, 0x18E3);  // Background
              menu_lower_body.fillRect(0, 0, 100, 80, 0x4229); // Calibration Point Item

              x_3 = 40;
              y_3 = 65;
              menu_lower_body.fillCircle(x_3, y_3, 4, WHITE);
              
              x_3 = 60;
              y_3 = 65;
              menu_lower_body.drawCircle(x_3, y_3, 4, WHITE);
              
              menu_lower_body.fillRect(110, 0, 55, 80, 0x4229); // Lens calibration Left Arrow Item
              menu_lower_body.fillRect(170, 0, 70, 80, 0x4229); // Lens Position
              menu_lower_body.fillRect(245, 0, 55, 80, 0x4229); // Lens calibration Right Arrow Item
              menu_lower_body.fillRect(0, 90, 100, 120, 0x4229);  // Menu Item Lens List
              menu_lower_body.fillRect(110, 90, 190, 120, 0x31C7);  // Menu Item Lens List

              menu_lower_body.setCursor(25,45);
              menu_lower_body.setFont(FSSB12);  // FSS8, FSS9, FSS12, FSS18
              menu_lower_body.setTextColor(WHITE);
              menu_lower_body.print("0.5m");
              menu_lower_body.setCursor(180,45);
              menu_lower_body.setTextColor(WHITE);
              menu_lower_body.print("126");

              menu_lower_body.setCursor(5,110);
              menu_lower_body.setFont(FSS9);  // FSS8, FSS9, FSS12, FSS18
              menu_lower_body.setTextColor(WHITE);
              menu_lower_body.print("Lidar: 0.55");
              menu_lower_body.setCursor(180,110);
              menu_lower_body.setTextColor(0x7C31);
              menu_lower_body.setFont(FSSB9);  // FSS8, FSS9, FSS12, FSS18
              menu_lower_body.print("Save");

              x_3 = 140;
              y_3 = 30;
              menu_lower_body.fillTriangle(0+x_3, 0+y_3, -10+x_3, 10+y_3, 0+x_3, 20+y_3, WHITE); 
              x_3 = 270;
              y_3 = 30;
              menu_lower_body.fillTriangle(0+x_3, 0+y_3, 10+x_3, 10+y_3, 0+x_3, 20+y_3, WHITE); 
              
              menu_top.pushSprite(0, 0);
              menu_upper_body.pushSprite(10, 60);
              menu_lower_body.pushSprite(10, 110);
              }

              else if (FocusPointBtn.inHotZone(point)) 
              {
                if (lens_calib_point == 0)
                {lens_calib_point = 1;}
                else 
                {lens_calib_point = 0;}
              }

              if (lens_calib_point == 1)
              {
              menu_lower_body.fillRect(0, 0, 100, 80, 0x4229); // Calibration Point Item
              menu_lower_body.setCursor(25,45);
              menu_lower_body.setFont(FSSB12);  // FSS8, FSS9, FSS12, FSS18
              menu_lower_body.setTextColor(WHITE);
              menu_lower_body.print("4 m");
              
              int x_3 = 40;
              int y_3 = 65;
              menu_lower_body.drawCircle(x_3, y_3, 4, WHITE);
              
              x_3 = 60;
              y_3 = 65;
              menu_lower_body.fillCircle(x_3, y_3, 4, WHITE);
              menu_lower_body.pushSprite(10, 110);
                }
                
        }
        delay(10);


}// end of main loop.
        
     
    
