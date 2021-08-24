#include <WiFi.h>
#include "meArm.h"

const char* ssid     = "MySID";         // SSID точки Wi-Fi
const char* password = "MySIDPassword"; // пароль точки Wi-Fi

meArm arm;

WiFiServer server (80);
// данные, поступающие с web-страницы
String header;
String valueString = String (14);
int pos1 = 0;
int pos2 = 0;
int pos3 = 0;
int pos4 = 0;
int pos5 = 0;
int pos6 = 0;
int posx = 20;
int posy = 125;
int posz = 0;
int posg = 0;

static void start_connection (void);

void setup()
{
  Serial.begin (9600);                      // открываем COM-порт 
  delay (10);
  
  start_connection ( );                     // запуск Wi-Fi 

  server.begin ( );                         // запуск Web-сервера

  // подключаем серводвигатели манипулятора
  // Base (центральный), Shoulder (правый), Elbow (левый), Gripper (захват)
  arm.begin (16, 17, 18, 19);
  delay (1000);  
  arm.gotoPoint (20, 130, 0); // исходное положение манипулятора
  
  delay (1000);  
  arm.openGripper ();
  delay (1000);
  arm.closeGripper ();
}

void loop ( )
{
  WiFiClient client = server.available ( ); // ожидание  соединения

  if (client)                               // если подключение есть,
  { 
    //Serial.println ("New Client.");       // отправить сообщение в COM-порт
    String currentLine = "";                // создать переменную типа String для сохранеиния данных подключения

    while (client.connected ( ))            // пока есть соединение,
    {
      if (client.available ( ))             // если есть данные,
      {
        char c = client.read ( );           // читать байт, а затем    
        //Serial.write (c);                 // отправить байт в COM-порт и
        header += c;                        // добавить байт в строку

        if (c == '\n')                      // если был перевод строки,
        {
          // если строка пустая, получаем два символа перевода строки подряд, т.е.
          // окончание клиентского запроса HTTP, посылаем ответ:
          if (currentLine.length ( ) == 0)
          {
            client.println ("HTTP/1.1 200 OK");
            client.println ("Content-type:text/html");
            client.println ("Connection: close");
            client.println ( );

            // далее идет сама web-страница:
            client.println ("<!DOCTYPE html><html>");
            client.println ("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println ("<meta charset=\"utf-8\">");
            client.println ("<link rel=\"icon\" href=\"data:,\">");
            client.println ("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto;}");
            client.println (".slider { height: 50px; width: 66%; }</style>");
            client.println ("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
                     
            client.println ("</head><body>");
            client.println ("<h3>Позиция X: <span id=\"xPos\"></span></h3>");
            client.println ("<input type=\"range\" min=\"-100\" max=\"100\" class=\"slider\" id=\"xSlider\" onchange=\"servo(xS.value,yS.value,zS.value,gS.value)\"/>");
            client.println ("<h3>Позиция Y: <span id=\"yPos\"></span></h3>");
            client.println ("<input type=\"range\" min=\"60\" max=\"200\" value=\"130\" class=\"slider\" id=\"ySlider\" onchange=\"servo(xS.value,yS.value,zS.value,gS.value)\"/>");
            client.println ("<h3>Позиция Z:  <span id=\"zPos\"></span></h3>");
            client.println ("<input type=\"range\" min=\"-60\"  max=\"60\"  class=\"slider\" id=\"zSlider\" onchange=\"servo(xS.value,yS.value,zS.value,gS.value)\"/>");
            client.println ("<h3>Захват   :  <span id=\"gPos\"></span></h3>");
            client.println ("<input type=\"range\" min=\"0\"  max=\"1\" value=\"0\" class=\"slider\" id=\"gSlider\" onchange=\"servo(xS.value,yS.value,zS.value,gS.value)\"/>");
           
            client.println ("<script>var xS = document.getElementById(\"xSlider\");");
            client.println ("var xP = document.getElementById(\"xPos\"); xP.innerHTML = xS.value;");
            
            client.println ("var yS = document.getElementById(\"ySlider\");");            
            client.println ("var yP = document.getElementById(\"yPos\"); yP.innerHTML = yS.value;");
            
            client.println ("var zS = document.getElementById(\"zSlider\");");
            client.println ("var zP = document.getElementById(\"zPos\"); zP.innerHTML = zS.value;");
            
            client.println ("var gS = document.getElementById(\"gSlider\");");
            client.println ("var gP = document.getElementById(\"gPos\"); gP.innerHTML = gS.value;");
            
            client.println ("xS.oninput = function() { xP.innerHTML = xS.value; }");
            client.println ("yS.oninput = function() { yP.innerHTML = yS.value; }");
            client.println ("zS.oninput = function() { zP.innerHTML = zS.value; }");
            client.println ("gS.oninput = function() { gP.innerHTML = gS.value; }");
            
            client.println ("$.ajaxSetup({timeout:1000}); function servo(pos_x,pos_y,pos_z,pos_g) { ");
            client.println ("$.get(\"/?value=\" + pos_x + \"X\" + pos_y + \"Y\" + pos_z + \"Z\" + pos_g + \"L&\"); {Connection: close};}</script>");
           
            client.println ("</body></html>");     
            
            // проверяем содержимое ответа после "/?value="
            if (header.indexOf ("GET /?value=") >= 0)
            {
              pos1 = header.indexOf ('='); // начало данных
              pos2 = header.indexOf ('X'); // последняя позиция координаты X 
              pos3 = header.indexOf ('Y'); // последняя позиция координаты Y
              pos4 = header.indexOf ('Z'); // последняя позиция координаты Z
              pos5 = header.indexOf ('L'); // состояние захвата (открыт,закрыт)
              pos6 = header.indexOf ('&'); // конец строки ответа
            
              valueString = header.substring (pos1+1, pos6);              
              Serial.println (valueString); // посылаем ответ в COM-порт для контроля
              
              valueString = header.substring (pos1+1, pos2);              
              posx = valueString.toInt ( ); // значение координаты X
              
              valueString = header.substring (pos2+1, pos3);              
              posy = valueString.toInt ( ); // значение координаты Y
              
              valueString = header.substring (pos3+1, pos4);              
              posz = valueString.toInt ( ); // значение координаты Z
              
              valueString = header.substring (pos4+1, pos5);              
              posg = valueString.toInt ( ); // состояние захвата
              
              arm.gotoPoint (posx, posy, posz);
              
              if (posg == 1) {arm.openGripper ();}
              else {arm.closeGripper ();}
              
            }
            client.println ( );             // окончание ответа HTTP
            break; 
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          currentLine += c;
        }
      }
    }
    header = ""; 

    client.stop ( );
    //Serial.println ("Client disconnected.");
    //Serial.println ("");
  }
}

void start_connection (void)
{
    // запуск соединения Wi-Fi
    Serial.println ( );
    Serial.println ( );
    Serial.print   ("Connecting to ");
    Serial.print   (ssid);
    Serial.print   (" ");    

    WiFi.begin(ssid, password);

    while (WiFi.status ( ) != WL_CONNECTED) 
    {
        delay(250);
        Serial.print (".");
    }

    Serial.println ("");
    Serial.println ("WiFi connected.");
    Serial.print   ("IP address: ");
    Serial.println (WiFi.localIP ( ));
}
