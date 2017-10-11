//8266 AP +ws 1.5号机 wifi ssid=ESP8266xxxxxxxx pwd=ap20171008 访问 http://192.168.128.1/pe15a.html 可见网页  
//主页面使用SPIFFS文件保存 上存flash文件工具ESP8266FS-0.3.0(所在位置D:\arduino-1.6.12\tools\ESP8266FS\tool\esp8266fs.jar) 目前只在1.6.12版本中成功上存文件，Flash Size:"4M(1M SPIFFS)" ，数据文件放在工程目录下/data/...文件夹里
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>      //文件系统库SPIFFS 

ESP8266WebServer server ( 80 );

int p_a_fx =13;   //A机方向  GPIO13 ->D12
int p_a_dl =0;   //A机动力   GPIO0->D3
int p_b_fx =16;   //B机方向  GPIO16->D13
int p_b_dl =12;   //B机动力  GPIO12->D11

void ch_page() {
  if(SPIFFS.exists("/pe15a.html")){  //如果文件存在
    File file = SPIFFS.open("/pe15a.html", "r");              //打开文件
    size_t sent = server.streamFile(file, "text/html");  //发送内容
    file.close();          //关闭文件
    return;
  }
  else
  {
    handleNotFound(); //404
  }
}

void pwm2_page() {

  String p= "";
  String sv;
  int    iv;
  int    ix;

//server.args()  这个应该是返回参数的个数
  
  if (server.hasArg("a"))      //是否有a参数
  {//设置A机运转状态
  sv=server.arg("a");  
  p="a="+sv;    //准备A相关的返回值
  iv=sv.toInt();
  if (iv>=0){
    digitalWrite(p_a_fx, 1);     //A设置方向为正向
  }  
  else{
    digitalWrite(p_a_fx, 0);     //A设置方向为逆向
    iv=iv*-1;
  }
  if (iv<=300)  //A机动力
    {analogWrite(p_a_dl,0); }
  else
    {analogWrite(p_a_dl,iv); } 
  }
  if (server.hasArg("b"))   //是否有b参数
  {
  sv=server.arg("b");   
  p=p+" b="+sv;    //准备B相关的返回值
  iv=sv.toInt();
  if (iv>=0){
    digitalWrite(p_b_fx, 0);     //B设置方向为逆向 A和变方向是相反的
  }  
  else{
    digitalWrite(p_b_fx, 1);     //B设置方向为正向 A和变方向是相反的
    iv=iv*-1;
  }  
  if (iv<=300)  //B机动力
    {analogWrite(p_b_dl,0); }
  else
    {analogWrite(p_b_dl,iv); } 
  }
  server.send(200, "text/plain", p);  //抛出返回信息
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
 
  server.send ( 404, "text/plain", message );  // 返回信息给浏览器（状态码，Content-type， 内容）
}

void setup() {
  SPIFFS.begin();

//启用电机A的2个管脚，全部设置为输出状态  
  pinMode(p_a_fx,OUTPUT); //A机方向
  digitalWrite(p_a_fx, 0);
  pinMode(p_a_dl,OUTPUT); //A机动力
  digitalWrite(p_a_dl, 0);
//启用电机B的2个管脚，全部设置为输出状态  
  pinMode(p_b_fx,OUTPUT); //B机方向
  digitalWrite(p_b_fx, 0);
  pinMode(p_b_dl,OUTPUT); //B机动力
  digitalWrite(p_b_dl, 0);
  
  analogWriteRange(1023);    //PWM取值范围
  
  // 设置内网
  IPAddress softLocal(192,168,128,1);
  IPAddress softGateway(192,168,128,1);
  IPAddress softSubnet(255,255,255,0);
  WiFi.softAPConfig(softLocal, softGateway, softSubnet);
  String apName = ("ESP8266_"+(String)ESP.getChipId());
  const char *softAPName = apName.c_str();
  WiFi.softAP(softAPName, "ap20171008");
  IPAddress myIP = WiFi.softAPIP();


  server.on ( "/inline", []() {server.send ( 200, "text/plain", "this works as well" );} );
  server.on ( "/pe15a.html", ch_page );
  server.on ( "/pwm15a.q", pwm2_page );
  server.onNotFound ( handleNotFound );
  server.begin();
}

void loop ( void ) {
  server.handleClient();
}
